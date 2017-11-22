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
class  CgleLatheExtrusion : public CgleSpiralExtrusion
{

public:

	CgleLatheExtrusion(int Points, double StartXForm[2][3],
										int ContourPoints, double Sweep, double Radius);

	virtual ~CgleLatheExtrusion();


	// overide next 3 members from CgleSpiralExtrusion;

	void  SetTForm(double dXFormdTheta[2][3]) override;


	void  Draw(double StartRadius, double dRadiusdTheta,
					double StartZ, double dZdTheta, double StartTheta,
					float Color_Array[][3], bool bTextured = false) override;

	void  WireDraw(double StartRadius, double dRadiusdTheta,
					double StartZ, double dZdTheta, double StartTheta,
					float Color_Array[][3], bool bTextured = false) override;


protected:


private:

	int FFacets;
	double FXForm[2][3];


};
//----------------------------------------------------------------------------
