#ifndef REMOTE_IMGUI_SERVER_H
#define REMOTE_IMGUI_SERVER_H

#include "BaseCode.h"

class CNetSocket;
using CNetSocketSharedPtr = std::shared_ptr<CNetSocket>;
class CRemoteIMGuiNetEventHandler;
class CNetworkService;
class CNetworkMessage;

class IMGuiIOGutter;
class IMGuiFrameGutter;
class IMGuiTextureMgr;

class CRemoteIMGuiServer
{
    CRemoteIMGuiServer();
    bool Init(const std::string& addr, int32_t port);
public:
    ~CRemoteIMGuiServer();
    CreateNewImpl(CRemoteIMGuiServer);
    void Destroy();

public:
    void OnTimer();
    
    void RegisterUIRender(const std::function<void()>& func);
public:
    void OnAccepted(const CNetSocketSharedPtr&);
    void OnDisconnected(const CNetSocketSharedPtr&);
    void OnRecvData(const CNetSocketSharedPtr& pSocket, CNetworkMessage&& recv_msg);
    CNetworkService* _getNetworkService(){return m_pNetworkService.get();}
    void SetXorZipMode(bool nXorZipMode) {m_nXorZipMode = nXorZipMode;}
    uint32_t GetXorZipMode() const {return m_nXorZipMode;}
private:
    void InitFont();

    bool NewFrame();
    bool ProcessIO();
    void ProcessUI();
    void EndFrame();

private:
    std::unique_ptr<CNetworkService> m_pNetworkService;
    std::vector< std::function<void()>> m_vecUI;
    std::shared_ptr<CRemoteIMGuiNetEventHandler> m_pEventHandler;
    CNetSocketSharedPtr m_pClient;
    
    std::unique_ptr<IMGuiIOGutter> m_IMGuiIOGutter;
    std::unique_ptr<IMGuiFrameGutter> m_IMGuiFrameGutter;
    std::unique_ptr<IMGuiTextureMgr> m_IMGuiTextureMgr;
    bool m_bFrameStop = false;
    uint32_t m_nXorZipMode = 0;
};
#endif /* REMOTE_IMGUI_SERVER_H */
