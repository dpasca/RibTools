/*
 *  RI_State.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_STATE_H
#define RI_STATE_H

#include "RI_Base.h"
#include "DContainers.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Framework.h"
#include "RI_Primitive.h"

//==================================================================
namespace RI
{

//==================================================================
/*


Options
*	WorldBegin/End
*	FrameBegin/End
	
Attributes
*	AttributeBegin/End
*	WorldBegin/End
*	FrameBegin/End
*	SolidBegin/End
*	ObjectBegin/End

Transform
*	TransformBegin/End
*	AttributeBegin/End
*	WorldBegin/End
*	FrameBegin/End
*	SolidBegin/End
*	ObjectBegin/End

*/

//==================================================================
//
//==================================================================
class State
{
	Stack<Mode>				mModeStack;
	CopyStack<Options>		mOptionsStack;
	CopyStack<Attributes>	mAttributesStack;
	CopyStack<Transform>	mTransformOpenStack;
	CopyStack<Transform>	mTransformCloseStack;

	Framework				mFramework;

	enum OpType
	{
		OPTYPE_OPTS,
		OPTYPE_ATRB,
		OPTYPE_PRIM,
		OPTYPE_STD_XFORM,	// excludes CoordinateSystem and TransformPoints
	};

public:
	State();
	~State();

	void	Begin( Token name );
	void	End();

	void	FrameBegin( int frame );
	void	FrameEnd();
	void	WorldBegin();
	void	WorldEnd();
	void	AttributeBegin();
	void	AttributeEnd();
	void	TransformBegin();
	void	TransformEnd();
	void	SolidBegin( Token operation );
	void	SolidEnd();
	ObjectHandle ObjectBegin();
	void	ObjectEnd();
	void	MotionBegin( int n, const float times[] );
	void	MotionEnd();

	// attributes
	void Bound( const BoundType &bound );

	void Detail( const BoundType &detail );

	void DetailRange(float	minVisible,
					 float	lowerTransition,
					 float	upperTransition,
					 float	maxVisible );
					 
	void GeometricApproximation(Token typeApproximation,
								   float valueApproximation );

	void Orientation( Token orientation );
	void Sides( int sides );

	// options
	void Format( int xRes, int yRes, float pixelRatio );
	void FrameAspectRatio( float ratio );
	void ScreenWindow( float left, float right, float bottom, float top );
	void CropWindow( float xMin, float xMax, float yMin, float yMax );
	void Projection( ParamList &params );
	void Clipping( float near, float farr );
	void DepthOfField( float fStop, float focalLength, float focalDistance );
	void Shutter( float openShutter, float closeShutter );

	// transforms
	void Identity();
	void TransformCmd( const float *pMtx );
	void Scale( float sx, float sy, float sz );
	void Rotate( float angDeg, float ax, float ay, float az );
	void Translate( float tx, float ty, float tz );

	// primitives
	void Cylinder( float radius, float zmin, float zmax, float thetamax );
	void Cone( float height, float radius, float thetamax );
	void Sphere( float radius, float zmin, float zmax, float thetamax );
	void Hyperboloid( const Vector3 &p1, const Vector3 &p2, float thetamax );
	void Paraboloid( float rmax, float zmin, float zmax, float thetamax );
	void Torus( float maxRadius, float minRadius,
			   float phimin, float phimax,
			   float thetamax );

private:
	void ErrHandler( Error errCode );
	bool checkPopMode( Mode expectedMode );
	void pushMode( Mode mode )
	{
		mModeStack.push( mode );
	}

	void popMode( Mode expectedMode )
	{
		checkPopMode( expectedMode );
	}

	enum StackFlag
	{
		SF_OPTS = 1,
		SF_ATRB = 2,
		SF_TRAN = 4
	};

	inline void pushStacks( const u_int flags );
	inline void popStacks( const u_int flags );

	bool verifyOpType( OpType optype );
	
	friend class VMaker;
};

}


#endif