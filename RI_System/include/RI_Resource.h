//==================================================================
/// RI_Resource.h
///
/// Created by Davide Pasca - 2009/5/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_RESOURCE_H
#define RI_RESOURCE_H

#include "RI_Base.h"

//==================================================================
namespace RI
{

//==================================================================
class ResourceBase
{
public:
	enum Type
	{
		TYPE_SHADER,
	};

private:
	std::string	mName;
	Type		mType;
	size_t		mRefCount;
	
	friend class ResourceManager;

public:
	ResourceBase( const char *pName, Type type ) :
		mName(pName),
		mType(type),
		mRefCount(0)
	{
	}
	
	virtual ~ResourceBase()
	{
		DASSERT( mRefCount == 0 );
	}
	
	void AddRef()
	{
		mRefCount += 1;
	}

	void SubRef()
	{
		DASSERT( mRefCount >= 1 );
		mRefCount -= 1;
	}

	size_t GetRef() const
	{
		return mRefCount;
	}
};

//==================================================================
template <class T>
class ResOwn
{
	T	*mPtr;

public:
	ResOwn() :
		mPtr(NULL)
	{
	}

	virtual ~ResOwn()
	{
		if ( mPtr )
			mPtr->SubRef();
	}

	void Borrow( T *ptr )
	{
		if ( mPtr )
			mPtr->SubRef();

		if ( mPtr = ptr )
			mPtr->AddRef();
	}

	void Borrow( const T *ptr )
	{
		if ( mPtr )
			mPtr->SubRef();

		if ( mPtr = (T *)ptr )
			mPtr->AddRef();
	}

	const T *Use() const
	{
	#ifdef _DEBUG
		if ( mPtr )
		{
			DASSERT( mPtr->GetRef() > 0 );
		}
	#endif
		return mPtr;
	}

	T *Use()
	{
	#ifdef _DEBUG
		if ( mPtr )
		{
			DASSERT( mPtr->GetRef() > 0 );
		}
	#endif
		return mPtr;
	}
};

//==================================================================
/// ResourceManager
//==================================================================
class ResourceManager
{
	DVec<ResourceBase *>	mpList;
public:
	ResourceManager()
	{
	}
	
	~ResourceManager()
	{
		Collect();
	}
	
	ResourceBase *AddResource( ResourceBase *pRes )
	{
		mpList.push_back( pRes );
		return pRes;
	}

	ResourceBase *FindResource( const char *pName, ResourceBase::Type type );

	void Collect();
};


//==================================================================
}

#endif