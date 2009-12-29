
displacement disp_textured(
	float   Km  	= 1.0,
			min_u   = 0.0,
			max_u   = 1.0,
			min_v   = 0.0,
			max_v   = 1.0;
	string  mapname = "")
{
	float   magnitude,
			level;

	if( mapname != "")
	{
		level = float 
			texture(
				mapname,
				(u - min_u) / (max_u - min_u),
				(v - min_v) / (max_v - min_v),
				"swidth", 0.0001,
				"twidth", 0.0001,
				"samples", 1);

		magnitude   = ((level * 2) - 1) * Km;
	}
	else
	{
		magnitude   = 0.0;
	}

	P   += normalize( N) * magnitude;
	N   = calculatenormal( P);
}

