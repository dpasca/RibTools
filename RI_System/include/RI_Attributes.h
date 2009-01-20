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
class Attributes
{
	SymbolList	*mpStatics;

public:
	Attributes();
	Attributes( const Attributes &attributes );

	void Init( SymbolList *pStatics, RevisionTracker *pRevision );

	~Attributes();

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
	void cmdBasis(RtToken ubasis, const float *pCustomUBasis, int ustep,
				  RtToken vbasis, const float *pCustomVBasis, int vstep );
				  
	const RtBasis &GetUBasis() const { return mpyUBasis ? mpyUBasis->value : *mpCustomUBasis; }
	const RtBasis &GetVBasis() const { return mpyVBasis ? mpyVBasis->value : *mpCustomVBasis; }

public:
	RevisionTracker	*mpRevision;

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
	const RtBasis *mpCustomUBasis;
	const RtBasis *mpCustomVBasis;
	
	int			mUSteps;
	int			mVSteps;
};

//==================================================================
}


#endif