//==================================================================
/// RI_Primitive_Base.h
///
/// Created by Davide Pasca - 2009/4/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_PRIMITIVE_BASE_H
#define RI_PRIMITIVE_BASE_H

#include "DMath/include/DMath.h"
#include "RI_Param.h"
#include "RI_Symbol.h"
#include "RI_MicroPolygonGrid.h"

//==================================================================
namespace RI
{

class Options;
class Attributes;
class Transform;
class HiderREYES;
class MicroPolygonGrid;

//==================================================================
/// PrimitiveBase
//==================================================================
class PrimitiveBase
{
	RefCount			mRefCnt;
public:
	enum Type
	{
		CONE,
		CYLINDER,
		SPHERE,
		HYPERBOLOID,
		PARABOLOID,
		TORUS,

		PATCHBILINEAR,
		PATCHBICUBIC,
		PATCHMESH,

		POLYGON,

		POINTSGENERALPOLYGONS,
	};

	Type				mType;
	Attributes			*mpAttribs;
	Transform			*mpTransform;

public:
	PrimitiveBase( Type type ) :
		mType(type),
		mpAttribs(NULL),
		mpTransform(NULL)
	{
	}

	virtual ~PrimitiveBase()
	{
	}

	PrimitiveBase *Borrow()
	{
		mRefCnt.AddRef();
		return this;
	}

	void Release()
	{
		if ( 0 == mRefCnt.SubRef() )
			delete this;
	}
	
	virtual bool	IsComplex() const = 0;
	virtual bool	IsSplitable() const = 0;

	void SetStates(
		Attributes	*pAttribs,
		Transform	*pTransform
	)
	{
		mpAttribs	= pAttribs	;
		mpTransform	= pTransform;
	}

	void CopyStates( PrimitiveBase &fromPrim )
	{
		mpAttribs	= fromPrim.mpAttribs	;
		mpTransform	= fromPrim.mpTransform;
	}
	
	bool IsUsed() const
	{
		return mRefCnt.GetCount() != 0;
	}
};

//==================================================================
/// ComplexPrimitiveBase
//==================================================================
class ComplexPrimitiveBase : public PrimitiveBase
{

public:
	ComplexPrimitiveBase( PrimitiveBase::Type type ) :
		PrimitiveBase(type)
	{
	}

	virtual ~ComplexPrimitiveBase() {}

		bool	IsComplex() const		{ return true;	}
		bool	IsSplitable() const		{ return false;	}

	virtual void	Simplify( HiderREYES &hider ) = 0;
};

//==================================================================
/// SimplePrimitiveBase
//==================================================================
class SimplePrimitiveBase : public PrimitiveBase
{
public:
	float	mURange[2];
	float	mVRange[2];
	u_int	mSplitCnt;
	int		mDiceGridWd;
	int		mDiceGridHe;

public:
	SimplePrimitiveBase( PrimitiveBase::Type type ) :
		PrimitiveBase(type),
		mSplitCnt(0),
		mDiceGridWd(-1),
		mDiceGridHe(-1)
	{
		mURange[0] = 0;
		mURange[1] = 1;
		mVRange[0] = 0;
		mVRange[1] = 1;
	}

	virtual ~SimplePrimitiveBase() {}

		bool	IsComplex() const		{ return false;	}
		bool	IsSplitable() const		{ return true;	}

	virtual SimplePrimitiveBase	*Clone() const = 0;

	bool SetupForDiceOrSplit(	const HiderREYES &hider,
								bool &out_uSplit,
								bool &out_vSplit );

	void	Split( HiderREYES &hider, bool uSplit, bool vSplit );

	// make a 3D bound, return false if the bound cannot be made
	virtual bool	MakeBound( Bound &out_bound ) const = 0;

	inline Point3	&EvalP(
						float u,
						float v,
						Point3 &out_pt ) const
	{
		Eval_dPdu_dPdv( u, v, out_pt, NULL, NULL );
		return out_pt;
	}

	virtual void	Eval_dPdu_dPdv(
						float u,
						float v,
						Point3 &out_pt,
						Vec3 *out_dPdu,
						Vec3 *out_dPdv ) const = 0;

	virtual void	Dice(
						MicroPolygonGrid &g,
						const Matrix44 &mtxWorldCamera,
						bool doColorCoded ) const;

	Vec2 CalcLocalUV( const Vec2 &gridUV ) const
	{
		return
			Vec2(
				DMix( mURange[0], mURange[1], gridUV.x ),
				DMix( mVRange[0], mVRange[1], gridUV.y )
			);
	}

};

//==================================================================
bool ParamsFindP(	ParamList &params,
					const SymbolList &staticSymbols,
					DVec<Vec3> &out_vectorP,
					int fromIdx=1 );

bool ParamsFindP(	ParamList &params,
					const SymbolList &staticSymbols,
					Vec3	*pOut_vectorP,
					int		expectedN,
					int		fromIdx=1 );

//==================================================================
}

#endif
