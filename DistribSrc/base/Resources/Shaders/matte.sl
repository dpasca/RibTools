/*==================================================================
/// matte.sl
///
/// This is a basic shader found in the RenderMan specs
//================================================================*/

surface matte( float Ka = 1; float Kd = 1;)
{
	normal Nf = faceforward(normalize(N), I);
	Oi = Os;
	Ci = Os * Cs * (Ka*ambient() + Kd*diffuse(Nf));
}
