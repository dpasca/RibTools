//==================================================================
/// RI_Base.h
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
///
/// Portions from the official RI.h
///   See: https://renderman.pixar.com/products/rispec/rispec_3_1/appendix.C.html
//==================================================================

#ifndef RI_BASE_H
#define RI_BASE_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DContainers.h"
#include "DMath/include/DMath.h"

//==================================================================
namespace RI
{

//==================================================================
static const u_int	MP_GRID_MAX_SIZE				= DMT_SIMD_PADSIZE( 48 ) * 48;
static const u_int	MP_GRID_MAX_SIZE_SIMD_BLKS		= DMT_SIMD_BLOCKS( MP_GRID_MAX_SIZE );
static const u_int	MP_GRID_MIN_DIM					= DMT_SIMD_FLEN;
// maximum dimension is the area divided by the smallest dimension
// example.. for SIMD 4, it would be.. maxdim = padded( padded(48)*48 / 4 ) -> 48*48 / 4 -> 576
static const u_int	MP_GRID_MAX_DIM					= DMT_SIMD_PADSIZE( MP_GRID_MAX_SIZE / DMT_SIMD_FLEN );
static const u_int	MP_GRID_MAX_DIM_SIMD_BLKS		= DMT_SIMD_BLOCKS( MP_GRID_MAX_DIM );

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
	E_OUTOFMEMORY_X		,
	E_PROTOCOLBOTCH		,
	E_STRINGTOOBIG		,
	E_SYNTAXERROR		,
	E_UNREGISTERED		,

};
	
//==================================================================
const char *ErrorToString( Error errCode );

//==================================================================
/// RefCount
//==================================================================
class RefCount
{
	int		mRefCount;
	
public:
	RefCount( const RefCount &from ) : mRefCount(0)
	{
	}
	
	RefCount &operator=( const RefCount &from )
	{
		mRefCount = 0;
	}

	RefCount() :
		mRefCount(0)
	{
	}
	
	~RefCount()
	{
		DASSERT( mRefCount == 0 );
	}

	void AddRef()
	{
		mRefCount += 1;
	}

	int SubRef()
	{
		DASSERT( mRefCount >= 1 );
		mRefCount -= 1;
		return mRefCount;
	}

	int GetCount() const
	{
		return mRefCount;
	}
};

//==================================================================
/// RCBase
//==================================================================
class RCBase
{
	int		mCount;
	
public:
	RCBase() :
		mCount(0)
	{
	}
	
	virtual ~RCBase()
	{
		DASSERT( mCount == 0 );
	}

	void AddRef()
	{
		mCount += 1;
	}

	int SubRef()
	{
		DASSERT( mCount >= 1 );
		mCount -= 1;
		int retVal = mCount;

		if ( retVal == 0 )
		{
			DDELETE( this );
		}

		return retVal;
	}

	int GetRef() const
	{
		return mCount;
	}

private:
	RCBase( const RCBase &from ) :	mCount(0)
	{
	}
	
	RCBase &operator=( const RCBase &from )
	{
		mCount = 0;
	}
};

//==================================================================
class RCBaseNoDel
{
	int		mCount;
	
public:
	RCBaseNoDel() :
		mCount(0)
	{
	}
	
	virtual ~RCBaseNoDel()
	{
		DASSERT( mCount == 0 );
	}

	void AddRef()
	{
		mCount += 1;
	}

	int SubRef()
	{
		DASSERT( mCount >= 1 );
		mCount -= 1;
		int retVal = mCount;

		return retVal;
	}

	int GetRef() const
	{
		return mCount;
	}

private:
	RCBaseNoDel( const RCBaseNoDel &from ) :	mCount(0)
	{
	}
	
	RCBaseNoDel &operator=( const RCBaseNoDel &from )
	{
		mCount = 0;
	}
};

//==================================================================
/// RefCount
//==================================================================
template <class T>
class RCOwn
{
	T	*mPtr;

public:
	RCOwn()						: mPtr(NULL)	{ }
	RCOwn( T *ptr )				: mPtr(NULL)	{ Borrow( ptr ); }
	RCOwn( const RCOwn &from )	: mPtr(NULL)	{ Borrow( from.get() ); }

	virtual ~RCOwn()
	{
		if ( mPtr )
			mPtr->SubRef();
	}

	RCOwn &operator=( const RCOwn &from )	{	Borrow( from.get() );	return *this; }
	RCOwn &operator=( T *ptr )				{	Borrow( ptr );			return *this; }
	RCOwn &operator=( const T *ptr )		{	Borrow( ptr );			return *this; }

private:
	void Borrow( T *ptr )
	{
		if ( ptr == mPtr )
			return;

		if ( mPtr )
			mPtr->SubRef();

		if ( mPtr = ptr )
			mPtr->AddRef();
	}

