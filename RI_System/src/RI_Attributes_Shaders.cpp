//==================================================================
/// RI_Attributes_Shaders.cpp
///
/// Created by Davide Pasca - 2009/11/26
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_State.h"
#include "RI_Attributes.h"

//==================================================================
namespace RI
{

//==================================================================
static void addShaderParam(
					SlShaderInst		&shaInst,
					const SymbolList	&globalSyms,
					ParamList			&params,
					size_t				fromIdx,
					const Matrix44		&mtxLocalCam,
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

	// HACK: "direction" becomes "to" ..for distant light annotation..
	bool paramIsDirection = false;
	if ( 0 == strcmp( pName, "direction" ) )
	{
		pName = "to";
		paramIsDirection = true;
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
	case Symbol::TYP_FLOAT:	{float tmp = params[fromIdx+1].Flt(); pSym->FillDataFromSISD( pData, 1, &tmp ); } break;

	// NOTE: Still just using the Transform Open.. this means not working for motion blur...
	case Symbol::TYP_POINT:
		{
			const float *p = params[fromIdx+1].PFlt( 3 );
			Float3_	vec( p[0], p[1], p[2] );

			if ( paramIsDirection )
			{
				// xyz1 * mtx
				vec = V3__V3W1_Mul_M44<Float_>( vec, mtxLocalCam );
			}
			else
			{
				// xyz1 * mtx
				vec = V3__V3W1_Mul_M44<Float_>( vec, mtxLocalCam );
			}

			*((Float3_ *)pData) = vec;
		}
		break;

	case Symbol::TYP_VECTOR:
		{
			const float *p = params[fromIdx+1].PFlt( 3 );
			Float3_	vec( p[0], p[1], p[2] );
			// xyz0 * mtx
			// TODO: confirm it really is W0 !
			vec = V3__V3W0_Mul_M44<Float_>( vec, state.GetCurTransformOpenMtx() );
			*((Float3_ *)pData) = vec;
		}
		break;

	case Symbol::TYP_NORMAL:
		{
			const float *p = params[fromIdx+1].PFlt( 3 );
			Float3_	vec( p[0], p[1], p[2] );
			// xyz0 * mtx
			// TODO: confirm it really is W0 !
			vec = V3__V3W0_Mul_M44<Float_>( vec, state.GetCurTransformOpenMtx() );
			*((Float3_ *)pData) = vec;
		}
		break;

	case Symbol::TYP_HPOINT:
		{
			const float *p = params[fromIdx+1].PFlt( 4 );
			Float4_	vec( p[0], p[1], p[2], p[3] );
			// xyzw * mtx
			vec = V4__V4_Mul_M44<Float_>( vec, state.GetCurTransformOpenMtx() );
			*((Float4_ *)pData) = vec;
		}
		break;

	case Symbol::TYP_COLOR:	pSym->FillDataFromSISD( pData, 1, params[fromIdx+1].PFlt( NCOLS )); break;
	case Symbol::TYP_STRING:pSym->FillDataFromSISD( pData, 1, params[fromIdx+1].PChar()		 ); break;
	case Symbol::TYP_MATRIX:pSym->FillDataFromSISD( pData, 1, params[fromIdx+1].PFlt( 16 )	 ); break;
	default:
		state.EXCEPTPrintf( "Unsupported data type or this operation (?)" );
		break;
	}
}

//==================================================================
void Attributes::getShaderParams(
						ParamList		&params,
						size_t			fromIdx,
						SlShaderInst	&shaderInst,
						const Matrix44	&mtxLocalCam )
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
					mtxLocalCam,
					*mpState );
		i += 1;
	}
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
		mpState->EXCEPTPrintf( "Could not assemble '%s' !", params.pSourceFileName );
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
}
