//==================================================================
/// RI_Tokens.cpp
///
/// Created by Davide Pasca - 2008/12/22
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Tokens.h"
#include "RI_Base.h"

RtToken	RI_FRAMEBUFFER		= "framebuffer";
RtToken RI_FILE				= "file";
RtToken	RI_RGB				= "rgb";
RtToken RI_RGBA				= "rgba";
RtToken	RI_RGBZ				= "rgbz";
RtToken RI_RGBAZ			= "rgbaz";
RtToken RI_A				= "a";
RtToken RI_Z				= "z";
RtToken RI_AZ				= "az";
RtToken	RI_PERSPECTIVE		= "perspective";
RtToken RI_ORTHOGRAPHIC		= "orthographic";
RtToken	RI_HIDDEN			= "hidden";
RtToken RI_PAINT			= "paint";
RtToken	RI_CONSTANT			= "constant";
RtToken RI_SMOOTH			= "smooth";
RtToken	RI_FLATNESS			= "flatness";
RtToken RI_FOV				= "fov";
RtToken	RI_AMBIENTLIGHT		= "ambientlight";
RtToken RI_POINTLIGHT		= "pointlight";
RtToken RI_DISTANTLIGHT		= "distantlight";
RtToken RI_SPOTLIGHT 		= "spotlight";
RtToken	RI_INTENSITY		= "intensity";
RtToken RI_LIGHTCOLOR		= "lightcolor";
RtToken RI_FROM				= "from";
RtToken RI_TO				= "to";
RtToken RI_CONEANGLE		= "coneangle";
RtToken RI_CONEDELTAANGLE	= "conedeltaangle";
RtToken RI_BEAMDISTRIBUTION	= "beamdistribution";
RtToken	RI_MATTE			= "matte";
RtToken RI_METAL			= "metal";
RtToken RI_SHINYMETAL		= "shinymetal";
RtToken RI_PLASTIC			= "plastic";
RtToken RI_PAINTEDPLASTIC	= "paintedplastic";
RtToken	RI_KA				= "Ka";
RtToken RI_KD				= "Kd";
RtToken RI_KS				= "Ks";
RtToken RI_ROUGHNESS		= "roughness";
RtToken RI_KR				= "Kr";
RtToken RI_TEXTURENAME		= "texturename";
RtToken RI_SPECULARCOLOR	= "specularcolor";
RtToken	RI_DEPTHCUE			= "depthcue";
RtToken RI_FOG				= "fog";   
RtToken RI_BUMPY			= "bumpy";   
RtToken	RI_MINDISTANCE		= "mindistance";
RtToken RI_MAXDISTANCE		= "maxdistance";
RtToken RI_BACKGROUND		= "background";
RtToken RI_DISTANCE			= "distance";
RtToken RI_AMPLITUDE		= "amplitude";
RtToken	RI_RASTER			= "raster";
RtToken RI_SCREEN			= "screen";
RtToken RI_CAMERA			= "camera";
RtToken RI_WORLD			= "world";
RtToken RI_OBJECT			= "object";
RtToken	RI_INSIDE			= "inside";
RtToken RI_OUTSIDE			= "outside";
RtToken RI_LH				= "lh";
RtToken RI_RH				= "rh";
RtToken	RI_P				= "P";
RtToken RI_PZ				= "Pz";
RtToken RI_PW				= "Pw";
RtToken RI_N				= "N";
RtToken RI_NP				= "Np";
RtToken RI_CS				= "Cs";
RtToken RI_OS				= "Os";
RtToken RI_S				= "s";
RtToken RI_T				= "t";
RtToken RI_ST				= "st";
RtToken	RI_BILINEAR			= "bilinear";
RtToken RI_BICUBIC			= "bicubic";
RtToken	RI_PRIMITIVE		= "primitive";
RtToken RI_INTERSECTION		= "intersection";
RtToken RI_UNION			= "union";
RtToken RI_DIFFERENCE		= "difference";
RtToken	RI_PERIODIC			= "periodic";
RtToken RI_NONPERIODIC		= "nonperiodic";
RtToken RI_CLAMP			= "clamp";
RtToken RI_BLACK			= "black";
RtToken	RI_IGNORE			= "ignore";
RtToken RI_PRINT			= "print";
RtToken RI_ABORT			= "abort";
RtToken RI_HANDLER			= "handler";

RtToken	RI_EMPTY_TOKEN		= "";

RtToken	RI_BEZIERBASIS		= "bezier";
RtToken	RI_BSPLINEBASIS		= "b-spline";
RtToken	RI_CATMULLROMBASIS	= "catmull-rom";
RtToken	RI_HERMITEBASIS		= "hermite";
RtToken	RI_POWERBASIS		= "power";
