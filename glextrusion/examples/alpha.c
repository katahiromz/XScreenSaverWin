
/* 
 * FILE:
 * alpha.c
 * 
 * FUNCTION:
 * Alpha-blending/transparency demo 
 * Demo illustrates how to draw transparent shapes.
 * This demo is very similar to the cone.c demo, except
 * that it draws the basic shape twice: once fat and transparent, 
 * and once skiny but solid-colored.
 *
 * Note that this algoroithm is somewhat faulty: correct transparency
 * is acheived by drawing back-to-front, which can be done by using
 * the stencil planes.  This demo does not do this.
 *
 * HISTORY:
 * Linas Vepstas March 1995
 * March 2003, Derived from cone demo
 * Copyright (c) 1995,2003 Linas Vepstas <linas@linas.org>
 */

/* required include files */
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/gle.h>
#include "main.h"

/* the arrays in which we will store out polyline */
#define NPTS 6
double radii [NPTS];
double radii_small [NPTS];
double points [NPTS][3];
float colors [NPTS][4];
float colors_thin [NPTS][4];
int idx = 0;

/* some utilities for filling that array */
#define PNT(x,y,z) { 			\
   points[idx][0] = x; 			\
   points[idx][1] = y; 			\
   points[idx][2] = z;			\
   idx ++;				\
}

#define COL(r,g,b) { 			\
   colors[idx][0] = r; 			\
   colors[idx][1] = g; 			\
   colors[idx][2] = b;			\
   colors[idx][3] = 1.0;		\
   colors_thin[idx][0] = r; 			\
   colors_thin[idx][1] = g; 			\
   colors_thin[idx][2] = b;			\
   colors_thin[idx][3] = 0.5;		\
}

#define RAD(r) {                 \
   radii[idx] = 1.5*r;               \
   radii_small[idx] = 0.4*r;     \
}

/* 
 * Initialize a bent shape with three segments. 
 * The data format is a polyline.
 *
 * NOTE that neither the first, nor the last segment are drawn.
 * The first & last segment serve only to determine that angle 
 * at which the endcaps are drawn.
 */

void InitStuff (void)
{
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   /* initialize the join style here */
   gleSetJoinStyle (TUBE_NORM_EDGE | TUBE_JN_ANGLE | TUBE_JN_CAP);

   RAD (1.0);
   COL (0.0, 0.0, 0.0);
   PNT (-6.0, 6.0, 0.0);

   RAD (1.0);
   COL (0.0, 0.8, 0.3);
   PNT (6.0, 6.0, 0.0);

   RAD (3.0);
   COL (0.8, 0.3, 0.0);
   PNT (6.0, -6.0, 0.0);

   RAD (0.5);
   COL (0.2, 0.3, 0.9);
   PNT (-6.0, -6.0, 0.0);

   RAD (2.0);
   COL (0.2, 0.8, 0.5);
   PNT (-6.0, 6.0, 0.0);

   RAD (1.0);
   COL (0.0, 0.0, 0.0);
   PNT (6.0, 6.0, 0.0);
}

/* draw the polycone shape */
void DrawStuff (void) 
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* set up some matrices so that the object spins with the mouse */
   glPushMatrix ();
   glTranslatef (0.0, 0.0, -80.0);
   glRotatef (lastx, 0.0, 1.0, 0.0);
   glRotatef (lasty, 1.0, 0.0, 0.0);

   /* Phew. FINALLY, Draw the polycone  -- */
   glePolyCone_c4f (idx, points, colors, radii_small);
   glePolyCone_c4f (idx, points, colors_thin, radii);
	
   glPopMatrix ();

   glutSwapBuffers ();
}

/* --------------------------- end of file ------------------- */
