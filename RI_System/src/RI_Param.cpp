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
const float	*Param::PFlt( size_t n )
{
	if ( type == FLT_ARR )
	{
		ensFltArr( n );
		return &u.floatArrayVal[0];
	}
	else
	if ( type == INT_ARR )
	{
		ensIntArr( n );
		u.floatArrayVal.resize( n );
		for (size_t i; i < u.intArrayVal.size(); ++i)
			u.floatArrayVal[i] = (float)u.intArrayVal[i];

		return &u.floatArrayVal[0];
	}
	else
		badType();


	return &u.floatArrayVal[0];
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
