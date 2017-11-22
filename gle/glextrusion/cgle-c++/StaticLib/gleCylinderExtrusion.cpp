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

#include "stdafx.h"
#include "extrusioninternals.h"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
//                            CgleCylinderExtrusion
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleCylinderExtrusion::CgleCylinderExtrusion(int Points, int Facets) : 
											CgleConeExtrusion(Points, Facets)
{
	try
	{
		delete[] m_ptrXforms;
		m_ptrXforms = NULL;
	}
	catch (...)
	{
	}

	// SetTextureMode(GLE_TEXTURE_VERTEX_CYL);
}
//----------------------------------------------------------------------------
CgleCylinderExtrusion::~CgleCylinderExtrusion()
{

}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleCylinderExtrusion::Draw(double Point_Array[][3],
												float Color_Array[][3], double Radius, bool bTextured)
{
	Gen_PolyCone(Point_Array, Color_Array, Radius, bTextured);
}
//----------------------------------------------------------------------------
void CgleCylinderExtrusion::WireDraw(double Point_Array[][3],
												float Color_Array[][3], double Radius, bool bTextured)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Draw(Point_Array, Color_Array, Radius, bTextured);
	glPopAttrib();
}
//----------------------------------------------------------------------------
