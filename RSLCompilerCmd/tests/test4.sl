/* Test file for shader parsing
   ..this comment is part of the test 8)*/

/*==========================================*/
vector bobo( float d )
{
	/* eventual dead code elimination
	would remove the two lines below */

	vector tt = vector(0,1,0);
	return tt + vector(0,0,2) + d;

	return vector(0,1,0);
}

/*==========================================*/
surface test4( float b )
{
	Oi = Os * xcomp( bobo( 2 ) );
}
