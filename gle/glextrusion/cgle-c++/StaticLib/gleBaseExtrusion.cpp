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
#include "gleBaseExtrusion.h"

extern gleGC *_gle_gc;
extern bool StatUseLitMat;                         // glColor / Tess routines

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CgleBaseExtrusion::CgleBaseExtrusion(int Points, int ContourPoints, double Radius)
{
	m_iPoints = Points;
	m_iContourPoints = ContourPoints;

	if (Radius == NULL)
		m_dRadius = 1.0;
	else
		m_dRadius = Radius;


	m_ptrUp = NULL;
	m_ptrXforms = NULL;
	m_ptrPath = NULL;
	m_ptrTwarr = NULL;
	m_bTexEnabled = false;

	m_bUseLitMaterial = true;

	m_ptrCont_Norms = new gleContourVector[m_iContourPoints + 1];
	m_ptrContours = new gleContourVector[m_iContourPoints + 1];


	FMemAnchor = new char[96 * m_iContourPoints];
	FXAJMemAnchor = new char[96 * m_iContourPoints];
	FROCJMemAnchor = new char[152 * m_iContourPoints];


	m_objTess = gluNewTess();

	FCapLoop = new double[(m_iContourPoints + 3) * 3];

	m_iJoinStyle = GLE_JN_ANGLE | GLE_JN_CAP | GLE_NORM_FACET |
																GLE_CONTOUR_CLOSED;
	m_iTexMode = GLE_TEXTURE_VERTEX_CYL;

}

