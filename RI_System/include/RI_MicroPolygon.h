//==================================================================
/// RI_MicroPolygon.h
///
/// Created by Davide Pasca - 2009/4/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_MICROPOLYGON_H
#define RI_MICROPOLYGON_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DContainers.h"
#include "DMath/include/DMath.h"

//==================================================================
namespace RI
{

//==================================================================
class MicroPolygon
{
public:
    int			mXMin;
    int			mYMin;
    int			mXMax;
    int			mYMax;
    float		mZMin;
    float		mZMax;
    u_int		mVertIdx0;
    u_int		mVertIdx1;
    u_int		mVertIdx2;
    u_int		mVertIdx3;
};

//==================================================================
}

#endif
