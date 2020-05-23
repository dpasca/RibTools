//==================================================================
/// RSLC_Constants.cpp
///
/// Created by Davide Pasca - 2009/10/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Constants.h"

//==================================================================
namespace RSLC
{

//==================================================================
static void addConstVariable( TokNode *pNode, TokNode *pRoot )
{
    DASSERT( pNode->GetTokIDType() == T_TYPE_VALUE );

    VarType		vtype;
    float		floatVal = 0;
    const char	*pStrVal = NULL;

    DVec<Variable> &vars = pRoot->GetVars();

    // see if it matches and existing constant..

    if ( pNode->GetTokID() == T_VL_NUMBER )
    {
        vtype = VT_FLOAT;
        floatVal = (float)atof( pNode->GetTokStr() );

        for (size_t i=0; i < vars.size(); ++i)
        {
            if NOT( vars[i].HasBaseVal( VT_FLOAT ) )
                continue;

            if ( vars[i].GetBaseValFloat() == floatVal )
            {
                pNode->mVarLink.Setup( pRoot, i );
                return;
            }
        }
    }
    else
    if ( pNode->GetTokID() == T_VL_STRING )
    {
        vtype = VT_STRING;
        pStrVal = pNode->GetTokStr();

        for (size_t i=0; i < vars.size(); ++i)
        {
            if NOT( vars[i].HasBaseVal( VT_STRING ) )
                continue;

            if ( 0 == strcmp( vars[i].GetBaseValString(), pStrVal ) )
            {
                pNode->mVarLink.Setup( pRoot, i );
                return;
            }
        }
    }
    else
    if ( pNode->GetTokID() == T_VL_BOOL_TRUE ||
         pNode->GetTokID() == T_VL_BOOL_FALSE )
    {
        //DASSERT( 0 );
        vtype = VT_BOOL;
    }
    else
    {
        DASSERT( 0 );
        return;
    }

    // ..if not found, then need to make it from scratch

    // setup the var link
    pNode->mVarLink.Setup( pRoot, pRoot->GetVars().size() );

    Variable	*pVar = &Dgrow( pRoot->GetVars() );

    pVar->mIsVarying		= false;
    pVar->mIsForcedDetail	= false;

    pVar->mInternalName		= DUT::SSPrintFS( "_@K%02i", pNode->mVarLink.GetVarIdx() );

    pVar->mVarType			= vtype;

    if ( pNode->GetTokID() == T_VL_NUMBER )
    {
        pVar->mBaseVal.Set( floatVal );
    }
    else
    if ( pNode->GetTokID() == T_VL_STRING )
    {
        pVar->mBaseVal.Set( pStrVal );
    }
    else
    if ( pNode->GetTokID() == T_VL_BOOL_TRUE ||
         pNode->GetTokID() == T_VL_BOOL_FALSE )
    {
        pVar->mBaseVal.Set( pNode->GetTokID() == T_VL_BOOL_TRUE );
    }
}

//==================================================================
static void realizeConstants_rec( TokNode *pNode, TokNode *pRoot )
{
    if ( pNode->mpToken && pNode->GetTokIDType() == T_TYPE_VALUE )
    {
        addConstVariable( pNode, pRoot );
    }

    for (size_t i=0; i < pNode->mpChilds.size(); ++i)
    {
        realizeConstants_rec( pNode->mpChilds[i], pRoot );
    }
}

//==================================================================
/// RealizeConstants
//==================================================================
void RealizeConstants( TokNode *pRoot )
{
    realizeConstants_rec( pRoot, pRoot );
}

//==================================================================
}
