#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string_view>
#include <vector>

#include "DImage/include/DImage.h"
#include "DImage/include/DImage_TIFF.h"
#include "DSystem/include/DUtils_MemFile.h"

namespace
{

constexpr U32 kExpectedWidth = 160;
constexpr U32 kExpectedHeight = 103;
constexpr size_t kSignatureWidth = 20;
constexpr size_t kSignatureHeight = 13;
constexpr size_t kSignatureSize = kSignatureWidth * kSignatureHeight;
constexpr int kLargeDifference = 48;

using Signature = std::array<std::uint8_t, kSignatureSize>;

// Regenerate these values only after visually reviewing an intentional change:
// RIKillerooVisualTests <render.tif> --print-reference
// clang-format off
constexpr Signature kExpectedLuminance = {
    0,   0,   0,   0,  0,  0,  0,  0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   7,   55,
    26,  0,   0,   0,  0,  0,  0,  0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   34,  85,  200, 194, 26,  0,   0,
    65,  152, 123, 23, 0,  0,  0,  0,   0,   0,  0,   0,   37,  197, 152, 156, 148, 201, 141, 143, 182, 201, 203, 224,
    67,  0,   0,   0,  0,  0,  0,  0,   6,   40, 30,  32,  26,  160, 239, 235, 213, 155, 171, 202, 218, 125, 48,  69,
    137, 146, 103, 20, 0,  0,  0,  0,   3,   75, 197, 206, 183, 84,  158, 167, 145, 145, 166, 137, 43,  0,   0,   0,
    0,   0,   0,   0,  0,  0,  22, 132, 106, 78, 156, 42,  4,   2,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   71,  23,  37, 69, 67, 97, 0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   39,  69,  22,
    0,   16,  103, 85, 0,  0,  0,  0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   4,   0,   74,  22,  66,
    0,   0,   0,   0,  0,  0,  0,  0,   0,   0,  0,   0,   0,   0,   0,   15,  117, 79,  105, 7,   0,   0,   0,   0,
    0,   0,   0,   0,  0,  0,  0,  0,   0,   0,  0,   23,  62,  129, 122, 0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,  0,  0,  0,  0,   13,  34, 1,   0,   0,   0,   0,   0,   0,   0,   0,   0};
constexpr Signature kExpectedAlpha = {
    0,   0,   0,   3,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    12,  173, 81,  0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   54,  227, 255,
    241, 27,  0,   0,   96,  183, 134, 27, 0,   0,   0,   0,   0,   0,   0,   0,   68,  254, 255, 255, 255, 228,
    151, 177, 255, 255, 255, 248, 72,  0,  0,   0,   0,   0,   0,   0,   19,  109, 104, 139, 250, 255, 255, 255,
    255, 255, 255, 255, 249, 132, 53,  85, 167, 189, 161, 39,  0,   0,   0,   0,   174, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 253, 189, 65,  0,  0,   0,   0,   0,   0,   0,   46,  232, 255, 255, 253, 254, 245, 106,
    48,  40,  14,  0,   0,   0,   0,   0,  0,   0,   0,   0,   0,   138, 172, 115, 231, 255, 135, 0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,  0,   0,   0,   84,  195, 39,  9,   150, 238, 116, 0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,  0,   0,   24,  12,  1,   154, 71,  105, 0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,  0,   47,  178, 123, 181, 10,  0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   49, 113, 197, 154, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   25,  60, 1,   0,   0,   0,   0,   0,   0,   0,   0,   0};

// clang-format on

struct VisualSignature
{
    Signature mLuminance{};
    Signature mAlpha{};
};

struct DifferenceSummary
{
    double mMean = 0.0;
    int mMaximum = 0;
    size_t mLargeCellCount = 0;
};

std::uint8_t quantizeSample(double value)
{
    const auto clampedValue = std::clamp(value, 0.0, 1.0);
    return static_cast<std::uint8_t>(std::lround(clampedValue * 255.0));
}

VisualSignature makeSignature(const DIMG::Image &image)
{
    std::array<double, kSignatureSize> luminanceSums{};
    std::array<double, kSignatureSize> alphaSums{};
    std::array<size_t, kSignatureSize> pixelCounts{};

    for (U32 y = 0; y < image.mHe; ++y)
    {
        const size_t signatureY = static_cast<size_t>(y) * kSignatureHeight / image.mHe;
        for (U32 x = 0; x < image.mWd; ++x)
        {
            const size_t signatureX = static_cast<size_t>(x) * kSignatureWidth / image.mWd;
            const size_t signatureIndex = signatureY * kSignatureWidth + signatureX;

            float samples[DIMG::Image::MAX_SAMP_PER_PIX]{};
            image.GetPixelAsFloat(x, y, samples);

            const double luminance = 0.2126 * samples[0] + 0.7152 * samples[1] + 0.0722 * samples[2];
            const double alpha = image.mSampPerPix >= 4 ? samples[3] : 1.0;

            luminanceSums[signatureIndex] += luminance;
            alphaSums[signatureIndex] += alpha;
            ++pixelCounts[signatureIndex];
        }
    }

    VisualSignature signature;
    for (size_t i = 0; i < kSignatureSize; ++i)
    {
        const auto pixelCount = static_cast<double>(pixelCounts[i]);
        signature.mLuminance[i] = quantizeSample(luminanceSums[i] / pixelCount);
        signature.mAlpha[i] = quantizeSample(alphaSums[i] / pixelCount);
    }
    return signature;
}

void printSignature(const char* pName, const Signature &signature)
{
    std::printf("constexpr Signature %s =\n{\n", pName);
    for (size_t y = 0; y < kSignatureHeight; ++y)
    {
        std::printf("    ");
        for (size_t x = 0; x < kSignatureWidth; ++x)
        {
            const size_t index = y * kSignatureWidth + x;
            std::printf("%3u%s", static_cast<unsigned int>(signature[index]), index + 1 == kSignatureSize ? "" : ",");
            if (x + 1 != kSignatureWidth)
                std::printf(" ");
        }
        std::printf("\n");
    }
    std::printf("};\n");
}

DifferenceSummary compareSignatures(const Signature &actual, const Signature &expected)
{
    DifferenceSummary summary;
    double differenceTotal = 0.0;

    for (size_t i = 0; i < kSignatureSize; ++i)
    {
        const int difference = std::abs(static_cast<int>(actual[i]) - static_cast<int>(expected[i]));
        differenceTotal += difference;
        summary.mMaximum = std::max(summary.mMaximum, difference);
        if (difference > kLargeDifference)
            ++summary.mLargeCellCount;
    }

    summary.mMean = differenceTotal / kSignatureSize;
    return summary;
}

bool validateKillerooRender(const DIMG::Image &image, bool printReference)
{
    if (image.mWd != kExpectedWidth || image.mHe != kExpectedHeight || image.mSampPerPix < 4)
    {
        std::fprintf(stderr, "Killeroo render format is %ux%u with %u samples; expected %ux%u RGBA\n", image.mWd,
                     image.mHe, image.mSampPerPix, kExpectedWidth, kExpectedHeight);
        return false;
    }

    const auto actual = makeSignature(image);
    if (printReference)
    {
        printSignature("kExpectedLuminance", actual.mLuminance);
        printSignature("kExpectedAlpha", actual.mAlpha);
        return true;
    }

    const auto luminanceDifference = compareSignatures(actual.mLuminance, kExpectedLuminance);
    const auto alphaDifference = compareSignatures(actual.mAlpha, kExpectedAlpha);

    constexpr double kMaximumMeanLuminanceDifference = 8.0;
    constexpr double kMaximumMeanAlphaDifference = 6.0;
    constexpr size_t kMaximumLargeDifferenceCells = 12;

    std::printf("Killeroo visual regression: luminance mean %.2f, max %d, large cells %zu; "
                "alpha mean %.2f, max %d, large cells %zu\n",
                luminanceDifference.mMean, luminanceDifference.mMaximum, luminanceDifference.mLargeCellCount,
                alphaDifference.mMean, alphaDifference.mMaximum, alphaDifference.mLargeCellCount);

    const bool matchesReference = luminanceDifference.mMean <= kMaximumMeanLuminanceDifference &&
                                  alphaDifference.mMean <= kMaximumMeanAlphaDifference &&
                                  luminanceDifference.mLargeCellCount <= kMaximumLargeDifferenceCells &&
                                  alphaDifference.mLargeCellCount <= kMaximumLargeDifferenceCells;
    if (!matchesReference)
    {
        std::fprintf(stderr, "Killeroo render differs substantially from the reviewed visual reference\n");
    }
    return matchesReference;
}

} // namespace

