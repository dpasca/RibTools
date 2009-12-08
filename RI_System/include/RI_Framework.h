//==================================================================
/// RI_Framework.h
///
/// Created by Davide Pasca - 2009/02/08
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_FRAMEWORK_H
#define RI_FRAMEWORK_H

#include "RI_Base.h"
#include "RI_Primitive.h"
#include "RI_Options.h"
#include "RI_HiderST.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;

//==================================================================
/// RenderOutputBase
//==================================================================
class RenderOutputBase
{
public:
	virtual ~RenderOutputBase() {}

	virtual void SetSize( u_int w, u_int h ) = 0;
	virtual void UpdateRegion( u_int x1, u_int y1, u_int w, u_int h, const float *pSrcData, u_int srcStride ) = 0;
	virtual void Blit() const = 0;
	virtual u_int GetCurWd() const = 0;
	virtual u_int GetCurHe() const = 0;
};

//==================================================================
/// RenderOutputNull
//==================================================================
class RenderOutputNull : public RenderOutputBase
{
public:
	RenderOutputNull() {}
	~RenderOutputNull() {}

		void SetSize( u_int w, u_int h ) {}
		void UpdateRegion( u_int x1, u_int y1, u_int w, u_int h, const float *pSrcData, u_int srcStride ) {}
		void Blit() const {}
		u_int GetCurWd() const {	return 0; }
		u_int GetCurHe() const {	return 0; }
};

//==================================================================
/// RenderOutputMem
//==================================================================
class RenderOutputMem : public RenderOutputBase
{
	u_int		mCurWd;
	u_int		mCurHe;
	DVec<float>	mData;

public:
	RenderOutputMem() :
		mCurWd(0),
		mCurHe(0)
	{
	}

	~RenderOutputMem()
	{
	}

	void SetSize( u_int w, u_int h )
	{
		mCurWd = w;
		mCurHe = h;
		mData.clear();
		mData.resize( w * h * 3 );
	}

	void UpdateRegion( u_int x1, u_int y1, u_int w, u_int h, const float *pSrcData, u_int srcStride )
	{
		u_int	x2 = x1 + w;
		u_int	y2 = y1 + h;

		float *pDest = &mData[0] + (x1 + y1 * mCurWd) * 3;

		size_t	wc = w * 3;

		for (size_t i=0; i < h; ++i)
		{
			for (size_t j=0; j < wc; ++j)
				pDest[j] = pSrcData[j];

			pSrcData += srcStride;
			pDest += wc;
		}
	}

	void Blit() const
	{
	}

	u_int GetCurWd() const	{ return mCurWd; }
	u_int GetCurHe() const	{ return mCurHe; }
};

//==================================================================
class RenderBucketsBase
{
public:
	virtual ~RenderBucketsBase() {}

	virtual void Render( Hider &hider ) = 0;
/*
private:
	RenderBucketsBase( const RenderBucketsBase &from ) {}
	void operator =( const RenderBucketsBase &from ) {}
*/
};

//==================================================================
/// Framework
//==================================================================
class Framework
{
public:
	RenderOutputBase	*mpRenderOutput;
	RenderBucketsBase	*mpRenderBuckets;
	const SymbolList	*mpGlobalSyms;
	Hider			mHider;

private:
	Options				mOptions;	//temporary, will remove

private:
	DVec<Attributes*>	mpUniqueAttribs;
	DVec<Transform*>	mpUniqueTransform;

	RevisionChecker		mAttrsRev;
	RevisionChecker		mTransRev;

public:
	Framework(	RenderOutputBase *pRenderOutput,
				RenderBucketsBase *pRenderBuckets,
				const Hider &hiderParams );

	void SetGlobalSyms( const SymbolList *pGlobalSyms )
	{
		mpGlobalSyms = pGlobalSyms;

		mHider.mpGlobalSyms = pGlobalSyms;
	}

	void WorldBegin(
				const Options &opt,
				const Matrix44 &mtxWorldCamera );

	void Insert(	PrimitiveBase		*pPrim,
					const Attributes	&attr,
					const Transform		&xform );

	void WorldEnd();

	static void RenderBucket_s( Hider &hider, HiderBucket &bucket );

private:

	void	worldEnd_simplify();
	void	worldEnd_splitAndAddToBuckets();
};

//==================================================================
}

#endif