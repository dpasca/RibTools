//==================================================================
/// DVector.h
///
/// Created by Davide Pasca - 2009/5/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DVECTOR_H
#define DVECTOR_H

//==================================================================
class SVec2
{
public:
	union {
		struct	{ float	x, y; };
		float	v2[2];
	};
};
//==================================================================
class SVec3
{
public:
	union {
		struct	{ float	x, y, z; };
		float	v3[3];
	};
};
//==================================================================
class SVec4
{
public:
	union {
		struct	{ float	x, y, z, w; };
		float	v4[4];
	};
};

//==================================================================
/// Vec2
//==================================================================
class Vec2
{
public:
	union {	
		struct {
			float	x, y;
		};
		float	v2[2];
	};

	//==================================================================
	Vec2()												{}
	Vec2( const Vec2 &v )		: x(v.x), y(v.y)		{}
	Vec2( float a_ )			: x(a_), y(a_)			{}
	Vec2( float x_, float y_ )	: x(x_), y(y_)			{}
	Vec2( const float *p_ )		: x(p_[0]), y(p_[1])	{}
	Vec2( const SVec2 &vs )		: x(vs.x), y(vs.y)		{}

	void Set( float x_, float y_ )
	{
		x = x_;
		y = y_;
	}
	
	void Set( const float *p_ )
	{
		x = p_[0];
		y = p_[1];
	}

	Vec2 operator + (float rval) const { return Vec2( x + rval, y + rval ); }
	Vec2 operator - (float rval) const { return Vec2( x - rval, y - rval ); }
	Vec2 operator * (float rval) const { return Vec2( x * rval, y * rval ); }
	Vec2 operator / (float rval) const { return Vec2( x / rval, y / rval ); }
	Vec2 operator + (const Vec2 &rval) const { return Vec2( x + rval.x, y + rval.y ); }
	Vec2 operator - (const Vec2 &rval) const { return Vec2( x - rval.x, y - rval.y ); }
	Vec2 operator * (const Vec2 &rval) const { return Vec2( x * rval.x, y * rval.y ); }
	Vec2 operator / (const Vec2 &rval) const { return Vec2( x / rval.x, y / rval.y ); }

	Vec2 operator -() const	{ return Vec2( -x, -y ); }

	Vec2 operator +=(const Vec2 &rval)	{ *this = *this + rval; return *this; }
};

//==================================================================
/// Vec3
//==================================================================
//__declspec(align(16))
class Vec3
{
public:
	union {
		struct {
			float	x, y, z;
		};
		float	v3[3];
	};

	//==================================================================
	Vec3()														{}
	Vec3( const Vec3 &v )		: x(v.x), y(v.y), z(v.z)		{}
	Vec3( float a_ )			: x(a_), y(a_), z(a_)			{}
	Vec3( float x_, float y_ , float z_ ) : x(x_), y(y_), z(z_)	{}
	Vec3( const float *p_ )		: x(p_[0]), y(p_[1]), z(p_[2])	{}
	Vec3( const SVec3 &vs )		: x(vs.x), y(vs.y), z(vs.z)		{}

	void Set( float x_, float y_, float z_ )
	{
		x = x_;
		y = y_;
		z = z_;
	}
	
	void Set( const float *p_ )
	{
		x = p_[0];
		y = p_[1];
		z = p_[2];
	}

	void SetZero()
	{
		x = y = z = 0;
	}
	
	Vec3 operator + (float rval) const { return Vec3( x + rval, y + rval, z + rval ); }
	Vec3 operator - (float rval) const { return Vec3( x - rval, y - rval, z - rval ); }
	Vec3 operator * (float rval) const { return Vec3( x * rval, y * rval, z * rval ); }
	Vec3 operator / (float rval) const { return Vec3( x / rval, y / rval, z / rval ); }
	Vec3 operator + (const Vec3 &rval) const { return Vec3( x + rval.x, y + rval.y, z + rval.z ); }
	Vec3 operator - (const Vec3 &rval) const { return Vec3( x - rval.x, y - rval.y, z - rval.z ); }
	Vec3 operator * (const Vec3 &rval) const { return Vec3( x * rval.x, y * rval.y, z * rval.z ); }
	Vec3 operator / (const Vec3 &rval) const { return Vec3( x / rval.x, y / rval.y, z / rval.z ); }

