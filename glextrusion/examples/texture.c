
/*
 * texture.c
 *
 * FUNCTION:
 * texture mapping hack
 *
 * HISTORY:
 * Created by Linas Vepstas April 1994
 * Reaction-Diffusion added Dec 1996
 * Copyright (c) 1994, 1996, Linas Vepstas <linas@linas.org>
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "texture.h"

Texture * current_texture = 0x0;

Texture * planet_texture = 0x0;
Texture * check_texture = 0x0;
Texture * barberpole_texture = 0x0;
Texture * wild_tooth_texture = 0x0;
Texture * reaction_texture = 0x0;

void (*texanim)(int) = 0x0;
void (*reaction_texanim)(int) = 0x0;

/* ======================================================= */

#define TEXTURE_SIZE 256

static Texture * 
create_planet_texture (void) 
{
   int i, j;
   Texture * tex;
   unsigned char * pixmap;

   pixmap = (unsigned char *) malloc (TEXTURE_SIZE*TEXTURE_SIZE*3*sizeof (unsigned char));

   for (i=0; i< TEXTURE_SIZE; i++) {
      for (j=0; j< TEXTURE_SIZE; j++) {

         int mi = i - TEXTURE_SIZE/2;
         int mj = j - TEXTURE_SIZE/2;

         pixmap [3*TEXTURE_SIZE*i + 3*j] = 100*mi*mi + (40*mj*mj >> 8);
         pixmap [3*TEXTURE_SIZE*i + 3*j + 1] = (10*mi*mi + 4*mj*mj) ;
         pixmap [3*TEXTURE_SIZE*i + 3*j + 2] = (1000*mi*mi + 400*mj*mj) >> 16 ;

      }
   }

   tex = (Texture *) malloc (sizeof (Texture));
   tex -> size = TEXTURE_SIZE;
   tex -> pixmap = pixmap;

   return tex;
}

/* ======================================================= */

static Texture * 
create_check_texture (void) 
{
   int i, j;
   Texture * tex;
   unsigned char * pixmap;

   pixmap = (unsigned char *) malloc (TEXTURE_SIZE*TEXTURE_SIZE*3*sizeof (unsigned char));

   for (i=0; i< TEXTURE_SIZE; i++) {
      for (j=0; j< TEXTURE_SIZE; j++) {

         pixmap [3*TEXTURE_SIZE*i + 3*j] = 
		255 * ( (((i)/32) %2) == (((j)/32) %2));
         pixmap [3*TEXTURE_SIZE*i + 3*j + 1] = 
		255 * ( (((i)/32) %2) == (((j)/32) %2));
         pixmap [3*TEXTURE_SIZE*i + 3*j + 2] = 
		255 * ( (((i)/32) %2) == (((j)/32) %2));
      }
   }

   tex = (Texture *) malloc (sizeof (Texture));
   tex -> size = TEXTURE_SIZE;
   tex -> pixmap = pixmap;

   return tex;
}

/* ======================================================= */

static Texture * 
create_barberpole_texture (void) 
{
   int i, j;
   Texture * tex;
   unsigned char * pixmap;

   pixmap = (unsigned char *) malloc (TEXTURE_SIZE*TEXTURE_SIZE*3*sizeof (unsigned char));

   for (i=0; i< TEXTURE_SIZE; i++) {
      for (j=0; j< TEXTURE_SIZE; j++) {

         pixmap [3*TEXTURE_SIZE*i + 3*j] = 255 * (((i+j)/32) %2);
         pixmap [3*TEXTURE_SIZE*i + 3*j + 1] = 255 * (((i+j)/32) %2);
         pixmap [3*TEXTURE_SIZE*i + 3*j + 2] = 255 * (((i+j)/32) %2);

      }
   }

   tex = (Texture *) malloc (sizeof (Texture));
   tex -> size = TEXTURE_SIZE;
   tex -> pixmap = pixmap;

   return tex;
}

/* ======================================================= */

