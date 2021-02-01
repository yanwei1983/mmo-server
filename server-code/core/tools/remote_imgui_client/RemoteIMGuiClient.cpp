#include "RemoteIMGuiClient.h"

#include "IMDataToProto.h"
#include "IMGuiFrameData.h"
#include "IMGuiFrameDataWarp.h"
#include "IMGuiIOData.h"
#include "MsgProcessRegister.h"
#include "NetClientSocket.h"
#include "NetEventHandler.h"
#include "NetworkService.h"
#include "imgui.pb.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

class CRemoteIMGuiNetEventHandler : public CNetEventHandler
{
public:
    CRemoteIMGuiNetEventHandler(CRemoteIMGuiClient* pServer)
        : m_pServer(pServer)
    {
    }
    virtual size_t GetRecvPacketSizeMax() const override { return _MAX_MSGSIZE * 1024; } // 4M_1Packet
    virtual void   OnConnected(const CNetSocketSharedPtr& socket) override { m_pServer->OnConnected(socket); }
    virtual void   OnDisconnected(const CNetSocketSharedPtr& socket) override { m_pServer->OnDisconnected(socket); }
    virtual void OnRecvData(const CNetSocketSharedPtr& pSocket, CNetworkMessage&& recv_msg) override { m_pServer->OnRecvData(pSocket, std::move(recv_msg)); }

private:
    CRemoteIMGuiClient* m_pServer;
};

CRemoteIMGuiClient::CRemoteIMGuiClient()
    : m_pEventHandler(std::make_shared<CRemoteIMGuiNetEventHandler>(this))
{
}

CRemoteIMGuiClient::~CRemoteIMGuiClient()
{
    Destroy();
}

void CRemoteIMGuiClient::Destroy()
{
    m_pServer.reset();
    if(m_pNetworkService)
    {
        m_pNetworkService->Destroy();
        m_pNetworkService.reset();
    }
    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_pWindow);
    glfwTerminate();
}

constexpr int window_width  = 640;
constexpr int window_height = 480;
bool          CRemoteIMGuiClient::Init(const std::string& addr, int32_t port)
{
    if(port == 0)
        return false;

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit())
        return false;
    m_pWindow = glfwCreateWindow(window_width, window_height, "Dear ImGui GLFW+OpenGL2 example", NULL, NULL);
    if(m_pWindow == NULL)
        return false;
    glfwMakeContextCurrent(m_pWindow);
    glfwSwapInterval(1); // Enable vsync

    // init dear imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = ImVec2(window_width, window_height);
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
    ImGui_ImplOpenGL2_Init();

    InitFont();

    m_IMGuiFrameGutter = std::make_unique<IMGuiFrameGutter>();
    //m_IMGuiFrameGutter->loadFromFile("e:/frame_dump.bin");
    m_pNetworkService.reset(CreateNew<CNetworkService>());
    CHECKF(m_pNetworkService.get());
    m_pNetworkService->AsyncConnectTo(addr, port, m_pEventHandler, true);

    return true;
}

void CRemoteIMGuiClient::InitFont()
{
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.Fonts->Build();

    unsigned char* pixels;
    int            width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
    //glBindTexture(GL_TEXTURE_2D, 1);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, (void*)pixels);

    io.Fonts->SetTexID((void*)1);
}

void CRemoteIMGuiClient::ProcessIO()
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();
    ImGuiIO& io = ImGui::GetIO();

    // send to server
    if(m_pServer)
    {
        auto io_data = IMGuiIOGutter::make_data(io);
        
        CNetworkMessage msg(imgui::CS_IO, io_data);
        m_pServer->SendNetworkMessage(std::move(msg));
    }
}

bool CRemoteIMGuiClient::NewFrame()
{
    if(m_IMGuiFrameGutter->GetFrames() == 0)
        return false;
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    return true;
}

