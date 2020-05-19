//==================================================================
/// DStr.h
///
/// Created by Davide Pasca - 2010/9/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DSTR_H
#define DSTR_H

#include "DUtils_Base.h"
#include "DContainers.h"

#include <string>

#if 1

typedef std::string DStr;

#else
//===============================================================
// Horrid and probably leaks, but (hopefully) only used for setup
class DStr
{
	DVec<char>	mData;

public:
    DStr(const char *str)
    {
		if NOT( str )
			mData.push_back( 0 );
		else
	        mData.append_array( str, strlen(str)+1 );
    }

    DStr(const char c)
    {
		mData.push_back( c );
		mData.push_back( 0 );
    }
    
    DStr()
    {
		mData.push_back( 0 );
    }
/*
    DStr(const DStr &other)
    {
        mData = other.mData;
    }

    DStr &operator = (const DStr &other)
    {
        mData = other.mData;
        return *this;
    }
*/
    ~DStr()
    {
    }

    bool empty() const
    {
        return mData[0] == 0;
    }

    void clear()
    {
        mData.resize( 1 );
		mData[0] = 0;
    }

    const char *c_str() const { return &mData[0]; }

    bool operator ==(const DStr &other) const { return 0 == strcmp( c_str(), other.c_str() ); }
    bool operator ==(const char *other) const { return 0 == strcmp( c_str(), other ); }
    bool operator !=(const DStr &other) const { return 0 != strcmp( c_str(), other.c_str() ); }
    bool operator !=(const char *other) const { return 0 != strcmp( c_str(), other ); }

    size_t size() const { return mData.size() - 1; }
    size_t length() const { return mData.size() - 1; }

    void operator +=(const DStr &other)
    {
		mData.resize( mData.size() - 1 );
		mData.append_array( &other.mData[0], other.mData.size() );
    }

    DStr operator + (const DStr &other) const
    {
        DStr newStr( *this );
        newStr += other;
        return newStr;
    }

    void operator +=( char ch )
    {
		mData[ mData.size() - 1 ] = ch;
		mData.push_back( 0 );
    }

    DStr operator + ( char ch ) const
    {
        DStr newStr( *this );
        newStr += ch;
        return newStr;
    }

    void resize(size_t newsize)
    {
        if (newsize < size())
        {
			mData.resize( newsize + 1 );
			mData[newsize] = 0;
            return;
        }

		// fill with null character.. but we want to keep it as a c-string..
		// so, nothing is done
		DASSERT( newsize == size() );
    }

	const char &operator[]( size_t idx ) const	{ return mData[ idx ]; }
		  char &operator[]( size_t idx )		{ return mData[ idx ]; }
};
#endif

#endif