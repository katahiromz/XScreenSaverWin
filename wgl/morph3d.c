/* -*- Mode: C; tab-width: 4 -*- */
/* morph3d --- Shows 3D morphing objects */

#if 0
static const char sccsid[] = "@(#)morph3d.c	5.01 2001/03/01 xlockmore";
#endif

#undef DEBUG_CULL_FACE

/*-
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 * The original code for this mode was written by Marcelo Fernandes Vianna
 * (me...) and was inspired on a WindowsNT(R)'s screen saver (Flower Box).
 * It was written from scratch and it was not based on any other source code.
 *
 * Porting it to xlock (the final objective of this code since the moment I
 * decided to create it) was possible by comparing the original Mesa's gear
 * demo with it's ported version to xlock, so thanks for Danny Sung (look at
 * gear.c) for his indirect help.
 *
 * Thanks goes also to Brian Paul for making it possible and inexpensive
 * to use OpenGL at home.
 *
 * If you are interested in the original version of this program (not a xlock
 * mode, please refer to the Mesa package (ftp iris.ssec.wisc.edu on /pub/Mesa)
 *
 * Since I'm not a native English speaker, my apologies for any grammatical
 * mistakes.
 *
 * My e-mail address is
 * mfvianna@centroin.com.br
 *
 * Marcelo F. Vianna (Feb-13-1997)
 *
 * Revision History:
 * 05-Apr-2002: Removed all gllist uses (fix some bug with nvidia driver)
 * 01-Mar-2001: Added FPS stuff E.Lassauge <lassauge@mail.dotcom.fr>
 * 27-Jul-1997: Speed ups by Marcelo F. Vianna.
 * 08-May-1997: Speed ups by Marcelo F. Vianna.
 *
 */

#define MODE_moebius
#define DELAY 40000
#define DEFAULTS		"*delay:		40000	\n"		\
						"*showFPS:      False   \n"		\
						"*count: 		0		\n"
#define refresh_morph3d NULL
#define morph3d_handle_event NULL

#if 0
	#ifdef STANDALONE
	# include "xlockmore.h"		/* from the xscreensaver distribution */
	#else /* !STANDALONE */
	# include "xlock.h"		/* from the xlockmore distribution */
	#endif /* !STANDALONE */
#endif

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include "win32.h"

#ifdef MODE_moebius

#if 0
ENTRYPOINT ModeSpecOpt morph3d_opts =
{0, (XrmOptionDescRec *) NULL, 0, (argtype *) NULL, (OptionStruct *) NULL};

	#ifdef USE_MODULES
	ModStruct   morph3d_description =
	{"morph3d", "init_morph3d", "draw_morph3d", "release_morph3d",
	 "draw_morph3d", "change_morph3d", (char *) NULL, &morph3d_opts,
	 1000, 0, 1, 1, 4, 1.0, "",
	 "Shows GL morphing polyhedra", 0, NULL};
	#endif
#endif

#define Scale4Window               0.3
#define Scale4Iconic               1.0

#define VectMul(X1,Y1,Z1,X2,Y2,Z2) (Y1)*(Z2)-(Z1)*(Y2),(Z1)*(X2)-(X1)*(Z2),(X1)*(Y2)-(Y1)*(X2)
#define sqr(A)                     ((A)*(A))

/* Increasing this values produces better image quality, the price is speed. */
#define tetradivisions             23
#define cubedivisions              20
#define octadivisions              21
#define dodecadivisions            10
#define icodivisions               15

#define tetraangle                 109.47122063449069174
#define cubeangle                  90.000000000000000000
#define octaangle                  109.47122063449069174
#define dodecaangle                63.434948822922009981
#define icoangle                   41.810314895778596167

#ifndef Pi
#define Pi                         M_PI
#endif
#define SQRT2                      1.4142135623730951455
#define SQRT3                      1.7320508075688771932
#define SQRT5                      2.2360679774997898051
#define SQRT6                      2.4494897427831778813
#define SQRT15                     3.8729833462074170214
#define cossec36_2                 0.8506508083520399322
#define cos72                      0.3090169943749474241
#define sin72                      0.9510565162951535721
#define cos36                      0.8090169943749474241
#define sin36                      0.5877852522924731292

/*************************************************************************/

