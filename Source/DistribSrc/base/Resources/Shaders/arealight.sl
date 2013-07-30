/*==================================================================
/// arealight.sl
///
///
///
//================================================================*/

light arealight(
			float intensity = 1;
			color lightcolor = 1;
			)
{
	illuminate (P, N, 1.5707963 /* PI/2 */)
	{
		Cl = (intensity / (L.L)) * lightcolor;
	}
}
