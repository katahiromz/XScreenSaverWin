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
class CgleScrew : protected CgleTwistExtrusion
{

public:

	CgleScrew(int Points, int ContourPoints, double Radius, double Twist);
	virtual ~CgleScrew();


	using CgleTwistExtrusion::SetExtrusionMode; 	// redeclare as public
	using CgleTwistExtrusion::GetExtrusionMode;

	using CgleTwistExtrusion::SetTextureMode;
	using CgleTwistExtrusion::GetTextureMode;

	using CgleTwistExtrusion::UseLitMaterial;
	using CgleTwistExtrusion::IsUsingLitMaterial;

	using CgleTwistExtrusion::LoadUpVector;

	using CgleTwistExtrusion::LoadContourPoint;


	void  Draw(double Length, float Color_Array[][3], bool bTextured = false);

	void  WireDraw(double Length, float Color_Array[][3], bool bTextured = false);

private:

	double FTwist;

};
//----------------------------------------------------------------------------
