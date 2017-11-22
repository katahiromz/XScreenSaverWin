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
//											 CgleLatheExtrusion
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleLatheExtrusion::CgleLatheExtrusion(int Points, double StartXForm[2][3], int ContourPoints,
													double Sweep, double Radius) :
									CgleSpiralExtrusion(Points, StartXForm, ContourPoints, Sweep, Radius)
{


	FXForm[0][0] = 0.0;
	FXForm[0][1] = 0.0;
	FXForm[0][2] = 0.0;

	FXForm[1][0] = 0.0;
	FXForm[1][1] = 0.0;
	FXForm[1][2] = 0.0;
}
//----------------------------------------------------------------------------
CgleLatheExtrusion::~CgleLatheExtrusion()
{
}
//----------------------------------------------------------------------------
void  CgleLatheExtrusion::SetTForm(double xform[2][3])
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			FXForm[i][j] = xform[i][j];
		}
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//void  CgleLatheExtrusion::LoadContourPoint(int Index, double X, double Y)
//{
//	CgleBaseExtrusion::LoadContourPoint(Index, X, Y);
//}
//----------------------------------------------------------------------------
void  CgleLatheExtrusion::Draw(double StartRadius,
										double dRadiusdTheta, double StartZ,
										double dZdTheta, double StartTheta,
										float Color_Array[][3], bool bTextured)
{

	double localup[3];
	double len;
	double trans[2];
	double delt[2][3];

	if (m_ptrUp[1] != 0.0)
	{
		localup[0] = m_ptrUp[0];
		localup[1] = 0.0;
		localup[2] = m_ptrUp[2];
		VEC_LENGTH(len, localup);
		if (len != 0.0)
		{
			len = 1.0 / len;
			localup[0] *= len;
			localup[2] *= len;
			VEC_SCALE(localup, len, localup);
		}
		else
		{
			// invalid up vector was passed in
			localup[0] = 0.0;
			localup[2] = 1.0;
		}
	}
	else
	{
		VEC_COPY(localup, m_ptrUp);
	}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			delt[i][j] = FXForm[i][j];
		}
	}

	trans[0] = localup[2] * dRadiusdTheta - localup[0] * dZdTheta;
	trans[1] = localup[0] * dRadiusdTheta + localup[2] * dZdTheta;

	// now, add this translation vector into the affine xform
	delt[0][2] += trans[0];
	delt[1][2] += trans[1];

	CgleSpiralExtrusion::SetTForm(delt);

	CgleSpiralExtrusion::Draw(StartRadius, 0.0, StartZ, 0.0, StartTheta,
		Color_Array, bTextured);
}
//----------------------------------------------------------------------------
void  CgleLatheExtrusion::WireDraw(double StartRadius,
	double dRadiusdTheta, double StartZ,
	double dZdTheta, double StartTheta,
	float Color_Array[][3], bool bTextured)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Draw(StartRadius, dRadiusdTheta, StartZ, dZdTheta, StartTheta, Color_Array, bTextured);
	glPopAttrib();
}
//----------------------------------------------------------------------------
