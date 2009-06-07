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
TokNode *TokNode::GetLeft()
{
	if NOT( mpParent )
		return NULL;

	for (size_t i=0; i < mpParent->mpChilds.size(); ++i)
	{
		if ( mpParent->mpChilds[i] == this )
		{
			if ( i > 0 )
				return mpParent->mpChilds[i-1];
			else
				return NULL;
		}
	}

	DASSTHROW( 0, ("Bad tree ?!") );
	return NULL;
}

//==================================================================
TokNode *TokNode::GetRight()
{
	if NOT( mpParent )
		return NULL;

	for (size_t i=0; i < mpParent->mpChilds.size(); ++i)
	{
		if ( mpParent->mpChilds[i] == this )
		{
			if ( (i+1) < mpParent->mpChilds.size() )
				return mpParent->mpChilds[i+1];
			else
				return NULL;
		}
	}

	DASSTHROW( 0, ("Bad tree ?!") );
	return NULL;
}

//==================================================================
TokNode *TokNode::GetPrev()
{
	if NOT( mpParent )
		return NULL;

	for (size_t i=0; i < mpParent->mpChilds.size(); ++i)
	{
		if ( mpParent->mpChilds[i] == this )
		{
			// return the sibling on the left if exists...
			if ( i > 0 )
				return mpParent->mpChilds[i-1];
			else	// ..otherwise return the parent
				return mpParent;
		}
	}

	DASSTHROW( 0, ("Bad tree ?!") );
	return NULL;
}

//==================================================================
TokNode *TokNode::GetNext()
{
	if NOT( mpParent )
	{
		if ( mpChilds.size() && mpChilds[0] )
			return mpChilds[0];
		else
			return NULL;
	}

	for (size_t i=0; i < mpParent->mpChilds.size(); ++i)
	{
		if ( mpParent->mpChilds[i] == this )
		{
			// return a sibling if any...
			if ( (i+1) < mpParent->mpChilds.size() )
				return mpParent->mpChilds[i+1];
			else	// ..return the first child if exists..
			if ( mpChilds.size() && mpChilds[0] )
				return mpChilds[0];
			else	// otherwise just give up (it's a leaf)
				return NULL;
		}
	}

	DASSTHROW( 0, ("Bad tree ?!") );
	return NULL;
}

//==================================================================
Variable *TokNode::FindVariableByDefName( const char *pName )
{
	for (size_t i=0; i < mVariables.size(); ++i)
	{
		Variable	*pVar = &mVariables[i];
		if ( pVar->HasDefName() )
			if ( 0 == strcmp( pVar->GetDefName(), pName ) )
			{
				return pVar;
			}
	}

	return NULL;
}

//==================================================================
static void defineBlockTypeAndID( TokNode *pNode, u_int &blockCnt )
{
	if ( pNode->mpToken )
	{
		if ( pNode->mpToken->id == T_OP_LFT_BRACKET )
		{
			pNode->mBlockType = BLKT_EXPRESSION;

			// are we at root level ?
			if ( pNode->mpParent && pNode->mpParent->mpParent == NULL )
			{
				TokNode	*pFnName = pNode->GetLeft();
				if ( pFnName )
				{
					TokNode	*pFnType = pFnName->GetLeft();
					if ( pFnType )
					{
						// shader
						if ( pFnType->mpToken->idType == T_TYPE_SHADERTYPE )
							pNode->mBlockType = BLKT_SHPARAMS;
						else	// function
						if ( pFnType->mpToken->idType == T_TYPE_DATATYPE )
							pNode->mBlockType = BLKT_SHPARAMS;
						else
						{
							DASSERT( 0 );
						}
					}
				}
			}
		}
		else
		if ( pNode->mpToken->id == T_OP_LFT_CRL_BRACKET )
		{
			pNode->mBlockType = BLKT_CODEBLOCK;
		}

		// are we in a block ?
		if ( pNode->mBlockType != BLKT_UNKNOWN )
		{
			// assign the ID and increment the counter
			pNode->mBlockID = blockCnt++;
		}
	}
	else
	{
		// block for root
		pNode->mBlockType = BLKT_ROOT;
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		defineBlockTypeAndID( pNode->mpChilds[i], blockCnt );
	}
}

//==================================================================
TokNode *MakeTree( DVec<Token> &tokens )
{
	TokNode	*pRoot = DNEW TokNode( NULL );

	//pRoot->mBlockType = AT_ROOT;

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

	u_int	blockCnt = 0;
	defineBlockTypeAndID( pRoot, blockCnt );

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
	{
		printf( "%s", pNode->mpToken->str.c_str() );
		
		if ( pNode->mpVarDef )
		{
			printf( " <<VARIABLE: %s>>", pNode->mpVarDef->mInternalName.c_str() );
		}
	}

	switch ( pNode->mBlockType )
	{
	case BLKT_UNKNOWN:		break;
	case BLKT_ROOT:			printf( " <ROOT %i> ", pNode->mBlockID );	break;
	case BLKT_SHPARAMS:		printf( " <PARAMS %i> ", pNode->mBlockID );	break;
	case BLKT_CODEBLOCK:	printf( " <CODEBLK %i> ", pNode->mBlockID );	break;
	case BLKT_EXPRESSION:	printf( " <EXPR %i> ", pNode->mBlockID );		break;
	}

	const DVec<Variable> &vars = pNode->GetVars();

	if ( vars.size() )
	{
		printf( " <<VAR_DEFS: " );

		for (size_t i=0; i < vars.size(); ++i)
		{
			printf( " %s %s %s,",
						vars[i].mpDetailTok ? 
							vars[i].mpDetailTok->str.c_str() :
							"",
/*
						vars[i].mIsVarying ? 
							"varying" :
							"uniform",
*/
						vars[i].mpDTypeTok->str.c_str(),
						vars[i].mpDefNameTok ?
							vars[i].mpDefNameTok->str.c_str() :
							"NONAME"
					);
		}

		printf( ">>" );
	}

	printf( "\n" );

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		TraverseTree( pNode->mpChilds[i], depth+1 );
	}
}

//==================================================================
}
