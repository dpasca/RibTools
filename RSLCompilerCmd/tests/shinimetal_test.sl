/* Test file for shader parsing
   ..this comment is part of the test 8)*/

surface
shinymetal(
	float Ka = 1;
	float Ks = 1;
	float Kr = 1;
	float roughness = .1;
	string texturename = ""; )
{
	normal Nf = faceforward(N, I);
	point D;
	
	float	one, two=1;
	
	{
		float one;
	}

	D = reflect(I, normalize(Nf));
	D = transform("world", point "world" (0, 0, 0) + D);

	Oi = Os;
	Ci = Os * Cs * (Ka*ambient() + Ks*specular(Nf, -I, roughness)
		+ Kr*color environment(texturename, D));
}
