//==================================================================
/// RI_SlAsmParser.h
///
/// Created by Davide Pasca - 2009/3/4
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLASMPARSER_H
#define RI_SLASMPARSER_H

#include "RI_Base.h"
#include "RI_SlShader.h"
#include "DSystem/include/DUtils_MemFile.h"

//==================================================================
namespace RI
{

//==================================================================
struct SlAsmOpCodeDef;

//==================================================================
/// SlAsmParser
//==================================================================
class SlAsmParser
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

	DVec<size_t>	mFuncOpBeginIdx;
	
public:
	//==================================================================
	SlAsmParser( DUT::MemFile &file, SlShader *pShader, const char *pName );

private:
	void doParse( DUT::MemFile &file );
	void processSpecialLabel( const Label &label );
	void resolveLabels();
	bool handleShaderTypeDef( const char *pLineWork, Section curSection );

	void parseDataLine( char lineBuff[], int lineCnt );
	const SlAsmOpCodeDef	*findOpDef( const char *pOpName, u_int &opCodeIdx );
	size_t findSymbol( const char *pName, bool ignoreCase ) const;
	size_t findOrAddTempSymbol( const char *pName );
	void parseCodeLine( char lineBuff[], int lineCnt );
	void parseCode_handleOperImmediate( const char *pTok );
	void parseCode_handleOperSymbol( const char *pTok, const SlAsmOpCodeDef *pOpDef, int operIdx );
	bool parseLabelDef( const char *pTok );
	void getVector( const char *pStr, float out_val[], int n );
	void getString( const char *pStr, SlStr &out_str );

	void onError( const char *pFmt, ... ) const;

	void verifySymbolType(	Symbol::Type slSymType,
							OperTypeID otExpected,
							int reportOpIdx,
							const char *pReportOpName );
};

//==================================================================
}

#endif