void CRemoteIMGuiClient::EndFrame()
{
    // ImGui::Render();
    ImGui::EndFrame();
    // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
    // you may need to backup/reset/restore current shader using the commented lines below.
    // GLint last_program;
    // glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    // glUseProgram(0);
    FrameDataWarp drawDataWarp;
    if(m_IMGuiFrameGutter->pop(drawDataWarp, ImGui::GetDrawListSharedData()) == true)
    { 
        auto count = m_IMGuiFrameGutter->GetFrames();
        if(m_bFrameStop && count < m_nFrameLowWaterMask)
        {
            m_bFrameStop = false;
            imgui::FrameStart framestart;
            CNetworkMessage msg(imgui::CS_FRAME_START, framestart);
            m_pServer->SendNetworkMessage(std::move(msg));
            
        }
        // Rendering
        glViewport(static_cast<GLint>(drawDataWarp.drawData.DisplayPos.x),
                   static_cast<GLint>(drawDataWarp.drawData.DisplayPos.y),
                   static_cast<GLsizei>(drawDataWarp.drawData.DisplaySize.x),
                   static_cast<GLsizei>(drawDataWarp.drawData.DisplaySize.y));
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(&drawDataWarp.drawData);
    }

    // glUseProgram(last_program);

    glfwMakeContextCurrent(m_pWindow);
    glfwSwapBuffers(m_pWindow);
}

bool CRemoteIMGuiClient::OnTimer()
{
    m_pNetworkService->RunOnce();

    if(glfwWindowShouldClose(m_pWindow))
        return false; 
    bool render_self = false;
    if(render_self)
    {
       // ImGuiIO& io = ImGui::GetIO();
       // glfwPollEvents();
       // ImGui_ImplOpenGL2_NewFrame();
       // ImGui_ImplGlfw_NewFrame();
       // ImGui::NewFrame();

       // {
       //     ImGui::SetNextWindowPos({20, 20});
       //     ImGui::SetNextWindowSize({400, 100});
       //     ImGui::Begin("Hello, world!");
       //     ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
       //     ImGui::End();
       //     ImGui::SetNextWindowPos({120, 140});
       //     ImGui::SetNextWindowSize({400, 200});
       //     ImGui::Begin("Some measured data");
       //     {
       //         static int   idx      = 0;
       //         static int   idx_last = 0;
       //         static float data[128];
       //         data[idx] = (0.8f * data[idx_last] + 0.2f * (rand() % 100 + 50));
       //         idx_last  = idx;
       //         if(++idx >= 128)
       //             idx = 0;
       //         ImGui::PlotHistogram("##signal", data, 128, idx, "Some signal", 0, FLT_MAX, ImGui::GetContentRegionAvail());
       //     }
       //     ImGui::End();
       //     ImGui::SetNextWindowPos({20, 360});
       //     ImGui::ShowAboutWindow();
       // }

       // // ImGui::EndFrame();
       // ImGui::Render();

       // glViewport(static_cast<GLint>(0), static_cast<GLint>(0), static_cast<GLsizei>(io.DisplaySize.x), static_cast<GLsizei>(io.DisplaySize.y));

       // ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
       // glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
       // glClear(GL_COLOR_BUFFER_BIT);
       // auto pDrawData = ImGui::GetDrawData();

       // imgui::FrameData frame;
       // imdata2proto::serialize(pDrawData, frame);
       ///* m_IMGuiFrameGutter->push(std::move(frame));
       // if(m_IMGuiFrameGutter->GetFrames() == 3600)
       // {
       //     m_IMGuiFrameGutter->dump("e:/frame_dump.bin");
       // }*/
       // FrameDataWarp drawDataWarp;
       // imdata2proto::deserialize(drawDataWarp, frame, ImGui::GetDrawListSharedData());
       // ImGui_ImplOpenGL2_RenderDrawData(&drawDataWarp.drawData);
       // //ImGui_ImplOpenGL2_RenderDrawData(pDrawData);
       // glfwMakeContextCurrent(m_pWindow);
       // glfwSwapBuffers(m_pWindow);
    }
    else
    {
        if(m_pServer)
        {
            if(NewFrame() == true)
            {
                EndFrame();
                ProcessIO();
            }
        }
    }
    
    

    return true;
}

