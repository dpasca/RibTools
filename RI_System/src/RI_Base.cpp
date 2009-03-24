/*
 *  RI_Base.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "RI_Base.h"

//==================================================================
namespace RI
{
	
//==================================================================
const char *ErrorToString( Error errCode )
{
	switch ( errCode )
	{
	case E_OK				:	return "E_OK";
	case E_NESTING			:	return "E_NESTING";
	case E_NOTOPTIONS		:	return "E_NOTOPTION";
	case E_NOTATTRIBS		:	return "E_NOTATTRIB";
	case E_NOTPRIMS			:	return "E_NOTPRIMS";
	case E_NOTSTARTED		:	return "E_NOTSTARTE";
	case E_ILLSTATE			:	return "E_ILLSTATE";
	case E_ARRAYTOOBIG		:	return "insufficient memory to construct array";
	case E_BADARGUMENT		:	return "incorrect parameter value";
	case E_BADARRAY			:	return "invalid array specification";
	case E_BADBASIS			:	return "undefined basis matrix name";
	case E_BADCOLOR			:	return "invalid color specification";
	case E_BADHANDLE		:	return "invalid light or object handle";
	case E_BADPARAMLIST		:	return "parameter list type mismatch";
	case E_BADRIPCODE		:	return "invalid encoded RIB request code";
	case E_BADSTRINGTOKEN	:	return "undefined encoded string token";
	case E_BADTOKEN			:	return "invalid binary token";
	case E_BADVERSION		:	return "protocol version number mismatch";
	case E_LIMITCHECK		:	return "overflowing an internal limit";
	case E_OUTOFMEMORY		:	return "generic instance of insufficient memory";
	case E_PROTOCOLBOTCH	:	return "malformed binary encoding";
	case E_STRINGTOOBIG		:	return "insufficient memory to read string";
	case E_SYNTAXERROR		:	return "general syntactic error";
	case E_UNREGISTERED		:	return "undefined RIB request";
	default					:	return "Unknown error !!!";
	}
}

//==================================================================
RtBasis	BezierBasis
(
	-1,	 3,	-3,	1,
	 3,	-6,	 3,	0,
	-3,	 3,	 0,	0,
	 1,	 0,	 0,	0
);

#define F6	6.0f

RtBasis	BSplineBasis
(
	-1/F6,	 3/F6,	-3/F6,	1/F6,
	 3/F6,	-6/F6,	 3/F6,	0/F6,
	-3/F6,	 0/F6,	 3/F6,	0/F6,
	 1/F6,	 4/F6,	 1/F6,	0/F6
);

RtBasis	CatmullRomBasis
(
	-0.5f,	 1.5f,	-1.5f,	 0.5f,
	 1,		-2.5f,	 2,		-0.5f,
	-0.5f,	 0,		 0.5f,	 0,
	 0,		 1,		 0,		 0
);

RtBasis	HermiteBasis
(
	 2,	-3,	 0,	1,
	-2,	 3,	 0,	0,
	 1,	-2,	 1,	0,
	 1,	-1,	 0,	0
);

RtBasis	PowerBasis
(
	1,	0,	0,	0,
	0,	1,	0,	0,
	0,	0,	1,	0,
	0,	0,	0,	1
);

//==================================================================
/// ResourceManager
//==================================================================
void ResourceManager::Collect()
{
	size_t	wi = 0;
	for (size_t ri=0; ri < mpList.size(); ++ri)
	{
		if ( mpList[ri]->mRefCount == 0 )
		{
			delete mpList[ri];
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
}