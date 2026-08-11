float negateValue( float value )
{
    return -value;
}

surface unary_operators( float scale = 2 )
{
    float negativeConstant = -2;
    float tightSubtraction = 1-scale;
    float multiplied = scale*-tightSubtraction;
    float grouped = -(scale+tightSubtraction);
    float positive = +scale;
    float chained = - -scale;
    float returned = negateValue( scale );
    float afterGroup = (scale+tightSubtraction)-scale;
    float subtractNegative = scale - -tightSubtraction;

    point constructorArguments = point( -2, +2, 0 );
    point negativePoint = -constructorArguments;
    vector negativeVector = -I;

    Ci = color(
        negativeConstant
        + tightSubtraction
        + multiplied
        + grouped
        + positive
        + chained
        + returned
        + afterGroup
        + subtractNegative );
    Oi = 1;
}
