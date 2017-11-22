
/*
 * texture.h
 *
 * FUNCTION:
 * texture mapping hack
 *
 * HISTORY:
 * Created by Linas Vepstas April 1994
 */

typedef struct {
   int size;
   unsigned char * pixmap;
} Texture;

extern Texture * current_texture;

extern Texture * planet_texture;
extern Texture * check_texture;
extern Texture * barberpole_texture;
extern Texture * wild_tooth_texture;
extern Texture * reaction_texture;

extern void (*texanim) (int);
extern void (*reaction_texanim) (int);

extern void setup_textures (void);

extern void TextureStyle (int msg); 



/* ================== END OF FILE ========================= */
