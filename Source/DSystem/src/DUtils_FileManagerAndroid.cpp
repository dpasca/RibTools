//==================================================================
/// DUtils_FileManagerAndroid.cpp
///
/// Created by Davide Pasca - 2012/2/13
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#define D_LOGGINGx
#define D_TRACKINGx

#include "dlog.h"
#if defined(ANDROID)
# include "platform.h"
#endif
#include "DUtils_FileManagerAndroid.h"

#if defined(ANDROID)
# include "native.h"
#endif

//==================================================================
namespace DUT
{

typedef unsigned char uint8_t;

//==================================================================

#if defined(ANDROID)
static bool isPrefsMode( const char *pMode )
{
	return (pMode != NULL && (strchr( pMode, 'p' ) || strchr( pMode, 'P' )));
}
#endif

//==================================================================
bool
FileManagerAndroid::FileExists(const char* pFileName, const char* pMode) const
{
#if defined(ANDROID)
	const bool prefs = isPrefsMode( pMode );

    if (prefs)
    {
        // TODO: This is terrible...  We end up loading the whole
        // file, but pref files should be small anyway.

        DVec<U8> data;
        return const_cast<FileManagerAndroid *>(this)->
            GrabFile(pFileName, data, pMode);
    }
    else
    {
        return ::FileExists(pFileName);
    }

#else

	return false;

#endif
}

//==================================================================
bool FileManagerAndroid::GrabFile(const char* pFileName, DVec<U8> &out_data, const char* pMode)
{
    DFUNCTION();
    DLOG("File: %s", pFileName);

#if defined(ANDROID)
	bool prefs = isPrefsMode( pMode );

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
            DPRINT("!! File '%s' was not base64 format.  Rejecting.", pFileName);
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
#endif

	return true;
}

//==================================================================
bool FileManagerAndroid::SaveFile(const char* pFileName, const U8 *pInData, size_t dataSize, const char* pMode)
{
    DFUNCTION();
    DLOG("Saving file: %s", pFileName);

#if defined(ANDROID)
	bool prefs = isPrefsMode( pMode );

    bool result = false;

    if (prefs)
    {
        char *base64 = ToBase64(pInData, dataSize);
        if (0 == base64)
        {
            DLOG("Failed to convert to base64");
            return false;
        }

        DLOG("Data: %s", pInData);
        DLOG("Base64: %s", base64);
        result = ::DoWritePrefs(pFileName, base64);
        free(base64);
    }
    else
    {
        result = FileWrite(pFileName, pInData, dataSize);
    }

    DLOG("Returning: %s", (result)?("true"):("false"));
    return result;
#endif

	return true;
}

//==================================================================
}
