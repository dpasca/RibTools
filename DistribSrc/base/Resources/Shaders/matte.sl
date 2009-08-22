/*==================================================================
/// matte.sl
///
/// Created by Davide Pasca - 2009/07/23
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================*/
/* This is a basic shader found in the RenderMan specs */

/*==================================================================*/
surface matte( float Ka = 1; float Kd = 1;)
{
	normal Nf = faceforward(normalize(N), I);
	Oi = Os;
	Ci = Os * Cs * (Ka*ambient() + Kd*diffuse(Nf));
}
