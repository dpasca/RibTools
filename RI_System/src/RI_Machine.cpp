/*
 *  RI_Machine.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "RI_Machine.h"

//==================================================================
namespace RI
{

//==================================================================
/// 
//==================================================================
Machine::Machine()
{
}

//==================================================================
void Machine::unknownCommand( const char *pCmdName )
{
	printf( "Unknown command %s !\n", pCmdName );
}

//==================================================================
static void exN(size_t n,
				const ParamList	&cmdParams )
{
	if ( n != cmdParams.size() )
		DASSTHROW( false, "Wrong param count" );
}

//==================================================================
static void mkBound( BoundType &out_Bound, ParamList &cmdParams )
{
	if ( cmdParams.size() == 1 )
	{
		const float *pSrc = cmdParams[0].PFlt(6);

		for (size_t i=0; i < 6; ++i)
			out_Bound.mBound[i] = pSrc[i];
	}
	else
	if ( cmdParams.size() == 6 )
	{
		for (size_t i=0; i < 6; ++i)
			out_Bound.mBound[i] = cmdParams[i].u.floatVal;
	}
	else
		DASSTHROW( false, "Wrong param count" );
}

//==================================================================
static Token matchToken( const char *pStr, Token pAllowedTokens[] )
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

	static Token tlSolidBegin[]		= { RI_PRIMITIVE, RI_INTERSECTION, RI_UNION, RI_DIFFERENCE, 0 };
	static Token tlGeometricApproximation[]	= { RI_FLATNESS, 0 };
	static Token tlOrientation[]	= { RI_OUTSIDE, RI_INSIDE, RI_LH, RI_RH, 0 };
	static Token tlBasis[]			= { RI_BEZIERBASIS, RI_BSPLINEBASIS, RI_POWERBASIS, RI_CATMULLROMBASIS, RI_HERMITEBASIS, 0 };

	if ( nm == "Begin" )			{ exN( 1, p ); mState.Begin( p[0].PChar() );	}	else
	if ( nm == "End" )				{ exN( 0, p ); mState.End();					}	else
	if ( nm == "FrameBegin" )		{ exN( 1, p ); mState.FrameBegin( p[0].Int() );	}	else
	if ( nm == "FrameEnd" )			{ exN( 0, p ); mState.FrameEnd();			}	else
	if ( nm == "WorldBegin" )		{ exN( 0, p ); mState.WorldBegin();			}	else
	if ( nm == "WorldEnd" )			{ exN( 0, p ); mState.WorldEnd();			}	else
	if ( nm == "AttributeBegin" )	{ exN( 0, p ); mState.AttributeBegin();		}	else
	if ( nm == "AttributeEnd" )		{ exN( 0, p ); mState.AttributeEnd();		}	else
	if ( nm == "TransformBegin" )	{ exN( 0, p ); mState.TransformBegin();		}	else
	if ( nm == "TransformEnd" )		{ exN( 0, p ); mState.TransformEnd();		}	else
	if ( nm == "SolidBegin" )		{ exN( 1, p ); mState.SolidBegin( matchToken( p[0].PChar(), tlSolidBegin ) );	}	else
	if ( nm == "SolidEnd" )			{ exN( 0, p ); mState.SolidEnd();			}	else
	if ( nm == "ObjectBegin" )		{ exN( 0, p ); mState.ObjectBegin();			}	else
	if ( nm == "ObjectEnd" )		{ exN( 0, p ); mState.ObjectEnd();			}	else
	if ( nm == "MotionBegin" )		{ exN( 2, p ); mState.MotionBegin( p[0].Int(), p[0].PFlt() ); } else
	if ( nm == "MotionEnd" )		{ exN( 0, p ); mState.MotionEnd();			}	else
	// attributes
	if ( nm == "Bound" )			{ BoundType b; mkBound( b, p ); mState.Bound( b );	}	else
	if ( nm == "Detail" )			{ BoundType b; mkBound( b, p ); mState.Detail( b ); }	else
	if ( nm == "DetailRange" )		{ exN( 4, p ); mState.DetailRange(p[0].Flt(),p[1].Flt(),p[2].Flt(),p[3].Flt());	}	else
	if ( nm == "GeometricApproximation" )
									{ exN( 2, p ); mState.GeometricApproximation( matchToken( p[0].PChar(), tlGeometricApproximation ), p[0].Flt() );	}	else
	if ( nm == "Orientation" )		{ exN( 1, p ); mState.Orientation( matchToken( p[0].PChar(), tlOrientation ) );	}	else
	if ( nm == "Sides" )			{ exN( 1, p ); mState.Sides( p[0].Int() );		}	else
	if ( nm == "Basis" )			{
		exN( 4, p );
		mState.Basis(
			matchToken( p[0].PChar(), tlBasis ),
			p[1].Int(),
			matchToken( p[2].PChar(), tlBasis ),
			p[3].Int()
			);
	}	else

	// options
	if ( nm == "Format" )			{ exN( 3, p ); mState.Format(			p[0].Int(), p[1].Int(), p[2].Flt() );	}	else
	if ( nm == "FrameAspectRatio" )	{ exN( 1, p ); mState.FrameAspectRatio( p[0].Flt() );	}	else
	if ( nm == "ScreenWindow" )		{ exN( 4, p ); mState.ScreenWindow(		p[0].Flt(), p[1].Flt(), p[2].Flt(), p[3].Flt() );	}	else
	if ( nm == "CropWindow" )		{ exN( 4, p ); mState.CropWindow(		p[0].Flt(), p[1].Flt(), p[2].Flt(), p[3].Flt() );	}	else
	if ( nm == "Projection" )		{ /*exN( 1, p );*/ mState.Projection(	p );	}	else
	if ( nm == "Clipping" )			{ exN( 2, p ); mState.Clipping(		p[0].Flt(), p[1].Flt() );	}	else
	if ( nm == "DepthOfField" )		{ exN( 3, p ); mState.DepthOfField(	p[0].Flt(), p[1].Flt(), p[2].Flt() );	}	else
	if ( nm == "Shutter" )			{ exN( 2, p ); mState.Shutter(		p[0].Flt(), p[1].Flt() );	}	else
	// transformations
	if ( nm == "Identity" )			{ exN( 0, p ); mState.Identity();							}	else
	if ( nm == "Transform" )		{ exN( 1, p ); mState.TransformCmd(	p[0].PFlt(16) );	}	else
	if ( nm == "Scale" )			{ exN( 3, p ); mState.Scale(		p[0].Flt(), p[1].Flt(), p[2].Flt() );	}	else
	if ( nm == "Rotate" )			{ exN( 4, p ); mState.Rotate(		p[0].Flt(), p[1].Flt(), p[2].Flt(), p[3].Flt() ); }	else
	if ( nm == "Translate" )		{ exN( 3, p ); mState.Translate(	p[0].Flt(), p[1].Flt(), p[2].Flt() );	}	else

	// primitives
	if ( nm == "Cone" )				{ exN( 3, p ); mState.Cone(			p[0].Flt(), p[1].Flt(), p[2].Flt() ); }	else
	if ( nm == "Cylinder" )			{ exN( 4, p ); mState.Cylinder(		p[0].Flt(), p[1].Flt(), p[2].Flt(), p[3].Flt() ); }	else
	if ( nm == "Sphere" )			{ exN( 4, p ); mState.Sphere(		p[0].Flt(), p[1].Flt(), p[2].Flt(), p[3].Flt() ); }	else
	if ( nm == "Hyperboloid" )		{ exN( 7, p ); mState.Hyperboloid(	Vector3( p[0].Flt(), p[1].Flt(), p[2].Flt() ),
																		Vector3( p[3].Flt(), p[4].Flt(), p[5].Flt() ),
																		p[6].Flt() ); }	else
	if ( nm == "Paraboloid" )		{ exN( 4, p ); mState.Paraboloid(	p[0].Flt(), p[1].Flt(), p[2].Flt(), p[3].Flt() ); }	else
	if ( nm == "Torus" )			{ exN( 5, p ); mState.Torus(		p[0].Flt(), p[1].Flt(), p[2].Flt(), p[3].Flt(), p[4].Flt() ); }	else

	// unknown
									{ unknownCommand( nm.c_str() ); }

}

//==================================================================
}