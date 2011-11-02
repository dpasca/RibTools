//==================================================================
/// DMT_Spline.h
///
/// Created by Davide Pasca - 2011/11/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DMT_SPLINE_H
#define DMT_SPLINE_H

#include "DMatrix44.h"

//==================================================================
namespace DMT
{

//==================================================================
template <class _S, class _T>
inline _S Spline(
				const _T &t,
				const Matrix44 &b,
				const _S &p0,
				const _S &p1,
				const _S &p2,
				const _S &p3 )
{
	_S v0(p0*b.mij(0,0) + p1*b.mij(0,1) + p2*b.mij(0,2) + p3*b.mij(0,3));
	_S v1(p0*b.mij(1,0) + p1*b.mij(1,1) + p2*b.mij(1,2) + p3*b.mij(1,3));
	_S v2(p0*b.mij(2,0) + p1*b.mij(2,1) + p2*b.mij(2,2) + p3*b.mij(2,3));
	_S v3(p0*b.mij(3,0) + p1*b.mij(3,1) + p2*b.mij(3,2) + p3*b.mij(3,3));

	return	v0 *t*t*t +
			v1 *t*t +
			v2 *t +
			v3 ;
}

//==================================================================
template <class _S, class _T>
inline _S SplineDeriv(
					const _T &t,
					const Matrix44 &b,
					const _S &p0,
					const _S &p1,
					const _S &p2,
					const _S &p3 )
{
	_S v0(p0*b.mij(0,0) + p1*b.mij(0,1) + p2*b.mij(0,2) + p3*b.mij(0,3));
	_S v1(p0*b.mij(1,0) + p1*b.mij(1,1) + p2*b.mij(1,2) + p3*b.mij(1,3));
	_S v2(p0*b.mij(2,0) + p1*b.mij(2,1) + p2*b.mij(2,2) + p3*b.mij(2,3));

	return	v0 *3*t*t +
			v1 *2*t +
			v2 ;
}

//==================================================================
static Matrix44	BezierBasis
(
	-1,	 3,	-3,	1,
	 3,	-6,	 3,	0,
	-3,	 3,	 0,	0,
	 1,	 0,	 0,	0
);

static Matrix44	BSplineBasis
(
	-1/6.f,	 3/6.f,	-3/6.f,	1/6.f,
	 3/6.f,	-6/6.f,	 3/6.f,	0/6.f,
	-3/6.f,	 0/6.f,	 3/6.f,	0/6.f,
	 1/6.f,	 4/6.f,	 1/6.f,	0/6.f
);

static Matrix44	CatmullRomBasis
(
	-0.5f,	 1.5f,	-1.5f,	 0.5f,
	 1,		-2.5f,	 2,		-0.5f,
	-0.5f,	 0,		 0.5f,	 0,
	 0,		 1,		 0,		 0
);

static Matrix44	HermiteBasis
(
	 2,	-3,	 0,	1,
	-2,	 3,	 0,	0,
	 1,	-2,	 1,	0,
	 1,	-1,	 0,	0
);

static Matrix44	PowerBasis
(
	1,	0,	0,	0,
	0,	1,	0,	0,
	0,	0,	1,	0,
	0,	0,	0,	1
);


//==================================================================
}

#endif
