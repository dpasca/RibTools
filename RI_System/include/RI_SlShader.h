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
enum OperTypeID
{
	OPRTYPE_F1 ,
	OPRTYPE_F2 ,
	OPRTYPE_F3 ,
	OPRTYPE_F4 ,
	OPRTYPE_M44,
	OPRTYPE_N
};

//==================================================================
enum OpCodeID
{
	OP_MOV,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_NORMALIZE,
	OP_N
};

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
	u_short	mOperandCount;
	u_short	mDestOpType;
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
class SlShader : public ResourceBase
{
public:
	DStr			mShaderName;
	DVec<SlSymbol>	mSymbols;
	DVec<SlCPUWord>	mCode;
	
	SlShader( const char *pName, const char *pShaderSource );
};

//==================================================================
/// SlShaderInstance
//==================================================================
class SlShaderInstance
{
public:
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
	
	void Run( MicroPolygonGrid &g );
};

//==================================================================
}

#endif
