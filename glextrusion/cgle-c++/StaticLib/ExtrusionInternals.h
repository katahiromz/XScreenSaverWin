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


//class CgleBaseExtrusion;

// some types
typedef double gleAffine[2][3];
typedef double gleVector[3];
typedef double gleContourVector[2];
typedef float gleColor[3];


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef float gleColor[3];
typedef double gleTwoVec[2];

#define __ROUND_TESS_PIECES 5


void CreateGC();
void DestroyGC();

//void AddItemToList(CgleBaseExtrusion* object);
//void RemoveItemFromList(CgleBaseExtrusion* object);
//void PurgeList();

void SetColor(float color[3]);

void save_normal (double *v);
void bgn_z_texgen (int inext, double len);
void vertex_flat_texgen_v (double *v, int jcnt, int which_end);
void normal_flat_texgen_v (double *v, int jcnt, int which_end);
void vertex_flat_model_v (double *v, int jcnt, int which_end);
void normal_flat_model_v (double *v, int jcnt, int which_end);
void vertex_cylinder_texgen_v (double *v, int jcnt, int which_end);
void normal_cylinder_texgen_v (double *v, int jcnt, int which_end);
void vertex_cylinder_model_v (double *v, int jcnt, int which_end);
void normal_cylinder_model_v (double *v, int jcnt, int which_end);
void bgn_sphere_texgen (int inext, double len);
void vertex_sphere_texgen_v (double *v, int jcnt, int which_end);
void normal_sphere_texgen_v (double *v, int jcnt, int which_end);
void vertex_sphere_model_v (double *v, int jcnt, int which_end);
void normal_sphere_model_v (double *v, int jcnt, int which_end);


typedef struct gleGC
{

   void (*bgn_gen_texture) (int, double);
   void (*n3f_gen_texture) (float *);
   void (*n3d_gen_texture) (double *);
   void (*v3f_gen_texture) (float *, int, int);
   void (*v3d_gen_texture) (double *, int, int);
   void (*end_gen_texture) (void);

   // "general knowledge" stuff

//    arguments passed into extrusion code
   int ncp;     // number of contour points
   gleContourVector *contour;    // 2D contour
   gleContourVector *cont_normal;  // 2D contour normals
   double *up;               // up vector
   int npoints;  // number of points in polyline
   gleVector *point_array;     // path
   gleColor *color_array;         // path colors
   gleAffine *xform_array;  // contour xforms

   //  used by texturing code
   int num_vert;
   int segment_number;
   double segment_length;
   double accum_seg_len;
   double prev_x;
   double prev_y;

} gleGC;


// These are used to convey info about topography to the
 // texture mapping routines

#define FRONT 		1
#define BACK 		2
#define FRONT_CAP 	3
#define BACK_CAP	4
#define FILLET		5





#define	T2F_D(x,y)	glTexCoord2d(x,y);



#define BGNTMESH(i,len)                                                \
{ 					                                                      \
	if(_gle_gc -> bgn_gen_texture)                                       \
     (*(_gle_gc -> bgn_gen_texture))(i,len);                           \
	glBegin (GL_TRIANGLE_STRIP); 			                                 \
}

#define BGNPOLYGON()                                                  \
{ 					                                                     \
	if(_gle_gc -> bgn_gen_texture)                                      \
     (*(_gle_gc -> bgn_gen_texture))();                               \
	glBegin (GL_POLYGON);                                               \
}

#define N3D(x)                                               \
{ 					                                              \
	if(_gle_gc -> n3d_gen_texture)                               \
     (*(_gle_gc -> n3d_gen_texture))(x);                       \
	glNormal3dv(x); 				                                  \
}

#define V3D(x,j,id)                                                       \
{ 					                                                         \
	if(_gle_gc -> v3d_gen_texture)                                          \
     (*(_gle_gc -> v3d_gen_texture))(x,j,id);                             \
	glVertex3dv(x); 			  	                                             \
}

#define ENDTMESH()                                                  \
{					                                                      \
	if(_gle_gc -> end_gen_texture)                                    \
     (*(_gle_gc -> end_gen_texture))();                             \
	glEnd ();					                                          \
}

#define ENDPOLYGON()                                                \
{					                                                      \
	if(_gle_gc -> end_gen_texture)                                    \
     (*(_gle_gc -> end_gen_texture))();                             \
	glEnd ();					                                          \
}

																													  











 
#ifdef __GUTIL_DOUBLE
#define gutDouble double
#else
#define gutDouble float  
#endif

// Rotation Utilities
extern void rot_axis_f (float omega, float axis[3]);
extern void rot_about_axis_f (float angle, float axis[3]);
extern void rot_omega_f (float axis[3]);
extern void urot_axis_f (float m[4][4], float omega, float axis[3]);
extern void urot_about_axis_f (float m[4][4], float angle, float axis[3]);
extern void urot_omega_f (float m[4][4], float axis[3]);

// double-precision versions
extern void rot_axis_d (double omega, double axis[3]);
extern void rot_about_axis_d (double angle, double axis[3]);
extern void rot_omega_d (double axis[3]);
extern void urot_axis_d (double m[4][4], double omega, double axis[3]);
extern void urot_about_axis_d (double m[4][4], double angle, double axis[3]);
extern void urot_omega_d (double m[4][4], double axis[3]);

// viewpoint functions
extern void uview_direction_d (double m[4][4],		// returned 
                        double v21[3],		// input
                        double up[3]);		// input

extern void uview_direction_f (float m[4][4],		// returned
                        float v21[3],		// input
                        float up[3]);		// input

extern void uviewpoint_d (double m[4][4],		// returned
                   double v1[3],		// input
                   double v2[3],		// input
                   double up[3]);		// input

extern void uviewpoint_f (float m[4][4],		// returned
                   float v1[3],		// input
                   float v2[3],		// input
                   float up[3]);		// input








// ====================================================== 
// Some compilers can't handle multiply-subscripted array's

#define AVAL(arr,n,i,j)  arr[n][i][j]
#define VVAL(arr,n,i)  arr[n][i];

// ======================================================

#define __GLE_DOUBLE

// ======================================================

#define urot_axis(a,b,c) 	urot_axis_d(a,b,c)
#define uview_direction(a,b,c) 	uview_direction_d(a,b,c)
#define uviewpoint(a,b,c,d) 	uviewpoint_d(a,b,c,d)

#if defined(WIN32)
   #pragma warning (disable:4244) // disable bogus conversion warnings
#endif




/*
 * rot.h
 *
 * FUNCTION:
 * rotation matrix utilities
 *
 * HISTORY:
 * Linas Vepstas Aug 1990
 */

/* ========================================================== */
/* 
 * The MACROS below generate and return more traditional rotation
 * matrices -- matrices for rotations about principal axes.
 */
/* ========================================================== */

#define ROTX_CS(m,cosine,sine)		\
{					\
   /* rotation about the x-axis */	\
					\
   m[0][0] = 1.0;			\
   m[0][1] = 0.0;			\
   m[0][2] = 0.0;			\
   m[0][3] = 0.0;			\
					\
   m[1][0] = 0.0;			\
   m[1][1] = (cosine);			\
   m[1][2] = (sine);			\
   m[1][3] = 0.0;			\
					\
   m[2][0] = 0.0;			\
   m[2][1] = -(sine);			\
   m[2][2] = (cosine);			\
   m[2][3] = 0.0;			\
					\
   m[3][0] = 0.0;			\
   m[3][1] = 0.0;			\
   m[3][2] = 0.0;			\
   m[3][3] = 1.0;			\
}				

/* ========================================================== */

#define ROTY_CS(m,cosine,sine)		\
{					\
   /* rotation about the y-axis */	\
					\
   m[0][0] = (cosine);			\
   m[0][1] = 0.0;			\
   m[0][2] = -(sine);			\
   m[0][3] = 0.0;			\
					\
   m[1][0] = 0.0;			\
   m[1][1] = 1.0;			\
   m[1][2] = 0.0;			\
   m[1][3] = 0.0;			\
					\
   m[2][0] = (sine);			\
   m[2][1] = 0.0;			\
   m[2][2] = (cosine);			\
   m[2][3] = 0.0;			\
					\
   m[3][0] = 0.0;			\
   m[3][1] = 0.0;			\
   m[3][2] = 0.0;			\
   m[3][3] = 1.0;			\
}

/* ========================================================== */

