/*==================================================================
/// dbg_show_I.sl
///
/// This is a basic shader found in the RenderMan specs
//================================================================*/

surface dbg_show_negI()
{
	Oi = color(1);
	vector negI = vector(0) - I;
	Ci = color( xcomp(negI), ycomp(negI), zcomp(negI) );
}
