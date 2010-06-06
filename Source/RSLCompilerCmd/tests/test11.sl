/*==================================================================
/// test11.sl
///
/// Created by Davide Pasca - 2010/02/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//================================================================*/

//==================================================================
void arrChange( float nonArr, output float rgb[3] )
{
	rgb[0] = rgb[1] = nonArr;
}

//==================================================================
void arrUse( float rgb[3] )
{
	float a = rgb[0];
	Oi = a;
}

//==================================================================
surface test11( float rgbcol[(1+1+1)] ) //{.5,.5,.5} 
{
	float	a, b, c;

	a = b = c = 1;

	arrChange( 2, rgbcol );

	Ci = color( rgbcol[0]+(3), rgbcol[0+1], rgbcol[2] );
	//Ci = color( rgbcol+(3), rgbcol+1*2, rgbcol );

	arrUse( rgbcol );
}
