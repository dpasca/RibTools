/*==================================================================
/// plastic.sl
///
/// This is a basic shader found in the RenderMan specs
//================================================================*/

surface plastic(
            float Ka = 1;
            float Kd =.5;
            float Ks =.5;
            float roughness =.1;
            color specularcolor = 1;)
{

    normal Nf = faceforward(normalize(N), I);
    vector V = vector(0) - normalize(I);
    Oi = Os;
    Ci = Os * (Cs * (Ka*ambient() + Kd*diffuse(Nf)) +
            specularcolor * Ks * specular(Nf, V, roughness) );
            
}
