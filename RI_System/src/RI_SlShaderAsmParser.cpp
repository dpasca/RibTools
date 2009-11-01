//==================================================================
/// RI_SlShaderAsmParser.cpp
///
/// Created by Davide Pasca - 2009/3/4
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "stdafx.h"
#include <memory>
#include <stdexcept>
#include <stdarg.h>
#include "DUtils.h"
#include "RI_SlRunContext.h"
#include "RI_SlShaderAsmParser.h"
#include "RI_MicroPolygonGrid.h"

//==================================================================
using std::auto_ptr;

//==================================================================
namespace RI
{

//==================================================================
#define OPC_FLG_RIGHTISIMM		1
#define OPC_FLG_UNIFORMOPERS	2
#define OPC_FLG_FUNCOP_BEGIN	4
#define OPC_FLG_FUNCOP_END		8

//==================================================================
struct OpCodeDef
{
	const char	*pName;
	u_int		OperCnt;
	u_int		Flags;
	OperTypeID	Types[5];
};

//==================================================================
static const char *gpDefParamBaseLabelName = "__defparam_";
static const char *gpMainLabelName = "__main";

//==================================================================
static OpCodeDef	gsOpCodeDefs[] =
{
	"ret"			,	0,			0,	OPRTYPE_NA,	OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
																					   
	"mov.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"mov.vs"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"mov.vv"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"mov.xx"		,	2,			0,	OPRTYPE_STR,OPRTYPE_STR,OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"abs.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"abs.vs"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"abs.vv"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"sign.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"add.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"add.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"add.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
	"add.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"sub.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"sub.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"sub.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
	"sub.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"mul.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"mul.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"mul.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
	"mul.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"div.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"div.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"div.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
	"div.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,

	"mov.vs3"		,	4,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,

	"dot.svv"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,

	"ld.s"			,	2,			OPC_FLG_RIGHTISIMM,		OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"ld.v"			,	4,			OPC_FLG_RIGHTISIMM,		OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"cmplt"			,	3,			OPC_FLG_UNIFORMOPERS,	OPRTYPE_F1,	OPRTYPE_F1,OPRTYPE_ADDR,OPRTYPE_NA,OPRTYPE_NA,

	"noise11"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"noise12"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"noise13"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"noise31"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F1, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"noise32"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"noise33"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"xcomp.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
	"ycomp.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
	"zcomp.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
	"setxcomp.vs"	,	2,			0,	OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"setycomp.vs"	,	2,			0,	OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"setzcomp.vs"	,	2,			0,	OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"pxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,
	"vxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,
	"nxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,
	"cxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,

	"normalize"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
	"faceforward"	,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,
	"diffuse"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
	"ambient"		,	1,			0,	OPRTYPE_F3,	OPRTYPE_NA, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"calculatenormal",	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"solarbegin.vs",	2,			OPC_FLG_FUNCOP_BEGIN, OPRTYPE_F3, OPRTYPE_F1, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"illuminance.vvs",	3,			OPC_FLG_FUNCOP_BEGIN, OPRTYPE_F3, OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"illuminance.v",	1,			OPC_FLG_FUNCOP_BEGIN, OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"funcopend"		,	0,			OPC_FLG_FUNCOP_END,	  OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	NULL			,	0,			0,	OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA
};

//==================================================================
/// ShaderAsmParser
//==================================================================
ShaderAsmParser::ShaderAsmParser( DUT::MemFile &file, SlShader *pShader, const char *pName ) :
	mpShader(pShader),
	mpName(pName),
	mFuncOpBeginIdx(DNPOS)
{
	doParse( file );

	resolveLabels();
}

//==================================================================
static void stripComments( char *pTxt )
{
	while ( pTxt[0] )
	{
		if ( pTxt[0] == ';' )
		{
			pTxt[0] = 0;
			return;
		}

		if ( pTxt[0] == '/' )
			if ( pTxt[1] == '/' )
			{
				pTxt[0] = 0;
				return;
			}

		++pTxt;
	}
}

//==================================================================
static bool beginsWithTokenI( const char *pStr, const char *pMatch )
{
	size_t	strLen = strlen( pStr );
	size_t	matchLen = strlen( pMatch );

	if ( strLen > matchLen &&
		 DUT::StrStrI( pStr, pMatch ) == pStr )
	{
		return
			pStr[matchLen] == ' ' ||
			pStr[matchLen] == '\t' ||
			pStr[matchLen] == '\n' ||
			pStr[matchLen] == '\r' ||
			pStr[matchLen] == '\f';
	}

	return false;
}

//==================================================================
bool ShaderAsmParser::handleShaderTypeDef( const char *pLineWork, Section curSection )
{
	SlShader::Type	type = SlShader::TYPE_UNKNOWN;

	if ( !strcasecmp( pLineWork, "light" ) )		type = SlShader::TYPE_LIGHT;	else
	if ( !strcasecmp( pLineWork, "surface" ) )		type = SlShader::TYPE_SURFACE;	else
	if ( !strcasecmp( pLineWork, "volume" ) )		type = SlShader::TYPE_VOLUME;	else
	if ( !strcasecmp( pLineWork, "displacement" ) )	type = SlShader::TYPE_DISPLACEMENT;	else
	if ( !strcasecmp( pLineWork, "transformation" ) )type = SlShader::TYPE_TRANSFORMATION;	else
	if ( !strcasecmp( pLineWork, "imager" ) )		type = SlShader::TYPE_IMAGER;

	if ( type == SlShader::TYPE_UNKNOWN )
	{
		if (
			beginsWithTokenI( pLineWork, "light" ) 			||
			beginsWithTokenI( pLineWork, "surface" )		||
			beginsWithTokenI( pLineWork, "volume" )			||
			beginsWithTokenI( pLineWork, "displacement" )	||
			beginsWithTokenI( pLineWork, "transformation" )	||
			beginsWithTokenI( pLineWork, "imager" ) )
		{
			onError( "Shader start/type needs to be defined alone in a line" );
		}

		return false;
	}

	if ( mpShader->mType != SlShader::TYPE_UNKNOWN )
		onError( "Shader start/type already defined !" );
	else
	{
		if ( curSection != SEC_CODE )
			onError( "Shader type must be defined in the .code block" );

		mpShader->mType = type;
	}

	return true;
}

//==================================================================
void ShaderAsmParser::doParse( DUT::MemFile &file )
{
	char		lineBuff[1024];
	Section		curSection = SEC_UNDEF;
	int			lineCnt = 0;

	while ( file.ReadTextLine( lineBuff, sizeof(lineBuff) ) )
	{
		char	lineWork[1024];

		strcpy_s( lineWork, lineBuff );

		stripComments( lineWork );

		DUT::StrStripBeginEndWhite( lineWork );

		if NOT( lineWork[0] )
		{
			++lineCnt;
			continue;
		}

		try
		{
			if ( handleShaderTypeDef( lineWork, curSection ) )
			{
				++lineCnt;
				continue;
			}

			if ( 0 == strcasecmp( lineWork, ".data" ) )
				curSection = SEC_DATA;
			else
			if ( 0 == strcasecmp( lineWork, ".code" ) )
				curSection = SEC_CODE;
			else
			if ( curSection == SEC_DATA )
			{
				parseDataLine( lineWork, lineCnt );
			}
			else
			if ( curSection == SEC_CODE )
			{
				parseCodeLine( lineWork, lineCnt );
			}
		}
		catch ( ... )
		{
			printf( "For shader '%s' at line %i\n%i) %s\n", mpName, lineCnt+1, lineCnt+1, lineBuff );
			throw;
		}

		++lineCnt;
	}

	if ( mpShader->mType == SlShader::TYPE_UNKNOWN )
		onError( "Shader type undefined !" );
}

//==================================================================
void ShaderAsmParser::processSpecialLabel( const Label &label )
{
	const char *pLabelName	= label.mName.c_str();

	// is a default param value label ?
	if ( strstr( pLabelName, gpDefParamBaseLabelName ) == pLabelName )
	{
		u_int		labelAddr	= label.mAddress;

		const char *pVarName = pLabelName + strlen( gpDefParamBaseLabelName );

		size_t	idx = findSymbol( pVarName, false );
		if ( idx == DNPOS )
			return;

		// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		// mDefaultValStartPC should be in a separate list in the shader.. without
		// being either in the symbol or symbol instance structure

		mpShader->mpShaSymsStartPCs[ idx ] = labelAddr;
	}
	else
	// is main/entry point label ?
	if ( strstr( pLabelName, gpMainLabelName ) == pLabelName )
	{
		u_int		labelAddr	= label.mAddress;

		mpShader->mStartPC = labelAddr;
	}
}

//==================================================================
void ShaderAsmParser::resolveLabels()
{
	for (size_t j=0; j < mLabelDefs.size(); ++j)
	{
		processSpecialLabel( mLabelDefs[j] );
	}

	// did we find the special label for the entry point ?
	if ( mpShader->mStartPC == INVALID_PC )
	{
		onError( "Entry point is missing !" );
	}

	for (size_t i=0; i < mLabelRefs.size(); ++i)
	{
		const Label	&ref = mLabelRefs[i];

		bool	found = false;
		for (size_t j=0; j < mLabelDefs.size(); ++j)
		{
			if ( 0 == strcmp( ref.mName.c_str(), mLabelDefs[j].mName.c_str() ) )
			{
				mpShader->mCode[ref.mAddress].mAddress.mOffset = mLabelDefs[j].mAddress;
				found = true;
			}
		}

		if NOT( found )
		{
			onError( "Undefined label %s", ref.mName.c_str() );
		}
	}
}

//==================================================================
void ShaderAsmParser::getVector( const char *pStr, float out_val[], int n )
{
	int	gotN;

	if ( n == 1 ) gotN = sscanf( pStr, "%f"			, out_val+0 ); else
	if ( n == 2 ) gotN = sscanf( pStr, "%f %f"		, out_val+0, out_val+1 ); else
	if ( n == 3 ) gotN = sscanf( pStr, "%f %f %f"	, out_val+0, out_val+1, out_val+2 ); else
	if ( n == 4 ) gotN = sscanf( pStr, "%f %f %f %f", out_val+0, out_val+1, out_val+2 , out_val+3 ); else
	{
		onError( "getVector() can't handle so many dimensions (^^;) !" );
	}

	if ( gotN != n && gotN != 1 )
		onError( "Got %i values, but %i (or 1) are required !", gotN, n );

	if ( gotN == 1 )
		for (int i=1; i < n; ++i)
			out_val[i] = out_val[0];
}

//==================================================================
void ShaderAsmParser::getString( const char *pStr, SlStr &out_str )
{
	const char *pStart = strstr( pStr, "\"" );

	if NOT( pStart )
		onError( "Malformed string ? '%s' ", pStr );

	out_str.mStr[0] = 0;

	++pStart;

	size_t	destIdx = 0;

	while ( pStart[0] )
	{
		if ( pStart[0] == '"' )
		{
			out_str.mStr[destIdx] = 0;
			return;
		}
		else
			out_str.mStr[destIdx] = pStart[0];

		++destIdx;
		++pStart;
	}

	onError( "Malformed string ? '%s' ", pStr );
}


//==================================================================
void ShaderAsmParser::parseDataLine( char lineBuff[], int lineCnt )
{
	//printf( "SEC_DATA: %s\n", lineBuff );

	const char *pLineEnd = lineBuff + strlen( lineBuff );

	char *pTokCtx;
	char *pTok;

	Symbol::CtorParams	params;


	if NOT( pTok = strtok_r( lineBuff, " \t", &pTokCtx ) )
		return;

	params.mpName = pTok;

	if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
	{
		onError( "Expecting storage definition" );
	}

	if ( 0 == strcmp( pTok, "parameter" ) ) params.mStorage = Symbol::STOR_PARAMETER; else
	if ( 0 == strcmp( pTok, "temporary" ) ) params.mStorage = Symbol::STOR_TEMPORARY; else
	if ( 0 == strcmp( pTok, "global"	) ) params.mStorage = Symbol::STOR_GLOBAL   ; else
	{
		onError( "Invalid storage definition: '%s'", pTok );
	}

	if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
		onError( "Expecting class definition" );

	if ( 0 == strcmp( pTok, "uniform"	) )	{ params.mClass	= Symbol::CLASS_MSK_UNIFORM;	} else
	if ( 0 == strcmp( pTok, "varying"	) )	{ params.mClass	= Symbol::CLASS_MSK_VARYING;	} else
	if ( 0 == strcmp( pTok, "vertex"	) )	{ params.mClass	= Symbol::CLASS_MSK_VERTEX;		} else
	if ( 0 == strcmp( pTok, "constant"  ) )	{ params.mClass	= Symbol::CLASS_MSK_CONSTANT;	} else
	{
		onError( "Invalid class definition: '%s'", pTok );
	}

	if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
		onError( "Expecting type definition" );

	if ( 0 == strcmp( pTok, "float"  ) ) params.mType = Symbol::TYP_FLOAT ; else
	if ( 0 == strcmp( pTok, "point"  ) ) params.mType = Symbol::TYP_POINT ; else
	if ( 0 == strcmp( pTok, "hpoint"  ) )params.mType = Symbol::TYP_HPOINT; else
	if ( 0 == strcmp( pTok, "color"  ) ) params.mType = Symbol::TYP_COLOR ; else
	if ( 0 == strcmp( pTok, "string" ) ) params.mType = Symbol::TYP_STRING; else
	if ( 0 == strcmp( pTok, "vector" ) ) params.mType = Symbol::TYP_VECTOR; else
	if ( 0 == strcmp( pTok, "normal" ) ) params.mType = Symbol::TYP_NORMAL; else
	if ( 0 == strcmp( pTok, "matrix" ) ) params.mType = Symbol::TYP_MATRIX; else
	{
		onError( "Invalid type definition: '%s'", pTok );
	}

	auto_ptr<Symbol>	pSymbol( DNEW Symbol( params ) );

	int	defParamCnt = 0;

	const char *pConstValueStr = pTok + strlen(pTok) + 1;
	if ( pConstValueStr < pLineEnd )
	{
		if NOT( pSymbol->IsConstant() )
			onError( "Explicit value supported only for constants. '%s' is not constant.", pTok );

		if ( pSymbol->mStorage == Symbol::STOR_GLOBAL )
			onError( "Global storage constants ('%s') should not define constant values themselves !", pTok );

		switch ( pSymbol->mType )
		{
		case Symbol::TYP_FLOAT :
			{
				float	tmp[4];
				getVector( pConstValueStr, tmp, 1 );
				pSymbol->InitConstValue( tmp );
			}
			break;

		case Symbol::TYP_COLOR :
			{
				float	tmp[4];
				getVector( pConstValueStr, tmp, 3 );	// assumes color is 3 components
				pSymbol->InitConstValue( tmp );
			}
			break;

		case Symbol::TYP_POINT :
		case Symbol::TYP_VECTOR:
		case Symbol::TYP_NORMAL:
			{
				float	tmp[4];
				getVector( pConstValueStr, tmp, 3 );
				pSymbol->InitConstValue( tmp );
			}
			break;

		case Symbol::TYP_HPOINT :
			{
				float	tmp[4];
				getVector( pConstValueStr, tmp, 4 );
				pSymbol->InitConstValue( tmp );
			}
			break;

		case Symbol::TYP_STRING:
			{
				SlStr	str;

				getString( pConstValueStr, str );
				pSymbol->InitConstValue( &str );
			}
			break;

		case Symbol::TYP_MATRIX:
				onError( "Currently unsupported default value (^^;> '%s'", pTok );
				break;
		}
	}
	else
	{
		if ( pSymbol->IsConstant() && pSymbol->mStorage != Symbol::STOR_GLOBAL )
			onError( "Missing constant value for '%s'", pTok );
	}

	mpShader->mpShaSyms.push_back( pSymbol.release() );
	mpShader->mpShaSymsStartPCs.push_back( INVALID_PC );
}

//==================================================================
const OpCodeDef	*ShaderAsmParser::findOpDef( const char *pOpName, u_int &opCodeIdx )
{
	for (size_t i=0; gsOpCodeDefs[i].pName != NULL; ++i)
		if ( 0 == strcasecmp( pOpName, gsOpCodeDefs[i].pName ) )
		{
			opCodeIdx = (u_int)i;
			return gsOpCodeDefs + i;
		}

	return NULL;
}

//==================================================================
size_t ShaderAsmParser::findSymbol( const char *pName, bool ignoreCase ) const
{
	for (size_t i=0; i < mpShader->mpShaSyms.size(); ++i)
	{
		const char *pShaderSymName = mpShader->mpShaSyms[i]->mName.c_str();

		if ( ignoreCase )
		{
			if ( 0 == strcasecmp( pName, pShaderSymName ) )
				return i;
		}
		else
		{
			if ( 0 == strcmp( pName, pShaderSymName ) )
				return i;
		}
	}

	return DNPOS;
}

//==================================================================
size_t ShaderAsmParser::findOrAddTempSymbol( const char *pName )
{
	size_t	idx = findSymbol( pName, true );
	if ( idx != DNPOS )
		return idx;

	size_t	len = strlen( pName );
	if ( len < 3 )
		return -1;

	char typeLower = tolower( pName[1] );

	if NOT(	typeLower == 's' ||
			typeLower == 'v' ||
			typeLower == 'x' )
		return -1;

	int	retIdx = (int)mpShader->mpShaSyms.size();

	bool	isVarying = true;

	// uniform register ?
	if ( pName[2] == 'u' || pName[2] == 'U' )
	{
		if ( len < 4 )
			return -1;

		isVarying = false;
	}

	auto_ptr<Symbol>	pSymbol( DNEW Symbol() );
	pSymbol->mName		= pName;
	pSymbol->mStorage	= Symbol::STOR_TEMPORARY;

	if ( isVarying )
		pSymbol->SetVarying();
	else
		pSymbol->SetUniform();

	if ( typeLower == 's' )
	{
		// scalar base
		pSymbol->mType	= Symbol::TYP_FLOAT;
	}
	else
	if ( typeLower == 'v' )
	{
		// vector base
		pSymbol->mType	= Symbol::TYP_VECTOR;
	}
	else
	{
		// string
		pSymbol->mType	= Symbol::TYP_STRING;
	}

	mpShader->mpShaSyms.push_back( pSymbol.release() );
	mpShader->mpShaSymsStartPCs.push_back( INVALID_PC );

	return retIdx;
}


//==================================================================
static OperTypeID getOperTypeFromSlSymbolType( Symbol::Type slSymType, bool &out_success )
{
	out_success = true;

	switch ( slSymType )
	{
	case Symbol::TYP_FLOAT:	return OPRTYPE_F1 ;

	case Symbol::TYP_POINT:
	case Symbol::TYP_VECTOR:
	case Symbol::TYP_NORMAL:	return OPRTYPE_F3 ;

	case Symbol::TYP_HPOINT:	return OPRTYPE_F4 ;

	case Symbol::TYP_COLOR:		return OPRTYPE_F3 ;

	case Symbol::TYP_MATRIX:	return OPRTYPE_M44;
	case Symbol::TYP_STRING:	return OPRTYPE_STR;

	default:
		out_success = false;
		return OPRTYPE_F1;
	}
}

//==================================================================
static bool isTempSymbol( const char *pTok )
{
	return pTok[0] == '$';
}

//==================================================================
void ShaderAsmParser::verifySymbolType(
								Symbol::Type slSymType,
								OperTypeID otExpected,
								int reportOpIdx,
								const char *pReportOpName )
{
	bool	success;
	OperTypeID	otSolved = getOperTypeFromSlSymbolType( slSymType, success );

	if ( !success || otSolved != otExpected )
	{
		// following should except really..
		onError( "Invalid operand type at position %i (%s)", reportOpIdx+1, pReportOpName );
	}
}

//==================================================================
void ShaderAsmParser::parseCode_handleOperImmediate( const char *pTok )
{
	SlCPUWord	word;

	float	val;
	if ( 1 != sscanf( pTok, "%f" , &val ) )
		onError( "Invalid immediate value %s", pTok );

	word.mImmFloat.mValue = val;

	mpShader->mCode.push_back( word );
}

//==================================================================
void ShaderAsmParser::parseCode_handleOperSymbol( const char *pTok, const OpCodeDef *pOpDef, int operIdx )
{
	OperTypeID	expectedOperType = pOpDef->Types[operIdx];

	SlCPUWord	word;

	if ( expectedOperType == OPRTYPE_ADDR )
	{
		// look for a label
		Label	*pLabel = mLabelRefs.grow();
		pLabel->mAddress = (u_int)mpShader->mCode.size();	// pointer to the CPU word that needs to be resolved later
		pLabel->mName	 = pTok;
		word.mAddress.mOffset = 0xcfcfcfcf;	// dummy value for now..
	}
	else
	{
		size_t	symbolIdx;
		if ( isTempSymbol( pTok ) )
		{
			symbolIdx = findOrAddTempSymbol( pTok );
		}
		else
		{
			symbolIdx = findSymbol( pTok, false );
		}

		if ( symbolIdx == DNPOS )
		{
			onError( "Symbol '%s' unknown !", pTok );
		}

		word.mSymbol.mTableOffset	= (u_int)symbolIdx;
		word.mSymbol.mIsVarying		= mpShader->mpShaSyms[symbolIdx]->IsVarying();
		word.mSymbol.mpOrigSymbol	= mpShader->mpShaSyms[symbolIdx];

		if ( pOpDef->Flags & OPC_FLG_UNIFORMOPERS )
		{
			if ( word.mSymbol.mIsVarying )
			{
				onError( "All operators must be uniform !", pTok );
			}
		}

		// verify that the symbol type matches with the operator type
		verifySymbolType(
					mpShader->mpShaSyms[symbolIdx]->mType,
					expectedOperType,
					operIdx,
					pTok );
	}

	mpShader->mCode.push_back( word );
}

//==================================================================
bool ShaderAsmParser::parseLabelDef( const char *pTok )
{
	size_t	len = strlen( pTok );

	if ( len && pTok[len-1] == ':' )
	{
		if ( len < 2 )
			onError( "Label has no name ?!\n" );

		Label *pLabel = mLabelDefs.grow();

		pLabel->mName = pTok;	// copy without the colon..
		pLabel->mName.resize( len - 1 );

		pLabel->mAddress = (u_int)mpShader->mCode.size();	// pointer to the CPU word of this label

		return true;
	}
	else
		return false;
}

//==================================================================
void ShaderAsmParser::parseCodeLine( char lineBuff[], int lineCnt )
{
	//printf( "SEC_CODE: %s\n", lineBuff );

	char *pTokCtx;
	char *pTok;

	if NOT( pTok = strtok_r( lineBuff, " \t", &pTokCtx ) )
		return;

	DUT::StrStripBeginEndWhite( pTok );

	if ( parseLabelDef( pTok ) )
		return;

	u_int	opCodeIdx;
	const OpCodeDef	*pOpDef = findOpDef( pTok, opCodeIdx );

	if NOT( pOpDef )
	{
		onError( "Unknown opcode '%s' !\n", pTok );
	}

	size_t	instrIdx = mpShader->mCode.size();

	if ( pOpDef->Flags & OPC_FLG_FUNCOP_BEGIN )
	{
		if ( mFuncOpBeginIdx != DNPOS )
		{
			onError( "Instruction %s cannot be nested !", pOpDef->pName );
		}
		else
			mFuncOpBeginIdx = instrIdx;
	}
	else
	if ( pOpDef->Flags & OPC_FLG_FUNCOP_END )
	{
		if ( mFuncOpBeginIdx == DNPOS )
		{
			onError( "Missing matching instruction for %s ", pOpDef->pName );
		}

		mpShader->mCode[mFuncOpBeginIdx].mOpCode.mFuncopEndAddr = mFuncOpBeginIdx;
		mFuncOpBeginIdx = DNPOS;
	}

	SlCPUWord		instruction;
	instruction.mOpCode.mTableOffset	= opCodeIdx;
	instruction.mOpCode.mOperandCount	= pOpDef->OperCnt;
	instruction.mOpCode.mFuncopEndAddr	= 0;
	instruction.mOpCode.mDbgLineNum		= lineCnt;

	mpShader->mCode.push_back( instruction );

	for (u_int i=0; i < pOpDef->OperCnt; ++i)
	{
		if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
		{
			onError( "missing operand #%i, "
					"expecting %i operands",
					i+1,
					pOpDef->OperCnt );
		}

		DUT::StrStripBeginEndWhite( pTok );

		if ( i > 0 && (pOpDef->Flags & OPC_FLG_RIGHTISIMM) )
			parseCode_handleOperImmediate( pTok );
		else
			parseCode_handleOperSymbol( pTok, pOpDef, i );
	}

	// write again
	mpShader->mCode[instrIdx] = instruction;
}

//==================================================================
void ShaderAsmParser::onError( const char *pFmt, ... ) const
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
}
