//==================================================================
/// RSLC_Arrays.cpp
///
/// Created by Davide Pasca - 2010/3/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <math.h>

#include "RSLC_Tree.h"
#include "RSLC_Exceptions.h"

#include "RSLC_Arrays.h"

//==================================================================
namespace RSLC
{

//==================================================================
void RealizeArraysSizes( TokNode *pNode )
{
	DVec<Variable>	&vars = pNode->GetVars();
	for (size_t i=0; i < vars.size(); ++i)
	{
	    if NOT( vars[i].mIsArray )
			continue;

		/* At this point, we are dealing with the following structure
		 (because of the reparenting by operators..
	
		float varname[3]

		..is..

		float
		[				// pSqBracket
			varname		// vars[i].mpDefNameNode
			3			// pArrSizeNode
		*/

		TokNode *pSqBracket = vars[i].mpDefNameNode->mpParent;
		DASSERT( pSqBracket != NULL );

		TokNode	*pArrSizeNode = pSqBracket->GetChildTry( 1 );
		DASSERT( pArrSizeNode != NULL );

		double	arrSize = pArrSizeNode->mpToken->ConstNum;
		U64		arrSizeInt = (U64)ceil( arrSize );

		// if it's negative or if it's not an integral value
		if ( arrSize < 0 || arrSizeInt != arrSize )
			throw Exception( pSqBracket, "Array size must be positive integral value" );

		// enforce a "large enough" maximum size
		if ( arrSize > (double)(1 << 24) )
			throw Exception( pSqBracket, "Array size is currently limited to %i !", 1 << 24 );

		// finally set the array size
		vars[i].mArraySize = (U32)arrSizeInt;
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
		RealizeArraysSizes( pNode->mpChilds[i] );
}

//==================================================================
void GenerateArrayAssignOperators( TokNode *pNode )
{
	return;

	// convert:
	//	=				// T_OP_ASSIGN
	//		[			// child 0
	//			arr
	//			idx
	//		src			// child 1
	//
	// ..to..
	//
	//	=[				// T_OP_ASSIGN_ARR_ITEM
	//		arr
	//		idx
	//		src
	if ( pNode->IsTokenID( T_OP_ASSIGN ) )
	{
		TokNode	*pDestNode = pNode->GetChildTry( 0 );
		DASSERT( pDestNode != NULL );

		if ( pDestNode->IsTokenID( T_OP_LFT_SQ_BRACKET ) )
		{
			DASSERT( pDestNode->mpChilds.size() == 2 );

			// "hijack" the assignment operator and turn it into
			// an "array item assignment" virtual operator
			pNode->mpToken->id = T_OP_ASSIGN_ARR_ITEM;
			pNode->mpToken->str = "=[";
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
		GenerateArrayAssignOperators( pNode->mpChilds[i] );
}

//==================================================================
}
