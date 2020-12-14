#include "AuthManager.h"

#include <sstream>

#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

#include "AuthService.h"
#include "GMManager.h"
#include "MD5.h"
#include "MsgProcessRegister.h"
#include "msg/world_service.pb.h"
#include "server_msg/server_side.pb.h"

const char*           AUTH_URL              = "https://example.com";
constexpr const char* AUTH_SERVER_SIGNATURE = "test";
constexpr int32_t     AUTH_KEY_CANUSE_SECS  = 180;

CAuthManager::CAuthManager()
{
    curlpp::initialize();
}

CAuthManager::~CAuthManager()
{
    Destory();
}

void CAuthManager::Destory()
{
    if(m_threadAuth)
    {
        m_threadAuth->Stop();
        m_threadAuth->Join();
        m_threadAuth.reset();
    }
}

bool CAuthManager::Init(CAuthService* pService)
{
    m_threadAuth = std::make_unique<CWorkerThread>("auth_thread");
    return true;
}

bool CAuthManager::IsAuthing(const VirtualSocket& vs) const
{
    return m_AuthDataList.find(vs) != m_AuthDataList.end();
}

bool CAuthManager::Auth(const std::string& openid, const std::string& auth, const VirtualSocket& vs)
{
    __ENTER_FUNCTION

    auto& auth_data   = m_AuthDataList[vs];
    auth_data.open_id = openid;
    auth_data.from    = vs;

    m_threadAuth->AddJob([this, openid_ = openid, auth_ = auth, vs_ = vs]() {
        try
        {
            curlpp::Easy request;

            using namespace curlpp::Options;
            request.setOpt(Url(AUTH_URL));
            std::string post_data = fmt::format(FMT_STRING("open_id={}&auth={}"), openid_, auth_);
            request.setOpt(PostFields(post_data));
            request.perform();

            auto               response_code = curlpp::infos::ResponseCode::get(request);
            constexpr uint32_t HTTP_OK       = 200;
            if(response_code != HTTP_OK)
            {
                std::stringstream buf;
                buf << request;
                m_threadAuth->_AddResult(std::bind(&CAuthManager::_OnAuthFail, this, vs_, buf.str()));
            }
            else
            {
                m_threadAuth->_AddResult(std::bind(&CAuthManager::_OnAuthSucc, this, vs_));
            }
        }
        catch(curlpp::LogicError& e)
        {

            m_threadAuth->_AddResult(std::bind(&CAuthManager::_OnAuthFail, this, vs_, e.what()));
        }
        catch(curlpp::RuntimeError& e)
        {
            m_threadAuth->_AddResult(std::bind(&CAuthManager::_OnAuthFail, this, vs_, e.what()));
        }
    });

    return true;
    __LEAVE_FUNCTION
    return false;
}

void CAuthManager::ProcessResult()
{
    m_threadAuth->ProcessResult();
}

void CAuthManager::_OnAuthFail(const VirtualSocket& vs, const std::string& str_detail)
{
    auto it = m_AuthDataList.find(vs);
    if(it == m_AuthDataList.end())
        return;
    auto& auth_data = it->second;

    //发送验证失败消息
    SC_LOGIN msg;
    msg.set_result_code(SC_LOGIN::EC_AUTH);
    msg.set_detail(str_detail);
    AuthService()->SendMsgToVirtualSocket(auth_data.from, msg);

    m_AuthDataList.erase(it);
}

void CAuthManager::_OnAuthSucc(const VirtualSocket& vs)
{
    auto it = m_AuthDataList.find(vs);
    if(it == m_AuthDataList.end())
        return;
    auto& auth_data = it->second;

    LOGLOGIN("Actor:{} AuthSucc.", auth_data.open_id.c_str());

    ServerMSG::SocketAuth auth_msg;
    auth_msg.set_vs(auth_data.from);
    auth_msg.set_open_id(auth_data.open_id);
    AuthService()->SendProtoMsgToZonePort(auth_data.from.GetServerPort(), auth_msg);

    SC_LOGIN result_msg;
    result_msg.set_result_code(SC_LOGIN::EC_SUCC);
    std::string md5str = md5(auth_data.open_id + std::to_string(TimeGetSecond() / AUTH_KEY_CANUSE_SECS) + AUTH_SERVER_SIGNATURE);
    result_msg.set_last_succ_key(md5str);
    AuthService()->SendMsgToVirtualSocket(auth_data.from, result_msg);

    m_AuthDataList.erase(it);
}

