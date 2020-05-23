//==================================================================
/// RSLC_VarLink.cpp
///
/// Created by Davide Pasca - 2009/10/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_VarLink.h"

//==================================================================
namespace RSLC
{

//==================================================================
VarLink::VarLink() :
    mpNode(NULL),
    mVarIdx(DNPOS)
{
}

//==================================================================
VarLink::VarLink( const VarLink &from )
{
    mpNode	= from.mpNode;
    mVarIdx	= from.mVarIdx;
    addRef();
}

//==================================================================
VarLink &VarLink::operator=( const VarLink &from )
{
    DASSERT( !from.mpNode || from.mpNode->mBlockID != 0xfeeefeee );

    subRef();
    mpNode	= from.mpNode;
    mVarIdx	= from.mVarIdx;
    addRef();

    return *this;
}

//==================================================================
VarLink::~VarLink()
{
    subRef();
}

//==================================================================
void VarLink::addRef()
{
#ifdef _DEBUG
    if ( mpNode )
    {
        // umm disabling for now...
        //mpNode->mpReferringVarLinks.find_or_push_back( this );
    }
#endif
}

//==================================================================
void VarLink::subRef()
{
#ifdef _DEBUG
    if ( mpNode )
    {
        // umm disabling for now...
        //DVec<VarLink*>::iterator it = mpNode->mpReferringVarLinks.find( this );
        //DASSERT( it != mpNode->mpReferringVarLinks.end() );
        //mpNode->mpReferringVarLinks.erase( it );
    }
#endif
}

//==================================================================
void VarLink::Setup( TokNode *pNode, size_t idx )
{
    mpNode = pNode;
    mVarIdx = idx;

    addRef();
}

//==================================================================
void VarLink::ReplaceNode( TokNode *pNewNode )
{
    subRef();

    mpNode = pNewNode;

    addRef();
}

//==================================================================
Variable * VarLink::GetVarPtr()
{
    return mVarIdx != DNPOS ? &mpNode->GetVars()[mVarIdx] : NULL;
}

//==================================================================
const Variable * VarLink::GetVarPtr() const
{
    return mVarIdx != DNPOS ? &mpNode->GetVars()[mVarIdx] : NULL;
}

//==================================================================
#ifdef _DEBUG
bool VarLink::IsNodeValid() const
{
    return mpNode == 0 || mpNode->mBlockID != 0xfeeefeee;
}
#endif

//==================================================================
}
