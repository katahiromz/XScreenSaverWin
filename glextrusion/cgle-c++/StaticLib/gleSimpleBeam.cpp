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
//												 CgleSimpleBeam
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleSimpleBeam::CgleSimpleBeam(int Points, double Radius) :
	CgleIBeam(Points, Radius)
{
	m_iPoints = Points;
	FSpinePoints = new gleVector[m_iPoints];

	for (int i = 0; i < Points; i++)
	{
		FSpinePoints[i][0] = 0.0;
		FSpinePoints[i][1] = 0.0;
	}
}
//----------------------------------------------------------------------------
CgleSimpleBeam::~CgleSimpleBeam()
{
	delete[] FSpinePoints;
	FSpinePoints = NULL;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleSimpleBeam::Draw(float Color_Array[][3], double Length, bool bTextured)
{
	double interval = 0;

	if (m_iPoints > 4)
		interval = Length / (double)(m_iPoints - 3);

	double end = Length / 2.0;

	for (int i = 2; i < m_iPoints - 2; i++)
		FSpinePoints[i][2] = -end + (interval * ((double)(i - 1)));

	FSpinePoints[0][2] = -end;
	FSpinePoints[1][2] = -end;
	FSpinePoints[m_iPoints - 2][2] = end;
	FSpinePoints[m_iPoints - 1][2] = end;

	int style = GetExtrusionMode();
	int newstyle = style | GLE_JN_CAP;
	SetExtrusionMode(newstyle);
	CgleIBeam::Draw(FSpinePoints, Color_Array, bTextured);
	SetExtrusionMode(style);
}
//----------------------------------------------------------------------------
void  CgleSimpleBeam::WireDraw(float Color_Array[][3], double Length, bool bTextured)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Draw(Color_Array, Length, bTextured);
	glPopAttrib();
}
//----------------------------------------------------------------------------
