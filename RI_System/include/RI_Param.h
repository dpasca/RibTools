/*
 *  RI_Param.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/18.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_PARAM_H
#define RI_PARAM_H

#include "DTypes.h"

//==================================================================
namespace RI
{

//==================================================================
struct Param
{
	enum
	{
		UNKNOWN,
		INT,
		FLT,
		INT_ARR,
		FLT_ARR,
		STR,
	};
	
	u_int	type;
	
	struct Values {
		int			intVal;
		float		floatVal;
		DStr		stringVal;
		DVec<int>	intArrayVal;
		DVec<float>	floatArrayVal;
	}u;

	int			Int() const
	{
		ensType( INT );
		return u.intVal;
	}
	
	float		Flt() const
	{
		if ( type == FLT )	return u.floatVal;	else
		if ( type == INT )	return u.intVal;	else
							badType();
	}
	
	const int	*PInt( size_t n=(size_t)-1 ) const	{ ensIntArr( n ); return &u.intArrayVal[0];	}
	const float	*PFlt( size_t n=(size_t)-1 );	// may need to convert from int array
	const char *PChar() const						{ ensType( STR ); return u.stringVal.c_str();}

private:
	void ensType( u_int type_ ) const
	{
		if ( type != type_ )
			badType();
	}		
	void ensIntArr( size_t n ) const
	{
		if ( type != INT_ARR && (n==(size_t)-1 || u.intArrayVal.size() == n) )
			badType();
	}		
	void ensFltArr( size_t n ) const
	{
		if ( type != FLT_ARR && (n==(size_t)-1 || u.floatArrayVal.size() == n) )
			badType();
	}
	void badType() const
	{
		DASSTHROW( false, "Bad Type !!" );
	}	
};

//==================================================================
typedef DVec<Param>	ParamList;

//==================================================================
}

#endif
