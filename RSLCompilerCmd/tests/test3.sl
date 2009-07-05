/* Test file for shader parsing
   ..this comment is part of the test 8)*/

/*==========================================*/
float dude( float a=1, float b )
{
	float c = a + b;
	return c;
}

/*==========================================*/
surface test2( float b )
{
	Ci = dude( 180 );
	Oi = Os;
}
