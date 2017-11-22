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
//                            CgleConeExtrusion
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

CgleConeExtrusion::CgleConeExtrusion(int Points, int Facets) :
												CgleBaseExtrusion(Points, Facets + 1, 1.0)
{
	FFacets = Facets;

	SetExtrusionMode(GLE_NORM_EDGE | GLE_JN_ROUND | GLE_JN_CAP |
		GLE_CONTOUR_CLOSED);

	m_ptrXforms = new gleAffine[m_iPoints];
	m_ptrUp = new double[3];
	SetTextureMode(GLE_TEXTURE_VERTEX_CYL);
}
//----------------------------------------------------------------------------
CgleConeExtrusion::~CgleConeExtrusion()
{

	delete[] m_ptrXforms;
	m_ptrXforms = NULL;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleConeExtrusion::Draw(double Point_Array[][3], float Color_Array[][3],
																double Radius_Array[], bool bTextured)
{
	// build 2D affine matrices from radius array
	for (int j = 0; j<m_iPoints; j++)
	{
		AVAL(m_ptrXforms, j, 0, 0) = Radius_Array[j];
		AVAL(m_ptrXforms, j, 0, 1) = 0.0;
		AVAL(m_ptrXforms, j, 0, 2) = 0.0;
		AVAL(m_ptrXforms, j, 1, 0) = 0.0;
		AVAL(m_ptrXforms, j, 1, 1) = Radius_Array[j];
		AVAL(m_ptrXforms, j, 1, 2) = 0.0;
	}

	Gen_PolyCone(Point_Array, Color_Array, 1.0, bTextured);
}
//----------------------------------------------------------------------------
void  CgleConeExtrusion::WireDraw(double Point_Array[][3], float Color_Array[][3],
																double Radius_Array[], bool bTextured)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	Draw(Point_Array, Color_Array, Radius_Array, bTextured);

	glPopAttrib();
}
//----------------------------------------------------------------------------
void  CgleConeExtrusion::Gen_PolyCone(double point_array[][3], float color_array[][3],
																double radius, bool bTextured)
{

	int saved_style;
	double v21[3];
	double len;

	double c, s;

	if (m_ptrXforms != NULL)
		radius = 1.0;

	s = sin(2.0*M_PI / ((double)FFacets));
	c = cos(2.0*M_PI / ((double)FFacets));


	m_ptrCont_Norms[0][0] = 1.0;
	m_ptrCont_Norms[0][1] = 0.0;
	m_ptrContours[0][0] = radius;
	m_ptrContours[0][1] = 0.0;

	// draw a norm using recursion relations
	for (int i = 1; i<FFacets; i++)
	{
		m_ptrCont_Norms[i][0] = m_ptrCont_Norms[i - 1][0] * c - m_ptrCont_Norms[i - 1][1] * s;
		m_ptrCont_Norms[i][1] = m_ptrCont_Norms[i - 1][0] * s + m_ptrCont_Norms[i - 1][1] * c;
		m_ptrContours[i][0] = radius * m_ptrCont_Norms[i][0];
		m_ptrContours[i][1] = radius * m_ptrCont_Norms[i][1];
	}

	m_ptrCont_Norms[FFacets][0] = m_ptrCont_Norms[0][0];
	m_ptrCont_Norms[FFacets][1] = m_ptrCont_Norms[0][1];
	m_ptrContours[FFacets][0] = m_ptrContours[0][0];
	m_ptrContours[FFacets][1] = m_ptrContours[0][1];



	// avoid degenerate vectors
	// first, find a non-zero length segment
	int p = 0;
	FIND_NON_DEGENERATE_POINT(p, m_iPoints, len, v21, point_array)

		if (p == m_iPoints)
			return;

	// next, check to see if this segment lies along x-axis
	if ((v21[0] == 0.0) && (v21[2] == 0.0))
		m_ptrUp[0] = m_ptrUp[1] = m_ptrUp[2] = 1.0;

	else
	{
		m_ptrUp[0] = m_ptrUp[2] = 0.0;
		m_ptrUp[1] = 1.0;
	}

	// save the current join style
	saved_style = m_iJoinStyle;//extrusion_join_style;
	SetExtrusionMode(GLE_CONTOUR_CLOSED | saved_style);

	CgleBaseExtrusion::DrawExtrusion(point_array, color_array, bTextured);

	// restore the join style
	SetExtrusionMode(saved_style);
}
//----------------------------------------------------------------------------
						 