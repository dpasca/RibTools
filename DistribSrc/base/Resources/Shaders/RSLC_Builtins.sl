/*==================================================================
/// RSLC_Builtins.sl
///
/// Created by Davide Pasca - 2009/07/23
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================*/
/* Basic declarations ad found in RenderMan specs and instrumented
for RibRender */

float	radians( float deg )
{
	return deg * 0.0174532925;
}

float	degrees( float rad )
{
	return rad * 57.29577957855;
}

float	sin( float a ){}
float	asin( float a ){}
float	cos( float a ){}
float	acos( float a ){}
float	tan( float a ){}
float	atan( float yoverx ){}
float	atan( float y, x ){}
float	pow( float x, y ){}
float	exp( float x ){}
float	sqrt( float x ){}
float	inversesqrt( float x ){}
float	log( float x ){}
float	log( float x, base ){}
float	mod( float a, b ){}
float	abs( float x )		{	_asm_abs( x ); }
float	sign( float x ){}

float	min( float	a, b )		{	float	tmp; _asm_min( tmp, a, b ); return tmp; }
point	min( point	a, b )		{	point	tmp; _asm_min( tmp, a, b ); return tmp; }
vector	min( vector	a, b )		{	vector	tmp; _asm_min( tmp, a, b ); return tmp; }
normal	min( normal	a, b )		{	normal	tmp; _asm_min( tmp, a, b ); return tmp; }
color	min( color	a, b )		{	color	tmp; _asm_min( tmp, a, b ); return tmp; }

float	max( float	a, b )		{	float	tmp; _asm_max( tmp, a, b ); return tmp; }
point	max( point	a, b )		{	point	tmp; _asm_max( tmp, a, b ); return tmp; }
vector	max( vector	a, b )		{	vector	tmp; _asm_max( tmp, a, b ); return tmp; }
normal	max( normal	a, b )		{	normal	tmp; _asm_max( tmp, a, b ); return tmp; }
color	max( color	a, b )		{	color	tmp; _asm_max( tmp, a, b ); return tmp; }

float	clamp( float	a, b )	{	float	tmp; _asm_clamp( tmp, a, b ); return tmp; }
point	clamp( point	a, b )	{	point	tmp; _asm_clamp( tmp, a, b ); return tmp; }
vector	clamp( vector	a, b )	{	vector	tmp; _asm_clamp( tmp, a, b ); return tmp; }
normal	clamp( normal	a, b )	{	normal	tmp; _asm_clamp( tmp, a, b ); return tmp; }
color	clamp( color	a, b )	{	color	tmp; _asm_clamp( tmp, a, b ); return tmp; }
                                                   
float	mix(float	x, y; float alpha )	{	float	tmp; _asm_mix( tmp, x, y, alpha ); return tmp; }
point	mix(point	x, y; float alpha )	{	point	tmp; _asm_mix( tmp, x, y, alpha ); return tmp; }
vector	mix(vector	x, y; float alpha )	{	vector	tmp; _asm_mix( tmp, x, y, alpha ); return tmp; }
normal	mix(normal	x, y; float alpha )	{	normal	tmp; _asm_mix( tmp, x, y, alpha ); return tmp; }
color	mix(color	x, y; float alpha )	{	color	tmp; _asm_mix( tmp, x, y, alpha ); return tmp; }
                                                                            