typedef struct {
	GLint       WindH, WindW;
	GLfloat     step;
	GLfloat     seno;
	int         object;
	int         edgedivisions;
	int         VisibleSpikes;
	void        (*draw_object) (ModeInfo * mi);
	float       Magnitude;
	const float *MaterialColor[20];
	//GLXContext *glx_context;
	HGLRC hglrc;
    int         arrayninit;

} morph3dstruct;

static const GLfloat front_shininess[] = {60.0f};
static const GLfloat front_specular[]  = {0.7f, 0.7f, 0.7f, 1.0f};
static const GLfloat ambient[]         = {0.0f, 0.0f, 0.0f, 1.0f};
static const GLfloat diffuse[]         = {1.0f, 1.0f, 1.0f, 1.0f};
static const GLfloat position0[]       = {1.0f, 1.0f, 1.0f, 0.0f};
static const GLfloat position1[]       = {-1.0f, -1.0f, 1.0f, 0.0f};
static const GLfloat lmodel_ambient[]  = {0.5f, 0.5f, 0.5f, 1.0f};
static const GLfloat lmodel_twoside[]  = {GL_TRUE};

static const GLfloat MaterialRed[]     = {0.7f, 0.0f, 0.0f, 1.0f};
static const GLfloat MaterialGreen[]   = {0.1f, 0.5f, 0.2f, 1.0f};
static const GLfloat MaterialBlue[]    = {0.0f, 0.0f, 0.7f, 1.0f};
static const GLfloat MaterialCyan[]    = {0.2f, 0.5f, 0.7f, 1.0f};
static const GLfloat MaterialYellow[]  = {0.7f, 0.7f, 0.0f, 1.0f};
static const GLfloat MaterialMagenta[] = {0.6f, 0.2f, 0.5f, 1.0f};
static const GLfloat MaterialWhite[]   = {0.7f, 0.7f, 0.7f, 1.0f};
static const GLfloat MaterialGray[]    = {0.5f, 0.5f, 0.5f, 1.0f};

static morph3dstruct *morph3d = (morph3dstruct *) NULL;

#define TRIANGLE(Edge, Amp, Divisions, Z, VS)                                                                    \
{                                                                                                                \
  GLfloat   Xf,Yf,Xa,Yb=0.0f,Xf2=0.0f,Yf2=0.0f,Yf_2=0.0f,Yb2,Yb_2;                                                   \
  GLfloat   Factor=0.0f,Factor1,Factor2;                                                                          \
  GLfloat   VertX,VertY,VertZ,NeiAX,NeiAY,NeiAZ,NeiBX,NeiBY,NeiBZ;                                               \
  GLfloat   Ax,Ay;                                                                                               \
  int       Ri,Ti;                                                                                               \
  GLfloat   Vr=(float)((Edge)*SQRT3/3);                                                                                   \
  GLfloat   AmpVr2=(float)((Amp)/sqr(Vr));                                                                                \
  GLfloat   Zf=(float)((Edge)*(Z));                                                                                       \
                                                                                                                 \
  Ax=(float)((Edge)*(+0.5/(Divisions))), Ay=(float)((Edge)*(-SQRT3/(2*Divisions)));                                                \
                                                                                                                 \
  Yf=Vr+Ay; Yb=Yf+0.001f;                                                                                         \
  for (Ri=1; Ri<=(Divisions); Ri++) {                                                                            \
    glBegin(GL_TRIANGLE_STRIP);                                                                                  \
    Xf=(float)Ri*Ax; Xa=Xf+0.001f;                                                                                \
    Yf2=sqr(Yf); Yf_2=sqr(Yf-Ay);                                                                                \
    Yb2=sqr(Yb); Yb_2=sqr(Yb-Ay);                                                                                \
    for (Ti=0; Ti<Ri; Ti++) {                                                                                    \
      Factor=1-(((Xf2=sqr(Xf))+Yf2)*AmpVr2);                                                                     \
      Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                                                          \
      Factor2=1-((Xf2+Yb2)*AmpVr2);                                                                              \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                           \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                    \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                    \
      glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                             \
      glVertex3f(VertX, VertY, VertZ);                                                                           \
      mi->polygon_count++;                                                                                       \
                                                                                                                 \
      Xf-=Ax; Yf-=Ay; Xa-=Ax; Yb-=Ay;                                                                            \
                                                                                                                 \
      Factor=1-(((Xf2=sqr(Xf))+Yf_2)*AmpVr2);                                                                    \
      Factor1=1-((sqr(Xa)+Yf_2)*AmpVr2);                                                                         \
      Factor2=1-((Xf2+Yb_2)*AmpVr2);                                                                             \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                           \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                    \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                    \
      glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                             \
      glVertex3f(VertX, VertY, VertZ);                                                                           \
      mi->polygon_count++;                                                                                       \
                                                                                                                 \
      Xf-=Ax; Yf+=Ay; Xa-=Ax; Yb+=Ay;                                                                            \
    }                                                                                                            \
    Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                                                             \
    Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                                                            \
    Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                            \
    VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                             \
    NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                      \
    NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                      \
    glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                               \
    glVertex3f(VertX, VertY, VertZ);                                                                             \
    Yf+=Ay; Yb+=Ay;                                                                                              \
    glEnd();                                                                                                     \
  }                                                                                                              \
  VS=(Factor<0);                                                                                                 \
}