static Texture * 
create_wild_tooth_texture (void) 
{
   int i, j;
   Texture * tex;
   unsigned char * pixmap;

   pixmap = (unsigned char *) malloc (TEXTURE_SIZE*TEXTURE_SIZE*3*sizeof (unsigned char));

   for (i=0; i< TEXTURE_SIZE; i++) {
      for (j=0; j< TEXTURE_SIZE; j++) {

         pixmap [3*TEXTURE_SIZE*i + 3*j] = 
         255 * ( (((i+j)/32) %2) == (((i-j)/32) %2));
         pixmap [3*TEXTURE_SIZE*i + 3*j + 1] = 
         255 * ( (((i+j)/32) %2) == (((i-j)/32) %2));
         pixmap [3*TEXTURE_SIZE*i + 3*j + 2] = 
         255 * ( (((i+j)/32) %2) == (((i-j)/32) %2));

      }
   }

   tex = (Texture *) malloc (sizeof (Texture));
   tex -> size = TEXTURE_SIZE;
   tex -> pixmap = pixmap;

   return tex;
}

/* ======================================================= */
/* reaction-diffusion textures, witkin & kass, siggraph 1991 */
/* this is a quick and dirty, sloppy, non-cpu-efficient
 * non-parametric-corrected isotropic impelemtation. */

static float CPfield[TEXTURE_SIZE][TEXTURE_SIZE];
static float CMfield[TEXTURE_SIZE][TEXTURE_SIZE];
static float RPfield[TEXTURE_SIZE][TEXTURE_SIZE];
static float RMfield[TEXTURE_SIZE][TEXTURE_SIZE];
static float tmpfield[TEXTURE_SIZE][TEXTURE_SIZE];

int niter=0;

static Texture * 
create_reaction_texture (void)
{
   int i,j; 
   Texture * tex;
   unsigned char * pixmap;

   pixmap = (unsigned char *) 
              malloc (TEXTURE_SIZE*TEXTURE_SIZE*3* sizeof (unsigned char));

   for (i=0; i< TEXTURE_SIZE; i++) {
      for (j=0; j<TEXTURE_SIZE; j++) {
         CPfield[i][j] = ((float) ((i*j)%4096)) / 4096.0;
         CMfield[i][j] = ((float) ((i*i+j*j)%8192)) / 8192.0;
         RPfield[i][j] = 0.0;
         RMfield[i][j] = 0.0;
      }
   }

   for (i=0; i< TEXTURE_SIZE; i++) {
      for (j=0; j<TEXTURE_SIZE; j++) {
         pixmap [3*TEXTURE_SIZE*i+3*j] =  255.0 * CPfield[i][j];
         pixmap [3*TEXTURE_SIZE*i+3*j+1] = 127.0 * (CPfield[i][j]+CMfield[i][j]);
         pixmap [3*TEXTURE_SIZE*i+3*j+2] = 255.0 * CMfield[i][j];
      }
   }

   tex = (Texture *) malloc (sizeof (Texture));
   tex -> size = TEXTURE_SIZE;
   tex -> pixmap = pixmap;

   reaction_texture = tex;
   return tex;
}

static float delta_t = 0.1;
static float h = 1.0;
static float ap = 1.0;
static float am = 0.5;
static float b = 0.0;
static float r = 0.5;

