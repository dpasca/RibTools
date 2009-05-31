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
	static const u_int		MAX_PROG_STACK = 32;

	size_t					mMaxPointsN;

public:
	u_int					mProgramCounterIdx;
	u_int					mProgramCounter[MAX_PROG_STACK];

public:
	u_int					mBlocksXN;
	u_int					mPointsYN;
	u_int					mPointsN;
	u_int					mBlocksN;
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

	void Setup(
			const Attributes &attribs,
			const SlShaderInstance *pShaderInst,
			u_int blocksXN,
			u_int pointsYN,
			size_t pointsN );

	SlCPUWord *GetOp( u_int argc )
	{
		return &mpShaderInst->mpShader->mCode[mProgramCounter[mProgramCounterIdx] + argc];
	}
	const SlCPUWord *GetOp( u_int argc ) const
	{
		return &mpShaderInst->mpShader->mCode[mProgramCounter[mProgramCounterIdx] + argc];
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
		return *mpShaderInst->mpShader->mSymbols[tableOff ];
	}

	bool IsSymbolVarying( u_int argc ) const
	{
		return GetOp(argc)->mSymbol.mIsVarying;
	}

	int GetSymbolVaryingStep( u_int argc ) const
	{
		return GetOp(argc)->mSymbol.mIsVarying ? 1 : 0;
	}

	template <class _T>
	_T *GetVoidRW( u_int argc )
	{
		return 0;
	}

	template <class _T>
	const _T *GetVoidRO( u_int argc ) const
	{
		return 0;
	}

	template <>
	SlScalar *GetVoidRW( u_int argc )
	{
		SlValue	&value = GetValue(argc);

		DASSERT( value.mpSrcSymbol->mType == SlSymbol::FLOAT &&
				 value.Flags.mCanChange != 0 );

		return (SlScalar *)value.Data.pVoidValue;
	}

	template <>
	const SlScalar *GetVoidRO( u_int argc ) const
	{
		const SlValue	&value = GetValue(argc);
		DASSERT( value.mpSrcSymbol->mType == SlSymbol::FLOAT );
		return (const SlScalar *)value.Data.pVoidValue;
	}

	template <>
	SlVec3 *GetVoidRW( u_int argc )
	{
		SlValue	&value = GetValue(argc);

		DASSERT(
			(value.mpSrcSymbol->mType == SlSymbol::POINT ||
			 value.mpSrcSymbol->mType == SlSymbol::COLOR ||
			 value.mpSrcSymbol->mType == SlSymbol::VECTOR ||
			 value.mpSrcSymbol->mType == SlSymbol::NORMAL) &&
			 value.Flags.mCanChange != 0 );

		return (SlVec3 *)value.Data.pVoidValue;
	}

	template <>
	const SlVec3 *GetVoidRO( u_int argc ) const
	{
		const SlValue	&value = GetValue(argc);

		DASSERT(
			(value.mpSrcSymbol->mType == SlSymbol::POINT ||
			 value.mpSrcSymbol->mType == SlSymbol::COLOR ||
			 value.mpSrcSymbol->mType == SlSymbol::VECTOR ||
			 value.mpSrcSymbol->mType == SlSymbol::NORMAL) );

		return (const SlVec3 *)value.Data.pVoidValue;
	}

	void InitializeSIMD( size_t samplesN );
	
	//bool IsProcessorActive( u_int i ) const { return mpSIMDFlags[i] == 0; }
	bool IsProcessorActive( u_int i ) const { return true; }
	void EnableProcessor( u_int i )			{ mpSIMDFlags[i] -= 1; }
	void DisableProcessor( u_int i )		{ mpSIMDFlags[i] += 1; }
	u_int GetProcessorsN() const			{ return mPointsN; }
	void NextInstruction()					{ mProgramCounter[mProgramCounterIdx] += GetOpCount() + 1; }
};

//==================================================================
}

#endif
