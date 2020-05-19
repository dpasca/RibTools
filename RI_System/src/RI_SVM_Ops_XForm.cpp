//==================================================================
/// RI_SVM_Ops_XForm.cpp
///
/// Created by Davide Pasca - 2009/10/7
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SVM_Context.h"
#include "RI_SVM_Ops_XForm.h"
#include "RI_MicroPolygonGrid.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
static void getMtxToCurrent(
					Context &ctx,
					Matrix44 &out_MtxToCurrent,
					const char *pFrSpaceName )
{
	// "camera" is "current" for us indeed
	if ( 0 == strcasecmp( pFrSpaceName, "current" ) ||
		 0 == strcasecmp( pFrSpaceName, "camera" ) )
	{
		out_MtxToCurrent.Identity();
	}
	else
	if ( 0 == strcasecmp( pFrSpaceName, "world" ) )
	{
		out_MtxToCurrent = ctx.mpGrid->mMtxWorldCamera;
	}
	else // consider "object" and "shader" to be the same for now
	if ( 0 == strcasecmp( pFrSpaceName, "object" ) ||
		 0 == strcasecmp( pFrSpaceName, "shader" ) )
	{
		out_MtxToCurrent = ctx.mpGrid->mMtxLocalCamera;
	}
	else
	{
		DASSTHROW( 0, ("Currently unsupported space cast '%s' !", pFrSpaceName) );
	}
}

//==================================================================
template <const Symbol::Type _TYPE>
void xFormname_VXV( Context &ctx, u_int blocksN )
{
	// should do stuff here 8)

		  Float3_*	pDes		= (		 Float3_*)ctx.GetRW( 1 );
	const SlStr  *	pSpaceName	= (const SlStr  *)ctx.GetRO( 2 );
	const Float3_*	pSrc		= (const Float3_*)ctx.GetRO( 3 );

	DASSERT( ctx.IsSymbolVarying( 2 ) == false );

	//const char	*pToSpaceName = "current";
	const char	*pFrSpaceName = pSpaceName->mStr;

	Matrix44	mat;
	getMtxToCurrent( ctx, mat, pFrSpaceName );

	int		src_offset = 0;
	int		src_step = ctx.GetSymbolVaryingStep( 3 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			if ( _TYPE == Symbol::TYP_POINT )  pDes[i] = V3__V3W1_Mul_M44<Float_>( pSrc[src_offset], mat ); else
			if ( _TYPE == Symbol::TYP_VECTOR ) pDes[i] = V3__V3W0_Mul_M44<Float_>( pSrc[src_offset], mat ); else
			if ( _TYPE == Symbol::TYP_NORMAL ) pDes[i] = V3__V3W0_Mul_M44<Float_>( pSrc[src_offset], mat ).GetNormalized();
		}

		src_offset	+= src_step;
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_PXFormname_VXV( Context &ctx, u_int blocksN )	{ xFormname_VXV<Symbol::TYP_POINT>( ctx, blocksN ); }
void Inst_VXFormname_VXV( Context &ctx, u_int blocksN )	{ xFormname_VXV<Symbol::TYP_VECTOR>( ctx, blocksN ); }
void Inst_NXFormname_VXV( Context &ctx, u_int blocksN )	{ xFormname_VXV<Symbol::TYP_NORMAL>( ctx, blocksN ); }

//==================================================================
// quite different for color ?
void Inst_CXFormname_VXV( Context &ctx, u_int blocksN )
{
	// should do stuff here 8)

		  Float3_	*pDes		= (		 Float3_*)ctx.GetRW( 1 );
	const SlStr		*pSpaceName	= (const SlStr	*)ctx.GetRO( 2 );
	const Float3_	*pSrc		= (const Float3_*)ctx.GetRO( 3 );

	DASSERT( ctx.IsSymbolVarying( 2 ) == false );

	//const char	*pToSpaceName = "current";
	const char	*pFrSpaceName = pSpaceName->mStr;

	Matrix44	mat;
	getMtxToCurrent( ctx, mat, pFrSpaceName );

	int		src_offset = 0;
	int		src_step = ctx.GetSymbolVaryingStep( 3 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			pDes[i] = V3__V3W1_Mul_M44<Float_>( pSrc[src_offset], mat );
		}

		src_offset	+= src_step;
	}

	ctx.NextInstruction();
}

//==================================================================
}
//==================================================================
}
