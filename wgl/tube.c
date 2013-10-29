/* tube, Copyright (c) 2001-2012 Jamie Zawinski <jwz@jwz.org>
 * Utility functions to create tubes and cones in GL.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <math.h>
#include "win32.h"

#if 0
	#ifdef HAVE_CONFIG_H
	# include "config.h"
	#endif


	#ifndef HAVE_COCOA
	# include <GL/gl.h>
	#endif

	#ifdef HAVE_JWZGLES
	# include "jwzgles.h"
	#endif /* HAVE_JWZGLES */
#endif

#include "tube.h"

typedef struct { GLfloat x, y, z; } XYZ;


static int
unit_tube (int faces, int smooth, int caps_p, int wire_p)
{
  int i;
  int polys = 0;
  GLfloat step = (float)(M_PI * 2 / faces);
  GLfloat s2 = step/2;
  GLfloat th;
  GLfloat x, y, x0=0, y0=0;
  int z = 0;

  int arraysize, out;
  struct { XYZ p; XYZ n; GLfloat s, t; } *array;

  arraysize = (faces+1) * 6;
  array = (void *) calloc (arraysize, sizeof(*array));
  if (! array) abort();
  out = 0;


  /* #### texture coords are currently not being computed */


  /* side walls
   */
  th = 0;
  x = 1;
  y = 0;

  if (!smooth)
    {
      x0 = COSF(s2);
      y0 = SINF(s2);
    }

  if (smooth) faces++;

  for (i = 0; i < faces; i++)
    {
      array[out].p.x = x;			/* bottom point A */
      array[out].p.y = 0;
      array[out].p.z = y;

      if (smooth)
        array[out].n = array[out].p;		/* its own normal */
      else
        {
          array[out].n.x = x0;			/* mid-plane normal */
          array[out].n.y = 0;
          array[out].n.z = y0;
        }
      out++;


      array[out].p.x = x;			/* top point A */
      array[out].p.y = 1;
      array[out].p.z = y;
      array[out].n = array[out-1].n;		/* same normal */
      out++;


      th += step;
      x  = COSF(th);
      y  = SINF(th);

      if (!smooth)
        {
          x0 = COSF(th + s2);
          y0 = SINF(th + s2);

          array[out].p.x = x;			/* top point B */
          array[out].p.y = 1;
          array[out].p.z = y;
          array[out].n = array[out-1].n;	/* same normal */
          out++;


          array[out] = array[out-3];		/* bottom point A */
          out++;

          array[out] = array[out-2];		/* top point B */
          out++;

          array[out].p.x = x;			/* bottom point B */
          array[out].p.y = 0;
          array[out].p.z = y;
          array[out].n = array[out-1].n;	/* same normal */
          out++;

          polys++;
        }

      polys++;
      if (out >= arraysize) abort();
    }

  glEnableClientState (GL_VERTEX_ARRAY);
  glEnableClientState (GL_NORMAL_ARRAY);
  glEnableClientState (GL_TEXTURE_COORD_ARRAY);

  glVertexPointer   (3, GL_FLOAT, sizeof(*array), &array[0].p);
  glNormalPointer   (   GL_FLOAT, sizeof(*array), &array[0].n);
  glTexCoordPointer (2, GL_FLOAT, sizeof(*array), &array[0].s);

  glFrontFace(GL_CCW);
  glDrawArrays ((wire_p ? GL_LINES :
                 (smooth ? GL_TRIANGLE_STRIP : GL_TRIANGLES)),
                0, out);


  /* End caps
   */
  if (caps_p)
    for (z = 0; z <= 1; z++)
      {
        out = 0;
        if (! wire_p)
          {
            array[out].p.x = 0;
            array[out].p.y = (float)z;
            array[out].p.z = 0;

            array[out].n.x = 0;
            array[out].n.y = (float)(z == 0 ? -1 : 1);
            array[out].n.z = 0;
            out++;
          }

        th = 0;
        for (i = (z == 0 ? 0 : faces);
             (z == 0 ? i <= faces : i >= 0);
             i += (z == 0 ? 1 : -1)) {
            GLfloat x = COSF(th);
            GLfloat y = SINF(th);

            array[out] = array[0];  /* same normal and texture */
            array[out].p.x = x;
            array[out].p.y = (float)z;
            array[out].p.z = y;
            out++;

            th += (z == 0 ? step : -step);

            polys++;
            if (out >= arraysize) abort();
          }

        glVertexPointer   (3, GL_FLOAT, sizeof(*array), &array[0].p);
        glNormalPointer   (   GL_FLOAT, sizeof(*array), &array[0].n);
        glTexCoordPointer (2, GL_FLOAT, sizeof(*array), &array[0].s);

        glFrontFace(GL_CCW);
        glDrawArrays ((wire_p ? GL_LINE_LOOP : GL_TRIANGLE_FAN), 0, out);
      }

  free(array);

  return polys;
}


