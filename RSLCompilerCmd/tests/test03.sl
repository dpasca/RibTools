/* Test file for shader parsing
   ..this comment is part of the test 8)*/

/*==========================================*/
float dude( float a=1, bb )
{
	float c = a + bb;
	return c + 1;
}

/*==========================================*/
surface test3( float b )
{
	Ci = dude( 180, P );
	Oi = Os;
}
