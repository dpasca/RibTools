//==================================================================
/// RSLC_Tree.cpp
///
/// Created by Davide Pasca - 2009/5/24
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Registers.h"

//==================================================================
namespace RSLC
{

//==================================================================
#ifdef _DEBUG
size_t	TokNode::sUIDCnt;
#endif

//==================================================================
TokNode::TokNode( Token *pObj ) :
	mpToken(pObj),
	mpParent(NULL),
	mNodeType(TYPE_STANDARD),
	mBlockType(BLKT_UNKNOWN),
	mBlockID(0)
{
#ifdef _DEBUG
	mUIDCnt = sUIDCnt++;
#endif
}

//==================================================================
TokNode::TokNode( const TokNode &from ) :
	mpToken			(from.mpToken),
	mVariables		(from.mVariables),
	mFunctions		(from.mFunctions),
	mpParent		(NULL),
	mNodeType		(from.mNodeType),
	mBlockType		(from.mBlockType),
	mBlockID		(0),
	mVarLink		(from.mVarLink)
{
#ifdef _DEBUG
	mUIDCnt = sUIDCnt++;
#endif
}

//==================================================================
TokNode::~TokNode()
{
	for (size_t i=0; i < mpChilds.size(); ++i)
		DSAFE_DELETE( mpChilds[i] );

#ifdef _DEBUG
	for (size_t i=0; i < mpReferringVarLinks.size(); ++i)
	{
		if ( mpReferringVarLinks[i]->GetNode() != this )
		{
			int yoyo = 1;
		}
	}
#endif
}

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
VarLink TokNode::FindVariableByDefName( const char *pName )
{
	VarLink	result;

	for (size_t i=0; i < mVariables.size(); ++i)
	{
		const Variable	*pVar = &mVariables[i];
		if ( pVar->HasDefName() )
			if ( 0 == strcmp( pVar->GetDefName(), pName ) )
			{
				result.Setup( this, i );
				break;
			}
	}

	return result;
}

//==================================================================
void TokNode::UnlinkFromParent()
{
	if NOT( mpParent )
		return;

	for (size_t i=0; i < mpParent->mpChilds.size(); ++i)
	{
		if ( mpParent->mpChilds[i] == this )
		{
			mpParent->mpChilds.erase( mpParent->mpChilds.begin()+i );
			return;
		}
	}

	DASSERT( 0 );
}

//==================================================================
void TokNode::ReplaceNode( TokNode *pNode )
{
	if NOT( pNode->mpParent )
		return;

	for (size_t i=0; i < pNode->mpParent->mpChilds.size(); ++i)
	{
		if ( pNode->mpParent->mpChilds[i] == pNode )
		{
			pNode->mpParent->mpChilds[i] = this;
			mpParent = pNode->mpParent;
			pNode->mpParent = NULL;
			return;
		}
	}

	DASSERT( 0 );
}

//==================================================================
RSLC::Register TokNode::GetRegister() const
{
	if ( mVarLink.IsValid() )
	{
		return mVarLink.GetVarPtr()->mBuild_Register;
	}
	else
		return Register();
}

//==================================================================
RSLC::VarType TokNode::GetVarType() const
{
	if ( mpToken->idType == T_TYPE_VALUE )
	{
		if ( mpToken->id == T_VL_NUMBER )
			return VT_FLOAT;
		else
		if ( mpToken->id == T_VL_STRING )
			return VT_STRING;

		DASSERT( 0 );
		return VT_UNKNOWN;
	}
	else
	if ( mVarLink.IsValid() )
		return mVarLink.GetVarPtr()->GetVarType();
	else
	{
		DASSERT( 0 );
		return VT_UNKNOWN;
	}
}

//==================================================================
bool TokNode::IsVarying() const
{
	if ( mpToken->idType == T_TYPE_VALUE )
		return false;
	else
	if ( mVarLink.IsValid() )
		return mVarLink.GetVarPtr()->IsVarying();
	else
	{
		DASSERT( 0 );
		return false;
	}
}

//==================================================================
static void defineBlockTypeAndID( TokNode *pNode, u_int &blockCnt )
{
	if ( pNode->mpToken )
	{
		if ( pNode->mpToken->id == T_OP_LFT_BRACKET )
		{
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
							pNode->SetBlockType( BLKT_SHPARAMS );
						else	// function
						if ( pFnType->mpToken->idType == T_TYPE_DATATYPE )
							pNode->SetBlockType( BLKT_FNPARAMS );
						else
						{
							throw Exception( "Return type unknown", pNode );
						}
					}
				}
			}

			if ( pNode->GetBlockType() == BLKT_UNKNOWN )
			{
				// set as a plain expression otherwise
				pNode->SetBlockType( BLKT_EXPRESSION );
			}
		}
		else
		if ( pNode->mpToken->id == T_OP_LFT_CRL_BRACKET )
		{
			pNode->SetBlockType( BLKT_CODEBLOCK );
		}

		// are we in a block ?
		if ( pNode->GetBlockType() != BLKT_UNKNOWN )
		{
			// assign the ID and increment the counter
			pNode->mBlockID = blockCnt++;
		}
	}
	else
	{
		// block for root
		pNode->SetBlockType( BLKT_ROOT );
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		defineBlockTypeAndID( pNode->mpChilds[i], blockCnt );
	}
}