#define ROTZ_CS(m,cosine,sine)		\
{					\
   /* rotation about the z-axis */	\
					\
   m[0][0] = (cosine);			\
   m[0][1] = (sine);			\
   m[0][2] = 0.0;			\
   m[0][3] = 0.0;			\
					\
   m[1][0] = -(sine);			\
   m[1][1] = (cosine);			\
   m[1][2] = 0.0;			\
   m[1][3] = 0.0;			\
					\
   m[2][0] = 0.0;			\
   m[2][1] = 0.0;			\
   m[2][2] = 1.0;			\
   m[2][3] = 0.0;			\
					\
   m[3][0] = 0.0;			\
   m[3][1] = 0.0;			\
   m[3][2] = 0.0;			\
   m[3][3] = 1.0;			\
}

/* ========================================================== */

/*
 * vvector.h
 *
 * FUNCTION:
 * This file contains a number of utilities useful for handling
 * 3D vectors
 *
 * HISTORY:
 * Written by Linas Vepstas, August 1991
 * Added 2D code, March 1993
 * Added Outer products, C++ proofed, Linas Vepstas October 1993
 */

/* ========================================================== */
/* Zero out a 2D vector */

#define VEC_ZERO_2(a)				\
{						\
   (a)[0] = (a)[1] = 0.0;			\
}

/* ========================================================== */
/* Zero out a 3D vector */

#define VEC_ZERO(a)				\
{						\
   (a)[0] = (a)[1] = (a)[2] = 0.0;		\
}

/* ========================================================== */
/* Zero out a 4D vector */

#define VEC_ZERO_4(a)				\
{						\
   (a)[0] = (a)[1] = (a)[2] = (a)[3] = 0.0;	\
}

/* ========================================================== */
/* Vector copy */

#define VEC_COPY_2(b,a)				\
{						\
   (b)[0] = (a)[0];				\
   (b)[1] = (a)[1];				\
}

/* ========================================================== */
/* Copy 3D vector */

#define VEC_COPY(b,a)				\
{						\
   (b)[0] = (a)[0];				\
   (b)[1] = (a)[1];				\
   (b)[2] = (a)[2];				\
}

/* ========================================================== */
/* Copy 4D vector */

#define VEC_COPY_4(b,a)				\
{						\
   (b)[0] = (a)[0];				\
   (b)[1] = (a)[1];				\
   (b)[2] = (a)[2];				\
   (b)[3] = (a)[3];				\
}

/* ========================================================== */
/* Vector difference */

#define VEC_DIFF_2(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] - (v1)[0];		\
   (v21)[1] = (v2)[1] - (v1)[1];		\
}

/* ========================================================== */
/* Vector difference */

#define VEC_DIFF(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] - (v1)[0];		\
   (v21)[1] = (v2)[1] - (v1)[1];		\
   (v21)[2] = (v2)[2] - (v1)[2];		\
}

/* ========================================================== */
/* Vector difference */

#define VEC_DIFF_4(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] - (v1)[0];		\
   (v21)[1] = (v2)[1] - (v1)[1];		\
   (v21)[2] = (v2)[2] - (v1)[2];		\
   (v21)[3] = (v2)[3] - (v1)[3];		\
}

/* ========================================================== */
/* Vector sum */

#define VEC_SUM_2(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] + (v1)[0];		\
   (v21)[1] = (v2)[1] + (v1)[1];		\
}

/* ========================================================== */
/* Vector sum */

#define VEC_SUM(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] + (v1)[0];		\
   (v21)[1] = (v2)[1] + (v1)[1];		\
   (v21)[2] = (v2)[2] + (v1)[2];		\
}

/* ========================================================== */
/* Vector sum */

#define VEC_SUM_4(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] + (v1)[0];		\
   (v21)[1] = (v2)[1] + (v1)[1];		\
   (v21)[2] = (v2)[2] + (v1)[2];		\
   (v21)[3] = (v2)[3] + (v1)[3];		\
}

/* ========================================================== */
/* scalar times vector */

#define VEC_SCALE_2(c,a,b)			\
{						\
   (c)[0] = (a)*(b)[0];				\
   (c)[1] = (a)*(b)[1];				\
}

/* ========================================================== */
/* scalar times vector */

#define VEC_SCALE(c,a,b)			\
{						\
   (c)[0] = (a)*(b)[0];				\
   (c)[1] = (a)*(b)[1];				\
   (c)[2] = (a)*(b)[2];				\
}

/* ========================================================== */
/* scalar times vector */

#define VEC_SCALE_4(c,a,b)			\
{						\
   (c)[0] = (a)*(b)[0];				\
   (c)[1] = (a)*(b)[1];				\
   (c)[2] = (a)*(b)[2];				\
   (c)[3] = (a)*(b)[3];				\
}

/* ========================================================== */
/* accumulate scaled vector */

#define VEC_ACCUM_2(c,a,b)			\
{						\
   (c)[0] += (a)*(b)[0];			\
   (c)[1] += (a)*(b)[1];			\
}

/* ========================================================== */
/* accumulate scaled vector */

#define VEC_ACCUM(c,a,b)			\
{						\
   (c)[0] += (a)*(b)[0];			\
   (c)[1] += (a)*(b)[1];			\
   (c)[2] += (a)*(b)[2];			\
}

/* ========================================================== */
/* accumulate scaled vector */

#define VEC_ACCUM_4(c,a,b)			\
{						\
   (c)[0] += (a)*(b)[0];			\
   (c)[1] += (a)*(b)[1];			\
   (c)[2] += (a)*(b)[2];			\
   (c)[3] += (a)*(b)[3];			\
}

/* ========================================================== */
/* Vector dot product */

#define VEC_DOT_PRODUCT_2(c,a,b)			\
{							\
   c = (a)[0]*(b)[0] + (a)[1]*(b)[1];			\
}

/* ========================================================== */
/* Vector dot product */

#define VEC_DOT_PRODUCT(c,a,b)				\
{							\
   c = (a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2];	\
}

/* ========================================================== */
/* Vector dot product */

#define VEC_DOT_PRODUCT_4(c,a,b)			\
{							\
   c = (a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2] + (a)[3]*(b)[3] ;	\
}

/* ========================================================== */
/* vector impact parameter (squared) */

#define VEC_IMPACT_SQ(bsq,direction,position)		\
{							\
   double len, llel;					\
   VEC_DOT_PRODUCT (len, position, position);		\
   VEC_DOT_PRODUCT (llel, direction, position);		\
   bsq = len - llel*llel;				\
}

/* ========================================================== */
/* vector impact parameter */

#define VEC_IMPACT(bsq,direction,position)		\
{							\
   VEC_IMPACT_SQ(bsq,direction,position);		\
   bsq = sqrt (bsq);					\
}

/* ========================================================== */
/* Vector length */

#define VEC_LENGTH_2(len,a)			\
{						\
   len = a[0]*a[0] + a[1]*a[1];			\
   len = sqrt (len);				\
}

/* ========================================================== */
/* Vector length */

#define VEC_LENGTH(len,a)			\
{						\
   len = (a)[0]*(a)[0] + (a)[1]*(a)[1];		\
   len += (a)[2]*(a)[2];			\
   len = sqrt (len);				\
}

/* ========================================================== */
/* Vector length */

#define VEC_LENGTH_4(len,a)			\
{						\
   len = (a)[0]*(a)[0] + (a)[1]*(a)[1];		\
   len += (a)[2]*(a)[2];			\
   len += (a)[3] * (a)[3];			\
   len = sqrt (len);				\
}

/* ========================================================== */
/* distance between two points */

#define VEC_DISTANCE(len,va,vb)			\
{						\
    double tmp[4];				\
    VEC_DIFF (tmp, vb, va);			\
    VEC_LENGTH (len, tmp);			\
}

/* ========================================================== */
/* Vector length */

#define VEC_CONJUGATE_LENGTH(len,a)		\
{						\
   len = 1.0 - a[0]*a[0] - a[1]*a[1] - a[2]*a[2];\
   len = sqrt (len);				\
}

/* ========================================================== */
/* Vector length */

#define VEC_NORMALIZE(a)			\
{						\
   double nlen;					\
   VEC_LENGTH (nlen,a);				\
   if (nlen != 0.0)   \
   {				\
      nlen = 1.0 / nlen;				\
      a[0] *= nlen;				\
      a[1] *= nlen;				\
      a[2] *= nlen;				\
   }						\
}

