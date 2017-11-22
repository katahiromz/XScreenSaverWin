
/* 
 * helicoid (gernalized torus) demo 
 *
 * FUNCTION:
 * This code provides a very simple example of the helicoid primitive.
 *
 * =======> MOUSE HOOKED UP TO SWEEP, HEIGHT < ========
 *
 * HISTORY:
 * Written by Linas Vepstas, March 1995
 * Copyright (c) 1995 Linas Vepstas <linas@linas.org>
 */

/* required include files */
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/gle.h>
#include "main.h"
#include "texture.h"

void InitStuff (void) {}

/* draw the helix shape */
void DrawStuff (void) 
{
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   /* set up some matrices so that the object spins with the mouse */
   glPushMatrix ();
   glTranslatef (0.0, 0.0, -80.0);
   glRotatef (220.0, 0.0, 1.0, 0.0);
   glRotatef (65.0, 1.0, 0.0, 0.0);

   /* Phew. FINALLY, Draw the helix  -- */
   gleSetJoinStyle (TUBE_NORM_EDGE | TUBE_JN_ANGLE | TUBE_JN_CAP);
   gleHelicoid (1.0, 6.0, -1.0, 
               0.0, (0.02*lasty-2.0), 0x0, 0x0, 0.0, 6.0*lastx);

   glPopMatrix ();

   glutSwapBuffers ();
}

/* ---------------------- end of file ------------------ */
