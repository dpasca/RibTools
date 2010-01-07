/*==================================================================
/// test9.sl
///
/// Created by Davide Pasca - 2010/01/05
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//================================================================*/

surface test9( string texturename )
{

/*
	bool a = true == false;
*/

	Ci = color( 1, 0, 0 );

	texturename != "";
	{
		Ci = color( 0, 0, 1 );
	}
	
/*
	if ( texturename != "" )
	{
		Ci = color( 0, 0, 1 );
	}
	else
	{
		Ci = color( 1, 0, 0 );
	}
*/

	Oi = 1;
}