#define SQUARE(Edge, Amp, Divisions, Z, VS)                                                                      \
{                                                                                                                \
  int       Xi,Yi;                                                                                               \
  GLfloat   Xf,Yf,Y,Xf2,Yf2,Y2,Xa,Xa2,Yb;                                                                        \
  GLfloat   Factor=0.0f,Factor1,Factor2;                                                                          \
  GLfloat   VertX,VertY,VertZ,NeiAX,NeiAY,NeiAZ,NeiBX,NeiBY,NeiBZ;                                               \
  GLfloat   Zf=(float)((Edge)*(Z));                                                                                       \
  GLfloat   AmpVr2=(float)((Amp)/sqr((Edge)*SQRT2/2));                                                                    \
                                                                                                                 \
  for (Yi=0; Yi<(Divisions); Yi++) {                                                                             \
    Yf=-((Edge)/2.0f) + ((float)Yi)/(Divisions)*(Edge);                                                           \
    Yf2=sqr(Yf);                                                                                                 \
    Y=Yf+1.0f/(Divisions)*(Edge);                                                                                 \
    Y2=sqr(Y);                                                                                                   \
    glBegin(GL_QUAD_STRIP);                                                                                      \
    for (Xi=0; Xi<=(Divisions); Xi++) {                                                                          \
      Xf=-((Edge)/2.0f) + ((float)Xi)/(Divisions)*(Edge);                                                         \
      Xf2=sqr(Xf);                                                                                               \
                                                                                                                 \
      Xa=Xf+0.001f; Yb=Y+0.001f;                                                                                   \
      Factor=1-((Xf2+Y2)*AmpVr2);                                                                                \
      Factor1=1-(((Xa2=sqr(Xa))+Y2)*AmpVr2);                                                                     \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                          \
      VertX=Factor*Xf;        VertY=Factor*Y;         VertZ=Factor*Zf;                                           \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Y-VertY;  NeiAZ=Factor1*Zf-VertZ;                                    \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                    \
      glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                             \
      glVertex3f(VertX, VertY, VertZ);                                                                           \
                                                                                                                 \
      Yb=Yf+0.001f;                                                                                               \
      Factor=1-((Xf2+Yf2)*AmpVr2);                                                                               \
      Factor1=1-((Xa2+Yf2)*AmpVr2);                                                                              \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                          \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                           \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                    \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                    \
      glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                             \
      glVertex3f(VertX, VertY, VertZ);                                                                           \
      mi->polygon_count++;                                                                                       \
    }                                                                                                            \
    glEnd();                                                                                                     \
  }                                                                                                              \
  VS=(Factor<0);                                                                                                 \
}

