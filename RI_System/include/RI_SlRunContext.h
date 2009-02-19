/*
 *  RI_SlRunContext.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/20.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_SLRUNCONTEXT_H
#define RI_SLRUNCONTEXT_H

#include "RI_SlShader.h"

//==================================================================
namespace RI
{

//==================================================================
/// SlRunContext
//==================================================================
class SlRunContext
{
public:
	u_int				mProgramCounter;
	u_int				mSIMDCount;
	int					*mpSIMDFlags;
	SlValue				*mpDataSegment;
	SlShaderInstance	*mpShaderInst;

	SlRunContext()
	{
		mProgramCounter = 0;
		mSIMDCount		= 0;
		mpSIMDFlags		= 0;
		mpDataSegment	= 0;
		mpShaderInst	= 0;
	}

	SlCPUWord *GetOp( u_int argc )
	{
		return &mpShaderInst->mpShader->mCode[mProgramCounter + argc];
	}
	const SlCPUWord *GetOp( u_int argc ) const
	{
		return &mpShaderInst->mpShader->mCode[mProgramCounter + argc];
	}
	
	u_int	GetOpCount() const
	{
		return GetOp(0)->mOpCode.mOperandCount;
	}

	SlValue			&GetValue( u_int argc )
	{
		return mpDataSegment[GetOp(argc)->mSymbol.mTableOffset];
	}
	const SlValue	&GetValue( u_int argc ) const
	{
		return mpDataSegment[GetOp(argc)->mSymbol.mTableOffset];
	}
	
	SlSymbol &GetSymbol( u_int argc )
	{
		return mpShaderInst->mpShader->mSymbols[
						GetOp(argc)->mSymbol.mTableOffset ];
	}

	bool IsSymbolVarying( u_int argc ) const
	{
		return GetOp(argc)->mSymbol.mIsVarying;
	}

	float		*GetFloat ( u_int argc ) { return GetValue(argc).Data.pFloatValue;	}
	Point3		*GetPoint ( u_int argc ) { return GetValue(argc).Data.pPointValue;	}
	Vector3		*GetVector( u_int argc ) { return GetValue(argc).Data.pVectorValue;	}
	Vector3		*GetNormal( u_int argc ) { return GetValue(argc).Data.pNormalValue;	}
	Vector3		*GetColor ( u_int argc ) { return GetValue(argc).Data.pColorValue;	}
	Matrix44	*GetMatrix( u_int argc ) { return GetValue(argc).Data.pMatrixValue;	}
	// no non-constant string
	void		*GetVoid  ( u_int argc ) { return GetValue(argc).Data.pVoidValue;	}

	const float		*GetFloat ( u_int argc ) const { return GetValue(argc).Data.pFloatValue;	}
	const Point3	*GetPoint ( u_int argc ) const { return GetValue(argc).Data.pPointValue;	}
	const Vector3	*GetVector( u_int argc ) const { return GetValue(argc).Data.pVectorValue;	}
	const Vector3	*GetNormal( u_int argc ) const { return GetValue(argc).Data.pNormalValue;	}
	const Vector3	*GetColor ( u_int argc ) const { return GetValue(argc).Data.pColorValue;	}
	const Matrix44	*GetMatrix( u_int argc ) const { return GetValue(argc).Data.pMatrixValue;	}
	const char		*GetString( u_int argc ) const { return GetValue(argc).Data.pStringValue;	}
	const void		*GetVoid  ( u_int argc ) const { return GetValue(argc).Data.pVoidValue;	}
	
	void InitializeSIMD( MicroPolygonGrid &g );
	
	bool IsProcessorActive( u_int i ) const { return mpSIMDFlags[i] == 0; }
	void EnableProcessor( u_int i )			{ mpSIMDFlags[i] -= 1; }
	void DisableProcessor( u_int i )		{ mpSIMDFlags[i] += 1; }
	u_int GetProcessorsN() const			{ return mSIMDCount; }
	void NextInstruction()					{ mProgramCounter += GetOpCount() + 1; }
};

//==================================================================
}

#endif
