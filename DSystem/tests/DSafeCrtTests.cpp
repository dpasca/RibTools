#include <cstring>
#include <stdexcept>

#include "DSystem/include/DSafeCrt.h"

int main()
{
    char formatted[16] {};
    if ( sprintf_s( formatted, "%s %i", "value", 7 ) != 7 )
        return 1;
    if ( std::strcmp( formatted, "value 7" ) != 0 )
        return 2;

    char copied[8] {};
    strcpy_s( copied, "abc" );
    strcat_s( copied, "def" );
    if ( std::strcmp( copied, "abcdef" ) != 0 )
        return 3;

#if !defined(_MSC_VER)
    char truncated[4] {};
    if ( sprintf_s( truncated, "%s", "1234" ) != -1 )
        return 4;
    if ( truncated[3] != 0 )
        return 5;

    try
    {
        char tooSmall[4] {};
        strcpy_s( tooSmall, "1234" );
        return 6;
    }
    catch ( const std::runtime_error & )
    {
    }
#endif

    return 0;
}