	Vec3 operator -() const	{ return Vec3( -x, -y, -z ); }

	Vec3 operator +=(const Vec3 &rval)	{ *this = *this + rval; return *this; }

	Vec3 GetNormalized() const
	{
		float	oom = 1.0f / sqrtf( x * x + y * y + z * z );
		
		return *this * oom;
	}

	Vec3 GetCross( const Vec3 &r_ ) const
	{
		return Vec3(
			y * r_.z - z * r_.y,
			z * r_.x - x * r_.z,
			x * r_.y - y * r_.x
		);
	}

	float GetDot( const Vec3 &r_ ) const
	{
		return  x * r_.x + y * r_.y + z * r_.z;
	}
};

//==================================================================
/// Vec4
//==================================================================
//__declspec(align(16))
class Vec4
{
public:
	union {
		struct {
			float	x, y, z, w;
		};
		float	v4[4];
	};

	//==================================================================
	Vec4()																		{}
	Vec4( const Vec4 &v )		: x(v.x), y(v.y), z(v.z), w(v.w)				{}
	Vec4( float a_ )			: x(a_), y(a_), z(a_), w(a_)					{}
	Vec4( float x_, float y_, float z_, float w_ ) : x(x_), y(y_), z(z_), w(w_)	{}
	Vec4( const float *p_ )		: x(p_[0]), y(p_[1]), z(p_[2]), w(p_[3])		{}
	Vec4( const SVec4 &vs )		: x(vs.x), y(vs.y), z(vs.z), w(vs.w)			{}

	Vec3 GetAsV3() const
	{
		return Vec3( v4 );
	}

	Vec4 operator + (float rval) const { return Vec4( x + rval, y + rval, z + rval, w + rval ); }
	Vec4 operator - (float rval) const { return Vec4( x - rval, y - rval, z - rval, w - rval ); }
	Vec4 operator * (float rval) const { return Vec4( x * rval, y * rval, z * rval, w * rval ); }
	Vec4 operator / (float rval) const { return Vec4( x / rval, y / rval, z / rval, w / rval ); }
	Vec4 operator + (const Vec4 &rval) const { return Vec4( x + rval.x, y + rval.y, z + rval.z, w + rval.w ); }
	Vec4 operator - (const Vec4 &rval) const { return Vec4( x - rval.x, y - rval.y, z - rval.z, w - rval.w ); }
	Vec4 operator * (const Vec4 &rval) const { return Vec4( x * rval.x, y * rval.y, z * rval.z, w * rval.w ); }
	Vec4 operator / (const Vec4 &rval) const { return Vec4( x / rval.x, y / rval.y, z / rval.z, w / rval.w ); }

	Vec4 operator -() const	{ return Vec4( -x, -y, -z, -w ); }

	Vec4 operator +=(const Vec4 &rval)	{ *this = *this + rval; return *this; }
};

//==================================================================
inline Vec2 operator * (float lval, const Vec2 &rval) { return rval * lval; }
inline Vec3 operator * (float lval, const Vec3 &rval) { return rval * lval; }
inline Vec4 operator * (float lval, const Vec4 &rval) { return rval * lval; }

//==================================================================
typedef	Vec2	Point2;
typedef	Vec3	Point3;
typedef	Vec4	Point4;
typedef	Vec3	Color;

typedef	SVec2	SPoint2;
typedef	SVec3	SPoint3;
typedef	SVec4	SPoint4;
typedef	SVec3	SColor;

//==================================================================
/// 
//==================================================================
template <class T>
inline T DMix( const T &a, const T &b, float t )
{
	return a + (b - a) * t;
}

#endif