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
//											 CgleHelicoidExtrusion
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleHelicoidExtrusion::CgleHelicoidExtrusion(int Points, double StartXForm[2][3],
	double Sweep, int Facets) :
	CgleSpiralExtrusion(Points, StartXForm,
		Facets, Sweep, NULL)
{

	FFacets = Facets;

}
//----------------------------------------------------------------------------
CgleHelicoidExtrusion::~CgleHelicoidExtrusion()
{
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  CgleHelicoidExtrusion::Draw(double CrossSectionRadius, double StartRadius,
													double dRadiusdTheta, double StartZ,
													double dZdTheta, double StartTheta,
													float Color_Array[][3], bool bTextured)
{

	double c, s;


	// initialize sine and cosine for circle recursion equations
	s = sin(2.0*M_PI / ((double)FFacets));
	c = cos(2.0*M_PI / ((double)FFacets));

	m_ptrCont_Norms[0][0] = 1.0;
	m_ptrCont_Norms[0][1] = 0.0;
	m_ptrContours[0][0] = CrossSectionRadius;
	m_ptrContours[0][1] = 0.0;

	// draw a norm using recursion relations
	for (int i = 1; i< FFacets + 1; i++)
	{
		m_ptrCont_Norms[i][0] = m_ptrCont_Norms[i - 1][0] * c - m_ptrCont_Norms[i - 1][1] * s;
		m_ptrCont_Norms[i][1] = m_ptrCont_Norms[i - 1][0] * s + m_ptrCont_Norms[i - 1][1] * c;
		m_ptrContours[i][0] = CrossSectionRadius * m_ptrCont_Norms[i][0];
		m_ptrContours[i][1] = CrossSectionRadius * m_ptrCont_Norms[i][1];
	}


	CgleSpiralExtrusion::Draw(StartRadius, dRadiusdTheta, StartZ, dZdTheta,
										StartTheta, Color_Array, bTextured);
}
//----------------------------------------------------------------------------
void  CgleHelicoidExtrusion::WireDraw(double CrossSectionRadius, double StartRadius,
													double dRadiusdTheta, double StartZ,
													double dZdTheta, double StartTheta,
													float Color_Array[][3], bool bTextured)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Draw(CrossSectionRadius, StartRadius, dRadiusdTheta, StartZ, dZdTheta,
										StartTheta, Color_Array, bTextured);
	glPopAttrib();
}
//----------------------------------------------------------------------------
