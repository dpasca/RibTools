#include <cmath>
#include <cstdlib>

#include "DSystem/include/DUtils_MemFile.h"
#include "DSystem/include/DUtils_Files.h"
#include "RI_System/include/RI_Texture.h"

namespace
{

bool samplesAreValid(const Float_ &samples)
{
    for (size_t sampleIndex = 0; sampleIndex < DMT_SIMD_FLEN; ++sampleIndex)
    {
        if (!std::isfinite(samples[sampleIndex]) || samples[sampleIndex] < 0.0f || samples[sampleIndex] > 1.0f)
        {
            return false;
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
    DUT::MemFile textureFile(argv[1]);
    RI::Texture texture(argv[1], textureFile);

    Float_ nearestSamples;
    texture.Sample_1_1x1(nearestSamples, Float_(-0.25f), Float_(1.25f));
    if (!samplesAreValid(nearestSamples))
    {
        return EXIT_FAILURE;
    }

    Float_ filteredSamples;
    texture.Sample_1_filter(filteredSamples, Float_(0.25f), Float_(0.75f));
    return samplesAreValid(filteredSamples) ? EXIT_SUCCESS : EXIT_FAILURE;
}
