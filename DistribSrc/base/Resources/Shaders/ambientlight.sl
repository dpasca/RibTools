/*==================================================================
/// ambientlight.sl
///
///
///
//================================================================*/

light ambientlight(
			float intensity = 1;
			color lightcolor = 1;
		)
{
	Cl = intensity * lightcolor;
	L = 0;
}
