/*
 *  RI_Attributes.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
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
	TokenManager	*mpTManager;

public:
	Attributes();
	void Init( TokenManager *pTManager );

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
	void cmdGeometricApproximation(RtToken typeApproximation,
								   float valueApproximation );
	
	void cmdOrientation( RtToken orientation );
	void cmdSides( int sides );
	void cmdBasis( RtToken ubasis, int ustep, RtToken vbasis, int vstep );

public:
	//==================================================================
	BoundType	mBound;				// Bound()
	BoundType	mDetail;			// Detail()

	float		mMinVisible;		// DetailRange()
	float		mLowerTransition;	//
	float		mUpperTransition;	//
	float		mMaxVisible;		//
	
	cpTokenVoid	mpTypeApproximationTok;	// GeometricApproximation()
	float		mValueApproximation;
	
	cpTokenVoid	mpOrientationTok;		// Orientation()
	int			mSides;				// Sides()
	
	cpTokenBasis	mpUBasisTok;			// Basis()
	cpTokenBasis	mpVBasisTok;
	int			mUSteps;
	int			mVSteps;
};

//==================================================================
}


#endif