//==================================================================
/// RRL_Translator.cpp
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
/// 
//==================================================================
Translator::Translator( const Params &params ) :
	mState(params.mState),
	mParams(params)
{
}

//==================================================================
void Translator::unknownCommand( const char *pCmdName )
{
	printf( "Unknown command %s !\n", pCmdName );
}

//==================================================================
void Translator::exN( size_t n, const RI::ParamList &cmdParams )
{
	if NOT( cmdParams.size() == n )
	{
		mState.ErrHandler(
				RI::E_BADARGUMENT,
				"Expecting %i params, got %i",
				n, cmdParams.size() );
		DASSTHROW( false, ("E_BADARGUMENT") );
	}
}

//==================================================================
void Translator::geN( size_t n, const RI::ParamList &cmdParams )
{
	if NOT( cmdParams.size() >= n  )
	{
		mState.ErrHandler(
				RI::E_BADARGUMENT,
				"Expecting at least %i params, got %i",
				n, cmdParams.size() );
		DASSTHROW( false, ("E_BADARGUMENT") );
	}
}

//==================================================================
static void mkBound( RI::Bound &out_Bound, RI::ParamList &cmdParams )
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
RtToken Translator::matchToken( const char *pStr, RtToken pAllowedTokens[] )
{
	for (int i=0; pAllowedTokens[i] != 0; ++i)
	{
		if ( 0 == strcmp( pStr, pAllowedTokens[i] ) )
			return pAllowedTokens[i];
	}
	
	return NULL;
}

//==================================================================
void Translator::addFormatCmd( RI::ParamList &p )
{
	exN( 3, p );

	int	inWd = (int)p[0];
	int	inHe = (int)p[1];

	// bad case
	if ( inWd < 0 || inHe < 0 )
	{
		DASSTHROW( false, ("Invalid size in Format") );
	}

	if ( mParams.mForcedLongDim > 0 )
	{
		int newWd;
		int newHe;

		if ( inWd > inHe )
		{
			newWd = mParams.mForcedLongDim;
			newHe = mParams.mForcedLongDim * inHe / DMAX( inWd, 1 );
		}
		else
		{
			newHe = mParams.mForcedLongDim;
			newWd = mParams.mForcedLongDim * inWd / DMAX( inHe, 1 );
		}

		mState.Format( newWd, newHe, p[2] );
	}
	else
	if ( mParams.mForcedWd > 0 && mParams.mForcedHe > 0 )
	{
		mState.Format( mParams.mForcedWd, mParams.mForcedHe, p[2] );
	}
	else
		mState.Format( p[0], p[1], p[2] );
}

//==================================================================
Translator::RetCmd
	Translator::AddCommand(
					const DStr		&cmdName,
					RI::ParamList	&cmdParams )
{
	const DStr	&nm = cmdName;
	RI::ParamList	&p = cmdParams;

	static RtToken tlSolidBegin[]		= { RI_PRIMITIVE, RI_INTERSECTION, RI_UNION, RI_DIFFERENCE, 0 };
	static RtToken tlGeometricApproximation[]	= { RI_FLATNESS, 0 };
	static RtToken tlOrientation[]		= { RI_OUTSIDE, RI_INSIDE, RI_LH, RI_RH, 0 };
	static RtToken tlBasis[]			= { RI_BEZIERBASIS, RI_BSPLINEBASIS, RI_POWERBASIS, RI_CATMULLROMBASIS, RI_HERMITEBASIS, 0 };

	if ( nm == "Begin" )			{ exN( 1, p ); mState.Begin( p[0] );		}	else
	if ( nm == "End" )				{ exN( 0, p ); mState.End();				}	else
	if ( nm == "FrameBegin" )		{ exN( 1, p ); mState.FrameBegin( p[0] );	}	else
	if ( nm == "FrameEnd" )			{ exN( 0, p ); mState.FrameEnd();			}	else
	if ( nm == "WorldBegin" )		{ exN( 0, p ); mState.WorldBegin();			}	else
	if ( nm == "WorldEnd" )			{ exN( 0, p ); mState.WorldEnd();	return CMD_WORLDEND; }	else
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
	if ( nm == "Bound" )			{ RI::Bound b; mkBound( b, p ); mState.DoBound( b );	}	else
	if ( nm == "Detail" )			{ RI::Bound b; mkBound( b, p ); mState.Detail( b ); }	else
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
	if ( nm == "AreaLightSource" )
	{
		geN( 1, p );	// at least one param (the shader name)
		mState.AreaLightSource( p );
	}
	else
	if ( nm == "LightSource" )
	{
		geN( 1, p );	// at least one param (the shader name)
		mState.LightSource( p );
	}
	else
	if ( nm == "Declare" )
	{
		exN( 2, p );
		mState.Declare( p );
	}
	else
	if ( nm == "Surface" )
	{
		geN( 1, p );
		mState.Surface( p );
	}
	else
	if ( nm == "Displacement" )
	{
		geN( 1, p );
		mState.Displacement( p );
	}
	else
	{
		// primitives
		if ( addCommand_prims( nm, p ) )
			return CMD_GENERIC;

		// transformations
		if ( addCommand_transforms( nm, p ) )
			return CMD_GENERIC;

		// options
		if ( addCommand_options( nm, p ) )
			return CMD_GENERIC;

		// unknown
		unknownCommand( nm.c_str() );
	}

	return CMD_GENERIC;
}

//==================================================================
}