%title "GLE Module. Extrusion and tubing primitives library"
%module gle
%include NumericArrays.i

/* 
 * gle.h
 *
 * FUNCTION:
 * Tubing and Extrusion header file.
 * This file provides protypes and defines for the extrusion 
 * and tubing primitives.
 *
 * HISTORY:
 * Linas Vepstas 1990, 1991
 */

#ifndef __GLE_H__
#define __GLE_H__

/* some types */
/* some types */
/*#define __GLE_DOUBLE  1
#if __GLE_DOUBLE
typedef double gleDouble;
#else 
typedef float gleDouble;
#endif*/
#define gleDouble double
typedef gleDouble gleAffine[2][3];

%{
#define gleDouble double
typedef gleDouble gleAffine[2][3];
%}


/* ====================================================== */

/* defines for tubing join styles */
#define TUBE_JN_RAW          0x1
#define TUBE_JN_ANGLE        0x2
#define TUBE_JN_CUT          0x3
#define TUBE_JN_ROUND        0x4
#define TUBE_JN_MASK         0xf    /* mask bits */
#define TUBE_JN_CAP          0x10

/* determine how normal vectors are to be handled */
#define TUBE_NORM_FACET      0x100
#define TUBE_NORM_EDGE       0x200
#define TUBE_NORM_PATH_EDGE  0x400 /* for spiral, lathe, helix primitives */
#define TUBE_NORM_MASK       0xf00    /* mask bits */

/* closed or open countours */
#define TUBE_CONTOUR_CLOSED	0x1000

#define GLE_TEXTURE_ENABLE	0x10000
#define GLE_TEXTURE_STYLE_MASK	0xff
#define GLE_TEXTURE_VERTEX_FLAT		1
#define GLE_TEXTURE_NORMAL_FLAT		2
#define GLE_TEXTURE_VERTEX_CYL		3
#define GLE_TEXTURE_NORMAL_CYL		4
#define GLE_TEXTURE_VERTEX_SPH		5
#define GLE_TEXTURE_NORMAL_SPH		6
#define GLE_TEXTURE_VERTEX_MODEL_FLAT	7
#define GLE_TEXTURE_NORMAL_MODEL_FLAT	8
#define GLE_TEXTURE_VERTEX_MODEL_CYL	9
#define GLE_TEXTURE_NORMAL_MODEL_CYL	10
#define GLE_TEXTURE_VERTEX_MODEL_SPH	11
#define GLE_TEXTURE_NORMAL_MODEL_SPH	12

#ifdef GL_32
/* HACK for GL 3.2 -- needed because no way to tell if lighting is on.  */
#define TUBE_LIGHTING_ON	0x80000000

#define gleExtrusion		extrusion
#define gleSetJoinStyle		setjoinstyle
#define gleGetJoinStyle		getjoinstyle
#define glePolyCone		polycone
#define glePolyCylinder		polycylinder
#define	gleSuperExtrusion	super_extrusion
#define	gleTwistExtrusion	twist_extrusion
#define	gleSpiral		spiral
#define	gleLathe		lathe
#define	gleHelicoid		helicoid
#define	gleToroid		toroid
#define	gleScrew		screw

#endif /* GL_32 */

#ifdef _NO_PROTO		/* NO ANSI C PROTOTYPING */

extern int gleGetJoinStyle ();
extern void gleSetJoinStyle ();
extern void glePolyCone ();
extern void glePolyCylinder ();
extern void gleExtrusion ();
extern void gleSuperExtrusion ();
extern void gleTwistExtrusion ();
extern void gleSpiral ();
extern void gleLathe ();
extern void gleHelicoid ();
extern void gleToroid ();
extern void gleScrew ();

#else /* _NO_PROTO */		/* ANSI C PROTOTYPING */

extern int gleGetJoinStyle (void);
extern void gleSetJoinStyle (int style);	/* bitwise OR of flags */

#typemaps for Numeric arrays
DOUBLE_ARRAY2D( point_array, [1][3], npoints )
%apply float ARRAY2D_NULL[1][3] { float color_array[1][3] }

/* draw polycylinder, specified as a polyline */
extern void glePolyCylinder (int npoints,	/* num points in polyline */
                   gleDouble point_array[1][3],	/* polyline vertces */
                   float color_array[1][3],     /* colors at polyline verts */
                   gleDouble radius);		/* radius of polycylinder */


#typemaps for Numeric arrays
%apply double VECTOR[1] { double radius_array[1] };

/* draw polycone, specified as a polyline with radii */
extern void glePolyCone (int npoints,	 /* numpoints in poly-line */
                   gleDouble point_array[1][3],	/* polyline vertices */
                   float color_array[1][3],	/* colors at polyline verts */
                   gleDouble radius_array[1]); /* cone radii at polyline verts */