float	floor( float x ){}
float	ceil( float x ){}
float	round( float x ){}
float	step( float min, value ){}
float	smoothstep( float min, max, value ){}
/*
float	filterstep ( float edge, s1; ...parameterlist...){}
float	filterstep ( float edge, s1, s2; ...parameterlist...){}
float	spline ( [string basis;] float value; float f1, f2, ..., fn, fn1 ){}
float	spline ( [string basis;] float value; float fvals[] ){}
color	spline ( [string basis;] float value; color c1, c2, ..., cn, cn1 ){}
color	spline ( [string basis;] float value; color cvals[] ){}
point	spline ( [string basis;] float value; point p1, p2, ..., pn, pn1 ){}
point	spline ( [string basis;] float value; point pvals[] ){}
vector	spline ( [string basis;] float value; vector v1, v2, ..., vn, vn1 ){}
vector	spline ( [string basis;] float value; vector vvals[] ){}
*/
float	Du( float p ){}
color	Du( color p ){}
vector	Du( point p ){}
vector	Du( vector p ){}
float	Dv( float p ){}
color	Dv( color p ){}
vector	Dv( point p ){}
vector	Dv( vector p ){}
float	Deriv( float num; float den ){}
color	Deriv( color num; float den ){}
vector	Deriv( point num; float den ){}
vector	Deriv( vector num; float den ){}
float	random(){}
color	random(){}
point	random(){}
float	noise( float v ){}
color	noise( float v ){}
point	noise( float v ){}
vector	noise( float v ){}
float	noise( float u, v ){} 
color	noise( float u, v ){} 
point	noise( float u, v ){} 
vector	noise( float u, v ){} 
float	noise( point pt ){}
color	noise( point pt ){}
point	noise( point pt ){}
vector	noise( point pt ){}
float	noise( point pt, float t ){}
color	noise( point pt, float t ){}
point	noise( point pt, float t ){}
vector	noise( point pt, float t ){}
/*
float	pnoise( float v, uniform float period ){}
float	pnoise( float u, v, uniform float uperiod, uniform float vperiod ){}
float	pnoise( point pt, uniform point pperiod ){}
float	pnoise( point pt, float t, uniform point pperiod, uniform float tperiod ){}
color	pnoise( float v, uniform float period ){}
color	pnoise( float u, v, uniform float uperiod, uniform float vperiod ){}
color	pnoise( point pt, uniform point pperiod ){}
color	pnoise( point pt, float t, uniform point pperiod, uniform float tperiod ){}
point	pnoise( float v, uniform float period ){}
point	pnoise( float u, v, uniform float uperiod, uniform float vperiod ){}
point	pnoise( point pt, uniform point pperiod ){}
point	pnoise( point pt, float t, uniform point pperiod, uniform float tperiod ){}
vector	pnoise( float v, uniform float period ){}
vector	pnoise( float u, v, uniform float uperiod, uniform float vperiod ){}
vector	pnoise( point pt, uniform point pperiod ){}
vector	pnoise( point pt, float t, uniform point pperiod, uniform float tperiod ){}
float	cellnoise( float v ){}
float	cellnoise( float u, v ){}
float	cellnoise( point pt ){}
float	cellnoise( point pt, float t ){}
color	cellnoise( float v ){}
color	cellnoise( float u, v ){}
color	cellnoise( point pt ){}
color	cellnoise( point pt, float t ){}
point	cellnoise( float v ){}
point	cellnoise( float u, v ){}
point	cellnoise( point pt ){}
point	cellnoise( point pt, float t ){}
vector	cellnoise( float v ){}
vector	cellnoise( float u, v ){}
vector	cellnoise( point pt ){}
vector	cellnoise( point pt, float t ){}
*/

float xcomp( point	P ){ float tmp; _asm_xcomp_sv( tmp, P ); return tmp; }
float xcomp( vector	P ){ float tmp; _asm_xcomp_sv( tmp, P ); return tmp; }
float xcomp( normal	P ){ float tmp; _asm_xcomp_sv( tmp, P ); return tmp; }
	   
float ycomp( point	P ){ float tmp; _asm_ycomp_sv( tmp, P ); return tmp; }
float ycomp( vector	P ){ float tmp; _asm_ycomp_sv( tmp, P ); return tmp; }
float ycomp( normal	P ){ float tmp; _asm_ycomp_sv( tmp, P ); return tmp; }
									  
float zcomp( point	P ){ float tmp; _asm_zcomp_sv( tmp, P ); return tmp; }
float zcomp( vector	P ){ float tmp; _asm_zcomp_sv( tmp, P ); return tmp; }
float zcomp( normal	P ){ float tmp; _asm_zcomp_sv( tmp, P ); return tmp; }

