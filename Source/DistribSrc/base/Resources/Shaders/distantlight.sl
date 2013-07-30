/*==================================================================
/// distantlight.sl
///
///
///
//================================================================*/

light distantlight(
			float intensity	 = 1;
			color lightcolor = 1;
			point from	= point "camera" (0, 0, 0);
			point to	= point "camera" (0, 0, 1);
			)
{
	solar ( to - from, 0 )
		Cl = intensity * lightcolor;
}
