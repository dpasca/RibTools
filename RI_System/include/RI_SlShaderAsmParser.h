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
#include "DSystem/include/DUtils_MemFile.h"

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
		SEC_UNDEF,
		SEC_DATA,
		SEC_CODE,
	};

	struct Label
	{
		DStr	mName;
		u_int	mAddress;
	};

	SlShader		*mpShader;
	const char		*mpName;
	DVec<Label>		mLabelDefs;
	DVec<Label>		mLabelRefs;
	
public:
	//==================================================================
	ShaderAsmParser( DUT::MemFile &file, SlShader *pShader, const char *pName );

private:
	void doParse( DUT::MemFile &file );
	void resolveLabels();
	bool handleShaderTypeDef( const char *pLineWork, Section curSection );

	void parseDataLine( char lineBuff[], int lineCnt );
	const OpCodeDef	*findOpDef( const char *pOpName, u_int &opCodeIdx );
	int findSymbol( const char *pName, bool ignoreCase ) const;
	int findOrAddTempSymbol( const char *pName );
	void parseCodeLine( char lineBuff[], int lineCnt );
	void parseCode_handleOperImmediate( const char *pTok );
	void parseCode_handleOperSymbol( const char *pTok, const OpCodeDef *pOpDef, int operIdx );
	bool parseLabelDef( const char *pTok );
	void getVector( const char *pStr, float out_val[], int n );

	void onError( const char *pFmt, ... );

	void verifySymbolType(	Symbol::Type slSymType,
							OperTypeID otExpected,
							int reportOpIdx,
							const char *pReportOpName );
};

//==================================================================
}

#endif
