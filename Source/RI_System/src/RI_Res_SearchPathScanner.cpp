//==================================================================
/// RI_Res_SearchPathScanner.cpp
///
/// Created by Davide Pasca - 2010/1/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Res_SearchPathScanner.h"

//==================================================================
namespace RI
{

//==================================================================
SearchPathScanner::SearchPathScanner(
				const char *pBaseDir,
				const char *pDefaultDir,
				const DVec<DStr> &searchPaths ) :
	mIdx(0),
	mpBaseDir(pBaseDir),
	mpDefaultDir(pDefaultDir),
	mpSearchPaths(&searchPaths)
{
}

//==================================================================
bool SearchPathScanner::GetNext( std::string &out_path, bool &out_pathIsAbsolute )
{
	// no search paths ?
	if NOT( mpSearchPaths->size() )
	{
		return getNextNoSPathList( out_path, out_pathIsAbsolute );
	}
	else
	{
		return getNextWithSPathList( out_path, out_pathIsAbsolute );
	}
}

//==================================================================
// remove trailing slash or backslash
static void strRemoveTrailingDirDiv( DStr &io_str )
{
	while ( io_str.length() )
	{
		char ch = io_str[ io_str.length() - 1 ];

		if ( ch != '/' && ch != '\\' )
			break;

		io_str.resize( io_str.length() - 1 );
	}
}

//==================================================================
bool SearchPathScanner::getNextNoSPathList( std::string &out_path, bool &out_pathIsAbsolute )
{
	out_pathIsAbsolute = true;

	// use the base dir first and then the app res dir..

	switch ( mIdx )
	{
	case 0:
		out_path = mpBaseDir;
		strRemoveTrailingDirDiv( out_path );
		++mIdx;
		return true;

	case 1:
		if ( mpDefaultDir )
		{
			out_path = mpDefaultDir;
			strRemoveTrailingDirDiv( out_path );
			++mIdx;
			return true;
		}

		return false;

	default:
		return false;
	}
}

//==================================================================
bool SearchPathScanner::getNextWithSPathList( std::string &out_path, bool &out_pathIsAbsolute )
{
	// not given the base dir yet ?
	if ( mpBaseDir )
	{
		// ..give this first
		out_pathIsAbsolute = true;
		out_path = mpBaseDir;
		strRemoveTrailingDirDiv( out_path );

		// set it to NULL to flag that we won't give it again
		mpBaseDir = NULL;

		return true;
	}

	// have we reached the end ?
	if ( mIdx >= mpSearchPaths->size() )
		return false;

	if NOT( mpDefaultDir )
	{
		// if we have no default dir, then skip all the @ until
		// the next actual path
		for (; mIdx < mpSearchPaths->size(); ++mIdx)
		{
			if ( (*mpSearchPaths)[mIdx] != "@" )
				break;
		}

		// have we reached the end ?
		if ( mIdx >= mpSearchPaths->size() )
			return false;
	}

	// is this the default path ?
	if ( (*mpSearchPaths)[mIdx] == "@" )
	{
		out_pathIsAbsolute = true;
		out_path = mpDefaultDir;
		strRemoveTrailingDirDiv( out_path );
	}
	else
	{
		// ..or just a plain path..
		out_pathIsAbsolute = false;	// not really 100% sure..
		out_path = (*mpSearchPaths)[mIdx];
		strRemoveTrailingDirDiv( out_path );
	}

	++mIdx;

	return true;
}

//==================================================================
}
