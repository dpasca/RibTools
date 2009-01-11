/*
 *  RI_Base.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 *  Portions from the official RI.h
 *    See: https://renderman.pixar.com/products/rispec/rispec_3_1/appendix.C.html
 */

#ifndef RI_BASE_H
#define RI_BASE_H

//==================================================================
namespace RI
{
	
//==================================================================
enum Mode
{
	MD_UNDEFINED,
	MD_BASE,
	MD_FRAME,
	MD_WORLD,
	MD_ATTRIBUTE,
	MD_TRANSFORM,
	MD_SOLID,
	MD_OBJECT,
	MD_MOTION,
};

//==================================================================
/// 
//==================================================================
enum Error
{
	E_OK				,
	E_NESTING			,
	E_NOTOPTIONS		,
	E_NOTATTRIBS		,
	E_NOTPRIMS			,
	E_NOTSTARTED		,
	E_ILLSTATE			,
	E_ARRAYTOOBIG		,
	E_BADARGUMENT		,
	E_BADARRAY			,
	E_BADBASIS			,
	E_BADCOLOR			,
	E_BADHANDLE			,
	E_BADPARAMLIST		,
	E_BADRIPCODE		,
	E_BADSTRINGTOKEN	,
	E_BADTOKEN			,
	E_BADVERSION		,
	E_LIMITCHECK		,
	E_OUTOFMEMORY		,
	E_PROTOCOLBOTCH		,
	E_STRINGTOOBIG		,
	E_SYNTAXERROR		,
	E_UNREGISTERED		,

};
	
//==================================================================
const char *ErrorToString( Error errCode );

//==================================================================
/// 
//==================================================================
typedef const char	*Token;
typedef void		*ObjectHandle;
typedef void		*LightHandle;

struct BoundType
{
	float	mBound[6];

	BoundType()
	{
	}

	BoundType( float val )
	{
		mBound[0] = val;
		mBound[1] = val;
		mBound[2] = val;
		mBound[3] = val;
		mBound[4] = val;
		mBound[5] = val;
	}
	
	BoundType( float x1, float y1, float z1,
			   float x2, float y2, float z2 )
	{
		mBound[0] = x1;
		mBound[1] = y1;
		mBound[2] = z1;
		mBound[3] = x2;
		mBound[4] = y2;
		mBound[5] = y2;
	}
};

//==================================================================
}

//==================================================================
/// 
//==================================================================
typedef short	RtBoolean;
typedef long	RtInt;
typedef float	RtFloat;

typedef const char	*RtToken; // DAVIDE - I think we want "const" here..

typedef RtFloat	RtColor[3];
typedef RtFloat	RtPoint[3];
typedef RtFloat	RtMatrix[4][4];
typedef RtFloat	RtBasis[4][4];
typedef RtFloat	RtBound[6];

typedef char	*RtString;

typedef char	*RtPointer;
typedef int	RtVoid;

typedef RtFloat	(*RtFloatFunc)();
typedef RtVoid	(*RtFunc)();

typedef RtPointer RtObjectHandle;
typedef RtPointer RtLightHandle;

#define RI_FALSE	0
#define RI_TRUE		(! RI_FALSE)
#define RI_INFINITY	1.0e38
#define RI_EPSILON	1.0e-10
#define RI_NULL		((RtToken)0)

#endif