/*
 *  RI_Machine.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "RI_Machine.h"

//==================================================================
namespace RI
{

//==================================================================
/// 
//==================================================================
Machine::Machine(
			FrameworkBase *pFramework,
			const char *pDefaultShadersDir,
			int forcedWd,
			int forcedHe ) :
	mState(pFramework, pDefaultShadersDir),
	mForcedWd(forcedWd),
	mForcedHe(forcedHe)
{
}

//==================================================================
void Machine::unknownCommand( const char *pCmdName )
{
	printf( "Unknown command %s !\n", pCmdName );
}

//==================================================================
void Machine::exN( size_t n, const ParamList &cmdParams )
{
	if NOT( cmdParams.size() == n )
	{
		mState.ErrHandler(
				E_BADARGUMENT,
				"Expecting %i params, got %i",
				n, cmdParams.size() );
		DASSTHROW( false, ("E_BADARGUMENT") );
	}
}

//==================================================================
void Machine::geN( size_t n, const ParamList &cmdParams )
{
	if NOT( cmdParams.size() >= n  )
	{
		mState.ErrHandler(
				E_BADARGUMENT,
				"Expecting at least %i params, got %i",
				n, cmdParams.size() );
		DASSTHROW( false, ("E_BADARGUMENT") );
	}
}

//==================================================================
static void mkBound( Bound &out_Bound, ParamList &cmdParams )
{
	if ( cmdParams.size() == 1 )
	{
		const float *pSrc = cmdParams[0].PFlt(6);

		out_Bound.SetMin( pSrc + 0 );
		out_Bound.SetMax( pSrc + 3 );
	}
	else
	if ( cmdParams.size() == 6 )
	{
		out_Bound.SetMin(
					cmdParams[0].u.floatVal,
					cmdParams[1].u.floatVal,
					cmdParams[2].u.floatVal );
		out_Bound.SetMax(
					cmdParams[3].u.floatVal,
					cmdParams[4].u.floatVal,
					cmdParams[5].u.floatVal );
	}
	else
		DASSTHROW( false, ("Wrong param count") );
}

//==================================================================
static RtToken matchToken( const char *pStr, RtToken pAllowedTokens[] )
{
	for (int i=0; pAllowedTokens[i] != 0; ++i)
	{
		// $$$ maybe case insesitive ?
		if ( 0 == strcmp( pStr, pAllowedTokens[i] ) )
			return pAllowedTokens[i];
	}
	
	return NULL;
}

//==================================================================
void Machine::AddCommand(	const DStr	&cmdName,
							ParamList	&cmdParams )
{
	const DStr	&nm = cmdName;
	ParamList	&p = cmdParams;

	static RtToken tlSolidBegin[]		= { RI_PRIMITIVE, RI_INTERSECTION, RI_UNION, RI_DIFFERENCE, 0 };
	static RtToken tlGeometricApproximation[]	= { RI_FLATNESS, 0 };
	static RtToken tlOrientation[]	= { RI_OUTSIDE, RI_INSIDE, RI_LH, RI_RH, 0 };
	static RtToken tlBasis[]			= { RI_BEZIERBASIS, RI_BSPLINEBASIS, RI_POWERBASIS, RI_CATMULLROMBASIS, RI_HERMITEBASIS, 0 };
	static RtToken tlPatch0[]			= { RI_BILINEAR, RI_BICUBIC, 0 };

	if ( nm == "Begin" )			{ exN( 1, p ); mState.Begin( p[0] );		}	else
	if ( nm == "End" )				{ exN( 0, p ); mState.End();				}	else
	if ( nm == "FrameBegin" )		{ exN( 1, p ); mState.FrameBegin( p[0] );	}	else
	if ( nm == "FrameEnd" )			{ exN( 0, p ); mState.FrameEnd();			}	else
	if ( nm == "WorldBegin" )		{ exN( 0, p ); mState.WorldBegin();			}	else
	if ( nm == "WorldEnd" )			{ exN( 0, p ); mState.WorldEnd();			}	else
	if ( nm == "AttributeBegin" )	{ exN( 0, p ); mState.AttributeBegin();		}	else
	if ( nm == "AttributeEnd" )		{ exN( 0, p ); mState.AttributeEnd();		}	else
	if ( nm == "TransformBegin" )	{ exN( 0, p ); mState.TransformBegin();		}	else
	if ( nm == "TransformEnd" )		{ exN( 0, p ); mState.TransformEnd();		}	else
	if ( nm == "SolidBegin" )		{ exN( 1, p ); mState.SolidBegin( matchToken( p[0], tlSolidBegin ) );	}	else
	if ( nm == "SolidEnd" )			{ exN( 0, p ); mState.SolidEnd();			}	else
	if ( nm == "ObjectBegin" )		{ exN( 0, p ); mState.ObjectBegin();		}	else
	if ( nm == "ObjectEnd" )		{ exN( 0, p ); mState.ObjectEnd();			}	else
	if ( nm == "MotionBegin" )		{ exN( 2, p ); mState.MotionBegin( p[0], p[0].PFlt() ); } else
	if ( nm == "MotionEnd" )		{ exN( 0, p ); mState.MotionEnd();			}	else
	// attributes
	if ( nm == "Bound" )			{ Bound b; mkBound( b, p ); mState.DoBound( b );	}	else
	if ( nm == "Detail" )			{ Bound b; mkBound( b, p ); mState.Detail( b ); }	else
	if ( nm == "DetailRange" )		{ exN( 4, p ); mState.DetailRange(p[0],p[1],p[2],p[3]);	}	else
	if ( nm == "GeometricApproximation" )
									{
										//exN( 2, p ); 
										////mState.GeometricApproximation( matchToken( p[0], tlGeometricApproximation ), p[1] );
										//mState.GeometricApproximation( p[0], p[1] );
									
									}	else
	if ( nm == "Orientation" )		{ exN( 1, p ); mState.Orientation( matchToken( p[0], tlOrientation ) );	}	else
	if ( nm == "Sides" )			{ exN( 1, p ); mState.Sides( p[0] );		}	else
	if ( nm == "Basis" )			{
		exN( 4, p );

		RtToken		pUName = NULL;
		RtToken		pVName = NULL;
		const float	*pUCustom = NULL;
		const float	*pVCustom = NULL;
		
		if ( p[0].IsString() )	pUName = matchToken( p[0], tlBasis );
		else					pUCustom = p[0].PFlt(16);

		if ( p[2].IsString() )	pVName = matchToken( p[2], tlBasis );
		else					pVCustom = p[2].PFlt(16);

		mState.Basis(
			pUName,
			pUCustom,
			p[1],
			pVName,
			pVCustom,
			p[3]
			);

	}
	else
	if ( nm == "LightSource" )
	{
		mState.LightSource( p );
		//exN( 1, p );
		//mState.FrameAspectRatio( p[0] );
	}
	else
	if ( nm == "Surface" )
	{
		mState.Surface( p );
	}
	else
	// options
	if ( nm == "Format" )
	{
		exN( 3, p );

		if ( mForcedWd > 0 && mForcedHe > 0 )
			mState.Format( mForcedWd, mForcedHe, p[2] );
		else
			mState.Format( p[0], p[1], p[2] );
	}
	else
	if ( nm == "FrameAspectRatio" )	{ exN( 1, p ); mState.FrameAspectRatio( p[0] );	}	else
	if ( nm == "ScreenWindow" )		{ exN( 4, p ); mState.ScreenWindow(		p[0], p[1], p[2], p[3] );	}	else
	if ( nm == "CropWindow" )		{ exN( 4, p ); mState.CropWindow(		p[0], p[1], p[2], p[3] );	}	else
	if ( nm == "Projection" )		{ geN( 1, p ); mState.Projection(	p );	}	else
	if ( nm == "Clipping" )			{ exN( 2, p ); mState.Clipping(		p[0], p[1] );	}	else
	if ( nm == "DepthOfField" )		{ exN( 3, p ); mState.DepthOfField(	p[0], p[1], p[2] );	}	else
	if ( nm == "Shutter" )			{ exN( 2, p ); mState.Shutter(		p[0], p[1] );	}	else
	if ( nm == "Color"	)			{ exN( 1, p ); mState.ColorSet(		p[0].PFlt(3) );	}	else
	if ( nm == "Opacity" )			{ exN( 1, p ); mState.Opacity(		p[0].PFlt(3) );	}	else
	
	
	// transformations
	if ( nm == "Identity" )			{ exN( 0, p ); mState.Identity();							}	else
	if ( nm == "ConcatTransform" )	{ exN( 1, p ); mState.ConcatTransform(	p[0].PFlt(16) );	}	else
	if ( nm == "Transform" )		{ exN( 1, p ); mState.TransformCmd(	p[0].PFlt(16) );	}	else
	if ( nm == "Scale" )			{ exN( 3, p ); mState.Scale(		p[0], p[1], p[2] );	}	else
	if ( nm == "Rotate" )			{ exN( 4, p ); mState.Rotate(		p[0], p[1], p[2], p[3] ); }	else
	if ( nm == "Translate" )		{ exN( 3, p ); mState.Translate(	p[0], p[1], p[2] );	}	else

	// primitives
	if ( nm == "Cone" )				{ exN( 3, p ); mState.Cone(			p[0], p[1], p[2] ); }	else
	if ( nm == "Cylinder" )			{ exN( 4, p ); mState.Cylinder(		p[0], p[1], p[2], p[3] ); }	else
	if ( nm == "Sphere" )			{ exN( 4, p ); mState.Sphere(		p[0], p[1], p[2], p[3] ); }	else
	if ( nm == "Hyperboloid" )		{ exN( 7, p ); mState.Hyperboloid(	Vector3( p[0], p[1], p[2] ),
																		Vector3( p[3], p[4], p[5] ),
																		p[6] ); }	else
	if ( nm == "Paraboloid" )		{ exN( 4, p ); mState.Paraboloid(	p[0], p[1], p[2], p[3] ); }	else
	if ( nm == "Torus" )			{ exN( 5, p ); mState.Torus(		p[0], p[1], p[2], p[3], p[4] ); }	else
	if ( nm == "Patch" )			{ geN( 3, p ); mState.Patch(		matchToken( p[0], tlPatch0 ), p ); }	else
	if ( nm == "PatchMesh" )		{ geN( 5, p ); mState.PatchMesh(	matchToken( p[0], tlPatch0 ), p ); }	else

	if ( nm == "Polygon" )				{ geN( 2, p ); mState.Polygon( p ); }	else
	if ( nm == "PointsGeneralPolygons" ){ geN( 5, p ); mState.PointsGeneralPolygons( p ); }	else
	// unknown
									{ unknownCommand( nm.c_str() ); }

}

//==================================================================
}