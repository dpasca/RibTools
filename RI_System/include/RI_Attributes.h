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
#include "RI_Param.h"
#include "RI_Tokens.h"
#include "RI_Symbol.h"
#include "RI_SlShader.h"
#include "RI_LightSource.h"

//==================================================================
namespace RI
{

//==================================================================
/// Attributess
//==================================================================
class Attributes
{
	SymbolList	*mpStatics;

public:
	Attributes();
	Attributes( const Attributes &attributes );

	const Attributes& operator=(const Attributes& rhs);

	void Init( SymbolList *pStatics, ResourceManager *pResManager, RevisionTracker *pRevision );

	~Attributes();

private:
	void copyFrom(const Attributes& rhs);

public:
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
				  
	const RtBasis &GetUBasis() const { return mpCustomUBasis ? *mpCustomUBasis : mpyUBasis->value; }
	const RtBasis &GetVBasis() const { return mpCustomVBasis ? *mpCustomVBasis : mpyVBasis->value; }

	void cmdColor( const Color &color );
	void cmdOpacity( const Color &color );
	bool cmdLightSource( ParamList &params );

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
	
	Color				mColor;
	Color				mOpacity;
	SlShaderInstance	mShaderInstance;
};

//==================================================================
}


#endif