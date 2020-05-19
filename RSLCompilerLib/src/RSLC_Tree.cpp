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
TokNode::TokNode( const Token *pObj ) :
	mpToken((Token *)pObj),
	mOwnToken(false),
	mpParent(NULL),
	mNodeType(TYPE_STANDARD),
	mBlockType(BLKT_UNKNOWN),
	mIsFuncOp(false),
	mOutputFuncOpEnd(false),
	mBlockID(0)
{
#ifdef _DEBUG
	mUIDCnt = sUIDCnt++;
#endif
}

//==================================================================
TokNode::TokNode(
			const char		*pTokStr,
			TokenID			tokId,
			TokenIDType		tokIdType,
			const TokNode	*pInheritNodeTokPos ) :
	mpToken(
		DNEW Token(
				pTokStr,
				tokId,
				tokIdType,
				pInheritNodeTokPos ? pInheritNodeTokPos->mpToken : NULL ) ),
	mOwnToken(true),
	mpParent(NULL),
	mNodeType(TYPE_STANDARD),
	mBlockType(BLKT_UNKNOWN),
	mIsFuncOp(false),
	mOutputFuncOpEnd(false),
	mBlockID(0)
{
#ifdef _DEBUG
	mUIDCnt = sUIDCnt++;
#endif
}

//==================================================================
TokNode::TokNode( const TokNode &from ) :
	mpToken			((from.mOwnToken && from.mpToken) ? DNEW Token( *from.mpToken ) : from.mpToken ),
	mOwnToken		(from.mOwnToken),
	mVariables		(from.mVariables),
	mFunctions		(from.mFunctions),
	mpParent		(NULL),
	mNodeType		(from.mNodeType),
	mBlockType		(from.mBlockType),
	mIsFuncOp		(from.mIsFuncOp),
	mOutputFuncOpEnd(from.mOutputFuncOpEnd),
	mBlockID		(0),
	mVarLink		(from.mVarLink),
	mFuncCall		(from.mFuncCall)
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

	if ( mOwnToken )
		DSAFE_DELETE( mpToken );

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
void TokNode::DeleteReplaceToken( Token *pToken )
{
	if ( mOwnToken )
		DSAFE_DELETE( mpToken );

	mpToken		= pToken;
	mOwnToken	= true;
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
		return NULL;

	for (size_t i=0; i < mpParent->mpChilds.size(); ++i)
	{
		if ( mpParent->mpChilds[i] == this )
		{
			size_t nextIdx = i + 1;
			// return a sibling if any...
			if ( nextIdx < mpParent->mpChilds.size() )
				return mpParent->mpChilds[ nextIdx ];
			else	// otherwise NULL
				return NULL;
		}
	}

	DASSTHROW( 0, ("Bad tree ?!") );
	return NULL;
}

