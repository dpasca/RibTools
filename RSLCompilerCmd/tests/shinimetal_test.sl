/* Test file for shader parsing
   ..this comment is part of the test 8)*/

float func0()
{
	return xcomp( vector( 6+1, 7, 8 ) );
}

float func1()
{
	float	a = 1;
	float	b = 2;
	float	c = 3;
	float	s = 4;
	float	t = 5;
	float	q = 6;

	return (a + b * c) + (s + t * q);
}

vector func2( vector a )
{
	float	b = 1 + 2 * 3;
	float	c = 1 + 2 + 3;
	c += 14;
	return vector( 1, 2, 3 );
}

surface
shinymetal(
	float Ka = 1;
	float Ks = 1;
	float Kr = 1;
	float roughness = .1;
	string texturename = ""; )
{
	float	one, two=1;

	normal Nf = faceforward(N, I);
	point D;
	
	{
		float one;
	}

	D = reflect(I, normalize(Nf));
	D = D + 0;
	D = transform("world", point "world" (0, 0, 0) + D);
	
	D += reflect( point "world" (0, 1, 0), point "object" (0, 0, 1) );
	
	D += func1( vector(2,0,0) );

	Oi = Os;
	Ci = Os * Cs * (Ka*ambient() + Ks*specular(Nf, -I, roughness)
		+ Kr*color environment(texturename, D));
}
