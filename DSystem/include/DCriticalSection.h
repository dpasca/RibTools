//==================================================================
/// DCriticalSection.h
///
/// Created by Davide Pasca - 2009/8/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DCRITICALSECTION_H
#define DCRITICALSECTION_H

#include "DUtils.h"

//==================================================================
namespace DUT
{

//===============================================================
class CriticalSection
{
    friend class Block;

#if defined(WIN32)
    void				*mCSection;
#endif

public:
    //===============================================================
    class Block
    {
        CriticalSection *mpCSection;

    public:
        Block( CriticalSection &csection ) :
            mpCSection(&csection)
        {
            mpCSection->Enter();
        }

        ~Block()
        {
            mpCSection->Leave();
        }
    };

public:
    CriticalSection();
    ~CriticalSection();

    void Enter();
    void Leave();
};

//===============================================================
#if 1
class LongCriticalSection
{
    friend class Block;

    CriticalSection	mCS;
    volatile bool	mIsBlocked;

public:
    //===============================================================
    class Block
    {
        LongCriticalSection *mpLCS;

    public:
        Block( LongCriticalSection &lcs ) :
            mpLCS(&lcs)
        {
            mpLCS->mCS.Enter();
            while ( mpLCS->mIsBlocked )
            {
                mpLCS->mCS.Leave();
                DUT::SleepMS( 1 );
                mpLCS->mCS.Enter();
            }
            mpLCS->mIsBlocked = true;
            mpLCS->mCS.Leave();
        }

        ~Block()
        {
            mpLCS->mCS.Enter();
            DASSERT( mpLCS->mIsBlocked == true );
            mpLCS->mIsBlocked = false;
            mpLCS->mCS.Leave();
        }
    };

    LongCriticalSection() :
        mIsBlocked(false)
    {
    }
};
#endif

//==================================================================
}

#endif

