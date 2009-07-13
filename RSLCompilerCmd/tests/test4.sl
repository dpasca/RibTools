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
	Oi = bobo( 2 );
}