/* ========================================================== */
/* Vector length */

#define VEC_RENORMALIZE(a,newlen)		\
{						\
   double len;					\
   VEC_LENGTH (len,a);				\
   if (len != 0.0) {				\
      len = newlen / len;				\
      a[0] *= len;				\
      a[1] *= len;				\
      a[2] *= len;				\
   }						\
}

/* ========================================================== */
/* 3D Vector cross product yeilding vector */

#define VEC_CROSS_PRODUCT(c,a,b)		\
{						\
   c[0] = (a)[1] * (b)[2] - (a)[2] * (b)[1];	\
   c[1] = (a)[2] * (b)[0] - (a)[0] * (b)[2];	\
   c[2] = (a)[0] * (b)[1] - (a)[1] * (b)[0];	\
}

/* ========================================================== */
/* Vector perp -- assumes that n is of unit length 
 * accepts vector v, subtracts out any component parallel to n */

#define VEC_PERP(vp,v,n)			\
{						\
   double dot;					\
						\
   VEC_DOT_PRODUCT (dot, v, n);			\
   vp[0] = (v)[0] - (dot) * (n)[0];		\
   vp[1] = (v)[1] - (dot) * (n)[1];		\
   vp[2] = (v)[2] - (dot) * (n)[2];		\
}

/* ========================================================== */
/* Vector parallel -- assumes that n is of unit length 
 * accepts vector v, subtracts out any component perpendicular to n */

#define VEC_PARALLEL(vp,v,n)			\
{						\
   double dot;					\
						\
   VEC_DOT_PRODUCT (dot, v, n);			\
   vp[0] = (dot) * (n)[0];			\
   vp[1] = (dot) * (n)[1];			\
   vp[2] = (dot) * (n)[2];			\
}

/* ========================================================== */
/* Vector reflection -- assumes n is of unit length */
/* Takes vector v, reflects it against reflector n, and returns vr */

#define VEC_REFLECT(vr,v,n)			\
{						\
   double vdot;					\
						\
   VEC_DOT_PRODUCT (vdot, v, n);			\
   vr[0] = (v)[0] - 2.0 * (vdot) * (n)[0];	\
   vr[1] = (v)[1] - 2.0 * (vdot) * (n)[1];	\
   vr[2] = (v)[2] - 2.0 * (vdot) * (n)[2];	\
}

/* ========================================================== */
/* Vector blending */
/* Takes two vectors a, b, blends them together */ 

#define VEC_BLEND(vr,sa,a,sb,b)			\
{						\
						\
   vr[0] = (sa) * (a)[0] + (sb) * (b)[0];	\
   vr[1] = (sa) * (a)[1] + (sb) * (b)[1];	\
   vr[2] = (sa) * (a)[2] + (sb) * (b)[2];	\
}

/* ========================================================== */
/* Vector print */

#define VEC_PRINT_2(a)					\
{							\
   double len;						\
   VEC_LENGTH_2 (len, a);					\
   printf (" a is %f %f length of a is %f \n", a[0], a[1], len); \
}

/* ========================================================== */
/* Vector print */

#define VEC_PRINT(a)					\
{							\
   double len;						\
   VEC_LENGTH (len, (a));				\
   printf (" a is %f %f %f length of a is %f \n", (a)[0], (a)[1], (a)[2], len); \
}

/* ========================================================== */
/* Vector print */

#define VEC_PRINT_4(a)					\
{							\
   double len;						\
   VEC_LENGTH_4 (len, (a));				\
   printf (" a is %f %f %f %f length of a is %f \n",	\
       (a)[0], (a)[1], (a)[2], (a)[3], len);		\
}

/* ========================================================== */
/* print matrix */

#define MAT_PRINT_4X4(mmm) {				\
   int i,j;						\
   printf ("matrix mmm is \n");				\
   if (mmm == NULL) {					\
      printf (" Null \n");				\
   } else {						\
      for (i=0; i<4; i++) {				\
         for (j=0; j<4; j++) {				\
            printf ("%f ", mmm[i][j]);			\
         }						\
         printf (" \n");				\
      }							\
   }							\
}

/* ========================================================== */
/* print matrix */

#define MAT_PRINT_3X3(mmm) {				\
   int i,j;						\
   printf ("matrix mmm is \n");				\
   if (mmm == NULL) {					\
      printf (" Null \n");				\
   } else {						\
      for (i=0; i<3; i++) {				\
         for (j=0; j<3; j++) {				\
            printf ("%f ", mmm[i][j]);			\
         }						\
         printf (" \n");				\
      }							\
   }							\
}

/* ========================================================== */
/* print matrix */

#define MAT_PRINT_2X3(mmm) {				\
   int i,j;						\
   printf ("matrix mmm is \n");				\
   if (mmm == NULL) {					\
      printf (" Null \n");				\
   } else {						\
      for (i=0; i<2; i++) {				\
         for (j=0; j<3; j++) {				\
            printf ("%f ", mmm[i][j]);			\
         }						\
         printf (" \n");				\
      }							\
   }							\
}

/* ========================================================== */
/* initialize matrix */

#define IDENTIFY_MATRIX_3X3(m)			\
{						\
   m[0][0] = 1.0;				\
   m[0][1] = 0.0;				\
   m[0][2] = 0.0;				\
						\
   m[1][0] = 0.0;				\
   m[1][1] = 1.0;				\
   m[1][2] = 0.0;				\
						\
   m[2][0] = 0.0;				\
   m[2][1] = 0.0;				\
   m[2][2] = 1.0;				\
}

/* ========================================================== */
/* initialize matrix */

#define IDENTIFY_MATRIX_4X4(m)			\
{						\
   m[0][0] = 1.0;				\
   m[0][1] = 0.0;				\
   m[0][2] = 0.0;				\
   m[0][3] = 0.0;				\
						\
   m[1][0] = 0.0;				\
   m[1][1] = 1.0;				\
   m[1][2] = 0.0;				\
   m[1][3] = 0.0;				\
						\
   m[2][0] = 0.0;				\
   m[2][1] = 0.0;				\
   m[2][2] = 1.0;				\
   m[2][3] = 0.0;				\
						\
   m[3][0] = 0.0;				\
   m[3][1] = 0.0;				\
   m[3][2] = 0.0;				\
   m[3][3] = 1.0;				\
}

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_2X2(b,a)	\
{				\
   b[0][0] = a[0][0];		\
   b[0][1] = a[0][1];		\
				\
   b[1][0] = a[1][0];		\
   b[1][1] = a[1][1];		\
				\
}

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_2X3(b,a)	\
{				\
   b[0][0] = a[0][0];		\
   b[0][1] = a[0][1];		\
   b[0][2] = a[0][2];		\
				\
   b[1][0] = a[1][0];		\
   b[1][1] = a[1][1];		\
   b[1][2] = a[1][2];		\
}

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_3X3(b,a)	\
{				\
   b[0][0] = a[0][0];		\
   b[0][1] = a[0][1];		\
   b[0][2] = a[0][2];		\
				\
   b[1][0] = a[1][0];		\
   b[1][1] = a[1][1];		\
   b[1][2] = a[1][2];		\
				\
   b[2][0] = a[2][0];		\
   b[2][1] = a[2][1];		\
   b[2][2] = a[2][2];		\
}

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_4X4(b,a)	\
{				\
   b[0][0] = a[0][0];		\
   b[0][1] = a[0][1];		\
   b[0][2] = a[0][2];		\
   b[0][3] = a[0][3];		\
				\
   b[1][0] = a[1][0];		\
   b[1][1] = a[1][1];		\
   b[1][2] = a[1][2];		\
   b[1][3] = a[1][3];		\
				\
   b[2][0] = a[2][0];		\
   b[2][1] = a[2][1];		\
   b[2][2] = a[2][2];		\
   b[2][3] = a[2][3];		\
				\
   b[3][0] = a[3][0];		\
   b[3][1] = a[3][1];		\
   b[3][2] = a[3][2];		\
   b[3][3] = a[3][3];		\
}

/* ========================================================== */
/* matrix transpose */

#define TRANSPOSE_MATRIX_2X2(b,a)	\
{				\
   b[0][0] = a[0][0];		\
   b[0][1] = a[1][0];		\
				\
   b[1][0] = a[0][1];		\
   b[1][1] = a[1][1];		\
}

/* ========================================================== */
/* matrix transpose */

