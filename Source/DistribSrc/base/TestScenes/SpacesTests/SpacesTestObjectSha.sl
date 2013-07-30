/*==================================================================
/// SpacesTestObjectSha.sl
///
/// Created by Davide Pasca - 2009/9/25
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================*/

/*==================================================================*/
surface SpacesTestObjectSha()
{
	Oi = color( 1, 1, 1 );

	normal nn = normal "object" ( 0, 0, 1 );

	Ci = color(
			xcomp( nn ),
			ycomp( nn ),
			zcomp( nn ) );
}
