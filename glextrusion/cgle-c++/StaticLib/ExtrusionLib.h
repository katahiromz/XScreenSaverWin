/***********************************************************
*									     GLE32 Extrusion Library							*
*							Copyright© 2000 - 2017 by Dave Richards	 			*
*										  All Rights Reserved.							*
*												Ver 5.0									*
*																				       		*
*											HISTORY:										*
*									Linas Vepstas 1990 - 1997							*
*									Dave Richards  2000 - 2017						*
*																							*
************************************************************/


#pragma once


#include <math.h>
#include <stdlib.h>

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")



void InitializeExtrusionLib();
void ReleaseExtrusionLib();



typedef double gleAffine[2][3];

typedef double gleVector[3];
typedef double gleContourVector[2];
typedef float gleColor[3];

typedef void  (*CapCallBack) (int iloop, double cap[][3],
							  float face_color[3],
							  double cut_vector[3],
							  double bisect_vector[3],
							  double norms[][3],
							  int frontwards,
							  void* Owner);   



// defines for tubing join styles
#define GLE_JN_RAW          0x1
#define GLE_JN_ANGLE        0x2
#define GLE_JN_CUT          0x3
#define GLE_JN_ROUND        0x4
#define GLE_JN_CAP          0x10
#define GLE_JN_MASK         0xf    // mask bits

// determine how normal vectors are to be handled
#define GLE_NORM_FACET      0x100
#define GLE_NORM_EDGE       0x200
#define GLE_NORM_PATH_EDGE  0x400 // for spiral, lathe, helix primitives
#define GLE_NORM_MASK       0xf00    // mask bits

// closed or open countours
#define GLE_CONTOUR_CLOSED	0x1000


#define GLE_TEXTURE_STYLE_MASK	0x0f  // texturing constants
#define GLE_TEXTURE_VERTEX_FLAT	0x01
#define GLE_TEXTURE_NORMAL_FLAT	0x02
#define GLE_TEXTURE_VERTEX_CYL	0x03
#define GLE_TEXTURE_NORMAL_CYL	0x04
#define GLE_TEXTURE_VERTEX_SPH	0x05
#define GLE_TEXTURE_NORMAL_SPH	0x06
#define GLE_TEXTURE_VERTEX_MODEL_FLAT	0x07
#define GLE_TEXTURE_NORMAL_MODEL_FLAT	0x08
#define GLE_TEXTURE_VERTEX_MODEL_CYL	0x09
#define GLE_TEXTURE_NORMAL_MODEL_CYL	0x0a
#define GLE_TEXTURE_VERTEX_MODEL_SPH	0x0b
#define GLE_TEXTURE_NORMAL_MODEL_SPH	0x0c




#include "gleBaseExtrusion.h"
#include "gleExtrusion.h"
#include "gleConeExtrusion.h"
#include "gleCylinderExtrusion.h"
#include "gleIBeam.h"
#include "gleSimpleBeam.h"
#include "gleTwistExtrusion.h"
#include "gleScrew.h"
#include "gleTwistedIBeam.h"
#include "gleSimpleTwistedIBeam.h"
#include "gleSpiralExtrusion.h"
#include "gleHelicoidExtrusion.h"
#include "gleLatheExtrusion.h"
#include "gleToroidExtrusion.h"
#include "gleTaperExtrusion.h"




