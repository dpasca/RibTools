//==================================================================
/// RI_Param.cpp
///
/// Created by Davide Pasca - 2008/12/18
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Param.h"

//==================================================================
namespace RI
{

//==================================================================
int FindParam( const char *pFindName,
			   u_int expectedType,
			   u_int altExpectedType,
			   int fromIdx,
			   ParamList &params )
{
	int n = (int)params.size() - 1;

	for (int i=fromIdx; i < n; i += 2)
	{
		DASSERT( params[i].type == Param::STR );

		if ( 0 == strcasecmp( pFindName, params[i] ) )
		{
			if ( params[i+1].type == expectedType ||
				 params[i+1].type == altExpectedType )
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
