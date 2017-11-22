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
class CgleIBeam : protected CgleBaseExtrusion
{

public:


	CgleIBeam(int Points, double Radius);
	virtual ~CgleIBeam();

	using CgleBaseExtrusion::UseLitMaterial;	   	// redeclare as public
	using CgleBaseExtrusion::IsUsingLitMaterial;

	using CgleBaseExtrusion::LoadUpVector;


	void  Draw(double Point_Array[][3], float Color_Array[][3], bool bTextured = false);

	void  WireDraw(double Point_Array[][3], float Color_Array[][3], bool bTextured = false);

};
//----------------------------------------------------------------------------

