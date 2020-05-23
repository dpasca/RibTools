/*==================================================================
/// SpacesTestScreenSha.sl
///
/// Created by Davide Pasca - 2009/9/24
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================*/

/*==================================================================*/
surface SpacesTestScreenSha()
{
    Oi = color( 1, 1, 1 );

    normal nn = normal "screen" ( 0, 0, 1 );

    Ci = color(
            xcomp( nn ),
            ycomp( nn ),
            zcomp( nn ) );
}
