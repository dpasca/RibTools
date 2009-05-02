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
	OP_MOVSS,
	OP_MOVVS,
	OP_MOVVV,
	OP_ADDSS,
	OP_ADDVS,
	OP_ADDVV,
	OP_SUBSS,
	OP_SUBVS,
	OP_SUBVV,
	OP_MULSS,
	OP_MULVS,
	OP_MULVV,
	OP_DIVSS,
	OP_DIVVS,
	OP_DIVVV,
	OP_NORMALIZE,
	OP_FACEFORWARD,
	OP_DIFFUSE,
	OP_AMBIENT,
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
		Vec3		*pVectorValue;
		Vec3		*pNormalValue;
		Vec3		*pColorValue;
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
	
	struct CtorParams
	{
		const char	*pName;
		const char	*pSource;
		const char	*pSourceFileName;
		
		CtorParams() :
			pName(NULL),
			pSource(NULL),
			pSourceFileName(NULL)
		{
		}
	};
	
	SlShader( const CtorParams &params );
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
	~SlShaderInstance() {}
	
	void SetParameter(
				const char		*pParamName,
				SlSymbol::Type	type,
				bool			isVarying,
				void			*pValue );

	SlValue	*Bind( MicroPolygonGrid &g );
	void Unbind( SlValue * &pDataSegment );
	
	void Run( MicroPolygonGrid &g, const Attributes *pAttribs );
};

//==================================================================
}

#endif
