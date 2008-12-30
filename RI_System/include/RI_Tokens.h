/*
 *  RI_Tokens.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/22.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 *  Portions from the official RI.h
 *    See: https://renderman.pixar.com/products/rispec/rispec_3_1/appendix.C.html
 */

#ifndef RI_TOKENS_H
#define RI_TOKENS_H

#include "RI_Base.h"

//==================================================================
extern RtToken	RI_FRAMEBUFFER, RI_FILE;
extern RtToken	RI_RGB, RI_RGBA, RI_RGBZ, RI_RGBAZ, RI_A, RI_Z, RI_AZ;
extern RtToken	RI_PERSPECTIVE, RI_ORTHOGRAPHIC;
extern RtToken	RI_HIDDEN, RI_PAINT;
extern RtToken	RI_CONSTANT, RI_SMOOTH;
extern RtToken	RI_FLATNESS, RI_FOV;
extern RtToken	RI_AMBIENTLIGHT, RI_POINTLIGHT, RI_DISTANTLIGHT, RI_SPOTLIGHT; 
extern RtToken	RI_INTENSITY, RI_LIGHTCOLOR, RI_FROM, RI_TO, RI_CONEANGLE, RI_CONEDELTAANGLE, RI_BEAMDISTRIBUTION;
extern RtToken	RI_MATTE, RI_METAL, RI_SHINYMETAL, RI_PLASTIC, RI_PAINTEDPLASTIC;
extern RtToken	RI_KA, RI_KD, RI_KS, RI_ROUGHNESS, RI_KR, RI_TEXTURENAME, RI_SPECULARCOLOR;
extern RtToken	RI_DEPTHCUE, RI_FOG, RI_BUMPY;
extern RtToken	RI_MINDISTANCE, RI_MAXDISTANCE, RI_BACKGROUND, RI_DISTANCE, RI_AMPLITUDE;
extern RtToken	RI_RASTER, RI_SCREEN, RI_CAMERA, RI_WORLD, RI_OBJECT;
extern RtToken	RI_INSIDE, RI_OUTSIDE, RI_LH, RI_RH;
extern RtToken	RI_P, RI_PZ, RI_PW, RI_N, RI_NP, RI_CS, RI_OS, RI_S, RI_T, RI_ST;
extern RtToken	RI_BILINEAR, RI_BICUBIC;
extern RtToken	RI_PRIMITIVE, RI_INTERSECTION, RI_UNION, RI_DIFFERENCE;
extern RtToken	RI_PERIODIC, RI_NONPERIODIC, RI_CLAMP, RI_BLACK;
extern RtToken	RI_IGNORE, RI_PRINT, RI_ABORT, RI_HANDLER;

extern RtToken	RI_EMPTY_TOKEN;

#endif
