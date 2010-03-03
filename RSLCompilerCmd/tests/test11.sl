/*==================================================================
/// test11.sl
///
/// Created by Davide Pasca - 2010/02/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//================================================================*/

surface test11( float rgbcol[(1+1+1)] ) //{.5,.5,.5} 
{
	Ci = color( rgbcol[0]+(3), rgbcol[0+1], rgbcol[2] );
	//Ci = color( rgbcol+(3), rgbcol+1*2, rgbcol );
	Oi = 1;
}
