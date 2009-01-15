/*
 *  RI_Tokens.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/22.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

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
RtToken	RI_KA				= "ka";
RtToken RI_KD				= "kd";
RtToken RI_KS				= "ks";
RtToken RI_ROUGHNESS		= "roughness";
RtToken RI_KR				= "kr";
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
RtToken	RI_P				= "p";
RtToken RI_PZ				= "pz";
RtToken RI_PW				= "pw";
RtToken RI_N				= "n";
RtToken RI_NP				= "np";
RtToken RI_CS				= "cs";
RtToken RI_OS				= "os";
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