	void Borrow( const T *ptr )
	{
		if ( ptr == mPtr )
			return;

		if ( mPtr )
			mPtr->SubRef();

		if ( mPtr = (T *)ptr )
			mPtr->AddRef();
	}

public:
	const T *get() const
	{
		DASSERT( !mPtr || mPtr->GetRef() > 0 );

		return mPtr;
	}

	T *get()
	{
		DASSERT( !mPtr || mPtr->GetRef() > 0 );

		return mPtr;
	}

	const T *operator->() const	{ return mPtr; }
		  T *operator->()		{ return mPtr; }
};

//==================================================================
class RevisionTracker
{
public:
	int	mRTrackRevisionCount;
	
	RevisionTracker() : mRTrackRevisionCount(0) {}

	void BumpRevision()
	{
		mRTrackRevisionCount += 1;
	}
};

//==================================================================
class RevisionChecker
{
public:
	int	mCurRevisionCount;
	
	RevisionChecker() : mCurRevisionCount(-1) {}

	bool Sync( const RevisionTracker &tracker )
	{
		if ( tracker.mRTrackRevisionCount != mCurRevisionCount )
		{
			mCurRevisionCount = tracker.mRTrackRevisionCount;
			return true;
		}
		else
		{
			return false;
		}
	}
};

//==================================================================
/// 
//==================================================================
typedef const char	*RtToken;
typedef void		*ObjectHandle;
typedef void		*LightHandle;

struct Bound
{
	Float3	mBox[2];

	Bound()
	{
	}

	Bound( float val )
	{
		mBox[0].Set( val, val, val );
		mBox[1].Set( val, val, val );
	}
	
	Bound( float x1, float y1, float z1,
		   float x2, float y2, float z2 )
	{
		mBox[0].Set( x1, y1, z1 );
		mBox[1].Set( x2, y2, z2 );
	}
	
	void SetMin( const float *p )			{ mBox[0].Set( p[0], p[1], p[2] );	}
	void SetMax( const float *p )			{ mBox[1].Set( p[0], p[1], p[2] );	}
	void SetMin( float x, float y, float z ){ mBox[0].Set( x, y, z );	}
	void SetMax( float x, float y, float z ){ mBox[1].Set( x, y, z );	}
	
	void Reset()
	{
		mBox[0].Set( FLT_MAX, FLT_MAX, FLT_MAX );
		mBox[1].Set( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	}

	void Expand( const Float3 &p )
	{
		mBox[0].x() = DMIN( mBox[0].x(), p.x() );
		mBox[0].y() = DMIN( mBox[0].y(), p.y() );
		mBox[0].z() = DMIN( mBox[0].z(), p.z() );

		mBox[1].x() = DMAX( mBox[1].x(), p.x() );
		mBox[1].y() = DMAX( mBox[1].y(), p.y() );
		mBox[1].z() = DMAX( mBox[1].z(), p.z() );
	}
	
	bool IsValid() const
	{
		return
			mBox[0].x() <= mBox[1].x() &&
			mBox[0].y() <= mBox[1].y() &&
			mBox[0].z() <= mBox[1].z();
	}
};

//==================================================================
static const size_t	NCOLS = 3;	// temporary !!
static const size_t	NOUTCOLS = 4;	// temporary !!

//==================================================================
}

//==================================================================
/*
enum VarType
{
	VARTYPE_UNKNOWN,
	VARTYPE_FLOAT,
	VARTYPE_INT,
	VARTYPE_BOOLEAN
};
*/

//==================================================================
/// 
//==================================================================
typedef short	RtBoolean;
typedef long	RtInt;
typedef float	RtFloat;

typedef const char	*RtToken; // DAVIDE - I think we want "const" here..

typedef RtFloat		RtColor[3];
typedef RtFloat		RtPoint[3];
typedef RtFloat		RtMatrix[4][4];
//typedef RtFloat	RtBasis[4][4];
typedef Matrix44	RtBasis;
typedef RtFloat		RtBound[6];

typedef char	*RtString;

typedef char	*RtPointer;
typedef int	RtVoid;

typedef RtFloat	(*RtFloatFunc)();
typedef RtVoid	(*RtFunc)();

typedef RtPointer RtObjectHandle;
typedef RtPointer RtLightHandle;

#define RI_FALSE	0
#define RI_TRUE		(! RI_FALSE)
#define RI_INFINITY	1.0e38f
#define RI_EPSILON	1.0e-10f
#define RI_NULL		((RtToken)0)

//==================================================================
namespace RI
{
extern RtBasis	BezierBasis;
extern RtBasis	BSplineBasis;
extern RtBasis	CatmullRomBasis;
extern RtBasis	HermiteBasis;
extern RtBasis	PowerBasis;
//==================================================================
}

//==================================================================
namespace RI
{

	static const u_int INVALID_PC = (u_int)-1;

//==================================================================
}

#endif