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
class  CgleSimpleTwistedBeam : protected CgleTwistedIBeam
{

public:


	CgleSimpleTwistedBeam(int Points, double Radius);
	virtual ~CgleSimpleTwistedBeam();



	using CgleTwistedIBeam::UseLitMaterial;
	using CgleTwistedIBeam::IsUsingLitMaterial;

	using CgleTwistedIBeam::LoadUpVector;



	void   Draw(float Color_Array[][3], double Twist_Array[],
					double Length, bool bTextured = false);

	void   WireDraw(float Color_Array[][3], double Twist_Array[],
					double Length, bool bTextured = false);


private:

	int m_iPrivatePoints;
	gleVector *FSpinePoints;
};
//----------------------------------------------------------------------------
