/*
 *  RI_Param.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/18.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "RI_Param.h"

//==================================================================
namespace RI
{

//==================================================================
const FltVec &Param::NumVec( size_t n )
{
	if ( type == FLT_ARR )
	{
		if ( n != DNPOS )
			ensFltArr( n );
	}
	else
	if ( type == INT_ARR )
	{
		if ( n != DNPOS )
			ensIntArr( n );

		u.floatArrayVal.resize( n );
		for (size_t i; i < u.intArrayVal.size(); ++i)
			u.floatArrayVal[i] = (float)u.intArrayVal[i];
	}
	else
		badType();

	return u.floatArrayVal;
}

//==================================================================
void ParamList::Add( const char *pStr )
{
	Param	*p = this->grow();

	p->type = Param::STR;
	p->u.stringVal = pStr;
}

void ParamList::Add( float val )
{
	Param	*p = this->grow();

	p->type = Param::FLT;
	p->u.floatVal = val;
}

void ParamList::Add( int val )
{
	Param	*p = this->grow();

	p->type = Param::INT;
	p->u.intVal = val;
}

//==================================================================
}
