/*
 *  RI_Framework.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "RI_State.h"
#include "RI_Primitive.h"

//==================================================================
namespace RI
{

//==================================================================
/// State
//==================================================================
State::State()
{
	mModeStack.push( MD_UNDEFINED );
	Begin( "dummy" );
}

//==================================================================
State::~State()
{
	End();
}

//==================================================================
bool State::checkPopMode( Mode expectedMode )
{
	if ( mModeStack.top() != expectedMode )
	{
		ErrHandler( E_NESTING );
		return false;
	}
	else
	{
		mModeStack.pop();
		return true;
	}
}

//==================================================================
bool State::verifyOpType( OpType optype )
{
	Mode	curMode = mModeStack.top();

	if ( optype == OPTYPE_OPTS )
	{
		if ( curMode != MD_BASE && curMode != MD_FRAME )
		{
			ErrHandler( E_NOTOPTIONS );
			return false;
		}
	}
	else
	if ( optype == OPTYPE_ATRB )
	{
		if ( curMode != MD_OBJECT )
		{
			ErrHandler( E_NOTATTRIBS );
			return false;
		}
	}
	else
	if ( optype == OPTYPE_PRIM )
	{
		switch( curMode )
		{
		case MD_WORLD:
		case MD_ATTRIBUTE:
		case MD_TRANSFORM:
		case MD_SOLID:
		case MD_OBJECT:
		case MD_MOTION:
			// set primitive
			break;

		default:			
			ErrHandler( E_NOTPRIMS );
			return false;
		}
	}
	else
	if ( optype == OPTYPE_STD_XFORM )
	{
		switch( curMode )
		{
		case MD_BASE:
		case MD_FRAME:
		case MD_WORLD:
		case MD_ATTRIBUTE:
		case MD_TRANSFORM:
		case MD_SOLID:
		//case MD_OBJECT:	// exclude object...
		case MD_MOTION:
			// set primitive
			break;

		default:			
			//not sure if it's the proper error for this
			ErrHandler( E_ILLSTATE );
			return false;
		}
	}

	return true;
}

//==================================================================
void State::pushStacks( const u_int flags )
{
	if ( flags & SF_OPTS )	mOptionsStack.push();
	if ( flags & SF_ATRB )	mAttributesStack.push();
	if ( flags & SF_TRAN )	mTransformOpenStack.push(), mTransformCloseStack.push();
}

//==================================================================
void State::popStacks( const u_int flags )
{
	if ( flags & SF_OPTS )	mOptionsStack.pop();
	if ( flags & SF_ATRB )	mAttributesStack.pop();
	if ( flags & SF_TRAN )	mTransformOpenStack.pop(), mTransformCloseStack.pop();
}

//==================================================================
void State::ErrHandler( Error errCode )
{
	printf( "Error %s !!\n", ErrorToString( errCode ) );
}

//==================================================================
void State::Begin( Token name )
{
	pushMode( MD_BASE );
}
//==================================================================
void State::End()
{
	popMode( MD_BASE );
}
//==================================================================
void State::FrameBegin( int frame )
{
	pushMode( MD_FRAME );
	pushStacks( SF_OPTS | SF_ATRB | SF_TRAN );
}
//==================================================================
void State::FrameEnd()
{
	popStacks( SF_OPTS | SF_ATRB | SF_TRAN );
	popMode( MD_FRAME );
}
//==================================================================
void State::WorldBegin()
{
	pushMode( MD_WORLD );
	pushStacks( SF_OPTS | SF_ATRB | SF_TRAN );

	mFramework.WorldBegin();
}
//==================================================================
void State::WorldEnd()
{
	mFramework.WorldEnd();

	popStacks( SF_OPTS | SF_ATRB | SF_TRAN );
	popMode( MD_WORLD );
}
//==================================================================
void State::AttributeBegin()
{
	pushMode( MD_ATTRIBUTE );
	pushStacks( SF_ATRB | SF_TRAN );
}
//==================================================================
void State::AttributeEnd()
{
	popStacks( SF_ATRB | SF_TRAN );
	popMode( MD_ATTRIBUTE );
}
//==================================================================
void State::TransformBegin()
{
	//printf( ">> " );
	//mTransformOpenStack.top().mMatrix.PrintOut();

	pushMode( MD_TRANSFORM );
	pushStacks( SF_TRAN );
}
//==================================================================
void State::TransformEnd()
{
	popStacks( SF_TRAN );
	popMode( MD_TRANSFORM );

	//printf( "<< " );
	//mTransformOpenStack.top().mMatrix.PrintOut();
}
//==================================================================
void State::SolidBegin( Token operation )
{
	pushMode( MD_SOLID );
	pushStacks( SF_ATRB | SF_TRAN );
}
//==================================================================
void State::SolidEnd()
{
	popStacks( SF_ATRB | SF_TRAN );
	popMode( MD_SOLID );
}
//==================================================================
ObjectHandle State::ObjectBegin()
{
	pushMode( MD_OBJECT );
	pushStacks( SF_ATRB | SF_TRAN );

	return NULL;
}
//==================================================================
void State::ObjectEnd()
{
	popStacks( SF_ATRB | SF_TRAN );
	popMode( MD_OBJECT );
}
//==================================================================
void State::MotionBegin( int n, const float times[] )
{
	pushMode( MD_MOTION );
}
//==================================================================
void State::MotionEnd()
{
	popMode( MD_MOTION );
}

// setting attributes
//==================================================================
void State::Bound( const BoundType &bound )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdBound( bound );
}
//==================================================================
void State::Detail( const BoundType &detail )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdDetail( detail );
}
//==================================================================
void State::DetailRange(float	minVisible,
				 float	lowerTransition,
				 float	upperTransition,
				 float	maxVisible )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdDetailRange(
					minVisible,
					lowerTransition,
					upperTransition,
					maxVisible );
}
//==================================================================
void State::GeometricApproximation(Token typeApproximation,
							   float valueApproximation )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdGeometricApproximation(
							typeApproximation,
							valueApproximation );
}
//==================================================================
void State::Orientation( Token orientation )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdOrientation( orientation );
}
//==================================================================
void State::Sides( int sides )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;
			
	mAttributesStack.top().cmdSides( sides );
}

// options
//==================================================================
void State::Format( int xRes, int yRes, float pixelRatio )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdFormat( xRes, yRes, pixelRatio );
}
//==================================================================
void State::FrameAspectRatio( float ratio )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdFrameAspectRatio( ratio );
}
//==================================================================
void State::ScreenWindow( float left, float right, float bottom, float top )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdScreenWindow( left, right, bottom, top );
}
//==================================================================
void State::CropWindow( float xMin, float xMax, float yMin, float yMax )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdCropWindow( xMin, xMax, yMin, yMax );
}
//==================================================================
void State::Projection( ParamList &params )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdProjection( params );
}
//==================================================================
void State::Clipping( float near, float farr )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdClipping( near, farr );
}
//==================================================================
void State::DepthOfField( float fStop, float focalLength, float focalDistance )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdDepthOfField( fStop, focalLength, focalDistance );
}
//==================================================================
void State::Shutter( float openShutter, float closeShutter )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdShutter( openShutter, closeShutter );
}

//==================================================================
void State::Identity()
{
	Matrix44 &m = mTransformOpenStack.top().mMatrix;
	m.Identity();
}

//==================================================================
void State::TransformCmd( const float *pMtx )
{
	if NOT( verifyOpType( OPTYPE_STD_XFORM ) )
		return;

	Matrix44 &m = mTransformOpenStack.top().mMatrix;
	m.CopyRowMajor( pMtx );
}

//==================================================================
void State::Scale( float sx, float sy, float sz )
{
	if NOT( verifyOpType( OPTYPE_STD_XFORM ) )
		return;

	Matrix44 &m = mTransformOpenStack.top().mMatrix;
	m = Matrix44::Scale( sx, sy, sz ) * m;
}

//==================================================================
void State::Rotate( float angDeg, float ax, float ay, float az )
{
	if NOT( verifyOpType( OPTYPE_STD_XFORM ) )
		return;

	Matrix44 &m = mTransformOpenStack.top().mMatrix;
	m = Matrix44::Rot( angDeg * DEG2RAD, ax, ay, az ) * m;
}

//==================================================================
void State::Translate( float tx, float ty, float tz )
{
	if NOT( verifyOpType( OPTYPE_STD_XFORM ) )
		return;

	Matrix44 &m = mTransformOpenStack.top().mMatrix;
	m = Matrix44::Translate( tx, ty, tz ) * m;
}

//==================================================================
void State::Cylinder( float radius, float zmin, float zmax, float thetamax )
{
	mFramework.Insert( new RI::Cylinder( radius, zmin, zmax, thetamax ),
					  mOptionsStack.top(),
					  mAttributesStack.top(),
					  mTransformOpenStack.top() );
}

//==================================================================
void State::Cone( float height, float radius, float thetamax )
{
	mFramework.Insert( new RI::Cone( height, radius, thetamax ),
					  mOptionsStack.top(),
					  mAttributesStack.top(),
					  mTransformOpenStack.top() );
}

//==================================================================
void State::Sphere( float radius, float zmin, float zmax, float thetamax )
{
	mFramework.Insert( new RI::Sphere( radius, zmin, zmax, thetamax ),
					  mOptionsStack.top(),
					  mAttributesStack.top(),
					  mTransformOpenStack.top() );
}

//==================================================================
}
