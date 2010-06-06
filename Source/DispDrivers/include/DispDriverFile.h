//==================================================================
/// DispDriverFile.h
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DISPDRIVERFILE_H
#define DISPDRIVERFILE_H

#include "DImage/include/DImage.h"

//==================================================================
// DispDriverFile
//==================================================================
class DispDriverFile
{
public:
	DispDriverFile( const char *pFileName, const DIMG::Image &srcImg );	
	~DispDriverFile();
};

#endif
