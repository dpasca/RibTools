/*
 *  RI_Tokens.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/22.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "RI_Tokens.h"
#include "RI_Base.h"

RtToken	RI_FRAMEBUFFER		= "framebuffer";
RtToken RI_FILE				= "file";
RtToken	RI_RGB				= "rgb";
RtToken RI_RGBA				= "rgba";
RtToken	RI_RGBZ				= "rgbz";
RtToken RI_RGBAZ			= "rgbaz";
RtToken RI_A				= "a";
RtToken RI_Z				= "z";
RtToken RI_AZ				= "az";
RtToken	RI_PERSPECTIVE		= "perspective";
RtToken RI_ORTHOGRAPHIC		= "orthographic";
RtToken	RI_HIDDEN			= "hidden";
RtToken RI_PAINT			= "paint";
RtToken	RI_CONSTANT			= "constant";
RtToken RI_SMOOTH			= "smooth";
RtToken	RI_FLATNESS			= "flatness";
RtToken RI_FOV				= "fov";
RtToken	RI_AMBIENTLIGHT		= "ambientlight";
RtToken RI_POINTLIGHT		= "pointlight";
RtToken RI_DISTANTLIGHT		= "distantlight";
RtToken RI_SPOTLIGHT 		= "spotlight";
RtToken	RI_INTENSITY		= "intensity";
RtToken RI_LIGHTCOLOR		= "lightcolor";
RtToken RI_FROM				= "from";
RtToken RI_TO				= "to";
RtToken RI_CONEANGLE		= "coneangle";
RtToken RI_CONEDELTAANGLE	= "conedeltaangle";
RtToken RI_BEAMDISTRIBUTION	= "beamdistribution";
RtToken	RI_MATTE			= "matte";
RtToken RI_METAL			= "metal";
RtToken RI_SHINYMETAL		= "shinymetal";
RtToken RI_PLASTIC			= "plastic";
RtToken RI_PAINTEDPLASTIC	= "paintedplastic";
RtToken	RI_KA				= "ka";
RtToken RI_KD				= "kd";
RtToken RI_KS				= "ks";
RtToken RI_ROUGHNESS		= "roughness";
RtToken RI_KR				= "kr";
RtToken RI_TEXTURENAME		= "texturename";
RtToken RI_SPECULARCOLOR	= "specularcolor";
RtToken	RI_DEPTHCUE			= "depthcue";
RtToken RI_FOG				= "fog";   
RtToken RI_BUMPY			= "bumpy";   
RtToken	RI_MINDISTANCE		= "mindistance";
RtToken RI_MAXDISTANCE		= "maxdistance";
RtToken RI_BACKGROUND		= "background";
RtToken RI_DISTANCE			= "distance";
RtToken RI_AMPLITUDE		= "amplitude";
RtToken	RI_RASTER			= "raster";
RtToken RI_SCREEN			= "screen";
RtToken RI_CAMERA			= "camera";
RtToken RI_WORLD			= "world";
RtToken RI_OBJECT			= "object";
RtToken	RI_INSIDE			= "inside";
RtToken RI_OUTSIDE			= "outside";
RtToken RI_LH				= "lh";
RtToken RI_RH				= "rh";
RtToken	RI_P				= "p";
RtToken RI_PZ				= "pz";
RtToken RI_PW				= "pw";
RtToken RI_N				= "n";
RtToken RI_NP				= "np";
RtToken RI_CS				= "cs";
RtToken RI_OS				= "os";
RtToken RI_S				= "s";
RtToken RI_T				= "t";
RtToken RI_ST				= "st";
RtToken	RI_BILINEAR			= "bilinear";
RtToken RI_BICUBIC			= "bicubic";
RtToken	RI_PRIMITIVE		= "primitive";
RtToken RI_INTERSECTION		= "intersection";
RtToken RI_UNION			= "union";
RtToken RI_DIFFERENCE		= "difference";
RtToken	RI_PERIODIC			= "periodic";
RtToken RI_NONPERIODIC		= "nonperiodic";
RtToken RI_CLAMP			= "clamp";
RtToken RI_BLACK			= "black";
RtToken	RI_IGNORE			= "ignore";
RtToken RI_PRINT			= "print";
RtToken RI_ABORT			= "abort";
RtToken RI_HANDLER			= "handler";

RtToken	RI_EMPTY_TOKEN		= "";

RtToken	RI_BEZIERBASIS		= "bezier";
RtToken	RI_BSPLINEBASIS		= "b-spline";
RtToken	RI_CATMULLROMBASIS	= "catmull-rom";
RtToken	RI_HERMITEBASIS		= "hermite";
RtToken	RI_POWERBASIS		= "power";

//==================================================================
namespace RI
{

//==================================================================
TokenManager::TokenManager()
{
	FindOrAddToken(	RI_FRAMEBUFFER				);
	FindOrAddToken(	RI_FILE						);
	FindOrAddToken(	RI_RGB						);
	FindOrAddToken(	RI_RGBA						);
	FindOrAddToken(	RI_RGBZ						);
	FindOrAddToken(	RI_RGBAZ					);
	FindOrAddToken(	RI_A						);
	FindOrAddToken(	RI_Z						);
	FindOrAddToken(	RI_AZ						);
	FindOrAddToken(	RI_PERSPECTIVE				);
	FindOrAddToken(	RI_ORTHOGRAPHIC				);
	FindOrAddToken(	RI_HIDDEN					);
	FindOrAddToken(	RI_PAINT					);
	FindOrAddToken(	RI_CONSTANT					);
	FindOrAddToken(	RI_SMOOTH					);
	FindOrAddToken(	RI_FLATNESS					);
	FindOrAddToken(	RI_FOV						);
	FindOrAddToken(	RI_AMBIENTLIGHT				);
	FindOrAddToken(	RI_POINTLIGHT				);
	FindOrAddToken(	RI_DISTANTLIGHT				);
	FindOrAddToken(	RI_SPOTLIGHT 				);
	FindOrAddToken(	RI_INTENSITY				);
	FindOrAddToken(	RI_LIGHTCOLOR				);
	FindOrAddToken(	RI_FROM						);
	FindOrAddToken(	RI_TO						);
	FindOrAddToken(	RI_CONEANGLE				);
	FindOrAddToken(	RI_CONEDELTAANGLE			);
	FindOrAddToken(	RI_BEAMDISTRIBUTION			);
	FindOrAddToken(	RI_MATTE					);
	FindOrAddToken(	RI_METAL					);
	FindOrAddToken(	RI_SHINYMETAL				);
	FindOrAddToken(	RI_PLASTIC					);
	FindOrAddToken(	RI_PAINTEDPLASTIC			);
	FindOrAddToken(	RI_KA						);
	FindOrAddToken(	RI_KD						);
	FindOrAddToken(	RI_KS						);
	FindOrAddToken(	RI_ROUGHNESS				);
	FindOrAddToken(	RI_KR						);
	FindOrAddToken(	RI_TEXTURENAME				);
	FindOrAddToken(	RI_SPECULARCOLOR			);
	FindOrAddToken(	RI_DEPTHCUE					);
	FindOrAddToken(	RI_FOG						);
	FindOrAddToken(	RI_BUMPY					);
	FindOrAddToken(	RI_MINDISTANCE				);
	FindOrAddToken(	RI_MAXDISTANCE				);
	FindOrAddToken(	RI_BACKGROUND				);
	FindOrAddToken(	RI_DISTANCE					);
	FindOrAddToken(	RI_AMPLITUDE				);
	FindOrAddToken(	RI_RASTER					);
	FindOrAddToken(	RI_SCREEN					);
	FindOrAddToken(	RI_CAMERA					);
	FindOrAddToken(	RI_WORLD					);
	FindOrAddToken(	RI_OBJECT					);
	FindOrAddToken(	RI_INSIDE					);
	FindOrAddToken(	RI_OUTSIDE					);
	FindOrAddToken(	RI_LH						);
	FindOrAddToken(	RI_RH						);
	FindOrAddToken(	RI_P						);
	FindOrAddToken(	RI_PZ						);
	FindOrAddToken(	RI_PW						);
	FindOrAddToken(	RI_N						);
	FindOrAddToken(	RI_NP						);
	FindOrAddToken(	RI_CS						);
	FindOrAddToken(	RI_OS						);
	FindOrAddToken(	RI_S						);
	FindOrAddToken(	RI_T						);
	FindOrAddToken(	RI_ST						);
	FindOrAddToken(	RI_BILINEAR					);
	FindOrAddToken(	RI_BICUBIC					);
	FindOrAddToken(	RI_PRIMITIVE				);
	FindOrAddToken(	RI_INTERSECTION				);
	FindOrAddToken(	RI_UNION					);
	FindOrAddToken(	RI_DIFFERENCE				);
	FindOrAddToken(	RI_PERIODIC					);
	FindOrAddToken(	RI_NONPERIODIC				);
	FindOrAddToken(	RI_CLAMP					);
	FindOrAddToken(	RI_BLACK					);
	FindOrAddToken(	RI_IGNORE					);
	FindOrAddToken(	RI_PRINT					);
	FindOrAddToken(	RI_ABORT					);
	FindOrAddToken(	RI_HANDLER					);
	FindOrAddToken(	RI_EMPTY_TOKEN				);
	FindOrAddToken(	RI_BEZIERBASIS				, BezierBasis		);
	FindOrAddToken(	RI_BSPLINEBASIS				, BSplineBasis		);
	FindOrAddToken(	RI_CATMULLROMBASIS			, CatmullRomBasis	);
	FindOrAddToken(	RI_HERMITEBASIS				, HermiteBasis		);
	FindOrAddToken(	RI_POWERBASIS				, PowerBasis		);
}

//==================================================================
template <class T>
size_t findIdxByName( const DVec<T> &vec, const char *pName )
{
	for (size_t i=0; i < vec.size(); ++i)
		if ( 0 == strcmp( pName, vec[i].pName ) )
			return i;

	return NPOS;
}

//==================================================================
template <class T>
const T *findElemByName( const DVec<T> &vec, const char *pName )
{
	for (size_t i=0; i < vec.size(); ++i)
		if ( 0 == strcmp( pName, vec[i].pName ) )
			return &vec[i];

	return NULL;
}

//==================================================================
void TokenManager::FindOrAddToken( const char *pName, const char *pDescr )
{
	if NOT( pDescr )
	{
		if ( NPOS == findIdxByName( mTokVoid, pName ) )
		{
			TokenVoid	*pNew = mTokVoid.grow();
			pNew->pName = pName;
		}
	}
}

//==================================================================
void TokenManager::FindOrAddToken( const char *pName, const RtBasis &basis )
{
	TokenBasis	*pTok = NULL;

	size_t	idx = findIdxByName( mTokBasis, pName );

	if ( NPOS == idx )
	{
		pTok = mTokBasis.grow();
		pTok->pName = pName;
	}

	pTok->value = basis;
}

//==================================================================
const TokenBase *TokenManager::Find( Type type, const char *pName ) const
{
	size_t	idx;

	switch ( type )
	{
	case	T_VOID:		return (const TokenBase *)findElemByName( mTokVoid	, pName );
	case	T_FLOAT:	return (const TokenBase *)findElemByName( mTokFloat	, pName );
	case	T_INT:		return (const TokenBase *)findElemByName( mTokInt	, pName );
	case	T_POINT:	return (const TokenBase *)findElemByName( mTokPoint	, pName );
	case	T_COLOR:	return (const TokenBase *)findElemByName( mTokColor	, pName );
	case	T_BASIS:	return (const TokenBase *)findElemByName( mTokBasis	, pName );
	}

	DASSERT( 0 );
	return NULL;
}

//==================================================================
}