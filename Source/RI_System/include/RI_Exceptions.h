//==================================================================
/// RI_Exceptions.h
///
/// Created by Davide Pasca - 2010/2/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_EXCEPTIONS_H
#define RI_EXCEPTIONS_H

#include "DSystem/include/DStr.h"

//==================================================================
namespace RI
{

//==================================================================
class Exception
{
	DStr	mMsg;

public:
	Exception( const char *pFmt, ... );

	~Exception()
	{
	}

	const DStr &GetMessage() const
	{
		return mMsg;
	}
};


//==================================================================
}

#endif
