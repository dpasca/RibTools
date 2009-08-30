//==================================================================
/// RI_SlShader_Ops_Lighting.cpp
///
/// Created by Davide Pasca - 2009/5/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Symbol.h"
#include "RI_State.h"
#include "RI_Attributes.h"
#include "RI_LightSource.h"
#include "RI_SlRunContext.h"
#include "RI_SlShader_Ops_Lighting.h"

//==================================================================
namespace RI
{

//==================================================================
namespace SOP
{

//==================================================================
static inline void illuminate(
				SlColor &accCol,
				const DVec<LightSourceT *>	&pLights,
				const DVec<U16>				&activeLights,
				const Point3 &pos,
				const SlVec3 &Nn,
				float illConeCosA )
{
	for (size_t i=0; i < activeLights.size(); ++i)
	{
		size_t		li	= activeLights[i];

		const LightSourceT	&light = *pLights[ li ];

		if ( light.mType == LightSourceT::TYPE_DISTANT )
		{
			SlColor	lightCol = SlColor( light.mColor.x(), light.mColor.y(), light.mColor.z() ) * light.mIntesity;
			SlVec3	lightDir( light.mRend.mDistant.mDirCS.x(), light.mRend.mDistant.mDirCS.y(), light.mRend.mDistant.mDirCS.z() );

			SlScalar	norLightCosA = Nn.GetDot( lightDir );
			//if ( norLightCosA < illConeCosA )
				accCol += lightCol * norLightCosA;
		}
	}

	//DASSERT( accCol.x >= 0 && accCol.y >= 0 && accCol.z >= 0 );
}

//==================================================================
static inline Color specularbrdf(
						 const Vec3f &L,
						 const Vec3f &V,
						 const Vec3f &N,
						 float ooRoughness
						)
{
	Vec3f	H = (L + V).GetNormalized();
	float	nh = N.GetDot( H );
	float	a = powf( DMAX( 0, nh ), ooRoughness );
	
	return Color( 1, 0, 0 );
}

//==================================================================
// this is a simplified version.. until lights become available 8)
/*
{
color C = 0;
illuminance( P, N, PI/2 )
	C += Cl * normalize(L).N;
return C;
*/

void Inst_Diffuse( SlRunContext &ctx )
{
		  SlColor* lhs	= ctx.GetVoidRW( (		SlColor*)0, 1 );
	const SlVec3 * op1	= ctx.GetVoidRO( (const SlVec3 *)0, 2 );

	const Symbol*	pPSymbol = ctx.mpSymbols->LookupVariable( "P", Symbol::POINT );
	const Point3*	pP = (const Point3 *)pPSymbol->GetData();

	const DVec<LightSourceT *>	&pLights	= ctx.mpAttribs->mpState->GetLightSources();
	const DVec<U16>				&actLights	= ctx.mpAttribs->mActiveLights;

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	float	illConeCosA = cosf( FM_PI_2 );
	
	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				SlVec3	Nn = op1[op1_offset].GetNormalized();

				SlColor	col( 0.0f );

				illuminate( col, pLights, actLights, *pP, Nn, illConeCosA );

				lhs[i] = col;
			}

			++pP;

			op1_offset += op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			SlVec3	Nn = op1[0].GetNormalized();
			SlColor	col( 0.0f );
			illuminate( col, pLights, actLights, *pP, Nn, illConeCosA );

			lhs[0] = col;
		}
	}

	ctx.NextInstruction();
}

//==================================================================
// this is a simplified version.. until lights become available 8)
void Inst_Ambient( SlRunContext &ctx )
{
	SlColor*	lhs	= ctx.GetVoidRW( (SlColor*)0, 1 );

	if NOT( ctx.mCache.mAmbientColDone )
	{
		ctx.mCache.mAmbientColDone = true;

		SlColor	ambCol( 0.f );

		const DVec<LightSourceT *>	&pLights	= ctx.mpAttribs->mpState->GetLightSources();
		const DVec<U16>				&actLights	= ctx.mpAttribs->mActiveLights;

		for (size_t i=0; i < actLights.size(); ++i)
		{
			size_t		li	= actLights[i];

			const LightSourceT	&light = *pLights[ li ];

			SlColor	lightCol( light.mColor.x(), light.mColor.y(), light.mColor.z() );
			SlScalar	lightInt( light.mIntesity );

			if ( light.mType == LightSourceT::TYPE_AMBIENT )
				ambCol += lightCol * lightInt;
		}

		ctx.mCache.mAmbientCol = ambCol;
	}

	//DASSERT( ambCol.x >= 0 && ambCol.y >= 0 && ambCol.z >= 0 );

	for (u_int i=0; i < ctx.mBlocksN; ++i)
	{
		if ( ctx.IsProcessorActive( i ) )
			lhs[i] = ctx.mCache.mAmbientCol;
	}

	ctx.NextInstruction();
}

//==================================================================
}

//==================================================================
}
