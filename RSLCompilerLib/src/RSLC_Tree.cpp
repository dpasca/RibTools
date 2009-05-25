//==================================================================
/// RSLC_Tree.cpp
///
/// Created by Davide Pasca - 2009/5/24
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"

//==================================================================
namespace RSLC
{

//==================================================================
static TokNode *findPrev( TokNode *pNode )
{
	TokNode *pParent = pNode->mpParent;

	if NOT( pParent )
		return NULL;

	for (size_t i=1; i < pParent->mpChilds.size(); ++i)
	{
		if ( pParent->mpChilds[i] == pNode )
		{
			return pParent->mpChilds[i-1];
		}
	}

	return NULL;
}

//==================================================================
static void determineAreaType( TokNode *pNode )
{
	if ( pNode->mpToken )
	{
		if ( pNode->mpToken->id == T_OP_LFT_BRACKET )
		{
			pNode->mAreaType = AT_EXPRESSION;

			// are we at root level ?
			if ( pNode->mpParent && pNode->mpParent->mpParent == NULL )
			{
				TokNode	*pShName = findPrev( pNode );
				if ( pShName )
				{
					TokNode	*pShType = findPrev( pShName );
					if ( pShType && pShType->mpToken->idType == T_TYPE_SHADERTYPE )
					{
						pNode->mAreaType = AT_SHPARAMS;
					}
				}
			}
		}
		else
		if ( pNode->mpToken->id == T_OP_LFT_CRL_BRACKET )
		{
			pNode->mAreaType = AT_CODEBLOCK;
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		determineAreaType( pNode->mpChilds[i] );
	}
}

//==================================================================
TokNode *MakeTree( DVec<Token> &tokens )
{
	TokNode	*pRoot = DNEW TokNode( NULL );

	//pRoot->mAreaType = AT_ROOT;

	TokNode	*pCurNode = pRoot;

	for (size_t i=0; i < tokens.size(); ++i)
	{
		switch ( tokens[i].id )
		{
		case RSLC::T_OP_LFT_BRACKET		:
		case RSLC::T_OP_LFT_SQ_BRACKET	:
		case RSLC::T_OP_LFT_CRL_BRACKET	:
			pCurNode = pCurNode->AddNewChild( &tokens[i] );
			break;

		case RSLC::T_OP_RGT_BRACKET		:
		case RSLC::T_OP_RGT_SQ_BRACKET	:
		case RSLC::T_OP_RGT_CRL_BRACKET	:
			pCurNode = pCurNode->mpParent;
			pCurNode->AddNewChild( &tokens[i] );
			break;

		default:
			pCurNode->AddNewChild( &tokens[i] );
			break;
		}
	}

	determineAreaType( pRoot );

	return pRoot;
}

//==================================================================
void TraverseTree( TokNode *pNode, int depth )
{
	for (int i=0; i < depth; ++i)
	{
		putchar( '\t' );
	}

	if NOT( pNode->mpToken )
		printf( "ROOT" );
	else
		printf( "%s", pNode->mpToken->str.c_str() );

	switch ( pNode->mAreaType )
	{
	case AT_UNKNOWN:	break;
	case AT_SHPARAMS:	printf( " .. PARAMS " );	break;
	case AT_CODEBLOCK:	printf( " .. BLOCK " );		break;
	case AT_EXPRESSION:	printf( " .. EXPR " );		break;
	}

	const DVec<Variable> &vars = pNode->GetVars();

	if ( vars.size() )
	{
		printf( " .. vars: " );

		for (size_t i=0; i < vars.size(); ++i)
		{
			printf( " (%s, %s) %s,",
						vars[i].mpDetailTok ? 
							vars[i].mpDetailTok->str.c_str() :
							"N/A",
						vars[i].mpDTypeTok->str.c_str(),
						vars[i].mpNameTok ?
							vars[i].mpNameTok->str.c_str() :
							"NONAME"
					);
		}
	}

	printf( "\n" );

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		TraverseTree( pNode->mpChilds[i], depth+1 );
	}
}

//==================================================================
}
