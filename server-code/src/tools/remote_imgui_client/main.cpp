// Dear ImGui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

#include <stdio.h>

#include "framefile.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
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

int main(int, char**)
{
    setlocale(LC_ALL, "en_US.UTF-8");


    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit())
        return 1;
    GLFWwindow* window = glfwCreateWindow(640,480, "Dear ImGui GLFW+OpenGL2 example", NULL, NULL);
    if(window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.DisplaySize = ImVec2(640,480);
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();


    // Our state
    constexpr bool record = false;
    if(record)
    {
        io.Fonts->AddFontDefault();
        io.Fonts->Build();
        ImGui_ImplOpenGL2_CreateFontsTexture();
        FrameFile file;
        while(!glfwWindowShouldClose(window))
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL2_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos({20, 20});
            ImGui::SetNextWindowSize({400, 100});
            ImGui::Begin("Hello, world!");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

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

            ImGui::SetNextWindowPos({20, 360});
            ImGui::ShowAboutWindow();

            ImGui::Render();
            // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
            // you may need to backup/reset/restore current shader using the commented lines below.
            // GLint last_program;
            // glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
            // glUseProgram(0);
           
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
            

            

            // glUseProgram(last_program);

            glfwMakeContextCurrent(window);
            glfwSwapBuffers(window);

            file.addFrame(ImGui::GetDrawData());
        }
        file.save("e:/frame.file");
    }
    else
    {
        TextureFile texture_file;
        texture_file.load("e:/texture.file");
        TextureFile::texture_data_t texture_data;
        while(texture_file.pop_texture(texture_data) == true)
        {
            glBindTexture(GL_TEXTURE_2D, texture_data.tex_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texture_data.width, texture_data.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, texture_data.pixels.get());
        }
        io.Fonts->AddFontDefault();
        io.Fonts->Build();
        io.Fonts->TexID = (void *)1;
        FrameFile file;
        file.load("e:/frame.file");

        // Main loop
        while(!glfwWindowShouldClose(window))
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL2_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            //send mouse_pos_and_button to server
            //io.MousePos
            //for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
            //{
            //    io.MouseDown[i]
            //}

            ImGui::Render();
            // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
            // you may need to backup/reset/restore current shader using the commented lines below.
            // GLint last_program;
            // glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
            // glUseProgram(0);
            ImDrawData              drawData;
            std::vector<ImDrawList> drawLists;
            if(file.getFrame(&drawData, drawLists, ImGui::GetDrawListSharedData()) == true)
            {
                // Rendering
                glViewport(static_cast<GLint>(drawData.DisplayPos.x),
                        static_cast<GLint>(drawData.DisplayPos.y),
                        static_cast<GLsizei>(drawData.DisplaySize.x),
                        static_cast<GLsizei>(drawData.DisplaySize.y));
                ImVec4    clear_color         = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
                glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL2_RenderDrawData(&drawData);
            }

            // glUseProgram(last_program);

            glfwMakeContextCurrent(window);
            glfwSwapBuffers(window);
        }
    }
    
    

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
