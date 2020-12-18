#include "AccountManager.h"

#include "Account.h"
#include "GMManager.h"
#include "MD5.h"
#include "MysqlConnection.h"
#include "Thread.h"
#include "WorldService.h"
#include "game_common_def.h"
#include "msg/world_service.pb.h"
#include "server_msg/server_side.pb.h"
CAccountManager::CAccountManager() {}

CAccountManager::~CAccountManager()
{
    Destroy();
}

void CAccountManager::Destroy()
{
    LOGDEBUG("CAccountManager::Destory");
    for(auto& pair_v: m_setAccount)
    {
        SAFE_DELETE(pair_v.second);
    }
    m_setAccount.clear();
    m_setAccountBySocket.clear();
    m_setWaitAccount.clear();
}

bool CAccountManager::Init(class CWorldService* pWorld)
{
    return true;
}

CAccount* CAccountManager::QueryAccountBySocket(const VirtualSocket& vs) const
{
    auto it = m_setAccountBySocket.find(vs);
    if(it != m_setAccountBySocket.end())
    {
        return it->second;
    }
    return nullptr;
}

void CAccountManager::Login(const VirtualSocket& vs, const std::string& openid)
{
    __ENTER_FUNCTION
    //判断当前是否已经有账号登陆了？ 如果已经有账号登陆了， 先将前一个Account踢下线
    bool bKicked = false;
    auto itFind  = m_setAccount.find(openid);
    if(itFind != m_setAccount.end())
    {
        // kick out
        CAccount* pOldAccount = itFind->second;
        pOldAccount->KickOut();
        m_setAccountBySocket.erase(pOldAccount->GetSocket());
        m_setAccount.erase(itFind);
        if(pOldAccount->IsWait())
        {
            auto it_waitAccount = std::find(m_setWaitAccount.begin(), m_setWaitAccount.end(), pOldAccount);
            if(it_waitAccount != m_setWaitAccount.end())
            {
                m_setWaitAccount.erase(it_waitAccount);
            }
        }

        SAFE_DELETE(pOldAccount);
        bKicked = true;
    }

    CAccount* pAccount = CAccount::CreateNew(openid, vs);
    CHECK(pAccount);
    LOGLOGIN("Actor:{} StartLogin", openid.c_str());

    m_setAccount[pAccount->GetOpenID()]         = pAccount;
    m_setAccountBySocket[pAccount->GetSocket()] = pAccount;

    if(bKicked || GMManager()->GetGMLevel(pAccount->GetOpenID()) > 0 || m_setAccount.size() - m_setWaitAccount.size() < _START_WAITING_ACCOUNT_COUNT)
    {
        //通知前端，登陆成功
        LOGLOGIN("ActorSucc:{}.", pAccount->GetOpenID().c_str());
        pAccount->SetWait(false);
        pAccount->SendActorInfo();
    }
    else
    {
        //放入等待登陆队列
        LOGLOGIN("ActorWait:{}.", pAccount->GetOpenID().c_str());

        pAccount->SetWait(true);
        m_setWaitAccount.push_back(pAccount);
        pAccount->SendWaitInfo();
    }

    __LEAVE_FUNCTION
}

void CAccountManager::Logout(const VirtualSocket& vs)
{
    __ENTER_FUNCTION

    auto itFind = m_setAccountBySocket.find(vs);
    if(itFind == m_setAccountBySocket.end())
        return;

    CAccount* pAccount = itFind->second;
    LOGLOGIN("ActorLogout:{}.", pAccount->GetOpenID().c_str());

    if(pAccount->IsWait() == false)
    {
        pAccount->KickOut();
    }

    m_setAccount.erase(pAccount->GetOpenID());
    m_setAccountBySocket.erase(itFind);
    if(pAccount->IsWait())
    {
        auto it_waitAccount = std::find(m_setWaitAccount.begin(), m_setWaitAccount.end(), pAccount);
        if(it_waitAccount != m_setWaitAccount.end())
        {
            m_setWaitAccount.erase(it_waitAccount);
        }
    }
    SAFE_DELETE(pAccount);
    __LEAVE_FUNCTION
}

void CAccountManager::OnTimer()
{
    __ENTER_FUNCTION

    //检查等待登陆队列， 如果等待登陆队列不为空
    //每次最多放行5个
    constexpr int32_t MAX_PROCESS_PER_TIMERS = 20;
    for(int32_t i = 0; i < MAX_PROCESS_PER_TIMERS; i++)
    {
        if(m_setWaitAccount.empty())
            break;
        if(m_setAccount.size() - m_setWaitAccount.size() >= _STOP_WAITING_ACCOUNT_COUNT)
            break;

        CAccount* pWaitAccount = m_setWaitAccount.front();
        m_setWaitAccount.pop_front();
        LOGLOGIN("ActorSucc:{}.", pWaitAccount->GetOpenID().c_str());
        pWaitAccount->SetWait(false);
        //通知前端
        pWaitAccount->SendActorInfo();
    }

    __LEAVE_FUNCTION
}

size_t CAccountManager::GetAccountSize() const
{
    return m_setAccount.size();
}

size_t CAccountManager::GetWaitAccountSize() const
{
    return m_setWaitAccount.size();
}
