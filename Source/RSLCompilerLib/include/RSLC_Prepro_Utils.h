//==================================================================
/// RSLC_Prepro_Utils.h
///
/// Created by Davide Pasca - 2010/2/14
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_PREPRO_UTILS_H
#define RSLC_PREPRO_UTILS_H

#include "DSystem/include/DContainers.h"
#include "RSLC_FatChars.h"

//==================================================================
namespace RSLC
{
//==================================================================
namespace PREPRO
{

static const char LF = 0x0a;
static const char CR = 0x0d;

//==================================================================
bool MatchesAdvance( const DVec<Fat8> &str, size_t &io_i, size_t end, const char *pFindStr );
void SkipHWhites( const DVec<Fat8> &text, size_t &i, size_t toEnd );

//==================================================================
template <class _T>
void CutVector( DVec<_T> &vec, size_t start, size_t end )
{
	DASSERT( start <= end );

	for (size_t i=end; i < vec.size(); ++i)
		vec[start+i-end] = vec[i];

	vec.resize( vec.size() - (end - start) );
}

//==================================================================
void CutVectorInclusive( DVec<Fat8> &vec, size_t start, size_t end );

//==================================================================
template <class _T>
void ReplaceSubVector( DVec<_T> &vec, size_t start, size_t end, const DVec<_T> &newSpan )
{
	// VEC  : -----------|start------------|end-----------|vec.size()
	// EXPA :            |newSpan-------------|newSpan.size()
	// CONTR:            |newSpan--------|newSpan.size()

	DASSERT( start <= end );

	size_t		oldSpanLen = end - start;

	ptrdiff_t	oldToNewSizeDiff = (ptrdiff_t)newSpan.size() - (ptrdiff_t)oldSpanLen;

	if ( oldToNewSizeDiff > 0 )
	{
		// expand
		size_t	oldVecSize = vec.size();

		vec.resize( oldVecSize + oldToNewSizeDiff );	// enlarge the vec

		// move the elements down to make space
		for (size_t i=oldVecSize; i > end; --i)
			vec[i-1+oldToNewSizeDiff] = vec[i-1];
	}
	else
	if ( oldToNewSizeDiff < 0 )
	{
		// contract
		for (size_t i=end; i < vec.size(); ++i)
			vec[i+oldToNewSizeDiff] = vec[i];

		vec.resize( vec.size() + oldToNewSizeDiff );
	}

	// finally copy the new span
	for (size_t i=0; i < newSpan.size(); ++i)
		vec[start+i] = newSpan[i];
}

//==================================================================
size_t GetAlphaNumBetweenSpaces(
				DVec<Fat8>	&text,
				size_t		i,
				size_t		lineEnd,
				FatBase		&fatBase,
				DStr	&out_symName );

//==================================================================
}
//==================================================================
}

#endif
