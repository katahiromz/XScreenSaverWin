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
class  CgleTwistExtrusion : public CgleBaseExtrusion
{

public:


	CgleTwistExtrusion(int Points, int ContourPoints, double Radius);
	virtual ~CgleTwistExtrusion();


	using CgleBaseExtrusion::LoadContourPoint;     	// redeclare as public


	void Draw(double Point_Array[][3], float Color_Array[][3], double Twist_Array[], bool bTextured = false);

	void WireDraw(double Point_Array[][3], float Color_Array[][3], double Twist_Array[], bool bTextured = false);

};
//----------------------------------------------------------------------------
