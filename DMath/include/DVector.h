/*
 *  DVector.h
 *  RibTools
 *
 *  Created by Davide Pasca on 08/12/31.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DVECTOR_H
#define DVECTOR_H

//==================================================================
/// Vector2
//==================================================================
class Vector2
{
public:
	
	union {
		
		struct {
			float	x, y;
		};
		
		float	v2[2];
	};

	//==================================================================
	Vector2()
	{}
	
	Vector2( float x_, float y_ ) :
		x(x_), y(y_)
	{}

	Vector2( const float *p_ ) :
		x(p_[0]), y(p_[1])
	{}

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
	
	Vector2 operator * (float rval) const { return Vector2( x * rval, y * rval ); }
	Vector2 operator + (float rval) const { return Vector2( x + rval, y + rval ); }
	Vector2 operator + (const Vector2 &rval) const { return Vector2( x + rval.x, y + rval.y ); }
	Vector2 operator - (const Vector2 &rval) const { return Vector2( x - rval.x, y - rval.y ); }
};


//==================================================================
/// Vector3
//==================================================================
class Vector3
{
public:
	
	union {
		
		struct {
			float	x, y, z;
		};
		
		float	v3[3];
	};

	//==================================================================
	Vector3()
	{}
	
	Vector3( float x_, float y_, float z_ ) :
		x(x_), y(y_), z(z_)
	{}

	Vector3( const float *p_ ) :
		x(p_[0]), y(p_[1]), z(p_[2])
	{}

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
	
	Vector3 operator * (float rval) const { return Vector3( x * rval, y * rval, z * rval ); }
	Vector3 operator + (float rval) const { return Vector3( x + rval, y + rval, z + rval ); }
	Vector3 operator + (const Vector3 &rval) const { return Vector3( x + rval.x, y + rval.y, z + rval.z ); }
	Vector3 operator - (const Vector3 &rval) const { return Vector3( x - rval.x, y - rval.y, z - rval.z ); }
};

inline Vector3 operator * (float lval, const Vector3 &rval)
{
	return rval * lval;
}

//==================================================================
/// Vector4
//==================================================================
class Vector4
{
public:
	
	union {
		
		struct {
			float	x, y, z, w;
		};
		
		float	v4[4];
	};

	//==================================================================
	Vector4()
	{}
	
	Vector4( float x_, float y_, float z_, float w_ ) :
		x(x_), y(y_), z(z_), w(w_)
	{}
};


typedef	Vector2	Point2;
typedef	Vector3	Point3;
typedef	Vector4	Point4;

#endif