/* Test file for shader parsing
   ..this comment is part of the test 8)*/

/*==========================================*/
vector bobo( float d )
{
	vector tt = vector(0,1,0);

	return tt + vector(0,0,2) + d;
}

/*==========================================*/
surface test4( float b )
{
	/* the following should fail.. but it doesn't yet (^^;) */
	float	s = bobo( 2 );

	Oi = 4;
}