#define PENTAGON(Edge, Amp, Divisions, Z, VS)                                                                    \
{                                                                                                                \
  int       Ri,Ti,Fi;                                                                                            \
  GLfloat   Xf,Yf,Xa,Yb,Xf2,Yf2;                                                                                 \
  GLfloat   Factor=0.0f,Factor1,Factor2;                                                                          \
  GLfloat   VertX,VertY,VertZ,NeiAX,NeiAY,NeiAZ,NeiBX,NeiBY,NeiBZ;                                               \
  GLfloat   Zf=(Edge)*(Z);                                                                                       \
  GLfloat   AmpVr2=(float)((Amp)/sqr((Edge)*cossec36_2));                                                                 \
                                                                                                                 \
  GLfloat x[6],y[6];                                                                                             \
                                                                                                                 \
  for(Fi=0;Fi<6;Fi++) {                                                                                          \
    x[Fi]=(float)(-COSF( Fi*2*Pi/5 + Pi/10 )/(Divisions)*cossec36_2*(Edge));                                               \
    y[Fi]=(float)(SINF( Fi*2*Pi/5 + Pi/10 )/(Divisions)*cossec36_2*(Edge));                                                \
  }                                                                                                              \
                                                                                                                 \
  for (Ri=1; Ri<=(Divisions); Ri++) {                                                                            \
    for (Fi=0; Fi<5; Fi++) {                                                                                     \
      glBegin(GL_TRIANGLE_STRIP);                                                                                \
      for (Ti=0; Ti<Ri; Ti++) {                                                                                  \
        Xf=(float)(Ri-Ti)*x[Fi] + (float)Ti*x[Fi+1];                                                             \
        Yf=(float)(Ri-Ti)*y[Fi] + (float)Ti*y[Fi+1];                                                             \
        Xa=Xf+0.001f; Yb=Yf+0.001f;                                                                                \
	Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                                                             \
	Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                                                            \
	Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                            \
        VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                         \
        NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                  \
        NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                  \
        glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                           \
		glVertex3f(VertX, VertY, VertZ);                                                                         \
	    mi->polygon_count++;                                                                                     \
                                                                                                                 \
        Xf-=x[Fi]; Yf-=y[Fi]; Xa-=x[Fi]; Yb-=y[Fi];                                                              \
                                                                                                                 \
		Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                                                         \
		Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                                                        \
		Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                        \
        VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                         \
        NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                  \
        NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                  \
        glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                           \
		glVertex3f(VertX, VertY, VertZ);                                                                         \
	    mi->polygon_count++;                                                                                     \
                                                                                                                 \
      }                                                                                                          \
      Xf=(float)(Ri*x[Fi+1]);                                                                                      \
      Yf=(float)(Ri*y[Fi+1]);                                                                                      \
      Xa=Xf+0.001f; Yb=Yf+0.001f;                                                                                  \
      Factor=1-(((Xf2=sqr(Xf))+(Yf2=sqr(Yf)))*AmpVr2);                                                           \
      Factor1=1-((sqr(Xa)+Yf2)*AmpVr2);                                                                          \
      Factor2=1-((Xf2+sqr(Yb))*AmpVr2);                                                                          \
      VertX=Factor*Xf;        VertY=Factor*Yf;        VertZ=Factor*Zf;                                           \
      NeiAX=Factor1*Xa-VertX; NeiAY=Factor1*Yf-VertY; NeiAZ=Factor1*Zf-VertZ;                                    \
      NeiBX=Factor2*Xf-VertX; NeiBY=Factor2*Yb-VertY; NeiBZ=Factor2*Zf-VertZ;                                    \
      glNormal3f(VectMul(NeiAX, NeiAY, NeiAZ, NeiBX, NeiBY, NeiBZ));                                             \
      glVertex3f(VertX, VertY, VertZ);                                                                           \
      glEnd();                                                                                                   \
    }                                                                                                            \
  }                                                                                                              \
  VS=(Factor<0);	                                                                                             \
}

