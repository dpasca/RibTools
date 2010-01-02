//==================================================================
/// RI_Primitive_Utils.h
///
/// Created by Davide Pasca - 2009/12/25
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_PRIMITIVE_UTILS_H
#define RI_PRIMITIVE_UTILS_H

//==================================================================
namespace RI
{

//==================================================================
template <class _S>
bool MakeBoundFromUVRange( const _S &This, Bound &out_bound )
{
#if 1

	out_bound.Reset();

	float	DVECTOR_SIMD_ALIGN( us ) [] = { This.mURange[0], This.mURange[1], This.mURange[0], This.mURange[1] };
	float	DVECTOR_SIMD_ALIGN( vs ) [] = { This.mVRange[0], This.mVRange[0], This.mVRange[1], This.mVRange[1] };

	SlVec3	Po[ RI_GET_SIMD_PAD_SUBS(4) ];

	This.EvalP( (const SlScalar *)us, (const SlScalar *)vs, Po, 4 );

	for (size_t i=0; i < 4; ++i)
	{
		size_t	blk = i / RI_SIMD_BLK_LEN;
		size_t	sub = i & (RI_SIMD_BLK_LEN-1);

		out_bound.Expand( Vec3f(
								Po[blk][0][sub],
								Po[blk][1][sub],
								Po[blk][2][sub]
							) );
	}

#else

	Point3	Po;
	out_bound.Reset();
	out_bound.Expand( This.EvalP( This.mURange[0], This.mVRange[0], Po ) );
	out_bound.Expand( This.EvalP( This.mURange[1], This.mVRange[0], Po ) );
	out_bound.Expand( This.EvalP( This.mURange[0], This.mVRange[1], Po ) );
	out_bound.Expand( This.EvalP( This.mURange[1], This.mVRange[1], Po ) );

#endif

	return true;
}

#define MAKE_BOUND_DIM_LEN	3

//==================================================================
template <class _S>
bool MakeBoundFromUVRange9( const _S &This, Bound &out_bound )
{
	out_bound.Reset();

	static const size_t	N_ELEMS = MAKE_BOUND_DIM_LEN*MAKE_BOUND_DIM_LEN;
	static const size_t	N_ELEMS_PAD = RI_GET_SIMD_PAD_SUBS( N_ELEMS );

	float	DVECTOR_SIMD_ALIGN( us ) [N_ELEMS_PAD];// = { This.mURange[0], This.mURange[1], This.mURange[0], This.mURange[1] };
	float	DVECTOR_SIMD_ALIGN( vs ) [N_ELEMS_PAD];// = { This.mVRange[0], This.mVRange[0], This.mVRange[1], This.mVRange[1] };

	size_t	idx = 0;
	float	v = This.mVRange[0];
	float	dv = (This.mVRange[1] - This.mVRange[0]) / (MAKE_BOUND_DIM_LEN - 1);
	float	du = (This.mURange[1] - This.mURange[0]) / (MAKE_BOUND_DIM_LEN - 1);
	for (int y=0; y < MAKE_BOUND_DIM_LEN; ++y)
	{
		float	u = This.mURange[0];
		for (int x=0; x < MAKE_BOUND_DIM_LEN; ++x, ++idx)
		{
			us[idx] = u;
			vs[idx] = v;
			u += du;
		}
		v += dv;
	}
	// replicate to the padding
	for (; idx < N_ELEMS_PAD; ++idx)
	{
		us[idx] = us[idx-1];
		vs[idx] = vs[idx-1];
	}

	SlVec3	Po[ N_ELEMS_PAD ];

	This.EvalP( (const SlScalar *)us, (const SlScalar *)vs, Po, N_ELEMS );

	for (size_t i=0; i < N_ELEMS; ++i)
	{
		size_t	blk = i / RI_SIMD_BLK_LEN;
		size_t	sub = i & (RI_SIMD_BLK_LEN-1);

		out_bound.Expand( Vec3f(
								Po[blk][0][sub],
								Po[blk][1][sub],
								Po[blk][2][sub]
							) );
	}

	return true;
}




//==================================================================
}

#endif
