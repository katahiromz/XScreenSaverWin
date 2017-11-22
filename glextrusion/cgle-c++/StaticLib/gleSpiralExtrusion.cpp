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
//----------------------------------------------------------------------------
//
//											 CgleSpiralExtrusion
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// The spiral primitive forms the basis for the helicoid primitive.
//
// Note that this primitive sweeps a contour along a helical path.
// The algorithm assumes that the path is embedded in Euclidean space,
// and uses parallel transport along the path.  Parallel transport
// provides the simplest mathematical model for moving a coordinate
// system along a curved path, but some of the effects of doing so
// may prove to be surprising to one uninitiated to the concept.
//
// Thus, we provide another, related, algorithm below, called "lathe"
// which introduces a torsion component along the path, correcting for
// the rotation induced by the helical path.
//
// If the above sounds like gobldy-gook to you, you may want to brush
// up on differential geometry. Recommend Spivak, Differential Geometry,
// Volume 1, pages xx-xx.
//----------------------------------------------------------------------------
CgleSpiralExtrusion::CgleSpiralExtrusion(int Points, double StartXForm[2][3], int ContourPoints,
														double Sweep, double Radius) : 
														CgleBaseExtrusion(Points, ContourPoints, Radius)
{

	double* Up = new double[3];// make up vector point along z axis
	Up[1] = Up[0] = 0.0;
	Up[2] = 1.0;

	LoadUpVector(Up);

	delete[] Up;

	FSweep = Sweep;

	SetExtrusionMode(GLE_JN_CAP | GLE_CONTOUR_CLOSED | GLE_NORM_PATH_EDGE
		| GLE_JN_ROUND);

	SetTextureMode(GLE_TEXTURE_NORMAL_MODEL_CYL);
	if (StartXForm != NULL)
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				FStartXForm[i][j] = StartXForm[i][j];
			}
		}
	}
	else
		FStartXForm[0][0] = -1E300;  // flag value

	if (StartXForm == NULL)
	{
		FSpiralMem_Anchor = new char[24 * Points];
		FPts = (double *)FSpiralMem_Anchor;
	}
	else
	{
		FSpiralMem_Anchor = new char[72 * Points];
		FPts = (double *)FSpiralMem_Anchor;
		m_ptrXforms = (gleAffine *)(FPts + 3 * Points);
	}
	FDeltaAngle = (M_PI / 180.0) * FSweep / ((double)(Points - 3));

}
//----------------------------------------------------------------------------
CgleSpiralExtrusion::~CgleSpiralExtrusion()
{

	delete[] FSpiralMem_Anchor;
	FSpiralMem_Anchor = NULL;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  CgleSpiralExtrusion::SetTForm(double dXFormdTheta[2][3])
{
	double mA[2][2], mB[2][2];
	double run[2][2];
	double deltaTrans[2];
	double trans[2];
	double delta = FDeltaAngle / (2.0 * M_PI);


	// If there is a deformation matrix specified, then a deformation
	// path must be generated also
	if (FStartXForm[0][0] != -1E300)
	{
		if (dXFormdTheta == NULL)
		{
			for (int i = 0; i<m_iPoints; i++)
			{
				m_ptrXforms[i][0][0] = FStartXForm[0][0];
				m_ptrXforms[i][0][1] = FStartXForm[0][1];
				m_ptrXforms[i][0][2] = FStartXForm[0][2];
				m_ptrXforms[i][1][0] = FStartXForm[1][0];
				m_ptrXforms[i][1][1] = FStartXForm[1][1];
				m_ptrXforms[i][1][2] = FStartXForm[1][2];
			}
		}
		else
		{
			//
			// if there is a differential matrix specified, treat it a
			// a tangent (algebraic, infinitessimal) matrix.  We need to
			// project it into the group of real 2x2 matricies.  (Note that
			// the specified matrix is affine.  We treat the translation
			// components linearly, and only treat the 2x2 submatrix as an
			// algebraic tangenet).
			//
			// For exponentiaition, we use the well known approx:
			// exp(x) = lim (N->inf) (1+x/N) ** N
			// and take N=32.
			//

			// initialize translation and delta translation
			deltaTrans[0] = delta * dXFormdTheta[0][2];
			deltaTrans[1] = delta * dXFormdTheta[1][2];
			trans[0] = FStartXForm[0][2];
			trans[1] = FStartXForm[1][2];

			// prepare the tangent matrix
			delta /= 32.0;
			mA[0][0] = 1.0 + delta * dXFormdTheta[0][0];
			mA[0][1] = delta * dXFormdTheta[0][1];
			mA[1][0] = delta * dXFormdTheta[1][0];
			mA[1][1] = 1.0 + delta * dXFormdTheta[1][1];

			// compute exponential of matrix
			MATRIX_PRODUCT_2X2(mB, mA, mA);  // squared
			MATRIX_PRODUCT_2X2(mA, mB, mB);  // 4th power
			MATRIX_PRODUCT_2X2(mB, mA, mA);  // 8th power
			MATRIX_PRODUCT_2X2(mA, mB, mB);  // 16th power
			MATRIX_PRODUCT_2X2(mB, mA, mA);  // 32nd power

								   // initialize running matrix
			COPY_MATRIX_2X2(run, FStartXForm);

			// remember, the first point is hidden -- load some, any
			// xform for the first point
			m_ptrXforms[0][0][0] = FStartXForm[0][0];
			m_ptrXforms[0][0][1] = FStartXForm[0][1];
			m_ptrXforms[0][0][2] = FStartXForm[0][2];
			m_ptrXforms[0][1][0] = FStartXForm[1][0];
			m_ptrXforms[0][1][1] = FStartXForm[1][1];
			m_ptrXforms[0][1][2] = FStartXForm[1][2];

			for (int i = 1; i<m_iPoints; i++)
			{
				m_ptrXforms[i][0][0] = run[0][0];
				m_ptrXforms[i][0][1] = run[0][1];
				m_ptrXforms[i][1][0] = run[1][0];
				m_ptrXforms[i][1][1] = run[1][1];

				// integrate to get exponential matrix
				// (Note that the group action is a left-action --
				// i.e. multiply on the left (not the right))
				MATRIX_PRODUCT_2X2(mA, mB, run);
				COPY_MATRIX_2X2(run, mA);

				m_ptrXforms[i][0][2] = trans[0];
				m_ptrXforms[i][1][2] = trans[1];
				trans[0] += deltaTrans[0];
				trans[1] += deltaTrans[1];

			}
		}
	}
}
//----------------------------------------------------------------------------
void CgleSpiralExtrusion::Draw(double StartRadius,
												double dRadiusdTheta, double StartZ,
												double dZdTheta, double StartTheta,
												float Color_Array[][3], bool bTextured)
{

	int saved_style;
	double delta, cprev, sprev, cdelta, sdelta, ccurr, scurr;

	StartTheta *= M_PI / 180.0;
	StartTheta -= FDeltaAngle;

	// initialize factors
	cprev = cos((double)StartTheta);
	sprev = sin((double)StartTheta);

	cdelta = cos((double)FDeltaAngle);
	sdelta = sin((double)FDeltaAngle);

	// renormalize differential factors
	delta = FDeltaAngle / (2.0 * M_PI);
	dZdTheta *= delta;
	dRadiusdTheta *= delta;

	// remember, the first point is hidden, so back-step
	StartZ -= dZdTheta;
	StartRadius -= dRadiusdTheta;

	// draw spiral path using recursion relations for sine, cosine *
	for (int i = 0; i<m_iPoints; i++)
	{
		FPts[3 * i] = StartRadius * cprev;
		FPts[3 * i + 1] = StartRadius * sprev;
		FPts[3 * i + 2] = (double)StartZ;

		StartZ += dZdTheta;
		StartRadius += dRadiusdTheta;
		ccurr = cprev * cdelta - sprev * sdelta;
		scurr = cprev * sdelta + sprev * cdelta;
		cprev = ccurr;
		sprev = scurr;
	}

	// save the current join style
	saved_style = CgleBaseExtrusion::GetExtrusionMode();//extrusion_join_style;

									    // Allow only angle joins (for performance reasons).
									    // The idea is that if the tesselation is fine enough, then an angle
									    // join should be sufficient to get the desired visual quality.  A
									    // raw join would look terrible, an cut join would leave garbage
									    // everywhere, and a round join will over-tesselate (and thus
									    // should be avoided for performance reasons).
									    //

	int style = saved_style & ~GLE_JN_MASK;
	style |= GLE_JN_ANGLE;

	CgleBaseExtrusion::SetExtrusionMode(style);

	CgleBaseExtrusion::DrawExtrusion((double(*)[3])FPts, Color_Array, bTextured);

}
//----------------------------------------------------------------------------
void CgleSpiralExtrusion::WireDraw(double StartRadius,
											double dRadiusdTheta, double StartZ,
											double dZdTheta, double StartTheta,
											float Color_Array[][3], bool bTextured)
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	Draw(StartRadius, dRadiusdTheta, StartZ, dZdTheta, StartTheta, Color_Array, bTextured);
	glPopAttrib();
}
//----------------------------------------------------------------------------
