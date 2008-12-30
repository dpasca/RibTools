/*
 *  RI_Attributes.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RI_ATTRIBUTES_H
#define RI_ATTRIBUTES_H

#include "RI_Base.h"
#include "RI_Tokens.h"

//==================================================================
namespace RI
{

//==================================================================
struct Color
{
	union {
		float	rgb[3];

		struct {
			float	r, g, b;
		} col;
	};
};

//==================================================================
/// Attributess
//==================================================================
class Attributes
{
public:
	Attributes() :
		mBound(RI_INFINITY),			// Bound()
		mDetail(RI_INFINITY),			// Detail()

		mMinVisible(0),					// DetailRange()
		mLowerTransition(0),			//
		mUpperTransition(RI_INFINITY),	//
		mMaxVisible(RI_INFINITY),		//

		mTypeApproximation(RI_EMPTY_TOKEN),	// GeometricApproximation()
		mValueApproximation(0),

		mOrientation(RI_OUTSIDE),		// Orientation()
		mSides(2)						// Sides()
	{
	}

	Attributes( const Attributes &attributes )	{ *this = attributes; }

	~Attributes()
	{
	}

/*
	//==================================================================
	Attributes &operator=(const Attributes &attributes)
	{
		*this = attributes;
	}
*/

	void cmdBound( const BoundType &bound );
	void cmdDetail( const BoundType &detail );
	void cmdDetailRange(float	minVisible,
						float	lowerTransition,
						float	upperTransition,
						float	maxVisible );
	void cmdGeometricApproximation(Token typeApproximation,
								   float valueApproximation );
	
	void cmdOrientation( Token orientation );
	void cmdSides( int sides );

private:
	//==================================================================
	BoundType	mBound;			// Bound()
	BoundType	mDetail;		// Detail()

	float	mMinVisible;		// DetailRange()
	float	mLowerTransition;	//
	float	mUpperTransition;	//
	float	mMaxVisible;		//
	
	Token	mTypeApproximation;	// GeometricApproximation()
	float	mValueApproximation;
	
	Token	mOrientation;		// Orientation()
	int		mSides;				// Sides()
};

//==================================================================
}


#endif