static void 
iterate_reaction_texture (int reset)
{
   int i,j, im1, ip1, jm1, jp1; 
   int iter;
   char * pixmap;
   float tmp;
   float cpmax, cpmin, cmmax, cmmin, cps, cms;
   float cpr, cmr;

   niter ++;
   printf ("diffusion-reaction iteration %d \n", niter);

   cpmax = -1.0e50;
   cpmin = +1.0e50;

   cmmax = -1.0e50;
   cmmin = +1.0e50;

   if (reset) {
      niter = 0;
      for (i=0; i< TEXTURE_SIZE; i++) {
         for (j=0; j<TEXTURE_SIZE; j++) {
            CPfield[i][j] = ((float) ((i*j)%4096)) / 4096.0;
            CMfield[i][j] = ((float) ((i*i+j*j)%8192)) / 8192.0;
            RPfield[i][j] = 0.0;
            RMfield[i][j] = 0.0;
         }
      }
   } else {

      for (iter=0; iter<1; iter ++) {

         /* reaction-diffusion */
         for (i=0; i< TEXTURE_SIZE; i++) {
            im1 = (i+TEXTURE_SIZE-1)%TEXTURE_SIZE;
            ip1 = (i+1)%TEXTURE_SIZE;
            for (j=0; j<TEXTURE_SIZE; j++) {
               jm1 = (j+TEXTURE_SIZE-1)%TEXTURE_SIZE;
               jp1 = (j+1)%TEXTURE_SIZE;

               tmp = CPfield[im1][j];
               tmp += CPfield[ip1][j];
               tmp += CPfield[i][jm1];
               tmp += CPfield[i][jp1];
               tmp -= 4.0 * CPfield[i][j];
               tmp *= ap*ap / (h*h);
               tmp -= b* CPfield[i][j];
               tmp += RPfield[i][j];
               tmpfield[i][j] = tmp;
            }
         }
         for (i=0; i< TEXTURE_SIZE; i++) {
            for (j=0; j<TEXTURE_SIZE; j++) {
               CPfield[i][j] += delta_t * tmpfield[i][j];
               if (cpmax < CPfield[i][j]) cpmax = CPfield[i][j];
               if (cpmin > CPfield[i][j]) cpmin = CPfield[i][j];
            }
         }
      
         for (i=0; i< TEXTURE_SIZE; i++) {
            im1 = (i+TEXTURE_SIZE-1)%TEXTURE_SIZE;
            ip1 = (i+1)%TEXTURE_SIZE;
            for (j=0; j<TEXTURE_SIZE; j++) {
               jm1 = (j+TEXTURE_SIZE-1)%TEXTURE_SIZE;
               jp1 = (j+1)%TEXTURE_SIZE;

               tmp = CMfield[im1][j];
               tmp += CMfield[ip1][j];
               tmp += CMfield[i][jm1];
               tmp += CMfield[i][jp1];
               tmp -= 4.0 * CMfield[i][j];
               tmp *= am*am / (h*h);
               tmp -= b* CMfield[i][j];
               tmp += RMfield[i][j];
               tmpfield[i][j] = tmp;
            }
         }
         for (i=0; i< TEXTURE_SIZE; i++) {
            for (j=0; j<TEXTURE_SIZE; j++) {
               CMfield[i][j] += delta_t * tmpfield[i][j];
               if (cmmax < CMfield[i][j]) cmmax = CMfield[i][j];
               if (cmmin > CMfield[i][j]) cmmin = CMfield[i][j];
            }
         }
      
         for (i=0; i< TEXTURE_SIZE; i++) {
            for (j=0; j<TEXTURE_SIZE; j++) {
               if ( CPfield[i][j] > CMfield[i][j]) {
                  RPfield[i][j] = r;
                  RMfield[i][j] = r;
               } else {
                  RPfield[i][j] = 0.0;
                  RMfield[i][j] = 0.0;
               }
            }
         }
      }
   }


   pixmap = reaction_texture -> pixmap;
   cps = 1.0 / (cpmax-cpmin);
   cms = 1.0 / (cmmax-cmmin);
   for (i=0; i< TEXTURE_SIZE; i++) {
      for (j=0; j<TEXTURE_SIZE; j++) {
         cpr = cps * (CPfield[i][j]-cpmin);
         cmr = cms * (CMfield[i][j]-cmmin);
         cpr = CPfield[i][j];
         cmr = CMfield[i][j];
         pixmap [3*TEXTURE_SIZE*i+3*j] = 255.0 * cpr;
         pixmap [3*TEXTURE_SIZE*i+3*j+1] = 70.0 * (cpr+cmr);
         pixmap [3*TEXTURE_SIZE*i+3*j+2] = 130.0 * cmr;
      }
   }
}

/* ======================================================= */

void setup_textures (void) {

   planet_texture = create_planet_texture ();
   check_texture = create_check_texture ();
   barberpole_texture = create_barberpole_texture ();
   wild_tooth_texture = create_wild_tooth_texture ();
   reaction_texture = create_reaction_texture ();

   reaction_texanim = iterate_reaction_texture;

   current_texture = wild_tooth_texture;
   current_texture = check_texture;
}


/* ================== END OF FILE ========================= */
