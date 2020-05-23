//==================================================================
/// RRL_Translator.h
///
/// Created by Davide Pasca - 2009/12/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RRL_TRANSLATOR_H
#define RRL_TRANSLATOR_H

#include "DSystem/include/DContainers.h"
#include "RI_System/include/RI_Base.h"
#include "RI_System/include/RI_State.h"

//==================================================================
namespace RRL
{

//==================================================================
/// 
//==================================================================
class Translator
{
public:
    struct Params
    {
        RI::State::Params	mState;
        int					mForcedLongDim		;
        int					mForcedWd			;
        int					mForcedHe			;

        Params() :
            mForcedLongDim		(-1),
            mForcedWd			(-1),
            mForcedHe			(-1)
        {
        }
    };

private:
    RI::State	mState;
    Params		mParams;
    DStr		mReadArchivePathFName;

public:
    Translator( const Params &params );

    enum RetCmd
    {
        CMD_GENERIC = 0,
        CMD_READARCHIVE,
        CMD_WORLDEND
    };

    RetCmd AddCommand(
                const DStr		&cmdName,
                RI::ParamList	&cmdParams,
                const char		*pFileName,
                int				cmdLine );

    void ErrHandler( RI::Error errCode )
    {
        printf( "Error %s !!\n", ErrorToString( errCode ) );
    }

    RI::State &GetState()	{	return mState;	}

    const char *GetReadArchivePathFName() const { return mReadArchivePathFName.c_str(); }

private:
    void unknownCommand( const char *pCmdName );
    void exN( size_t n, const RI::ParamList &cmdParams );
    void geN( size_t n, const RI::ParamList &cmdParams );

    void addFormatCmd( RI::ParamList &p );

    bool addCommand_prims(
        const DStr		&nm,
        RI::ParamList	&p,
        const char		*pFileName,
        int				cmdLine );

    bool addCommand_options( const DStr &nm, RI::ParamList &p );
    bool addCommand_transforms( const DStr &nm, RI::ParamList &p );

    static RtToken matchToken( const char *pStr, RtToken pAllowedTokens[] );
};

//==================================================================
}

#endif
