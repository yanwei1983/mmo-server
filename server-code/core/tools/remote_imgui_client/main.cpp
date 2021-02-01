// Dear ImGui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

#include <stdio.h>

#include "RemoteIMGuiClient.h"
#include "get_opt.h"


int main(int argc, char* argv[])
{
    get_opt opt(argc, (const char**)argv);
    BaseCode::InitLog("./log");
    std::unique_ptr<CRemoteIMGuiClient> pClient(CreateNew<CRemoteIMGuiClient>(opt["--addr"], atoi(opt["--port"].c_str()) ));
    
    while(pClient->OnTimer() == true)
    {

    }


   

    return 0;
}