//==================================================================
static bool doBracketsMatch( TokenID lastOpenBraket, TokenID thisCloseBracket )
{
	if ((lastOpenBraket == RSLC::T_OP_LFT_BRACKET && thisCloseBracket == RSLC::T_OP_RGT_BRACKET) ||
		(lastOpenBraket == RSLC::T_OP_LFT_SQ_BRACKET && thisCloseBracket == RSLC::T_OP_RGT_SQ_BRACKET) ||
		(lastOpenBraket == RSLC::T_OP_LFT_CRL_BRACKET && thisCloseBracket == RSLC::T_OP_RGT_CRL_BRACKET) )
		return true;
	else
		return false;
}

//==================================================================
void MakeTree( TokNode *pRoot, DVec<Token> &tokens )
{
	//pRoot->mBlockType = AT_ROOT;

	TokNode	*pCurNode = pRoot;

	DVec<TokenID>	bracketsMemory;

	for (size_t i=0; i < tokens.size(); ++i)
	{
		switch ( tokens[i].id )
		{
		case RSLC::T_OP_LFT_BRACKET		:
		case RSLC::T_OP_LFT_SQ_BRACKET	:
		case RSLC::T_OP_LFT_CRL_BRACKET	:
			bracketsMemory.push_back( tokens[i].id );
			pCurNode = pCurNode->AddNewChild( &tokens[i] );
			break;

		case RSLC::T_OP_RGT_BRACKET		:
		case RSLC::T_OP_RGT_SQ_BRACKET	:
		case RSLC::T_OP_RGT_CRL_BRACKET	:
			if (!pCurNode->mpParent ||
				!bracketsMemory.size() ||
				!doBracketsMatch( bracketsMemory.back(), tokens[i].id ) )
			{
				throw Exception( "Mismatched brackets ?", &tokens[i] );
			}
			bracketsMemory.pop_back();

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
}

//==================================================================
void RemoveClosingBrackets( TokNode *pNode, int *pParentScanIdx )
{
	if ( pNode->mpToken )
	{
		switch ( pNode->mpToken->id )
		{
		case RSLC::T_OP_RGT_BRACKET		:
		case RSLC::T_OP_RGT_SQ_BRACKET	:
		case RSLC::T_OP_RGT_CRL_BRACKET	:
			
			// right brackets should have no children !
			DASSERT( pNode->mpChilds.size() == 0 );

			pNode->UnlinkFromParent();
			DSAFE_DELETE( pNode );

			if ( pParentScanIdx )
				*pParentScanIdx -= 1;
			
			return;
		}
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		RemoveClosingBrackets( pNode->mpChilds[i], &i );
	}
}

//==================================================================
void RemoveSemicolons( TokNode *pNode, int *pParentScanIdx )
{
	if ( pNode->mpToken && pNode->mpToken->id == RSLC::T_OP_SEMICOL )
	{
		// right brackets should have no children !
		DASSERT( pNode->mpChilds.size() == 0 );

		pNode->UnlinkFromParent();
		DSAFE_DELETE( pNode );

		if ( pParentScanIdx )
			*pParentScanIdx -= 1;
		
		return;
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		RemoveSemicolons( pNode->mpChilds[i], &i );
	}
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
		
		if ( pNode->mVarLink.GetVarPtr() )
		{
			Register	reg = pNode->mVarLink.GetVarPtr()->mBuild_Register;

			if ( reg.IsValid() )
			{
				std::string regName = GetRegName( reg );

				printf( " // %s - %s - %s",
					regName.c_str(),
					pNode->mVarLink.GetVarPtr()->mInternalName.c_str(),
					pNode->mVarLink.GetVarPtr()->IsVarying() ? "varying" : "uniform"
					);
			}
			else
			{
				printf( " // %s - %s",
					pNode->mVarLink.GetVarPtr()->mInternalName.c_str(),
					pNode->mVarLink.GetVarPtr()->IsVarying() ? "varying" : "uniform"
					);
			}
		}
	}

	switch ( pNode->GetBlockType() )
	{
	case BLKT_UNKNOWN:		break;
	case BLKT_ROOT:			printf( " <ROOT %i> ", pNode->mBlockID );	break;
	case BLKT_SHPARAMS:		printf( " <SH PARAMS %i> ", pNode->mBlockID );	break;
	case BLKT_FNPARAMS:		printf( " <FN PARAMS %i> ", pNode->mBlockID );	break;
	case BLKT_CODEBLOCK:	printf( " <CODEBLK %i> ", pNode->mBlockID );	break;
	case BLKT_EXPRESSION:	printf( " <EXPR %i> ", pNode->mBlockID );		break;
	}

	const DVec<Variable> &vars = pNode->GetVars();

	if ( vars.size() )
	{
		printf( " <<VAR_DEFS: " );

		for (size_t i=0; i < vars.size(); ++i)
		{
			printf( " %s %s %s (%s),",
						vars[i].IsVarying() ? 
							"varying" :
							"uniform",

						VarTypeToString( vars[i].GetVarType() ),

						vars[i].mpDefNameTok ?
							vars[i].mpDefNameTok->str.c_str() :
							"N/A",

						vars[i].mInternalName.c_str()
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
