//==================================================================
/// RI_RRASM_Parser.h
///
/// Created by Davide Pasca - 2010/1/10
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_RRASM_PARSER_H
#define RI_RRASM_PARSER_H

#include "RI_Base.h"
#include "RI_SVM_Shader.h"
#include "DSystem/include/DUtils_MemFile.h"

//==================================================================
namespace RI
{
//==================================================================
namespace RRASM
{

//==================================================================
struct OpCodeDef;

//==================================================================
/// Parser
//==================================================================
class Parser
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

    SVM::Shader		*mpShader;
    const char		*mpName;
    DVec<Label>		mLabelDefs;
    DVec<Label>		mLabelRefs;

    DVec<size_t>	mFuncOpBeginIdx;
    
public:
    //==================================================================
    Parser( DUT::MemFile &file, SVM::Shader *pShader, const char *pName );

private:
    void doParse( DUT::MemFile &file );
    void processSpecialLabel( const Label &label );
    void resolveLabels();
    bool handleShaderTypeDef( const char *pLineWork, Section curSection );

    void parseDataLine( char lineBuff[], int lineCnt );
    const OpCodeDef	*findOpDef( const char *pOpName, u_int &opCodeIdx );
    size_t findSymbol( const char *pName, bool ignoreCase ) const;
    size_t findOrAddTempSymbol( const char *pName );
    void parseCodeLine( char lineBuff[], int lineCnt );
    void parseCode_handleOperImmediate( const char *pTok );
    void parseCode_handleOperSymbol( const char *pTok, const OpCodeDef *pOpDef, int operIdx );
    bool parseLabelDef( const char *pTok );
    void getVector( const char *pStr, float out_val[], int n );
    void getString( const char *pStr, SlStr &out_str );

    void onError( const char *pFmt, ... ) const;
};

//==================================================================
}
//==================================================================
}

#endif
