/* config.h.in.  Generated from configure.ac by autoheader.  */
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
#define STDC_HEADERS

/* limits.h header present */
#define HAVE_LIMITS_H 1

/* Build for OpenGL by default, and not for old IrisGL aka GL 3.2 */
#define  OPENGL_10 1          
#define GL_32 1

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

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#undef LT_OBJDIR

/* Name of package */
#undef PACKAGE

/* Define to the address where bug reports for this package should be sent. */
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
#undef PACKAGE_TARNAME

/* Define to the home page for this package. */
#undef PACKAGE_URL

/* Define to the version of this package. */
#undef PACKAGE_VERSION

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Version number of package */
#undef VERSION

/* Define to 1 if the X Window System is missing or not being used. */
#undef X_DISPLAY_MISSING
