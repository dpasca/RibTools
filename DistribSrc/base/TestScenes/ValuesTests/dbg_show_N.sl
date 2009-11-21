/*==================================================================
/// dbg_show_N.sl
///
/// This is a basic shader found in the RenderMan specs
//================================================================*/

surface dbg_show_N()
{
	Oi = color(1);
	Ci = color( xcomp(N), ycomp(N), zcomp(N) );
}
