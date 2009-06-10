//==================================================================
/// RI_SlShaderAsmParser.cpp
///
/// Created by Davide Pasca - 2009/3/4
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include <memory>
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
struct OpCodeDef
{
	const char	*pName;
	u_int		OperCnt;
	bool		RightIsImmediate;
	OperTypeID	Types[3];
};

//==================================================================
static OpCodeDef	gsOpCodeDefs[] =
{
	"movss"		,		2,	0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,
	"movsv"		,		2,	0,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_NA,
	"movvs"		,		2,	0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,
	"movvv"		,		2,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,
							
	"absss"		,		2,	0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,
	"abssv"		,		2,	0,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_NA,
	"absvs"		,		2,	0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,
	"absvv"		,		2,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,
							
	"addss"		,		3,	0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,
	"addsv"		,		3,	0,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_F3,
	"addvs"		,		3,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,
	"addvv"		,		3,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,
							
	"subss"		,		3,	0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,
	"subsv"		,		3,	0,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_F3,
	"subvs"		,		3,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,
	"subvv"		,		3,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,
							
	"mulss"		,		3,	0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,
	"mulsv"		,		3,	0,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_F3,
	"mulvs"		,		3,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,
	"mulvv"		,		3,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,
							
	"divss"		,		3,	0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,
	"divsv"		,		3,	0,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_F3,
	"divvs"		,		3,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,
	"divvv"		,		3,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,
							
	"lds"		,		2,	1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"ldv"		,		4,	1,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
							
	"noise11"		,	2,	0,	OPRTYPE_F1,	OPRTYPE_F1, OPRTYPE_NA,
	"noise12"		,	2,	0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA,
	"noise13"		,	2,	0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA,
							
	"noise31"		,	2,	0,	OPRTYPE_F3,	OPRTYPE_F1, OPRTYPE_NA,
	"noise32"		,	2,	0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA,
	"noise33"		,	2,	0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA,
							
	"xcompsv"		,	2,	0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA,
	"ycompsv"		,	2,	0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA,
	"zcompsv"		,	2,	0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA,
	"setxcompvs"	,	2,	0,	OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,
	"setycompvs"	,	2,	0,	OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,
	"setzcompvs"	,	2,	0,	OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,
							
	"normalize"		,	2,	0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA,
	"faceforward"	,	3,	0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,
	"diffuse"		,	2,	0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA,
	"ambient"		,	1,	0,	OPRTYPE_F3,	OPRTYPE_NA, OPRTYPE_NA,
	"calculatenormal",	2,	0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA,
							
	"ret"			,	0,	0,	OPRTYPE_NA,	OPRTYPE_NA, OPRTYPE_NA,

	NULL
};

//==================================================================
/// ShaderAsmParser
//==================================================================
ShaderAsmParser::ShaderAsmParser( DUT::MemFile &file, SlShader *pShader, const char *pName ) :
	mpFile(&file),
	mpShader(pShader),
	mpName(pName)
{
	doParse( file );
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
		return false;

	if ( mpShader->mType != SlShader::TYPE_UNKNOWN )
		onError( "Shader start/type already defined !" );
	else
	{
		if ( curSection != SEC_CODE )
			onError( "Shader type must be defined in the .code block" );
		
		mpShader->mType = type;
		mpShader->mStartPC = mpShader->mCode.size();
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
				continue;

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
			printf( "For shader '%s' at line %i\n%i) %s", mpName, lineCnt+1, lineCnt+1, lineBuff );
			throw;
		}

		++lineCnt;
	}

	if ( mpShader->mType == SlShader::TYPE_UNKNOWN )
		onError( "Shader type undefined !" );
}