//==================================================================
void TokNode::AddAfterThis( TokNode *pNode )
{
	for (size_t i=0; i < mpParent->mpChilds.size(); ++i)
	{
		if ( mpParent->mpChilds[i] == this )
		{
			pNode->mpParent = mpParent;

			size_t nextIdx = i + 1;

			if ( nextIdx < mpParent->mpChilds.size() )
			{
				mpParent->mpChilds.insert(
					mpParent->mpChilds.begin() + nextIdx,
					pNode );
			}
			else
			{
				mpParent->mpChilds.push_back( pNode );
			}

			return;
		}
	}

	DASSTHROW( 0, ("Broken tree ?") );
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
void TokNode::Reparent( TokNode *pNewParent )
{
	if ( mpParent == pNewParent )
	{
		DASSERT( 0 );
		return;
	}

	UnlinkFromParent();
	mpParent = pNewParent;
}

//==================================================================
void TokNode::ReplaceNode( TokNode *pOldNode )
{
	if NOT( pOldNode->mpParent )
		return;

	for (size_t i=0; i < pOldNode->mpParent->mpChilds.size(); ++i)
	{
		if ( pOldNode->mpParent->mpChilds[i] == pOldNode )
		{
			pOldNode->mpParent->mpChilds[i] = this;
			mpParent = pOldNode->mpParent;
			pOldNode->mpParent = NULL;
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
bool TokNode::IsVarArray() const
{
	if ( GetTokIDType() == T_TYPE_VALUE )
	{
		return false;
	}
	else
	if ( mVarLink.IsValid() )
		return mVarLink.GetVarPtr()->IsArray();
	else
	if ( mFuncCall.mpFunc )
	{
		return mFuncCall.mpFunc->mRetVarIsArray;
	}
	else
	{
		DASSERT( 0 );
		return false;
	}
}

//==================================================================
RSLC::VarType TokNode::GetVarType() const
{
	if ( GetTokIDType() == T_TYPE_VALUE )
	{
		if ( GetTokID() == T_VL_NUMBER )
			return VT_FLOAT;
		else
		if ( GetTokID() == T_VL_STRING )
			return VT_STRING;
		else
		if ( GetTokID() == T_VL_BOOL_TRUE )
			return VT_BOOL;
		else
		if ( GetTokID() == T_VL_BOOL_FALSE )
			return VT_BOOL;

		DASSERT( 0 );
		return VT_UNKNOWN;
	}
	else
	if ( mVarLink.IsValid() )
		return mVarLink.GetVarPtr()->GetVarType();
	else
	if ( mFuncCall.mpFunc )
	{
		return mFuncCall.mpFunc->mRetVarType;
	}
	else
	{
		//DASSERT( 0 );
		return VT_UNKNOWN;
	}
}

//==================================================================
bool TokNode::IsVarying() const
{
	if ( GetTokIDType() == T_TYPE_VALUE )
		return false;
	else
	if ( mVarLink.IsValid() )
		return mVarLink.GetVarPtr()->IsVarying();
	else
	{
		//DASSERT( 0 );
		return false;
	}
}

//==================================================================
bool TokNode::TrySetVarying_AndForceIfTrue( bool onoff )
{
	if ( GetTokIDType() == T_TYPE_VALUE )
		return !onoff;	// if it's a value then it's ok if "uniform" is expected
	else
	if ( mVarLink.IsValid() )
	{
		Variable	*pVar = mVarLink.GetVarPtr();

		// are we changing ?
		if ( pVar->IsVarying() != onoff )
		{
			if ( pVar->IsForcedDetail() )
				return false;	// detail is forced ? Failure..
			else
			{
				// ..all good, we can change the detail
				pVar->SetVarying( onoff );
			}
		}
		else
		{
			// not changing.. all fine then..
		}

		// try force the detail if varying anyway
		if ( onoff && !pVar->IsForcedDetail() )
			pVar->SetForcedDetail( true );

		return true;
	}
	else
	{
		//DASSERT( 0 );
		return false;
	}
}

//==================================================================
static void defineBlockTypeAndID_OnBracket( TokNode *pBracketNode )
{
	DASSERT( pBracketNode->GetBlockType() == BLKT_UNKNOWN );

	TokNode	*pFnName = pBracketNode->GetLeft();
	if NOT( pFnName )
	{
		// set as a plain expression and return
		pBracketNode->SetBlockType( BLKT_EXPRESSION );
		return;
	}

	// preceded by a non-terminal ?
	// ..see if it's a function call or a function declaration..
	if ( pFnName->IsNonTerminal() )
	{
		// what's before the function name ?
		TokNode	*pFnType = pFnName->GetLeft();
		if ( pFnType )
		{
			// __funcop keyword.. ? then it's a declaration
			if ( pFnType->GetTokID() == T_KW___funcop )
			{
				pBracketNode->SetBlockType( BLKT_DECL_PARAMS_FN );
				return;
			}

			// shader type ? Then it's shader params block for sure
			if ( pFnType->GetTokIDType() == T_TYPE_SHADERTYPE )
			{
				pBracketNode->SetBlockType( BLKT_DECL_PARAMS_SH );
				return;
			}
		}

		// here fn-type could be a datatype or nothing at all..
		// ..but at this stage we just care to know that it's
		// not an expression.. but rather the params block
		// of a function definition or a function call
		pBracketNode->SetBlockType( BLKT_CALL_OR_DELC_PARAMS_FN );
		return;
	}
	else // is it a constructor instead ? e.g. color(0,0,1)
	if ( pFnName->IsDataType() )
	{
		pBracketNode->SetBlockType( BLKT_CALL_OR_DELC_PARAMS_FN );
		return;
	}
	else // is it a space-casted constructor instead ? e.g. color "rgb" (0,0,1)
	if ( pFnName->GetTokID() == T_VL_STRING )
	{
		pBracketNode->SetBlockType( BLKT_CALL_OR_DELC_PARAMS_FN );
		return;
	}

	// set as a plain expression otherwise
	pBracketNode->SetBlockType( BLKT_EXPRESSION );
}

//==================================================================
static void defineBlockTypeAndID( TokNode *pNode, u_int &blockCnt )
{
	if ( pNode->mpToken )
	{
		if ( pNode->IsTokenID( T_OP_LFT_BRACKET ) )
		{
			defineBlockTypeAndID_OnBracket( pNode );
		}
		else
		if ( pNode->IsTokenID( T_OP_LFT_CRL_BRACKET ) )
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
		defineBlockTypeAndID( pNode->mpChilds[i], blockCnt );
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
void MakeTree( TokNode *pRoot, DVec<Token> &tokens, u_int &out_blockCnt )
{
	//TokNode	*pParent = pRoot;
	//DVec<TokNode*>	pParentsMemory;
	//pParentsMemory.push_back( pParent );
	//pPrev = pParent = pParent->AddNewChild( &tokens[i] );
	//pParentsMemory.pop_back();

	out_blockCnt = 0;

	TokNode			*pNode = pRoot;
	DVec<TokenID>	bracketsMemory;

	for (size_t i=0; i < tokens.size(); ++i)
	{
		switch ( tokens[i].id )
		{
		case RSLC::T_OP_LFT_BRACKET		:
		case RSLC::T_OP_LFT_CRL_BRACKET	:
		case RSLC::T_OP_LFT_SQ_BRACKET	:
			bracketsMemory.push_back( tokens[i].id );

			if ( tokens[i].id == RSLC::T_OP_LFT_SQ_BRACKET )
			{
				pNode->AddNewChild( &tokens[i] );

				Token	*pNewBrkTok =
					DNEW Token(
							"(",
							RSLC::T_OP_LFT_BRACKET,
							RSLC::T_TYPE_OPERATOR,
							&tokens[i] );

				pNode = pNode->AddNewChild( pNewBrkTok );

				//pNode = pNewParent;
			}
			else
			// square bracket is indents itself
			//if ( tokens[i].id == RSLC::T_OP_LFT_SQ_BRACKET )
			//{		
			//	if NOT( pNode->mpChilds.size() )
			//		throw Exception( "Misplaced square bracket ?", &tokens[i] );

			//	pNode = pNode->mpChilds[pNode->mpChilds.size()-1]->AddNewChild( &tokens[i] );
			//}
			//else
			{
				pNode = pNode->AddNewChild( &tokens[i] );
			}
			break;

		case RSLC::T_OP_RGT_BRACKET		:
		case RSLC::T_OP_RGT_CRL_BRACKET	:
		case RSLC::T_OP_RGT_SQ_BRACKET	:
			if (!pNode->mpParent ||
				!bracketsMemory.size() ||
				!doBracketsMatch( bracketsMemory.back(), tokens[i].id ) )
			{
				throw Exception( "Mismatched brackets ?", &tokens[i] );
			}

			pNode = pNode->mpParent;
			// square bracket is indented itself
			//if ( tokens[i].id == RSLC::T_OP_RGT_SQ_BRACKET )
			//	pNode = pNode->mpParent;

			bracketsMemory.pop_back();
			break;

		default:
			pNode->AddNewChild( &tokens[i] );
			break;
		}
	}

	defineBlockTypeAndID( pRoot, out_blockCnt );
}

//==================================================================
#if 0
void RemoveClosingBrackets( TokNode *pNode, int *pParentScanIdx )
{
	if ( pNode->mpToken )
	{
		switch ( pNode->GetTokID() )
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
#endif

#if 1
//==================================================================
void RemoveOpeningExprBrackets( TokNode *pNode, int *pParentScanIdx )
{
	if ( pNode->mpToken )
	{
		if ( pNode->IsExpressionBlock() )
		{
			DASSERT( pNode->mpChilds.size() == 1 );

			TokNode	*pChildToKeep = pNode->mpChilds[0];

			TokNode	*pParent = pNode->mpParent;

			pChildToKeep->ReplaceNode( pNode );
			//pChildToKeep->Reparent( pParent );
			
			pNode->UnlinkFromParent();
			pNode->mpChilds.clear();
			DSAFE_DELETE( pNode );

			if ( pParentScanIdx )
				*pParentScanIdx -= 1;

			return;
		}
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		RemoveOpeningExprBrackets( pNode->mpChilds[i], &i );
	}
}
#endif

//==================================================================
void RemoveSemicolons( TokNode *pNode, int *pParentScanIdx )
{
	if ( pNode->mpToken && pNode->GetTokID() == RSLC::T_OP_SEMICOL )
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
				DStr regName = reg.GetName();

				printf( "        // %s - %s - %s",
					regName.c_str(),
					pNode->mVarLink.GetVarPtr()->mInternalName.c_str(),
					pNode->mVarLink.GetVarPtr()->IsVarying() ? "varying" : "uniform"
					);
			}
			else
			{
				printf( "        // %s - %s",
					pNode->mVarLink.GetVarPtr()->mInternalName.c_str(),
					pNode->mVarLink.GetVarPtr()->IsVarying() ? "varying" : "uniform"
					);
			}
		}
		else
		{
			printf( "        //" );
		}
	}

	if ( pNode->mNodeType == TokNode::TYPE_FUNCDEF )
		printf( " <FUNCDEF> " );

	switch ( pNode->GetBlockType() )
	{
	case BLKT_UNKNOWN:			break;
	case BLKT_ROOT:				printf( " <ROOT %i> ", pNode->mBlockID );	break;
	case BLKT_CALL_OR_DELC_PARAMS_FN: printf( " <CALL OR DECL PARAMS %i> ", pNode->mBlockID );	break;
	case BLKT_DECL_PARAMS_SH:	printf( " <DECL PARAMS SH %i> ", pNode->mBlockID );	break;
	case BLKT_DECL_PARAMS_FN:	printf( " <DECL PARAMS FN %i> ", pNode->mBlockID );	break;
	case BLKT_CALL_PARAMS_FN:	printf( " <CALL PARAMS FN %i> ", pNode->mBlockID );	break;
	case BLKT_CODEBLOCK:		printf( " <CODEBLOCK %i> ", pNode->mBlockID );	break;
	case BLKT_EXPRESSION:		printf( " <EXPRESSION %i> ", pNode->mBlockID );		break;
	default:
		DASSERT( 0 );
		break;
	}

	const DVec<Variable> &vars = pNode->GetVars();

	if ( vars.size() )
	{
		printf( " <<VAR_DEFS: " );

		for (size_t i=0; i < vars.size(); ++i)
		{
			printf( " %s %s %s (%s)",
						vars[i].IsVarying() ? 
							"varying" :
							"uniform",

						VarTypeToString( vars[i].GetVarType() ),

						vars[i].mpDefNameNode ?
							vars[i].mpDefNameNode->GetTokStr() :
							"N/A",

						vars[i].mInternalName.c_str()
					);

			if ( vars[i].IsArray() )
				printf( "[%u]", vars[i].mArraySize );

			printf( "," );
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
