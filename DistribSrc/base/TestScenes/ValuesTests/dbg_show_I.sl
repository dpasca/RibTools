/*==================================================================
/// dbg_show_I.sl
///
/// Simple shader for testing
//================================================================*/

surface dbg_show_I()
{
    Oi = color(1);
    Ci = color( xcomp(I), ycomp(I), zcomp(I) );
}
