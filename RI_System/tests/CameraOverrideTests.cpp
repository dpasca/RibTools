#include <cmath>
#include <cstdlib>
#include <numbers>

#include "DSystem/include/DUtils_Files.h"
#include "RibRenderLib/include/RRL_Translator.h"

namespace
{

bool matricesAreNear(const Matrix44 &lhs, const Matrix44 &rhs)
{
    constexpr float tolerance = 0.00001f;
    for (size_t row = 0; row < 4; ++row)
    {
        for (size_t column = 0; column < 4; ++column)
        {
            if (std::fabs(lhs.mij(row, column) - rhs.mij(row, column)) > tolerance)
            {
                return false;
            }
        }
    }

    return true;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        return EXIT_FAILURE;
    }

    DUT::InstallFileManagerStd();

    RI::Hider::Params hiderParams;
    RI::Framework::Params frameworkParams;
    frameworkParams.mpHiderParams = &hiderParams;
    RI::Framework framework(frameworkParams);

    DIO::FileManagerDisk fileManager;
    RRL::Translator::Params translatorParams;
    translatorParams.mState.mpFramework = &framework;
    translatorParams.mState.mpFileManager = &fileManager;
    translatorParams.mState.mDefaultShadersDir = argv[1];
    translatorParams.mState.mCameraAdjustment = Matrix44::Translate(1.0f, -2.0f, 3.0f);
    translatorParams.mForcedFov = 60.0f;

    RRL::Translator translator(translatorParams);
    auto &state = translator.GetState();
    state.Begin("camera-override-test");
    state.Format(1, 1, 1.0f);
    state.Translate(0.0f, 0.0f, 4.0f);

    RI::ParamList projectionParams;
    projectionParams.Add(RI_PERSPECTIVE);
    projectionParams.Add(RI_FOV);
    projectionParams.Add(30.0f);
    translator.AddCommand("Projection", projectionParams, "CameraOverrideTests", 1);

    state.WorldBegin();

    const auto expectedCamera = Matrix44::Translate(0.0f, 0.0f, 4.0f) * Matrix44::Translate(1.0f, -2.0f, 3.0f);
    const auto expectedProjection =
        Matrix44::Perspective(60.0f * std::numbers::pi_v<float> / 180.0f, 1.0f, RI_EPSILON, RI_INFINITY);

    const auto cameraIsCorrect = matricesAreNear(state.GetWorldCameraMtx(), expectedCamera);
    const auto projectionIsCorrect = matricesAreNear(state.GetCurOptions().mMtxCamProj, expectedProjection);
    const auto fovWasOverridden = std::fabs(projectionParams[2].Flt() - 60.0f) < 0.00001f;

    state.WorldEnd();
    state.End();

    return cameraIsCorrect && projectionIsCorrect && fovWasOverridden ? EXIT_SUCCESS : EXIT_FAILURE;
}
