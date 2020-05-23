/*==================================================================
/// dbg_show_Nf.sl
///
/// Simple shader for testing
//================================================================*/

surface dbg_show_Nf()
{
    normal Nf = faceforward(normalize(N), I);

    Oi = color(1);
    Ci = color( xcomp(Nf), ycomp(Nf), zcomp(Nf) );
}
