//==================================================================
/// ImGuiHost.cpp
///
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "RibToolsUI/ImGuiHost.h"

#include <cmath>
#include <cstdio>
#include <stdexcept>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

namespace RTUI
{

//==================================================================
static void glfwErrorCallback(int error, const char* pDescription)
{
    std::fprintf(stderr, "GLFW error %d: %s\n", error, pDescription);
}

//==================================================================
ImGuiHost::ImGuiHost(const Params &params) : mIniFilename(params.mIniFilename)
{
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        throw std::runtime_error("Could not initialize GLFW");
    }

    const auto scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    const auto width = static_cast<int>(std::lround(params.mWidth * scale));
    const auto height = static_cast<int>(std::lround(params.mHeight * scale));

    mpWindow = glfwCreateWindow(width, height, params.mTitle.c_str(), nullptr, nullptr);
    if (!mpWindow)
    {
        glfwTerminate();
        throw std::runtime_error("Could not create the RibTools application window");
    }

    glfwMakeContextCurrent(mpWindow);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigDpiScaleFonts = true;
    io.ConfigDpiScaleViewports = true;
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.IniFilename = mIniFilename.empty() ? nullptr : mIniFilename.c_str();

    ImGui::StyleColorsDark();
    auto &style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.FontScaleDpi = scale;
    io.Fonts->AddFontDefaultVector();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    if (!ImGui_ImplGlfw_InitForOpenGL(mpWindow, true))
    {
        ImGui::DestroyContext();
        glfwDestroyWindow(mpWindow);
        mpWindow = nullptr;
        glfwTerminate();
        throw std::runtime_error("Could not initialize Dear ImGui's GLFW backend");
    }

    if (!ImGui_ImplOpenGL2_Init())
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(mpWindow);
        mpWindow = nullptr;
        glfwTerminate();
        throw std::runtime_error("Could not initialize Dear ImGui's OpenGL backend");
    }
}

//==================================================================
ImGuiHost::~ImGuiHost()
{
    if (!mpWindow)
    {
        return;
    }

    glfwMakeContextCurrent(mpWindow);
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(mpWindow);
    mpWindow = nullptr;
    glfwTerminate();
}

//==================================================================
bool ImGuiHost::ShouldClose() const
{
    return !mpWindow || glfwWindowShouldClose(mpWindow);
}

//==================================================================
bool ImGuiHost::BeginFrame()
{
    glfwPollEvents();

    if (ShouldClose())
    {
        return false;
    }

    if (glfwGetWindowAttrib(mpWindow, GLFW_ICONIFIED))
    {
        ImGui_ImplGlfw_Sleep(10);
        return false;
    }

    glfwMakeContextCurrent(mpWindow);
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    return true;
}

//==================================================================
void ImGuiHost::EndFrame(const ImVec4 &clearColor)
{
    ImGui::Render();

    glfwMakeContextCurrent(mpWindow);

    int displayWidth = 0;
    int displayHeight = 0;
    glfwGetFramebufferSize(mpWindow, &displayWidth, &displayHeight);
    glViewport(0, 0, displayWidth, displayHeight);
    glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    const auto &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        auto* pPreviousContext = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(pPreviousContext);
    }

    glfwMakeContextCurrent(mpWindow);
    glfwSwapBuffers(mpWindow);
}

//==================================================================
void ImGuiHost::RequestClose()
{
    if (mpWindow)
    {
        glfwSetWindowShouldClose(mpWindow, GLFW_TRUE);
    }
}

}
