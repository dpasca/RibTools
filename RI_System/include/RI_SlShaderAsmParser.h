//==================================================================
/// RI_SlShaderAsmParser.h
///
/// Created by Davide Pasca - 2009/3/4
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSHADERASMPARSER_H
#define RI_SLSHADERASMPARSER_H

#include "RI_Base.h"
#include "RI_SlShader.h"

//==================================================================
namespace RI
{

//==================================================================
struct OpCodeDef;

//==================================================================
/// ShaderAsmParser
//==================================================================
class ShaderAsmParser
{
	enum Section
	{
		DATA,
		CODE,
	};
	
	SlShader		*mpShader;
	DUT::MemFile	*mpFile;
	const char		*mpName;
	
public:
	//==================================================================
	ShaderAsmParser( DUT::MemFile &file, SlShader *pShader, const char *pName );

private:
	void doParse( DUT::MemFile &file );
	void parseDataLine( char lineBuff[], int lineCnt );
	const OpCodeDef	*findOpDef( const char *pOpName, u_int &opCodeIdx );
	int findSymbol( const char *pName, bool ignoreCase ) const;
	int findOrAddTempSymbol( const char *pName );
	void parseCodeLine( char lineBuff[], int lineCnt );
	void getVector( const char *pStr, float out_val[], int n );

	void onError( const char *pFmt, ... );

	void verifySymbolType(	SlSymbol::Type slSymType,
							OperTypeID otExpected,
							int reportOpIdx,
							const char *pReportOpName );
};

//==================================================================
}

#endif
