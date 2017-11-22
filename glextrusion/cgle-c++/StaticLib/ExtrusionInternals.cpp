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
#include "extrusionlib.h"
#include "extrusioninternals.h"

gleGC *_gle_gc = 0x0;
float Color[3], ambmat[4], difmat[4], emismat[4];// static vars for
bool StatUseLitMat;                         // glColor / Tess routines

//----------------------------------------------------------------------------
void CreateGC()
{
	if (_gle_gc == 0x0)
	{
		_gle_gc =  (gleGC*) malloc (sizeof (gleGC));

	   _gle_gc -> bgn_gen_texture = 0x0;
	   _gle_gc -> n3f_gen_texture = 0x0;
	   _gle_gc -> n3d_gen_texture = 0x0;
	   _gle_gc -> v3f_gen_texture = 0x0;
	   _gle_gc -> v3d_gen_texture = 0x0;
	   _gle_gc -> end_gen_texture = 0x0;

	   _gle_gc -> num_vert = 0;
	   _gle_gc -> segment_number = 0;
	   _gle_gc -> segment_length = 0.0;
	   _gle_gc -> accum_seg_len = 0.0;
	   _gle_gc -> prev_x = 0.0;
	   _gle_gc -> prev_y = 0.0;

	   ambmat[3] = difmat[3] = emismat[3] = 1.0;
	}
}
//----------------------------------------------------------------------------
void DestroyGC()
{
	if (_gle_gc != 0x0)
		free (_gle_gc);
	_gle_gc = 0x0;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SetColor(float color[3])
{

   for (int i = 0;i < 3;i++)
      Color[i] = color[i];


   if (StatUseLitMat)
   {
      difmat[0] = 0.4f * Color[0];
      difmat[1] = 0.4f * Color[1];
      difmat[2] = 0.4f * Color[2];

      glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,difmat);

      emismat[0] = 0.3f * Color[0];
      emismat[1] = 0.3f * Color[1];
      emismat[2] = 0.3f * Color[2];

      glMaterialfv(GL_FRONT,GL_EMISSION,emismat);
   }
   else
      glColor3fv(Color);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#define segment_number (_gle_gc -> segment_number)
#define segment_length (_gle_gc -> segment_length)
#define accum_seg_len (_gle_gc -> accum_seg_len)
#define num_vert  (_gle_gc -> num_vert)
#define prev_x  (_gle_gc -> prev_x)
#define prev_y  (_gle_gc -> prev_y)


static double save_nx = 0.0;
static double save_ny = 0.0;
static double save_nz = 0.0;

//----------------------------------------------------------------------------
void save_normal (double *v)
{
   save_nx = v[0];
   save_ny = v[1];
   save_nz = v[2];
}
//----------------------------------------------------------------------------
 void bgn_sphere_texgen (int inext, double len)
{
   segment_number = inext - 1;
   segment_length = len;
   num_vert = 0;
}
//
 // this routine assumes that the vertex passed in has been normalized
 // (i.e. is of unit length)
 //

//----------------------------------------------------------------------------
inline void sphere_texgen (double x, double y, double z,
                                                       int jcnt, int which_end)
{
   double theta, phi;

   // let phi and theta range fro 0 to 1
   phi = 0.5 * atan2 (x, y) / M_PI;
   phi += 0.5;

   theta = 1.0 - acos (z) / M_PI;

   // if first vertex, merely record the texture coords
   if (num_vert == 0)
   {
      prev_x = phi;
      prev_y = theta;
      num_vert ++;
   }
   else
   {

      // if texture coordinates changed radically, wrap them
      if ((prev_y - theta) > 0.6)
      {
         theta +=1.0;
      }
      else if ((prev_y - theta) < -0.6)
      {
         theta -=1.0;
      } // else no-op
      prev_y = theta;


      // if texture coordinates changed radically, wrap them
      if ((prev_x - phi) > 0.6)
      {
         phi +=1.0;
      }
      else if ((prev_x - phi) < -0.6)
      {
         phi -=1.0;
      } // else no-op
      prev_x = phi;

   }

   T2F_D (phi, theta);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// mappers

 void vertex_sphere_texgen_v (double *v, int jcnt, int which_end)
{
   double x = v[0]; double y = v[1]; double z = v[2];
   double r;

   r = 1.0 / sqrt (x*x + y*y + z*z);
   x *= r;
   y *= r;
   z *= r;
   sphere_texgen (x, y, z, jcnt, which_end);
}
//----------------------------------------------------------------------------
void normal_sphere_texgen_v (double *v, int jcnt, int which_end)
{
   sphere_texgen (save_nx, save_ny, save_nz, jcnt, which_end);
}
//----------------------------------------------------------------------------
void vertex_sphere_model_v (double *v, int jcnt, int which_end)
{
   double x = _gle_gc->contour[jcnt][0];
   double y = _gle_gc->contour[jcnt][1];
   double z = v[2];
   double r;

   r = 1.0 / sqrt (x*x + y*y + z*z);
   x *= r;
   y *= r;
   z *= r;
   sphere_texgen (x, y, z, jcnt, which_end);
}
//----------------------------------------------------------------------------
void normal_sphere_model_v (double *v, int jcnt, int which_end)
{
   if (!(_gle_gc -> cont_normal))
      return;
   sphere_texgen (_gle_gc->cont_normal[jcnt][0], 
                _gle_gc->cont_normal[jcnt][1], 0.0, jcnt, which_end);
}
//----------------------------------------------------------------------------
void bgn_z_texgen (int inext, double len)
{

   // accumulate the previous length
   accum_seg_len += segment_length;

   // save current values
   segment_number = inext - 1;
   segment_length = len;

   // reset counter on first segment
   if (1 >= segment_number) accum_seg_len = 0.0;

   num_vert = 0;
}
//----------------------------------------------------------------------------
inline void cylinder_texgen (double x, double y, double z,
                             int jcnt, int which_end)
{
   double phi;

   // let phi and theta range fro 0 to 1
   phi = 0.5 * atan2 (x, y) / M_PI;
   phi += 0.5;

   // if first vertex, merely record the texture coords
   if (num_vert == 0)
   {
      prev_x = phi;
      num_vert ++;
   }
   else
   {
      // if texture coordinates changed radically, wrap them
      if ((prev_x - phi) > 0.6)
      {
         phi +=1.0;
      }
      else if ((prev_x - phi) < -0.6)
      {
         phi -=1.0;
      } // else no-op
      prev_x = phi;
   }

   if (FRONT == which_end)
      T2F_D (phi, accum_seg_len);

   if (BACK == which_end)
      T2F_D (phi, accum_seg_len + segment_length);

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// mappers
void vertex_cylinder_texgen_v (double *v, int jcnt, int which_end)
{
   double x = v[0];
   double y = v[1];
   double z = v[2];
   double r;

   r = 1.0 / sqrt (x*x + y*y);
   x *= r;
   y *= r;
   cylinder_texgen (x, y, z, jcnt, which_end);
}
//----------------------------------------------------------------------------
void normal_cylinder_texgen_v (double *v, int jcnt, int which_end)
{
   cylinder_texgen (save_nx, save_ny, save_nz, jcnt, which_end);
}
//----------------------------------------------------------------------------
void vertex_cylinder_model_v (double *v, int jcnt, int which_end)
{
   double x = _gle_gc->contour[jcnt][0];
   double y = _gle_gc->contour[jcnt][1];
   double z = v[2];
   double r;

   r = 1.0 / sqrt (x*x + y*y);
   x *= r;
   y *= r;
   cylinder_texgen (x, y, z, jcnt, which_end);
}
//----------------------------------------------------------------------------
void normal_cylinder_model_v (double *v, int jcnt, int which_end)
{
   if (!(_gle_gc -> cont_normal))
      return;

   cylinder_texgen (_gle_gc->cont_normal[jcnt][0],
                _gle_gc->cont_normal[jcnt][1], 0.0, jcnt, which_end);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline void flat_texgen (double x, double y, double z,
                             int jcnt, int which_end)
{
   if (FRONT == which_end)
      T2F_D (x, accum_seg_len);

   if (BACK == which_end)
      T2F_D (x, accum_seg_len + segment_length);

}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void vertex_flat_texgen_v (double *v, int jcnt, int which_end)
{
   flat_texgen (v[0], v[1], v[2], jcnt, which_end);
}
//----------------------------------------------------------------------------
void normal_flat_texgen_v (double *v, int jcnt, int which_end)
{
   flat_texgen (save_nx, save_ny, save_nz, jcnt, which_end);
}
//----------------------------------------------------------------------------
void vertex_flat_model_v (double *v, int jcnt, int which_end)
{
   flat_texgen (_gle_gc->contour[jcnt][0],
                _gle_gc->contour[jcnt][1], v[2], jcnt, which_end);
}
//----------------------------------------------------------------------------
void normal_flat_model_v (double *v, int jcnt, int which_end)
{
   if (!(_gle_gc -> cont_normal)) return;
   flat_texgen (_gle_gc->cont_normal[jcnt][0],
                _gle_gc->cont_normal[jcnt][1], 0.0, jcnt, which_end);
}
//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/* 
 * The macro and subroutine INTERSECT are designed to compute the
 * intersection of a line (defined by the points v1 and v2) and a plane
 * (defined as plane which is normal to the vector n, and contains the
 * point p).  Both sect the array "sect", which is the point of
 * interesection.
 * 
 * The subroutine returns a value indicating if the specified inputs
 * represented a degenerate case. Valid is TRUE if the computed
 * intersection is valid, else it is FALSE.
 */


//----------------------------------------------------------------------------
  /*
void intersect (double sect[3],	// returned
                double p[3],	// input
                double n[3],	// input
                double v1[3],	// input
                double v2[3])	// input
{
   INTERSECT (sect, p, n, v1, v2);
}
     */
//----------------------------------------------------------------------------
/* 
 * The macro and subroutine BISECTING_PLANE compute a normal vecotr that
 * describes the bisecting plane between three points (v1, v2 and v3).  
 * This bisecting plane has the following properties:
 * 1) it contains the point v2
 * 2) the angle it makes with v21 == v2 - v1 is equal to the angle it 
 *    makes with v32 == v3 - v2 
 * 3) it is perpendicular to the plane defined by v1, v2, v3.
 *
 * Having input v1, v2, and v3, it returns a vector n.
 * Note that n is NOT normalized (is NOT of unit length).
 *
 * The subroutine returns a value indicating if the specified inputs
 * represented a degenerate case. Valid is TRUE if the computed
 * intersection is valid, else it is FALSE.
 */

int bisecting_plane (double n[3],	// returned
                      double v1[3],	// input
                      double v2[3],	// input
                      double v3[3])	// input
{
   int valid;

   BISECTING_PLANE (valid, n, v1, v2, v3);
   return (valid);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//
 // MODULE Name: view.c
 //
 // FUNCTION:
 // This module provides two different routines that compute and return
 // viewing matrices.  Both routines take a direction and an up vector,
 // and return a matrix that transforms the direction to the z-axis, and
 // the up-vector to the y-axis.
 //
 // HISTORY:
 // written by Linas Vepstas August 1991
 // Added double precision interface, March 1993, Linas
 //

//----------------------------------------------------------------------------
//
 // The uviewdirection subroutine computes and returns a 4x4 rotation
 // matrix that puts the negative z axis along the direction v21 and
 // puts the y axis along the up vector.
 //
 // Note that this code is fairly tolerant of "weird" paramters.
 // It normalizes when necessary, it does nothing when vectors are of
 // zero length, or are co-linear.  This code shouldn't croak, no matter
 // what the user sends in as arguments.
 //
void uview_direction_d (double m[4][4],		// returned
                        double v21[3],		//input
                        double up[3])		// input
{
   double amat[4][4];
   double bmat[4][4];
   double cmat[4][4];
   double v_hat_21[3];
   double v_xy[3];
   double sine, cosine;
   double len;
   double up_proj[3];
   double tmp[3];

   // find the unit vector that points in the v21 direction
   VEC_COPY (v_hat_21, v21);
   VEC_LENGTH (len, v_hat_21);
   if (len != 0.0) {
      len = 1.0 / len;
      VEC_SCALE (v_hat_21, len, v_hat_21);

      // rotate z in the xz-plane until same latitude
      sine = sqrt ( 1.0 - v_hat_21[2] * v_hat_21[2]);
      ROTY_CS (amat, (-v_hat_21[2]), (-sine));

   } else {

      // error condition: zero length vecotr passed in -- do nothing
      IDENTIFY_MATRIX_4X4 (amat);
   }


   // project v21 onto the xy plane
   v_xy[0] = v21[0];
   v_xy[1] = v21[1];
   v_xy[2] = 0.0;
   VEC_LENGTH (len, v_xy);

   // rotate in the x-y plane until v21 lies on z axis ---
    // but of course, if its already there, do nothing
   if (len != 0.0) { 

      // want xy projection to be unit vector, so that sines/cosines pop out
      len = 1.0 / len;
      VEC_SCALE (v_xy, len, v_xy);

      // rotate the projection of v21 in the xy-plane over to the x axis
      ROTZ_CS (bmat, v_xy[0], v_xy[1]);

      // concatenate these together
      MATRIX_PRODUCT_4X4 (cmat, amat, bmat);

   } else {

      // no-op -- vector is already in correct position
      COPY_MATRIX_4X4 (cmat, amat);
   }

   // up vector really should be perpendicular to the x-form direction --
    // Use up a couple of cycles, and make sure it is,
    // just in case the user blew it.
    //
   VEC_PERP (up_proj, up, v_hat_21);
   VEC_LENGTH (len, up_proj);
   if (len != 0.0) {

      // normalize the vector
      len = 1.0/len;
      VEC_SCALE (up_proj, len, up_proj);
   
      // compare the up-vector to the  y-axis to get the cosine of the angle
      tmp [0] = cmat [1][0];
      tmp [1] = cmat [1][1];
      tmp [2] = cmat [1][2];
      VEC_DOT_PRODUCT (cosine, tmp, up_proj);
   
      // compare the up-vector to the x-axis to get the sine of the angle
      tmp [0] = cmat [0][0];
      tmp [1] = cmat [0][1];
      tmp [2] = cmat [0][2];
      VEC_DOT_PRODUCT (sine, tmp, up_proj);
   
      // rotate to align the up vector with the y-axis
      ROTZ_CS (amat, cosine, -sine);

      // This xform, although computed last, acts first
      MATRIX_PRODUCT_4X4 (m, amat, cmat);

   } else {

      // error condition: up vector is indeterminate (zero length)
       // -- do nothing
      COPY_MATRIX_4X4 (m, cmat);
   }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
 // The uviewpoint subroutine computes and returns a 4x4 matrix that
 // translates the origen to the point v1, puts the negative z axis
 // along the direction v21==v2-v1, and puts the y axis along the up
 // vector.
 //
void uviewpoint_d (double m[4][4],		// returned
                   double v1[3],		// input
                   double v2[3],		// input
                   double up[3])		// input
{
   double v_hat_21 [3];
   double trans_mat[4][4];
   double rot_mat[4][4];
   // find the vector that points in the v21 direction
   VEC_DIFF (v_hat_21, v2, v1);

   // compute rotation matrix that takes -z axis to the v21 axis,
    // and y to the up dierction
   uview_direction_d (rot_mat, v_hat_21, up);

   // build matrix that translates the origin to v1
   IDENTIFY_MATRIX_4X4 (trans_mat);
   trans_mat[3][0] = v1[0];
   trans_mat[3][1] = v1[1];
   trans_mat[3][2] = v1[2];

   // concatenate the matrices together
   MATRIX_PRODUCT_4X4 (m, rot_mat, trans_mat);

}

//----------------------------------------------------------------------------

void urot_axis_d (double m[4][4], 		// returned
                  double omega, 		// input
                  double axis[3])		// input
{
   double s, c, ssq, csq, cts;
   double tmp;

   //
    // The formula coded up below can be derived by using the
    // homomorphism between SU(2) and O(3), namely, that the
    // 3x3 rotation matrix R is given by
    //      t.R.v = S(-1) t.v S
    // where
    // t are the Pauli matrices (similar to Quaternions, easier to use)
    // v is an arbitrary 3-vector
    // and S is a 2x2 hermitian matrix:
    //     S = exp ( i omega t.axis / 2 )
    //
    // (Also, remember that computer graphics uses the transpose of R).
    //
    // The Pauli matrices are:
    //
    //  tx = (0 1)          ty = (0 -i)            tz = (1  0)
    //       (1 0)               (i  0)                 (0 -1)
    //
    // Note that no error checking is done -- if the axis vector is not
    // of unit length, you'll get strange results.
    //

   tmp = (double) omega / 2.0;
   s = sin (tmp);
   c = cos (tmp);

   ssq = s*s;
   csq = c*c;

   m[0][0] = m[1][1] = m[2][2] = csq - ssq;

   ssq *= 2.0;

   // on-diagonal entries
   m[0][0] += ssq * axis[0]*axis[0];
   m[1][1] += ssq * axis[1]*axis[1];
   m[2][2] += ssq * axis[2]*axis[2];

   // off-diagonal entries
   m[0][1] = m[1][0] = axis[0] * axis[1] * ssq;
   m[1][2] = m[2][1] = axis[1] * axis[2] * ssq;
   m[2][0] = m[0][2] = axis[2] * axis[0] * ssq;

   cts = 2.0 * c * s;

   tmp = cts * axis[2];
   m[0][1] += tmp;
   m[1][0] -= tmp;

   tmp = cts * axis[0];
   m[1][2] += tmp;
   m[2][1] -= tmp;

   tmp = cts * axis[1];
   m[2][0] += tmp;
   m[0][2] -= tmp;

   // homogeneous entries
   m[0][3] = m[1][3] = m[2][3] = m[3][2] = m[3][1] = m[3][0] = 0.0;
   m[3][3] = 1.0;


}
//----------------------------------------------------------------------------
void urot_about_axis_d (double m[4][4], 		// returned
                        double angle, 		// input
                        double axis[3])		// input
{
   gutDouble len, ax[3];

   angle *= M_PI/180.0;		// convert to radians

   // renormalize axis vector, if needed
   len = axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2];

   // we can save some machine instructions by normalizing only
    // if needed.  The compiler should be able to schedule in the
    // if test "for free".
   if (len != 1.0) {
      len = (gutDouble) (1.0 / sqrt ((double) len));
      ax[0] = axis[0] * len;
      ax[1] = axis[1] * len;
      ax[2] = axis[2] * len;
      urot_axis_d (m, angle, (double*)ax);
   } else {
      urot_axis_d (m, angle, (double*)axis);
   }
}
//----------------------------------------------------------------------------
void urot_omega_d (double m[4][4], 	// returned
                   double axis[3])		// input
{
   gutDouble len, ax[3];

   // normalize axis vector
   len = axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2];

   len = (gutDouble) (1.0 / sqrt ((double) len));
   ax[0] = axis[0] * len;
   ax[1] = axis[1] * len;
   ax[2] = axis[2] * len;

   // the amount of rotation is equal to the length, in radians
   urot_axis_d (m, len, (double*)ax);

}
//----------------------------------------------------------------------------
void rot_axis_d (double omega, 		// input
               double axis[3])		// input
{
   double m[4][4];

   (void) urot_axis_d (m, omega, axis);
    glMultMatrixd ((const double *)m);
}
//----------------------------------------------------------------------------
void rot_about_axis_d (double angle, 		// input
                       double axis[3])		// input
{
   double m[4][4];

   (void) urot_about_axis_d (m, angle, axis);
   glMultMatrixd ((const double *)m);
}
//----------------------------------------------------------------------------
void rot_omega_d (double axis[3])		// input
{
   double m[4][4];

   (void) urot_omega_d (m, axis);
   glMultMatrixd ((const double *)m);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
 // The routines below generate and return more traditional rotation
 // matrices -- matrices for rotations about principal axes.
 //
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void urotx_sc_d (double m[4][4], 		// returned
                 double cosine,		// input
                 double sine) 		// input
{
   // create matrix that represents rotation about the x-axis

   ROTX_CS (m, cosine, sine);
}
//----------------------------------------------------------------------------
void uroty_sc_d (double m[4][4], 		// returned
                 double cosine,		// input
                 double sine) 		// input
{
   // create matriy that represents rotation about the y-ayis

   ROTX_CS (m, cosine, sine);
}
//----------------------------------------------------------------------------
void urotz_sc_d (double m[4][4], 		// returned
                 double cosine,		// input
                 double sine) 		// input
{
   // create matriz that represents rotation about the z-azis

   ROTX_CS (m, cosine, sine);
}
//----------------------------------------------------------------------------
