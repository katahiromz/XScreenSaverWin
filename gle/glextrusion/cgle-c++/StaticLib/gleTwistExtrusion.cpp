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
//												 CgleTwistExtrusion
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleTwistExtrusion::CgleTwistExtrusion(int Points, int ContourPoints, double Radius) :
	CgleBaseExtrusion(Points, ContourPoints, Radius)
{

	SetExtrusionMode(GLE_JN_CAP | GLE_CONTOUR_CLOSED | GLE_NORM_FACET |
		GLE_JN_ROUND);

	m_ptrXforms = new gleAffine[m_iPoints];
}
//----------------------------------------------------------------------------
CgleTwistExtrusion::~CgleTwistExtrusion()
{
	delete[] m_ptrXforms;
	m_ptrXforms = NULL;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleTwistExtrusion::Draw(double Point_Array[][3],// polyline
											float Color_Array[][3],// color of polyline
											double Twist_Array[], bool bTextured)//countour twists	  (in degrees)
{
	double angle;
	double si, co;

	for (int j = 0; j<m_iPoints; j++)
	{
		angle = (M_PI / 180.0) * Twist_Array[j];
		si = sin(angle);
		co = cos(angle);
		AVAL(m_ptrXforms, j, 0, 0) = co;
		AVAL(m_ptrXforms, j, 0, 1) = -si;
		AVAL(m_ptrXforms, j, 0, 2) = 0.0;
		AVAL(m_ptrXforms, j, 1, 0) = si;
		AVAL(m_ptrXforms, j, 1, 1) = co;
		AVAL(m_ptrXforms, j, 1, 2) = 0.0;
	}

	CgleBaseExtrusion::DrawExtrusion(Point_Array, Color_Array, bTextured);
}
//----------------------------------------------------------------------------
void CgleTwistExtrusion::WireDraw(double Point_Array[][3],// polyline
	float Color_Array[][3],// color of polyline
	double Twist_Array[], bool bTextured)//countour twists
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Draw(Point_Array, Color_Array, Twist_Array, bTextured);
	glPopAttrib();
}
//----------------------------------------------------------------------------
