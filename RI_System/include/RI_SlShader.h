/*
 *  RI_SlShader.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/19.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_SLSHADER_H
#define RI_SLSHADER_H

#include "RI_Base.h"
#include "RI_SlSymbol.h"
#include "RI_MicroPolygonGrid.h"

//==================================================================
namespace RI
{

//==================================================================
class SlValue
{
public:
	union
	{
		float		*pFloatValue;
		Point3		*pPointValue;
		Vector3		*pVectorValue;
		Vector3		*pNormalValue;
		Vector3		*pColorValue;
		Matrix44	*pMatrixValue;
		const char	*pStringValue;
		void		*pVoidValue;
	} Data;
};

//==================================================================
struct SlOpCode
{
	u_int	mTableOffset;
	u_int	mOperandCount;
};

//==================================================================
struct SlSymbolWord
{
	u_int	mTableOffset;
	bool	mIsVarying;
};

//==================================================================
struct SlAddress
{
	u_int	mOffset;
};

//==================================================================
union SlCPUWord
{
	SlOpCode		mOpCode;
	SlSymbolWord	mSymbol;
	SlAddress		mAddress;
};

//==================================================================
/// SlShader
//==================================================================
class SlShader
{
public:
	DStr			mShaderName;
	DVec<SlSymbol>	mSymbols;
	DVec<SlCPUWord>	mCode;
};

//==================================================================
/// SlShaderInstance
//==================================================================
class SlShaderInstance
{
	SlShader			*mpShader;
	//DVec<SlParameter>	mCallingParams;
	SlSymbolList		mCallingParams;

public:
	SlShaderInstance();
	
	void SetParameter(
				const char		*pParamName,
				SlSymbol::Type	type,
				bool			isVarying,
				void			*pValue );
				
	SlValue	*Bind( MicroPolygonGrid &g );
};

//==================================================================
}

#endif
