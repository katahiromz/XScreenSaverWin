
man_MANS = 		\
  gle.3gle 		\
  gleExtrusion.3gle 	\
  gleHelicoid.3gle 	\
  gleLathe.3gle 	\
  glePolyCone.3gle 	\
  glePolyCylinder.3gle 	\
  gleScrew.3gle 	\
  gleSetJoinStyle.3gle 	\
  gleSetNumSides.3gle 	\
  gleSpiral.3gle 	\
  gleSuperExtrusion.3gle \
  gleTextureMode.3gle 	\
  gleToroid.3gle 	\
  gleTwistExtrusion.3gle         

man_RAW = $(man_MANS:.3gle=.man)

EXTRA_DIST = \
  .cvsignore \
  $(man_RAW) \
  Makefile.sgi

SUFFIXES = .man .3gle

# for the ordinary install, just copy the raw files ....
.man.3gle:
	cp -p $< $@

# preformat and compress  -- these should install in the preformat dir
.man.3gle.gz:
	nroff -man < $< > $*.nr
	gzip $*.nr
	mv $*.nr.gz $@

.man.ps:
	psroff -t -man < $< > $*.ps


CLEANFILES = $(man_MANS)
