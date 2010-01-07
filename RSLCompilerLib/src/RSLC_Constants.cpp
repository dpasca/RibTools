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
	DASSERT( pNode->mpToken->idType == T_TYPE_VALUE );

	VarType		vtype;
	float		floatVal = 0;
	const char	*pStrVal = NULL;

	DVec<Variable> &vars = pRoot->GetVars();

	// see if it matches and existing constant..

	if ( pNode->mpToken->id == T_VL_NUMBER )
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
	if ( pNode->mpToken->id == T_VL_STRING )
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
	if ( pNode->mpToken->id == T_VL_BOOL_TRUE ||
		 pNode->mpToken->id == T_VL_BOOL_FALSE )
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

	Variable	*pVar = pRoot->GetVars().grow();

	pVar->mIsVarying		= false;
	pVar->mIsForcedDetail	= false;

	pVar->mInternalName		= DUT::SSPrintFS( "_@K%02i", pNode->mVarLink.GetVarIdx() );

	pVar->mVarType			= vtype;

	if ( pNode->mpToken->id == T_VL_NUMBER )
	{
		pVar->mBaseVal.Set( floatVal );
	}
	else
	if ( pNode->mpToken->id == T_VL_STRING )
	{
		pVar->mBaseVal.Set( pStrVal );
	}
	else
	if ( pNode->mpToken->id == T_VL_BOOL_TRUE ||
		 pNode->mpToken->id == T_VL_BOOL_FALSE )
	{
		pVar->mBaseVal.Set( pNode->mpToken->id == T_VL_BOOL_TRUE );
	}
}

//==================================================================
static void realizeConstants_rec( TokNode *pNode, TokNode *pRoot )
{
	if ( pNode->mpToken && pNode->mpToken->idType == T_TYPE_VALUE )
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
#if 0
	const DVec<Function> &funcs = pRoot->GetFuncs();

	for (size_t i=0; i < funcs.size(); ++i)
	{
		const Function	&func = funcs[i];

/*
		if NOT( func.IsShader() )
			continue;
*/

		if ( func.mpParamsNode )
		{
			for (size_t j=0; j < func.mpParamsNode->mpChilds.size(); ++j)
			{
				TokNode	*pParamNode = func.mpParamsNode->mpChilds[j];
				realizeConstants_rec( pParamNode, pRoot );
			}
		}

		for (size_t j=0; j < func.mpCodeBlkNode->mpChilds.size(); ++j)
		{
			TokNode	*pNode = func.mpCodeBlkNode->mpChilds[j];

			realizeConstants_rec( pNode, pRoot );
		}
	}

#else

	realizeConstants_rec( pRoot, pRoot );

#endif
}

//==================================================================
}
