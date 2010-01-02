//==================================================================
/// RI_SlShader_Ops_XForm.cpp
///
/// Created by Davide Pasca - 2009/10/7
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SlRunContext.h"
#include "RI_SlShader_Ops_XForm.h"
#include "RI_MicroPolygonGrid.h"

//==================================================================
namespace RI
{

//==================================================================
namespace SOP
{

//==================================================================
static void getMtxToCurrent(
					SlRunContext &ctx,
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
void xFormname_VXV( SlRunContext &ctx )
{
	// should do stuff here 8)

		  Float3_*	pDes		= ctx.GetVoidRW( (		Float3_ *)0, 1 );
	const SlStr*	pSpaceName	= ctx.GetVoidRO( (const SlStr  *)0, 2 );
	const Float3_*	pSrc		= ctx.GetVoidRO( (const Float3_ *)0, 3 );

	DASSERT( ctx.IsSymbolVarying( 2 ) == false );

	bool	des_varying = ctx.IsSymbolVarying( 1 );

	//const char	*pToSpaceName = "current";
	const char	*pFrSpaceName = pSpaceName->mStr;

	Matrix44	mat;
	getMtxToCurrent( ctx, mat, pFrSpaceName );

	if ( des_varying )
	{
		int		src_offset = 0;
		int		src_step = ctx.GetSymbolVaryingStep( 3 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				if ( _TYPE == Symbol::TYP_POINT )  pDes[i] = V3__V3W1_Mul_M44<Float_>( pSrc[src_offset], mat ); else
				if ( _TYPE == Symbol::TYP_VECTOR ) pDes[i] = V3__V3W0_Mul_M44<Float_>( pSrc[src_offset], mat ); else
				if ( _TYPE == Symbol::TYP_NORMAL ) pDes[i] = V3__V3W0_Mul_M44<Float_>( pSrc[src_offset], mat ).GetNormalized();
			}

			src_offset	+= src_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 3 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			if ( _TYPE == Symbol::TYP_POINT )  pDes[0] = V3__V3W1_Mul_M44<Float_>( pSrc[0], mat ); else
			if ( _TYPE == Symbol::TYP_VECTOR ) pDes[0] = V3__V3W0_Mul_M44<Float_>( pSrc[0], mat ); else
			if ( _TYPE == Symbol::TYP_NORMAL ) pDes[0] = V3__V3W0_Mul_M44<Float_>( pSrc[0], mat ).GetNormalized();
		}
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_PXFormname_VXV( SlRunContext &ctx )	{ xFormname_VXV<Symbol::TYP_POINT>( ctx ); }
void Inst_VXFormname_VXV( SlRunContext &ctx )	{ xFormname_VXV<Symbol::TYP_VECTOR>( ctx ); }
void Inst_NXFormname_VXV( SlRunContext &ctx )	{ xFormname_VXV<Symbol::TYP_NORMAL>( ctx ); }

//==================================================================
// quite different for color ?
void Inst_CXFormname_VXV( SlRunContext &ctx )
{
	// should do stuff here 8)

		  Float3_*	pDes		= ctx.GetVoidRW( (		Float3_ *)0, 1 );
	const SlStr*	pSpaceName	= ctx.GetVoidRO( (const SlStr  *)0, 2 );
	const Float3_*	pSrc		= ctx.GetVoidRO( (const Float3_ *)0, 3 );

	DASSERT( ctx.IsSymbolVarying( 2 ) == false );

	bool	des_varying = ctx.IsSymbolVarying( 1 );

	//const char	*pToSpaceName = "current";
	const char	*pFrSpaceName = pSpaceName->mStr;

	Matrix44	mat;
	getMtxToCurrent( ctx, mat, pFrSpaceName );

	if ( des_varying )
	{
		int		src_offset = 0;
		int		src_step = ctx.GetSymbolVaryingStep( 3 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				pDes[i] = V3__V3W1_Mul_M44<Float_>( pSrc[src_offset], mat );
			}

			src_offset	+= src_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 3 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			pDes[0] = V3__V3W1_Mul_M44<Float_>( pSrc[0], mat );
		}
	}

	ctx.NextInstruction();
}

//==================================================================
}

//==================================================================
}