#typemaps for Numeric arrays
DOUBLE_ARRAY2D( contour, [1][2], ncp )
%apply double ARRAY2D[1][2] { double cont_normal[1][2] };
%apply double VECTOR_NULL[3] { double up[3] };

/* extrude arbitrary 2D contour along arbitrary 3D path */
extern void gleExtrusion (int ncp,         /* number of contour points */
                gleDouble contour[1][2],     /* 2D contour */
                gleDouble cont_normal[1][2], /* 2D contour normals */
                gleDouble up[3],            /* up vector for contour */
                int npoints,            /* numpoints in poly-line */
                gleDouble point_array[1][3], /* polyline vertices */
                float color_array[1][3]); /* colors at polyline verts */


#typemaps for Numeric arrays
%apply double VECTOR[1] { double twist_array[1] };

/* extrude 2D contour, specifying local rotations (twists) */
extern void gleTwistExtrusion (int ncp,         /* number of contour points */
                gleDouble contour[1][2],    /* 2D contour */
                gleDouble cont_normal[1][2], /* 2D contour normals */
                gleDouble up[3],           /* up vector for contour */
                int npoints,           /* numpoints in poly-line */
                gleDouble point_array[1][3],        /* polyline vertices */
                float color_array[1][3],        /* color at polyline verts */
                gleDouble twist_array[1]);   /* countour twists (in degrees) */


#typemaps for Numeric arrays
%apply double ARRAY3D_NULL[1][2][3] { double xform_array[1][2][3] };

/* extrude 2D contour, specifying local affine tranformations */
extern void gleSuperExtrusion (int ncp,  /* number of contour points */
                gleDouble contour[1][2],    /* 2D contour */
                gleDouble cont_normal[1][2], /* 2D contour normals */
                gleDouble up[3],           /* up vector for contour */
                int npoints,           /* numpoints in poly-line */
                gleDouble point_array[1][3],        /* polyline vertices */
                float color_array[1][3],        /* color at polyline verts */
                gleDouble xform_array[1][2][3]);   /* 2D contour xforms */


#typemaps for Numeric arrays
%apply double ARRAY2D_NULL[2][3] { double startXform[2][3] };
%apply double ARRAY2D_NULL[2][3] { double dXformdTheta[2][3] };

/* spiral moves contour along helical path by parallel transport */
extern void gleSpiral (int ncp,        /* number of contour points */
             gleDouble contour[1][2],    /* 2D contour */
             gleDouble cont_normal[1][2], /* 2D contour normals */
             gleDouble up[3],           /* up vector for contour */
             gleDouble startRadius,	/* spiral starts in x-y plane */
             gleDouble drdTheta,        /* change in radius per revolution */
             gleDouble startZ,		/* starting z value */
             gleDouble dzdTheta,        /* change in Z per revolution */
             gleDouble startXform[2][3], /* starting contour affine xform */
             gleDouble dXformdTheta[2][3], /* tangent change xform per revoln */
             gleDouble startTheta,	/* start angle in x-y plane */
             gleDouble sweepTheta);	/* degrees to spiral around */


/* lathe moves contour along helical path by helically shearing 3D space */
extern void gleLathe (int ncp,        /* number of contour points */
             gleDouble contour[1][2],    /* 2D contour */
             gleDouble cont_normal[1][2], /* 2D contour normals */
             gleDouble up[3],           /* up vector for contour */
             gleDouble startRadius,	/* spiral starts in x-y plane */
             gleDouble drdTheta,        /* change in radius per revolution */
             gleDouble startZ,		/* starting z value */
             gleDouble dzdTheta,        /* change in Z per revolution */
             gleDouble startXform[2][3], /* starting contour affine xform */
             gleDouble dXformdTheta[2][3], /* tangent change xform per revoln */
             gleDouble startTheta,	/* start angle in x-y plane */
             gleDouble sweepTheta);	/* degrees to spiral around */


/* similar to spiral, except contour is a circle */
extern void gleHelicoid (gleDouble rToroid, /* circle contour (torus) radius */
             gleDouble startRadius,	/* spiral starts in x-y plane */
             gleDouble drdTheta,        /* change in radius per revolution */
             gleDouble startZ,		/* starting z value */
             gleDouble dzdTheta,        /* change in Z per revolution */
             gleDouble startXform[2][3], /* starting contour affine xform */
             gleDouble dXformdTheta[2][3], /* tangent change xform per revoln */
             gleDouble startTheta,	/* start angle in x-y plane */
             gleDouble sweepTheta);	/* degrees to spiral around */


