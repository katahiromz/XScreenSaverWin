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

#include "stdafx.h"
#include "extrusioninternals.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
//										 CgleTwistedIBeam
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleTwistedIBeam::CgleTwistedIBeam(int Points, double Radius) :
	CgleTwistExtrusion(Points, 12, Radius)
{

	SetExtrusionMode(GLE_JN_CAP | GLE_CONTOUR_CLOSED | GLE_NORM_FACET |
		GLE_JN_ANGLE);


	LoadContourPoint(0, 1.0, 1.0);
	LoadContourPoint(1, -1.0, 1.0);
	LoadContourPoint(2, -1.0, 0.9);
	LoadContourPoint(3, -0.15, 0.9);
	LoadContourPoint(4, -0.15, -0.9);
	LoadContourPoint(5, -1.0, -0.9);
	LoadContourPoint(6, -1.0, -1.0);
	LoadContourPoint(7, 1.0, -1.0);
	LoadContourPoint(8, 1.0, -0.9);
	LoadContourPoint(9, 0.15, -0.9);
	LoadContourPoint(10, 0.15, 0.9);
	LoadContourPoint(11, 1.0, 0.9);
	LoadContourPoint(12, 1.0, 1.0);
}
//----------------------------------------------------------------------------
CgleTwistedIBeam::~CgleTwistedIBeam()
{
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleTwistedIBeam::Draw(double Point_Array[][3], float Color_Array[][3],
	double Twist_Array[], bool bTextured)
{
	CgleTwistExtrusion::Draw(Point_Array, Color_Array, Twist_Array, bTextured);
}
//----------------------------------------------------------------------------
void CgleTwistedIBeam::WireDraw(double Point_Array[][3], float Color_Array[][3],
	double Twist_Array[], bool bTextured)
{
	CgleTwistExtrusion::WireDraw(Point_Array, Color_Array, Twist_Array, bTextured);
}
//----------------------------------------------------------------------------