void CRemoteIMGuiClient::OnConnected(const CNetSocketSharedPtr& ptr)
{
    m_pServer = ptr;
    ProcessIO();
}

void CRemoteIMGuiClient::OnDisconnected(const CNetSocketSharedPtr&)
{
    m_pServer.reset();
}

void CRemoteIMGuiClient::OnRecvData(const CNetSocketSharedPtr& pSocket, CNetworkMessage&& recv_msg)
{
    MSG_HEAD* pHead = recv_msg.GetMsgHead();
    switch(pHead->msg_cmd)
    {
        case imgui::SC_FRAME:
        {
            imgui::FrameData msg;
            if(msg.ParseFromArray(recv_msg.GetMsgBody(), recv_msg.GetBodySize()) == false)
            {
                LOGERROR("ParseFromArray Fail:{}", imgui::SC_FRAME);
                return;
            }
            m_IMGuiFrameGutter->push(std::move(msg));
            auto count = m_IMGuiFrameGutter->GetFrames();
            if(count > m_nFrameHighWaterMask && m_bFrameStop == false)
            {
                m_bFrameStop = true;
                imgui::FrameStop framestop;
                CNetworkMessage msg(imgui::CS_FRAME_STOP, framestop);
                m_pServer->SendNetworkMessage(std::move(msg));
                
            }
            else if(m_bFrameStop && count < m_nFrameLowWaterMask)
            {
                m_bFrameStop = false;
                imgui::FrameStart framestart;
                CNetworkMessage msg(imgui::CS_FRAME_START, framestart);
                m_pServer->SendNetworkMessage(std::move(msg));
                
            }
        }
        break;
        case imgui::SC_FRAME_DIFF:
        {
            std::string_view data((char*)recv_msg.GetMsgBody(), recv_msg.GetBodySize());
            imgui::FrameData frame = m_IMGuiFrameGutter->diff_last_string(data);
            m_IMGuiFrameGutter->push(std::move(frame));
        }
        break;
        case imgui::SC_TEXTURE:
        {
            imgui::TextureData texture_data;
            if(texture_data.ParseFromArray(recv_msg.GetMsgBody(), recv_msg.GetBodySize()) == false)
            {
                LOGERROR("ParseFromArray Fail:{}", imgui::SC_FRAME);
                return;
            }

            glBindTexture(GL_TEXTURE_2D, texture_data.tex_id());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            int32_t gl_tex_format = GL_RGBA;
            switch(texture_data.format())
            {
                case imgui::Alpha:
                    gl_tex_format = GL_ALPHA;
                    break;
                case imgui::RGB:
                    gl_tex_format = GL_RGB;
                    break;
                case imgui::RGBA:
                    gl_tex_format = GL_RGBA;
                    break;
                default:
                    break;
            }
            int32_t gl_tex_type = GL_UNSIGNED_BYTE;
            switch(texture_data.type())
            {
                case imgui::UNSIGNED_BYTE:
                    gl_tex_type = GL_UNSIGNED_BYTE;
                    break;
                case imgui::UNSIGNED_SHORT:
                    gl_tex_type = GL_UNSIGNED_SHORT;
                    break;
                case imgui::UNSIGNED_INT:
                    gl_tex_type = GL_UNSIGNED_INT;
                    break;
                case imgui::FLOAT:
                    gl_tex_type = GL_FLOAT;
                    break;
                default:
                    break;
            }
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         gl_tex_format,
                         texture_data.width(),
                         texture_data.height(),
                         0,
                         gl_tex_format,
                         gl_tex_type,
                         (void*)texture_data.pixels().data());
        }
        break;
        default:
        {
        }
    }
}
