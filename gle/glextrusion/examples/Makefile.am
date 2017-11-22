
noinst_PROGRAMS = 	\
  alpha         \
  beam          \
  candlestick   \
  cone          \
  cylinder      \
  helix         \
  helix2        \
  helix3        \
  helix4        \
  helixtex      \
  horn          \
  joincone      \
  joinoffset    \
  joinstyle     \
  screw         \
  taper         \
  texas         \
  transport     \
  twistoid      \
  twistex       \
  twoid

  

alpha_SOURCES = alpha.c mainjoin.c
beam_SOURCES = beam.c mainsimple.c
cone_SOURCES = cone.c mainsimple.c
cylinder_SOURCES = cylinder.c mainsimple.c
helix_SOURCES = helix.c mainsimple.c
helix2_SOURCES = helix2.c mainsimple.c
helix3_SOURCES = helix3.c mainsimple.c
helix4_SOURCES = helix4.c mainsimple.c
helixtex_SOURCES = helixtex.c texture.c maintex.c
horn_SOURCES = horn.c mainsimple.c
joincone_SOURCES = cone.c mainjoin.c
joinoffset_SOURCES = joinoffset.c mainjoin.c
joinstyle_SOURCES = cylinder.c mainjoin.c
screw_SOURCES = screw.c texture.c maintex.c
taper_SOURCES = taper.c mainsimple.c
texas_SOURCES = texas.c mainjoin.c
twistoid_SOURCES = twoid.c
twistex_SOURCES = twistoid.c texture.c maintex.c


EXTRA_DIST = \
  .cvsignore Makefile.examples rundemo README

noinst_HEADERS = \
  main.h \
  texture.h

AM_CPPFLAGS = \
  @X_CFLAGS@ \
  -I${top_srcdir} \
  -I${top_srcdir}/src 

LDADD = @X_LIBS@  -L../src/

docdir = $(datadir)/doc/gle/examples

doc_DATA = \
   COPYING \
   Makefile.examples \
   README \
   rundemo \
   main.h \
   texture.h \
   alpha.c \
   beam.c \
   candlestick.c \
   cone.c \
   cylinder.c \
   helix.c \
   helix2.c \
   helix3.c \
   helix4.c \
   helixtex.c \
   horn.c \
   joinoffset.c \
   mainjoin.c \
   mainsimple.c \
   maintex.c \
   screw.c \
   taper.c \
   texas.c \
   texture.c \
   transport.c \
   twoid.c \
   twistoid.c


run:
	rundemo
