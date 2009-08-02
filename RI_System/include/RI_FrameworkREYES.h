//==================================================================
/// RI_FrameworkREYES.h
///
/// Created by Davide Pasca - 2009/02/08
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_FRAMEWORKREYES_H
#define RI_FRAMEWORKREYES_H

#include "RI_Base.h"
#include "RI_Primitive.h"
#include "RI_Options.h"
#include "RI_HiderREYES.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;

//==================================================================
class RenderOutputBase
{
public:
	virtual ~RenderOutputBase() {};

	virtual void Update( u_int w, u_int h, const float *pSrcData ) = 0;
	virtual void Blit() const = 0;
	virtual u_int GetCurWd() const = 0;
	virtual u_int GetCurHe() const = 0;
};

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

	void Update( u_int w, u_int h, const float *pSrcData )
	{
		mCurWd = w;
		mCurHe = h;
		mData.clear();
		mData.append_array( pSrcData, w * h * 3 );
	}

	void Blit() const
	{
	}

	u_int GetCurWd() const	{ return mCurWd; }
	u_int GetCurHe() const	{ return mCurHe; }
};

//==================================================================
/// FrameworkREYES
//==================================================================
class FrameworkREYES
{
public:
	RenderOutputBase	*mpRenderOutput;
	SymbolList			*mpStatics;
	HiderREYES			mHider;

private:
	Options				mOptions;	//temporary, will remove

private:
	DVec<Attributes*>	mpUniqueAttribs;
	DVec<Transform*>	mpUniqueTransform;

	RevisionChecker		mAttrsRev;
	RevisionChecker		mTransRev;

public:
	FrameworkREYES(	RenderOutputBase *pRenderOutput,
					const HiderREYES &hiderParams );

	void SetStatics( SymbolList *pStatics )
	{
		mpStatics = pStatics;

		mHider.mpStatics = pStatics;
	}

	void WorldBegin(
				const Options &opt,
				const Matrix44 &mtxWorldCamera );

	void Insert(	PrimitiveBase		*pPrim,
					const Attributes	&attr,
					const Transform		&xform );

	void WorldEnd();

private:
	static void renderBucket_s( HiderREYES &hider, Bucket &bucket );

	void	worldEnd_simplify();
	void	worldEnd_splitAndAddToBuckets();
	void	worldEnd_renderBuckets();
};

//==================================================================
}

#endif