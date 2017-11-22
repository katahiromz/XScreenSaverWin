## -*-m4-*-

dnl Process this file with autoreconf -vi to produce a configure script.

# FILE:
# configure.ac
#
# FUNCTION:
# implements checks for a variety of system-specific functions
#
# Contents:
#   Headers   - Autoconf header stuff
#   Variables - Hardcoded variables
#   Programs  - Check for programs binaries
#   Functions - Check for functions
#   With      - Check for --with/without options
#   Enable    - Check for --enable/disable options
#   Libraries - Check for libraries
#   Footer    - Autoconf footer stuff

### --------------------------------------------------------------------------
### Headers
### check for various programs, and stuff (do this first because later
### commands depend on them):

AC_INIT([glextrusion], 3.1.1, [], [gle], [https://github.com/linas/glextrusion])
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_MACRO_DIR([m4])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])],
                            [AC_SUBST([AM_DEFAULT_VERBOSITY],1)])

AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_LIBTOOL

AC_ISC_POSIX
AC_PROG_MAKE_SET
AC_HEADER_STDC

AC_CHECK_FUNCS(memcpy)

### --------------------------------------------------------------------------
### Variables
### Set up all the initial variable values...

# We should always see these errors...
CFLAGS="${CFLAGS} -Wall"

# Strict, or lenient tesselator?
AC_ARG_ENABLE(lenient-tess,
       [  --enable-lenient-tess    compile for lenient tesselator],
       AC_DEFINE(LENIENT_TESSELATOR,1),
       AC_DEFINE(DELICATE_TESSELATOR,1) )

# Disable texture mapping?
AC_ARG_ENABLE(auto-texture,
       [  --disable-auto-texture   compile with texture mapping disabled],
       AC_DEFINE(AUTO_TEXTURE,0),
       AC_DEFINE(AUTO_TEXTURE,1) )

# Should we build for old IrisGL?
AC_ARG_ENABLE(irisgl,
       [  --enable-irisgl          compile for old IrisGL/GL-3.2],
       AC_DEFINE(GL_32,1),
       AC_DEFINE(OPENGL_10,1) )

AC_ARG_ENABLE( debug,
       [  --enable-debug           compile with debugging flags set],
       CFLAGS="${CFLAGS} -g"
       LDFLAGS="${LDFLAGS} -g"
       AC_DEFINE(DEBUG_OUTPUT,1),  )

AC_ARG_ENABLE( profile,
       [  --enable-profile         compile with profiling set],
       CFLAGS="${CFLAGS} -pg"
       LDFLAGS="${LDFLAGS} -pg")

AC_ARG_ENABLE( warnings,
       [  --enable-warnings         compile with lots of warnings
generated],
       CFLAGS="${CFLAGS} -g -Wall -ansi -pedantic -Wwrite-strings -Wid-clash-31 -Wpointer-arith -Wcast-qual -Wcast-align -Wtraditional -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs -Wshadow -Woverloaded-virtual -Winline -felide-constructors -fnonnull-objects"
       LDFLAGS="${LDFLAGS} -g -Wall"
       )


### --------------------------------------------------------------------------
### Libraries
LIBS="$LIBS -lm"

# We're going to set up our own X configure variables.  These are only
# used in side configure.  At the end, we use them to set X_LIBS.
# This allows us to be careful about libarary ordering, in case that's
# important.

AC_PATH_XTRA
if test x"$no_x" = xyes; then
      AC_ERROR([Can not find X11 development headers or libraries.])
fi

# AC_CHECK_HEADER(GL/gl.h)  
# AC_CHECK_HEADER(GL/glut.h)  

# configure seems to work best when tested subroutines take no arguments
AC_CHECK_LIB(GL, glGetError,
        X_LIBS="-lGL $X_LIBS",
        AC_MSG_ERROR([Cannot find required GL library]),
        $X_PRE_LIBS $X_LIBS -lX11 $X_EXTRA_LIBS)

AC_CHECK_LIB(GLU, gluNewQuadric,
        X_LIBS="-lGLU $X_LIBS",
        AC_MSG_ERROR([Cannot find required GLU library]),
        $X_PRE_LIBS $X_LIBS -lX11 $X_EXTRA_LIBS)

# ----------------------------------------------------------
# Newest versions of glut require Xmu, which doesn't show up 
# by default in X_LIBS
AC_CHECK_LIB(Xmu, XmuLookupStandardColormap,
        X_LIBS="-lXmu $X_LIBS",
        AC_MSG_ERROR([Cannot find required Xmu library]),
        $X_PRE_LIBS $X_LIBS -lX11 $X_EXTRA_LIBS)

AC_CHECK_LIB(Xi, XListInputDevices,
        X_LIBS="-lXi -lXext $X_LIBS",
        AC_MSG_ERROR([Cannot find required Xi (X11 Input Extension) library]),
        $X_PRE_LIBS $X_LIBS -lX11 $X_EXTRA_LIBS)

# latest versions of glut also use Xi the XInput extension
AC_CHECK_LIB(glut, glutSwapBuffers,
        X_LIBS="-lglut $X_LIBS",
        AC_MSG_ERROR([Cannot find required glut library]),
        $X_PRE_LIBS -lXmu $X_LIBS -lX11 $X_EXTRA_LIBS)


# This is how to use the variables set by AC_PATH_XTRA:
#       cc @X_CFLAGS@ -c -o foo.o foo.c
#       cc @X_LIBS@ (-lfoo...) @X_PRE_LIBS@ -lX11 @X_EXTRA_LIBS@
# (Perhaps X_LIBS should have been called X_LDFLAGS.)

X_LIBS="-lgle $X_LIBS -lXt -lX11"

AC_OUTPUT(
          Makefile
          src/Makefile
          doc/Makefile
          doc/html/Makefile
          examples/Makefile
          man/Makefile
          ms-visual-c/Makefile
          ms-visual-c/gle/Makefile
          swig/Makefile
          )