//==================================================================
void ShaderAsmParser::getVector( const char *pStr, float out_val[], int n )
{
	int	gotN;
	
	if ( n == 1 ) gotN = sscanf( pStr, "%f"			, out_val+0, out_val+1, out_val+2 ); else
	if ( n == 2 ) gotN = sscanf( pStr, "%f %f"		, out_val+0, out_val+1, out_val+2 ); else
	if ( n == 3 ) gotN = sscanf( pStr, "%f %f %f"	, out_val+0, out_val+1, out_val+2 ); else
	if ( n == 4 ) gotN = sscanf( pStr, "%f %f %f %f", out_val+0, out_val+1, out_val+2 ); else
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
void ShaderAsmParser::parseDataLine( char lineBuff[], int lineCnt )
{
	//printf( "SEC_DATA: %s\n", lineBuff );
	
	const char *pLineEnd = lineBuff + strlen( lineBuff );
	
	char *pTokCtx;
	char *pTok;
	
	auto_ptr<SlSymbol>	pSymbol( DNEW SlSymbol() );
	pSymbol->Reset();

	if NOT( pTok = strtok_r( lineBuff, " \t", &pTokCtx ) )
		return;

	pSymbol->mName = pTok;

	if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
	{
		onError( "ERROR: Expecting storage definition" );
	}
		
	if ( 0 == strcmp( pTok, "constant"  ) ) pSymbol->mStorage = SlSymbol::CONSTANT ; else
	if ( 0 == strcmp( pTok, "parameter" ) ) pSymbol->mStorage = SlSymbol::PARAMETER; else
	if ( 0 == strcmp( pTok, "temporary" ) ) pSymbol->mStorage = SlSymbol::TEMPORARY; else
	if ( 0 == strcmp( pTok, "global"	) ) pSymbol->mStorage = SlSymbol::GLOBAL   ; else
	{
		onError( "ERROR: Invalid storage definition: '%s'", pTok );
	}

	if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
	{
		onError( "ERROR: Expecting variability definition" );
	}

	if ( 0 == strcmp( pTok, "varying"	) )
	{
		pSymbol->mIsVarying = true;

		if ( pSymbol->mStorage == SlSymbol::CONSTANT )
		{
			onError( "ERROR: constant variables cannot be varying: '%s'", pTok );
		}
	}
	else
	if ( 0 == strcmp( pTok, "uniform"	) )
	{
		pSymbol->mIsVarying = false;
	}
	else
	{
		onError( "ERROR: Invalid variability definition: '%s'", pTok );
	}

/*
	if NOT( pSymbol->mIsVarying )
		if ( pSymbol->mStorage == SlSymbol::TEMPORARY )
			onError( "Use 'temporary' only for varying values !" );
*/

	if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
	{
		onError( "ERROR: Expecting type definition" );
	}

	if ( 0 == strcmp( pTok, "float"  ) ) pSymbol->mType = SlSymbol::FLOAT ; else
	if ( 0 == strcmp( pTok, "point"  ) ) pSymbol->mType = SlSymbol::POINT ; else
	if ( 0 == strcmp( pTok, "color"  ) ) pSymbol->mType = SlSymbol::COLOR ; else
	if ( 0 == strcmp( pTok, "string" ) ) pSymbol->mType = SlSymbol::STRING; else
	if ( 0 == strcmp( pTok, "vector" ) ) pSymbol->mType = SlSymbol::VECTOR; else
	if ( 0 == strcmp( pTok, "normal" ) ) pSymbol->mType = SlSymbol::NORMAL; else
	if ( 0 == strcmp( pTok, "matrix" ) ) pSymbol->mType = SlSymbol::MATRIX; else
	{
		onError( "ERROR: Invalid type definition: '%s'", pTok );
	}

	int	defParamCnt = 0;

	const char *pDefaultValueStr = pTok + strlen(pTok) + 1;
	if ( pDefaultValueStr < pLineEnd )
	{
		float	tmp[64];

		switch ( pSymbol->mType )
		{
		case SlSymbol::FLOAT : getVector( pDefaultValueStr, tmp, 1 ); break;
		case SlSymbol::POINT : getVector( pDefaultValueStr, tmp, 3 ); break;
		case SlSymbol::COLOR : getVector( pDefaultValueStr, tmp, 3 ); break;
		case SlSymbol::VECTOR:
		case SlSymbol::NORMAL: getVector( pDefaultValueStr, tmp, 3 ); break;
		case SlSymbol::STRING:
		case SlSymbol::MATRIX:
				onError( "Currently unsupported default value (^^;> '%s'", pTok );
				break;
		}

		pSymbol->AllocDefault( tmp );

		printf( "Default value '%s'\n", pDefaultValueStr );
	}
	else
	{
		if ( pSymbol->mStorage == SlSymbol::CONSTANT )
			onError( "ERROR: Missing constant value for '%s'", pTok );
	}

	mpShader->mSymbols.push_back( pSymbol.release() );
}

//==================================================================
const OpCodeDef	*ShaderAsmParser::findOpDef( const char *pOpName, u_int &opCodeIdx )
{
	for (size_t i=0; gsOpCodeDefs[i].pName != NULL; ++i)
		if ( 0 == strcasecmp( pOpName, gsOpCodeDefs[i].pName ) )
		{
			opCodeIdx = i;
			return gsOpCodeDefs + i;
		}
		
	return NULL;
}

//==================================================================
int ShaderAsmParser::findSymbol( const char *pName, bool ignoreCase ) const
{
	for (size_t i=0; i < mpShader->mSymbols.size(); ++i)
	{
		const char *pShaderSymName = mpShader->mSymbols[i]->mName.c_str();
		
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

	return -1;
}

//==================================================================
int ShaderAsmParser::findOrAddTempSymbol( const char *pName )
{
	int	idx = findSymbol( pName, true );
	if ( idx != -1 )
		return idx;
		
	size_t	len = strlen( pName );
	if ( len < 3 )
		return -1;

	if NOT(	pName[1] == 's' || pName[1] == 'S' ||
			pName[1] == 'v' || pName[1] == 'V' )
		return -1;

	int	retIdx = (int)mpShader->mSymbols.size();

	auto_ptr<SlSymbol>	pSymbol( DNEW SlSymbol() );
	pSymbol->mName		= pName;
	pSymbol->mStorage	= SlSymbol::TEMPORARY;
	pSymbol->mIsVarying	= true;
	pSymbol->mArraySize	= 0;
	pSymbol->mpValArray	= NULL;

	if ( pName[1] == 's' || pName[1] == 'S' )
	{
		// scalar base
		pSymbol->mType	= SlSymbol::FLOAT;
	}
	else
	{
		// vector base
		pSymbol->mType	= SlSymbol::VECTOR;
	}

	mpShader->mSymbols.push_back( pSymbol.release() );

	return retIdx;
}


//==================================================================
static OperTypeID getOperTypeFromSlSymbolType( SlSymbol::Type slSymType, bool &out_success )
{
	out_success = true;

	switch ( slSymType )
	{
	case SlSymbol::FLOAT:	return OPRTYPE_F1 ;
	//case SlSymbol:::		return OPRTYPE_F2 ;
	case SlSymbol::POINT:
	case SlSymbol::COLOR:
	case SlSymbol::VECTOR:
	case SlSymbol::NORMAL:
							return OPRTYPE_F3 ;
	//case SlSymbol:::		return OPRTYPE_F4 ;
	case SlSymbol::MATRIX:	return OPRTYPE_M44;
	
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
								SlSymbol::Type slSymType,
								OperTypeID otExpected,
								int reportOpIdx,
								const char *pReportOpName )
{
	bool	success;
	OperTypeID	otSolved = getOperTypeFromSlSymbolType( slSymType, success );

	if ( !success || otSolved != otExpected )
	{
		// following should except really..
		onError( "ERROR: Invalid operand type at position %i (%s)", reportOpIdx+1, pReportOpName );
	}
}

//==================================================================
void ShaderAsmParser::parseCode_handleOperImmediate( const char *pTok )
{
	SlCPUWord	word;

	float	val;
	if ( 1 != sscanf( pTok, "%f" , &val ) )
		onError( "ERROR: Invalid immediate value %s", pTok );

	word.mImmFloat.mValue = val;

	mpShader->mCode.push_back( word );
}

//==================================================================
void ShaderAsmParser::parseCode_handleOperSymbol( const char *pTok, const OpCodeDef *pOpDef, int operIdx )
{
	SlCPUWord	word;

	int	symbolIdx;
	if ( isTempSymbol( pTok ) )
	{
		symbolIdx = findOrAddTempSymbol( pTok );
	}
	else
	{
		symbolIdx = findSymbol( pTok, false );
	}

	if ( symbolIdx == -1 )
	{
		onError( "ERROR: Symbol '%s' unknown !", pTok );
	}

	word.mSymbol.mTableOffset	= (u_int)symbolIdx;
	word.mSymbol.mIsVarying		= mpShader->mSymbols[symbolIdx]->mIsVarying;
	word.mSymbol.mpOrigSymbol	= mpShader->mSymbols[symbolIdx];

	mpShader->mCode.push_back( word );

	// verify that the symbol type matches with the operator type
	verifySymbolType(
				mpShader->mSymbols[symbolIdx]->mType,
				pOpDef->Types[operIdx],
				operIdx,
				pTok );
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
	
	u_int	opCodeIdx;
	const OpCodeDef	*pOpDef = findOpDef( pTok, opCodeIdx );
	
	if NOT( pOpDef )
	{
		onError( "ERROR: Unknown opcode '%s' !\n", pTok );
	}

	SlCPUWord		instruction;
	instruction.mOpCode.mTableOffset	= opCodeIdx;
	instruction.mOpCode.mOperandCount	= pOpDef->OperCnt;
	instruction.mOpCode.mDestOpType		= 0;
	instruction.mOpCode.mDbgLineNum		= lineCnt;

	size_t	instrIdx = mpShader->mCode.size();
	mpShader->mCode.push_back( instruction );

	for (u_int i=0; i < pOpDef->OperCnt; ++i)
	{
		if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
		{
			onError( "ERROR: missing operand #%i, "
					"expecting %i operands",
					i+1,
					pOpDef->OperCnt );
		}

		DUT::StrStripBeginEndWhite( pTok );

		if ( i > 0 && pOpDef->RightIsImmediate )
			parseCode_handleOperImmediate( pTok );
		else
			parseCode_handleOperSymbol( pTok, pOpDef, i );

		// copy the type of the first operator into the opcode
		if ( i == 0 )
		{
			instruction.mOpCode.mDestOpType = pOpDef->Types[i];
		}
	}

	// write again
	mpShader->mCode[instrIdx] = instruction;
}

//==================================================================
void ShaderAsmParser::onError( const char *pFmt, ... )
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

