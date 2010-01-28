//==================================================================
/// DispWindows.h
///
/// Created by Davide Pasca - 2010/1/28
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DISPWINDOWS_H
#define DISPWINDOWS_H

#include "DSystem/include/DContainers.h"
#include "DispDrivers/include/DispDriverFBuffOGL.h"
#include "RI_System/include/RI_Options.h"

//==================================================================
/// DispWindows
//==================================================================
class DispWindows
{
	struct Window
	{
		DispDriverFBuffOGL	*mpDispDriver;
		int					mWinId;
	};

	static DVec<Window>	msWindows;

public:
	DispWindows();

	~DispWindows();

	//===============================================================
	void AddWindow( const RI::Options::Display &disp );

private:
	//===============================================================
	static void sDisplayFunc();

	//===============================================================
	static void sReshapeFunc( int width, int height );

};

#endif