#define TRANSPOSE_MATRIX_3X3(b,a)	\
{				\
   b[0][0] = a[0][0];		\
   b[0][1] = a[1][0];		\
   b[0][2] = a[2][0];		\
				\
   b[1][0] = a[0][1];		\
   b[1][1] = a[1][1];		\
   b[1][2] = a[2][1];		\
				\
   b[2][0] = a[0][2];		\
   b[2][1] = a[1][2];		\
   b[2][2] = a[2][2];		\
}

/* ========================================================== */
/* matrix transpose */

#define TRANSPOSE_MATRIX_4X4(b,a)	\
{				\
   b[0][0] = a[0][0];		\
   b[0][1] = a[1][0];		\
   b[0][2] = a[2][0];		\
   b[0][3] = a[3][0];		\
				\
   b[1][0] = a[0][1];		\
   b[1][1] = a[1][1];		\
   b[1][2] = a[2][1];		\
   b[1][3] = a[3][1];		\
				\
   b[2][0] = a[0][2];		\
   b[2][1] = a[1][2];		\
   b[2][2] = a[2][2];		\
   b[2][3] = a[3][2];		\
				\
   b[3][0] = a[0][3];		\
   b[3][1] = a[1][3];		\
   b[3][2] = a[2][3];		\
   b[3][3] = a[3][3];		\
}

/* ========================================================== */
/* multiply matrix by scalar */

#define SCALE_MATRIX_2X2(b,s,a)		\
{					\
   b[0][0] = (s) * a[0][0];		\
   b[0][1] = (s) * a[0][1];		\
					\
   b[1][0] = (s) * a[1][0];		\
   b[1][1] = (s) * a[1][1];		\
}

/* ========================================================== */
/* multiply matrix by scalar */

#define SCALE_MATRIX_3X3(b,s,a)		\
{					\
   b[0][0] = (s) * a[0][0];		\
   b[0][1] = (s) * a[0][1];		\
   b[0][2] = (s) * a[0][2];		\
					\
   b[1][0] = (s) * a[1][0];		\
   b[1][1] = (s) * a[1][1];		\
   b[1][2] = (s) * a[1][2];		\
					\
   b[2][0] = (s) * a[2][0];		\
   b[2][1] = (s) * a[2][1];		\
   b[2][2] = (s) * a[2][2];		\
}

/* ========================================================== */
/* multiply matrix by scalar */

#define SCALE_MATRIX_4X4(b,s,a)		\
{					\
   b[0][0] = (s) * a[0][0];		\
   b[0][1] = (s) * a[0][1];		\
   b[0][2] = (s) * a[0][2];		\
   b[0][3] = (s) * a[0][3];		\
					\
   b[1][0] = (s) * a[1][0];		\
   b[1][1] = (s) * a[1][1];		\
   b[1][2] = (s) * a[1][2];		\
   b[1][3] = (s) * a[1][3];		\
					\
   b[2][0] = (s) * a[2][0];		\
   b[2][1] = (s) * a[2][1];		\
   b[2][2] = (s) * a[2][2];		\
   b[2][3] = (s) * a[2][3];		\
					\
   b[3][0] = s * a[3][0];		\
   b[3][1] = s * a[3][1];		\
   b[3][2] = s * a[3][2];		\
   b[3][3] = s * a[3][3];		\
}

/* ========================================================== */
/* multiply matrix by scalar */

#define ACCUM_SCALE_MATRIX_2X2(b,s,a)		\
{					\
   b[0][0] += (s) * a[0][0];		\
   b[0][1] += (s) * a[0][1];		\
					\
   b[1][0] += (s) * a[1][0];		\
   b[1][1] += (s) * a[1][1];		\
}

/* +========================================================== */
/* multiply matrix by scalar */

#define ACCUM_SCALE_MATRIX_3X3(b,s,a)		\
{					\
   b[0][0] += (s) * a[0][0];		\
   b[0][1] += (s) * a[0][1];		\
   b[0][2] += (s) * a[0][2];		\
					\
   b[1][0] += (s) * a[1][0];		\
   b[1][1] += (s) * a[1][1];		\
   b[1][2] += (s) * a[1][2];		\
					\
   b[2][0] += (s) * a[2][0];		\
   b[2][1] += (s) * a[2][1];		\
   b[2][2] += (s) * a[2][2];		\
}

/* +========================================================== */
/* multiply matrix by scalar */

#define ACCUM_SCALE_MATRIX_4X4(b,s,a)		\
{					\
   b[0][0] += (s) * a[0][0];		\
   b[0][1] += (s) * a[0][1];		\
   b[0][2] += (s) * a[0][2];		\
   b[0][3] += (s) * a[0][3];		\
					\
   b[1][0] += (s) * a[1][0];		\
   b[1][1] += (s) * a[1][1];		\
   b[1][2] += (s) * a[1][2];		\
   b[1][3] += (s) * a[1][3];		\
					\
   b[2][0] += (s) * a[2][0];		\
   b[2][1] += (s) * a[2][1];		\
   b[2][2] += (s) * a[2][2];		\
   b[2][3] += (s) * a[2][3];		\
					\
   b[3][0] += (s) * a[3][0];		\
   b[3][1] += (s) * a[3][1];		\
   b[3][2] += (s) * a[3][2];		\
   b[3][3] += (s) * a[3][3];		\
}

/* +========================================================== */
/* matrix product */
/* c[x][y] = a[x][0]*b[0][y]+a[x][1]*b[1][y]+a[x][2]*b[2][y]+a[x][3]*b[3][y];*/

#define MATRIX_PRODUCT_2X2(c,a,b)		\
{						\
   c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[1][0];	\
   c[0][1] = a[0][0]*b[0][1]+a[0][1]*b[1][1];	\
						\
   c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[1][0];	\
   c[1][1] = a[1][0]*b[0][1]+a[1][1]*b[1][1];	\
						\
}

/* ========================================================== */
/* matrix product */
/* c[x][y] = a[x][0]*b[0][y]+a[x][1]*b[1][y]+a[x][2]*b[2][y]+a[x][3]*b[3][y];*/

#define MATRIX_PRODUCT_3X3(c,a,b)				\
{								\
   c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[1][0]+a[0][2]*b[2][0];	\
   c[0][1] = a[0][0]*b[0][1]+a[0][1]*b[1][1]+a[0][2]*b[2][1];	\
   c[0][2] = a[0][0]*b[0][2]+a[0][1]*b[1][2]+a[0][2]*b[2][2];	\
								\
   c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[1][0]+a[1][2]*b[2][0];	\
   c[1][1] = a[1][0]*b[0][1]+a[1][1]*b[1][1]+a[1][2]*b[2][1];	\
   c[1][2] = a[1][0]*b[0][2]+a[1][1]*b[1][2]+a[1][2]*b[2][2];	\
								\
   c[2][0] = a[2][0]*b[0][0]+a[2][1]*b[1][0]+a[2][2]*b[2][0];	\
   c[2][1] = a[2][0]*b[0][1]+a[2][1]*b[1][1]+a[2][2]*b[2][1];	\
   c[2][2] = a[2][0]*b[0][2]+a[2][1]*b[1][2]+a[2][2]*b[2][2];	\
}

/* ========================================================== */
/* matrix product */
/* c[x][y] = a[x][0]*b[0][y]+a[x][1]*b[1][y]+a[x][2]*b[2][y]+a[x][3]*b[3][y];*/

