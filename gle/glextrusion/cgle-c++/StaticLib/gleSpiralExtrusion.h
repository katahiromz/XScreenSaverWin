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


//----------------------------------------------------------------------------
class CgleSpiralExtrusion : public CgleBaseExtrusion
{

public:

	CgleSpiralExtrusion(int Points, double StartXForm[2][3],
							int ContourPoints, double Sweep, double Radius);

	virtual ~CgleSpiralExtrusion();


	using CgleBaseExtrusion::LoadContourPoint;	// redeclare as public


	virtual void  SetTForm(double dXFormdTheta[2][3]);


	virtual void Draw(double StartRadius, double dRadiusdTheta, double StartZ,
							double dZdTheta, double StartTheta,
							float Color_Array[][3], bool bTextured = false);

	virtual void WireDraw(double StartRadius, double dRadiusdTheta, double StartZ,
							double dZdTheta, double StartTheta,
							float Color_Array[][3], bool bTextured = false);



protected:


	char* FSpiralMem_Anchor;
	double *FPts;
	double FDeltaAngle, FSweep;
	double FStartXForm[2][3];

};
//----------------------------------------------------------------------------
