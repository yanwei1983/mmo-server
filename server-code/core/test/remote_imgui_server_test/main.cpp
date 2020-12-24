#include "get_opt.h"
#include "RemoteIMGuiServer.h"
#include "imgui/imgui.h"
#include "NetworkService.h"
#include "NetworkMessage.h"
#include "RandomGet.h"
int main(int argc, char* argv[])
{
    get_opt opt(argc, (const char**)argv);
    
    BaseCode::InitLog("./log");

    auto addr = opt["--addr"];
    auto port =opt["--port"];
    int nPort = atoi(port.c_str());
    auto pServer = CRemoteIMGuiServer::CreateNew(addr, nPort);
    CHECKF(pServer);
    printf("server start!\r\n");
    //test
    bool bDemoWindow = false;
    int32_t nXorZipMode = 0;
    pServer->RegisterUIRender([&bDemoWindow, &nXorZipMode, pServer]()
    {
        auto& io = ImGui::GetIO();
        ImGui::SetNextWindowPos({20, 20});
        ImGui::SetNextWindowSize({400, 200});
        ImGui::Begin("Hello, world!");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("MousePos X:%.1f Y:%.1f", io.MousePos.x, io.MousePos.y);
        ImGui::Text("Display X:%.1f Y:%.1f", io.DisplaySize.x, io.DisplaySize.y);
        ImGui::Text("MouseDown [%d] [%d] [%d] [%d] [%d]", io.MouseDown[0], io.MouseDown[1],io.MouseDown[2],io.MouseDown[3],io.MouseDown[4]);
        ImGui::Text("SendBPS %.4f MB/s", pServer->_getNetworkService()->GetSendBPS().GetAvgBPS()/1024.0f);
        ImGui::Text("RecvBPS %.4f MB/s", pServer->_getNetworkService()->GetRecvBPS().GetAvgBPS()/1024.0f);
        
        ImGui::Checkbox("DemoWindows", &bDemoWindow);

        ImGui::Text("XorMode:"); ImGui::SameLine();
        ImGui::RadioButton("all data", &nXorZipMode, 0); ImGui::SameLine();
        ImGui::RadioButton("xor bin", &nXorZipMode, 1); 

        
        
        ImGui::End();

        pServer->SetXorZipMode(nXorZipMode);
    });

    pServer->RegisterUIRender([]()
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
    pServer->RegisterUIRender([&bDemoWindow]()
    {
        if(bDemoWindow)
        {
            ImGui::ShowDemoWindow(&bDemoWindow);
        }
    });



    while(true)
    {
        pServer->OnTimer();
    }
}