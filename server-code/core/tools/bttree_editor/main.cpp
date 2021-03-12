// Dear ImGui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.
#include <memory>
#include <vector>
#include "IMGuiFileDialog.h"
#include "StringAlgo.h"

#include "fmt/format.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"
#include <stdio.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>

#include "imnode/imnodes.h"
#include "btree_editor.h"
#include "btnode.h"

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


std::vector<std::unique_ptr<BTTreeEditor> > tree_editor_list;

static bool show_app_style_editor = false;
static bool show_opendialog = false;
static int32_t editor_idx = 1;
static void ShowMenuFile()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            ImGui::MenuItem("(menu)", NULL, false, false);
            if (ImGui::MenuItem("New", "Ctrl+N")) 
            {
                int32_t editor_id = editor_idx++;
                auto tree_editor = std::make_unique<BTTreeEditor>(editor_id, attempt_format("Untitled-{}", editor_id));
                tree_editor->OnInit();
                tree_editor_list.emplace_back(std::move(tree_editor));
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) 
            {
               igfd::ImGuiFileDialog::Instance()->OpenModal("OpenDialog", "Open File", ".json", "res/bttree/");
            }
            ImGui::MenuItem("StyleSetting", NULL, &show_app_style_editor);
            if (ImGui::MenuItem("Quit", "Alt+F4")) 
            {
                std::exit(0);
            }

            

            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}


// Demonstrate creating a simple static window with no decoration
// + a context-menu to choose which corner of the screen to use.
static bool s_open_tip_overlay = true;
static void ShowTipOverlay(bool* p_open)
{
    const float DISTANCE = 10.0f;
    static int corner = 3;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | 
                                    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
                                    ImGuiWindowFlags_NoNav;
    if (corner != -1)
    {
        window_flags |= ImGuiWindowFlags_NoMove;
        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    //ImGui::SetNextWindowFocus();
    if (ImGui::Begin("Tip overlay", p_open, window_flags))
    {
        ImFont* pChineseFont = ImGui::GetIO().Fonts->Fonts[1];
        ImGui::PushFont(pChineseFont);
        ImGui::Text("右键空白处创建节点");
        ImGui::Text("选中节点或链接，右键删除或调整子节点");
        ImGui::Text("右键子窗口标题栏，可以保存");
        ImGui::PopFont();
        ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
    }
    ImGui::End();
}


int main(int, char**)
{
    //setlocale(LC_ALL, "zh-CN.UTF-8");
    setlocale(LC_ALL, "en-US.UTF-8");
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL2 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    imnodes::Initialize();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    imnodes::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    imnodes::GetIO().link_detach_with_modifier_click.modifier = &ImGui::GetIO().KeyCtrl;
    imnodes::PushAttributeFlag(imnodes::AttributeFlags_EnableLinkDetachWithDragClick);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyClean.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    ImFont* chinese_font = io.Fonts->AddFontFromFileTTF("res/fonts/chinese.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    //IM_ASSERT(font != NULL);
    io.Fonts->Build();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);



    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    // Main loop
    while (!glfwWindowShouldClose(window))
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

        ShowMenuFile();
        
        for(const auto& v : tree_editor_list)
        {
            v->OnShow();
        }
        ShowTipOverlay(&s_open_tip_overlay);


        if (show_app_style_editor)
        {
            ImGui::Begin("Dear ImGui Style Editor", &show_app_style_editor);
            ImGui::ShowStyleEditor();
            ImGui::End();
        }

        ImFont* pChineseFont = ImGui::GetIO().Fonts->Fonts[1];
        ImGui::PushFont(pChineseFont);
        if (igfd::ImGuiFileDialog::Instance()->FileDialog("OpenDialog")) 
        {
            // action if OK
            if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
            {
                std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
                auto base_name = igfd::ImGuiFileDialog::Instance()->GetCurrentFileName();
                // action
                
                if(base_name.empty() == false)
                {
                    int32_t editor_id = tree_editor_list.size()+1;
                    auto tree_editor = std::make_unique<BTTreeEditor>(editor_id, base_name);
                    tree_editor->OnInit();
                    tree_editor->LoadFrom(filePathName);
                    tree_editor_list.emplace_back(std::move(tree_editor));
                }
            }
            // close
            igfd::ImGuiFileDialog::Instance()->CloseDialog("OpenDialog");
        }
        ImGui::PopFont();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
        // you may need to backup/reset/restore current shader using the commented lines below.
        //GLint last_program;
        //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        //glUseProgram(0);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        //glUseProgram(last_program);

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);



        for(auto it = tree_editor_list.begin(); it!= tree_editor_list.end();)
        {
            if((*it)->WantClose())
            {
                (*it)->OnShutDown();
                it = tree_editor_list.erase(it);
            }
            else
                it++;
        }
    }

    for(const auto& v : tree_editor_list)
    {
        v->OnShutDown();
    }
    tree_editor_list.clear();
    
    // Cleanup
    imnodes::Shutdown();

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
