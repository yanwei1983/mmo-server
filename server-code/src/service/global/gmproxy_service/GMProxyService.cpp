#include "GMProxyService.h"

#include <functional>

#include <brpc/channel.h>
#include <brpc/server.h>
#include <sys/queue.h>

#include "EventManager.h"
#include "LoggingMgr.h"
#include "MD5.h"
#include "MessagePort.h"
#include "MessageRoute.h"
#include "MonitorMgr.h"
#include "MsgProcessRegister.h"
#include "NetMSGProcess.h"
#include "NetSocket.h"
#include "NetworkMessage.h"
#include "RPCService.h"

#include "event2/buffer.h"
#include "event2/http.h"
#include "event2/http_struct.h"
#include "event2/keyvalq_struct.h"
#include "proxy_service.pb.h"
#include "server_msg/server_side.pb.h"
#include "tinyxml2/tinyxml2.h"

// handle HTTP response of accessing builtin services of the target server.
static void handle_response(brpc::Controller* client_cntl, brpc::Controller* server_cntl, google::protobuf::Closure* server_done)
{
    // Copy all headers. The "Content-Length" will be overwriteen.
    server_cntl->http_response() = client_cntl->http_response();
    // Copy content.
    server_cntl->response_attachment() = client_cntl->response_attachment();
    // Notice that we don't set RPC to failed on http errors because we
    // want to pass unchanged content to the users otherwise RPC replaces
    // the content with ErrorText.
    if(client_cntl->Failed() && client_cntl->ErrorCode() != brpc::EHTTP)
    {
        server_cntl->SetFailed(client_cntl->ErrorCode(), "%s", client_cntl->ErrorText().c_str());
    }
    delete client_cntl;
    server_done->Run();
}

// A http_master_service.
class ProxyServiceImpl : public ProxyService
{
    CGMProxyService* m_pService;
    int32_t          m_internal_port;

public:
    ProxyServiceImpl(CGMProxyService* pService, int32_t internal_port)
        : m_pService(pService)
        , m_internal_port(internal_port)
    {
    }
    virtual ~ProxyServiceImpl(){};

    bool TransToTarget(const char* server_addr, int32_t port, brpc::Controller* server_cntl, google::protobuf::Closure* done)
    {
        if(server_addr == nullptr)
            return false;
        // Create the http channel on-the-fly. Notice that we've set
        // `defer_close_second' in main() so that dtor of channels do not
        // close connections immediately and ad-hoc creation of channels
        // often reuses the not-yet-closed connections.
        brpc::Channel        http_chan;
        brpc::ChannelOptions http_chan_opt;
        http_chan_opt.protocol = brpc::PROTOCOL_HTTP;
        if(http_chan.Init(server_addr, port, &http_chan_opt) != 0)
        {
            server_cntl->SetFailed(brpc::EINTERNAL, "Fail to connect to %s:%d", server_addr, port);
            return false;
        }

        // Remove "Accept-Encoding". We need to insert additional texts
        // before </body>, preventing the server from compressing the content
        // simplifies our code. The additional bandwidth consumption shouldn't
        // be an issue for infrequent checking out of builtin services pages.
        server_cntl->http_request().RemoveHeader("Accept-Encoding");

        brpc::Controller* client_cntl = new brpc::Controller;
        client_cntl->http_request()   = server_cntl->http_request();
        // Remove "Host" so that RPC will laterly serialize the (correct)
        // target server in.
        client_cntl->http_request().RemoveHeader("host");

        // Setup the URI.
        const brpc::URI& server_uri = server_cntl->http_request().uri();
        std::string      uri        = server_uri.path();
        if(!server_uri.query().empty())
        {
            uri.push_back('?');
            uri.append(server_uri.query());
        }
        if(!server_uri.fragment().empty())
        {
            uri.push_back('#');
            uri.append(server_uri.fragment());
        }
        client_cntl->http_request().uri() = uri;
        int64_t timeout_ms                = 5000;
        client_cntl->set_timeout_ms(timeout_ms);

        // Keep content as it is.
        client_cntl->request_attachment() = server_cntl->request_attachment();

        http_chan.CallMethod(NULL, client_cntl, NULL, NULL, brpc::NewCallback(&handle_response, client_cntl, server_cntl, done));

        UNUSED(m_pService);
        UNUSED(m_internal_port);
        return true;
    }

    virtual void default_method(google::protobuf::RpcController* cntl_base, const HttpRequest*, HttpResponse*, google::protobuf::Closure* done)
    {
        brpc::ClosureGuard done_guard(done);
        brpc::Controller*  server_cntl = static_cast<brpc::Controller*>(cntl_base);

        // Get or set target. Notice that we don't access FLAGS_target directly
        // which is thread-unsafe (for string flags).
        int32_t            server_id     = 0;
        const std::string* server_id_str = server_cntl->http_request().uri().GetQuery("server");
        if(server_id_str)
        {
            server_id = atoi(server_id_str->c_str());
        }

        if(server_id != 0)
        {
            auto pInfo = GetMessageRoute()->QueryServiceInfo(ServerPort(server_id, GM_SERVICE, 0));
            if(pInfo == nullptr)
            {
                server_cntl->SetFailed(brpc::EINTERNAL, "Fail to connect to server:%d", server_id);
                return;
            }

            if(TransToTarget(pInfo->publish_addr.c_str(), pInfo->publish_port, server_cntl, done) == true)
            {
                done_guard.release();
            }
        }
        else
        {
            // broadcast
            server_cntl->SetFailed(brpc::EINTERNAL, "Fail to connect to server:%d", server_id);
            return;
        }
    }
};

