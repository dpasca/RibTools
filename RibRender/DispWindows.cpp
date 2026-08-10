//==================================================================
/// DispWindows.cpp
///
/// Created by Davide Pasca - 2010/1/28
/// Dear ImGui front end added in 2026.
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "DispWindows.h"

#include <algorithm>
#include <utility>

#include "RibToolsUI/ImGuiHost.h"

//==================================================================
DispWindows::~DispWindows() = default;

//==================================================================
void DispWindows::Init(int argc, char** ppArgv, const char* pWindowBaseName)
{
    static_cast<void>(argc);
    static_cast<void>(ppArgv);

    if (pWindowBaseName && pWindowBaseName[0])
    {
        mWindowBaseName = pWindowBaseName;
    }
}

//==================================================================
void DispWindows::AddWindow(const RI::Options::Display &display)
{
    ensureHost(display);

    auto title = display.mName.c_str();
    if (!title[0])
    {
        title = "Render Output";
    }

    mWindows.push_back(Window{title, std::make_unique<DispDriverFBuffOGL>(title, display.mImage), true});
}

//==================================================================
void DispWindows::MainLoop()
{
    if (!moHost)
    {
        return;
    }

    while (!moHost->ShouldClose())
    {
        if (!moHost->BeginFrame())
        {
            continue;
        }

        drawMainMenu();
        const auto dockspaceId = ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
        drawWindows(dockspaceId);
        moHost->EndFrame();
    }
}

//==================================================================
void DispWindows::ensureHost(const RI::Options::Display &display)
{
    if (moHost)
    {
        return;
    }

    const auto width = std::clamp(static_cast<int>(display.mImage.mWd) + 180, 720, 1500);
    const auto height = std::clamp(static_cast<int>(display.mImage.mHe) + 140, 520, 1100);
    moHost =
        std::make_unique<RTUI::ImGuiHost>(RTUI::ImGuiHost::Params{mWindowBaseName, width, height, "rib-render.ini"});
}

//==================================================================
void DispWindows::drawMainMenu()
{
    if (!ImGui::BeginMainMenuBar())
    {
        return;
    }

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Quit"))
        {
            moHost->RequestClose();
        }
        ImGui::EndMenu();
    }

    ImGui::TextDisabled("Drag an output tab outside the main window to detach it.");
    ImGui::EndMainMenuBar();
}

//==================================================================
void DispWindows::drawWindows(ImGuiID dockspaceId)
{
    auto openWindowCount = 0;

    for (size_t windowIndex = 0; windowIndex < mWindows.size(); ++windowIndex)
    {
        auto &window = mWindows[windowIndex];
        if (!window.mOpen)
        {
            continue;
        }

        ++openWindowCount;
        ImGui::SetNextWindowDockID(dockspaceId, ImGuiCond_FirstUseEver);

        const auto windowId = window.mTitle + "##render-output-" + std::to_string(windowIndex);
        const auto visible = ImGui::Begin(windowId.c_str(), &window.mOpen);
        if (visible)
        {
            const auto imageWidth = static_cast<float>(window.moDisplay->GetCurWd());
            const auto imageHeight = static_cast<float>(window.moDisplay->GetCurHe());
            const auto available = ImGui::GetContentRegionAvail();
            const auto scale = std::max(0.01f, std::min(available.x / imageWidth, available.y / imageHeight));
            const ImVec2 imageSize(imageWidth * scale, imageHeight * scale);

            const auto cursor = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(cursor.x + std::max(0.0f, (available.x - imageSize.x) * 0.5f),
                                       cursor.y + std::max(0.0f, (available.y - imageSize.y) * 0.5f)));

            ImGui::Image(ImTextureRef(static_cast<ImTextureID>(window.moDisplay->GetTextureId())), imageSize,
                         ImVec2(0.0f, 0.0f),
                         ImVec2(window.moDisplay->GetTextureUMax(), window.moDisplay->GetTextureVMax()));
        }
        ImGui::End();
    }

    if (openWindowCount == 0)
    {
        moHost->RequestClose();
    }
}
