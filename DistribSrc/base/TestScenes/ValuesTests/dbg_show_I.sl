/*==================================================================
/// dbg_show_I.sl
///
/// This is a basic shader found in the RenderMan specs
//================================================================*/

surface dbg_show_I()
{
	Oi = color(1);
	Ci = color( xcomp(I), ycomp(I), zcomp(I) );
}
