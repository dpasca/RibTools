//==================================================================
/// RRL_Translator_Prims.cpp
///
/// Created by Davide Pasca - 2009/12/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RRL_Translator.h"

//==================================================================
namespace RRL
{

//==================================================================
bool Translator::addCommand_prims(
		const DStr		&nm,
		RI::ParamList	&p,
		const char		*pFileName,
		int				cmdLine )
{
	static RtToken tlPatch0[]			= { RI_BILINEAR, RI_BICUBIC, 0 };

	if ( nm == "Cone" )				{ exN( 3, p ); mState.Cone(			p[0], p[1], p[2] ); }	else
	if ( nm == "Cylinder" )			{ exN( 4, p ); mState.Cylinder(		p[0], p[1], p[2], p[3] ); }	else
	if ( nm == "Sphere" )			{ exN( 4, p ); mState.Sphere(		p[0], p[1], p[2], p[3] ); }	else
	if ( nm == "Hyperboloid" )		{ exN( 7, p ); mState.Hyperboloid(	Float3( p[0], p[1], p[2] ),
																		Float3( p[3], p[4], p[5] ),
																		p[6] ); }	else
	if ( nm == "Paraboloid" )		{ exN( 4, p ); mState.Paraboloid(	p[0], p[1], p[2], p[3] ); }	else
	if ( nm == "Torus" )			{ exN( 5, p ); mState.Torus(		p[0], p[1], p[2], p[3], p[4] ); }	else
	if ( nm == "Patch" )			{ geN( 3, p ); mState.Patch(		matchToken( p[0], tlPatch0 ), p ); }	else
	if ( nm == "PatchMesh" )		{ geN( 5, p ); mState.PatchMesh(	matchToken( p[0], tlPatch0 ), p ); }	else

	if ( nm == "NuPatch" )			{
										geN( 11, p );
										int		nu		= (int)p[0];
										int		uorder	= (int)p[1];
										const float	*pUknot	= p[2].PFlt();
										float	umin	= (float)p[3];
										float	umax	= (float)p[4];
										int		nv		= (int)p[5];
										int		vorder	= (int)p[6];
										const float	*pVknot	= p[7].PFlt();
										float	vmin	= (float)p[8];
										float	vmax	= (float)p[9];

										mState.NuPatch(
												nu		,
												uorder	,
												pUknot	,
												umin	,
												umax	,
												nv		,
												vorder	,
												pVknot	,
												vmin	,
												vmax	,
												p
											);

									}	else

	if ( nm == "Polygon" )				{ geN( 2, p ); mState.Polygon( p ); }	else
	if ( nm == "PointsPolygons" )		{ geN( 4, p ); mState.PointsPolygons( p ); } else
	if ( nm == "PointsGeneralPolygons" ){ geN( 5, p ); mState.PointsGeneralPolygons( p ); }
	else
		return false;

#if defined(DEBUG) || defined(_DEBUG)
	mState.mParams.mpFramework->Dbg_MarkLastPrim( pFileName, cmdLine );
#endif

	return true;
}

//==================================================================
}
