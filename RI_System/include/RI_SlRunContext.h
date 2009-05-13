//==================================================================
/// RI_SlRunContext.h
///
/// Created by Davide Pasca - 2009/2/20
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLRUNCONTEXT_H
#define RI_SLRUNCONTEXT_H

#include "RI_SlShader.h"

//==================================================================
namespace RI
{

class Attributes;
class SymbolList;

//==================================================================
/// SlRunContext
//==================================================================
class SlRunContext
{
	size_t					mMaxPointsN;

public:
	u_int					mProgramCounter;
	u_int					mSIMDCount;
	u_int					mSIMDBlocksN;
	int						*mpSIMDFlags;
	SlValue					*mpDataSegment;
	const SlShaderInstance	*mpShaderInst;
	const SlSymbolList		*mpSymbols;
	const Attributes		*mpAttribs;

	class Cache
	{
	public:
		bool	mAmbientColDone;
		SlColor	mAmbientCol;

		Cache()
		{
			mAmbientColDone = false;
		}
	} mCache;

	SlRunContext( const SlSymbolList &symbols, size_t maxPointsN );
	~SlRunContext();

	void Setup( const Attributes &attribs, size_t pointsN );

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
		u_int	tableOff = GetOp(argc)->mSymbol.mTableOffset;
		return mpDataSegment[tableOff];
	}
	const SlValue	&GetValue( u_int argc ) const
	{
		u_int	tableOff = GetOp(argc)->mSymbol.mTableOffset;
		return mpDataSegment[tableOff];
	}

	SlSymbol &GetSymbol( u_int argc )
	{
		u_int	tableOff = GetOp(argc)->mSymbol.mTableOffset;
		return mpShaderInst->mpShader->mSymbols[tableOff ];
	}

	bool IsSymbolVarying( u_int argc ) const
	{
		return GetOp(argc)->mSymbol.mIsVarying;
	}

	int GetSymbolVaryingStep( u_int argc ) const
	{
		return GetOp(argc)->mSymbol.mIsVarying ? 1 : 0;
	}

	void		*GetVoid  ( u_int argc ) { return GetValue(argc).Data.pVoidValue;	}
	//float		*GetFloat ( u_int argc ) { return GetValue(argc).Data.pFloatValue;	}
	//Vec3f		*GetColor ( u_int argc ) { return GetValue(argc).Data.pColorValue;	}
	//Matrix44	*GetMatrix( u_int argc ) { return GetValue(argc).Data.pMatrixValue;	}
	// no non-constant string

	const void		*GetVoid  ( u_int argc ) const { return GetValue(argc).Data.pVoidValue;	}
	//const float		*GetFloat ( u_int argc ) const { return GetValue(argc).Data.pFloatValue;	}
	//const Vec3f		*GetColor ( u_int argc ) const { return GetValue(argc).Data.pColorValue;	}
	//const Matrix44	*GetMatrix( u_int argc ) const { return GetValue(argc).Data.pMatrixValue;	}
	//const char		*GetString( u_int argc ) const { return GetValue(argc).Data.pStringValue;	}

	void InitializeSIMD( size_t samplesN );
	
	//bool IsProcessorActive( u_int i ) const { return mpSIMDFlags[i] == 0; }
	bool IsProcessorActive( u_int i ) const { return true; }
	void EnableProcessor( u_int i )			{ mpSIMDFlags[i] -= 1; }
	void DisableProcessor( u_int i )		{ mpSIMDFlags[i] += 1; }
	u_int GetProcessorsN() const			{ return mSIMDCount; }
	void NextInstruction()					{ mProgramCounter += GetOpCount() + 1; }
};

//==================================================================
}

#endif
