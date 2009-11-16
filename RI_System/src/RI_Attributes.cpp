//==================================================================
/// RI_Attributes.cpp
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "stdafx.h"
#include <stdarg.h>
#include <stdexcept>
#include "RI_State.h"
#include "RI_Symbol.h"
#include "RI_Attributes.h"
#include "DSystem/include/DUtils_Files.h"

//==================================================================
namespace RI
{

//==================================================================
/// Attributes
//==================================================================
Attributes::Attributes()
{
	mpCustomUBasis	= NULL;
	mpCustomVBasis	= NULL;
	mpResManager	= NULL;
}

//==================================================================
Attributes::Attributes( const Attributes &attributes )
{
	copyFrom( attributes );
}

//==================================================================
Attributes& Attributes::operator=(const Attributes& rhs)
{
	DSAFE_DELETE( mpCustomUBasis );
	DSAFE_DELETE( mpCustomVBasis );

	copyFrom( rhs );

	return *this;
}

//==================================================================
void Attributes::copyFrom(const Attributes& rhs)
{
	mpState				= rhs.mpState;
	mpGlobalSyms		= rhs.mpGlobalSyms;
	mpResManager		= rhs.mpResManager;

	mpRevision			= rhs.mpRevision			;
	mBound				= rhs.mBound				;
	mDetail				= rhs.mDetail				;
	mMinVisible			= rhs.mMinVisible			;
	mLowerTransition	= rhs.mLowerTransition		;
	mUpperTransition	= rhs.mUpperTransition		;
	mMaxVisible			= rhs.mMaxVisible			;
	mpTypeApproximation	= rhs.mpTypeApproximation	;
	mValueApproximation	= rhs.mValueApproximation	;
	mOrientationFlipped	= rhs.mOrientationFlipped	;
	mSides				= rhs.mSides				;
	mpUBasis			= rhs.mpUBasis				;
	mpVBasis			= rhs.mpVBasis				;

	mpCustomUBasis = NULL;
	mpCustomVBasis = NULL;

	if ( rhs.mpCustomUBasis )
		mpCustomUBasis = DNEW Matrix44( *rhs.mpCustomUBasis );

	if ( rhs.mpCustomVBasis )
		mpCustomVBasis = DNEW Matrix44( *rhs.mpCustomVBasis );

	mUSteps				= rhs.mUSteps				;
	mVSteps				= rhs.mVSteps				;
	mColor				= rhs.mColor				;
	mOpacity			= rhs.mOpacity				;
	moSurfaceSHI.Borrow(  rhs.moSurfaceSHI )		;
	moDisplaceSHI.Borrow( rhs.moDisplaceSHI )		;
	mActiveLights		= rhs.mActiveLights			;
}

//==================================================================
Attributes::~Attributes()
{
	DSAFE_DELETE( mpCustomUBasis );
	DSAFE_DELETE( mpCustomVBasis );
}

//==================================================================
void Attributes::Init(
				  State				*pState,
				  SymbolList		*pGlobalSyms,
				  ResourceManager	*pResManager,
				  RevisionTracker	*pRevision )
{
	mpState		= pState;
	mpGlobalSyms= pGlobalSyms;
	mpResManager= pResManager;
	mpRevision	= pRevision;

	mBound		= RI_INFINITY;
	mDetail		= RI_INFINITY;

	mMinVisible			= 0;
	mLowerTransition	= 0;
	mUpperTransition	= RI_INFINITY;
	mMaxVisible			= RI_INFINITY;

	cmdGeometricApproximation( RI_EMPTY_TOKEN, 0 );
	cmdOrientation( RI_OUTSIDE );
	cmdSides( 2 );
	cmdBasis( RI_BEZIERBASIS, NULL, 3, RI_BEZIERBASIS, NULL, 3 );

	mpCustomUBasis = NULL;
	mpCustomVBasis = NULL;

	mColor.Set( 1, 1, 1 );
	mOpacity.Set( 1, 1, 1 );

	moSurfaceSHI = DNEW SlShaderInst( 
		(SlShader *)pResManager->FindResource( "dbg_normal_col",
													ResourceBase::TYPE_SHADER ) );
}

//==================================================================
const LightSourceT * Attributes::GetLight( size_t actLightIdx ) const
{
	size_t	lightIdx = (size_t)mActiveLights[ actLightIdx ];

	return mpState->GetLightSources()[ lightIdx ];
}

//==================================================================
void Attributes::cmdBound( const Bound &bound )
{
	mBound = bound;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdDetail( const Bound &detail )
{
	mDetail = detail;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdDetailRange(float	minVisible,
								float	lowerTransition,
								float	upperTransition,
								float	maxVisible )
{
	mMinVisible			= minVisible;
	mLowerTransition	= lowerTransition;
	mUpperTransition	= upperTransition;
	mMaxVisible			= maxVisible;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdGeometricApproximation(RtToken typeApproximation,
										   float valueApproximation )
{
	mpTypeApproximation	= mpGlobalSyms->FindSymbol( typeApproximation );
	mValueApproximation		= valueApproximation;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdOrientation( RtToken orientation )
{
	if ( 0 == strcasecmp( orientation, RI_INSIDE ) )
	{
		mOrientationFlipped = true;
	}
	else
	if ( 0 == strcasecmp( orientation, RI_OUTSIDE ) )
	{
		mOrientationFlipped = false;
	}
	else
	if ( 0 == strcasecmp( orientation, RI_LH ) )
	{
		printf( "WARNING: Unsupported LH orientation\n" );	// TODO
	}
	else
	if ( 0 == strcasecmp( orientation, RI_RH ) )
	{
		printf( "WARNING: Unsupported RH orientation\n" );	// TODO
	}

	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdSides( int sides )
{
	mSides	= sides;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdBasis(
				RtToken ubasis, const float *pCustomUBasis, int ustep,
				RtToken vbasis, const float *pCustomVBasis, int vstep )
{
	if ( ubasis )
		mpUBasis = mpGlobalSyms->FindSymbol( ubasis );
	else
	{
		DSAFE_DELETE( mpCustomUBasis );
		mpCustomUBasis = DNEW Matrix44( pCustomUBasis );
	}

	if ( vbasis )
		mpVBasis = mpGlobalSyms->FindSymbol( vbasis );
	else
	{
		DSAFE_DELETE( mpCustomVBasis );
		mpCustomVBasis = DNEW Matrix44( pCustomVBasis );
	}

	mUSteps	= ustep;
	mVSteps	= vstep;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdColor( const Color &color )
{
	mColor = color;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdOpacity( const Color &color )
{
	mOpacity = color;
	mpRevision->BumpRevision();
}

//==================================================================
static void addShaderParam(
					SlShaderInst		&shaInst,
					const SymbolList	&globalSyms,
					ParamList			&params,
					size_t				fromIdx,
					State				&state )
{
	const char *pName = params[ fromIdx ].PChar();

	// check for strings that require no extra param here.. if any ! (???)

	// follow for params that require a following param
	bool	hasNextParam = ( ( fromIdx + 1 ) < params.size() );
	if NOT( hasNextParam )
	{
		state.EXCEPTPrintf( "Expecting parameter !" );
	}

	// is it a global symbol ?
	const Symbol *pSym = globalSyms.FindSymbol( pName );

	// if it is, then add an instance of the symbol
	if ( pSym )
	{
		if NOT( pSym->IsUniform() )
			state.EXCEPTPrintf( "Inline params must be uniform ! (for now.. 8)" );
	}
	else
	{
		// if not, then see if it's a new symbol declaration in the form
		// <class> <type> <name>
		pSym = shaInst.mCallSymList.Add( pName, Symbol::STOR_PARAMETER );
	}

	SymbolI *pSymI = shaInst.mCallSymIList.AddInstance( *pSym, 1 );	// 1... uniform only !


	void *pData = pSymI->GetRWData();

	switch ( pSym->mType )
	{
	case Symbol::TYP_FLOAT:	{float tmp = params[fromIdx+1].Flt(); pSym->FillData( pData, 1, &tmp ); } break;
	case Symbol::TYP_POINT:	pSym->FillData( pData, 1, params[fromIdx+1].PFlt( 3 )	 ); break;
	case Symbol::TYP_VECTOR:pSym->FillData( pData, 1, params[fromIdx+1].PFlt( 3 )	 ); break;
	case Symbol::TYP_NORMAL:pSym->FillData( pData, 1, params[fromIdx+1].PFlt( 3 )	 ); break;
    case Symbol::TYP_HPOINT:pSym->FillData( pData, 1, params[fromIdx+1].PFlt( 4 )	 ); break;
	case Symbol::TYP_COLOR:	pSym->FillData( pData, 1, params[fromIdx+1].PFlt( NCOLS )); break;
	case Symbol::TYP_STRING:pSym->FillData( pData, 1, params[fromIdx+1].PChar()		 ); break;
	case Symbol::TYP_MATRIX:pSym->FillData( pData, 1, params[fromIdx+1].PFlt( 16 )	 ); break;
	default:
		state.EXCEPTPrintf( "Unsupported data type or this operation (?)" );
		break;
	}
}

//==================================================================
static void onError( const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[1024];
	vsnprintf( buff, _countof(buff)-1, pFmt, vl );

	va_end( vl );

	puts( buff );

	throw std::runtime_error( buff );
}

//==================================================================
static void onException( const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[1024];
	vsnprintf( buff, _countof(buff)-1, pFmt, vl );

	va_end( vl );

	puts( buff );
}

//==================================================================
static void onWarning( const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[1024];
	vsnprintf( buff, _countof(buff)-1, pFmt, vl );

	va_end( vl );

	puts( buff );
}

//==================================================================
SlShader *Attributes::loadShader( const char *pBasePath, const char *pAppResDir, const char *pSName )
{
	char	buff[1024];

	SlShader::CtorParams	params;
	params.pName			= pSName;
	params.pAppResDir		= pAppResDir;
	params.pSourceFileName	= buff;

#if !defined(DISABLE_SL_SHADERS)
	sprintf( buff, "%s/%s.sl", pBasePath, pSName );
	if NOT( DUT::FileExists( buff ) )
#endif
	{
		sprintf( buff, "%s/%s.rrasm", pBasePath, pSName );
		if NOT( DUT::FileExists( buff ) )
		{
			// couldn't find
			return NULL;
		}
	}

	SlShader *pShader = NULL;

	try {
		pShader = DNEW SlShader( params, mpState->GetFileManager() );
	} catch ( ... )
	{
		onException( "Could not assemble '%s' !", params.pSourceFileName );
		return NULL;
	}

	mpResManager->AddResource( pShader );

	return pShader;
}

//==================================================================
SlShader *Attributes::getShader( const char *pShaderName, const char *pAlternateName )
{

	SlShader	*pShader =
			(SlShader *)mpResManager->FindResource( pShaderName,
													ResourceBase::TYPE_SHADER );

	if ( pShader )
		return pShader;

	const char *pAppResDir = mpState->GetDefShadersDir();

	if ( pShader = loadShader( mpState->GetBaseDir(), pAppResDir, pShaderName ) )
		return pShader;

	if ( pShader = loadShader( pAppResDir, pAppResDir, pShaderName ) )
		return pShader;

	if ( pAlternateName )
		if ( pShader =
				(SlShader *)mpResManager->FindResource( pAlternateName,
														ResourceBase::TYPE_SHADER ) )
			return pShader;

	return NULL;
}

//==================================================================
void Attributes::getShaderParams(
						ParamList		&params,
						size_t			fromIdx,
						SlShaderInst	&shaderInst )
{
	for (size_t i=fromIdx; i < params.size(); ++i)
	{
		if NOT( params[i].IsString() )
			continue;

		const char *pName = params[i].PChar();

		// check for strings that require no extra param here.. if any !

		// follow for params that require a following param
		bool	hasNextParam = ((i+1) < params.size());
		if NOT( hasNextParam )
		{
			mpState->EXCEPTPrintf( "Expecting parameter !" );
			//return;
		}

		addShaderParam(
					shaderInst,
					*mpGlobalSyms,
					params,
					i,
					*mpState );
		i += 1;
	}
}

//==================================================================
void Attributes::cmdLightSource( ParamList &params, const Transform &xform, const Matrix44 &mtxWorldCam )
{
	const char	*pLightTypeName = "";

	// get type and ID
	if ( params.size() < 2 || !params[0].IsString() || !params[1].IsIntVal() )
	{
		mpState->EXCEPTPrintf( "Bad LightSource definition !" );
		//return;
	}

	const char *pShaderName = params[0].PChar();

	SlShader	*pShader = getShader( pShaderName, NULL );

	if NOT( pShader )
	{
		mpState->EXCEPTPrintf( "Could not find the light shader '%s' !", params[0].PChar() );
		//return;
	}

	LightSourceT	*pLight = DNEW LightSourceT();

/*
	if ( 0 == strcasecmp( "ambientlight", params[0].PChar() ) )
	{
		pLight->mType = LightSourceT::TYPE_AMBIENT;
	}
	else
	if ( 0 == strcasecmp( "distantlight", params[0].PChar() ) )
	{
		pLight->mType = LightSourceT::TYPE_DISTANT;
	}
	else
	if ( 0 == strcasecmp( "arealight", params[0].PChar() ) )
	{
		pLight->mType = LightSourceT::TYPE_DISTANT;
	}
*/

	pLight->moShaderInst = DNEW SlShaderInst( pShader, MP_GRID_MAX_SIZE );

	pLight->mID = params[1].Int();

	pLight->mIsAmbient = !pShader->mHasDirPosInstructions;

	getShaderParams( params, 2, *pLight->moShaderInst.Use() );

	//pLight->UpdateRend( xform, mtxWorldCam );

	size_t listIdx = mpState->AddLightSource( pLight );

	DASSERT( listIdx < 65536 );	// 16 bit limit..

	mActiveLights.find_or_push_back( (U16)listIdx );

	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdSurface( ParamList &params )
{
	if ( params.size() < 1 )
	{
		onError( "Missing parameters for 'surface' command !" );
	}

	const char *pShaderName;

	if ( mpState->mParams.mForcedSurfaceShader.length() )
		pShaderName = mpState->mParams.mForcedSurfaceShader.c_str();
	else
		pShaderName = params[0].PChar();

	SlShader	*pShader = getShader( pShaderName, "matte" );

	if ( pShader )
	{
		moSurfaceSHI = DNEW SlShaderInst( pShader );

		getShaderParams( params, 1, *moSurfaceSHI.Use() );
	}

	// $$$ should really check if the shader or any params really changed
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdDisplacement( ParamList &params )
{
	if ( params.size() < 1 )
	{
		onError( "Missing parameters for 'displacement' command !" );
	}

	const char *pShaderName = params[0].PChar();

	SlShader	*pShader = getShader( pShaderName, NULL );

	if ( pShader )
	{
		moDisplaceSHI = DNEW SlShaderInst( pShader );

		getShaderParams( params, 1, *moDisplaceSHI.Use() );
	}

	// $$$ should really check if the shader or any params really changed
	mpRevision->BumpRevision();
}

//==================================================================
}
