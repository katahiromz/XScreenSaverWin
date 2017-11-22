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
#include "ExtrusionLib.h"
#include "extrusioninternals.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
//								 Factory Functions
//
//----------------------------------------------------------------------------
////----------------------------------------------------------------------------
// void gleDestroyAllExtrusionObjects()
//{
//	PurgeList();
//}		
////----------------------------------------------------------------------------
////----------------------------------------------------------------------------
// CgleExtrusion* gleCreateExtrusion(int points,
//																				  int contourpoints, double Radius)
//{
//   CgleExtrusion* ptr = new CgleExtrusion(points, contourpoints, Radius);
//   AddItemToList(ptr);
//   return(ptr);
//}
////----------------------------------------------------------------------------
// void gleDestroyExtrusion(CgleExtrusion* object)
//{
//   RemoveItemFromList((CgleBaseExtrusion*)object);
//}
////----------------------------------------------------------------------------



 /*

//----------------------------------------------------------------------------
 CgleConeExtrusion* gleCreateConeExtrusion(int Points, int Facets)
{
   CgleConeExtrusion* ptr = new CgleConeExtrusion(Points, 
																		Facets);
   AddItemToList((CgleBaseExtrusion*)ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
 void gleDestroyConeExtrusion(CgleConeExtrusion* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleCylinderExtrusion*  gleCreateCylinderExtrusion(int Points,
                                                                    int Facets)
{
   CgleCylinderExtrusion* ptr = new CgleCylinderExtrusion
                                                              (Points, Facets);
   AddItemToList((CgleBaseExtrusion*)ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
 void  gleDestroyCylinderExtrusion(CgleCylinderExtrusion* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleIBeam* gleCreateIBeam(int Points, double Radius)
{
   CgleIBeam* ptr = new CgleIBeam(Points, Radius);
   AddItemToList((CgleBaseExtrusion*)ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
 void gleDestroyIBeam(CgleIBeam* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleSimpleBeam* gleCreateSimpleBeam(double Radius,
                                                      int InternalPoints)
{
   InternalPoints = (int)(floor((float)InternalPoints));

   if (InternalPoints < 0)
      InternalPoints = 4;
   else
      InternalPoints += 4;

   CgleSimpleBeam* ptr = new CgleSimpleBeam(InternalPoints, Radius);

   AddItemToList((CgleBaseExtrusion*)ptr);

   return(ptr);
}
//----------------------------------------------------------------------------
 void gleDestroySimpleBeam(CgleSimpleBeam* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleTwistExtrusion* gleCreateTwistExtrusion(int Points,
                                              int ContourPoints, double Radius)
{
   CgleTwistExtrusion* ptr = new CgleTwistExtrusion(Points,
                                                        ContourPoints, Radius);
   AddItemToList((CgleBaseExtrusion*) ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
 void gleDestroyTwistExtrusion(CgleTwistExtrusion* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleScrew*  gleCreateScrew(int ContourPoints, double Radius,
                                                                 double Twist)
{
   int points = ((int) fabs (Twist / 18.0)) + 2;

   CgleScrew* ptr = new CgleScrew(points, ContourPoints, Radius, Twist);
   AddItemToList((CgleBaseExtrusion*)ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
 void  gleDestroyScrew(CgleScrew* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleTwistedIBeam* gleCreateTwistedIBeam(int Points,
                                                               double Radius)
{
   CgleTwistedIBeam* ptr = new CgleTwistedIBeam(Points, Radius);
   AddItemToList((CgleBaseExtrusion*)ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
 void  gleDestroyTwistedIBeam(CgleTwistedIBeam* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleSimpleTwistedBeam*  gleCreateSimpleTwistedBeam(double Radius,
                                                      int InternalPoints)
{
   InternalPoints = (int)(floor((float)InternalPoints));

   if (InternalPoints < 0)
      InternalPoints = 4;
   else
      InternalPoints += 4;

   CgleSimpleTwistedBeam* ptr = new CgleSimpleTwistedBeam
                                                     (InternalPoints, Radius);
   AddItemToList((CgleBaseExtrusion*)ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
 void  gleDestroySimpleTwistedBeam(CgleSimpleTwistedBeam* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleSpiralExtrusion*  gleCreateSpiralExtrusion(double Sweep,
                                                            int ContourPoints, int Slices, double Radius)
{

   int points = (int) ((((double) Slices) /360.0) * fabs(Sweep)) + 4;

   CgleSpiralExtrusion* ptr =  new CgleSpiralExtrusion(points, NULL,
																							ContourPoints, Sweep, Radius);
   AddItemToList((CgleBaseExtrusion*)ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
 void  gleDestroySpiralExtrusion(CgleSpiralExtrusion* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleHelicoidExtrusion* gleCreateHelicoidExtrusion(double Sweep,
                                                     int Facets, int Slices)
{

   int points = (int) ((((double) Slices) /360.0) * fabs(Sweep)) + 4;

   CgleHelicoidExtrusion* ptr = new CgleHelicoidExtrusion(points, NULL, Sweep,Facets);

   AddItemToList((CgleBaseExtrusion*)ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
 void gleDestroyHelicoidExtrusion(CgleHelicoidExtrusion* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleLatheExtrusion*  gleCreateLatheExtrusion(double Sweep,
																			double StartXForm[2][3], int ContourPoints,
																										int Slices, double Radius)
{

   double xform[2][3];

   if (StartXForm == NULL)
   {
      xform[0][0] = 1.0;
      xform[0][1] = 0.0;
      xform[0][2] = 0.0;
      xform[1][0] = 0.0;
      xform[1][1] = 1.0;
      xform[1][2] = 0.0;

   }
   else
   {
      for (int i = 0;i < 2;i++)
      {
         for (int j = 0;j < 3;j++)
         {
            xform[i][j] = StartXForm[i][j];
         }
      }
   }


   int points = (int) ((((double) Slices) /360.0) * fabs(Sweep)) + 4;

   CgleLatheExtrusion* ptr =  new CgleLatheExtrusion(points,xform,ContourPoints, Sweep, Radius);

   AddItemToList((CgleBaseExtrusion*)ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
 void  gleDestroyLatheExtrusion(CgleLatheExtrusion* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleToroidExtrusion*  gleCreateToroidExtrusion(double Sweep,
                                                     int Facets, int Slices)
{

   int points = (int) ((((double) Slices) /360.0) * fabs(Sweep)) + 4;

   double xform[2][3];

   xform[0][0] = 1.0;
   xform[0][1] = 0.0;
   xform[0][2] = 0.0;
   xform[1][0] = 0.0;
   xform[1][1] = 1.0;
   xform[1][2] = 0.0;


   CgleToroidExtrusion* ptr = new CgleToroidExtrusion(points, xform, Sweep,Facets);

   AddItemToList((CgleBaseExtrusion*)ptr);
   return(ptr);
}
//----------------------------------------------------------------------------
  void  gleDestroyToroidExtrusion(CgleToroidExtrusion* object)
{
   RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
 CgleTaperExtrusion*  gleCreateTaperExtrusion(int ContourPoints,
																					double  Radius, double Twist)
{
	int points = (int)(( fabs (Twist / 18.0)) + 2);
	CgleTaperExtrusion* ptr = new CgleTaperExtrusion(points, ContourPoints, Radius, Twist);

	AddItemToList((CgleBaseExtrusion*)ptr);
	return(ptr);
}
//----------------------------------------------------------------------------
 void  gleDestroyTaperExtrusion(CgleTaperExtrusion* object)
{
	RemoveItemFromList((CgleBaseExtrusion*)object);
}
//----------------------------------------------------------------------------
					*/	 