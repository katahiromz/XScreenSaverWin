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
#include "gleExtrusion.h"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
//									CgleExtrusion
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleExtrusion::CgleExtrusion(int Points, int ContourPoints, double Radius) :
									CgleBaseExtrusion(Points, ContourPoints, Radius)
{
}
//----------------------------------------------------------------------------
CgleExtrusion::~CgleExtrusion()
{
}
//----------------------------------------------------------------------------
void CgleExtrusion::SetTForm(double *xform[][2][3])
{
	m_ptrXforms = (double(*)[2][3])xform;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleExtrusion::Draw(double Point_Array[][3], float Color_Array[][3], bool bTextured)
{
	DrawExtrusion(Point_Array, Color_Array, bTextured);
}
//----------------------------------------------------------------------------
void CgleExtrusion::WireDraw(double Point_Array[][3], float Color_Array[][3], bool bTextured)
{
	DrawWireExtrusion(Point_Array, Color_Array, bTextured);
}
//----------------------------------------------------------------------------
