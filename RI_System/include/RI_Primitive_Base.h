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
	RefCount	mRefCnt;

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

		NUPATCH,

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
			DDELETE( this );
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

	inline SlVec3	&EvalP(
						const SlVec2 &uv,
						SlVec3 &out_pt ) const
	{
		Eval_dPdu_dPdv( uv, out_pt, NULL, NULL );
		return out_pt;
	}

	virtual void	Eval_dPdu_dPdv(
						float u,
						float v,
						Point3 &out_pt,
						Vec3f *out_dPdu,
						Vec3f *out_dPdv ) const
	{
		DASSERT( 0 );
	}

	virtual void	Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const = 0;
					/*
					{
						//out_pt = SlVec3( 0.0f );
						Vec3f dPdu;
						Vec3f dPdv;
						Vec3f pt;

						for (int i=0; i < RI_SIMD_BLK_LEN; ++i)
						{
							Eval_dPdu_dPdv(
								uv[0][i],
								uv[1][i],
								pt,
								&dPdu,
								&dPdv );

							out_pt[0][i] = pt[0];
							out_pt[1][i] = pt[1];
							out_pt[2][i] = pt[2];

							if ( out_dPdu )
							{
								out_dPdu[0][i] = dPdu[0];
								out_dPdu[1][i] = dPdu[1];
								out_dPdu[2][i] = dPdu[2];
							}

							if ( out_dPdv )
							{
								out_dPdv[0][i] = dPdv[0];
								out_dPdv[1][i] = dPdv[1];
								out_dPdv[2][i] = dPdv[2];
							}
						}
					}
					*/

	virtual void	Dice(
						MicroPolygonGrid &g,
						const Matrix44 &mtxWorldCamera,
						bool doColorCoded ) const;

	Vec2f CalcLocalUV( const Vec2f &gridUV ) const
	{
		return
			Vec2f(
				DMix( mURange[0], mURange[1], gridUV.x() ),
				DMix( mVRange[0], mVRange[1], gridUV.y() )
			);
	}

};

//==================================================================
bool ParamsFindP(	ParamList &params,
					const SymbolList &staticSymbols,
					DVec<Vec3f> &out_vectorP,
					int fromIdx=1 );

bool ParamsFindP(	ParamList &params,
					const SymbolList &staticSymbols,
					Vec3f	*pOut_vectorP,
					int		expectedN,
					int		fromIdx=1 );

//==================================================================
}

#endif
