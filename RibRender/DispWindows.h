//==================================================================
/// DispWindows.h
///
/// Dear ImGui display host for framebuffer outputs.
//==================================================================

#ifndef DISPWINDOWS_H
#define DISPWINDOWS_H

#include <memory>
#include <string>
#include <vector>

#include "DispDrivers/include/DispDriverFBuffOGL.h"
#include "RI_System/include/RI_Options.h"
#include "RibToolsUI/ImGuiHost.h"

//==================================================================
class DispWindows
{
public:
    DispWindows() = default;
    ~DispWindows();

    void Init(int argc, char** ppArgv, const char* pWindowBaseName);
    void AddWindow(const RI::Options::Display &display);

    bool HasWindows() const
    {
        return !mWindows.empty();
    }

    void MainLoop();

private:
    struct Window
    {
        std::string mTitle;
        std::unique_ptr<DispDriverFBuffOGL> moDisplay;
        bool mOpen{true};
    };

    std::string mWindowBaseName{"RibRender"};
    std::unique_ptr<RTUI::ImGuiHost> moHost;
    std::vector<Window> mWindows;

    void ensureHost(const RI::Options::Display &display);
    void drawMainMenu();
    void drawWindows(ImGuiID dockspaceId);
};

#endif
