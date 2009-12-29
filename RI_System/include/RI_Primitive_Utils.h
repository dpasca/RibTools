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

	const float srcUVs[RI_GET_SIMD_PAD_SUBS(4)][2] = {
		This.mURange[0], This.mVRange[0],
		This.mURange[1], This.mVRange[0],
		This.mURange[0], This.mVRange[1],
		This.mURange[1], This.mVRange[1]
	};

	out_bound.Reset();

	for (size_t blkIdx=0; blkIdx < RI_GET_SIMD_BLOCKS( 4 ); ++blkIdx)
	{
		SlVec2	uvs;

		size_t	iStart	= (blkIdx + 0) * RI_SIMD_BLK_LEN + 0;
		size_t	iEnd	= (blkIdx + 1) * RI_SIMD_BLK_LEN + 0;
		iEnd = DMIN( 4, iEnd );
		size_t	n = iEnd - iStart;

		for (size_t subIdx=0; subIdx < n; ++subIdx)
		{
			uvs[0][subIdx] = srcUVs[(blkIdx + 0) * RI_SIMD_BLK_LEN + subIdx][0];
			uvs[1][subIdx] = srcUVs[(blkIdx + 0) * RI_SIMD_BLK_LEN + subIdx][1];
		}

		SlVec3	Po;
		This.EvalP( uvs, Po );


		for (size_t subIdx=0; subIdx < n; ++subIdx)
		{
			out_bound.Expand( Vec3f(
									Po[0][subIdx],
									Po[1][subIdx],
									Po[2][subIdx]
								) );
		}
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



//==================================================================
}

#endif
