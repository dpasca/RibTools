/*==================================================================
/// constant.sl
///
/// This is a basic shader found in the RenderMan specs
//================================================================*/

surface constant()
{
	Oi = Os;
	Ci = Os * Cs;
}
