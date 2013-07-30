/*==================================================================
/// dbg_show_I.sl
///
/// Simple shader for testing
//================================================================*/

surface dbg_show_negI()
{
	Oi = color(1);
	vector negI = vector(0) - I;
	Ci = color( xcomp(negI), ycomp(negI), zcomp(negI) );
}
