/*
 *  RI_Param.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/18.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "RI_Param.h"

//==================================================================
namespace RI
{

//==================================================================
int findParam( const char *pFindName, u_int expectedType, int fromIdx, ParamList &params )
{
	DASSERT( ((params.size()-fromIdx) & 1) == 0 );

	if NOT( ((params.size()-fromIdx) & 1) == 0 )
		return -1;

	for (int i=fromIdx; i < (int)params.size(); i += 2)
	{
		DASSERT( params[i].type == Param::STR );

		if ( 0 == strcasecmp( pFindName, params[i] ) )
		{
			if ( params[i+1].type == expectedType )
				return i+1;
			else
				return -1;
		}
	}

	return -1;
}

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

		// first time ?
		if ( u.floatArrayVal.size() != n )
		{
			// build it
			u.floatArrayVal.resize( n );
			for (size_t i=0; i < u.intArrayVal.size(); ++i)
				u.floatArrayVal[i] = (float)u.intArrayVal[i];
		}
#ifdef _DEBUG
		else
		{
			// check it !
			for (size_t i=0; i < u.intArrayVal.size(); ++i)
				DASSERT( u.floatArrayVal[i] == (float)u.intArrayVal[i] );
		}
#endif
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
