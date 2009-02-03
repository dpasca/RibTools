/*
 *  RI_State_Error.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/12.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "RI_State.h"

//==================================================================
namespace RI
{

//==================================================================
void State::ErrHandler( Error errCode )
{
	//printf( "Error %s\n", ErrorToString( errCode ) );
	DASSTHROW( 0, ("Error %s\n", ErrorToString( errCode )) );
}

//==================================================================
void State::ErrHandler( Error errCode, const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[1024];
	vsnprintf( buff, _countof(buff)-1, pFmt, vl );

	va_end( vl );

	//printf( "Error %s -- %s\n", ErrorToString( errCode ), buff );
	DASSTHROW( 0, ("Error %s -- %s\n", ErrorToString( errCode ), buff) );
}

//==================================================================
bool State::checkPopMode( Mode expectedMode )
{
	if ( mModeStack.top() != expectedMode )
	{
		ErrHandler( E_NESTING );
		return false;
	}
	else
	{
		mModeStack.pop();
		return true;
	}
}

//==================================================================
bool State::verifyOpType( OpType optype )
{
	Mode	curMode = mModeStack.top();

	if ( optype == OPTYPE_OPTS )
	{
		if ( curMode != MD_BASE && curMode != MD_FRAME )
		{
			ErrHandler( E_NOTOPTIONS );
			return false;
		}
	}
	else
	if ( optype == OPTYPE_ATRB )
	{
		//if ( curMode != MD_ATTRIBUTE )
		if ( curMode == MD_OBJECT )
		{
			ErrHandler( E_NOTATTRIBS );
			return false;
		}
	}
	else
	if ( optype == OPTYPE_PRIM )
	{
		switch( curMode )
		{
		case MD_WORLD:
		case MD_ATTRIBUTE:
		case MD_TRANSFORM:
		case MD_SOLID:
		case MD_OBJECT:
		case MD_MOTION:
			// set primitive
			break;

		default:			
			ErrHandler( E_NOTPRIMS );
			return false;
		}
	}
	else
	if ( optype == OPTYPE_STD_XFORM )
	{
		switch( curMode )
		{
		case MD_BASE:
		case MD_FRAME:
		case MD_WORLD:
		case MD_ATTRIBUTE:
		case MD_TRANSFORM:
		case MD_SOLID:
		//case MD_OBJECT:	// exclude object...
		case MD_MOTION:
			// set primitive
			break;

		default:			
			//not sure if it's the proper error for this
			ErrHandler( E_ILLSTATE );
			return false;
		}
	}

	return true;
}

//==================================================================
bool State::verifyBasis( RtToken basis, int steps )
{
	struct
	{
		RtToken	token;
		int		steps;
	} table[] = 
	{
		RI_BEZIERBASIS,		3,
		RI_BSPLINEBASIS,	1,
		RI_CATMULLROMBASIS,	2,
		RI_HERMITEBASIS,	1,
		RI_POWERBASIS,		4,
	};

	for (int i=0; i < (int)_countof(table); ++i)
	{
		if ( table[i].token == basis )
		{
			if ( steps == table[i].steps )
				return true;
			else
			{
				ErrHandler( E_BADARGUMENT );
				return false;
			}
		}
	}

	ErrHandler( E_BADBASIS );
	return false;
}

//==================================================================
}