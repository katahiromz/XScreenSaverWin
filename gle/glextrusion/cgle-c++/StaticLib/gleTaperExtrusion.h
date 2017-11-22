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
class CgleTaperExtrusion : public CgleBaseExtrusion
{


public:


	CgleTaperExtrusion(int Points, int ContourPoints, double Radius, double Twist);
	virtual ~CgleTaperExtrusion();


	using CgleBaseExtrusion::LoadContourPoint;	// redeclare as public


	void   Draw(double Length, double Distortion, float Color_Array[][3], bool bTextured = false);

	void   WireDraw(double Length, double Distortion, float Color_Array[][3], bool bTextured = false);


private:

	double FTwist;

};
//----------------------------------------------------------------------------