#define MATRIX_PRODUCT_4X4(c,a,b)		\
{						\
   c[0][0] = a[0][0]*b[0][0]+a[0][1]*b[1][0]+a[0][2]*b[2][0]+a[0][3]*b[3][0];\
   c[0][1] = a[0][0]*b[0][1]+a[0][1]*b[1][1]+a[0][2]*b[2][1]+a[0][3]*b[3][1];\
   c[0][2] = a[0][0]*b[0][2]+a[0][1]*b[1][2]+a[0][2]*b[2][2]+a[0][3]*b[3][2];\
   c[0][3] = a[0][0]*b[0][3]+a[0][1]*b[1][3]+a[0][2]*b[2][3]+a[0][3]*b[3][3];\
						\
   c[1][0] = a[1][0]*b[0][0]+a[1][1]*b[1][0]+a[1][2]*b[2][0]+a[1][3]*b[3][0];\
   c[1][1] = a[1][0]*b[0][1]+a[1][1]*b[1][1]+a[1][2]*b[2][1]+a[1][3]*b[3][1];\
   c[1][2] = a[1][0]*b[0][2]+a[1][1]*b[1][2]+a[1][2]*b[2][2]+a[1][3]*b[3][2];\
   c[1][3] = a[1][0]*b[0][3]+a[1][1]*b[1][3]+a[1][2]*b[2][3]+a[1][3]*b[3][3];\
						\
   c[2][0] = a[2][0]*b[0][0]+a[2][1]*b[1][0]+a[2][2]*b[2][0]+a[2][3]*b[3][0];\
   c[2][1] = a[2][0]*b[0][1]+a[2][1]*b[1][1]+a[2][2]*b[2][1]+a[2][3]*b[3][1];\
   c[2][2] = a[2][0]*b[0][2]+a[2][1]*b[1][2]+a[2][2]*b[2][2]+a[2][3]*b[3][2];\
   c[2][3] = a[2][0]*b[0][3]+a[2][1]*b[1][3]+a[2][2]*b[2][3]+a[2][3]*b[3][3];\
						\
   c[3][0] = a[3][0]*b[0][0]+a[3][1]*b[1][0]+a[3][2]*b[2][0]+a[3][3]*b[3][0];\
   c[3][1] = a[3][0]*b[0][1]+a[3][1]*b[1][1]+a[3][2]*b[2][1]+a[3][3]*b[3][1];\
   c[3][2] = a[3][0]*b[0][2]+a[3][1]*b[1][2]+a[3][2]*b[2][2]+a[3][3]*b[3][2];\
   c[3][3] = a[3][0]*b[0][3]+a[3][1]*b[1][3]+a[3][2]*b[2][3]+a[3][3]*b[3][3];\
}

/* ========================================================== */
/* matrix times vector */

#define MAT_DOT_VEC_2X2(p,m,v)					\
{								\
   p[0] = m[0][0]*v[0] + m[0][1]*v[1];				\
   p[1] = m[1][0]*v[0] + m[1][1]*v[1];				\
}

/* ========================================================== */
/* matrix times vector */

#define MAT_DOT_VEC_3X3(p,m,v)					\
{								\
   p[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2];		\
   p[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2];		\
   p[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2];		\
}

/* ========================================================== */
/* matrix times vector */

#define MAT_DOT_VEC_4X4(p,m,v)					\
{								\
   p[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2]*v[2] + m[0][3]*v[3];	\
   p[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2]*v[2] + m[1][3]*v[3];	\
   p[2] = m[2][0]*v[0] + m[2][1]*v[1] + m[2][2]*v[2] + m[2][3]*v[3];	\
   p[3] = m[3][0]*v[0] + m[3][1]*v[1] + m[3][2]*v[2] + m[3][3]*v[3];	\
}

/* ========================================================== */
/* vector transpose times matrix */
/* p[j] = v[0]*m[0][j] + v[1]*m[1][j] + v[2]*m[2][j]; */

#define VEC_DOT_MAT_3X3(p,v,m)					\
{								\
   p[0] = v[0]*m[0][0] + v[1]*m[1][0] + v[2]*m[2][0];		\
   p[1] = v[0]*m[0][1] + v[1]*m[1][1] + v[2]*m[2][1];		\
   p[2] = v[0]*m[0][2] + v[1]*m[1][2] + v[2]*m[2][2];		\
}

/* ========================================================== */
/* affine matrix times vector */
/* The matrix is assumed to be an affine matrix, with last two 
 * entries representing a translation */

#define MAT_DOT_VEC_2X3(p,m,v)					\
{								\
   p[0] = m[0][0]*v[0] + m[0][1]*v[1] + m[0][2];		\
   p[1] = m[1][0]*v[0] + m[1][1]*v[1] + m[1][2];		\
}

/* ========================================================== */
/* inverse transpose of matrix times vector
 *
 * This macro computes inverse transpose of matrix m, 
 * and multiplies vector v into it, to yeild vector p
 *
 * DANGER !!! Do Not use this on normal vectors!!!
 * It will leave normals the wrong length !!!
 * See macro below for use on normals.
 */

#define INV_TRANSP_MAT_DOT_VEC_2X2(p,m,v)			\
{								\
   double det;						\
								\
   det = m[0][0]*m[1][1] - m[0][1]*m[1][0];			\
   p[0] = m[1][1]*v[0] - m[1][0]*v[1];				\
   p[1] = - m[0][1]*v[0] + m[0][0]*v[1];			\
								\
   /* if matrix not singular, and not orthonormal, then renormalize */ \
   if ((det!=1.0) && (det != 0.0)) {				\
      det = 1.0 / det;						\
      p[0] *= det;						\
      p[1] *= det;						\
   }								\
}

/* ========================================================== */
/* transform normal vector by inverse transpose of matrix 
 * and then renormalize the vector 
 *
 * This macro computes inverse transpose of matrix m, 
 * and multiplies vector v into it, to yeild vector p
 * Vector p is then normalized.
 */

   // do nothing if off-diagonals are zero and diagonals are    equal 

#define NORM_XFORM_2X2(p,m,v)					\
{								\
   double xlen;							\
								\
   if ((m[0][1] != 0.0) || (m[1][0] != 0.0) || (m[0][0] != m[1][1])) { \
      p[0] = m[1][1]*v[0] - m[1][0]*v[1];			\
      p[1] = - m[0][1]*v[0] + m[0][0]*v[1];			\
								\
      xlen = p[0]*p[0] + p[1]*p[1];				\
      xlen = 1.0 / sqrt (xlen);					\
      p[0] *= xlen;						\
      p[1] *= xlen;						\
   } else {							\
      VEC_COPY_2 (p, v);					\
   }								\
}

/* ========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define OUTER_PRODUCT_2X2(m,v,t)				\
{								\
   m[0][0] = v[0] * t[0];					\
   m[0][1] = v[0] * t[1];					\
								\
   m[1][0] = v[1] * t[0];					\
   m[1][1] = v[1] * t[1];					\
}

/* ========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define OUTER_PRODUCT_3X3(m,v,t)				\
{								\
   m[0][0] = v[0] * t[0];					\
   m[0][1] = v[0] * t[1];					\
   m[0][2] = v[0] * t[2];					\
								\
   m[1][0] = v[1] * t[0];					\
   m[1][1] = v[1] * t[1];					\
   m[1][2] = v[1] * t[2];					\
								\
   m[2][0] = v[2] * t[0];					\
   m[2][1] = v[2] * t[1];					\
   m[2][2] = v[2] * t[2];					\
}

/* ========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define OUTER_PRODUCT_4X4(m,v,t)				\
{								\
   m[0][0] = v[0] * t[0];					\
   m[0][1] = v[0] * t[1];					\
   m[0][2] = v[0] * t[2];					\
   m[0][3] = v[0] * t[3];					\
								\
   m[1][0] = v[1] * t[0];					\
   m[1][1] = v[1] * t[1];					\
   m[1][2] = v[1] * t[2];					\
   m[1][3] = v[1] * t[3];					\
								\
   m[2][0] = v[2] * t[0];					\
   m[2][1] = v[2] * t[1];					\
   m[2][2] = v[2] * t[2];					\
   m[2][3] = v[2] * t[3];					\
								\
   m[3][0] = v[3] * t[0];					\
   m[3][1] = v[3] * t[1];					\
   m[3][2] = v[3] * t[2];					\
   m[3][3] = v[3] * t[3];					\
}

/* +========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define ACCUM_OUTER_PRODUCT_2X2(m,v,t)				\
{								\
   m[0][0] += v[0] * t[0];					\
   m[0][1] += v[0] * t[1];					\
								\
   m[1][0] += v[1] * t[0];					\
   m[1][1] += v[1] * t[1];					\
}

/* +========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define ACCUM_OUTER_PRODUCT_3X3(m,v,t)				\
{								\
   m[0][0] += v[0] * t[0];					\
   m[0][1] += v[0] * t[1];					\
   m[0][2] += v[0] * t[2];					\
								\
   m[1][0] += v[1] * t[0];					\
   m[1][1] += v[1] * t[1];					\
   m[1][2] += v[1] * t[2];					\
								\
   m[2][0] += v[2] * t[0];					\
   m[2][1] += v[2] * t[1];					\
   m[2][2] += v[2] * t[2];					\
}

/* +========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define ACCUM_OUTER_PRODUCT_4X4(m,v,t)				\
{								\
   m[0][0] += v[0] * t[0];					\
   m[0][1] += v[0] * t[1];					\
   m[0][2] += v[0] * t[2];					\
   m[0][3] += v[0] * t[3];					\
								\
   m[1][0] += v[1] * t[0];					\
   m[1][1] += v[1] * t[1];					\
   m[1][2] += v[1] * t[2];					\
   m[1][3] += v[1] * t[3];					\
								\
   m[2][0] += v[2] * t[0];					\
   m[2][1] += v[2] * t[1];					\
   m[2][2] += v[2] * t[2];					\
   m[2][3] += v[2] * t[3];					\
								\
   m[3][0] += v[3] * t[0];					\
   m[3][1] += v[3] * t[1];					\
   m[3][2] += v[3] * t[2];					\
   m[3][3] += v[3] * t[3];					\
}

/* +========================================================== */
/* determinant of matrix
 *
 * Computes determinant of matrix m, returning d
 */

