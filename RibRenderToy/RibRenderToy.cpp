//==================================================================
/// RibRenderToy.cpp
///
/// Created by Davide Pasca - 2009/4/25
/// Dear ImGui front end added in 2026.
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "RibRenderToy.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <exception>
#include <numbers>
#include <system_error>

#include <GLFW/glfw3.h>

#include "DSystem/include/DUtils.h"
#include "DSystem/include/DUtils_Files.h"
#include "RibToolsBase/include/RibToolsBase.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

namespace fs = std::filesystem;

namespace
{

constexpr const char* APP_NAME = "RibRenderToy";
constexpr const char* APP_VERSION = "0.11";
constexpr float CAMERA_ROTATION_SPEED = 0.25f;
constexpr float CAMERA_TRANSLATION_SPEED = 0.01f;
constexpr float CAMERA_DOLLY_SPEED = 0.25f;

//==================================================================
Matrix44 makeCameraAdjustment(
    float yawDegrees,
    float pitchDegrees,
    float truck,
    float pedestal,
    float dolly)
{
    constexpr auto degreesToRadians = std::numbers::pi_v<float> / 180.0f;
    const auto translation = Matrix44::Translate(-truck, -pedestal, -dolly);
    const auto yaw = Matrix44::Rot(-yawDegrees * degreesToRadians, 0.0f, 1.0f, 0.0f);
    const auto pitch = Matrix44::Rot(-pitchDegrees * degreesToRadians, 1.0f, 0.0f, 0.0f);
    return translation * yaw * pitch;
}

//==================================================================
bool isHelpArgument(const char* pArgument)
{
    return std::strcmp(pArgument, "-h") == 0 || std::strcmp(pArgument, "--help") == 0 ||
           std::strcmp(pArgument, "-help") == 0;
}

//==================================================================
bool isSmokeTestArgument(const char* pArgument)
{
    return std::strcmp(pArgument, "--smoke-test") == 0;
}

//==================================================================
bool isDetachRenderArgument(const char* pArgument)
{
    return std::strcmp(pArgument, "--detach-render") == 0;
}

//==================================================================
void printUsage(const char* pExecutable)
{
    std::printf("%s v%s\n\n", APP_NAME, APP_VERSION);
    std::printf("%s [--detach-render] [scene.rib]\n", pExecutable);
    std::printf("%s --smoke-test [scene.rib]\n", pExecutable);
    std::printf("\nDrop a RIB file into the window or select an included scene in the Scenes panel.\n");
}

}

//==================================================================
RibRenderToyApp::RibRenderToyApp(const fs::path &executablePath)
    : mHost(RTUI::ImGuiHost::Params{std::string(APP_NAME) + " v" + APP_VERSION, 1200, 760, "rib-render-toy.ini"}),
      mExecutablePath(executablePath), mResourcesPath(executablePath / "Resources"),
      mStatus("Choose an included scene or drop a RIB file into the window.")
{
    glfwSetWindowUserPointer(mHost.GetWindow(), this);
    glfwSetDropCallback(mHost.GetWindow(), dropCallback);
    scanScenes();
}

