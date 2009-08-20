//==================================================================
/// RI_Param.h
///
/// Created by Davide Pasca - 2008/12/18
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_PARAM_H
#define RI_PARAM_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DContainers.h"

//==================================================================
namespace RI
{

//==================================================================
typedef DVec<float>	FltVec;
	
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
		FltVec		floatArrayVal;
	}u;

	Param() : type(UNKNOWN) {}
	~Param() {}

	/*
	Param &operator=( const Param &rhs )
	{
		type = rhs.type;
		switch ( type )
		{
		case INT:		u.intVal = rhs.u.intVal;	break;
		case FLT:		u.floatVal = rhs.u.floatVal;	break;
		case INT_ARR: 	u.stringVal = rhs.u.stringVal;	break;
		case FLT_ARR: 	u.intArrayVal = rhs.u.intArrayVal;	break;
		case STR:		u.floatArrayVal = rhs.u.floatArrayVal;	break;

		case UNKNOWN:
		default:
				DASSERT( 0 );
				break;
		}
		
		return *this;
	}
	*/

	inline int			Int() const
	{
		if ( type == INT )		return u.intVal;	else
		if ( type == INT_ARR )	return u.intArrayVal[0];	else
								{ badType(); return 0; }
	}

	inline float		Flt() const
	{
		if ( type == FLT )		return u.floatVal;	else
		if ( type == INT )		return (float)u.intVal;	else
		if ( type == FLT_ARR )	return u.floatArrayVal[0];	else
		if ( type == INT_ARR )	return (float)u.intArrayVal[0];	else
							{ badType(); return 0; }
	}
	
	const FltVec	&NumVec( size_t n=DNPOS );	// may need to convert from int array

	const int		*PInt( size_t n=DNPOS ) const	{ ensIntArr( n );		return &u.intArrayVal[0];		}
	size_t			IntArrSize() const				{ ensIntArr( DNPOS );	return u.intArrayVal.size();	}
	const float		*PFlt( size_t n=DNPOS )			{ return &NumVec( n )[0];						}
	size_t			FltArrSize()					{ return NumVec().size();						}
	const char		*PChar()				const	{ ensType( STR ); return u.stringVal.c_str();	}

	bool IsString() const	{ return type == STR;	}
	bool IsIntVal() const	{ return type == INT || type == INT_ARR; }

	operator const char*	() const	{	return PChar();	}
	operator float			() const	{	return Flt();	}
	operator int			() const	{	return Int();	}

private:
	void ensType( u_int type_ ) const
	{
		if ( type != type_ )
			badType();
	}		
	void ensIntArr( size_t n ) const
	{
		if ( type != INT_ARR && (n==DNPOS || u.intArrayVal.size() == n) )
			badType();
	}		
	void ensFltArr( size_t n ) const
	{
		if ( type != FLT_ARR && (n==DNPOS || u.floatArrayVal.size() == n) )
			badType();
	}
	void badType() const
	{
		DASSTHROW( false, ("Bad Type !!") );
	}	
};

//==================================================================
class ParamList : public DVec<Param>
{

public:
	void Add( const char *pStr );
	void Add( float val );
	void Add( int val );
};

//==================================================================
	int findParam( const char *pFindName, u_int expectedType, int fromIdx, ParamList &params );

//==================================================================
}

#endif
