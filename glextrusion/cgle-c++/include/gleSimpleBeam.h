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
class  CgleSimpleBeam : protected CgleIBeam
{

public:


	CgleSimpleBeam(int Points, double Radius);
	virtual ~CgleSimpleBeam();


	using CgleIBeam::UseLitMaterial;
	using CgleIBeam::IsUsingLitMaterial;

	using CgleIBeam::LoadUpVector;



	void Draw(float Color_Array[][3], double Length, bool bTextured = false);

	void WireDraw(float Color_Array[][3], double Length, bool bTextured = false);


private:

	int FPoints;
	gleVector *FSpinePoints;

};
//----------------------------------------------------------------------------
