#include "RemoteIMGuiServer.h"
#include "imgui/imgui.h"
#include "MsgProcessRegister.h"
#include "IMDataToProto.h"
#include "NetClientSocket.h"
#include "NetworkService.h"
#include "NetEventHandler.h"
#include "IMGuiFrameData.h"
#include "IMGuiIOData.h"
#include "IMGuiTextureMgr.h"

class CRemoteIMGuiNetEventHandler : public CNetEventHandler
{  
public:
    CRemoteIMGuiNetEventHandler(CRemoteIMGuiServer* pServer)
        : m_pServer(pServer)
    {
    }
    virtual size_t GetSendPacketSizeMax() const override { return _MAX_MSGSIZE * 1024; }//4M_1Packet
    virtual void OnAccepted(const CNetSocketSharedPtr& socket) override { m_pServer->OnAccepted(socket); }
    virtual void OnDisconnected(const CNetSocketSharedPtr& socket) override { m_pServer->OnDisconnected(socket); }
    virtual void OnRecvData(const CNetSocketSharedPtr& pSocket, byte* pBuffer, size_t len) override { m_pServer->OnRecvData(pSocket, pBuffer, len); }

private:
    CRemoteIMGuiServer* m_pServer;
};


CRemoteIMGuiServer::CRemoteIMGuiServer()
:m_pEventHandler(std::make_shared<CRemoteIMGuiNetEventHandler>(this))
{

}

CRemoteIMGuiServer::~CRemoteIMGuiServer()
{
    Destroy();
}

void CRemoteIMGuiServer::Destroy()
{
    if(m_pNetworkService)
    {
        m_pNetworkService->Destroy();
        m_pNetworkService.reset();
    }
    ImGui::DestroyContext();
}

bool CRemoteIMGuiServer::Init(const std::string& addr, int32_t port)
{
    if(port == 0)
        return false;
    // init dear imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = ImVec2(640,480);
    ImGui::StyleColorsDark();
    

    m_IMGuiIOGutter = std::make_unique<IMGuiIOGutter>();
    m_IMGuiFrameGutter = std::make_unique<IMGuiFrameGutter>();
    m_IMGuiTextureMgr = std::make_unique<IMGuiTextureMgr>();
    m_pNetworkService = std::make_unique<CNetworkService>();
    CHECKF(m_pNetworkService->Listen(addr, port, m_pEventHandler));

    InitFont();

    //test
    RegisterUIRender([]()
    {
        ImGui::SetNextWindowPos({20, 20});
        ImGui::SetNextWindowSize({400, 100});
        ImGui::Begin("Hello, world!");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    });

    RegisterUIRender([]()
    {
        ImGui::SetNextWindowPos({120, 140});
        ImGui::SetNextWindowSize({400, 200});
        ImGui::Begin("Some measured data");
        {
            static int   idx      = 0;
            static int   idx_last = 0;
            static float data[128];
            data[idx] = (0.8f * data[idx_last] + 0.2f * (rand() % 100 + 50));
            idx_last  = idx;
            if(++idx >= 128)
                idx = 0;
            ImGui::PlotHistogram("##signal", data, 128, idx, "Some signal", 0, FLT_MAX, ImGui::GetContentRegionAvail());
        }
        ImGui::End();
    });
    RegisterUIRender([]()
    {
        ImGui::SetNextWindowPos({20, 360});
        ImGui::ShowAboutWindow();
    });

    return true;
}

void CRemoteIMGuiServer::InitFont()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.Fonts->Build();

    unsigned char * pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    m_IMGuiTextureMgr->store_texture(1, width, height, pixels);
    io.Fonts->SetTexID((void*)1);
    
}

void CRemoteIMGuiServer::ProcessIO()
{
    ImGuiIO& io = ImGui::GetIO();
    m_IMGuiIOGutter->pop(io);
}

void CRemoteIMGuiServer::NewFrame()
{
    ImGui::NewFrame();
    ProcessIO();
}

void CRemoteIMGuiServer::RegisterUIRender(const std::function<void()>& func)
{
    m_vecUI.push_back(func);
}

void CRemoteIMGuiServer::ProcessUI()
{
    for(const auto& func : m_vecUI)
    {
        func();
    }
}

void CRemoteIMGuiServer::EndFrame()
{
    // generate ImDrawData
    ImGui::EndFrame();
    ImGui::Render();

    if(m_pClient)
    {
        // store ImDrawData for asynchronous dispatching to WS clients
        auto pDrawData = ImGui::GetDrawData();
        
        imgui::FrameData frame;
        imdata2proto::serialize(pDrawData, frame);

        CNetworkMessage msg(imgui::SC_FRAME, frame);
        m_pClient->SendNetworkMessage(msg);
    }    
}

void CRemoteIMGuiServer::OnTimer()
{
    m_pNetworkService->RunOnce();

    if(m_pClient)
    {
        NewFrame();
        ProcessUI();
        EndFrame();
    }
}

void CRemoteIMGuiServer::OnAccepted(const CNetSocketSharedPtr& ptr)
{
    m_pClient = ptr;

    m_IMGuiTextureMgr->foreach([this](const auto& pair_v)
    {
        CNetworkMessage msg(imgui::SC_TEXTURE, pair_v.second);
        m_pClient->SendNetworkMessage(msg);
    });
    
}

void CRemoteIMGuiServer::OnDisconnected(const CNetSocketSharedPtr&)
{
    m_pClient.reset();
}

void CRemoteIMGuiServer::OnRecvData(const CNetSocketSharedPtr& pSocket, byte* pBuffer, size_t len)
{
    MSG_HEAD* pHead = (MSG_HEAD*)pBuffer;
    switch(pHead->msg_cmd)
    {
        case imgui::CS_IO:
        {
            imgui::IOData msg;
            if(msg.ParseFromArray(pBuffer + sizeof(MSG_HEAD), len - sizeof(MSG_HEAD)) == false)
            {
                LOGERROR("ParseFromArray Fail:{}", imgui::CS_IO);
                return;
            }
            m_IMGuiIOGutter->push(std::move(msg));            
        }
        default:
        {

        }
    }
}