void	setxcomp( output point	 P; float x ){}
void	setxcomp( output vector	 P; float x ){}
void	setxcomp( output normal	 P; float x ){}

void	setycomp( output point	 P; float y ){}
void	setycomp( output vector	 P; float y ){}
void	setycomp( output normal	 P; float y ){}
										  
void	setzcomp( output point	 P; float z ){}
void	setzcomp( output vector	 P; float z ){}
void	setzcomp( output normal	 P; float z ){}

float	length( vector V )		{	float tmp; _asm_length( tmp, V ); return tmp;	}
vector	normalize( vector V )	{	vector tmp; _asm_normalize( tmp, V ); return tmp;	}
float	distance( point P1, P2 ){	float tmp; _asm_distance( tmp, P1, P2 ); return tmp;	}

/*
float	ptlined ( point Q, P1, P2 ){}
float	rotate ( point Q; float angle; point P1, P2 ){}
float	area( point P ){}
*/
/* vector	faceforward( vector N, I [, Nref]){} */

vector	faceforward( vector N, I )	{	vector tmp; _asm_faceforward( tmp, N, I ); return tmp;	}
vector	reflect( vector I, N )		{	vector tmp; _asm_reflect( tmp, I, N ); return tmp;	}

/*
vector	refract( vector I, N; float eta ){}
color	fresnel( vector I, N; float eta; output float Kr, Kt; [output vector R, T] ){}	// color ?
point	transform( string tospace; point p ){}
point	transform( string fromspace, tospace; point p ){}
point	transform( matrix m; point p ){}
point	transform( string fromspace; matrix m; point p ){}
vector	vtransform( string tospace; vector v ){}
vector	vtransform( string fromspace, tospace; vector v ){}
vector	vtransform( matrix m; vector v ){}
vector	vtransform( string fromspace; matrix m; vector v ){}
normal	ntransform( string tospace; normal n ){}
normal	ntransform( string fromspace, tospace; normal n ){}
normal	ntransform( matrix m; normal n ){}
normal	ntransform( string fromspace; matrix m; normal n ){}
*/
point	calculatenormal( point P ){}
float	comp( color c; float index ){}
void	setcomp( output color c; float index, value ){}
/*
color	ctransform( string tospace; color C ){}
color	ctransform( string fromspace, tospace; color C ){}
float	comp( matrix m; float row, column ){}
void	setcomp( output matrix m; float row, column, value ){}
float	determinant( matrix m ){}
matrix	translate( matrix m; vector t ){}
matrix	rotate( matrix m; float angle; vector axis ){}
matrix	scale( matrix m; point s ){}
string	concat( string a, b, ... ){}
void	printf( string pattern, val1, val2,..., valn ){}
string	format( string pattern, val1, val2,..., valn ){}
float	match( string pattern, subject ){}
*/

color	ambient()			{	color tmp; _asm_ambient( tmp );		return tmp;	}
color	diffuse( normal N )	{	color tmp; _asm_diffuse( tmp, N );	return tmp;	}

color	specular( normal N; vector V; float roughness ){}
color	specularbrdf( vector L; normal N; vector V; float roughness ){}
color	phong( normal N; vector V; float size ){}
/*
color	trace( point P, point R ){}
float	texture( string name[channel]; [texture coordinates,] [parameterlist] ){}
color	texture( string name[channel]; [texture coordinates,] [parameterlist] ){}
float	environment( string name[channel]; texture coordinates, [parameterlist] ){}
color	environment( string name[channel]; texture coordinates, [parameterlist] ){}
float	shadow( string name[channel]; texture coordinates[, parameterlist] ){}
float	textureinfo( string texturename, dataname; output type variable ){}
*/

vector vector( float x, float y, float z )	{ vector tmp; _asm_mov_vs3( tmp, x, y, z ); return tmp; }
color color( float r, float g, float b )	{ color tmp; _asm_mov_vs3( tmp, r, g, b ); return tmp; }

