//==================================================================
/// DUtils_Files.cpp
///
/// Created by Davide Pasca - 2009/7/23
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

//#include "dlog.h"
#define DFUNCTION()	// temporary
#define DLOG		// temporary

#include "DUtils_Files.h"

#if defined(ANDROID)
# include "native.h"
#endif

//==================================================================
namespace DUT
{

typedef unsigned char uint8_t;

static const char encode_base64[]=
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static uint8_t decode_base64(char b64val)
{
    if (b64val == '/')
    {
        return 63;
    }
    if (b64val == '+')
    {
        return 62;
    }
    if ((b64val >= 'A') && (b64val <= 'Z'))
    {
        return (uint8_t )(b64val - 'A');
    }
    if ((b64val >= 'a') && (b64val <= 'z'))
    {
        return (uint8_t )(26 + (b64val - 'a'));
    }
    if ((b64val >= '0') && (b64val <= '9'))
    {
        return (uint8_t )(52 + (b64val - '0'));
    }
    return 0xffu;
}

static char *ToBase64(const void *data, size_t len)
{
    // Calculate length.  3 bytes -> 4 bytes.
    // Remainder:         0  1  2
    // Extra b64 bytes:   0  2  3

    size_t numBlocks = len / 3;
    size_t remainder = len - (numBlocks*3);
    size_t extrab64bytes = 0;
    switch (remainder)
    {
    case 1:
        extrab64bytes = 2;
        break;
    case 2:
        extrab64bytes = 3;
        break;
    default:
        break;
    }
    size_t outLength = (numBlocks * 4) + (extrab64bytes);

    DLOG("incoming data has %d bytes.  b64 data has %d bytes (%d blocks, "
         "%d extra)", len, outLength, numBlocks, extrab64bytes);

    // Data pointers

    const uint8_t *dataBytes = (const uint8_t *)data;
    char *outBytes = (char *)malloc(outLength+1);

    // Return values

    char * const retVal = outBytes;

    // Blocks

    for (size_t blockIdx = 0 ; blockIdx < numBlocks ; ++blockIdx)
    {
        uint8_t a = dataBytes[0];
        uint8_t b = dataBytes[1];
        uint8_t c = dataBytes[2];
        dataBytes += 3;

        uint8_t outA = a >> 2;
        uint8_t outB = ((a & 0x3) << 4) | (b >> 4);
        uint8_t outC = ((b & 0xf) << 2) | (c >> 6);
        uint8_t outD = c & 0x3f;

        DASSERT(outA < 64);
        DASSERT(outB < 64);
        DASSERT(outC < 64);
        DASSERT(outD < 64);

        outBytes[0] = encode_base64[outA];
        outBytes[1] = encode_base64[outB];
        outBytes[2] = encode_base64[outC];
        outBytes[3] = encode_base64[outD];
        outBytes += 4;
    }

    // Extras

    switch (remainder)
    {
    case 1:
        {
            uint8_t a = dataBytes[0];
            dataBytes += 1;

            uint8_t outA = a >> 2;
            uint8_t outB = ((a & 0x3) << 4);

            outBytes[0] = encode_base64[outA];
            outBytes[1] = encode_base64[outB];
            outBytes += 2;
        }
        break;

    case 2:
        {
            uint8_t a = dataBytes[0];
            uint8_t b = dataBytes[1];
            dataBytes += 2;

            uint8_t outA = a >> 2;
            uint8_t outB = ((a & 0x3) << 4) | (b >> 4);
            uint8_t outC = ((b & 0xf) << 2);

            outBytes[0] = encode_base64[outA];
            outBytes[1] = encode_base64[outB];
            outBytes[2] = encode_base64[outC];
            outBytes += 3;
        }

    default:
        break;
    }

    DASSERT((size_t )(outBytes - retVal) == outLength);
    *outBytes = '\0';

    return retVal;
}

static void *FromBase64(const char * base64, size_t *out_length)
{
    DFUNCTION();

    size_t base64Length = strlen(base64);
    if (1 == (base64Length % 4))
    {
        DLOG("Got invalid string length %d", base64Length);
        DLOG("String is '%s'", base64);
        return 0;
    }

    size_t numBlocks = base64Length / 4;
    size_t remainders = base64Length - (numBlocks * 4);
    DASSERT(remainders != 1);

    size_t extraBytes = 0;
    switch(remainders)
    {
    case 2:
        extraBytes = 1;
        break;
    case 3:
        extraBytes = 2;
        break;
    default:
        break;
    }

    // Setup memory

    size_t numOutBytes = (numBlocks * 3) + extraBytes;
    uint8_t *outBytes = (uint8_t *)malloc(numOutBytes);

    uint8_t * const retVal = outBytes;
    *out_length = numOutBytes;

    // Blocks

    for (size_t blockIdx = 0 ; blockIdx < numBlocks ; ++blockIdx)
    {
        uint8_t a = decode_base64(base64[0]);
        uint8_t b = decode_base64(base64[1]);
        uint8_t c = decode_base64(base64[2]);
        uint8_t d = decode_base64(base64[3]);
        if (a > 63 || b > 63 || c > 63 || d > 63)
        {
            DLOG("Failed to decode one of %c%c%c%c", base64[0], base64[1],
                 base64[2], base64[3]);
            free(retVal);
            return 0;
        }
        base64 += 4;


        outBytes[0] = (a << 2) | (b >> 4);
        outBytes[1] = (b << 4) | (c >> 2);
        outBytes[2] = (c << 6) | d;
        outBytes += 3;
    }

    // Extras

    switch (remainders)
    {
    case 2:
        {
            // Just 1 byte
            uint8_t a = decode_base64(base64[0]);
            uint8_t b = decode_base64(base64[1]);
            if (a > 63 || b > 63)
            {
                DLOG("Failed to decode one of %c%c", base64[0], base64[1]);
                free(retVal);
                return 0;
            }
            base64 += 2;

            outBytes[0] = (a << 2) | (b >> 4);
            outBytes += 1;
        }
        break;

    case 3:
        {
            // 2 more output bytes
            uint8_t a = decode_base64(base64[0]);
            uint8_t b = decode_base64(base64[1]);
            uint8_t c = decode_base64(base64[2]);
            if (a > 63 || b > 63 || c > 63)
            {
                DLOG("Failed to decode one of %c%c%c", base64[0], base64[1],
                     base64[2]);
                free(retVal);
                return 0;
            }
            base64 += 3;

            outBytes[0] = (a << 2) | (b >> 4);
            outBytes[1] = (b << 4) | (c >> 2);
            outBytes += 2;
        }
        break;

    default:
        break;
    }

    DASSERT((size_t )(outBytes - retVal) == numOutBytes);

    return retVal;
}

//==================================================================
bool GrabFile( const char *pFileName, DVec<U8> &out_data, bool prefs )
{
    DFUNCTION();
    DLOG("File: %s", pFileName);

#if defined(ANDROID)
    if (prefs)
    {
        // Read from the prefs storage

        DLOG("Grabbing file (prefs): %s", pFileName);
        char *contents = ::DoReadPrefs(pFileName);
        if (0 == contents)
        {
            DLOG("Failed to open the file");
            return false;
        }

        // Decode into binary

        size_t binLength = 0;
        void *binData = FromBase64(contents, &binLength);
        free(contents);
        if (binData == 0)
        {
            DLOG("File was not base64 format.  Rejecting it.");
            return false;
        }

        // Copy into output array and free original

        // TODO: Could be sped up by pre-calculating length and not
        // copying.

        out_data.resize(binLength);
        memcpy(&out_data[0], binData, binLength);
        free(binData);

        DLOG("copied %d bytes", binLength);
        return true;
    }

    ::FileLoad(pFileName);
    int datasize;
    void *data;
    if (!FileGetData(pFileName, &data, &datasize))
    {
        //DASSTHROW(0, ("failed loading file '%s'", pFileName));
        return false;
    }

    out_data.resize(datasize);

    DVERBOSE("GrabFile: memcpy-ing '%s', %d bytes", pFileName, datasize);
    memcpy(&out_data[0], data, datasize);

    FileRemove(pFileName);
    DVERBOSE("GrabFile: released");
#else

    (void )prefs;
	out_data.clear();

	FILE	*pFile = fopen( pFileName, "rb" );
	if NOT( pFile )
	{
		return false;
	}

	fseek( pFile, 0, SEEK_END );
	out_data.resize( ftell( pFile ) );
	fseek( pFile, 0, SEEK_SET );

	if ( out_data.size() )
		fread( &out_data[0], 1, out_data.size(), pFile );

	fclose( pFile );
#endif

	return true;
}

//==================================================================
bool SaveFile( const char *pFileName, const U8 *pInData, size_t dataSize,
               bool prefs)
{
    DFUNCTION();
    DLOG("Saving file: %s", pFileName);

#if defined(ANDROID)
    bool result = false;

    if (prefs)
    {
        char *base64 = ToBase64(pInData, dataSize);
        if (0 == base64)
        {
            return false;
        }

        result = ::DoWritePrefs(pFileName, base64);
        free(base64);
    }
    else
    {
        result = FileWrite(pFileName, pInData, dataSize);
    }

    DLOG("Returning: %s", (result)?("true"):("false"));
    return result;
#else

    (void )prefs;
	FILE	*pFile = fopen( pFileName, "wb" );
	if NOT( pFile )
	{
		return false;
	}

	if ( dataSize != fwrite( pInData, 1, dataSize, pFile ) )
	{
		fclose( pFile );
		return false;
	}

	fclose( pFile );
#endif

	return true;
}

//==================================================================
FILE *BeginGrabFile( const char *pFileName, size_t &out_fileSize )
{
	out_fileSize = 0;

	FILE	*pFile = fopen( pFileName, "rb" );
	if NOT( pFile )
		return NULL;

	fseek( pFile, 0, SEEK_END );
	out_fileSize = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

	return pFile;
}

//==================================================================
bool EndGrabFile( FILE *pFile, void *pDest, size_t readSize )
{
	if ( readSize != fread( pDest, 1, readSize, pFile ) )
	{
		fclose( pFile );
		return false;
	}

	fclose( pFile );
	return true;
}

//==================================================================
bool FileExists( const char *pFileName, bool prefs )
{
#if defined(ANDROID)

    // TODO: This is terrible...  We end up loading the whole file,
    // but pref files should be small anyway.

    DASSTHROW(prefs, "FileExists() only supported for preference files");

    DVec<U8> data;
    return GrabFile(pFileName, data, prefs);

#else

    (void )prefs;
	FILE	*pFile = fopen( pFileName, "rb" );

	if ( pFile )
	{
		fclose( pFile );
		return true;
	}
	else
	{
		return false;
	}

#endif
}

//==================================================================
const char *GetFileNameOnly( const char *pPathFileName )
{
	int	len = strlen( pPathFileName );

	for (int i=len-1; i >= 0; --i)
		if ( pPathFileName[i] == '/' || pPathFileName[i] == '\\' )
			return pPathFileName + i + 1;

	return pPathFileName + len;
}

//==================================================================
const char *GetFileNameExt( const char *pPathFileName )
{
	int	len = strlen( pPathFileName );

	for (int i=len-1; i >= 0; --i)
		if ( pPathFileName[i] == '.' )
			return pPathFileName + i + 1;

	return pPathFileName + len;
}

//==================================================================
char *GetFileNameExt( char *pPathFileName )
{
	return (char *)GetFileNameExt( (const char *)pPathFileName );
}

//==================================================================
DStr GetDirNameFromFPathName( const char *pInFPathname )
{
	const char *pInFPathnameEnd = pInFPathname + strlen( pInFPathname );

	const char *pFNamePtr = GetFileNameOnly( pInFPathname );

	if ( pFNamePtr >= pInFPathnameEnd )
		return DStr();	// and empty string after all..
	else
	{
		size_t	len = pFNamePtr - pInFPathname - 1;

		DStr	tmp( pInFPathname );
		tmp.resize( len );

		return tmp;
	}
}

//==================================================================
// Not really needed for now by RibTools
#if 0
DStr GetFullDirNameFromFPathName( const char *pInFPathname )
{
#if 0
	return GetDirNameFromFPathName( pInFPathname );

#else
	DStr	tmp = GetDirNameFromFPathName( pInFPathname );

	char	buff[2048];

#if defined(WIN32)
	GetFullPathNameA( tmp.c_str(), _countof(buff), buff, NULL );

#elif defined(__linux__) || defined(__APPLE__)
	buff[0] = 0;
	DASSERT( 0 );	// TODO

#endif

	return buff;
#endif
}
#endif

//==================================================================
}
