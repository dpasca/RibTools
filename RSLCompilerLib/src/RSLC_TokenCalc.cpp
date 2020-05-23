//==================================================================
/// RSLC_TokenCalc.cpp
///
/// Created by Davide Pasca - 2010/3/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <memory>
#include "RSLC_TokenCalc.h"

//==================================================================
namespace RSLC
{

//==================================================================
Token *TokenCalcBiOp(
                TokenID			operTokID,
                const Token		&val1,
                const Token		&val2 )
{
    if ( val1.id != val1.id &&
         ((val1.id != T_VL_BOOL_TRUE && val1.id != T_VL_BOOL_FALSE) ||
          (val2.id != T_VL_BOOL_TRUE && val2.id != T_VL_BOOL_FALSE)) )
        return NULL;

    std::unique_ptr<Token> pOut_Tok( DNEW Token( val1 ) );

    switch ( val1.id )
    {
    case T_VL_NUMBER:
        switch ( operTokID )
        {
        case T_OP_PLUS:		pOut_Tok->SetAsNumValue( val1.ConstNum +  val2.ConstNum ); break;
        case T_OP_MINUS:	pOut_Tok->SetAsNumValue( val1.ConstNum -  val2.ConstNum ); break;
        case T_OP_MUL:		pOut_Tok->SetAsNumValue( val1.ConstNum *  val2.ConstNum ); break;
        case T_OP_DIV:		pOut_Tok->SetAsNumValue( val1.ConstNum /  val2.ConstNum ); break;
        case T_OP_LSEQ:		pOut_Tok->SetAsBoolValue( val1.ConstNum <= val2.ConstNum ); break;
        case T_OP_GEEQ:		pOut_Tok->SetAsBoolValue( val1.ConstNum >= val2.ConstNum ); break;
        case T_OP_LSTH:		pOut_Tok->SetAsBoolValue( val1.ConstNum <  val2.ConstNum ); break;
        case T_OP_GRTH:		pOut_Tok->SetAsBoolValue( val1.ConstNum >  val2.ConstNum ); break;
        case T_OP_EQ:		pOut_Tok->SetAsBoolValue( val2.ConstNum == val2.ConstNum ); break;
        case T_OP_NEQ:		pOut_Tok->SetAsBoolValue( val1.ConstNum != val2.ConstNum ); break;

        default:
            DASSERT( 0 );
            return NULL;
        }
        break;

    case T_VL_STRING:
        switch ( operTokID )
        {
        case T_OP_PLUS:		pOut_Tok->SetAsStringValue( val1.str +  val2.str ); break;
        case T_OP_EQ:		pOut_Tok->SetAsBoolValue( val1.str == val2.str ); break;
        case T_OP_NEQ:		pOut_Tok->SetAsBoolValue( val1.str != val2.str ); break;

        default:
            DASSERT( 0 );
            return NULL;
        }
        break;

    case T_VL_BOOL_TRUE:
    case T_VL_BOOL_FALSE:
        switch ( operTokID )
        {
        case T_OP_EQ:		pOut_Tok->SetAsBoolValue( val1.id == val2.id ); break;
        case T_OP_NEQ:		pOut_Tok->SetAsBoolValue( val1.id != val2.id ); break;

        default:
            DASSERT( 0 );
            return NULL;
        }
        break;

    default:
        DASSERT( 0 );
        return NULL;
    }

    return pOut_Tok.release();
}

//==================================================================
}