static thread_local CGMProxyService* tls_pService;
CGMProxyService*                     GMProxyService()
{
    return tls_pService;
}
void SetGMProxyServicePtr(CGMProxyService* ptr)
{
    tls_pService = ptr;
}

extern "C" __attribute__((visibility("default"))) IService* ServiceCreate(WorldID_t idWorld, ServiceType_t idServiceType, ServiceIdx_t idServiceIdx)
{
    return CGMProxyService::CreateNew(ServerPort{idWorld, idServiceType, idServiceIdx});
}

//////////////////////////////////////////////////////////////////////////
CGMProxyService::CGMProxyService() {}

CGMProxyService::~CGMProxyService() {}

void CGMProxyService::Release()
{

    Destory();
    delete this;
}

void CGMProxyService::Destory()
{
    __ENTER_FUNCTION
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };
    if(m_pRPCService)
    {
        m_pRPCService->StopRPCServer();
        m_pRPCService->ClearRPCServices();
        m_pRPCService.reset();
    }
    DestoryServiceCommon();

    __LEAVE_FUNCTION
}

bool CGMProxyService::Init(const ServerPort& nServerPort)
{
    //各种初始化
    tls_pService = this;
    scope_guards scope_exit;
    scope_exit += []() {
        tls_pService = nullptr;
    };

    CServiceCommon::Init(nServerPort);
    auto oldNdc = BaseCode::SetNdc(GetServiceName());
    scope_exit += [oldNdc]() {
        BaseCode::SetNdc(oldNdc);
    };

    if(CreateService(200) == false)
        return false;

    //注册消息
    {
        auto pNetMsgProcess = GetNetMsgProcess();
        for(const auto& [k, v]: MsgProcRegCenter<CGMProxyService>::instance().m_MsgProc)
        {
            pNetMsgProcess->Register(k, std::get<0>(v), std::get<1>(v));
        }
    }

    {
        const ServerAddrInfo* pAddrInfo = GetMessageRoute()->QueryServiceInfo(GetServerPort());
        if(pAddrInfo == nullptr)
        {
            LOGFATAL("CGMProxyService QueryServerInfo {} fail", GetServerPort().GetServiceID());
            return false;
        }
        m_pRPCService.reset(CRPCService::CreateNew(GetServerPort().GetServiceID()));
        CHECKF(m_pRPCService.get());
        CHECKF(
            m_pRPCService->StartRPCServer(pAddrInfo->publish_port, pAddrInfo->debug_port, true, new ProxyServiceImpl(this, pAddrInfo->debug_port)));
    }

    return true;
}

ON_SERVERMSG(CGMProxyService, ServiceRegister)
{
    ServerPort server_port{msg.serverport()};
    GetMessageRoute()->SetWorldReady(server_port.GetWorldID(), true);
    GetMessageRoute()->ReloadServiceInfo(msg.update_time(), server_port.GetWorldID());
}

ON_SERVERMSG(CGMProxyService, ServiceReady)
{
    ServerPort server_port{msg.serverport()};
    GetMessageRoute()->SetWorldReady(server_port.GetWorldID(), msg.ready());
}

ON_SERVERMSG(CGMProxyService, ServiceHttpResponse)
{
    LOGMESSAGE("recv_httpresponse:{}", msg.uid());

    auto req = GMProxyService()->FindDelayResponse(msg.uid());
    if(req == nullptr)
    {
        LOGERROR("notfind request:{}", msg.uid());
        return;
    }

    if(msg.response_code() != HTTP_OK)
    {
        evhttp_send_error(req, msg.response_code(), msg.response_reason().c_str());
    }
    else
    {
        evbuffer* pbuf = nullptr;
        if(msg.response_txt().empty() == false)
        {
            pbuf = evbuffer_new();
            evbuffer_add(pbuf, msg.response_txt().c_str(), msg.response_txt().size());
        }
        evhttp_send_reply(req, msg.response_code(), msg.response_reason().c_str(), pbuf);
        if(pbuf)
            evbuffer_free(pbuf);
    }

    LOGMESSAGE("response_send:{} code:{} res:{} tt:{}", msg.uid(), msg.response_code(), msg.response_reason().c_str(), msg.response_txt().c_str());
}

void CGMProxyService::AddDelayResponse(uint64_t uid, struct evhttp_request* req)
{
    m_RequestMap[uid] = req;
}

struct evhttp_request* CGMProxyService::FindDelayResponse(uint64_t uid)
{
    auto it = m_RequestMap.find(uid);
    if(it != m_RequestMap.end())
    {
        struct evhttp_request* req = it->second;
        m_RequestMap.erase(it);
        return req;
    }
    return nullptr;
}

