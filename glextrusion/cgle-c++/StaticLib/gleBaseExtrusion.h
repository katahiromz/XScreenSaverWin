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

//----------------------------------------------------------------------------
class CgleBaseExtrusion
{


public:


	CgleBaseExtrusion(int Points, int ContourPoints, double  Radius);
	virtual ~CgleBaseExtrusion();


	void SetExtrusionMode(int mode);
	int  GetExtrusionMode() const;

	void SetTextureMode(int mode);
	int  GetTextureMode() const;

	void UseLitMaterial(bool value);
	bool IsUsingLitMaterial() const;

	void LoadUpVector(double Up[3]);


protected:

	void  LoadContourPoint(int Index, double X, double Y);

	void DrawExtrusion(double Point_Array[][3], float Color_Array[][3], bool bTextured = false);


	void DrawWireExtrusion(double Point_Array[][3], float Color_Array[][3], bool bTextured = false);


	gleContourVector *m_ptrCont_Norms, *m_ptrContours;

	double   *m_ptrPointArray;
	gleColor   *m_ptrColorArray;

	double m_dRadius;

	int     m_iPoints, m_iContourPoints;
	gleVector *m_ptrPath;
	double  *m_ptrTwarr;
	double  *m_ptrUp;

	bool m_bUseLitMaterial;
	int m_iJoinStyle;

	int m_iTexMode;

	bool m_bTexEnabled;

	int m_iINext;
	double m_dLen;

	void PrepareGC();

	gleAffine *m_ptrXforms;



private:


	char  *FMemAnchor, *FXAJMemAnchor, *FROCJMemAnchor, *FMallocedArea;

	GLUtesselator   *m_objTess;

	double *FDRSECPoints, *FCapLoop, *FNNormCapLoop, *FNNormLoop;



	void Draw_Fillet_Triangle_N_Norms(double va[3], double vb[3],
		double vc[3], int face, float front_color[3],
		double na[3], double nb[3]);


	void Draw_Segment_Edge_N(double front_contour[][3],
		double back_contour[][3], double norm_cont[][3]);

	void Draw_Segment_C_And_Edge_N(double front_contour[][3],
		double back_contour[][3], double norm_cont[][3]);

	void Draw_Segment_Facet_N(double front_contour[][3],
		double back_contour[][3], double norm_cont[][3]);

	void Draw_Segment_C_And_Facet_N(double front_contour[][3],
		double back_contour[][3], double norm_cont[][3]);


	void Draw_Binorm_Segment_Edge_N(double front_contour[][3],
		double back_contour[][3], double front_norm[][3],
		double back_norm[][3]);

	void Draw_Binorm_Segment_C_And_Edge_N(double front_contour[][3],
		double back_contour[][3], double front_norm[][3],
		double back_norm[][3]);

	void Draw_Binorm_Segment_Facet_N(double front_contour[][3],
		double back_contour[][3], double front_norm[][3],
		double back_norm[][3]);

	void Draw_Binorm_Segment_C_And_Facet_N(double front_contour[][3],
		double back_contour[][3], double front_norm[][3],
		double back_norm[][3]);

	void Draw_Fillets_And_Join_N_Norms(int ncp, double trimmed_loop[][3],
		double untrimmed_loop[][3], int is_trimmed[],
		double bis_origin[3], double bis_vector[3],
		double normals[][3], float front_color[3],
		double cut_vector[3], int face, CapCallBack Cap_CallBack);

	static void Draw_Round_Style_Cap_Callback(int ncp, double cap[][3],
		float face_color[3], double cut[3],
		double bi[3], double norms[][3],
		int frontwards, void* clOwner);

	static void  Null_Cap_Callback(int iloop, double cap[][3],
		float face_color[3], double cut_vector[3],
		double bisect_vector[3], double norms[][3],
		int frontwards, void* clOwner);


	static void Draw_Cut_Style_Cap_Callback(int iloop, double cap[][3],
		float face_color[3], double cut_vector[3],
		double bisect_vector[3], double norms[][3],
		int frontwards, void* clOwner);


	void Draw_Angle_Style_Back_Cap(double bi[3], double point_array[][3]);

	void Draw_Angle_Style_Front_Cap(double bi[3],
		double point_array[][3]);

	void Draw_Raw_Segment_C_And_Facet_N();

	void Draw_Raw_Segment_Facet_N();

	void Draw_Raw_Segment_C_And_Edge_N();

	void Draw_Raw_Segment_Edge_N();

	void Draw_Back_Contour_Cap(double contour[][3]);

	void Draw_Front_Contour_Cap(double contour[][3]);

	void Up_Sanity_Check(double up[3]);

	void Draw_Raw_Style_End_Cap(double contour[][2], double zval,
		int frontwards);

	void Extrusion_Round_Or_Cut_Join();

	void Extrusion_Angle_Join();

	void Extrusion_Raw_Join();


};
//----------------------------------------------------------------------------
