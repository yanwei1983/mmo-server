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
#include "NetworkMessage.h"

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
    virtual void OnRecvData(const CNetSocketSharedPtr& pSocket, CNetworkMessage&& recv_msg) override { m_pServer->OnRecvData(pSocket, std::move(recv_msg)); }

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
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();
    

    m_IMGuiIOGutter = std::make_unique<IMGuiIOGutter>();
    m_IMGuiFrameGutter = std::make_unique<IMGuiFrameGutter>();
    m_IMGuiTextureMgr = std::make_unique<IMGuiTextureMgr>();
    m_pNetworkService.reset( CNetworkService::CreateNew() );
    CHECKF(m_pNetworkService.get());
    CHECKF(m_pNetworkService->Listen(addr, port, m_pEventHandler));

    InitFont();

    

    return true;
}

void CRemoteIMGuiServer::InitFont()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    m_pChineseFont = io.Fonts->AddFontFromFileTTF("res/fonts/chinese.ttf", 20, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    //m_pChineseFont = io.Fonts->AddFontFromMemoryCompressedTTF(chinese_font_compressed_data, chinese_font_compressed_size, 20, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    io.Fonts->Build();

    unsigned char * pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    m_IMGuiTextureMgr->store_texture(1, width, height, imgui::Alpha, imgui::UNSIGNED_BYTE, pixels);
    io.Fonts->SetTexID((void*)1);
    
}

bool CRemoteIMGuiServer::ProcessIO()
{
    ImGuiIO& io = ImGui::GetIO();
    return m_IMGuiIOGutter->pop(io);
}

bool CRemoteIMGuiServer::NewFrame()
{
    if(ProcessIO() == false)
        return false;

    ImGui::NewFrame();
    return true;
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

    if(m_pClient && m_bFrameStop == false)
    {
        // store ImDrawData for asynchronous dispatching to WS clients
        auto pDrawData = ImGui::GetDrawData();
        
        imgui::FrameData frame;
        imdata2proto::serialize(pDrawData, frame);
        
        if(m_nXorZipMode == 1)
        {           
            std::string output = m_IMGuiFrameGutter->diff_last_string(std::move(frame));;
            
            CNetworkMessage msg(imgui::SC_FRAME_DIFF,(byte*) output.data(), output.size());
            m_pClient->SendNetworkMessage(std::move(msg));
        }
        else
        {
            CNetworkMessage msg(imgui::SC_FRAME, frame);
            m_pClient->SendNetworkMessage(std::move(msg));
        }
        
    }    
}

void CRemoteIMGuiServer::OnTimer()
{
    m_pNetworkService->RunOnce();

    if(m_pClient)
    {
        if(NewFrame() == true)
        {
            ProcessUI();
            EndFrame();
        }
    }
}

void CRemoteIMGuiServer::OnAccepted(const CNetSocketSharedPtr& ptr)
{
    m_pClient = ptr;

    m_IMGuiTextureMgr->foreach([this](const auto& pair_v)
    {
        CNetworkMessage msg(imgui::SC_TEXTURE, pair_v.second);
        m_pClient->SendNetworkMessage(std::move(msg));
    });
    
}

void CRemoteIMGuiServer::OnDisconnected(const CNetSocketSharedPtr&)
{
    m_pClient.reset();
    m_IMGuiIOGutter->drop_all();
}

void CRemoteIMGuiServer::OnRecvData(const CNetSocketSharedPtr& pSocket, CNetworkMessage&& recv_msg)
{
    MSG_HEAD* pHead = recv_msg.GetMsgHead();
    switch(pHead->msg_cmd)
    {
        case imgui::CS_IO:
        {
            imgui::IOData msg;
            if(msg.ParseFromArray(recv_msg.GetMsgBody(), recv_msg.GetBodySize()) == false)
            {
                LOGERROR("ParseFromArray Fail:{}", imgui::CS_IO);
                return;
            }
            m_IMGuiIOGutter->push(std::move(msg));            
        }
        break;
        case imgui::CS_FRAME_STOP:
        {
            m_bFrameStop = true;
            
        }
        break;
        case imgui::CS_FRAME_START:
        {
            m_bFrameStop = false;
            
        }
        break;
        default:
        {

        }
    }
}


