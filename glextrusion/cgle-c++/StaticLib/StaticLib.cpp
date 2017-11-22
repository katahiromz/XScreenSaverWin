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
void InitializeExtrusionLib()
{
	CreateGC();
}
//----------------------------------------------------------------------------
void ReleaseExtrusionLib()
{
	DestroyGC();
}
//----------------------------------------------------------------------------