//==================================================================
int RibRenderToyApp::Run(int frameLimit)
{
    auto renderedFrameCount = 0;

    while (!mHost.ShouldClose())
    {
        if (!mHost.BeginFrame())
        {
            continue;
        }

        processPendingRender();
        drawWorkspace();
        mHost.EndFrame();

        ++renderedFrameCount;
        if (frameLimit > 0 && renderedFrameCount >= frameLimit)
        {
            mHost.RequestClose();
        }
    }

    if (frameLimit > 0 && (mStatusIsError || !moDisplay))
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

//==================================================================
void RibRenderToyApp::SetFileToRender(const fs::path &filePath)
{
    std::error_code error;
    auto absolutePath = filePath;
    if (filePath.is_relative())
    {
        absolutePath = fs::absolute(filePath, error);
        if (error)
        {
            absolutePath = filePath;
        }
    }

    resetCamera();
    mPreviewRenderRequested = false;
    mPendingFile = absolutePath.lexically_normal();
    mPathInput = mPendingFile.string();
    mRenderRequested = true;
    mStatus = "Render queued...";
    mStatusIsError = false;
}

//==================================================================
void RibRenderToyApp::DetachRenderWindow()
{
    mDetachRenderRequested = true;
}

//==================================================================
void RibRenderToyApp::scanScenes()
{
    mScenes.clear();

    const auto scenesRoot = mExecutablePath / "TestScenes";
    std::error_code error;
    if (!fs::is_directory(scenesRoot, error))
    {
        mStatus = "The bundled TestScenes directory could not be found.";
        mStatusIsError = true;
        return;
    }

    const auto options = fs::directory_options::skip_permission_denied;
    for (fs::recursive_directory_iterator it(scenesRoot, options, error), end; it != end; it.increment(error))
    {
        if (error)
        {
            error.clear();
            continue;
        }

        if (!it->is_regular_file(error) || it->path().extension() != ".rib")
        {
            continue;
        }

        auto relativePath = fs::relative(it->path(), scenesRoot, error);
        if (error)
        {
            error.clear();
            relativePath = it->path().filename();
        }

        mScenes.push_back(SceneEntry{it->path().lexically_normal(), relativePath.generic_string()});
    }

    std::sort(mScenes.begin(), mScenes.end(),
              [](const SceneEntry &lhs, const SceneEntry &rhs)
              {
                  return lhs.mLabel < rhs.mLabel;
              });
}

//==================================================================
void RibRenderToyApp::processPendingRender()
{
    if (!mRenderRequested)
    {
        return;
    }

    mRenderRequested = false;

    if (!mPendingFile.empty())
    {
        mLastFile = mPendingFile;
        mPendingFile.clear();
    }

    if (mLastFile.empty())
    {
        return;
    }

    std::error_code error;
    if (!fs::is_regular_file(mLastFile, error))
    {
        mStatus = "RIB file not found: " + mLastFile.string();
        mStatusIsError = true;
        return;
    }

    const auto forcedWidth = mUseSceneResolution ? -1 : std::clamp(mRequestedWidth, 1, 16384);
    const auto forcedHeight = mUseSceneResolution ? -1 : std::clamp(mRequestedHeight, 1, 16384);
    const auto isPreview = mPreviewRenderRequested;
    const auto forcedLongDimension = isPreview ? std::clamp(mPreviewLongDimension, 32, 4096) : -1;
    mPreviewRenderRequested = false;

    mHiderParams.mDbgOnlyBucketAtX = mPendingBucketX;
    mHiderParams.mDbgOnlyBucketAtY = mPendingBucketY;
    renderFile(mLastFile, forcedWidth, forcedHeight, forcedLongDimension, isPreview);
    mHiderParams.mDbgOnlyBucketAtX = -1;
    mHiderParams.mDbgOnlyBucketAtY = -1;
    mPendingBucketX = -1;
    mPendingBucketY = -1;
}

//==================================================================
bool RibRenderToyApp::renderFile(
    const fs::path &filePath,
    int forcedWidth,
    int forcedHeight,
    int forcedLongDimension,
    bool isPreview)
{
    const auto startTime = std::chrono::steady_clock::now();

    RI::Framework::Params frameworkParams;
    frameworkParams.mFallBackFBuffDisplay = true;
    frameworkParams.mpHiderParams = &mHiderParams;
    RI::Framework framework(frameworkParams);

    DIO::FileManagerDisk fileManager;

    RRL::Render::Params params;
    params.mTrans.mState.mpFramework = &framework;
    params.mTrans.mState.mpFileManager = &fileManager;
    const auto baseDirectory = filePath.parent_path().string();
    params.mTrans.mState.mBaseDir = baseDirectory.c_str();

    const auto shadersPath = mResourcesPath / "Shaders";
    const auto shadersDirectory = shadersPath.string();
    params.mTrans.mState.mDefaultShadersDir = shadersDirectory.c_str();
    params.mTrans.mState.mCameraAdjustment = makeCameraAdjustment(
        mCameraYawDegrees,
        mCameraPitchDegrees,
        mCameraTruck,
        mCameraPedestal,
        mCameraDolly);

    if (mOverrideCameraFov)
    {
        params.mTrans.mForcedFov = mCameraFovDegrees;
    }

    if (mHiderParams.mDbgColorCodedGrids)
    {
        params.mTrans.mState.mForcedSurfaceShader = "constant";
    }

    params.mTrans.mForcedWd = forcedWidth;
    params.mTrans.mForcedHe = forcedHeight;
    params.mTrans.mForcedLongDim = forcedLongDimension;

    const auto filePathString = filePath.string();
    params.mpFileName = filePathString.c_str();
    params.mpOnFrameEndCB = handleDisplaysCallback;
    params.mpOnFrameEndCBData = this;

    try
    {
        RRL::Render render(params);
    }
    catch (const std::bad_alloc &)
    {
        mStatus = "The render ran out of memory.";
        mStatusIsError = true;
        return false;
    }
    catch (const RI::Exception &exception)
    {
        mStatus = exception.GetMessage_().c_str();
        mStatusIsError = true;
        return false;
    }
    catch (const std::exception &exception)
    {
        mStatus = exception.what();
        mStatusIsError = true;
        return false;
    }
    catch (...)
    {
        mStatus = "An unknown exception interrupted the render.";
        mStatusIsError = true;
        return false;
    }

    const auto endTime = std::chrono::steady_clock::now();
    mLastRenderMilliseconds = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    mLastRenderWasPreview = isPreview;
    mStatus = isPreview ? "Rendered camera preview of " : "Rendered ";
    mStatus += filePath.filename().string();
    mStatusIsError = false;
    return true;
}

//==================================================================
void RibRenderToyApp::handleDisplays(const RRL::DisplayList &displays)
{
    if (displays.empty())
    {
        return;
    }

    const auto* pDisplay = displays.front();
    auto display = std::make_unique<DispDriverFBuffOGL>(pDisplay->mName.c_str(), pDisplay->mImage);

    mLastUsedWidth = static_cast<int>(pDisplay->mImage.mWd);
    mLastUsedHeight = static_cast<int>(pDisplay->mImage.mHe);
    moDisplay = std::move(display);
}

//==================================================================
void RibRenderToyApp::drawWorkspace()
{
    drawMainMenu();

    const auto dockspaceId = ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
    buildInitialLayout(dockspaceId);

    drawSceneBrowser();
    drawRenderSettings();
    drawRenderView();
}

//==================================================================
void RibRenderToyApp::drawMainMenu()
{
    if (!ImGui::BeginMainMenuBar())
    {
        return;
    }

    if (ImGui::BeginMenu("File"))
    {
        const auto canRender = !mLastFile.empty() || !mPendingFile.empty();
        if (ImGui::MenuItem("Render Again", nullptr, false, canRender))
        {
            requestRender();
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Quit"))
        {
            mHost.RequestClose();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
        ImGui::MenuItem("Fit Image", nullptr, &mFitImage);
        if (ImGui::MenuItem("Detach Render Window"))
        {
            mDetachRenderRequested = true;
        }
        if (ImGui::MenuItem("Reset Docking Layout"))
        {
            mResetLayoutRequested = true;
        }
        ImGui::Separator();
        ImGui::TextDisabled("Render panels can be dragged outside the main window.");
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

//==================================================================
void RibRenderToyApp::drawSceneBrowser()
{
    ImGui::Begin("Scenes");
    ImGui::TextWrapped("Select a bundled scene, enter a path, or drop a .rib file onto the app.");

    ImGui::SetNextItemWidth(-70.0f);
    const auto openFromKeyboard =
        ImGui::InputTextWithHint("##rib-path", "Path to a RIB file", &mPathInput, ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    const auto openFromButton = ImGui::Button("Open");

    if ((openFromKeyboard || openFromButton) && !mPathInput.empty())
    {
        auto path = fs::path(mPathInput);
        std::error_code error;
        if (path.is_relative() && !fs::exists(path, error))
        {
            path = mExecutablePath / path;
        }
        SetFileToRender(path);
    }

    ImGui::TextUnformatted("Filter");
    mSceneFilter.Draw("##scene-filter", -1.0f);
    ImGui::Separator();

    if (ImGui::BeginChild("SceneList"))
    {
        for (size_t sceneIndex = 0; sceneIndex < mScenes.size(); ++sceneIndex)
        {
            const auto &scene = mScenes[sceneIndex];
            if (!mSceneFilter.PassFilter(scene.mLabel.c_str()))
            {
                continue;
            }

            const auto selected = mSelectedScene == static_cast<int>(sceneIndex);
            if (ImGui::Selectable(scene.mLabel.c_str(), selected))
            {
                mSelectedScene = static_cast<int>(sceneIndex);
                SetFileToRender(scene.mPath);
            }

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::SetTooltip("%s", scene.mPath.string().c_str());
            }
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

//==================================================================
void RibRenderToyApp::drawRenderSettings()
{
    ImGui::Begin("Render Settings");

    if (mLastFile.empty())
    {
        ImGui::TextDisabled("No scene selected");
    }
    else
    {
        ImGui::TextWrapped("%s", mLastFile.filename().string().c_str());
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        {
            ImGui::SetTooltip("%s", mLastFile.string().c_str());
        }
    }

    const auto canRender = !mLastFile.empty() || !mPendingFile.empty();
    ImGui::BeginDisabled(!canRender);
    if (ImGui::Button("Render full resolution", ImVec2(-1.0f, 0.0f)))
    {
        requestRender();
    }
    ImGui::EndDisabled();

    ImGui::SeparatorText("Resolution");
    ImGui::Checkbox("Use scene resolution", &mUseSceneResolution);

    ImGui::BeginDisabled(mUseSceneResolution);
    ImGui::TextUnformatted("Width");
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputInt("##render-width", &mRequestedWidth);
    ImGui::TextUnformatted("Height");
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputInt("##render-height", &mRequestedHeight);
    mRequestedWidth = std::clamp(mRequestedWidth, 1, 16384);
    mRequestedHeight = std::clamp(mRequestedHeight, 1, 16384);
    ImGui::EndDisabled();

    ImGui::TextUnformatted("Camera preview long side");
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputInt("##preview-long-side", &mPreviewLongDimension);
    mPreviewLongDimension = std::clamp(mPreviewLongDimension, 32, 4096);

    ImGui::Checkbox("Fit image to panel", &mFitImage);

    ImGui::SeparatorText("Camera");
    ImGui::TextWrapped("Drag the image to turn. Shift-drag to pan; use the mouse wheel to dolly.");

    ImGui::BeginDisabled(!canRender);
    auto cameraEditFinished = false;

    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("Yaw##camera", &mCameraYawDegrees, 0.25f, -360.0f, 360.0f, "%.2f deg");
    cameraEditFinished |= ImGui::IsItemDeactivatedAfterEdit();

    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("Pitch##camera", &mCameraPitchDegrees, 0.25f, -89.0f, 89.0f, "%.2f deg");
    cameraEditFinished |= ImGui::IsItemDeactivatedAfterEdit();
    mCameraPitchDegrees = std::clamp(mCameraPitchDegrees, -89.0f, 89.0f);

    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("Truck##camera", &mCameraTruck, 0.05f, 0.0f, 0.0f, "%.3f");
    cameraEditFinished |= ImGui::IsItemDeactivatedAfterEdit();

    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("Pedestal##camera", &mCameraPedestal, 0.05f, 0.0f, 0.0f, "%.3f");
    cameraEditFinished |= ImGui::IsItemDeactivatedAfterEdit();

    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("Dolly##camera", &mCameraDolly, 0.05f, 0.0f, 0.0f, "%.3f");
    cameraEditFinished |= ImGui::IsItemDeactivatedAfterEdit();

    ImGui::SetNextItemWidth(-1.0f);
    ImGui::DragFloat("Mouse movement scale", &mCameraMovementScale, 0.05f, 0.001f, 10000.0f, "%.3f");
    mCameraMovementScale = std::clamp(mCameraMovementScale, 0.001f, 10000.0f);

    if (ImGui::Checkbox("Override field of view", &mOverrideCameraFov))
    {
        requestPreviewRender();
    }

    ImGui::BeginDisabled(!mOverrideCameraFov);
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::SliderFloat("Field of view", &mCameraFovDegrees, 5.0f, 150.0f, "%.1f deg");
    cameraEditFinished |= ImGui::IsItemDeactivatedAfterEdit();
    ImGui::EndDisabled();

    if (cameraEditFinished)
    {
        requestPreviewRender();
    }

    if (ImGui::Button("Reset camera", ImVec2(-1.0f, 0.0f)))
    {
        resetCamera();
        requestPreviewRender();
    }
    if (ImGui::Button("Render camera preview", ImVec2(-1.0f, 0.0f)))
    {
        requestPreviewRender();
    }
    ImGui::EndDisabled();

    ImGui::SeparatorText("Debug");
    auto debugChanged = false;
    debugChanged |= ImGui::Checkbox("Pick bucket", &mHiderParams.mDbgShowBuckets);
    debugChanged |= ImGui::Checkbox("Color-coded grids", &mHiderParams.mDbgColorCodedGrids);
    debugChanged |= ImGui::Checkbox("Rasterize vertices", &mHiderParams.mDbgRasterizeVerts);

    if (debugChanged && !mLastFile.empty())
    {
        requestRender();
    }

    ImGui::SeparatorText("Status");
    if (mStatusIsError)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.42f, 0.42f, 1.0f));
    }
    ImGui::TextWrapped("%s", mStatus.c_str());
    if (mStatusIsError)
    {
        ImGui::PopStyleColor();
    }

    if (moDisplay)
    {
        ImGui::Text(
            "%d x %d, %.2f ms%s",
            mLastUsedWidth,
            mLastUsedHeight,
            mLastRenderMilliseconds,
            mLastRenderWasPreview ? " (camera preview)" : "");
    }

    ImGui::Spacing();
    ImGui::TextDisabled("Drag the Render tab out, or use View > Detach Render Window.");
    ImGui::End();
}

//==================================================================
void RibRenderToyApp::drawRenderView()
{
    if (mDetachRenderRequested)
    {
        if (auto* pRenderWindow = ImGui::FindWindowByName("Render"))
        {
            ImGui::DockContextProcessUndockWindow(ImGui::GetCurrentContext(), pRenderWindow);
        }

        const auto* pMainViewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowDockID(0, ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(pMainViewport->Pos.x + 80.0f, pMainViewport->Pos.y + 80.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(760.0f, 620.0f), ImGuiCond_Always);
        mDetachRenderRequested = false;
    }

    ImGui::Begin("Render");

    if (!moDisplay)
    {
        const auto available = ImGui::GetContentRegionAvail();
        const auto messageSize = ImGui::CalcTextSize("Choose a scene to render");
        ImGui::SetCursorPos(ImVec2(std::max(0.0f, (available.x - messageSize.x) * 0.5f),
                                   std::max(0.0f, (available.y - messageSize.y) * 0.5f)));
        ImGui::TextDisabled("Choose a scene to render");
        ImGui::End();
        return;
    }

    const auto imageWidth = static_cast<float>(moDisplay->GetCurWd());
    const auto imageHeight = static_cast<float>(moDisplay->GetCurHe());
    const auto available = ImGui::GetContentRegionAvail();

    auto scale = 1.0f;
    if (mFitImage && imageWidth > 0.0f && imageHeight > 0.0f)
    {
        scale = std::min(available.x / imageWidth, available.y / imageHeight);
        scale = std::max(scale, 0.01f);
    }

    const ImVec2 displaySize(imageWidth * scale, imageHeight * scale);
    if (mFitImage)
    {
        const auto cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(cursor.x + std::max(0.0f, (available.x - displaySize.x) * 0.5f),
                                   cursor.y + std::max(0.0f, (available.y - displaySize.y) * 0.5f)));
    }

    const auto texture = ImTextureRef(static_cast<ImTextureID>(moDisplay->GetTextureId()));
    ImGui::Image(texture, displaySize, ImVec2(0.0f, 0.0f),
                 ImVec2(moDisplay->GetTextureUMax(), moDisplay->GetTextureVMax()));
    const auto imageHovered = ImGui::IsItemHovered();

    if (mHiderParams.mDbgShowBuckets && imageHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        const auto mouse = ImGui::GetMousePos();
        const auto imageMin = ImGui::GetItemRectMin();
        const auto x =
            std::clamp(static_cast<int>((mouse.x - imageMin.x) * imageWidth / displaySize.x), 0, mLastUsedWidth - 1);
        const auto y =
            std::clamp(static_cast<int>((mouse.y - imageMin.y) * imageHeight / displaySize.y), 0, mLastUsedHeight - 1);
        requestBucketRender(x, y);
    }

    if (!mHiderParams.mDbgShowBuckets && imageHovered)
    {
        const auto &io = ImGui::GetIO();
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            if (io.KeyShift)
            {
                mCameraTruck -= io.MouseDelta.x * CAMERA_TRANSLATION_SPEED * mCameraMovementScale;
                mCameraPedestal += io.MouseDelta.y * CAMERA_TRANSLATION_SPEED * mCameraMovementScale;
            }
            else
            {
                mCameraYawDegrees += io.MouseDelta.x * CAMERA_ROTATION_SPEED;
                mCameraPitchDegrees += io.MouseDelta.y * CAMERA_ROTATION_SPEED;
                mCameraPitchDegrees = std::clamp(mCameraPitchDegrees, -89.0f, 89.0f);
            }
            mCameraDragActive = true;
        }

        if (io.MouseWheel != 0.0f)
        {
            mCameraDolly += io.MouseWheel * CAMERA_DOLLY_SPEED * mCameraMovementScale;
            requestPreviewRender();
        }
    }

    if (mCameraDragActive && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        mCameraDragActive = false;
        requestPreviewRender();
    }

    if (ImGui::BeginPopupContextItem("RenderContextMenu"))
    {
        if (ImGui::MenuItem("Render Again"))
        {
            requestRender();
        }
        if (ImGui::MenuItem("Render Camera Preview"))
        {
            requestPreviewRender();
        }
        if (ImGui::MenuItem("Reset Camera"))
        {
            resetCamera();
            requestPreviewRender();
        }
        ImGui::MenuItem("Fit Image", nullptr, &mFitImage);
        ImGui::EndPopup();
    }

    ImGui::End();
}

//==================================================================
void RibRenderToyApp::buildInitialLayout(ImGuiID dockspaceId)
{
    if (!mFirstLayout && !mResetLayoutRequested)
    {
        return;
    }
    mFirstLayout = false;

    const auto* pExistingNode = ImGui::DockBuilderGetNode(dockspaceId);
    if (!mResetLayoutRequested && pExistingNode && pExistingNode->IsSplitNode())
    {
        return;
    }
    mResetLayoutRequested = false;

    ImGui::DockBuilderRemoveNode(dockspaceId);
    ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetMainViewport()->WorkSize);

    ImGuiID leftDockId = 0;
    auto renderDockId = dockspaceId;
    ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.27f, &leftDockId, &renderDockId);

    ImGuiID settingsDockId = 0;
    ImGuiID scenesDockId = 0;
    ImGui::DockBuilderSplitNode(leftDockId, ImGuiDir_Down, 0.45f, &settingsDockId, &scenesDockId);

    ImGui::DockBuilderDockWindow("Scenes", scenesDockId);
    ImGui::DockBuilderDockWindow("Render Settings", settingsDockId);
    ImGui::DockBuilderDockWindow("Render", renderDockId);
    ImGui::DockBuilderFinish(dockspaceId);
}

//==================================================================
void RibRenderToyApp::requestRender()
{
    if (!mLastFile.empty() || !mPendingFile.empty())
    {
        mPreviewRenderRequested = false;
        mRenderRequested = true;
    }
}

//==================================================================
void RibRenderToyApp::requestPreviewRender()
{
    if (!mLastFile.empty() || !mPendingFile.empty())
    {
        mPreviewRenderRequested = true;
        mRenderRequested = true;
    }
}

//==================================================================
void RibRenderToyApp::requestBucketRender(int x, int y)
{
    mPendingBucketX = x;
    mPendingBucketY = y;
    requestRender();
}

//==================================================================
void RibRenderToyApp::resetCamera()
{
    mCameraYawDegrees = 0.0f;
    mCameraPitchDegrees = 0.0f;
    mCameraTruck = 0.0f;
    mCameraPedestal = 0.0f;
    mCameraDolly = 0.0f;
    mCameraMovementScale = 1.0f;
    mCameraFovDegrees = 45.0f;
    mOverrideCameraFov = false;
    mCameraDragActive = false;
}

//==================================================================
void RibRenderToyApp::handleDisplaysCallback(void* pUserData, const RRL::DisplayList &displays)
{
    static_cast<RibRenderToyApp*>(pUserData)->handleDisplays(displays);
}

//==================================================================
void RibRenderToyApp::dropCallback(GLFWwindow* pWindow, int pathCount, const char** ppPaths)
{
    auto* pApp = static_cast<RibRenderToyApp*>(glfwGetWindowUserPointer(pWindow));
    if (!pApp || pathCount < 1 || !ppPaths || !ppPaths[0])
    {
        return;
    }

    pApp->SetFileToRender(ppPaths[0]);
}

//==================================================================
int main(int argc, char** argv)
{
    if (argc >= 2 && isHelpArgument(argv[1]))
    {
        printUsage(argv[0]);
        return EXIT_SUCCESS;
    }

    DUT::InstallFileManagerStd();

    try
    {
        const DStr executablePath = RTB::FindRibToolsDir(argv);
        RibRenderToyApp app(executablePath.c_str());

        if (argc >= 2 && isSmokeTestArgument(argv[1]))
        {
            const auto smokeScene = argc >= 3
                                        ? fs::path(argv[2])
                                        : fs::path(executablePath.c_str()) / "TestScenes/LightsTests/SphereAmbient.rib";
            app.SetFileToRender(smokeScene);
            return app.Run(3);
        }

        fs::path scenePath;
        for (auto argumentIndex = 1; argumentIndex < argc; ++argumentIndex)
        {
            if (isDetachRenderArgument(argv[argumentIndex]))
            {
                app.DetachRenderWindow();
                continue;
            }

            if (!scenePath.empty())
            {
                std::fprintf(stderr, "Unexpected argument: %s\n", argv[argumentIndex]);
                printUsage(argv[0]);
                return EXIT_FAILURE;
            }
            scenePath = argv[argumentIndex];
        }

        if (!scenePath.empty())
        {
            app.SetFileToRender(scenePath);
        }

        return app.Run();
    }
    catch (const std::exception &exception)
    {
        std::fprintf(stderr, "%s\n", exception.what());
        return EXIT_FAILURE;
    }
}
