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
class  CgleCylinderExtrusion : protected CgleConeExtrusion
{

public:

	CgleCylinderExtrusion(int Points, int Facets);
	virtual ~CgleCylinderExtrusion();


	using CgleConeExtrusion::SetExtrusionMode;   	// redeclare as public
	using CgleConeExtrusion::GetExtrusionMode;

	using CgleConeExtrusion::SetTextureMode;
	using CgleConeExtrusion::GetTextureMode;

	using CgleConeExtrusion::UseLitMaterial;
	using CgleConeExtrusion::IsUsingLitMaterial;

	using CgleConeExtrusion::LoadUpVector;


	void Draw(double Point_Array[][3], float Color_Array[][3], double Radius, bool bTextured = false);

	void WireDraw(double Point_Array[][3], float Color_Array[][3], double Radius, bool bTextured = false);


};
//----------------------------------------------------------------------------
