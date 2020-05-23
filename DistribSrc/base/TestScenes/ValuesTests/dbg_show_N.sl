/*==================================================================
/// dbg_show_N.sl
///
/// Simple shader for testing
//================================================================*/

surface dbg_show_N()
{
    Oi = color(1);
    Ci = color( xcomp(N), ycomp(N), zcomp(N) );
}