//----------------------------------------------------------------------------
CgleBaseExtrusion::~CgleBaseExtrusion()
{

	delete[] FMemAnchor;
	delete[] FXAJMemAnchor;
	delete[] FROCJMemAnchor;

	gluDeleteTess(m_objTess);

	delete[] FCapLoop;

	delete[] m_ptrContours;
	delete[] m_ptrCont_Norms;

	if (m_ptrUp != NULL)
	{
		delete[] m_ptrUp;
		m_ptrUp = NULL;
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  CgleBaseExtrusion::PrepareGC()
{
	if (m_bTexEnabled)
	{

		_gle_gc->ncp = m_iContourPoints;
		_gle_gc->contour = m_ptrContours;
		_gle_gc->cont_normal = m_ptrCont_Norms;
		_gle_gc->up = m_ptrUp;
		_gle_gc->npoints = m_iPoints;
		_gle_gc->point_array = (double(*)[3])m_ptrPointArray;
		_gle_gc->color_array = (float(*)[3])m_ptrColorArray;
		_gle_gc->xform_array = m_ptrXforms;

		switch (m_iTexMode & GLE_TEXTURE_STYLE_MASK)
		{

			case GLE_TEXTURE_VERTEX_FLAT:
				_gle_gc->bgn_gen_texture = bgn_z_texgen;
				_gle_gc->v3d_gen_texture = vertex_flat_texgen_v;
				_gle_gc->n3d_gen_texture = 0x0;
				break;

			case GLE_TEXTURE_NORMAL_FLAT:
				_gle_gc->bgn_gen_texture = bgn_z_texgen;
				_gle_gc->v3d_gen_texture = normal_flat_texgen_v;
				_gle_gc->n3d_gen_texture = save_normal;
				break;

			case GLE_TEXTURE_VERTEX_MODEL_FLAT:
				_gle_gc->bgn_gen_texture = bgn_z_texgen;
				_gle_gc->v3d_gen_texture = vertex_flat_model_v;
				_gle_gc->n3d_gen_texture = 0x0;
				break;

			case GLE_TEXTURE_NORMAL_MODEL_FLAT:
				_gle_gc->bgn_gen_texture = bgn_z_texgen;
				_gle_gc->v3d_gen_texture = normal_flat_model_v;
				_gle_gc->n3d_gen_texture = 0x0;
				break;

			case GLE_TEXTURE_VERTEX_CYL:
				_gle_gc->bgn_gen_texture = bgn_z_texgen;
				_gle_gc->v3d_gen_texture = vertex_cylinder_texgen_v;
				_gle_gc->n3d_gen_texture = 0x0;
				break;

			case GLE_TEXTURE_NORMAL_CYL:
				_gle_gc->bgn_gen_texture = bgn_z_texgen;
				_gle_gc->v3d_gen_texture = normal_cylinder_texgen_v;
				_gle_gc->n3d_gen_texture = save_normal;
				break;

			case GLE_TEXTURE_VERTEX_MODEL_CYL:
				_gle_gc->bgn_gen_texture = bgn_z_texgen;
				_gle_gc->v3d_gen_texture = vertex_cylinder_model_v;
				_gle_gc->n3d_gen_texture = 0x0;
				break;

			case GLE_TEXTURE_NORMAL_MODEL_CYL:
				_gle_gc->bgn_gen_texture = bgn_z_texgen;
				_gle_gc->v3d_gen_texture = normal_cylinder_model_v;
				_gle_gc->n3d_gen_texture = 0x0;
				break;

			case GLE_TEXTURE_VERTEX_SPH:
				_gle_gc->bgn_gen_texture = bgn_sphere_texgen;
				_gle_gc->v3d_gen_texture = vertex_sphere_texgen_v;
				_gle_gc->n3d_gen_texture = 0x0;
				break;

			case GLE_TEXTURE_NORMAL_SPH:
				_gle_gc->bgn_gen_texture = bgn_sphere_texgen;
				_gle_gc->v3d_gen_texture = normal_sphere_texgen_v;
				_gle_gc->n3d_gen_texture = save_normal;
				break;

			case GLE_TEXTURE_VERTEX_MODEL_SPH:
				_gle_gc->bgn_gen_texture = bgn_sphere_texgen;
				_gle_gc->v3d_gen_texture = vertex_sphere_model_v;
				_gle_gc->n3d_gen_texture = 0x0;
				break;

			case GLE_TEXTURE_NORMAL_MODEL_SPH:
				_gle_gc->bgn_gen_texture = bgn_sphere_texgen;
				_gle_gc->v3d_gen_texture = normal_sphere_model_v;
				_gle_gc->n3d_gen_texture = 0x0;
				break;

		}
	}
	else
	{

		_gle_gc->ncp = -1;
		_gle_gc->contour = NULL;
		_gle_gc->cont_normal = NULL;
		_gle_gc->up = NULL;
		_gle_gc->npoints = -1;
		_gle_gc->point_array = NULL;
		_gle_gc->color_array = NULL;
		_gle_gc->xform_array = NULL;

		_gle_gc->bgn_gen_texture = 0x0;
		_gle_gc->n3f_gen_texture = 0x0;
		_gle_gc->n3d_gen_texture = 0x0;
		_gle_gc->v3f_gen_texture = 0x0;
		_gle_gc->v3d_gen_texture = 0x0;
		_gle_gc->end_gen_texture = 0x0;
	}
}

//----------------------------------------------------------------------------
void CgleBaseExtrusion::SetExtrusionMode(int style)
{
	m_iJoinStyle = style;
}
//----------------------------------------------------------------------------
int CgleBaseExtrusion::GetExtrusionMode()  const
{
	return(m_iJoinStyle);
}
//----------------------------------------------------------------------------
void CgleBaseExtrusion::SetTextureMode(int mode)
{
	m_iTexMode = mode;
}
//----------------------------------------------------------------------------
int   CgleBaseExtrusion::GetTextureMode() const
{
	return(m_iTexMode);
}
//----------------------------------------------------------------------------
void CgleBaseExtrusion::LoadUpVector(double Up[3])
{
	if (m_ptrUp == NULL)
		m_ptrUp = new double[3];

	for (int i = 0; i < 3; i++)
		m_ptrUp[i] = Up[i];
}
//----------------------------------------------------------------------------
void CgleBaseExtrusion::LoadContourPoint(int Index, double dX,
	double dY)
{

	double alen, ax, ay;
	m_ptrContours[Index][0] = m_dRadius * (dX);
	m_ptrContours[Index][1] = m_dRadius * (dY);
	if (Index != 0)
	{
		ax = m_ptrContours[Index][0] - m_ptrContours[Index - 1][0];
		ay = m_ptrContours[Index][1] - m_ptrContours[Index - 1][1];
		if (!((ax == 0.0) && (ay == 0.0)))
			alen = sqrt(ax*ax + ay*ay);
		else
			alen = 0.0;
		if (alen != 0)
			alen = ((1.0) / (alen));

		m_ptrCont_Norms[Index - 1][0] = (ay *= alen);
		m_ptrCont_Norms[Index - 1][1] = -(ax *= alen);

	}
}
//--------------------------------------------------------------------------
void  CgleBaseExtrusion::UseLitMaterial(bool value)
{
	m_bUseLitMaterial = value;
}
//----------------------------------------------------------------------------
bool CgleBaseExtrusion::IsUsingLitMaterial()  const
{
	return(m_bUseLitMaterial);
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleBaseExtrusion::DrawExtrusion(double Point_Array[][3],
	float Color_Array[][3], bool bTextured)
{
	m_ptrPointArray = NULL;
	m_ptrColorArray = NULL;

	if (bTextured == true)
		m_bTexEnabled = true;

	StatUseLitMat = m_bUseLitMaterial;

	if (Color_Array != NULL)
		(float(*)[3])m_ptrColorArray = Color_Array;

	m_ptrPointArray = &Point_Array[0][0];

	PrepareGC();

	switch (m_iJoinStyle & GLE_JN_MASK)
	{
		case GLE_JN_RAW:
			(void)Extrusion_Raw_Join();
			break;

		case GLE_JN_ANGLE:
			(void)Extrusion_Angle_Join();
			break;

		case GLE_JN_CUT:
		case GLE_JN_ROUND:
			// This routine used for both cut and round styles
			(void)Extrusion_Round_Or_Cut_Join();
			break;

		default:
			break;
	}
	m_bTexEnabled = false;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleBaseExtrusion::DrawWireExtrusion(double Point_Array[][3],
	float Color_Array[][3], bool bTextured)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawExtrusion(Point_Array, Color_Array, bTextured);
	glPopAttrib();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CgleBaseExtrusion::Draw_Fillet_Triangle_N_Norms (double va[3],
													  double vb[3],
													  double vc[3],
													  int face,
													  float front_color[3],
													  double na[3],
													  double nb[3])
{

	if (front_color != NULL)
		SetColor (front_color);

	if (!m_bTexEnabled)
	{
		glBegin (GL_TRIANGLE_STRIP);
		if (m_iJoinStyle & GLE_NORM_FACET)
		{
			glNormal3dv(na);
			if (face)
			{
				glVertex3dv(va);
				glVertex3dv(vb);
			}
			else
			{
				glVertex3dv(vb);
				glVertex3dv(va);
			}
			glVertex3dv(vc);
		}
		else
		{
			if (face)
			{
				glNormal3dv(na);
				glVertex3dv(va);
				glNormal3dv(nb);
				glVertex3dv(vb);
			}
			else
			{
				glNormal3dv(nb);
				glVertex3dv(vb);
				glNormal3dv(na);
				glVertex3dv(va);
				glNormal3dv(nb);
			}
			glVertex3dv(vc);
		}
		glEnd();


	}
	else
	{
		BGNTMESH (-5, 0.0);

		if (m_iJoinStyle & GLE_NORM_FACET)
		{
			N3D (na);

			if (face)
			{
				V3D (va, -1, FILLET);
				V3D (vb, -1, FILLET);
			}
			else
			{
				V3D (vb, -1, FILLET);
				V3D (va, -1, FILLET);
			}
			V3D (vc, -1, FILLET);
		}
		else
		{
			if (face)
			{
				N3D (na);
				V3D (va, -1, FILLET);
				N3D (nb);
				V3D (vb, -1, FILLET);
			}
			else
			{
				N3D (nb);
				V3D (vb, -1, FILLET);
				N3D (na);
				V3D (va, -1, FILLET);
				N3D (nb);
			}
			V3D (vc, -1, FILLET);
		}
		ENDTMESH ();
	}


}			  
//----------------------------------------------------------------------------
void CgleBaseExtrusion::Draw_Segment_Edge_N (double front_contour[][3],
											 double back_contour[][3],
											 double norm_cont[][3])
{

	if (!m_bTexEnabled)
	{
		// draw the tube segment
		glBegin (GL_TRIANGLE_STRIP);

		for (int j = 0; j < m_iContourPoints; j++)
		{
			glNormal3dv(norm_cont[j]);
			glVertex3dv(front_contour[j]);
			glVertex3dv(back_contour[j]);
		}

		if  (m_iJoinStyle & GLE_CONTOUR_CLOSED)

		{
			// connect back up to first point of contour
			glNormal3dv(norm_cont[0]);
			glVertex3dv(front_contour[0]);
			glVertex3dv(back_contour[0]);
		}
		glEnd();
	}
	else
	{
		// draw the tube segment
		BGNTMESH (m_iINext,m_dLen);
		for (int j=0; j<m_iContourPoints; j++)
		{
			N3D (norm_cont[j]);
			V3D (front_contour[j], j, FRONT);
			V3D (back_contour[j], j, BACK);
		}

		if  (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			N3D (norm_cont[0]);
			V3D (front_contour[0], 0, FRONT);
			V3D (back_contour[0], 0, BACK);
		}
		ENDTMESH ();
	}

}

// ============================================================

void CgleBaseExtrusion::Draw_Segment_C_And_Edge_N (double front_contour[][3],
												   double back_contour[][3],
												   double norm_cont[][3])
{

	if (!m_bTexEnabled)
	{
		// draw the tube segment
		glBegin(GL_TRIANGLE_STRIP);
		for (int j=0; j<m_iContourPoints; j++)
		{
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(norm_cont[j]);
			glVertex3dv(front_contour[j]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			glNormal3dv(norm_cont[j]);
			glVertex3dv(back_contour[j]);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)

		{
			// connect back up to first point of contour
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(norm_cont[0]);
			glVertex3dv(front_contour[0]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			glNormal3dv(norm_cont[0]);
			glVertex3dv(back_contour[0]);
		}
		glEnd();
	}
	else
	{

		// draw the tube segment
		BGNTMESH (m_iINext, m_dLen);
		for (int j = 0; j < m_iContourPoints; j++)
		{
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (norm_cont[j]);
			V3D (front_contour[j], j, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			N3D (norm_cont[j]);
			V3D (back_contour[j], j, BACK);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (norm_cont[0]);
			V3D (front_contour[0], 0, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			N3D (norm_cont[0]);
			V3D (back_contour[0], 0, BACK);
		}
		ENDTMESH ();
	}
}
// ============================================================

void CgleBaseExtrusion::Draw_Segment_Facet_N (double front_contour[][3],
											  double back_contour[][3],
											  double norm_cont[][3])
{
	if (!m_bTexEnabled)
	{
		// draw the tube segment
		glBegin(GL_TRIANGLE_STRIP);
		for (int j=0; j<m_iContourPoints-1; j++)
		{
			glNormal3dv(norm_cont[j]);
			glVertex3dv(front_contour[j]);
			glVertex3dv(back_contour[j]);
			glVertex3dv(front_contour[j+1]);
			glVertex3dv(back_contour[j+1]);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			glNormal3dv(norm_cont[m_iContourPoints-1]);
			glVertex3dv(front_contour[m_iContourPoints-1]);
			glVertex3dv(back_contour[m_iContourPoints-1]);
			glVertex3dv(front_contour[0]);
			glVertex3dv(back_contour[0]);
		}

		glEnd();
	}
	else
	{

		// draw the tube segment
		BGNTMESH (m_iINext, m_dLen);
		for (int j=0; j<m_iContourPoints-1; j++)
		{
			N3D (norm_cont[j]);
			V3D (front_contour[j], j, FRONT);
			V3D (back_contour[j], j, BACK);
			V3D (front_contour[j+1], j+1, FRONT);
			V3D (back_contour[j+1], j+1, BACK);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			N3D (norm_cont[m_iContourPoints-1]);
			V3D (front_contour[m_iContourPoints-1], m_iContourPoints-1, FRONT);
			V3D (back_contour[m_iContourPoints-1], m_iContourPoints-1, BACK);
			V3D (front_contour[0], 0, FRONT);
			V3D (back_contour[0], 0, BACK);
		}

		ENDTMESH ();
	}

}

// ============================================================

void CgleBaseExtrusion::Draw_Segment_C_And_Facet_N(double front_contour[][3],
												   double back_contour[][3],
												   double norm_cont[][3])
{
	// Note about this code:
	// At first, when looking at this code, it appears to be a bit bizarre:
	// the N3F() call appears to be repeated multiple times, for no
	// apparent purpose.  It would seem that a performance improvement
	// would be gained by stripping it out. !DONT DO IT!
	// When there are no local lights or viewers, the V3F() subroutine
	// does not trigger a recalculation of the lighting equations.
	// However, we MUST trigger lighting, since otherwise colors come out
	// wrong.  Trigger lighting by doing an N3F call.
	//

	if (!m_bTexEnabled)
	{
		// draw the tube segment
		glBegin(GL_TRIANGLE_STRIP);
		for (int j=0; j<m_iContourPoints-1; j++)
		{
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(norm_cont[j]);
			glVertex3dv(front_contour[j]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			glNormal3dv(norm_cont[j]);
			glVertex3dv(back_contour[j]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(norm_cont[j]);
			glVertex3dv(front_contour[j+1]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			glNormal3dv(norm_cont[j]);
			glVertex3dv(back_contour[j+1]);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)

		{
			// connect back up to first point of contour
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(norm_cont[m_iContourPoints-1]);
			glVertex3dv(front_contour[m_iContourPoints-1]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			glNormal3dv(norm_cont[m_iContourPoints-1]);
			glVertex3dv(back_contour[m_iContourPoints-1]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(norm_cont[m_iContourPoints-1]);
			glVertex3dv(front_contour[0]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			glNormal3dv(norm_cont[m_iContourPoints-1]);
			glVertex3dv(back_contour[0]);
		}

		glEnd();
	}
	else
	{
		// draw the tube segment

		BGNTMESH (m_iINext, m_dLen);
		for (int j=0; j<m_iContourPoints-1; j++)
		{
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (norm_cont[j]);
			V3D (front_contour[j], j, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			N3D (norm_cont[j]);
			V3D (back_contour[j], j, BACK);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (norm_cont[j]);
			V3D (front_contour[j+1], j+1, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			N3D (norm_cont[j]);
			V3D (back_contour[j+1], j+1, BACK);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (norm_cont[m_iContourPoints-1]);
			V3D (front_contour[m_iContourPoints-1], m_iContourPoints-1, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			N3D (norm_cont[m_iContourPoints-1]);
			V3D (back_contour[m_iContourPoints-1], m_iContourPoints-1, BACK);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (norm_cont[m_iContourPoints-1]);
			V3D (front_contour[0], 0, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			N3D (norm_cont[m_iContourPoints-1]);
			V3D (back_contour[0], 0, BACK);
		}

		ENDTMESH ();
	}
}		  
// ============================================================
// ============================================================
//
// This routine draws a segment with normals specified at each end.
//
void CgleBaseExtrusion::Draw_Binorm_Segment_Edge_N (double front_contour[][3],
													double back_contour[][3],
													double front_norm[][3],
													double back_norm[][3])
{

	if (!m_bTexEnabled)
	{
		// draw the tube segment
		glBegin(GL_TRIANGLE_STRIP);
		for (int j=0; j< m_iContourPoints ; j++)
		{
			glNormal3dv(front_norm[j]);
			glVertex3dv(front_contour[j]);
			glNormal3dv(back_norm[j]);
			glVertex3dv(back_contour[j]);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)

		{
			// connect back up to first point of contour
			glNormal3dv(front_norm[0]);
			glVertex3dv(front_contour[0]);
			glNormal3dv(back_norm[0]);
			glVertex3dv(back_contour[0]);
		}
		glEnd();
	}
	else
	{
		// draw the tube segment
		BGNTMESH (m_iINext, m_dLen);
		for (int j = 0; j < m_iContourPoints; j++)
		{
			N3D (front_norm[j]);
			V3D (front_contour[j], j, FRONT);
			N3D (back_norm[j]);
			V3D (back_contour[j], j, BACK);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			N3D (front_norm[0]);
			V3D (front_contour[0], 0, FRONT);
			N3D (back_norm[0]);
			V3D (back_contour[0], 0, BACK);
		}
		ENDTMESH ();
	}
}
// ============================================================
void CgleBaseExtrusion::Draw_Binorm_Segment_C_And_Edge_N(double front_contour[][3],
														 double back_contour[][3],
														 double front_norm[][3],
														 double back_norm[][3])
{
	if (!m_bTexEnabled)
	{
		// draw the tube segment
		glBegin(GL_TRIANGLE_STRIP);
		for (int j=0; j<m_iContourPoints; j++)
		{
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(front_norm[j]);
			glVertex3dv(front_contour[j]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			glNormal3dv(back_norm[j]);
			glVertex3dv(back_contour[j]);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)

		{
			// connect back up to first point of contour
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(front_norm[0]);
			glVertex3dv(front_contour[0]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			glNormal3dv(back_norm[0]);
			glVertex3dv(back_contour[0]);
		}
		glEnd();
	}
	else
	{
		// draw the tube segment
		BGNTMESH (m_iINext, m_dLen);
		for (int j = 0; j < m_iContourPoints; j++)
		{
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (front_norm[j]);
			V3D (front_contour[j], j, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			N3D (back_norm[j]);
			V3D (back_contour[j], j, BACK);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (front_norm[0]);
			V3D (front_contour[0], 0, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			N3D (back_norm[0]);
			V3D (back_contour[0], 0, BACK);
		}
		ENDTMESH ();
	}
}
// ============================================================
//
// This routine draws a piece of the round segment with psuedo-facet
// normals.  I say "psuedo-facet" because the resulting object looks
// much, much better than real facet normals, and is what the  round
// join style was meant to accomplish for face normals.
//
void CgleBaseExtrusion::Draw_Binorm_Segment_Facet_N (double front_contour[][3],
													 double back_contour[][3],
													 double front_norm[][3],
													 double back_norm[][3])
{
	if (!m_bTexEnabled)
	{
		// draw the tube segment
		glBegin(GL_TRIANGLE_STRIP);
		for (int j=0; j<m_iContourPoints-1; j++)
		{
			glNormal3dv(front_norm[j]);
			glVertex3dv(front_contour[j]);

			glNormal3dv(back_norm[j]);
			glVertex3dv(back_contour[j]);

			glNormal3dv(front_norm[j]);
			glVertex3dv(front_contour[j+1]);

			glNormal3dv(back_norm[j]);
			glVertex3dv(back_contour[j+1]);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)

		{
			// connect back up to first point of contour
			glNormal3dv(front_norm[m_iContourPoints-1]);
			glVertex3dv(front_contour[m_iContourPoints-1]);

			glNormal3dv(back_norm[m_iContourPoints-1]);
			glVertex3dv(back_contour[m_iContourPoints-1]);

			glNormal3dv(front_norm[m_iContourPoints-1]);
			glVertex3dv(front_contour[0]);

			glNormal3dv(back_norm[m_iContourPoints-1]);
			glVertex3dv(back_contour[0]);
		}
		glEnd();
	}
	else
	{
		// draw the tube segment
		BGNTMESH (m_iINext, m_dLen);
		for (int j = 0; j< m_iContourPoints - 1; j++)
		{
			N3D (front_norm[j]);
			V3D (front_contour[j], j, FRONT);

			N3D (back_norm[j]);
			V3D (back_contour[j], j, BACK);

			N3D (front_norm[j]);
			V3D (front_contour[j+1], j+1, FRONT);

			N3D (back_norm[j]);
			V3D (back_contour[j+1], j+1, BACK);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			N3D (front_norm[m_iContourPoints-1]);
			V3D (front_contour[m_iContourPoints-1], m_iContourPoints-1, FRONT);

			N3D (back_norm[m_iContourPoints-1]);
			V3D (back_contour[m_iContourPoints-1], m_iContourPoints-1, BACK);

			N3D (front_norm[m_iContourPoints-1]);
			V3D (front_contour[0], 0, FRONT);

			N3D (back_norm[m_iContourPoints-1]);
			V3D (back_contour[0], 0, BACK);
		}
		ENDTMESH ();
	}

}

// ============================================================

void CgleBaseExtrusion::Draw_Binorm_Segment_C_And_Facet_N(
	double front_contour[][3],
	double back_contour[][3],
	double front_norm[][3],
	double back_norm[][3])
{

	if (!m_bTexEnabled)
	{
		// draw the tube segment
		glBegin(GL_TRIANGLE_STRIP);

		for (int j=0; j<m_iContourPoints-1; j++)
		{
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(front_norm[j]);
			glVertex3dv(front_contour[j]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			glNormal3dv(back_norm[j]);
			glVertex3dv(back_contour[j]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(front_norm[j]);
			glVertex3dv(front_contour[j+1]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			glNormal3dv(back_norm[j]);
			glVertex3dv(back_contour[j+1]);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(front_norm[m_iContourPoints-1]);
			glVertex3dv(front_contour[m_iContourPoints-1]);


			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			glNormal3dv(back_norm[m_iContourPoints-1]);
			glVertex3dv(back_contour[m_iContourPoints-1]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			glNormal3dv(front_norm[m_iContourPoints-1]);
			glVertex3dv(front_contour[0]);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			glNormal3dv(back_norm[m_iContourPoints-1]);
			glVertex3dv(back_contour[0]);
		}
		glEnd();
	}
	else
	{
		// draw the tube segment
		BGNTMESH (m_iINext, m_dLen);
		for (int j=0; j<m_iContourPoints-1; j++)
		{
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (front_norm[j]);
			V3D (front_contour[j], j, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			N3D (back_norm[j]);
			V3D (back_contour[j], j, BACK);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (front_norm[j]);
			V3D (front_contour[j+1], j+1, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			N3D (back_norm[j]);
			V3D (back_contour[j+1], j+1, BACK);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (front_norm[m_iContourPoints-1]);
			V3D (front_contour[m_iContourPoints-1], m_iContourPoints-1, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			N3D (back_norm[m_iContourPoints-1]);
			V3D (back_contour[m_iContourPoints-1], m_iContourPoints-1, BACK);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
			N3D (front_norm[m_iContourPoints-1]);
			V3D (front_contour[0], 0, FRONT);

			SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext ][0]);
			N3D (back_norm[m_iContourPoints-1]);
			V3D (back_contour[0], 0, BACK);
		}

		ENDTMESH ();
	}

}

// ============================================================

void CgleBaseExtrusion::Draw_Fillets_And_Join_N_Norms( int ncp,
													  double trimmed_loop[][3],
													  double untrimmed_loop[][3],
													  int is_trimmed[],
													  double bis_origin[3],
													  double bis_vector[3],
													  double normals[][3],
													  float front_color[3],
													  //   float back_color[3],
													  double cut_vector[3],
													  int face,
													  CapCallBack Cap_CallBack)
{
	int istop;
	int icnt, icnt_prev = 0, iloop;
	double sect[3];
	double tmp_vec[3];
	int save_style;
	int was_trimmed = FALSE;


	FNNormCapLoop = new double[(ncp + 3)*6];

	FNNormLoop =  FNNormCapLoop + (ncp+3)*3;

	// if the first point on the contour isn't trimmed, go ahead and
	// drop an edge down to the bisecting plane, (thus starting the
	// join).  (Only need to do this for cut join, its bad if done for
	// round join).
	//
	// But if the first point is trimmed, keep going until one
	// is found that is not trimmed, and start join there.

	icnt = 0;
	iloop = 0;
	if (!is_trimmed[0])
	{
		if (m_iJoinStyle & GLE_JN_CUT)
		{
			VEC_SUM (tmp_vec, trimmed_loop[0], bis_vector);
			INNERSECT (sect,
				bis_origin,
				bis_vector,
				trimmed_loop[0],
				tmp_vec);
			VEC_COPY ( (&FNNormCapLoop[3*iloop]), sect);
			VEC_COPY ( (&FNNormLoop[3*iloop]), normals[0]);
			iloop ++;
		}
		VEC_COPY ( (&FNNormCapLoop[3*iloop]), (trimmed_loop[0]));
		VEC_COPY ( (&FNNormLoop[3*iloop]), normals[0]);
		iloop++;
		icnt_prev = icnt;
		icnt ++;
	}
	else
	{

		// else, loop until an untrimmed point is found
		was_trimmed = TRUE;
		while (is_trimmed[icnt])
		{
			icnt_prev = icnt;
			icnt ++;
			if (icnt >= ncp)
			{
				return;    // oops - everything was trimmed
			}
		}
	}

	// Start walking around the end cap.  Every time the end loop is
	// trimmed, we know we'll need to draw a fillet triangle.  In
	// addition, after every pair of visibility changes, we draw a cap.
	if (m_iJoinStyle & GLE_CONTOUR_CLOSED)

	{
		istop = ncp;
	}
	else
	{
		istop = ncp-1;
	}

	// save the join style, and disable a closed contour.
	// Need to do this so partial contours don't close up.
	save_style = GetExtrusionMode ();
	SetExtrusionMode (save_style & ~GLE_CONTOUR_CLOSED);

	for (; icnt_prev < istop; icnt_prev ++, icnt ++, icnt %= ncp)
	{

		// There are four interesting cases for drawing caps and fillets:
		//    1) this & previous point were trimmed.  Don't do anything,
		//       advance counter.
		//    2) this point trimmed, previous not -- draw fillet, and
		//       draw cap.
		//    3) this point not trimmed, previous one was -- compute
		//       intersection point, draw fillet with it, and save
		//       point for cap contour.
		//    4) this & previous point not trimmed -- save for endcap.
		//

		// Case 1 -- noop, just advance pointers
		if (is_trimmed[icnt_prev] && is_trimmed[icnt])
		{
		}

		// Case 2 --  Hah! first point! compute intersect & draw fillet!
		if (is_trimmed[icnt_prev] && !is_trimmed[icnt])
		{

			// important note: the array "untrimmed" contains valid
			// untrimmed data ONLY when is_trim is TRUE.  Otherwise,
			// only "trim" containes valid data

			// compute intersection
			INNERSECT (sect, 
				bis_origin,
				bis_vector,
				untrimmed_loop[icnt_prev],
				trimmed_loop[icnt]);

			// Draw Fillet
			Draw_Fillet_Triangle_N_Norms (trimmed_loop[icnt_prev],
				trimmed_loop[icnt],
				sect,
				face,
				front_color,
				normals[icnt_prev],
				normals[icnt]);


			VEC_COPY ( (&FNNormCapLoop[3*iloop]), sect);
			VEC_COPY ( (&FNNormLoop[3*iloop]), normals[icnt_prev]);
			iloop ++;
			VEC_COPY ( (&FNNormCapLoop[3*iloop]), (trimmed_loop[icnt]));
			VEC_COPY ( (&FNNormLoop[3*iloop]), normals[icnt]);
			iloop++;
		}

		// Case 3 -- add to collection of points
		if (!is_trimmed[icnt_prev] && !is_trimmed[icnt])
		{
			VEC_COPY ( (&FNNormCapLoop[3*iloop]), (trimmed_loop[icnt]));
			VEC_COPY ( (&FNNormLoop[3*iloop]), normals[icnt]);
			iloop++;
		}

		// Case 4 -- Hah! last point!  draw fillet & draw cap!
		if (!is_trimmed[icnt_prev] && is_trimmed[icnt])
		{
			was_trimmed = TRUE;

			// important note: the array "untrimmed" contains valid
			// untrimmed data ONLY when is_trim is TRUE.  Otherwise,
			// only "trim" containes valid data

			// compute intersection
			INNERSECT (sect,
				bis_origin,
				bis_vector,
				trimmed_loop[icnt_prev],
				untrimmed_loop[icnt]);

			// Draw Fillet
			Draw_Fillet_Triangle_N_Norms (trimmed_loop[icnt_prev],
				trimmed_loop[icnt],
				sect,
				face,
				front_color,
				normals[icnt_prev],
				normals[icnt]);

			VEC_COPY ( (&FNNormCapLoop[3*iloop]), sect);

			// OK, maybe phong normals are wrong, but at least facet
			// normals will come out OK.
			if (m_iJoinStyle & GLE_NORM_FACET)

			{
				VEC_COPY ( (&FNNormLoop[3*iloop]), normals[icnt_prev]);
			}
			else
			{
				VEC_COPY ( (&FNNormLoop[3*iloop]), normals[icnt]);
			}
			iloop ++;

			// draw cap
			if (iloop >= 3)
				Cap_CallBack (iloop,(double (*)[3]) FNNormCapLoop,
				front_color, cut_vector,
				bis_vector,
				(double (*)[3]) FNNormLoop,
				face,(void*)this);

			// reset cap counter
			iloop = 0;
		}
	}

	// now, finish up in the same way that we started.  If the last
	// point of the contour is visible, drop an edge to the bisecting
	// plane, thus finishing the join, and then, draw the join!

	icnt --;  // decrement to make up for loop exit condititons
	icnt += ncp;
	icnt %= ncp;
	if ((!is_trimmed[icnt]) && (iloop >= 2))
	{

		if (m_iJoinStyle & GLE_JN_CUT)

		{
			VEC_SUM (tmp_vec, trimmed_loop[icnt], bis_vector);
			INNERSECT (sect, 
				bis_origin,
				bis_vector,
				trimmed_loop[icnt],
				tmp_vec);
			VEC_COPY ( (&FNNormCapLoop[3*iloop]), sect);
			VEC_COPY ( (&FNNormLoop[3*iloop]), normals[icnt]);
			iloop ++;
		}

		// if nothing was ever trimmed, then we want to draw the
		// cap the way the user asked for it -- closed or not closed.
		// Therefore, reset the closure flag to its original state.
		//
		if (!was_trimmed)
		{
			SetExtrusionMode (save_style);
		}

		// draw cap
		Cap_CallBack (iloop,
			(double (*)[3]) FNNormCapLoop,
			front_color,
			cut_vector,
			bis_vector,
			(double (*)[3]) FNNormLoop,
			face, (void*)this);
	}

	// rest to the saved style
	SetExtrusionMode (save_style);
	delete[] FNNormCapLoop;
}													   
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  CgleBaseExtrusion::Null_Cap_Callback(int iloop,double cap[][3],
										   float face_color[3], double cut_vector[3],
										   double bisect_vector[3], double norms[][3],
										   int frontwards, void* clOwner)
{
}
//----------------------------------------------------------------------------
void CgleBaseExtrusion::Draw_Round_Style_Cap_Callback (int ncp,double cap[][3],
													   float face_color[3],
													   double cut[3],
													   double bi[3],
													   double norms[][3],
													   int frontwards, void* clOwner)
{



	double axis[3];
	double xycut[3];
	double theta;
	double *last_contour, *next_contour;
	double *last_norm, *next_norm;
	double *cap_z;
	double *tmp;
	int i, j, k;
	double m[4][4];

	if (face_color != NULL)
		SetColor (face_color);

	// ------------ start setting up rotation matrix -------------
	// if the cut vector is NULL (this should only occur in
	// a degenerate case), then we can't draw anything. return.
	if (cut == NULL)
		return;

	// make sure that the cut vector points inwards
	if (cut[2] > 0.0)
		VEC_SCALE (cut, -1.0, cut);

	// make sure that the bi vector points outwards
	if (bi[2] < 0.0)
		VEC_SCALE (bi, -1.0, bi);

	// determine the axis we are to rotate about to get bi-contour.
	// Note that the axis will always lie in the x-y plane
	VEC_CROSS_PRODUCT (axis, cut, bi);

	// reverse the cut vector for the back cap --
	// need to do this to get angle right
	if (!frontwards)
		VEC_SCALE (cut, -1.0, cut);

	// get angle to rotate by -- arccos of dot product of cut with cut
	// projected into the x-y plane
	xycut [0] = 0.0;
	xycut [1] = 0.0;
	xycut [2] = 1.0;
	VEC_PERP (xycut, cut, xycut);
	VEC_NORMALIZE (xycut);
	VEC_DOT_PRODUCT (theta, xycut, cut);

	theta = acos (theta);

	// we'll tesselate round joins into a number of teeny pieces
	theta /= (double) __ROUND_TESS_PIECES;

	// get the matrix
	urot_axis_d (m, theta, axis);

	// ------------ done setting up rotation matrix -------------

	((CgleBaseExtrusion*)clOwner)->FMallocedArea = new char[104*ncp];
	last_contour = (double *) ((CgleBaseExtrusion*)clOwner)->FMallocedArea;
	next_contour = last_contour +  3*ncp;
	cap_z = next_contour + 3*ncp;
	last_norm = cap_z + ncp;
	next_norm = last_norm + 3*ncp;

	// make first copy of contour
	if (frontwards)
	{
		for (j=0; j<ncp; j++)
		{
			last_contour[3*j] = cap[j][0];
			last_contour[3*j+1] = cap[j][1];
			last_contour[3*j+2] = cap_z[j] = cap[j][2];
		}

		if (norms != NULL)
		{
			for (j=0; j<ncp; j++)
			{
				VEC_COPY ((&last_norm[3*j]), norms[j]);
			}
		}
	}
	else
	{
		// in order for backfacing polygon removal to work correctly, have
		// to have the sense in which the joins are drawn to be reversed
		// for the back cap.  This can be done by reversing the order of
		// the contour points.  Normals are a bit trickier, since the
		// reversal is off-by-one for facet normals as compared to edge
		// normals.
		for (j=0; j<ncp; j++)
		{
			k = ncp - j - 1;
			last_contour[3*k] = cap[j][0];
			last_contour[3*k+1] = cap[j][1];
			last_contour[3*k+2] = cap_z[k] = cap[j][2];
		}

		if (norms != NULL)
		{
			if (((CgleBaseExtrusion*)clOwner)->m_iJoinStyle & GLE_NORM_FACET)

			{
				for (j=0; j<ncp-1; j++)
				{
					k = ncp - j - 2;
					VEC_COPY ((&last_norm[3*k]), norms[j]);
				}
			}
			else
			{
				for (j=0; j<ncp; j++)
				{
					k = ncp - j - 1;
					VEC_COPY ((&last_norm[3*k]), norms[j]);
				}
			}
		}
	}

	// &&&&&&&&&&&&&& start drawing cap &&&&&&&&&&&&&

	for (i=0; i<__ROUND_TESS_PIECES; i++)
	{
		for (j=0; j<ncp; j++)
		{
			next_contour [3*j+2] -= cap_z[j];
			last_contour [3*j+2] -= cap_z[j];
			MAT_DOT_VEC_3X3 ( (&next_contour[3*j]), m, (&last_contour[3*j]));
			next_contour [3*j+2] += cap_z[j];
			last_contour [3*j+2] += cap_z[j];
		}

		if (norms != NULL)
		{
			for (j=0; j<ncp; j++)
			{
				MAT_DOT_VEC_3X3 ( (&next_norm[3*j]), m, (&last_norm[3*j]));
			}
		}

		int temp = ((CgleBaseExtrusion*)clOwner)->m_iINext;     // fucked up! need to buffer m_dLen too!
		((CgleBaseExtrusion*)clOwner)->m_iINext = 0;        // have to buffer these two values. Everywhere else
		int tempcp = ((CgleBaseExtrusion*)clOwner)->m_iContourPoints;// ncp and inext equal the globals, but
		((CgleBaseExtrusion*)clOwner)->m_iContourPoints = ncp;             // not here.
		double templen = ((CgleBaseExtrusion*)clOwner)->m_dLen;
		((CgleBaseExtrusion*)clOwner)->m_dLen = 0.0;

		// OK, now render it all

		if (((CgleBaseExtrusion*)clOwner)->m_iJoinStyle & GLE_NORM_FACET)
		{

			((CgleBaseExtrusion*)clOwner)->Draw_Binorm_Segment_Facet_N ((gleVector *) next_contour,
				(gleVector *) last_contour,
				(gleVector *) next_norm,
				(gleVector *) last_norm);
		}
		else
		{
			((CgleBaseExtrusion*)clOwner)->Draw_Binorm_Segment_Edge_N ((gleVector *) next_contour,
				(gleVector *) last_contour,
				(gleVector *) next_norm,
				(gleVector *) last_norm);
		}
		((CgleBaseExtrusion*)clOwner)->m_iINext = temp;
		((CgleBaseExtrusion*)clOwner)->m_iContourPoints = tempcp;
		((CgleBaseExtrusion*)clOwner)->m_dLen = templen;


		// swap contours
		tmp = next_contour;
		next_contour = last_contour;
		last_contour = tmp;

		tmp = next_norm;
		next_norm = last_norm;
		last_norm = tmp;
	}
	delete[] ((CgleBaseExtrusion*)clOwner)->FMallocedArea;
	// &&&&&&&&&&&&&& end drawing cap &&&&&&&&&&&&&
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CgleBaseExtrusion::Draw_Cut_Style_Cap_Callback (int iloop,double cap[][3],
													 float face_color[3], double cut_vector[3],
													 double bisect_vector[3], double norms[][3],
													 int frontwards, void* clOwner)
{

	int i;


	gluTessCallback (((CgleBaseExtrusion*)clOwner)->m_objTess, 
		GLU_TESS_BEGIN, (void(__stdcall*)())glBegin);

	gluTessCallback (((CgleBaseExtrusion*)clOwner)->m_objTess, GLU_TESS_VERTEX,
		(void(__stdcall*)()) glVertex3dv);

	gluTessCallback (((CgleBaseExtrusion*)clOwner)->m_objTess, GLU_TESS_END, 
		(void(__stdcall*)())glEnd);

	if (face_color != NULL)
		SetColor (face_color);

	if (frontwards)
	{
		// if lighting is on, specify the endcap normal
		if (cut_vector != NULL)
		{
			// if normal pointing in wrong direction, flip it.
			if (cut_vector[2] < 0.0)
				VEC_SCALE (cut_vector, -1.0, cut_vector);

			N3D (cut_vector);
		}
		gluTessBeginPolygon (((CgleBaseExtrusion*)clOwner)->m_objTess,NULL);
		gluTessBeginContour (((CgleBaseExtrusion*)clOwner)->m_objTess);

		for (i=0; i<iloop; i++)
		{
			gluTessVertex (((CgleBaseExtrusion*)clOwner)->m_objTess, cap[i], cap[i]);
		}
		gluTessEndContour (((CgleBaseExtrusion*)clOwner)->m_objTess);
		gluTessEndPolygon (((CgleBaseExtrusion*)clOwner)->m_objTess);
	}
	else
	{
		// if lighting is on, specify the endcap normal

		if (cut_vector != NULL)
		{
			// if normal pointing in wrong direction, flip it.
			if (cut_vector[2] > 0.0)
			{
				VEC_SCALE (cut_vector, -1.0, cut_vector);
			}
			N3D (cut_vector);
		}
		// the sense of the loop is reversed for backfacing culling
		gluTessBeginPolygon (((CgleBaseExtrusion*)clOwner)->m_objTess,NULL);
		gluTessBeginContour (((CgleBaseExtrusion*)clOwner)->m_objTess);
		for (i=iloop-1; i>-1; i--)
		{
			gluTessVertex (((CgleBaseExtrusion*)clOwner)->m_objTess, cap[i], cap[i]);
		}
		gluTessEndContour (((CgleBaseExtrusion*)clOwner)->m_objTess);
		gluTessEndPolygon (((CgleBaseExtrusion*)clOwner)->m_objTess);
	}
}			   
//----------------------------------------------------------------------------
void CgleBaseExtrusion::Draw_Angle_Style_Back_Cap ( double bi[3],
												   double point_array[][3])
{
	int j;

	if (bi[2] > 0.0)
	{
		VEC_SCALE (bi, -1.0, bi);
	}

	N3D(bi);

	gluTessCallback (m_objTess, GLU_TESS_BEGIN, (void(__stdcall*)())glBegin);

	gluTessCallback (m_objTess, GLU_TESS_VERTEX,
		(void(__stdcall*)()) glVertex3dv);

	gluTessCallback (m_objTess, GLU_TESS_END, (void(__stdcall*)())glEnd);

	gluTessBeginPolygon (m_objTess,NULL);
	gluTessBeginContour (m_objTess);

	for (j=m_iContourPoints-1; j>=0; j--)
	{
		gluTessVertex (m_objTess, point_array[j], point_array[j]);
	}
	gluTessEndContour (m_objTess);
	gluTessEndPolygon (m_objTess);
}

//----------------------------------------------------------------------------
// ============================================================
void CgleBaseExtrusion::Draw_Angle_Style_Front_Cap (double bi[3],// biscetor
													double point_array[][3])
{

	if (bi[2] < 0.0)
		VEC_SCALE (bi, -1.0, bi);


	N3D(bi);

	gluTessCallback (m_objTess, GLU_TESS_BEGIN,(void(__stdcall*)()) glBegin);

	gluTessCallback (m_objTess, GLU_TESS_VERTEX,
		(void(__stdcall*)()) glVertex3dv);

	gluTessCallback (m_objTess, GLU_TESS_END, (void(__stdcall*)())glEnd);

	gluTessBeginPolygon (m_objTess,NULL);
	gluTessBeginContour (m_objTess);

	for (int j=0; j<m_iContourPoints; j++)
	{
		gluTessVertex (m_objTess, point_array[j], point_array[j]);
	}
	gluTessEndContour (m_objTess);
	gluTessEndPolygon (m_objTess);
}			   
//----------------------------------------------------------------------------
// ============================================================
// This routine draws a segment of raw-join-style tubing.
// Essentially, we assume that the proper transformations have already
// been performed to properly orient the tube segment -- our only task
// left is to render
// C_AND_FACET_N -- DRAW FACET NORMALS AND COLORS

void CgleBaseExtrusion::Draw_Raw_Segment_C_And_Facet_N ()

{

	if (!m_bTexEnabled)
	{
		double point[3];
		double norm[3];

		// draw the tube segment
		norm [2] = 0.0;
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < m_iContourPoints-1; j++)
		{
			// facet normals require one normal per four vertices;
			// However, we have to respecify normal each time at each vertex
			// so that the lighting equation gets triggered.  (V3F does NOT
			// automatically trigger the lighting equations -- it only
			// triggers when there is a local light)

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

			norm [0] = m_ptrCont_Norms[j][0];
			norm [1] = m_ptrCont_Norms[j][1];
			glNormal3dv(norm);

			point [0] = m_ptrContours[j][0];
			point [1] = m_ptrContours[j][1];
			point [2] = 0.0;
			glVertex3dv(point);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			glNormal3dv(norm);
			point [2] = - m_dLen;
			glVertex3dv(point);



			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);
			glNormal3dv(norm);

			point [0] = m_ptrContours[j+1][0];
			point [1] = m_ptrContours[j+1][1];
			point [2] = 0.0;
			glVertex3dv(point);


			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			glNormal3dv(norm);
			point [2] = - m_dLen;
			glVertex3dv(point);

		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)

		{
			// connect back up to first point of m_ptrContours
			point [0] = m_ptrContours[m_iContourPoints-1][0];
			point [1] = m_ptrContours[m_iContourPoints-1][1];
			point [2] = 0.0;
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

			norm [0] = m_ptrCont_Norms[m_iContourPoints-1][0];
			norm [1] = m_ptrCont_Norms[m_iContourPoints-1][1];
			glNormal3dv(norm);
			glVertex3dv(point);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			glNormal3dv(norm);

			point [2] = - m_dLen;
			glVertex3dv(point);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

			norm [0] = m_ptrCont_Norms[0][0];
			norm [1] = m_ptrCont_Norms[0][1];
			glNormal3dv(norm);

			point [0] = m_ptrContours[0][0];
			point [1] = m_ptrContours[0][1];
			point [2] = 0.0;
			glVertex3dv(point);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			glNormal3dv(norm);

			point [2] = - m_dLen;
			glVertex3dv(point);
		}

		glEnd();

		// draw the endcaps, if the join style calls for it

		if (m_iJoinStyle & GLE_JN_CAP)

		{

			// draw the front cap
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);
			norm [0] = norm [1] = 0.0;
			norm [2] = 1.0;
			glNormal3dv(norm);
			Draw_Raw_Style_End_Cap ( m_ptrContours,
				0.0, TRUE);

			// draw the back cap
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			norm [2] = -1.0;
			glNormal3dv(norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				-m_dLen, FALSE);
		}

	}
	else
	{
		double point[3];
		double norm[3];

		// draw the tube segment
		norm [2] = 0.0;
		BGNTMESH (m_iINext, m_dLen);
		for (int j = 0; j < m_iContourPoints-1; j++)
		{
			// facet normals require one normal per four vertices;
			// However, we have to respecify normal each time at each vertex
			// so that the lighting equation gets triggered.  (V3F does NOT
			// automatically trigger the lighting equations -- it only
			// triggers when there is a local light)

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

			norm [0] = m_ptrCont_Norms[j][0];
			norm [1] = m_ptrCont_Norms[j][1];
			N3D (norm);

			point [0] = m_ptrContours[j][0];
			point [1] = m_ptrContours[j][1];
			point [2] = 0.0;
			V3D (point, j, FRONT);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			N3D (norm);
			point [2] = - m_dLen;
			V3D (point, j, BACK);


			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);
			N3D (norm);

			point [0] = m_ptrContours[j+1][0];
			point [1] = m_ptrContours[j+1][1];
			point [2] = 0.0;
			V3D (point, j+1, FRONT);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			N3D (norm);
			point [2] = - m_dLen;
			V3D (point, j+1, BACK);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of contour
			point [0] = m_ptrContours[m_iContourPoints-1][0];
			point [1] = m_ptrContours[m_iContourPoints-1][1];
			point [2] = 0.0;
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

			norm [0] = m_ptrCont_Norms[m_iContourPoints-1][0];
			norm [1] = m_ptrCont_Norms[m_iContourPoints-1][1];
			N3D (norm);
			V3D (point, m_iContourPoints-1, FRONT);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			N3D (norm);

			point [2] = - m_dLen;
			V3D (point, m_iContourPoints-1, BACK);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

			norm [0] = m_ptrCont_Norms[0][0];
			norm [1] = m_ptrCont_Norms[0][1];
			N3D (norm);

			point [0] = m_ptrContours[0][0];
			point [1] = m_ptrContours[0][1];
			point [2] = 0.0;
			V3D (point, 0, FRONT);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			N3D (norm);

			point [2] = - m_dLen;
			V3D (point, 0, BACK);
		}

		ENDTMESH ();

		// draw the endcaps, if the join style calls for it
		if (m_iJoinStyle & GLE_JN_CAP)
		{

			// draw the front cap
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);
			norm [0] = norm [1] = 0.0;
			norm [2] = 1.0;
			N3D (norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				0.0, TRUE);

			// draw the back cap
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			norm [2] = -1.0;
			N3D (norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				-m_dLen, FALSE);
		}
	}
}
//----------------------------------------------------------------------------
// ============================================================
// This routine draws a segment of raw-join-style tubing.
// Essentially, we assume that the proper transformations have already
// been performed to properly orient the tube segment -- our only task
// left is to render
// FACET_N -- DRAW ONLY FACET NORMALS

void CgleBaseExtrusion::Draw_Raw_Segment_Facet_N ()

{

	if (!m_bTexEnabled)
	{

		double point[3];
		double norm[3];

		// draw the tube segment
		norm [2] = 0.0;
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < m_iContourPoints-1; j++)
		{
			// facet normals require one normal per four vertices
			norm [0] = m_ptrCont_Norms[j][0];
			norm [1] = m_ptrCont_Norms[j][1];
			glNormal3dv(norm);

			point [0] = m_ptrContours[j][0];
			point [1] = m_ptrContours[j][1];
			point [2] = 0.0;
			glVertex3dv(point);

			point [2] = - m_dLen;
			glVertex3dv(point);

			point [0] = m_ptrContours[j+1][0];
			point [1] = m_ptrContours[j+1][1];
			point [2] = 0.0;
			glVertex3dv(point);

			point [2] = - m_dLen;
			glVertex3dv(point);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)

		{
			// connect back up to first point of m_ptrContours
			norm [0] = m_ptrCont_Norms[m_iContourPoints-1][0];
			norm [1] = m_ptrCont_Norms[m_iContourPoints-1][1];
			glNormal3dv(norm);

			point [0] = m_ptrContours[m_iContourPoints-1][0];
			point [1] = m_ptrContours[m_iContourPoints-1][1];
			point [2] = 0.0;
			glVertex3dv(point);

			point [2] = - m_dLen;
			glVertex3dv(point);

			point [0] = m_ptrContours[0][0];
			point [1] = m_ptrContours[0][1];
			point [2] = 0.0;
			glVertex3dv(point);

			point [2] = - m_dLen;
			glVertex3dv(point);
		}

		glEnd();

		// draw the endcaps, if the join style calls for it
		if (m_iJoinStyle & GLE_JN_CAP)

		{

			// draw the front cap
			norm [0] = norm [1] = 0.0;
			norm [2] = 1.0;
			glNormal3dv(norm);
			Draw_Raw_Style_End_Cap ( m_ptrContours,
				0.0, TRUE);

			// draw the back cap
			norm [2] = -1.0;
			glNormal3dv(norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				-m_dLen, FALSE);
		}
	}
	else
	{

		double point[3];
		double norm[3];

		// draw the tube segment
		norm [2] = 0.0;
		BGNTMESH (m_iINext, m_dLen);

		for (int j = 0; j < m_iContourPoints-1; j++)
		{
			// facet normals require one normal per four vertices
			norm [0] = m_ptrCont_Norms[j][0];
			norm [1] = m_ptrCont_Norms[j][1];
			N3D (norm);

			point [0] = m_ptrContours[j][0];
			point [1] = m_ptrContours[j][1];
			point [2] = 0.0;
			V3D (point, j, FRONT);

			point [2] = - m_dLen;
			V3D (point, j, BACK);

			point [0] = m_ptrContours[j+1][0];
			point [1] = m_ptrContours[j+1][1];
			point [2] = 0.0;
			V3D (point, j+1, FRONT);

			point [2] = - m_dLen;
			V3D (point, j+1, BACK);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			//connect back up to first point of m_ptrContours
			norm [0] = m_ptrCont_Norms[m_iContourPoints-1][0];
			norm [1] = m_ptrCont_Norms[m_iContourPoints-1][1];
			N3D (norm);

			point [0] = m_ptrContours[m_iContourPoints-1][0];
			point [1] = m_ptrContours[m_iContourPoints-1][1];
			point [2] = 0.0;
			V3D (point, m_iContourPoints-1, FRONT);

			point [2] = - m_dLen;
			V3D (point, m_iContourPoints-1, BACK);

			point [0] = m_ptrContours[0][0];
			point [1] = m_ptrContours[0][1];
			point [2] = 0.0;
			V3D (point, 0, FRONT);

			point [2] = - m_dLen;
			V3D (point, 0, BACK);
		}

		ENDTMESH ();

		// draw the endcaps, if the join style calls for it
		if (m_iJoinStyle & GLE_JN_CAP)
		{
			// draw the front cap
			norm [0] = norm [1] = 0.0;
			norm [2] = 1.0;
			N3D (norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				0.0, TRUE);

			// draw the back cap
			norm [2] = -1.0;
			N3D (norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				-m_dLen, FALSE);
		}
	}
}
//----------------------------------------------------------------------------

// ============================================================
// This routine draws a segment of raw-join-style tubing.
// Essentially, we assume that the proper transformations have already
// been performed to properly orient the tube segment -- our only task
// left is to render
// C_AND_EDGE_N -- DRAW EDGE NORMALS AND COLORS

void CgleBaseExtrusion::Draw_Raw_Segment_C_And_Edge_N ()
{
	if (!m_bTexEnabled)
	{

		double point[3];
		double norm[3];

		// draw the tube segment
		norm [2] = 0.0;
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < m_iContourPoints; j++)
		{
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

			norm [0] = m_ptrCont_Norms[j][0];
			norm [1] = m_ptrCont_Norms[j][1];
			glNormal3dv(norm);

			point [0] = m_ptrContours[j][0];
			point [1] = m_ptrContours[j][1];
			point [2] = 0.0;
			glVertex3dv(point);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			glNormal3dv(norm);

			point [2] = - m_dLen;
			glVertex3dv(point);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)

		{
			// connect back up to first point of m_ptrContours
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

			norm [0] = m_ptrCont_Norms[0][0];
			norm [1] = m_ptrCont_Norms[0][1];
			glNormal3dv(norm);

			point [0] = m_ptrContours[0][0];
			point [1] = m_ptrContours[0][1];
			point [2] = 0.0;
			glVertex3dv(point);


			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			norm [0] = m_ptrCont_Norms[0][0];
			norm [1] = m_ptrCont_Norms[0][1];
			glNormal3dv(norm);

			point [2] = - m_dLen;
			glVertex3dv(point);
		}

		glEnd();

		// draw the endcaps, if the join style calls for it
		if (m_iJoinStyle & GLE_JN_CAP)

		{
			// draw the front cap
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);
			norm [0] = norm [1] = 0.0;
			norm [2] = 1.0;
			glNormal3dv(norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				0.0, TRUE);

			// draw the back cap
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			norm [2] = -1.0;
			glNormal3dv(norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				-m_dLen, FALSE);
		}

	}
	else
	{
		double point[3];
		double norm[3];

		// draw the tube segment
		norm [2] = 0.0;
		BGNTMESH (m_iINext, m_dLen);

		for (int j = 0; j < m_iContourPoints; j++)
		{
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

			norm [0] = m_ptrCont_Norms[j][0];
			norm [1] = m_ptrCont_Norms[j][1];
			N3D (norm);

			point [0] = m_ptrContours[j][0];
			point [1] = m_ptrContours[j][1];
			point [2] = 0.0;
			V3D (point, j, FRONT);

			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			N3D (norm);

			point [2] = - m_dLen;
			V3D (point, j, BACK);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of m_ptrContours
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

			norm [0] = m_ptrCont_Norms[0][0];
			norm [1] = m_ptrCont_Norms[0][1];
			N3D (norm);

			point [0] = m_ptrContours[0][0];
			point [1] = m_ptrContours[0][1];
			point [2] = 0.0;
			V3D (point, 0, FRONT);


			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			norm [0] = m_ptrCont_Norms[0][0];
			norm [1] = m_ptrCont_Norms[0][1];
			N3D (norm);

			point [2] = - m_dLen;
			V3D (point, 0, BACK);
		}

		ENDTMESH ();

		// draw the endcaps, if the join style calls for it
		if (m_iJoinStyle & GLE_JN_CAP)
		{
			// draw the front cap
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);
			norm [0] = norm [1] = 0.0;
			norm [2] = 1.0;
			N3D (norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				0.0, TRUE);

			// draw the back cap
			SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
			norm [2] = -1.0;
			N3D (norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				-m_dLen, FALSE);
		}
	}
}

//----------------------------------------------------------------------------
// ============================================================
// This routine draws a segment of raw-join-style tubing.
// Essentially, we assume that the proper transformations have already
// been performed to properly orient the tube segment -- our only task
// left is to render
// EDGE_N -- DRAW ONLY EDGE NORMALS

void CgleBaseExtrusion::Draw_Raw_Segment_Edge_N()

{
	if (!m_bTexEnabled)
	{
		double point[3];
		double norm[3];

		// draw the tube segment
		norm [2] = 0.0;
		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < m_iContourPoints; j++)
		{
			norm [0] = m_ptrCont_Norms[j][0];
			norm [1] = m_ptrCont_Norms[j][1];
			glNormal3dv(norm);

			point [0] = m_ptrContours[j][0];
			point [1] = m_ptrContours[j][1];
			point [2] = 0.0;
			glVertex3dv(point);

			point [2] = - m_dLen;
			glVertex3dv(point);
		}

		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of m_ptrContours
			norm [0] = m_ptrCont_Norms[0][0];
			norm [1] = m_ptrCont_Norms[0][1];
			norm [2] = 0.0;
			glNormal3dv(norm);

			point [0] = m_ptrContours[0][0];
			point [1] = m_ptrContours[0][1];
			point [2] = 0.0;
			glVertex3dv(point);

			point [2] = - m_dLen;
			glVertex3dv(point);
		}

		glEnd();

		// draw the endcaps, if the join style calls for it
		if (m_iJoinStyle & GLE_JN_CAP)

		{
			// draw the front cap
			norm [0] = norm [1] = 0.0;
			norm [2] = 1.0;
			glNormal3dv(norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				0.0, TRUE);

			// draw the back cap
			norm [2] = -1.0;
			glNormal3dv(norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				-m_dLen, FALSE);
		}
	}
	else
	{

		double point[3];
		double norm[3];

		// draw the tube segment
		norm [2] = 0.0;
		BGNTMESH (m_iINext, m_dLen);
		for (int j = 0; j < m_iContourPoints; j++)
		{
			norm [0] = m_ptrCont_Norms[j][0];
			norm [1] = m_ptrCont_Norms[j][1];
			N3D (norm);

			point [0] = m_ptrContours[j][0];
			point [1] = m_ptrContours[j][1];
			point [2] = 0.0;
			V3D (point, j, FRONT);

			point [2] = - m_dLen;
			V3D (point, j, BACK);
		}
		if (m_iJoinStyle & GLE_CONTOUR_CLOSED)
		{
			// connect back up to first point of m_ptrContours
			norm [0] = m_ptrCont_Norms[0][0];
			norm [1] = m_ptrCont_Norms[0][1];
			norm [2] = 0.0;
			N3D (norm);

			point [0] = m_ptrContours[0][0];
			point [1] = m_ptrContours[0][1];
			point [2] = 0.0;
			V3D (point, 0, FRONT);

			point [2] = - m_dLen;
			V3D (point, 0, BACK);
		}

		ENDTMESH ();

		// draw the endcaps, if the join style calls for it
		if (m_iJoinStyle & GLE_JN_CAP)
		{

			// draw the front cap
			norm [0] = norm [1] = 0.0;
			norm [2] = 1.0;
			N3D (norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				0.0, TRUE);

			// draw the back cap
			norm [2] = -1.0;
			N3D (norm);
			Draw_Raw_Style_End_Cap (m_ptrContours,
				-m_dLen, FALSE);
		}
	}
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// ============================================================
// This routine does what it says: It draws a clockwise cap
// from a 3D contour loop list
//
void CgleBaseExtrusion::Draw_Back_Contour_Cap (double contour[][3])
{
	gluTessCallback (m_objTess, GLU_TESS_BEGIN, (void(__stdcall*)())glBegin);

	gluTessCallback (m_objTess, GLU_TESS_VERTEX,
		(void(__stdcall*)()) glVertex3dv);

	gluTessCallback (m_objTess, GLU_TESS_END, (void(__stdcall*)())glEnd);

	gluTessBeginPolygon (m_objTess,NULL);
	gluTessBeginContour (m_objTess);

	// draw the end cap
	// draw the loop clockwise for the back cap
	// the sense of the loop is reversed for backfacing culling
	for (int j = m_iContourPoints-1; j>-1; j--)
	{
		gluTessVertex (m_objTess, contour[j], contour[j]);
	}
	gluTessEndContour (m_objTess);
	gluTessEndPolygon (m_objTess);
}

// ============================================================
// This routine does what it says: It draws a counter-clockwise cap
// from a 3D contour loop list
//
//----------------------------------------------------------------------------
void CgleBaseExtrusion::Draw_Front_Contour_Cap ( double contour[][3])
{


	gluTessCallback (m_objTess, GLU_TESS_BEGIN, (void(__stdcall*)())glBegin);

	gluTessCallback (m_objTess, GLU_TESS_VERTEX,
		(void(__stdcall*)()) glVertex3dv);

	gluTessCallback (m_objTess, GLU_TESS_END, (void(__stdcall*)())glEnd);

	gluTessBeginPolygon (m_objTess,NULL);
	gluTessBeginContour (m_objTess);


	for (int j = 0; j<m_iContourPoints; j++)
	{
		gluTessVertex (m_objTess, contour[j], contour[j]);
	}
	gluTessEndContour (m_objTess);
	gluTessEndPolygon (m_objTess);
}
//----------------------------------------------------------------------------
void CgleBaseExtrusion::Up_Sanity_Check (double up[3])	
{
	int i;
	double len;
	double diff[3];

	// now, right off the bat, we should make sure that the up vector
	// is in fact perpendicular to the polyline direction
	VEC_DIFF (diff,  ((double(*)[3])m_ptrPointArray)[1],
		((double(*)[3])m_ptrPointArray)[0]);
	VEC_LENGTH (len, diff);
	if (len == 0.0)
	{
		// This error message should go through "official" error interface

		// fprintf (stderr, "Extrusion: Warning: initial segment zero length \n");


		// loop till we find something that ain't of zero length
		for (i=1; i<m_iPoints-2; i++)
		{
			VEC_DIFF (diff, ((double(*)[3])m_ptrPointArray)[i+1],
				((double(*)[3])m_ptrPointArray)[i]);
			VEC_LENGTH (len, diff);
			if (len != 0.0) break;
		}
	}

	// normalize diff to be of unit length
	len = 1.0 / len;
	VEC_SCALE (diff, len, diff);

	// we want to take only perpendicular component of up w.r.t. the
	// initial segment
	VEC_PERP (up, up, diff);

	VEC_LENGTH (len, up);
	if (len == 0.0)
	{
		// This error message should go through "official" error interface
		//  fprintf (stderr, "Extrusion: Warning: ");
		//  fprintf (stderr, "contour up vector parallel to tubing direction \n");

		// do like the error message says ...
		VEC_COPY (up, diff);
	}


}
//----------------------------------------------------------------------------
void CgleBaseExtrusion::Draw_Raw_Style_End_Cap (double contour[][2],
												double zval,// where to draw cap
												int frontwards)// front or back cap
{

	FDRSECPoints = new double[3*m_iContourPoints];

	gluTessCallback (m_objTess, GLU_TESS_BEGIN, (void(__stdcall*)())glBegin);

	gluTessCallback (m_objTess, GLU_TESS_VERTEX,
		(void(__stdcall*)()) glVertex3dv);

	gluTessCallback (m_objTess, GLU_TESS_END, (void(__stdcall*)())glEnd);

	gluTessBeginPolygon (m_objTess, NULL);
	gluTessBeginContour (m_objTess);

	// draw the loop counter clockwise for the front cap
	if (frontwards)
	{
		for (int j=0; j<m_iContourPoints; j++)
		{
			FDRSECPoints [3*j] = contour[j][0];
			FDRSECPoints [3*j+1] = contour[j][1];
			FDRSECPoints [3*j+2] = zval;
			gluTessVertex (m_objTess, &FDRSECPoints[3*j], &FDRSECPoints[3*j]);
		}

	}
	else
	{
		// the sense of the loop is reversed for backfacing culling
		for (int j=m_iContourPoints-1; j>-1; j--)
		{
			FDRSECPoints [3*j] = contour[j][0];
			FDRSECPoints [3*j+1] = contour[j][1];
			FDRSECPoints [3*j+2] = zval;
			gluTessVertex (m_objTess, &FDRSECPoints[3*j], &FDRSECPoints[3*j]);
		}
	}

	gluTessEndContour (m_objTess);
	gluTessEndPolygon (m_objTess);

	delete[] FDRSECPoints;	   
}										  
//----------------------------------------------------------------------------
// This routine draws "cut" style extrusions.
void CgleBaseExtrusion::Extrusion_Round_Or_Cut_Join()
{
	int i, j;
	int inextnext;

	double m[4][4];
	double GLE_len, seg_len;
	double diff[3];
	double bi_0[3], bi_1[3];		// bisecting plane
	double bisector_0[3], bisector_1[3];		// bisecting plane
	double cut_0[3], cut_1[3];	// cutting planes
	double lcut_0[3], lcut_1[3];	// cutting planes
	int valid_cut_0, valid_cut_1;	// flag -- cut vector is valid
	double end_point_0[3], end_point_1[3];
	double torsion_point_0[3], torsion_point_1[3];
	double isect_point[3];
	double origin[3], neg_z[3];
	double yup[3];		// alternate up vector
	double *front_cap, *back_cap;	// arrays containing the end caps
	double *front_loop = NULL, *back_loop = NULL; // arrays containing the tube ends
	double *front_norm = NULL, *back_norm = NULL; // arrays containing normal vecs
	double *norm_loop = NULL, *tmp; // normal vectors, cast into 3d from 2d
	int *front_is_trimmed, *back_is_trimmed;   // T or F
	float *front_color, *back_color;  // pointers to segment colors

	CapCallBack cap_callback;
	CapCallBack tmp_cap_callback = NULL;

	int join_style_is_cut;      // TRUE if join style is cut
	double dot;                  // partial dot product
	int first_time = TRUE;
	double *cut_vec;

	// create a local, block scope copy of of the join style.
	// this will alleviate wasted cycles and register write-backs
	// choose the right callback, depending on the choosen join style
	if (m_iJoinStyle & GLE_JN_CUT)
	{
		join_style_is_cut = TRUE;
		cap_callback =  Draw_Cut_Style_Cap_Callback;
	}
	else
	{
		join_style_is_cut = FALSE;
		cap_callback =  Draw_Round_Style_Cap_Callback;
	}

	// By definition, the contour passed in has its up vector pointing in
	// the y direction
	if (m_ptrUp == NULL)
	{
		yup[0] = 0.0;
		yup[1] = 1.0;
		yup[2] = 0.0;
	} 
	else 
	{
		VEC_COPY (yup, m_ptrUp);
	}

	// ========== "up" vector sanity check ==========
	(void) Up_Sanity_Check (yup);

	// the origin is at the origin
	origin [0] = 0.0;
	origin [1] = 0.0;
	origin [2] = 0.0;

	// and neg_z is at neg z
	neg_z[0] = 0.0;
	neg_z[1] = 0.0;
	neg_z[2] = 1.0;

	front_norm = (double *) FROCJMemAnchor;
	back_norm = front_norm + 3*m_iContourPoints;
	front_loop = (double *) (back_norm + 3*m_iContourPoints);
	back_loop = front_loop + 3*m_iContourPoints;
	front_cap = back_loop + 3*m_iContourPoints;
	back_cap  = front_cap + 3*m_iContourPoints;
	front_is_trimmed = (int *) (back_cap + 3*m_iContourPoints);
	back_is_trimmed = front_is_trimmed + m_iContourPoints;

	// =======================================

	// |-|-|-|-|-|-|-|-| SET UP FOR FIRST SEGMENT |-|-|-|-|-|-|-|

	// ignore all segments of zero length
	i = 1;
	m_iINext = i;

	FIND_NON_DEGENERATE_POINT (m_iINext, m_iPoints, seg_len, diff,
		((double(*)[3])m_ptrPointArray));
	GLE_len = seg_len;	// store for later use

	// may as well get the normals set up now
	if (m_ptrXforms == NULL)
	{
		norm_loop = front_norm;
		back_norm = norm_loop;
		for (j=0; j<m_iContourPoints; j++)
		{
			norm_loop[3*j] = m_ptrCont_Norms[j][0];
			norm_loop[3*j+1] = m_ptrCont_Norms[j][1];
			norm_loop[3*j+2] = 0.0;
		}
	}
	else
	{
		for (j=0; j<m_iContourPoints; j++)
		{
			NORM_XFORM_2X2 ( (&front_norm[3*j]),
				m_ptrXforms[m_iINext-1],
				m_ptrCont_Norms [j]);
			front_norm[3*j+2] = 0.0;
			back_norm[3*j+2] = 0.0;
		}
	}

	// get the bisecting plane
	bisecting_plane (bi_0, ((double(*)[3])m_ptrPointArray)[i-1],
		((double(*)[3])m_ptrPointArray)[i],
		((double(*)[3])m_ptrPointArray)[m_iINext]);

	//compute cutting plane
	CUTTING_PLANE (valid_cut_0, cut_0,((double(*)[3])m_ptrPointArray)[i-1],
		((double(*)[3])m_ptrPointArray)[i],
		((double(*)[3])m_ptrPointArray)[m_iINext]);

	// reflect the up vector in the bisecting plane
	VEC_REFLECT (yup, yup, bi_0);

	// |-|-|-|-|-|-|-|-| START LOOP OVER SEGMENTS |-|-|-|-|-|-|-|

	// draw tubing, not doing the first segment
	while (m_iINext<m_iPoints-1)
	{

		inextnext = m_iINext;
		// ignore all segments of zero length
		FIND_NON_DEGENERATE_POINT (inextnext, m_iPoints,
			seg_len, diff, ((double(*)[3])m_ptrPointArray));


		// get the far bisecting plane
		bisecting_plane (bi_1, ((double(*)[3])m_ptrPointArray)[i],
			((double(*)[3])m_ptrPointArray)[m_iINext],
			((double(*)[3])m_ptrPointArray)[inextnext]);


		// compute cutting plane
		CUTTING_PLANE (valid_cut_1, cut_1, ((double(*)[3])m_ptrPointArray)[i],
			((double(*)[3])m_ptrPointArray)[m_iINext],
			((double(*)[3])m_ptrPointArray)[inextnext]);

		// rotate so that z-axis points down v2-v1 axis,
		// and so that origen is at v1
		uviewpoint (m, ((double(*)[3])m_ptrPointArray)[i],
			((double(*)[3])m_ptrPointArray)[m_iINext], yup);
		glPushMatrix ();
		glMultMatrixd ((double*)m);
		//      MULTMATRIX (m);

		// rotate the cutting planes into the local coordinate system
		MAT_DOT_VEC_3X3 (lcut_0, m, cut_0);
		MAT_DOT_VEC_3X3 (lcut_1, m, cut_1);

		// rotate the bisecting planes into the local coordinate system
		MAT_DOT_VEC_3X3 (bisector_0, m, bi_0);
		MAT_DOT_VEC_3X3 (bisector_1, m, bi_1);


		neg_z[2] = -GLE_len;

		// draw the tube
		// --------- START OF TMESH GENERATION --------------
		for (j=0; j<m_iContourPoints; j++)
		{

			// set up the endpoints for segment clipping
			if (m_ptrXforms == NULL)
			{
				VEC_COPY_2 (end_point_0, m_ptrContours[j]);
				VEC_COPY_2 (end_point_1, m_ptrContours[j]);
				VEC_COPY_2 (torsion_point_0, m_ptrContours[j]);
				VEC_COPY_2 (torsion_point_1, m_ptrContours[j]);
			}
			else
			{
				// transform the contour points with the local xform
				MAT_DOT_VEC_2X3 (end_point_0,
					m_ptrXforms[m_iINext-1], m_ptrContours[j]);
				MAT_DOT_VEC_2X3 (torsion_point_0,
					m_ptrXforms[m_iINext], m_ptrContours[j]);
				MAT_DOT_VEC_2X3 (end_point_1,
					m_ptrXforms[m_iINext], m_ptrContours[j]);
				MAT_DOT_VEC_2X3 (torsion_point_1,
					m_ptrXforms[m_iINext-1], m_ptrContours[j]);

				// if there are normals and there are affine xforms,
				// then compute local coordinate system normals.
				// Set up the back normals. (The front normals we inherit
				// from previous pass through the loop).
				if (m_ptrCont_Norms != NULL)
				{
					// do up the normal vectors with the inverse transpose
					NORM_XFORM_2X2 ( (&back_norm[3*j]),
						m_ptrXforms[m_iINext],
						m_ptrCont_Norms [j]);
				}
			}
			end_point_0 [2] = 0.0;
			torsion_point_0 [2] = 0.0;
			end_point_1 [2] = - GLE_len;
			torsion_point_1 [2] = - GLE_len;

			// The two end-points define a line.  Intersect this line
			// against the clipping plane defined by the PREVIOUS
			// tube segment.

			// if this and the last tube are co-linear, don't cut the angle
			// if you do, a divide by zero will result.  This and last tube
			// are co-linear when the cut vector is of zero length
			if (valid_cut_0 && join_style_is_cut)
			{
				INNERSECT (isect_point,  // isect point (returned)
					origin,		// point on intersecting plane
					lcut_0,		// normal vector to plane
					end_point_0,	// point on line
					end_point_1);	// another point on the line

				// determine whether the raw end of the extrusion would have
				// been cut, by checking to see if the raw and is on the
				// far end of the half-plane defined by the cut vector.
				// If the raw end is not "cut", then it is "trimmed".
				//
				if (lcut_0[2] < 0.0)
				{
					VEC_SCALE (lcut_0, -1.0, lcut_0);
				}
				dot = lcut_0[0] * end_point_0[0];
				dot += lcut_0[1] * end_point_0[1];

				VEC_COPY ((&front_loop[3*j]), isect_point);
			}
			else
			{
				// actual value of dot not interseting; need
				// only be positive so that if test below failes
				dot = 1.0;
				VEC_COPY ((&front_loop[3*j]), end_point_0);
			}

			INNERSECT (isect_point, 	// intersection point (returned)
				origin,		// point on intersecting plane
				bisector_0,		// normal vector to plane
				end_point_0,	// point on line
				torsion_point_1);	// another point on the line

			// trim out interior of intersecting tube
			// ... but save the untrimmed version for drawing the endcaps
			// ... note that cap contains valid data ONLY when is_trimmed
			// is TRUE.
			if ((dot <= 0.0) || (isect_point[2] < front_loop[3*j+2]))
			{
				VEC_COPY ((&front_cap[3*j]), (&front_loop [3*j]));
				VEC_COPY ((&front_loop[3*j]), isect_point);
				front_is_trimmed[j] = TRUE;
			}
			else
			{
				front_is_trimmed[j] = FALSE;
			}

			// if intersection is behind the end of the segment,
			// truncate to the end of the segment
			// Note that coding front_loop [3*j+2] = -GLE_len;
			// doesn't work when twists are involved,
			if (front_loop[3*j+2] < -GLE_len)
			{
				VEC_COPY( (&front_loop[3*j]), end_point_1);
			}

			// ---------------------------------------------------
			// The two end-points define a line.  We did one endpoint
			// above. Now do the other.Intersect this line
			// against the clipping plane defined by the NEXT
			// tube segment.

			// if this and the last tube are co-linear, don't cut the angle
			// if you do, a divide by zero will result.  This and last tube
			// are co-linear when the cut vector is of zero length
			if (valid_cut_1 && join_style_is_cut)
			{
				INNERSECT (isect_point,  // isect point (returned)
					neg_z,		// point on intersecting plane
					lcut_1,		// normal vector to plane
					end_point_1,	// point on line
					end_point_0);	// another point on the line

				if (lcut_1[2] > 0.0)
				{
					VEC_SCALE (lcut_1, -1.0, lcut_1);
				}
				dot = lcut_1[0] * end_point_1[0];
				dot += lcut_1[1] * end_point_1[1];


				VEC_COPY ((&back_loop[3*j]), isect_point);
			}
			else
			{
				// actual value of dot not interseting; need
				// only be positive so that if test below failes
				dot = 1.0;
				VEC_COPY ((&back_loop[3*j]), end_point_1);
			}

			INNERSECT (isect_point, 	// intersection point (returned)
				neg_z,		// point on intersecting plane
				bisector_1,		// normal vector to plane
				torsion_point_0,	// point on line
				end_point_1);	// another point on the line

			// cut out interior of intersecting tube
			// ... but save the uncut version for drawing the endcaps
			// ... note that cap contains valid data ONLY when is
			//_trimmed is TRUE.
			if ((dot <= 0.0) || (isect_point[2] > back_loop[3*j+2]))
			{
				VEC_COPY ((&back_cap[3*j]), (&back_loop [3*j]));
				VEC_COPY ((&back_loop[3*j]), isect_point);
				back_is_trimmed[j] = TRUE;
			}
			else
			{
				back_is_trimmed[j] = FALSE;
			}

			// if intersection is behind the end of the segment,
			// truncate to the end of the segment
			// Note that coding back_loop [3*j+2] = 0.0;
			// doesn't work when twists are involved,
			if (back_loop[3*j+2] > 0.0)
			{
				VEC_COPY( (&back_loop[3*j]), end_point_0);
			}
		}

		// --------- END OF TMESH GENERATION --------------

		/// |||||||||||||||||| START SEGMENT DRAW ||||||||||||||||||||
		/// There are six different cases we can have for presence and/or
		// absecnce of colors and normals, and for interpretation of
		// normals. The blechy set of nested if statements below
		// branch to each of the six cases

		m_dLen = GLE_len;

		if (m_ptrXforms == NULL)
		{
			if (m_ptrColorArray == NULL)
			{
				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Segment_Facet_N (  (gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) norm_loop);
				else
					Draw_Segment_Edge_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) norm_loop);
			}
			else
			{
				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Segment_C_And_Facet_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) norm_loop);
				else
					Draw_Segment_C_And_Edge_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) norm_loop);
			}
		}

		else
		{
			if ((float*)m_ptrColorArray == NULL)
			{
				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Binorm_Segment_Facet_N ( (gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) front_norm,
					(gleVector *) back_norm);

				else
					Draw_Binorm_Segment_Edge_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) front_norm,
					(gleVector *) back_norm);
			}
			else
			{
				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Binorm_Segment_C_And_Facet_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) front_norm,
					(gleVector *) back_norm);
				else
					Draw_Binorm_Segment_C_And_Edge_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) front_norm,
					(gleVector *) back_norm);
			}
		}
		// |||||||||||||||||| END SEGMENT DRAW ||||||||||||||||||||

		// v^v^v^v^v^v^v^v^v  BEGIN END CAPS v^v^v^v^v^v^v^v^v^v^v^v

		// if end caps are required, draw them. But don't draw any
		// but the very first and last caps
		if (first_time)
		{
			first_time = FALSE;
			tmp_cap_callback = cap_callback;
			cap_callback = Null_Cap_Callback;

			if (m_iJoinStyle & GLE_JN_CAP)
			{
				if (m_ptrColorArray != NULL)
					SetColor (m_ptrColorArray[m_iINext-1]);
				Draw_Angle_Style_Front_Cap ( bisector_0,
					(double (*)[3]) front_loop);
			}
		}
		// v^v^v^v^v^v^v^v^v  END END CAPS v^v^v^v^v^v^v^v^v^v^v^v

		// $$$$$$$$$$$$$$$$ BEGIN -1, FILLET & JOIN DRAW $$$$$$$$$$$$$$$$$
		//
		// Now, draw the fillet triangles, and the join-caps.
		//
		if (m_ptrColorArray != NULL)
		{
			front_color = ((float(*)[3])m_ptrColorArray)[m_iINext-1];
			back_color = ((float(*)[3])m_ptrColorArray)[m_iINext];
		}
		else
		{
			front_color = NULL;
			back_color = NULL;
		}

		// the flag valid-cut is true if the cut vector has a valid
		// value (i.e. if a degenerate case has not occured).
		//
		if (valid_cut_0)
			cut_vec = lcut_0;
		else
			cut_vec = NULL;

		Draw_Fillets_And_Join_N_Norms (m_iContourPoints, (gleVector *) front_loop,
			(gleVector *) front_cap,
			front_is_trimmed,
			origin,
			bisector_0,
			(gleVector *) front_norm,
			front_color,                               
			cut_vec,
			TRUE,
			cap_callback);

		// v^v^v^v^v^v^v^v^v  BEGIN END CAPS v^v^v^v^v^v^v^v^v^v^v^v
		if (m_iINext == m_iPoints-2)
		{
			if (m_iJoinStyle & GLE_JN_CAP)
			{
				if (m_ptrColorArray != NULL)
					SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);

				Draw_Angle_Style_Back_Cap ( bisector_1,
					(double (*)[3]) back_loop);
				cap_callback = Null_Cap_Callback;
			}
		}
		else
		{
			// restore ability to draw cap
			cap_callback = tmp_cap_callback;
		}
		// v^v^v^v^v^v^v^v^v  END END CAPS v^v^v^v^v^v^v^v^v^v^v^v

		// the flag valid-cut is true if the cut vector has a valid
		// value (i.e. if a degenerate case has not occured).
		//
		if (valid_cut_1)
			cut_vec = lcut_1;
		else
			cut_vec = NULL;

		Draw_Fillets_And_Join_N_Norms (m_iContourPoints,(gleVector *) back_loop,
			(gleVector *) back_cap,
			back_is_trimmed,
			neg_z,
			bisector_1,
			(gleVector *) back_norm,
			back_color,
			cut_vec,
			FALSE,
			cap_callback);
		// $$$$$$$$$$$$$$$$ END FILLET & JOIN DRAW $$$$$$$$$$$$$$$$$

		// pop this matrix, do the next set
		glPopMatrix ();

		// slosh stuff over to next vertex
		tmp = front_norm;
		front_norm = back_norm;
		back_norm = tmp;

		GLE_len = seg_len;
		i = m_iINext;
		m_iINext = inextnext;
		VEC_COPY (bi_0, bi_1);
		VEC_COPY (cut_0, cut_1);
		valid_cut_0 = valid_cut_1;

		// reflect the up vector in the bisecting plane
		VEC_REFLECT (yup, yup, bi_0);
	}
	// |-|-|-|-|-|-|-|-| END LOOP OVER SEGMENTS |-|-|-|-|-|-|-|

}

//----------------------------------------------------------------------------
void CgleBaseExtrusion::Extrusion_Angle_Join ()
{
	int i, j;
	int inextnext;
	double m[4][4];
	double len;
	double len_seg;
	double diff[3];
	double bi_0[3], bi_1[3];		// bisecting plane
	double bisector_0[3], bisector_1[3];	// bisecting plane
	double end_point_0[3], end_point_1[3];
	double origin[3], neg_z[3];
	double yup[3];		// alternate up vector
	double *front_loop, *back_loop;   // contours in 3D
	double *norm_loop;
	double *front_norm, *back_norm, *tmp; // contour normals in 3D
	int first_time;

	// By definition, the contour passed in has its up vector pointing in
	// the y direction
	if (m_ptrUp == NULL)
	{
		yup[0] = 0.0;
		yup[1] = 1.0;
		yup[2] = 0.0;
	}
	else
		VEC_COPY (yup, m_ptrUp);


	// ========== "up" vector sanity check ==========
	(void) Up_Sanity_Check (yup);

	// the origin is at the origin
	origin [0] = 0.0;
	origin [1] = 0.0;
	origin [2] = 0.0;

	// and neg_z is at neg z
	neg_z[0] = 0.0;
	neg_z[1] = 0.0;
	neg_z[2] = 1.0;

	// ignore all segments of zero length
	i = 1;
	m_iINext = i;
	FIND_NON_DEGENERATE_POINT (m_iINext, m_iPoints, len, diff,
		((double(*)[3])m_ptrPointArray));
	len_seg = len;	// store for later use

	// get the bisecting plane
	bisecting_plane (bi_0, ((double(*)[3])m_ptrPointArray)[0],
		((double(*)[3])m_ptrPointArray)[1],
		((double(*)[3])m_ptrPointArray)[m_iINext]);
	// reflect the up vector in the bisecting plane
	VEC_REFLECT (yup, yup, bi_0);

	// malloc the storage we'll need for relaying changed contours to the
	// drawing routines.

	front_loop = (double *) FXAJMemAnchor;
	back_loop = front_loop + 3 * m_iContourPoints;
	front_norm = (double *) (back_loop + 3 * m_iContourPoints);
	back_norm = front_norm + 3 * m_iContourPoints;
	norm_loop = front_norm;

	// may as well get the normals set up now
	if (m_ptrXforms == NULL)
	{
		for (j=0; j<m_iContourPoints; j++)
		{
			norm_loop[3*j] = m_ptrCont_Norms[j][0];
			norm_loop[3*j+1] = m_ptrCont_Norms[j][1];
			norm_loop[3*j+2] = 0.0;
		}
	}
	else
	{
		for (j=0; j<m_iContourPoints; j++)
		{
			NORM_XFORM_2X2((&front_norm[3*j]),
				((double(*)[2][3])m_ptrXforms)[m_iINext - 1],
				m_ptrCont_Norms [j]);
			front_norm[3*j+2] = 0.0;
			back_norm[3*j+2] = 0.0;
		}
	}

	first_time = TRUE;
	// draw tubing, not doing the first segment
	while (m_iINext<m_iPoints-1)
	{

		inextnext = m_iINext;
		// ignore all segments of zero length
		FIND_NON_DEGENERATE_POINT (inextnext, m_iPoints, len, diff,
			((double(*)[3])m_ptrPointArray));
		// get the next bisecting plane
		bisecting_plane (bi_1, ((double(*)[3])m_ptrPointArray)[i],
			((double(*)[3])m_ptrPointArray)[m_iINext],
			((double(*)[3])m_ptrPointArray)[inextnext]);

		// rotate so that z-axis points down v2-v1 axis,
		// and so that origen is at v1
		uviewpoint (m, ((double(*)[3])m_ptrPointArray)[i],
			((double(*)[3])m_ptrPointArray)[m_iINext], yup);
		glPushMatrix ();
		glMultMatrixd ((const double *)m);

		// rotate the bisecting planes into the local coordinate system
		MAT_DOT_VEC_3X3 (bisector_0, m, bi_0);
		MAT_DOT_VEC_3X3 (bisector_1, m, bi_1);

		neg_z[2] = -len_seg;

		// draw the tube
		// --------- START OF TMESH GENERATION --------------
		for (j=0; j<m_iContourPoints; j++)
		{

			// if there are normals, and there are either affine xforms, OR
			// path-edge normals need to be drawn, then compute local
			// coordinate system normals.
			//

			// set up the back normals. (The front normals we inherit
			// from previous pass through the loop)
			if (m_ptrXforms != NULL)
			{
				// do up the normal vectors with the inverse transpose
				NORM_XFORM_2X2 ( (&back_norm[3*j]),
					((double(*)[2][3])m_ptrXforms)[m_iINext],
					m_ptrCont_Norms [j]);
			}

			// Note that if the xform array is NULL, then normals are
			// constant, and are set up outside of the loop.
			//

			//
			// if there are normal vectors, and the style calls for it,
			// then we want to project the normal vectors into the
			// bisecting plane. (This style is needed to make toroids, etc.
			// look good: Without this, segmentation artifacts show up
			// under lighting.
			///
			if (m_iJoinStyle & GLE_NORM_PATH_EDGE)

			{
				// Hmm, if no affine xforms, then we haven't yet set
				// back vector. So do it.
				if (m_ptrXforms == NULL)
				{
					back_norm[3*j] = m_ptrCont_Norms[j][0];
					back_norm[3*j+1] = m_ptrCont_Norms[j][1];
				}

				// now, start with a fresh normal (z component equal to
				// zero), project onto bisecting plane (by computing
				// perpendicular componenet to bisect vector, and renormalize
				// (since projected vector is not of unit length
				front_norm[3*j+2] = 0.0;
				VEC_PERP ((&front_norm[3*j]), (&front_norm[3*j]), bisector_0);
				VEC_NORMALIZE ((&front_norm[3*j]));

				back_norm[3*j+2] = 0.0;
				VEC_PERP ((&back_norm[3*j]), (&back_norm[3*j]), bisector_1);
				VEC_NORMALIZE ((&back_norm[3*j]));
			}

			// Next, we want to define segements. We find the endpoints of
			// the segments by intersecting the contour with the bisecting
			// plane.  If there is no local affine transform, this is easy.
			//
			// If there is an affine tranform, then we want to remove the
			// torsional component, so that the intersection points won't
			// get twisted out of shape.  We do this by applying the
			// local affine transform to the entire coordinate system.
			//
			if (m_ptrXforms == NULL)
			{
				end_point_0 [0] = m_ptrContours[j][0];
				end_point_0 [1] = m_ptrContours[j][1];

				end_point_1 [0] = m_ptrContours[j][0];
				end_point_1 [1] = m_ptrContours[j][1];
			}
			else
			{
				// transform the contour points with the local xform
				MAT_DOT_VEC_2X3 (end_point_0,
					((double(*)[2][3])m_ptrXforms)[m_iINext-1], m_ptrContours[j]);
				MAT_DOT_VEC_2X3 (end_point_1,
					((double(*)[2][3])m_ptrXforms)[m_iINext-1], m_ptrContours[j]);
			}

			end_point_0 [2] = 0.0;
			end_point_1 [2] = - len_seg;

			// The two end-points define a line.  Intersect this line
			// against the clipping plane defined by the PREVIOUS
			// tube segment.

			INNERSECT ((&front_loop[3*j]), // intersection point (returned)
				origin,		// point on intersecting plane
				bisector_0,		// normal vector to plane
				end_point_0,	// point on line
				end_point_1);	// another point on the line

			// The two end-points define a line.  Intersect this line
			// against the clipping plane defined by the NEXT
			// tube segment.

			// if there's an affine coordinate change, be sure to use it
			if (m_ptrXforms != NULL)
			{
				// transform the contour points with the local xform
				MAT_DOT_VEC_2X3 (end_point_0,
					((double(*)[2][3])m_ptrXforms)[m_iINext],m_ptrContours[j]);
				MAT_DOT_VEC_2X3 (end_point_1,
					((double(*)[2][3])m_ptrXforms)[m_iINext],m_ptrContours[j]);
			}

			INNERSECT ((&back_loop[3*j]),	// intersection point (returned)
				neg_z,		// point on intersecting plane
				bisector_1,		// normal vector to plane
				end_point_0,	// point on line
				end_point_1);	// another point on the line

		}

		// --------- END OF TMESH GENERATION --------------

		// v^v^v^v^v^v^v^v^v  BEGIN END CAPS v^v^v^v^v^v^v^v^v^v^v^v

		// if end caps are required, draw them. But don't draw any
		// but the very first and last caps
		if (m_iJoinStyle & GLE_JN_CAP)

		{
			if (first_time)
			{
				if (m_ptrColorArray != NULL)
					SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext-1][0]);

				first_time = FALSE;

				Draw_Angle_Style_Front_Cap ( bisector_0,
					(gleVector *) front_loop);
			}
			if (m_iINext == m_iPoints-2)
			{
				if (m_ptrColorArray != NULL)
					SetColor (&((float(*)[3])m_ptrColorArray)[m_iINext][0]);

				Draw_Angle_Style_Back_Cap ( bisector_1,
					(gleVector *) back_loop);
			}
		}
		// v^v^v^v^v^v^v^v^v  END END CAPS v^v^v^v^v^v^v^v^v^v^v^v

		// |||||||||||||||||| START SEGMENT DRAW ||||||||||||||||||||
		// There are six different cases we can have for presence and/or
		// absecnce of colors and normals, and for interpretation of
		// normals. The blechy set of nested if statements below
		// branch to each of the six cases

		m_dLen = len_seg;

		if ((m_ptrXforms == NULL) && (!(m_iJoinStyle & GLE_NORM_PATH_EDGE)))

		{
			if (m_ptrColorArray == NULL)
			{
				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Segment_Facet_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) norm_loop);
				else
					Draw_Segment_Edge_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) norm_loop );
			}
			else
			{

				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Segment_C_And_Facet_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) norm_loop);
				else
					Draw_Segment_C_And_Edge_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) norm_loop);
			}
		}
		else
		{
			if (m_ptrColorArray == NULL)
			{

				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Binorm_Segment_Facet_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) front_norm,
					(gleVector *) back_norm);
				else
					Draw_Binorm_Segment_Edge_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) front_norm,
					(gleVector *) back_norm);
			}
			else
			{

				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Binorm_Segment_C_And_Facet_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) front_norm,
					(gleVector *) back_norm);
				else
					Draw_Binorm_Segment_C_And_Edge_N ((gleVector *) front_loop,
					(gleVector *) back_loop,
					(gleVector *) front_norm,
					(gleVector *) back_norm);
			}
		}
		// |||||||||||||||||| END SEGMENT DRAW ||||||||||||||||||||

		// pop this matrix, do the next set
		glPopMatrix ();

		// bump everything to the next vertex
		len_seg = len;
		i = m_iINext;
		m_iINext = inextnext;
		VEC_COPY (bi_0, bi_1);

		// trade norm loops
		tmp = front_norm;
		front_norm = back_norm;
		back_norm = tmp;

		// reflect the up vector in the bisecting plane
		VEC_REFLECT (yup, yup, bi_0);
	}
}

