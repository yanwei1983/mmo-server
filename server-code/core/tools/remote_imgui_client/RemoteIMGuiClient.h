#ifndef REMOTE_IMGUI_CLIENT_CPP
#define REMOTE_IMGUI_CLIENT_CPP


#include <memory>
#include "BaseCode.h"

class CNetSocket;
using CNetSocketSharedPtr = std::shared_ptr<CNetSocket>;
class CRemoteIMGuiNetEventHandler;
class CNetworkService;
class CNetworkMessage;

class IMGuiIOGutter;
class IMGuiFrameGutter;
class IMGuiTextureMgr;

class CRemoteIMGuiClient
{
    CRemoteIMGuiClient();
    bool Init(const std::string& addr, int32_t port);
public:
    ~CRemoteIMGuiClient();
    CreateNewImpl(CRemoteIMGuiClient);
    void Destroy();

public:
    bool OnTimer();
    
    void RegisterUIRender(const std::function<void()>& func);
public:
    void OnConnected(const CNetSocketSharedPtr&);   
    void OnDisconnected(const CNetSocketSharedPtr&);
    void OnRecvData(const CNetSocketSharedPtr& pSocket, CNetworkMessage&& recv_msg);
private:
    void InitFont();

    bool NewFrame();
    void ProcessIO();
    void EndFrame();

private:
    std::unique_ptr<CNetworkService> m_pNetworkService;
    std::vector< std::function<void()>> m_vecUI;
    std::shared_ptr<CRemoteIMGuiNetEventHandler> m_pEventHandler;

    CNetSocketSharedPtr m_pServer;
    struct GLFWwindow*  m_pWindow;
    
    std::unique_ptr<IMGuiFrameGutter> m_IMGuiFrameGutter;
    bool m_bFrameStop = false;
    uint32_t m_nFrameHighWaterMask = 360;
    uint32_t m_nFrameLowWaterMask = 60;
};

#endif /* REMOTE_IMGUI_CLIENT_CPP */
