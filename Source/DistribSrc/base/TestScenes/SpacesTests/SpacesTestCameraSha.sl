/*==================================================================
/// SpacesTestCameraSha.sl
///
/// Created by Davide Pasca - 2009/9/25
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================*/

/*==================================================================*/
surface SpacesTestCameraSha()
{
	Oi = color( 1, 1, 1 );

	normal nn = normal "camera" ( 0, 0, 1 );

	Ci = color(
			xcomp( nn ),
			ycomp( nn ),
			zcomp( nn ) );
}
