//==================================================================
/// RI_Net_Protocol.h
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_NET_PROTOCOL_H
#define RI_NET_PROTOCOL_H

//==================================================================
namespace RI
{

//==================================================================
struct NetRendJob
{
	char	FileName[512];
	char	BaseDir[512];
	char	DefaultResourcesDir[512];
	I32		ForcedWd;
	I32		ForcedHe;
};

//==================================================================
}

#endif
