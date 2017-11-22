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
//											 CgleTaperExtrusion
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleTaperExtrusion::CgleTaperExtrusion(int Points,
														int ContourPoints, double Radius,
														double Twist) :
														CgleBaseExtrusion(Points, ContourPoints,
															Radius)
{
	SetExtrusionMode(GLE_JN_CAP | GLE_CONTOUR_CLOSED | GLE_NORM_FACET |
		GLE_JN_ROUND);

	SetTextureMode(GLE_TEXTURE_NORMAL_MODEL_CYL);

	FTwist = Twist;
	m_ptrPath = new gleVector[Points];
	m_ptrXforms = new gleAffine[m_iPoints];
}
//----------------------------------------------------------------------------
CgleTaperExtrusion::~CgleTaperExtrusion()
{
	delete[] m_ptrXforms;
	m_ptrXforms = NULL;

	delete[] m_ptrPath;
	m_ptrPath = NULL;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleTaperExtrusion::Draw(double Length, double Distortion,
											float Color_Array[][3], bool bTextured)
{

	double startz = -(Length / 2.0);
	double endz = Length / 2.0;
	double si, co, taper, aponent;
	double taper_step, taper_z;


	double currz, delta;
	double currang, delang;
	aponent = fabs(Distortion);

	if (aponent == 0.0)
		aponent = 1.0E-10;
	taper_z = -.9999;
	taper_step = 1.9998 / (m_iPoints - 3);

	delta = (endz - startz) / ((double)(m_iPoints - 3));
	currz = startz - delta;
	delang = 2.0 * (FTwist / ((double)(m_iPoints - 3)));
	currang = -delang;

	for (int i = 1; i< m_iPoints - 1; i++)
	{
		m_ptrPath[i][0] = 0.0;
		m_ptrPath[i][1] = 0.0;
		m_ptrPath[i][2] = currz;

		si = sin(currang);
		co = cos(currang);
		taper = pow((1.0 - pow(fabs(taper_z), 1.0 / aponent)), aponent);
		taper_z += taper_step;

		m_ptrXforms[i][0][0] = taper * co;
		m_ptrXforms[i][0][1] = -taper * si;
		m_ptrXforms[i][0][2] = 0.0;
		m_ptrXforms[i][1][0] = taper * si;
		m_ptrXforms[i][1][1] = taper * co;
		m_ptrXforms[i][1][2] = 0.0;

		currz += delta;
		currang += delang;
	}

	m_ptrPath[0][0] = m_ptrPath[1][0];
	m_ptrPath[0][1] = m_ptrPath[1][1];
	m_ptrPath[0][2] = m_ptrPath[1][2];

	m_ptrPath[m_iPoints - 1][0] = m_ptrPath[m_iPoints - 2][0];
	m_ptrPath[m_iPoints - 1][1] = m_ptrPath[m_iPoints - 2][1];
	m_ptrPath[m_iPoints - 1][2] = m_ptrPath[m_iPoints - 2][2];

	m_ptrXforms[0][0][0] = m_ptrXforms[1][0][0];
	m_ptrXforms[0][0][1] = m_ptrXforms[1][0][1];
	m_ptrXforms[0][0][2] = m_ptrXforms[1][0][2];
	m_ptrXforms[0][1][0] = m_ptrXforms[1][1][0];
	m_ptrXforms[0][1][1] = m_ptrXforms[1][1][1];
	m_ptrXforms[0][1][2] = m_ptrXforms[1][1][0];

	m_ptrXforms[m_iPoints - 1][0][0] = m_ptrXforms[m_iPoints - 2][0][0];
	m_ptrXforms[m_iPoints - 1][0][1] = m_ptrXforms[m_iPoints - 2][0][1];
	m_ptrXforms[m_iPoints - 1][0][2] = m_ptrXforms[m_iPoints - 2][0][2];
	m_ptrXforms[m_iPoints - 1][1][0] = m_ptrXforms[m_iPoints - 2][1][0];
	m_ptrXforms[m_iPoints - 1][1][1] = m_ptrXforms[m_iPoints - 2][1][1];
	m_ptrXforms[m_iPoints - 1][1][2] = m_ptrXforms[m_iPoints - 2][1][0];

	CgleBaseExtrusion::DrawExtrusion(m_ptrPath, Color_Array, bTextured);
}
//----------------------------------------------------------------------------
void CgleTaperExtrusion::WireDraw(double Length, double Distortion,
												float Color_Array[][3], bool bTextured)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Draw(Length, Distortion, Color_Array, bTextured);
	glPopAttrib();
}
//----------------------------------------------------------------------------
