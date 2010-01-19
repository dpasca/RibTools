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
#include "RI_Res_SearchPathScanner.h"

//==================================================================
namespace RI
{

//==================================================================
static void addShaderParam(
					SVM::ShaderInst		&shaInst,
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
						SVM::ShaderInst	&shaderInst,
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
SVM::Shader *Attributes::loadShader( const char *pBasePath, const char *pBaseIncDir, const char *pSName, bool &out_fileExists )
{
	FileManagerBase	&fmanager = mpState->GetFileManager();

	char	buff[1024];

	sprintf( buff, "%s/%s.sl", pBasePath, pSName );
	if NOT( fmanager.FileExists( buff ) )
	{
		sprintf( buff, "%s/%s.rrasm", pBasePath, pSName );

		if NOT( fmanager.FileExists( buff ) )
		{
			out_fileExists = false;
			return NULL;
		}
	}

	out_fileExists = true;

	SVM::Shader::CtorParams	params;
	params.pName			= pSName;
	params.pBaseIncDir		= pBaseIncDir;
	params.pSourceFileName	= buff;

	SVM::Shader *pShader = NULL;

	try {
		pShader = DNEW SVM::Shader( params, mpState->GetFileManager() );
	} catch ( ... )
	{
		mpState->EXCEPTPrintf( "Could not assemble '%s' !", params.pSourceFileName );
		return NULL;
	}

	mpResManager->AddResource( pShader );

	return pShader;
}

//==================================================================
SVM::Shader *Attributes::getShader( const char *pShaderName, const char *pAlternateName )
{
	// try see if we have it loaded already
	SVM::Shader	*pShader =
			(SVM::Shader *)mpResManager->FindResource( pShaderName,
													ResourceBase::TYPE_SHADER );

	if ( pShader )
		return pShader;

	const char *pBaseIncDir = mpState->GetDefShadersDir();

	// try look into the search path
	const DVec<DStr>	&spaths = mpState->GetCurOptions().mSearchPaths[ Options::SEARCHPATH_SHADER ];

	SearchPathScanner	pathScanner( mpState->GetBaseDir(), pBaseIncDir, spaths );

	std::string	usePath;
	bool		usePathIsAbsolute;

	while ( pathScanner.GetNext( usePath, usePathIsAbsolute ) )
	{
		bool	found;
		if ( pShader = loadShader( usePath.c_str(), pBaseIncDir, pShaderName, found ) )
			return pShader;

		if ( !found && !usePathIsAbsolute )
		{
			// WARNING: tricky path discovery.. we also try ribfilepath/searchpath
			usePath = std::string( mpState->GetBaseDir() ) + "/" + usePath;

			if ( pShader = loadShader( usePath.c_str(), pBaseIncDir, pShaderName, found ) )
				return pShader;
		}

		// in case we found it, but there is an error of some sort..
		// ..just give up instead of trying different paths
		if ( found )
			break;
	}

	if ( pAlternateName )
	{
		if ( pShader =
				(SVM::Shader *)mpResManager->FindResource( pAlternateName,
														ResourceBase::TYPE_SHADER ) )
			return pShader;
	}

	return NULL;
}

//==================================================================
}
