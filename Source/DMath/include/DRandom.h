//==================================================================
/// DRandom.h
///
/// Created by Davide Pasca - 2009/12/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DRANDOM_H
#define DRANDOM_H

#include "DMathBase.h"

//==================================================================
// very simple random generator as seen on wikipedia (^^;)
class DRandom
{
	DU32	m_w;
	DU32	m_z;
	
public:
	DRandom( DU64 seed )
	{
		m_w = (DU32)(seed & 0xffffffff);
		m_z = (DU32)((seed >> 32) & 0xffffffff);
		m_w += 36969;
		m_z += 18000;
	}

	DU32 NextU32()
	{
		m_z = 36969 * (m_z & 65535) + (m_z >> 16);
		m_w = 18000 * (m_w & 65535) + (m_w >> 16);
		return (m_z << 16) + m_w;
	}

	int NextI32()
	{
		return (int)NextU32();
	}

	float NextF0_1()
	{
		return NextU32() / (float)0xffffffff;
	}

	int Next32Range( int rmin, int rmax )
	{
		int delta = rmax - rmin + 1;

		int val = (int)(NextU32() & 0x7fffffff);

		return val % delta + rmin;
	}
};

#endif