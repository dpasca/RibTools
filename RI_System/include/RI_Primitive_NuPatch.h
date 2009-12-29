//==================================================================
/// RI_Primitive_NuPatch.h
///
/// Created by Davide Pasca - 2009/12/25
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_PRIMITIVE_NUPATCH_H
#define RI_PRIMITIVE_NUPATCH_H

#include "RI_Primitive_Base.h"
#include "RI_Primitive_Utils.h"

//==================================================================
namespace RI
{

//==================================================================
/// NuPatch
//==================================================================
class NuPatch : public SimplePrimitiveBase
{
	class BaseDef : public RCBase
	{
	public:
		DVec<float>		mUKnots;
		DVec<float>		mVKnots;
		DVec<Vec4f>		mCtrlPws;
		int				mUOrder;
		int				mVOrder;
		int				mNu;
		int				mNv;

	public:
		BaseDef( int uorder, int vorder, int nu, int nv ) :
			mUOrder(uorder),
			mVOrder(vorder),
			mNu(nu),
			mNv(nv)
		{
		}
	};

	RCOwn<BaseDef>	moBaseDef;

public:
	NuPatch(
			int			nu		,
			int			uorder	,
			const float	*pUknot	,
			float		umin	,
			float		umax	,
			int			nv		,
			int			vorder	,
			const float	*pVknot	,
			float		vmin	,
			float		vmax	,
			ParamList	&params
			);

	NuPatch( const NuPatch &from ) :
		SimplePrimitiveBase(from)
	{
		moBaseDef.Borrow( from.moBaseDef );
	}

	void operator = ( const NuPatch &from )	{	moBaseDef.Borrow( from.moBaseDef );	}

		NuPatch	*Clone() const {	return DNEW NuPatch( *this ); }

		bool MakeBound( Bound &out_bound ) const
		{
			return MakeBoundFromUVRange( *this, out_bound );
		}

		void Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const;
};
	
//==================================================================
}

#endif