//----------------------------------------------------------------------------

// ============================================================
// This routine draws "raw" style extrusions.  By "raw" style, it is
// meant extrusions with square ends: ends that are cut at 90 degrees to
// the length of the extrusion.  End caps are NOT drawn, unless the end cap
// style is indicated.
//
void CgleBaseExtrusion::Extrusion_Raw_Join()
{
	int i, j;
	double len;
	double m[4][4];
	double diff[3];
	double bi_0[3];		// bisecting plane
	double yup[3];		// alternate up vector
	double nrmv[3];
	short no_cols, no_xform;     //booleans
	double *front_loop = NULL, *back_loop = NULL;  // countour loops
	double *front_norm = NULL, *back_norm = NULL;  // countour loops
	double *tmp;

	nrmv[0] = nrmv[1] = 0.0;   // used for drawing end caps
					   // use some local variables for needed booleans

	no_cols = (m_ptrColorArray == NULL);
	no_xform = (m_ptrXforms == NULL);

	// alloc loop arrays if needed
	if (!no_xform)
	{
		front_loop = (double *)FMemAnchor;
		back_loop = front_loop + 3 * m_iContourPoints;
		front_norm = back_loop + 3 * m_iContourPoints;
		back_norm = front_norm + 3 * m_iContourPoints;
	}

	// By definition, the contour passed in has its up vector pointing in
	// the y direction
	if (m_ptrUp == NULL)
	{
		yup[0] = 0.0;
		yup[1] = 1.0;
		yup[2] = 0.0;
	}
	else
		VEC_COPY(yup, m_ptrUp);

	// ========== "up" vector sanity check ==========
	(void)Up_Sanity_Check(yup);

	// ignore all segments of zero length
	i = 1;
	m_iINext = i;
	FIND_NON_DEGENERATE_POINT(m_iINext, m_iPoints, len, diff,
		((double(*)[3])m_ptrPointArray));

	// first time through, get the loops
	if (!no_xform)
	{
		for (j = 0; j<m_iContourPoints; j++)
		{
			MAT_DOT_VEC_2X3((&front_loop[3 * j]),
				((double(*)[2][3])m_ptrXforms)[m_iINext - 1],
				m_ptrContours[j]);
			front_loop[3 * j + 2] = 0.0;
		}
		for (j = 0; j<m_iContourPoints; j++)
		{
			NORM_XFORM_2X2((&front_norm[3 * j]),
				((double(*)[2][3])m_ptrXforms)[m_iINext - 1],
				m_ptrCont_Norms[j]);
			front_norm[3 * j + 2] = 0.0;
			back_norm[3 * j + 2] = 0.0;
		}
	}

	// draw tubing, not doing the first segment
	while (m_iINext<m_iPoints - 1)
	{

		// get the two bisecting planes
		bisecting_plane(bi_0, ((double(*)[3])m_ptrPointArray)[i - 1],
			((double(*)[3])m_ptrPointArray)[i],
			((double(*)[3])m_ptrPointArray)[m_iINext]);

		// reflect the up vector in the bisecting plane
		VEC_REFLECT(yup, yup, bi_0);

		// rotate so that z-axis points down v2-v1 axis,
		// and so that origen is at v1
		uviewpoint(m, ((double(*)[3])m_ptrPointArray)[i],
			((double(*)[3])m_ptrPointArray)[m_iINext], yup);
		glPushMatrix();
		glMultMatrixd((const double *)m);

		// There are six different cases we can have for presence and/or
		// absecnce of colors and normals, and for interpretation of
		// normals. The blechy set of nested if statements below
		// branch to each of the six cases

		m_dLen = len;
		if (no_xform)
		{
			if (no_cols)
			{
				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Raw_Segment_Facet_N();

				else
					Draw_Raw_Segment_Edge_N();
			}
			else
			{
				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Raw_Segment_C_And_Facet_N();

				else
					Draw_Raw_Segment_C_And_Edge_N();
			}
		}
		else
		{

			// else -- there are scales and offsets to deal with
			for (j = 0; j<m_iContourPoints; j++)
			{
				MAT_DOT_VEC_2X3((&back_loop[3 * j]),
					((double(*)[2][3])m_ptrXforms)[m_iINext], m_ptrContours[j]);
				back_loop[3 * j + 2] = -len;
				front_loop[3 * j + 2] = 0.0;
			}

			for (j = 0; j<m_iContourPoints; j++)
			{
				NORM_XFORM_2X2((&back_norm[3 * j]),
					((double(*)[2][3])m_ptrXforms)[m_iINext],
					m_ptrCont_Norms[j]);
			}

			if (no_cols)
			{

				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Binorm_Segment_Facet_N((double(*)[3]) front_loop,
					(double(*)[3]) back_loop,
						(double(*)[3]) front_norm,
						(double(*)[3]) back_norm);
				else
					Draw_Binorm_Segment_Edge_N((double(*)[3]) front_loop,
					(double(*)[3]) back_loop,
						(double(*)[3]) front_norm,
						(double(*)[3]) back_norm);
				if (m_iJoinStyle & GLE_JN_CAP)

				{
					nrmv[2] = 1.0;
					N3D(nrmv);
					Draw_Front_Contour_Cap((gleVector *)front_loop);
					nrmv[2] = -1.0;
					N3D(nrmv);
					Draw_Back_Contour_Cap((gleVector *)back_loop);
				}
			}
			else
			{

				if (m_iJoinStyle & GLE_NORM_FACET)
					Draw_Binorm_Segment_C_And_Facet_N((double(*)[3]) front_loop,
					(double(*)[3]) back_loop,
						(double(*)[3]) front_norm,
						(double(*)[3]) back_norm);
				else
					Draw_Binorm_Segment_C_And_Edge_N((double(*)[3]) front_loop,
					(double(*)[3]) back_loop,
						(double(*)[3]) front_norm,
						(double(*)[3]) back_norm);

				if (m_iJoinStyle & GLE_JN_CAP)

				{
					SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext - 1][0]);
					nrmv[2] = 1.0;
					N3D(nrmv);
					Draw_Front_Contour_Cap((gleVector *)front_loop);

					SetColor(&((float(*)[3])m_ptrColorArray)[m_iINext][0]);
					nrmv[2] = -1.0;
					N3D(nrmv);
					Draw_Back_Contour_Cap((gleVector *)back_loop);
				}
			}
		}

		// pop this matrix, do the next set
		glPopMatrix();

		// flop over transformed loops
		tmp = front_loop;
		front_loop = back_loop;
		back_loop = tmp;
		tmp = front_norm;
		front_norm = back_norm;
		back_norm = tmp;

		i = m_iINext;
		// ignore all segments of zero length
		FIND_NON_DEGENERATE_POINT(m_iINext, m_iPoints, len, diff,
			((double(*)[3])m_ptrPointArray));

	}
}
