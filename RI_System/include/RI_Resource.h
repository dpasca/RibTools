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
class ResourceBase : public RCBaseNoDel
{
public:
	enum Type
	{
		TYPE_SHADER,
		TYPE_TEXTURE,
	};

private:
	std::string	mName;
	Type		mType;
	
	friend class ResourceManager;

public:
	ResourceBase( const char *pName, Type type ) :
		mName(pName),
		mType(type)
	{
	}
	
	virtual ~ResourceBase()
	{
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
	
	ResourceBase *AddResource( ResourceBase *pRes );

	ResourceBase *FindResource( const char *pName, ResourceBase::Type type );

	void Collect();
};


//==================================================================
}

#endif