static int
unit_cone (int faces, int smooth, int cap_p, int wire_p)
{
  int i;
  int polys = 0;
  GLfloat step = (float)(M_PI * 2 / faces);
  GLfloat s2 = step/2;
  GLfloat th;
  GLfloat x, y, x0, y0;

  int arraysize, out;
  struct { XYZ p; XYZ n; GLfloat s, t; } *array;

  arraysize = (faces+1) * 3;
  array = (void *) calloc (arraysize, sizeof(*array));
  if (! array) abort();
  out = 0;


  /* #### texture coords are currently not being computed */


  /* side walls
   */

  th = 0;
  x = 1;
  y = 0;
  x0 = COSF(s2);
  y0 = SINF(s2);

  for (i = 0; i < faces; i++)
    {
      array[out].p.x = x;		/* bottom point A */
      array[out].p.y = 0;
      array[out].p.z = y;

      if (smooth)
        array[out].n = array[out].p;	/* its own normal */
      else
        {
          array[out].n.x = x0;		/* mid-plane normal */
          array[out].n.y = 0;
          array[out].n.z = y0;
        }
      out++;


      array[out].p.x = 0;		/* tip point */
      array[out].p.y = 1;
      array[out].p.z = 0;

      array[out].n.x = x0;		/* mid-plane normal */
      array[out].n.y = 0;
      array[out].n.z = y0;
      out++;


      th += step;
      x0 = COSF(th + s2);
      y0 = SINF(th + s2);
      x  = COSF(th);
      y  = SINF(th);

      array[out].p.x = x;		/* bottom point B */
      array[out].p.y = 0;
      array[out].p.z = y;

      if (smooth)
        array[out].n = array[out].p;	/* its own normal */
      else
        array[out].n = array[out-1].n;  /* same normal as other two */
      out++;


      if (out >= arraysize) abort();
      polys++;
    }

  glEnableClientState (GL_VERTEX_ARRAY);
  glEnableClientState (GL_NORMAL_ARRAY);
  glEnableClientState (GL_TEXTURE_COORD_ARRAY);

  glVertexPointer   (3, GL_FLOAT, sizeof(*array), &array[0].p);
  glNormalPointer   (   GL_FLOAT, sizeof(*array), &array[0].n);
  glTexCoordPointer (2, GL_FLOAT, sizeof(*array), &array[0].s);

  glFrontFace(GL_CCW);
  glDrawArrays ((wire_p ? GL_LINES : GL_TRIANGLES), 0, out);


  /* End cap
   */
  if (cap_p)
    {
      out = 0;

      if (! wire_p)
        {
          array[out].p.x = 0;
          array[out].p.y = 0;
          array[out].p.z = 0;

          array[out].n.x = 0;
          array[out].n.y = -1;
          array[out].n.z = 0;
          out++;
        }

      for (i = 0, th = 0; i <= faces; i++)
        {
          GLfloat x = COSF(th);
          GLfloat y = SINF(th);

          array[out] = array[0];  /* same normal and texture */
          array[out].p.x = x;
          array[out].p.y = 0;
          array[out].p.z = y;
          out++;
          th += step;
          polys++;
          if (out >= arraysize) abort();
        }

      glVertexPointer   (3, GL_FLOAT, sizeof(*array), &array[0].p);
      glNormalPointer   (   GL_FLOAT, sizeof(*array), &array[0].n);
      glTexCoordPointer (2, GL_FLOAT, sizeof(*array), &array[0].s);

      glFrontFace(GL_CCW);
      glDrawArrays ((wire_p ? GL_LINE_LOOP : GL_TRIANGLE_FAN), 0, out);
    }

  free (array);

  return polys;
}


static int
tube_1 (GLfloat x1, GLfloat y1, GLfloat z1,
        GLfloat x2, GLfloat y2, GLfloat z2,
        GLfloat diameter, GLfloat cap_size,
        int faces, int smooth, int caps_p, int wire_p,
        int cone_p)
{
  GLfloat length, X, Y, Z;
  int polys = 0;

  if (diameter <= 0) abort();

  X = (x2 - x1);
  Y = (y2 - y1);
  Z = (z2 - z1);

  if (X == 0 && Y == 0 && Z == 0)
    return 0;

  length = (float)sqrt (X*X + Y*Y + Z*Z);

  glPushMatrix();

  glTranslatef(x1, y1, z1);
  glRotatef ((float)(-atan2 (X, Y)               * (180 / M_PI)), 0.0f, 0.0f, 1.0f);
  glRotatef ((float)( atan2 (Z, sqrt(X*X + Y*Y)) * (180 / M_PI)), 1.0f, 0.0f, 0.0f);
  glScalef (diameter, length, diameter);

  /* extend the endpoints of the tube by the cap size in both directions */
  if (cap_size != 0)
    {
      GLfloat c = cap_size/length;
      glTranslatef (0, -c, 0);
      glScalef (1, 1+c+c, 1);
    }

  if (cone_p)
    polys = unit_cone (faces, smooth, caps_p, wire_p);
  else
    polys = unit_tube (faces, smooth, caps_p, wire_p);

  glPopMatrix();
  return polys;
}


int
tube (GLfloat x1, GLfloat y1, GLfloat z1,
      GLfloat x2, GLfloat y2, GLfloat z2,
      GLfloat diameter, GLfloat cap_size,
      int faces, int smooth, int caps_p, int wire_p)
{
  return tube_1 (x1, y1, z1, x2, y2, z2, diameter, cap_size,
                 faces, smooth, caps_p, wire_p,
                 0);
}


int
cone (GLfloat x1, GLfloat y1, GLfloat z1,
      GLfloat x2, GLfloat y2, GLfloat z2,
      GLfloat diameter, GLfloat cap_size,
      int faces, int smooth, int cap_p, int wire_p)
{
  return tube_1 (x1, y1, z1, x2, y2, z2, diameter, cap_size,
                 faces, smooth, cap_p, wire_p,
                 1);
}