/* similar to lathe, except contour is a circle */
extern void gleToroid (gleDouble rToroid, /* circle contour (torus) radius */
             gleDouble startRadius,	/* spiral starts in x-y plane */
             gleDouble drdTheta,        /* change in radius per revolution */
             gleDouble startZ,		/* starting z value */
             gleDouble dzdTheta,        /* change in Z per revolution */
             gleDouble startXform[2][3], /* starting contour affine xform */
             gleDouble dXformdTheta[2][3], /* tangent change xform per revoln */
             gleDouble startTheta,	/* start angle in x-y plane */
             gleDouble sweepTheta);	/* degrees to spiral around */


/* draws a screw shape */
extern void gleScrew (int ncp,          /* number of contour points */
             gleDouble contour[1][2],    /* 2D contour */
             gleDouble cont_normal[1][2], /* 2D contour normals */
             gleDouble up[3],           /* up vector for contour */
             gleDouble startz,          /* start of segment */
             gleDouble endz,            /* end of segment */
             gleDouble twist);          /* number of rotations */

extern void gleTextureMode (int mode);

#typmaps for Numeric arrays
%apply double VECTOR[3] { double axis[3]};
%apply double ARRAY2D[4][4] {double m[4][4]};

/* Rotation Utilities */
extern void rot_axis (gleDouble omega, gleDouble axis[3]);
extern void rot_about_axis (gleDouble angle, gleDouble axis[3]);
extern void rot_omega (gleDouble axis[3]);
extern void rot_prince (gleDouble omega, char axis);
extern void urot_axis (gleDouble m[4][4], gleDouble omega, gleDouble axis[3]);
extern void urot_about_axis (gleDouble m[4][4], gleDouble angle, gleDouble axis[3]);
extern void urot_omega (gleDouble m[4][4], gleDouble axis[3]);
extern void urot_prince (gleDouble m[4][4], gleDouble omega, char axis);

#typmaps for Numeric arrays
%apply double VECTOR[3]{double v21[3]};
%apply double VECTOR[3]{double up[3]};
%apply double VECTOR[3]{double v1[3]};
%apply double VECTOR[3]{double v2[3]};

/* viewpoint functions */
extern void uview_direction (gleDouble m[4][4],	/* returned */
                        gleDouble v21[3],	/* input */
                        gleDouble up[3]);	/* input */

extern void uviewpoint (gleDouble m[4][4],	/* returned */
                   gleDouble v1[3],		/* input */
                   gleDouble v2[3],		/* input */
                   gleDouble up[3]);		/* input */

#endif /* _NO_PROTO */

#endif /* __GLE_H__ */
/* ================== END OF FILE ======================= */