#define DETERMINANT_2X2(d,m)					\
{								\
   d = m[0][0] * m[1][1] - m[0][1] * m[1][0];			\
}

/* ========================================================== */
/* determinant of matrix
 *
 * Computes determinant of matrix m, returning d
 */

#define DETERMINANT_3X3(d,m)					\
{								\
   d = m[0][0] * (m[1][1]*m[2][2] - m[1][2] * m[2][1]);		\
   d -= m[0][1] * (m[1][0]*m[2][2] - m[1][2] * m[2][0]);	\
   d += m[0][2] * (m[1][0]*m[2][1] - m[1][1] * m[2][0]);	\
}

/* ========================================================== */
/* i,j,th cofactor of a 4x4 matrix
 *
 */

#define COFACTOR_4X4_IJ(fac,m,i,j) 				\
{								\
   int ii[4], jj[4], k;						\
								\
   /* compute which row, columnt to skip */			\
   for (k=0; k<i; k++) ii[k] = k;				\
   for (k=i; k<3; k++) ii[k] = k+1;				\
   for (k=0; k<j; k++) jj[k] = k;				\
   for (k=j; k<3; k++) jj[k] = k+1;				\
								\
   (fac) = m[ii[0]][jj[0]] * (m[ii[1]][jj[1]]*m[ii[2]][jj[2]] 	\
                            - m[ii[1]][jj[2]]*m[ii[2]][jj[1]]); \
   (fac) -= m[ii[0]][jj[1]] * (m[ii[1]][jj[0]]*m[ii[2]][jj[2]]	\
                             - m[ii[1]][jj[2]]*m[ii[2]][jj[0]]);\
   (fac) += m[ii[0]][jj[2]] * (m[ii[1]][jj[0]]*m[ii[2]][jj[1]]	\
                             - m[ii[1]][jj[1]]*m[ii[2]][jj[0]]);\
								\
   /* compute sign */						\
   k = i+j;							\
   if ( k != (k/2)*2) {						\
      (fac) = -(fac);						\
   }								\
}

/* ========================================================== */
/* determinant of matrix
 *
 * Computes determinant of matrix m, returning d
 */

#define DETERMINANT_4X4(d,m)					\
{								\
   double cofac;						\
   COFACTOR_4X4_IJ (cofac, m, 0, 0);				\
   d = m[0][0] * cofac;						\
   COFACTOR_4X4_IJ (cofac, m, 0, 1);				\
   d += m[0][1] * cofac;					\
   COFACTOR_4X4_IJ (cofac, m, 0, 2);				\
   d += m[0][2] * cofac;					\
   COFACTOR_4X4_IJ (cofac, m, 0, 3);				\
   d += m[0][3] * cofac;					\
}

/* ========================================================== */
/* cofactor of matrix
 *
 * Computes cofactor of matrix m, returning a
 */

#define COFACTOR_2X2(a,m)					\
{								\
   a[0][0] = (m)[1][1];						\
   a[0][1] = - (m)[1][0];						\
   a[1][0] = - (m)[0][1];						\
   a[1][1] = (m)[0][0];						\
}

/* ========================================================== */
/* cofactor of matrix
 *
 * Computes cofactor of matrix m, returning a
 */

#define COFACTOR_3X3(a,m)					\
{								\
   a[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1];			\
   a[0][1] = - (m[1][0]*m[2][2] - m[2][0]*m[1][2]);		\
   a[0][2] = m[1][0]*m[2][1] - m[1][1]*m[2][0];			\
   a[1][0] = - (m[0][1]*m[2][2] - m[0][2]*m[2][1]);		\
   a[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0];			\
   a[1][2] = - (m[0][0]*m[2][1] - m[0][1]*m[2][0]);		\
   a[2][0] = m[0][1]*m[1][2] - m[0][2]*m[1][1];			\
   a[2][1] = - (m[0][0]*m[1][2] - m[0][2]*m[1][0]);		\
   a[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0]);		\
}

/* ========================================================== */
/* cofactor of matrix
 *
 * Computes cofactor of matrix m, returning a
 */

#define COFACTOR_4X4(a,m)					\
{								\
   int i,j;							\
								\
   for (i=0; i<4; i++) {					\
      for (j=0; j<4; j++) {					\
         COFACTOR_4X4_IJ (a[i][j], m, i, j);			\
      }								\
   }								\
}

/* ========================================================== */
/* adjoint of matrix
 *
 * Computes adjoint of matrix m, returning a
 * (Note that adjoint is just the transpose of the cofactor matrix)
 */

#define ADJOINT_2X2(a,m)					\
{								\
   a[0][0] = (m)[1][1];						\
   a[1][0] = - (m)[1][0];						\
   a[0][1] = - (m)[0][1];						\
   a[1][1] = (m)[0][0];						\
}

/* ========================================================== */
/* adjoint of matrix
 *
 * Computes adjoint of matrix m, returning a
 * (Note that adjoint is just the transpose of the cofactor matrix)
 */


#define ADJOINT_3X3(a,m)					\
{								\
   a[0][0] = m[1][1]*m[2][2] - m[1][2]*m[2][1];			\
   a[1][0] = - (m[1][0]*m[2][2] - m[2][0]*m[1][2]);		\
   a[2][0] = m[1][0]*m[2][1] - m[1][1]*m[2][0];			\
   a[0][1] = - (m[0][1]*m[2][2] - m[0][2]*m[2][1]);		\
   a[1][1] = m[0][0]*m[2][2] - m[0][2]*m[2][0];			\
   a[2][1] = - (m[0][0]*m[2][1] - m[0][1]*m[2][0]);		\
   a[0][2] = m[0][1]*m[1][2] - m[0][2]*m[1][1];			\
   a[1][2] = - (m[0][0]*m[1][2] - m[0][2]*m[1][0]);		\
   a[2][2] = m[0][0]*m[1][1] - m[0][1]*m[1][0]);		\
}

/* ========================================================== */
/* adjoint of matrix
 *
 * Computes adjoint of matrix m, returning a
 * (Note that adjoint is just the transpose of the cofactor matrix)
 */

#define ADJOINT_4X4(a,m)					\
{								\
   int i,j;							\
								\
   for (i=0; i<4; i++) {					\
      for (j=0; j<4; j++) {					\
         COFACTOR_4X4_IJ (a[j][i], m, i, j);			\
      }								\
   }								\
}

/* ========================================================== */
/* compute adjoint of matrix and scale
 *
 * Computes adjoint of matrix m, scales it by s, returning a
 */

#define SCALE_ADJOINT_2X2(a,s,m)				\
{								\
   a[0][0] = (s) * m[1][1];					\
   a[1][0] = - (s) * m[1][0];					\
   a[0][1] = - (s) * m[0][1];					\
   a[1][1] = (s) * m[0][0];					\
}

/* ========================================================== */
/* compute adjoint of matrix and scale
 *
 * Computes adjoint of matrix m, scales it by s, returning a
 */

#define SCALE_ADJOINT_3X3(a,s,m)				\
{								\
   a[0][0] = (s) * (m[1][1] * m[2][2] - m[1][2] * m[2][1]);	\
   a[1][0] = (s) * (m[1][2] * m[2][0] - m[1][0] * m[2][2]);	\
   a[2][0] = (s) * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);	\
								\
   a[0][1] = (s) * (m[0][2] * m[2][1] - m[0][1] * m[2][2]);	\
   a[1][1] = (s) * (m[0][0] * m[2][2] - m[0][2] * m[2][0]);	\
   a[2][1] = (s) * (m[0][1] * m[2][0] - m[0][0] * m[2][1]);	\
								\
   a[0][2] = (s) * (m[0][1] * m[1][2] - m[0][2] * m[1][1]);	\
   a[1][2] = (s) * (m[0][2] * m[1][0] - m[0][0] * m[1][2]);	\
   a[2][2] = (s) * (m[0][0] * m[1][1] - m[0][1] * m[1][0]);	\
}

