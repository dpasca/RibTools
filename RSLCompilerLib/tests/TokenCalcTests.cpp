#include <memory>

#include "RSLCompilerLib/include/RSLC_TokenCalc.h"

namespace
{

RSLC::Token makeNumber( double value )
{
    RSLC::Token token;
    token.SetAsNumValue( value );
    return token;
}

bool isBoolean( const RSLC::Token *pToken, bool expected )
{
    if ( pToken == nullptr )
        return false;

    return pToken->id == (expected ? RSLC::T_VL_BOOL_TRUE : RSLC::T_VL_BOOL_FALSE);
}

} // namespace

int main()
{
    const auto two = makeNumber( 2.0 );
    const auto three = makeNumber( 3.0 );

    std::unique_ptr<RSLC::Token> unequalResult(
        RSLC::TokenCalcBiOp( RSLC::T_OP_EQ, two, three ) );
    if ( !isBoolean( unequalResult.get(), false ) )
        return 1;

    std::unique_ptr<RSLC::Token> equalResult(
        RSLC::TokenCalcBiOp( RSLC::T_OP_EQ, three, three ) );
    if ( !isBoolean( equalResult.get(), true ) )
        return 2;

    std::unique_ptr<RSLC::Token> sumResult(
        RSLC::TokenCalcBiOp( RSLC::T_OP_PLUS, two, three ) );
    if ( sumResult == nullptr || sumResult->id != RSLC::T_VL_NUMBER || sumResult->ConstNum != 5.0 )
        return 3;

    RSLC::Token stringValue;
    stringValue.SetAsStringValue( "2" );
    std::unique_ptr<RSLC::Token> mismatchedResult(
        RSLC::TokenCalcBiOp( RSLC::T_OP_PLUS, two, stringValue ) );
    if ( mismatchedResult != nullptr )
        return 4;

    RSLC::Token trueValue;
    trueValue.SetAsBoolValue( true );
    RSLC::Token falseValue;
    falseValue.SetAsBoolValue( false );

    std::unique_ptr<RSLC::Token> booleanResult(
        RSLC::TokenCalcBiOp( RSLC::T_OP_NEQ, trueValue, falseValue ) );
    if ( !isBoolean( booleanResult.get(), true ) )
        return 5;

    auto negativeResult =
        RSLC::TokenCalcUnaryOp( RSLC::T_OP_UNARY_MINUS, three );
    if ( negativeResult == nullptr ||
         negativeResult->id != RSLC::T_VL_NUMBER ||
         negativeResult->ConstNum != -3.0 )
        return 6;

    auto positiveResult =
        RSLC::TokenCalcUnaryOp( RSLC::T_OP_UNARY_PLUS, three );
    if ( positiveResult == nullptr ||
         positiveResult->id != RSLC::T_VL_NUMBER ||
         positiveResult->ConstNum != 3.0 )
        return 7;

    auto invalidUnaryResult =
        RSLC::TokenCalcUnaryOp( RSLC::T_OP_UNARY_MINUS, stringValue );
    if ( invalidUnaryResult != nullptr )
        return 8;

    return 0;
}