%{
#include <unistd.h>
#include <stdlib.h>
#if defined(_WIN32) && !defined(__clang__)
# include <malloc.h>
#endif

#include "port.h"
#include "tube_gc.h"

void _gleFeedback (  
   void (*bgn_feedback) (int, double),
   void (*n3d_feedback) (double *),
   void (*v3d_feedback) (double *, int, int),
   void (*end_feedback) ()
   )
{
      INIT_GC(); 

      _gle_gc -> bgn_gen_texture = bgn_feedback;
      _gle_gc -> n3d_gen_texture = n3d_feedback;
      _gle_gc -> v3d_gen_texture = v3d_feedback;
      _gle_gc -> end_gen_texture = end_feedback;

}

#include <stdio.h>
static int nbpts = 0;
static int max_nbpts = 0;
static int nbnorm = 0;
static int max_norm = 0;
static int nbstrip = 0;
static int max_strip = 0;
static double *vertices = NULL;
static double *normals = NULL;
static int *strips = NULL;

double *growDouble(double *array, int n, int *max, int dim)
{
  double *p;
  /* fixme ... catch memory allocation error, return error code */
  (*max) += n;
  p = (double *)realloc( array, (*max)*dim*sizeof(double));
/*  printf("allocate %d %p\n", (*max), p ); */
  return p;
}

int *growInt(int *array, int n, int *max, int dim)
{
  int *p;
  /* fixme ... catch memory allocation error, return error code */
  (*max) += n;
  p = (int *)realloc( array, (*max)*dim*sizeof(int));
/*  printf("allocate %d %p\n", (*max), p ); */
  return p;
}

static void add_Vertex(double v[3])
{
  if (nbpts >= max_nbpts) vertices = growDouble(vertices, 1000, &max_nbpts, 3);
  vertices[nbpts*3] = v[0];
  vertices[(nbpts*3)+1] = v[1];
  vertices[(nbpts*3)+2] = v[2];
  /*printf("V %d %d %p, %g %g %g\n", nbpts, max_nbpts, vertices, 
	 vertices[nbpts*3], vertices[(nbpts*3)+1], vertices[(nbpts*3)+2]);*/
  nbpts++;
}

static void add_Normal(double v[3])
{
  if (nbnorm >= max_norm) normals = growDouble(normals, 1000, &max_norm, 3);
  normals[nbnorm*3] = v[0];
  normals[(nbnorm*3)+1] = v[1];
  normals[(nbnorm*3)+2] = v[2];
/*  printf("N %d %d %p %g %g %g\n", nbnorm, max_norm, normals,
	 normals[nbnorm*3], normals[(nbnorm*3)+1], normals[(nbnorm*3)+2]); */
  nbnorm++;
}


#include <GL/gl.h>

static void add_Strip()
{
  /*
  printf("addStrip %d %d %d\n", nbstrip, nbpts, nbnorm);
  */
  if (nbstrip >= max_strip) strips = growInt(strips, 100, &max_strip, 2);
  strips[nbstrip*2] = nbpts;
  strips[(nbstrip*2)+1] = nbnorm;
  /*
  printf("addStrip %d %d %d\n", nbstrip, strips[nbstrip*2], strips[(nbstrip*2)+1]);
  */
  nbstrip++;
}

static double _mat_[16];

static void bgn_feedback (int inext, double len) {
  int i;
    glGetDoublev(GL_MODELVIEW_MATRIX, (double *)_mat_);
  /*
  printf("Begin mod ");
  for (i=0; i<16; i++)
    printf("%7.3f",_mat_[i]);
  printf("\n");*/
  
 
}

static void end_feedback () {
   add_Strip();
}

static void multmat(double v[3], double mat[16], double vm[3], int homo)
{
    int i;
    vm[0] = mat[0]*v[0] + mat[4]*v[1] + mat[8]*v[2] + homo*mat[12];
    vm[1] = mat[1]*v[0] + mat[5]*v[1] + mat[9]*v[2] + homo*mat[13];
    vm[2] = mat[2]*v[0] + mat[6]*v[1] + mat[10]*v[2] + homo*mat[14];
}

static void normal_feedback (double v[3]) {
   GLdouble vt[3];
   /*
   printf("n1 %lf %lf %lf\n", v[0], v[1], v[2]);
   */
   multmat(v, _mat_, &vt[0], 0);
   /*
   printf("n2 %lf %lf %lf\n", vt[0], vt[1], vt[2]);
   */
   /* printf("%lf %lf %lf\n", vt[0], vt[1], vt[2]); */
   add_Normal(vt);
}

static void vertex_feedback (double v[3], int jcnt, int end)
{
   GLdouble vt[3];

   /*
   printf("v %lf %lf %lf\n", v[0], v[1], v[2]);
   */
   multmat(v, _mat_, &vt[0], 1);
   /*
   printf("%7.3f %7.3f %7.3f %7.3f %7.3f %7.3f\n", v[0], v[1], v[2],
           vt[0], vt[1], vt[2]);
   */
   add_Vertex(vt);
}
%}

%inline %{
  void gleFeedBack() {
    nbpts = nbnorm = nbstrip = max_nbpts = max_norm = max_strip = 0;
    _gleFeedback(bgn_feedback, normal_feedback, vertex_feedback, end_feedback);
  }

 void gleFreeFeedbackBuffers() {
/*
   if (vertices) free(vertices);
   if (normals) free(normals);
   if (strips) free(strips);
*/
 }
%}

%{
static PyObject *gleGetTriangleMesh(PyObject *self, PyObject *args)
{
  PyArrayObject *v_array, *n_array, *s_array;
  int dims[2];

  dims[0] = nbpts;
  dims[1] = 3;
  v_array = (PyArrayObject *)PyArray_FromDimsAndData(2, dims, PyArray_DOUBLE,
						     (char *)vertices);
  v_array->flags |= OWN_DATA;

  dims[0] = nbnorm;
  dims[1] = 3;
  n_array = (PyArrayObject *)PyArray_FromDimsAndData(2, dims, PyArray_DOUBLE,
						     (char *)normals);
  n_array->flags |= OWN_DATA;

  dims[0] = nbstrip;
  dims[1] = 2;
  s_array = (PyArrayObject *)PyArray_FromDimsAndData(2, dims, PyArray_INT,
						     (char *)strips);
  s_array->flags |= OWN_DATA;

  return Py_BuildValue("OOO", v_array, n_array, s_array);
}
 %}

%native(gleGetTriangleMesh) gleGetTriangleMesh;
