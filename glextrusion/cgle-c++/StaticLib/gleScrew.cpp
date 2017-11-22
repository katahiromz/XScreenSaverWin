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
//												 CgleScrew
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleScrew::CgleScrew(int Points, int ContourPoints, double Radius, double Twist) :
	CgleTwistExtrusion(Points, ContourPoints, Radius)
{

	SetExtrusionMode(GLE_JN_CAP | GLE_CONTOUR_CLOSED | GLE_NORM_FACET
																						| GLE_JN_ANGLE);


	SetTextureMode(GLE_TEXTURE_VERTEX_CYL);
	FTwist = Twist;
	m_ptrPath = new gleVector[Points];
	m_ptrTwarr = new double[Points];

}
//----------------------------------------------------------------------------
CgleScrew::~CgleScrew()
{
	delete[] m_ptrPath;
	m_ptrPath = NULL;

	delete[] m_ptrTwarr;
	m_ptrTwarr = NULL;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleScrew::Draw(double Length, float Color_Array[][3], bool bTextured)
{

	double startz = 0.0;
	double endz = Length;

	double currz, delta;
	double currang, delang;


	// fill in the extrusion array and the twist array uniformly
	delta = (endz - startz) / ((double)(m_iPoints - 3));
	currz = startz - delta;
	delang = FTwist / ((double)(m_iPoints - 3));
	currang = -delang;

	for (int i = 1; i< m_iPoints - 1; i++)
	{
		m_ptrPath[i][0] = 0.0;
		m_ptrPath[i][1] = 0.0;
		m_ptrPath[i][2] = currz;
		currz += delta;
		m_ptrTwarr[i] = currang;
		currang += delang;
	}
	m_ptrPath[0][0] = m_ptrPath[1][0];
	m_ptrPath[0][1] = m_ptrPath[1][1];
	m_ptrPath[0][2] = m_ptrPath[1][2];
	m_ptrTwarr[0] = m_ptrTwarr[1];

	m_ptrPath[m_iPoints - 1][0] = m_ptrPath[m_iPoints - 2][0];
	m_ptrPath[m_iPoints - 1][1] = m_ptrPath[m_iPoints - 2][1];
	m_ptrPath[m_iPoints - 1][2] = m_ptrPath[m_iPoints - 2][2];
	m_ptrTwarr[m_iPoints - 1] = m_ptrTwarr[m_iPoints - 2];


	CgleTwistExtrusion::Draw(m_ptrPath, Color_Array, m_ptrTwarr, bTextured);
}
//----------------------------------------------------------------------------
void CgleScrew::WireDraw(double Length, float Color_Array[][3], bool bTextured)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Draw(Length, Color_Array, bTextured);
	glPopAttrib();
}
//----------------------------------------------------------------------------
