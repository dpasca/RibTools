//==================================================================
/// RI_Resource.cpp
///
/// Created by Davide Pasca - 2009/5/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Resource.h"

//==================================================================
namespace RI
{

//==================================================================
/// ResourceManager
//==================================================================
void ResourceManager::Collect()
{
	// TODO: should not block during delete
	DUT::CriticalSection::Block	lock( mCSection );

	size_t	wi = 0;
	for (size_t ri=0; ri < mpList.size(); ++ri)
	{
		if ( mpList[ri]->GetRef() == 0 )
		{
			DDELETE( mpList[ri] );
			mpList[ri] = NULL;
		}
		else
		{
			if ( wi != ri )
				mpList[wi++] = mpList[ri];
		}
	}
	mpList.resize( wi );
}

//==================================================================
ResourceBase * ResourceManager::AddResource( ResourceBase *pRes )
{
	DUT::CriticalSection::Block	lock( mCSection );

	DASSERT( pRes != NULL );
	mpList.push_back( pRes );
	return pRes;
}

//==================================================================
ResourceBase *ResourceManager::FindResource(
								const char *pName,
								ResourceBase::Type type )
{
	DUT::CriticalSection::Block	lock( mCSection );

	for (size_t i=0; i < mpList.size(); ++i)
	{
		if ( mpList[i]->mType == type )
			if ( 0 == strcasecmp( pName, mpList[i]->mName.c_str() ) )
				return mpList[i];
	}

	return NULL;
}

//==================================================================
}
