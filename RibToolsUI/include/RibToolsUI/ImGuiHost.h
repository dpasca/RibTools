//==================================================================
/// ImGuiHost.h
///
/// Shared GLFW and Dear ImGui application shell for RibTools.
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef RIBTOOLSUI_IMGUIHOST_H
#define RIBTOOLSUI_IMGUIHOST_H

#include <string>

#include "imgui.h"

struct GLFWwindow;

namespace RTUI
{

//==================================================================
class ImGuiHost
{
public:
    struct Params
    {
        std::string mTitle{"RibTools"};
        int mWidth{1200};
        int mHeight{760};
        std::string mIniFilename{"ribtools-ui.ini"};
    };

    explicit ImGuiHost(const Params &params);
    ~ImGuiHost();

    ImGuiHost(const ImGuiHost &) = delete;
    ImGuiHost &operator=(const ImGuiHost &) = delete;

    bool ShouldClose() const;
    bool BeginFrame();
    void EndFrame(const ImVec4 &clearColor = ImVec4(0.08f, 0.09f, 0.11f, 1.0f));
    void RequestClose();

    GLFWwindow* GetWindow() const
    {
        return mpWindow;
    }

private:
    std::string mIniFilename;
    GLFWwindow* mpWindow{};
};

}

#endif
