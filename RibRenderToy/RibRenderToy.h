//==================================================================
/// RibRenderToy.h
///
/// Interactive Dear ImGui front end for the RibTools renderer.
//==================================================================

#ifndef RIBRENDERTOY_H
#define RIBRENDERTOY_H

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "DispDrivers/include/DispDriverFBuffOGL.h"
#include "RI_System/include/RI_HiderST.h"
#include "RibRenderLib/include/RibRenderLib.h"
#include "RibToolsUI/ImGuiHost.h"

//==================================================================
class RibRenderToyApp
{
public:
    explicit RibRenderToyApp(const std::filesystem::path &executablePath);

    int Run(int frameLimit = 0);
    void SetFileToRender(const std::filesystem::path &filePath);
    void DetachRenderWindow();

private:
    struct SceneEntry
    {
        std::filesystem::path mPath;
        std::string mLabel;
    };

    RTUI::ImGuiHost mHost;
    std::filesystem::path mExecutablePath;
    std::filesystem::path mResourcesPath;
    std::vector<SceneEntry> mScenes;
    ImGuiTextFilter mSceneFilter;
    RI::Hider::Params mHiderParams;
    std::unique_ptr<DispDriverFBuffOGL> moDisplay;
    std::filesystem::path mPendingFile;
    std::filesystem::path mLastFile;
    std::string mPathInput;
    std::string mStatus;
    double mLastRenderMilliseconds{};
    float mCameraYawDegrees{};
    float mCameraPitchDegrees{};
    float mCameraTruck{};
    float mCameraPedestal{};
    float mCameraDolly{};
    float mCameraMovementScale{1.0f};
    float mCameraFovDegrees{45.0f};
    int mLastUsedWidth{};
    int mLastUsedHeight{};
    int mRequestedWidth{512};
    int mRequestedHeight{512};
    int mPreviewLongDimension{192};
    int mPendingBucketX{-1};
    int mPendingBucketY{-1};
    int mSelectedScene{-1};
    bool mUseSceneResolution{true};
    bool mFitImage{true};
    bool mOverrideCameraFov{};
    bool mRenderRequested{};
    bool mPreviewRenderRequested{};
    bool mLastRenderWasPreview{};
    bool mCameraDragActive{};
    bool mStatusIsError{};
    bool mFirstLayout{true};
    bool mResetLayoutRequested{};
    bool mDetachRenderRequested{};

    void scanScenes();
    void processPendingRender();
    bool renderFile(
        const std::filesystem::path &filePath,
        int forcedWidth,
        int forcedHeight,
        int forcedLongDimension,
        bool isPreview);
    void handleDisplays(const RRL::DisplayList &displays);

    void drawWorkspace();
    void drawMainMenu();
    void drawSceneBrowser();
    void drawRenderSettings();
    void drawRenderView();
    void buildInitialLayout(ImGuiID dockspaceId);

    void requestRender();
    void requestPreviewRender();
    void requestBucketRender(int x, int y);
    void resetCamera();

    static void handleDisplaysCallback(void* pUserData, const RRL::DisplayList &displays);
    static void dropCallback(GLFWwindow* pWindow, int pathCount, const char** ppPaths);
};

#endif