static void
draw_tetra(ModeInfo * mi)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);

	TRIANGLE(2.0f, mp->seno, mp->edgedivisions, (float)(0.5 / SQRT6), mp->VisibleSpikes);

	glPushMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef((float)-tetraangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	TRIANGLE(2.0f, mp->seno, mp->edgedivisions, 0.5 / SQRT6, mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180.0f + tetraangle), 0.5f, (float)(SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	TRIANGLE(2.0f, mp->seno, mp->edgedivisions, 0.5 / SQRT6, mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180.0f + tetraangle), 0.5f, (float)(-SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	TRIANGLE(2.0f, mp->seno, mp->edgedivisions, 0.5 / SQRT6, mp->VisibleSpikes);
}

static void
draw_cube(ModeInfo * mi)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);

	SQUARE(2.0f, mp->seno, mp->edgedivisions, 0.5f, mp->VisibleSpikes)

	glRotatef(cubeangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	SQUARE(2.0f, mp->seno, mp->edgedivisions, 0.5f, mp->VisibleSpikes)
	glRotatef(cubeangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	SQUARE(2.0f, mp->seno, mp->edgedivisions, 0.5f, mp->VisibleSpikes)
	glRotatef(cubeangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	SQUARE(2.0f, mp->seno, mp->edgedivisions, 0.5f, mp->VisibleSpikes)
	glRotatef(cubeangle, 0.0f, 1.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	SQUARE(2.0f, mp->seno, mp->edgedivisions, 0.5f, mp->VisibleSpikes)
	glRotatef((float)(2 * cubeangle), 0.0f, 1.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	SQUARE(2.0f, mp->seno, mp->edgedivisions, 0.5f, mp->VisibleSpikes)
}

static void
draw_octa(ModeInfo * mi)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);
	TRIANGLE(2, mp->seno, mp->edgedivisions, 1 / SQRT6, mp->VisibleSpikes);

	glPushMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef((float)(-180 + octaangle), 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	TRIANGLE(2.0f, mp->seno, mp->edgedivisions, (float)(1 / SQRT6), mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-octaangle), 0.5f, (float)(SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	TRIANGLE(2.0f, mp->seno, mp->edgedivisions, (float)(1 / SQRT6), mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)-octaangle, 0.5f, (float)(-SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	TRIANGLE(2.0f, mp->seno, mp->edgedivisions, (float)(1 / SQRT6), mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	TRIANGLE(2.0f, mp->seno, mp->edgedivisions, (float)(1 / SQRT6), mp->VisibleSpikes);
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef((float)(-180.0f + octaangle), 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	TRIANGLE(2, mp->seno, mp->edgedivisions, (float)(1 / SQRT6), mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-octaangle), 0.5f, (float)(SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[6]);
	TRIANGLE(2, mp->seno, mp->edgedivisions, (float)(1 / SQRT6), mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-octaangle), 0.5f, (float)(-SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[7]);
	TRIANGLE(2, mp->seno, mp->edgedivisions, (float)(1 / SQRT6), mp->VisibleSpikes);
}

static void
draw_dodeca(ModeInfo * mi)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

#define TAU ((SQRT5+1)/2)

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);

	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);

	glPushMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glPushMatrix();
	glRotatef((float)-dodecaangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef((float)-dodecaangle, (float)cos72, (float)sin72, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef((float)-dodecaangle, (float)cos72, (float)-sin72, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef((float)dodecaangle, (float)cos36, (float)-sin36, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
	glPopMatrix();
	glRotatef((float)dodecaangle, (float)cos36, (float)sin36, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[6]);
	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glPushMatrix();
	glRotatef((float)-dodecaangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[7]);
	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef((float)-dodecaangle, (float)cos72, (float)sin72, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[8]);
	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef((float)-dodecaangle, (float)cos72, (float)-sin72, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[9]);
	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef((float)dodecaangle, (float)cos36, (float)-sin36, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[10]);
	PENTAGON(1.0f, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
	glPopMatrix();
	glRotatef((float)dodecaangle, (float)cos36, (float)sin36, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[11]);
	PENTAGON(1, mp->seno, mp->edgedivisions, (float)(sqr(TAU) * sqrt((TAU + 2) / 5) / 2), mp->VisibleSpikes);
}

static void
draw_icosa(ModeInfo * mi)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[0]);

	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);

	glPushMatrix();

	glPushMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef((float)-icoangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[1]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (3 * SQRT3 + SQRT15) / 12, mp->VisibleSpikes);
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180.0f + icoangle), 0.5f, (float)(SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[2]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (3 * SQRT3 + SQRT15) / 12, mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180 + icoangle), 0.5f, (float)(-SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[3]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (3 * SQRT3 + SQRT15) / 12, mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180 + icoangle), 0.5f, (float)(SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[4]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (3 * SQRT3 + SQRT15) / 12, mp->VisibleSpikes);
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180 + icoangle), 0.5f, (float)(SQRT3 / 2), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[5]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (3 * SQRT3 + SQRT15) / 12, mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef((float)-icoangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[6]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (3 * SQRT3 + SQRT15) / 12, mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180 + icoangle), 0.5f, (float)(-SQRT3 / 2), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[7]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (3 * SQRT3 + SQRT15) / 12, mp->VisibleSpikes);
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180 + icoangle), 0.5f, (float)(-SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[8]);
	TRIANGLE(1.5, mp->seno, mp->edgedivisions, (3 * SQRT3 + SQRT15) / 12, mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef((float)-icoangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[9]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[10]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef((float)-icoangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[11]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180 + icoangle), 0.5f, (float)(SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[12]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180 + icoangle), 0.5, (float)(-SQRT3 / 2), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[13]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
	glPopMatrix();
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180 + icoangle), 0.5, (float)(SQRT3 / 2), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[14]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180 + icoangle), 0.5f, (float)(SQRT3 / 2), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[15]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef((float)-icoangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[16]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180 + icoangle), 0.5f, (float)(-SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[17]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	glRotatef((float)(-180.0f + icoangle), 0.5f, (float)(-SQRT3 / 2.0f), 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[18]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
	glPopMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef((float)-icoangle, 1.0f, 0.0f, 0.0f);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mp->MaterialColor[19]);
	TRIANGLE(1.5f, mp->seno, mp->edgedivisions, (float)((3 * SQRT3 + SQRT15) / 12), mp->VisibleSpikes);
}

ENTRYPOINT void
reshape_morph3d(ModeInfo * mi, int width, int height)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glViewport(0, 0, mp->WindW = (GLint) width, mp->WindH = (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
}

static void
pinit(ModeInfo * mi)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	glClearDepth(1.0);
	glColor3f(1.0, 1.0, 1.0);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_specular);

	switch (mp->object) {
		case 2:
			mp->draw_object = draw_cube;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialCyan;
			mp->MaterialColor[3] = MaterialMagenta;
			mp->MaterialColor[4] = MaterialYellow;
			mp->MaterialColor[5] = MaterialBlue;
			mp->edgedivisions = cubedivisions;
			mp->Magnitude = 2.0;
			break;
		case 3:
			mp->draw_object = draw_octa;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialBlue;
			mp->MaterialColor[3] = MaterialWhite;
			mp->MaterialColor[4] = MaterialCyan;
			mp->MaterialColor[5] = MaterialMagenta;
			mp->MaterialColor[6] = MaterialGray;
			mp->MaterialColor[7] = MaterialYellow;
			mp->edgedivisions = octadivisions;
			mp->Magnitude = 2.5;
			break;
		case 4:
			mp->draw_object = draw_dodeca;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialCyan;
			mp->MaterialColor[3] = MaterialBlue;
			mp->MaterialColor[4] = MaterialMagenta;
			mp->MaterialColor[5] = MaterialYellow;
			mp->MaterialColor[6] = MaterialGreen;
			mp->MaterialColor[7] = MaterialCyan;
			mp->MaterialColor[8] = MaterialRed;
			mp->MaterialColor[9] = MaterialMagenta;
			mp->MaterialColor[10] = MaterialBlue;
			mp->MaterialColor[11] = MaterialYellow;
			mp->edgedivisions = dodecadivisions;
			mp->Magnitude = 2.0;
			break;
		case 5:
			mp->draw_object = draw_icosa;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialBlue;
			mp->MaterialColor[3] = MaterialCyan;
			mp->MaterialColor[4] = MaterialYellow;
			mp->MaterialColor[5] = MaterialMagenta;
			mp->MaterialColor[6] = MaterialRed;
			mp->MaterialColor[7] = MaterialGreen;
			mp->MaterialColor[8] = MaterialBlue;
			mp->MaterialColor[9] = MaterialWhite;
			mp->MaterialColor[10] = MaterialCyan;
			mp->MaterialColor[11] = MaterialYellow;
			mp->MaterialColor[12] = MaterialMagenta;
			mp->MaterialColor[13] = MaterialRed;
			mp->MaterialColor[14] = MaterialGreen;
			mp->MaterialColor[15] = MaterialBlue;
			mp->MaterialColor[16] = MaterialCyan;
			mp->MaterialColor[17] = MaterialYellow;
			mp->MaterialColor[18] = MaterialMagenta;
			mp->MaterialColor[19] = MaterialGray;
			mp->edgedivisions = icodivisions;
			mp->Magnitude = 2.5;
			break;
		default:
			mp->draw_object = draw_tetra;
			mp->MaterialColor[0] = MaterialRed;
			mp->MaterialColor[1] = MaterialGreen;
			mp->MaterialColor[2] = MaterialBlue;
			mp->MaterialColor[3] = MaterialWhite;
			mp->edgedivisions = tetradivisions;
			mp->Magnitude = 2.5;
			break;
	}
	if (MI_IS_MONO(mi)) {
		int         loop;

		for (loop = 0; loop < 20; loop++)
			mp->MaterialColor[loop] = MaterialGray;
	}
}

ENTRYPOINT void
init_morph3d(ModeInfo * mi)
{
	morph3dstruct *mp;

	if (morph3d == NULL) {
		if ((morph3d = (morph3dstruct *) calloc(MI_NUM_SCREENS(mi),
					    sizeof (morph3dstruct))) == NULL)
			return;
	}
	mp = &morph3d[MI_SCREEN(mi)];
	mp->step = (float)NRAND(90);
	mp->VisibleSpikes = 1;

	if ((mp->hglrc = init_GL(mi)) != NULL) {

		reshape_morph3d(mi, MI_WIDTH(mi), MI_HEIGHT(mi));
		glDrawBuffer(GL_BACK);
		mp->object = MI_COUNT(mi);
		if (mp->object <= 0 || mp->object > 5)
			mp->object = NRAND(5) + 1;
		pinit(mi);
	} else {
		MI_CLEARWINDOW(mi);
	}
}

ENTRYPOINT void
draw_morph3d(ModeInfo * mi)
{
	HDC display = MI_DISPLAY(mi);
	HWND window = MI_WINDOW(mi);
	morph3dstruct *mp;

	if (morph3d == NULL)
		return;
	mp = &morph3d[MI_SCREEN(mi)];

	MI_IS_DRAWN(mi) = True;

	if (!mp->hglrc)
		return;

    mi->polygon_count = 0;
	wglMakeCurrent(display, mp->hglrc);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(0.0, 0.0, -10.0);

	if (!MI_IS_ICONIC(mi)) {
		glScalef((float)(Scale4Window * mp->WindH / mp->WindW), (float)Scale4Window, (float)Scale4Window);
		glTranslatef((float)(2.5 * mp->WindW / mp->WindH * sin(mp->step * 1.11)), (float)(2.5 * cos(mp->step * 1.25 * 1.11)), 0.0f);
	} else {
		glScalef((float)(Scale4Iconic * mp->WindH / mp->WindW), (float)Scale4Iconic, (float)Scale4Iconic);
	}

	glRotatef(mp->step * 100, 1, 0, 0);
	glRotatef(mp->step * 95, 0, 1, 0);
	glRotatef(mp->step * 90, 0, 0, 1);

	mp->seno = (float)((SINF(mp->step) + 1.0f / 3.0f) * (4.0f / 5.0f) * mp->Magnitude);

	if (mp->VisibleSpikes) {
#ifdef DEBUG_CULL_FACE
		int         loop;

		for (loop = 0; loop < 20; loop++)
			mp->MaterialColor[loop] = MaterialGray;
#endif
		glDisable(GL_CULL_FACE);
	} else {
#ifdef DEBUG_CULL_FACE
		int         loop;

		for (loop = 0; loop < 20; loop++)
			mp->MaterialColor[loop] = MaterialWhite;
#endif
		glEnable(GL_CULL_FACE);
	}

	mp->draw_object(mi);

	glPopMatrix();

	if (MI_IS_FPS(mi)) do_fps (mi);
	SwapBuffers(display);

	mp->step += 0.05f;
}

#ifndef STANDALONE
ENTRYPOINT void
change_morph3d(ModeInfo * mi)
{
	morph3dstruct *mp = &morph3d[MI_SCREEN(mi)];

	if (!mp->hglrc)
		return;

	mp->object = (mp->object) % 5 + 1;
	pinit(mi);
}
#endif /* !STANDALONE */

ENTRYPOINT void
release_morph3d(ModeInfo * mi)
{
	if (morph3d != NULL) {
		(void) free((void *) morph3d);
		morph3d = (morph3dstruct *) NULL;
	}
	FreeAllGL(mi);
}

#endif

XSCREENSAVER_MODULE ("Morph3D", morph3d)
