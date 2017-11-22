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
//									 CgleSimpleTwistedIBeam
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleSimpleTwistedBeam::CgleSimpleTwistedBeam(int Points,
	double Radius) : CgleTwistedIBeam(Points, Radius)
{
	m_iPrivatePoints = Points;
	FSpinePoints = new gleVector[Points];

	for (int i = 0; i < Points; i++)
	{
		FSpinePoints[i][0] = 0.0;
		FSpinePoints[i][1] = 0.0;
	}
}
//----------------------------------------------------------------------------
CgleSimpleTwistedBeam::~CgleSimpleTwistedBeam()
{
	delete[] FSpinePoints;
	FSpinePoints = NULL;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleSimpleTwistedBeam::Draw(float Color_Array[][3],
	double Twist_Array[], double Length, bool bTextured)
{
	double interval = 0;

	if (m_iPrivatePoints > 4)
		interval = Length / (m_iPrivatePoints - 3);

	double end = Length / 2.0;

	for (int i = 2; i < m_iPrivatePoints - 2; i++)
		FSpinePoints[i][2] = -end + (interval * (i - 1));

	FSpinePoints[0][2] = -end;
	FSpinePoints[1][2] = -end;
	FSpinePoints[m_iPrivatePoints - 2][2] = end;
	FSpinePoints[m_iPrivatePoints - 1][2] = end;

	int style = CgleBaseExtrusion::GetExtrusionMode();
	int newstyle = style | GLE_JN_CAP;
	SetExtrusionMode(newstyle);
	CgleTwistedIBeam::Draw(FSpinePoints, Color_Array, Twist_Array, bTextured);
	SetExtrusionMode(style);

}
//----------------------------------------------------------------------------
void  CgleSimpleTwistedBeam::WireDraw(float Color_Array[][3],
	double Twist_Array[], double Length, bool bTextured)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Draw(Color_Array, Twist_Array, Length, bTextured);
	glPopAttrib();
}
//----------------------------------------------------------------------------