/* ========================================================== */
/* compute adjoint of matrix and scale
 *
 * Computes adjoint of matrix m, scales it by s, returning a
 */

#define SCALE_ADJOINT_4X4(a,s,m)				\
{								\
   int i,j;							\
								\
   for (i=0; i<4; i++) {					\
      for (j=0; j<4; j++) {					\
         COFACTOR_4X4_IJ (a[j][i], m, i, j);			\
         a[j][i] *= s;						\
      }								\
   }								\
}

/* ========================================================== */
/* inverse of matrix 
 *
 * Compute inverse of matrix a, returning determinant m and 
 * inverse b
 */

#define INVERT_2X2(b,det,a)			\
{						\
   double tmp;					\
   DETERMINANT_2X2 (det, a);			\
   tmp = 1.0 / (det);				\
   SCALE_ADJOINT_2X2 (b, tmp, a);		\
}

/* ========================================================== */
/* inverse of matrix 
 *
 * Compute inverse of matrix a, returning determinant m and 
 * inverse b
 */

#define INVERT_3X3(b,det,a)			\
{						\
   double tmp;					\
   DETERMINANT_3X3 (det, a);			\
   tmp = 1.0 / (det);				\
   SCALE_ADJOINT_3X3 (b, tmp, a);		\
}

/* ========================================================== */
/* inverse of matrix 
 *
 * Compute inverse of matrix a, returning determinant m and 
 * inverse b
 */

#define INVERT_4X4(b,det,a)			\
{						\
   double tmp;					\
   DETERMINANT_4X4 (det, a);			\
   tmp = 1.0 / (det);				\
   SCALE_ADJOINT_4X4 (b, tmp, a);		\
}

/* ========================================================== */

#define DIFF3(_a,_b,_c)          \
{                                \
    (_c)[0] = (_a)[0] - (_b)[0]; \
    (_c)[1] = (_a)[1] - (_b)[1]; \
    (_c)[2] = (_a)[2] - (_b)[2]; \
}






/*
 * FUNCTION:
 * This file contains a number of utilities useful to 3D graphics in
 * general, and to the generation of tubing and extrusions in particular
 * 
 * HISTORY:
 * Written by Linas Vepstas, August 1991
 * Updated to correctly handle degenerate cases, Linas,  February 1993 
 */

#define BACKWARDS_INTERSECT (2)

/* ========================================================== */
/*
 * the Degenerate_Tolerance token represents the greatest amount by
 * which different scales in a graphics environment can differ before
 * they should be considered "degenerate".   That is, when one vector is
 * a million times longer than another, changces are that the second will
 * be less than a pixel long, and therefore was probably meant to be
 * degenerate (by the CAD package, etc.)  But what should this tolerance
 * be?  At least 1 in onethousand (since screen sizes are 1K pixels), but
 * les than 1 in 4 million (since this is the limit of single-precision
 * floating point accuracy).  Of course, if double precision were used,
 * then the tolerance could be increased.
 * 
 * Potentially, this naive assumption could cause problems if the CAD
 * package attempts to zoom in on small details, and turns out, certain
 * points should not hvae been degenerate.  The problem presented here
 * is that the tolerance could run out before single-precision ran
 * out, and so the CAD packages would perceive this as a "bug".
 * One alternative is to fiddle around & try to tighten the tolerance.
 * However, the right alternative is to code the graphics pipeline in
 * double-precision (and tighten the tolerance).
 *
 * By the way, note that Degernate Tolerance is a "dimensionless"
 * quantitiy -- it has no units -- it does not measure feet, inches,
 * millimeters or pixels.  It is used only in the computations of ratios
 * and relative lengths.
 */

/* 
 * Right now, the tolerance is set to 2 parts in a million, which
 * corresponds to a 19-bit distinction of mantissas. Note that
 * single-precsion numbers have 24 bit mantissas.
 */

#define DEGENERATE_TOLERANCE   (0.000002)

/* ========================================================== */
/* 
 * The macro and subroutine INTERSECT are designed to compute the
 * intersection of a line (defined by the points v1 and v2) and a plane
 * (defined as plane which is normal to the vector n, and contains the
 * point p).  Both return the point sect, which is the point of
 * interesection.
 *
 * This MACRO attemps to be fairly robust by checking for a divide by
 * zero.
 */

/* ========================================================== */
/*
 * HACK ALERT
 * The intersection parameter t has the nice property that if t>1,
 * then the intersection is "in front of" p1, and if t<0, then the
 * intersection is "behind" p2. Unfortunately, as the intersecting plane
 * and the line become parallel, t wraps through infinity -- i.e. t can
 * become so large that t becomes "greater than infinity" and comes back 
 * as a negative number (i.e. winding number hopped by one unit).  We 
 * have no way of detecting this situation without adding gazzillions 
 * of lines of code of topological algebra to detect the winding number;
 * and this would be incredibly difficult, and ruin performance.
 * 
 * Thus, we've installed a cheap hack for use by the "cut style" drawing
 * routines. If t proves to be a large negative number (more negative
 * than -5), then we assume that t was positive and wound through
 * infinity.  This makes most cuts look good, without introducing bogus
 * cuts at infinity.
 */
/* ========================================================== */

#define INTERSECT(sect,p,n,v1,v2)			\
{							\
   double deno, numer, t, omt;			\
							\
   deno = (v1[0] - v2[0]) * n[0];			\
   deno += (v1[1] - v2[1]) * n[1];			\
   deno += (v1[2] - v2[2]) * n[2];			\
   							\
   if (deno == 0.0) {					\
      VEC_COPY (n, v1);					\
      /* printf ("Intersect: Warning: line is coplanar with plane \n"); */ \
   } else {						\
							\
      numer = (p[0] - v2[0]) * n[0];			\
      numer += (p[1] - v2[1]) * n[1];			\
      numer += (p[2] - v2[2]) * n[2];			\
							\
      t = numer / deno;					\
      omt = 1.0 - t;					\
							\
      sect[0] = t * v1[0] + omt * v2[0];		\
      sect[1] = t * v1[1] + omt * v2[1];		\
      sect[2] = t * v1[2] + omt * v2[2];		\
   }							\
}

/* ========================================================== */
/* 
 * The macro and subroutine BISECTING_PLANE compute a normal vector that
 * describes the bisecting plane between three points (v1, v2 and v3).  
 * This bisecting plane has the following properties:
 * 1) it contains the point v2
 * 2) the angle it makes with v21 == v2 - v1 is equal to the angle it 
 *    makes with v32 == v3 - v2 
 * 3) it is perpendicular to the plane defined by v1, v2, v3.
 *
 * Having input v1, v2, and v3, it returns a unit vector n.
 *
 * In some cases, the user may specify degenerate points, and still
 * expect "reasonable" or "obvious" behaviour.  The "expected"
 * behaviour for these degenerate cases is:
 *
 * 1) if v1 == v2 == v3, then return n=0
 * 2) if v1 == v2, then return v32 (normalized).
 * 3) if v2 == v3, then return v21 (normalized).
 * 4) if v1, v2 and v3 co-linear, then return v21 (normalized).
 *
 * Mathematically, these special cases "make sense" -- we just have to
 * code around potential divide-by-zero's in the code below.
 */

/* ========================================================== */

