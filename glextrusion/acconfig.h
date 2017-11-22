/********************************************************************\
 * acconfig.h -- configuration defines for gle                      *
 * Copyright (C) 2000 Linas Vepstas (linas@linas.org)               *
\********************************************************************/


#ifndef __GLE_CONFIG_H__
#define __GLE_CONFIG_H__

/* Package name and version number */
#undef PACKAGE
#undef VERSION

/* Standard C headers present */
#undef STDC_HEADERS

/* limits.h header present */
#undef HAVE_LIMITS_H

/* Build for OpenGL by default, and not for old IrisGL aka GL 3.2 */
#define  OPENGL_10 1          
#undef   GL_32 

/* Disable debugging stuff (debugging replaces GL output with printfs) */
#undef   DEBUG_OUTPUT

/* Do we have a lenient tesselator? */
#undef  LENIENT_TESSELATOR
#define DELICATE_TESSELATOR 1

/* Enable texture mapping by default. */
#define AUTO_TEXTURE 1

/* assume a modern C compiler */
#undef FUNKY_C


#endif