void CGMProxyService::OnLogicThreadProc()
{

    CServiceCommon::OnLogicThreadProc();
}

void CGMProxyService::OnLogicThreadCreate()
{
    tls_pService = this;
    CServiceCommon::OnLogicThreadCreate();
}

void CGMProxyService::OnLogicThreadExit()
{
    CServiceCommon::OnLogicThreadExit();
}

bool CheckKVMap(const std::unordered_map<std::string, std::string>& kvmap)
{
    auto itSign = kvmap.find("sign");
    if(itSign == kvmap.end())
        return false;

    std::string signature = itSign->second;
    std::string check_str;
    for(const auto& [k, v]: kvmap)
    {
        if(k == "sign")
            continue;
        check_str += k;
        check_str += v;
    }
    constexpr const char* PASSWD = "b1zrOz5XMPd8";
    if(signature != md5(check_str + PASSWD))
        return false;
    return true;
}

// void CGMProxyService::OnReciveHttp(struct evhttp_request *req)
//{
//	const char* uri = evhttp_request_get_uri(req);
//	auto decoded = evhttp_uri_parse(uri);
//
//	if(!decoded)
//	{
//		LOGERROR("BAD REQUESTED URI: {}", uri);
//		evhttp_send_error(req, HTTP_BADREQUEST, nullptr);
//		return;
//	}
//
//	auto pPath = evhttp_uri_get_path(decoded);
//	auto pQuery = evhttp_uri_get_query(decoded);
//	if (nullptr == pQuery)
//	{
//		LOGERROR("evhttp_uri_get_query failed: {} !! ", uri);
//		evhttp_send_error(req, HTTP_BADMETHOD, nullptr);
//		return;
//	}
//	LOGMESSAGE("recv: {} {}", pPath, pQuery);
//
//	struct evkeyvalq querys;
//	scope_guards scope_exit;
//	scope_exit += [&] { evhttp_clear_headers(&querys); };
//
//	if (0 != evhttp_parse_query_str(pQuery, &querys))
//	{
//		LOGERROR("evhttp_parse_query_str failed: {} !! ", uri);
//		evhttp_send_error(req, HTTP_BADMETHOD, nullptr);
//		return;
//	}
//
//	std::unordered_map<std::string,std::string> kvmap;
//	struct evkeyval *query;
//	TAILQ_FOREACH(query, &querys, next)
//	{
//		char* pVal = evhttp_htmlescape(query->value);
//		kvmap[query->key] = pVal;
//		free(pVal);
//	}
//
//
//	//验证KV
//	if(CheckKVMap(kvmap) == false)
//	{
//		LOGERROR("BAD KVCheck URI: {}", uri);
//		evhttp_send_error(req, HTTP_BADREQUEST, nullptr);
//		return;
//	}
//	//处理
//	std::string timeStr = kvmap["time"];
//	if(timeStr.empty() == false)
//	{
//		time_t t = atol(timeStr.c_str());
//		constexpr const time_t TIME_OUT_SECOND = 3600;
//		if(abs(TimeGetSecond() - t) > TIME_OUT_SECOND)
//		{
//			LOGERROR("BAD time URI: {}", uri);
//			evhttp_send_error(req, HTTP_BADREQUEST, nullptr);
//			return ;
//		}
//	}
//
//	std::string mothedStr = kvmap["mothed"];
//	if(mothedStr.empty())
//	{
//		LOGERROR("BAD mothed URI: {}", uri);
//		evhttp_send_error(req, HTTP_BADMETHOD, nullptr);
//		return ;
//	}
//
//	std::string sidStr = kvmap["sid"];
//	if(sidStr.empty())
//	{
//		LOGERROR("BAD sidStr URI: {}", uri);
//		evhttp_send_error(req, HTTP_BADMETHOD, nullptr);
//		return ;
//	}
//	uint16_t idWorld = atol(sidStr.c_str());
//	if(idWorld != 0)
//	{
//		if(GetMessageRoute()->IsWorldReady(idWorld) == false)
//		{
//			LOGERROR("World is Not Ready: {}", idWorld);
//			evhttp_send_error(req, HTTP_BADMETHOD, nullptr);
//			return ;
//		}
//	}
//
//	//充值/后台讯风操作等
//
//	//产生对应的消息发送给对应的服务器
//	auto uid = GMProxyService()->CreateUID();
//	MsgServiceHttpRequest msg;
//	msg.set_uid(uid);
//	msg.mutable_kvmap()->insert(kvmap.begin(), kvmap.end());
//	if(idWorld == 0)
//	{
//		//广播
//	}
//	else
//	{
//		SendProtoMsgToZonePort(ServerPort(idWorld, GM_SERVICE, 0), MsgID_ServiceHttpRequest, msg);
//	}
//	evhttp_request_own(req);
//	GMProxyService()->AddDelayResponse(uid, req);
//	LOGMESSAGE("start_process:{}", uid);
//
//}