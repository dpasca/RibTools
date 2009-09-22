//==================================================================
/// RSLC_Defs_StdVars.h
///
/// Created by Davide Pasca - 2009/7/16
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_DEFS_STDVARS_H
#define RSLC_DEFS_STDVARS_H

//==================================================================
namespace RSLC
{

//==================================================================
const char *_gGlobalsDefs[] =
{
	"color"	,	"varying"	,	"Cs"	,	// also uniform ?
	"color"	,	"varying"	,	"Os"	,	// also uniform ?
	"point"	,	"varying"	,	"P"	,
	//"point"	,	"varying"	,	"Ps"	,
	"point"	,	"varying"	,	"dPdu"	,
	"point"	,	"varying"	,	"dPdv"	,
	"normal",	"varying"	,	"N"	,
	"normal",	"varying"	,	"Ng"	,	// also uniform ?
	"float"	,	"varying"	,	"u"	,
	"float"	,	"varying"	,	"v"	,
	"float"	,	"varying"	,	"du"	,	// also uniform ?
	"float"	,	"varying"	,	"dv"	,	// also uniform ?
	"float"	,	"varying"	,	"s"	,		// also uniform ?
	"float"	,	"varying"	,	"t"	,		// also uniform ?
	"vector",	"varying"	,	"L"	,		// also uniform ?
	"color"	,	"varying"	,	"Cl"	,	// also uniform ?
	"vector",	"varying"	,	"I"	,
	"color"	,	"varying"	,	"Ci"	,
	"color"	,	"varying"	,	"Oi"	,
	"point"	,	"uniform"	,	"E"	,
	//""	,	""	,	"A"	,
};


//==================================================================
}

#endif
