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
class  CgleHelicoidExtrusion : protected CgleSpiralExtrusion
{

public:

	CgleHelicoidExtrusion(int Points, double StartXForm[2][3], double Sweep, int Facets);

	virtual ~CgleHelicoidExtrusion();


	using CgleSpiralExtrusion::SetExtrusionMode;   	// redeclare as public
	using CgleSpiralExtrusion::GetExtrusionMode;

	using CgleSpiralExtrusion::SetTextureMode;
	using CgleSpiralExtrusion::GetTextureMode;

	using CgleSpiralExtrusion::UseLitMaterial;
	using CgleSpiralExtrusion::IsUsingLitMaterial;

	using CgleSpiralExtrusion::LoadUpVector;



	void    Draw(double CrossSectionRadius, double StartRadius,
							double dRadiusdTheta, double StartZ,
							double dZdTheta, double StartTheta,
							float Color_Array[][3], bool bTextured = false);

	void    WireDraw(double CrossSectionRadius, double StartRadius,
							double dRadiusdTheta, double StartZ,
							double dZdTheta, double StartTheta,
							float Color_Array[][3], bool bTextured = false);


private:

	int FFacets;

};
//----------------------------------------------------------------------------
