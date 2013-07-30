/* Test file for shader parsing
   ..this comment is part of the test 8)*/

surface
test1(
	float Ka = 1; )
{
	Ci = Ka * ambient();

	ambient();

	
	Ci = ambient();

	Oi = Os;
}
