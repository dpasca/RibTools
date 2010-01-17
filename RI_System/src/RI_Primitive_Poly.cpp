//==================================================================
/// RI_Primitive_Poly.cpp
///
/// Created by Davide Pasca - 2009/4/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "DMath.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Primitive_Poly.h"
#include "RI_Primitive_Patch.h"
#include "RI_HiderST.h"

//==================================================================
namespace RI
{

//==================================================================
/// Polygon
//==================================================================
Polygon::Polygon( ParamList &params, const SymbolList &globalSymbols ) :
	ComplexPrimitiveBase(POLYGON),
	mParams(params)
{
}

//==================================================================
static void simplifyAddTriangle(
				Hider &hider,
				const Float3 &v1,
				const Float3 &v2,
				const Float3 &v3,
				ParamList &params,
				ComplexPrimitiveBase &srcPrim
				)
{
	Float3	mid = (v1 + v2 + v3) * (1.0f/3);
	Float3	a	= (v1 + v2) * 0.5f;
	Float3	b	= (v2 + v3) * 0.5f;
	Float3	c	= (v1 + v3) * 0.5f;

	Float3	patchVerts[4];
	patchVerts[0] = v1;
	patchVerts[1] = a;
	patchVerts[2] = c;
	patchVerts[3] = mid;
	hider.InsertSimple( DNEW PatchBilinear( params, patchVerts ), srcPrim );
	patchVerts[0] = v2;
	patchVerts[1] = a;
	patchVerts[2] = b;
	patchVerts[3] = mid;
	hider.InsertSimple( DNEW PatchBilinear( params, patchVerts ), srcPrim );
	patchVerts[0] = v3;
	patchVerts[1] = b;
	patchVerts[2] = c;
	patchVerts[3] = mid;
	hider.InsertSimple( DNEW PatchBilinear( params, patchVerts ), srcPrim );
}

//==================================================================
void Polygon::Simplify( Hider &hider )
{
	int	PValuesParIdx = FindParam( "P", Param::FLT_ARR, 0, mParams );
	if ( PValuesParIdx == -1 )
	{
		DASSTHROW( 0, ("Missing 'P'") );
		return;
	}

	const FltVec	&paramP = mParams[PValuesParIdx].NumVec();
	
	int	last	= (int)paramP.size()/3 - 1;
	int	start	= 1;
	int	end		= DMIN( (int)3, last );
	
	Float3	patchVerts[4];
	
	while ( (end - start) == 2 )
	{
		patchVerts[0].Set( &paramP[3 * 0] );
		patchVerts[1].Set( &paramP[3 * start] );
		patchVerts[2].Set( &paramP[3 * end] );
		patchVerts[3].Set( &paramP[3 * (start+1)] );

		hider.InsertSimple(
				DNEW PatchBilinear( mParams, patchVerts ),
				*this );
		
		start	= end;
		end		= DMIN( start+2, last );
	}

	if ( (end - start) == 1 )
	{	
		simplifyAddTriangle(
						hider,
						Float3( &paramP[3 * 0] ),
						Float3( &paramP[3 * start] ),
						Float3( &paramP[3 * end] ),
						mParams,
						*this
					);
	}
}

//==================================================================
/// 
//==================================================================
PointsGeneralPolygons::PointsGeneralPolygons( ParamList &params, const SymbolList &globalSymbols ) :
	ComplexPrimitiveBase(POINTSGENERALPOLYGONS),
	mParams(params)
{
}

//==================================================================
static const size_t MAX_VERT_INFO = 32;
struct VertInfo
{
	const char	*pName;
	int			parIdx;
};

//==================================================================
static void tessellateToBilinearPatches(
							Hider	 &hider,
							const FltVec &paramP,
							const int	 *pIndices,
							int			 indicesN,
							ParamList	 &primParams,
							ComplexPrimitiveBase &srcPrim )
{
	int	last	= (int)indicesN - 1;
	int	start	= 1;
	int	end		= DMIN( (int)3, last );

	Float3	patchVerts[4];

	while ( (end - start) == 2 )
	{
		patchVerts[0].Set( &paramP[3 * pIndices[ 0			] ] );
		patchVerts[1].Set( &paramP[3 * pIndices[ start		] ] );
		patchVerts[2].Set( &paramP[3 * pIndices[ end		] ] );
		patchVerts[3].Set( &paramP[3 * pIndices[ (start+1)	] ] );

		hider.InsertSimple(
				DNEW PatchBilinear( primParams, patchVerts ),
				srcPrim );

		start	= end;
		end		= DMIN( start+2, last );
	}

	if ( (end - start) == 1 )
	{	
		simplifyAddTriangle(
						hider,
						Float3( &paramP[3 * pIndices[ 0		] ] ),
						Float3( &paramP[3 * pIndices[ start	] ] ),
						Float3( &paramP[3 * pIndices[ end	] ] ),
						primParams,
						srcPrim
					);
	}
}

//==================================================================
void PointsGeneralPolygons::Simplify( Hider &hider )
{
	size_t		nloopsN = mParams[0].IntArrSize();
	const int	*pNLoops = mParams[0].PInt();

	size_t		nvertsN = mParams[1].IntArrSize();
	const int	*pNVerts = mParams[1].PInt();

	size_t		vertsN = mParams[2].IntArrSize();
	const int	*pVerts = mParams[2].PInt();

	VertInfo	vinfo[MAX_VERT_INFO];
	size_t		vinfoN = 0;

	int	PValuesParIdx = -1;

	for (size_t pi=3; pi < mParams.size(); ++pi)
	{
		if NOT( mParams[pi].IsString() )
		{
			DASSTHROW( 0, ("Expecting parameter name !") );
			return;
		}

		if ( (pi+1) >= mParams.size() )
		{
			DASSTHROW( 0, ("Missing values") );
		}

		const char *pNameParam = mParams[pi].PChar();

		++pi;

		if ( 0 == strcasecmp( pNameParam, "P" ) )
		{
			PValuesParIdx = (int)pi;
		}

		vinfo[vinfoN].pName	= pNameParam;
		vinfo[vinfoN].parIdx= (int)pi;
		++vinfoN;
	}

	if ( PValuesParIdx == -1 )
	{
		DASSTHROW( 0, ("Missing 'P'") );
		return;
	}

	const FltVec	&paramP = mParams[PValuesParIdx].NumVec();

	size_t nVertsIdx	= 0;
	size_t idxVertsIdx	= 0;

	for (size_t i=0; i < nloopsN; ++i)
	{
		DASSTHROW( pNLoops[i] >= 0, ("Negative loops count ?!") );

		size_t nLoops = pNLoops[i];

		nLoops = DMIN( nLoops, 1 );	// only the first loop
		
		DASSERT( (nVertsIdx+nLoops) <= nvertsN );

		for (size_t j=0; j < nLoops; ++j)
		{
			int nVerts = pNVerts[ nVertsIdx++ ];

			DASSERT( (idxVertsIdx+nVerts) <= vertsN );

			tessellateToBilinearPatches(
							hider,
							paramP,
							&pVerts[ idxVertsIdx ],
							nVerts,
							mParams,
							*this );

			idxVertsIdx += nVerts;
		}
	}
}

//==================================================================
}