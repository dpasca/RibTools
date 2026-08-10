#include <cmath>
#include <limits>

#include "DImage/include/DImage.h"

namespace
{

bool testWrappedPixels()
{
    DIMG::Image image( 2, 2, 1, DIMG::Image::ST_U8, -1, "y" );
    image.GetPixelPtrRW( 0, 0 )[0] = 1;
    image.GetPixelPtrRW( 1, 0 )[0] = 2;
    image.GetPixelPtrRW( 0, 1 )[0] = 3;
    image.GetPixelPtrRW( 1, 1 )[0] = 4;

    U8 *pixels[4] {};
    image.GetPixelQuadWrapR( -1, -1, pixels );

    return
        pixels[0][0] == 4 &&
        pixels[1][0] == 3 &&
        pixels[2][0] == 2 &&
        pixels[3][0] == 1;
}

bool testU32Conversion()
{
    DIMG::Image image( 1, 1, 1, DIMG::Image::ST_U32, -1, "y" );
    const float input[DIMG::Image::MAX_SAMP_PER_PIX] { 1.0f };
    image.SetPixelFromFloat( 0, 0, input );

    const auto stored = *reinterpret_cast<const U32 *>(image.GetPixelPtrR( 0, 0 ));
    if ( stored != std::numeric_limits<U32>::max() )
        return false;

    float output[DIMG::Image::MAX_SAMP_PER_PIX] {};
    image.GetPixelAsFloat( 0, 0, output );
    return std::fabs( output[0] - 1.0f ) < 0.000001f;
}

} // namespace

int main()
{
    if ( !testWrappedPixels() )
        return 1;

    if ( !testU32Conversion() )
        return 2;

    return 0;
}
