
/*
   shoulderlight.sl - distant light, always over right shoulder
*/

light shoulderlight( float intensity = 1; color lightcolor = 1;)
{
	point from	= transform( "world", point "screen" ( -2, -2, 0));
	point to	= transform( "world", point "screen" ( -2, -2, 1));

	solar( to - from, 0);

	Cl = intensity * lightcolor;
}