void CAuthManager::CancleAuth(const VirtualSocket& vs)
{
    auto it = m_AuthDataList.find(vs);
    if(it == m_AuthDataList.end())
        return;

    LOGLOGIN("Actor:{} AuthCancle.", it->second.open_id);

    m_AuthDataList.erase(it);
}

bool CAuthManager::CheckProgVer(const std::string& prog_ver) const
{
    return true;
}

ON_SERVERMSG(CAuthService, SocketClose)
{
    AuthManager()->CancleAuth(msg.vs());
}

ON_MSG(CAuthService, CS_LOGIN)
{
    CHECK(msg.openid().empty() == false);
    if(AuthManager()->IsAuthing(pMsg->GetFrom()))
    {
        LOGLOGIN("VS:{} Actor:{} IsAleardyInAuth.", pMsg->GetFrom(), msg.openid());

        //当前正在验证，通知客户端
        SC_LOGIN result_msg;
        result_msg.set_result_code(SC_LOGIN::EC_WAIT_AUTH);
        AuthService()->SendMsgToVirtualSocket(pMsg->GetFrom(), result_msg);
        return;
    }
    auto nGMLev = GMManager()->GetGMLevel(msg.openid());
    //校验程序版本号
    if(nGMLev == 0 && (msg.prog_ver().empty() || AuthManager()->CheckProgVer(msg.prog_ver()) == false))
    {
        //发送错误给前端
        LOGLOGIN("Actor:{} CheckProgVerFail.", msg.openid().c_str());
        SC_LOGIN result_msg;
        result_msg.set_result_code(SC_LOGIN::EC_PROG_VER);
        AuthService()->SendMsgToVirtualSocket(pMsg->GetFrom(), result_msg);
        return;
    }

    if(msg.last_succ_key().empty() == false)
    {
        //曾经验证成功过， 检查2次校验串
        std::string md5str = md5(msg.openid() + std::to_string(TimeGetSecond() / AUTH_KEY_CANUSE_SECS) + AUTH_SERVER_SIGNATURE);
        if(nGMLev == 0 && msg.last_succ_key() != md5str)
        {
            //发送错误给前端
            LOGLOGIN("Actor:{} MD5CHECKFail.", msg.openid().c_str());

            SC_LOGIN result_msg;
            result_msg.set_result_code(SC_LOGIN::EC_LAST_KEY);
            AuthService()->SendMsgToVirtualSocket(pMsg->GetFrom(), result_msg);
            return;
        }
        else
        {
            LOGLOGIN("Actor:{} LoginAuthByMD5.", msg.openid().c_str());
            //可以直接登陆了
            ServerMSG::SocketAuth auth_msg;
            auth_msg.set_vs(pMsg->GetFrom());
            auth_msg.set_open_id(msg.openid());
            AuthService()->SendProtoMsgToZonePort(pMsg->GetFrom().GetServerPort(), auth_msg);

            SC_LOGIN result_msg;
            result_msg.set_result_code(SC_LOGIN::EC_SUCC);
            result_msg.set_last_succ_key(md5str);
            AuthService()->SendMsgToVirtualSocket(pMsg->GetFrom(), result_msg);

            return;
        }
    }
    else if(msg.auth().empty())
    {
        SC_LOGIN result_msg;
        result_msg.set_result_code(SC_LOGIN::EC_AUTH);
        AuthService()->SendMsgToVirtualSocket(pMsg->GetFrom(), result_msg);
        return;
    }

    LOGLOGIN("Actor:{} StartAuth.", msg.openid().c_str());
    AuthManager()->Auth(msg.openid(), msg.auth(), pMsg->GetFrom());
}
