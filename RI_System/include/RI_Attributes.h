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
#include "RI_Symbol.h"

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
class Attributes : public RevisionTracker
{
	SymbolList	*mpStatics;

public:
	Attributes();
	void Init( SymbolList *pTManager );

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

	void cmdBound( const Bound &bound );
	void cmdDetail( const Bound &detail );
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
	SymbolList	mSymbols;

	Bound		mBound;				// DoBound()
	Bound		mDetail;			// Detail()

	float		mMinVisible;		// DetailRange()
	float		mLowerTransition;	//
	float		mUpperTransition;	//
	float		mMaxVisible;		//
	
	CPSymVoid	mpyTypeApproximation;	// GeometricApproximation()
	float		mValueApproximation;
	
	CPSymVoid	mpyOrientation;		// Orientation()
	int			mSides;				// Sides()
	
	CPSymBasis	mpyUBasis;			// Basis()
	CPSymBasis	mpyVBasis;
	int			mUSteps;
	int			mVSteps;
};

//==================================================================
}


#endif