/*==================================================================
/// T01_TestIfElse.sl
///
///
//================================================================*/

surface T01_TestIfElse( string texturename="" )
{
    if ( texturename != "" )
    {
        Ci = color( 0, 1, 0 );
    }
    else
    {
        Ci = color( 1, 0, 0 );
    }

    Oi = 1;
}