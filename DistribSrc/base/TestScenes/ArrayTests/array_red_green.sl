//==================================================================
/// array_red_green.sl
/// Visual regression shader for fixed-size scalar arrays.
//==================================================================

surface array_red_green( float selectedIndex = 0 )
{
    float red[2];
    float green[2];
    float blue[2];

    red[0] = 1;
    red[1] = 0;
    green[0] = 0;
    green[1] = 1;
    blue[0] = blue[1] = 0;

    float arrayIndex;
    if ( selectedIndex == 0 )
    {
        arrayIndex = 0;
    }
    else
    {
        arrayIndex = 1;
    }

    Ci = color(
        red[arrayIndex],
        green[arrayIndex],
        blue[arrayIndex] );
    Oi = 1;
}
