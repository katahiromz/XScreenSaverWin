
/* Copyright (c) Mark J. Kilgard, 1995. */

/* This program is freely distributable without licensing fees 
   and is provided without guarantee or warrantee expressed or 
   implied. This program is -not- in the public domain. */

#if 0
	#if 0   /* for Mesa */
	# include "glutint.h"
	#else   /* for xscreensaver */
	# ifdef HAVE_CONFIG_H
	#  include "config.h"
	# endif
	# ifndef HAVE_COCOA
	#  include <GL/gl.h>
	# endif
	# ifdef HAVE_JWZGLES
	#  include "jwzgles.h"
	# endif /* HAVE_JWZGLES */
	# undef APIENTRY
	# define APIENTRY /**/
	#endif
#endif

#include "xws2win.h"
#include <GL/gl.h>
#undef APIENTRY
#define APIENTRY

#include "glutstroke.h"

/* CENTRY */
int APIENTRY 
glutStrokeWidth(GLUTstrokeFont font, int c)
{
  StrokeFontPtr fontinfo;
  const StrokeCharRec *ch;

  fontinfo = (StrokeFontPtr) font;

  if (c < 0 || c >= fontinfo->num_chars)
    return 0;
  ch = &(fontinfo->ch[c]);
  if (ch)
    return ch->right;
  else
    return 0;
}

int APIENTRY 
glutStrokeLength(GLUTstrokeFont font, const unsigned char *string)
{
  int c, length;
  StrokeFontPtr fontinfo;
  const StrokeCharRec *ch;

  fontinfo = (StrokeFontPtr) font;

  length = 0;
  for (; *string != '\0'; string++) {
    c = *string;
    if (c >= 0 && c < fontinfo->num_chars) {
      ch = &(fontinfo->ch[c]);
      if (ch)
        length += ch->right;
    }
  }
  return length;
}

/* ENDCENTRY */