int main(int argc, char** ppArgs)
{
    if (argc < 2 || argc > 3)
    {
        std::fprintf(stderr, "Expected the rendered TIFF path and optional --print-reference\n");
        return 1;
    }

    const bool printReference = argc == 3;
    if (printReference && std::string_view(ppArgs[2]) != "--print-reference")
    {
        std::fprintf(stderr, "Unknown option: %s\n", ppArgs[2]);
        return 1;
    }

    try
    {
        const std::filesystem::path imagePath(ppArgs[1]);
        std::ifstream inputStream(imagePath, std::ios::binary);
        const auto fileBegin = std::istreambuf_iterator<char>(inputStream);
        const auto fileEnd = std::istreambuf_iterator<char>();
        const std::vector<U8> fileData(fileBegin, fileEnd);
        if (fileData.empty())
        {
            std::fprintf(stderr, "Killeroo visual regression image '%s' is missing or empty\n",
                         imagePath.string().c_str());
            return 2;
        }

        DUT::MemFile inputFile(fileData.data(), fileData.size());
        DIMG::Image image;
        DIMG::LoadTIFF(image, inputFile, imagePath.string().c_str());
        return validateKillerooRender(image, printReference) ? 0 : 3;
    }
    catch (const std::exception &exception)
    {
        std::fprintf(stderr, "Could not validate Killeroo render: %s\n", exception.what());
        return 4;
    }
}