#define BISECTING_PLANE(valid,n,v1,v2,v3)			\
{								\
   double v21[3], v32[3];					\
   double len21, len32;						\
   double dot;							\
								\
   VEC_DIFF (v21, v2, v1);					\
   VEC_DIFF (v32, v3, v2);					\
								\
   VEC_LENGTH (len21, v21);					\
   VEC_LENGTH (len32, v32);					\
								\
   if (len21 <= DEGENERATE_TOLERANCE * len32) {			\
								\
      if (len32 == 0.0) {					\
         /* all three points lie ontop of one-another */	\
         VEC_ZERO (n);						\
         valid = FALSE;						\
      } else {							\
         /* return a normalized copy of v32 as bisector */	\
         len32 = 1.0 / len32;					\
         VEC_SCALE (n, len32, v32);				\
         valid = TRUE;						\
      }								\
								\
   } else {							\
								\
      valid = TRUE;						\
								\
      if (len32 <= DEGENERATE_TOLERANCE * len21) {		\
         /* return a normalized copy of v21 as bisector */	\
         len21 = 1.0 / len21;					\
         VEC_SCALE (n, len21, v21);				\
								\
      } else {							\
								\
         /* normalize v21 to be of unit length */		\
         len21 = 1.0 / len21;					\
         VEC_SCALE (v21, len21, v21);				\
								\
         /* normalize v32 to be of unit length */		\
         len32 = 1.0 / len32;					\
         VEC_SCALE (v32, len32, v32);				\
								\
         VEC_DOT_PRODUCT (dot, v32, v21);			\
								\
         /* if dot == 1 or -1, then points are colinear */	\
         if ((dot >= (1.0-DEGENERATE_TOLERANCE)) || 		\
             (dot <= (-1.0+DEGENERATE_TOLERANCE))) {		\
            VEC_COPY (n, v21);					\
         } else {						\
   								\
            /* go do the full computation */ 			\
            n[0] = dot * (v32[0] + v21[0]) - v32[0] - v21[0];	\
            n[1] = dot * (v32[1] + v21[1]) - v32[1] - v21[1];	\
            n[2] = dot * (v32[2] + v21[2]) - v32[2] - v21[2];	\
								\
            /* if above if-test's passed, 			\
             * n should NEVER be of zero length */		\
            VEC_NORMALIZE (n);					\
         } 							\
      } 							\
   } 								\
}

/* ========================================================== */
/*
 * The block of code below is ifdef'd out, and is here for reference
 * purposes only.  It performs the "mathematically right thing" for
 * computing a bisecting plane, but is, unfortunately, subject ot noise
 * in the presence of near degenerate points.  Since computer graphics,
 * due to sloppy coding, laziness, or correctness, is filled with
 * degenerate points, we can't really use this version.  The code above
 * is far more appropriate for graphics.
 */

#ifdef MATHEMATICALLY_EXACT_GRAPHICALLY_A_KILLER
#define BISECTING_PLANE(n,v1,v2,v3)				\
{								\
   double v21[3], v32[3];					\
   double len21, len32;						\
   double dot;							\
								\
   VEC_DIFF (v21, v2, v1);					\
   VEC_DIFF (v32, v3, v2);					\
								\
   VEC_LENGTH (len21, v21);					\
   VEC_LENGTH (len32, v32);					\
								\
   if (len21 == 0.0) {						\
								\
      if (len32 == 0.0) {					\
         /* all three points lie ontop of one-another */	\
         VEC_ZERO (n);						\
         valid = FALSE;						\
      } else {							\
         /* return a normalized copy of v32 as bisector */	\
         len32 = 1.0 / len32;					\
         VEC_SCALE (n, len32, v32);				\
      }								\
								\
   } else {							\
								\
      /* normalize v21 to be of unit length */			\
      len21 = 1.0 / len21;					\
      VEC_SCALE (v21, len21, v21);				\
								\
      if (len32 == 0.0) {					\
         /* return a normalized copy of v21 as bisector */	\
         VEC_COPY (n, v21);					\
      } else {							\
								\
         /* normalize v32 to be of unit length */		\
         len32 = 1.0 / len32;					\
         VEC_SCALE (v32, len32, v32);				\
								\
         VEC_DOT_PRODUCT (dot, v32, v21);			\
								\
         /* if dot == 1 or -1, then points are colinear */	\
         if ((dot == 1.0) || (dot == -1.0)) {			\
            VEC_COPY (n, v21);					\
         } else {						\
   								\
            /* go do the full computation */ 			\
            n[0] = dot * (v32[0] + v21[0]) - v32[0] - v21[0];	\
            n[1] = dot * (v32[1] + v21[1]) - v32[1] - v21[1];	\
            n[2] = dot * (v32[2] + v21[2]) - v32[2] - v21[2];	\
								\
            /* if above if-test's passed, 			\
             * n should NEVER be of zero length */		\
            VEC_NORMALIZE (n);					\
         } 							\
      } 							\
   } 								\
}
#endif

/* ========================================================== */
/*
 * This macro computes the plane perpendicular to the the plane
 * defined by three points, and whose normal vector is givven as the
 * difference between the two vectors ...
 * 
 * (See way below for the "math" model if you want to understand this.
 * The comments about relative errors above apply here.)
 */

#define CUTTING_PLANE(valid,n,v1,v2,v3)				\
{								\
   double v21[3], v32[3];					\
   double len21, len32;						\
   double lendiff;						\
								\
   VEC_DIFF (v21, v2, v1);					\
   VEC_DIFF (v32, v3, v2);					\
								\
   VEC_LENGTH (len21, v21);					\
   VEC_LENGTH (len32, v32);					\
								\
   if (len21 <= DEGENERATE_TOLERANCE * len32) {			\
								\
      if (len32 == 0.0) {					\
         /* all three points lie ontop of one-another */	\
         VEC_ZERO (n);						\
         valid = FALSE;						\
      } else {							\
         /* return a normalized copy of v32 as cut-vector */	\
         len32 = 1.0 / len32;					\
         VEC_SCALE (n, len32, v32);				\
         valid = TRUE;						\
      }								\
								\
   } else {							\
								\
      valid = TRUE;						\
								\
      if (len32 <= DEGENERATE_TOLERANCE * len21) {		\
         /* return a normalized copy of v21 as cut vector */	\
         len21 = 1.0 / len21;					\
         VEC_SCALE (n, len21, v21);				\
      } else {							\
								\
         /* normalize v21 to be of unit length */		\
         len21 = 1.0 / len21;					\
         VEC_SCALE (v21, len21, v21);				\
								\
         /* normalize v32 to be of unit length */		\
         len32 = 1.0 / len32;					\
         VEC_SCALE (v32, len32, v32);				\
								\
         VEC_DIFF (n, v21, v32);				\
         VEC_LENGTH (lendiff, n);				\
								\
         if (lendiff < DEGENERATE_TOLERANCE) {			\
            VEC_ZERO (n);					\
            valid = FALSE;					\
         } else {						\
            lendiff = 1.0 / lendiff;				\
            VEC_SCALE (n, lendiff, n);				\
         } 							\
      } 							\
   } 								\
}
         /* if the perp vector is very small, then the two 	\
          * vectors are darn near collinear, and the cut 	\
          * vector is probably poorly defined. */		\

/* ========================================================== */
/*
#ifdef MATHEMATICALLY_EXACT_GRAPHICALLY_A_KILLER
#define CUTTING_PLANE(n,v1,v2,v3)		\
{						\
   double v21[3], v32[3];			\
						\
   VEC_DIFF (v21, v2, v1);			\
   VEC_DIFF (v32, v3, v2);			\
						\
   VEC_NORMALIZE (v21);				\
   VEC_NORMALIZE (v32);				\
						\
   VEC_DIFF (n, v21, v32);			\
   VEC_NORMALIZE (n);				\
}
#endif
*/

/* ============================================================ */
/* This macro is used in several places to cycle through a series of
 * points to find the next non-degenerate point in a series */

#define FIND_NON_DEGENERATE_POINT(inext,npoints,len,diff,point_array)   \
{                                                                       \
   double slen;							\
   double summa[3];							\
   									\
   do {                                                                 \
      /* get distance to next point */                                  \
      VEC_DIFF (diff, point_array[inext+1], point_array[inext]);        \
      VEC_LENGTH (len, diff);                                           \
      VEC_SUM (summa, point_array[inext+1], point_array[inext]);        \
      VEC_LENGTH (slen, summa);                                         \
      slen *= DEGENERATE_TOLERANCE;					\
      inext ++;                                                         \
   } while ((len <= slen) && (inext < npoints-1));                      \
}

/* ========================================================== */

extern int bisecting_plane (double n[3],    /* returned */
                      double v1[3],  /* input */
                      double v2[3],  /* input */
                      double v3[3]);  /* input */




//#define INLINE_INTERSECT
//#ifdef INLINE_INTERSECT
#define INNERSECT(sect,p,n,v1,v2) { INTERSECT(sect,p,n,v1,v2); }
//#else
//#define INNERSECT(sect,p,n,v1,v2) intersect(sect,p,n,v1,v2)
//#endif /* INLINE_INTERSECT */

//---------------------------------------------------------------------------

