//#define MOVIE

// ******** GRAPHICS *********

// GFX DATATYPES

typedef struct
{
	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
} t_paletteentry;

typedef struct {
	int32 w,h;		// size
	int32 pitch;  // how many bytes per hline
	uint8 *data;	// linear bitmap
} t_ik_image;

typedef struct {
	int32 w,h;    // size
	uint32 co;     // average color
	uint8 *data;	// linear bitmap 
} t_ik_sprite;

typedef struct {
	int32 num;
	t_ik_sprite **spr;
} t_ik_spritepak;

typedef struct {
	uint16 w,h;		// size
	uint8 *data;	// linear bitmap 
} t_ik_font;

// GFX GLOBALS

extern t_ik_image *screen;
extern uint8 globalpal[768];
extern uint8 currentpal[768];
extern t_ik_image screenbuf;
extern int gfx_width, gfx_height, gfx_fullscreen, gfx_switch;
extern int gfx_redraw;
extern int c_minx, c_miny, c_maxx, c_maxy;

extern unsigned char *gfx_transbuffer;
extern unsigned char *gfx_lightbuffer;
extern unsigned char *gfx_addbuffer;

extern int32 sin1k[1024];
extern int32 cos1k[1024];

// load, generate or delete images
t_ik_image *new_image(int32 w, int32 h);
void del_image(t_ik_image *img);
t_ik_image *ik_load_pcx(char *fname, uint8 *pal);
t_ik_image *ik_load_tga(char *fname, uint8 *pal);
void ik_save_screenshot(t_ik_image *img, uint8 *pal);
void ik_save_tga(char *fname, t_ik_image *img, uint8 *pal);

// input/output
void ik_setclip(int32 left, int32 top, int32 right, int32 bottom);
void ik_putpixel(t_ik_image *img, int32 x, int32 y, uint32 c);
int32 ik_getpixel(t_ik_image *img, int32 x, int32 y);
uint8 *ik_image_pointer(t_ik_image *img, int32 x, int32 y);
void ik_drawline(t_ik_image *img, int32 xb, int32 yb, int32 xe, int32 ye, int32 c1, int32 c2=0, uint8 mask=255, uint8 fx=0);
void ik_drawbox(t_ik_image *img, int32 xb, int32 yb, int32 xe, int32 ye, int32 c);
void ik_copybox(t_ik_image *src, t_ik_image *dst, int32 xb, int32 yb, int32 xe, int32 ye, int32 xd, int32 yd);
void ik_drawmeter(t_ik_image *img, int32 xb, int32 yb, int32 xe, int32 ye, int32 typ, int32 val, int32 c, int32 c2);
void ik_draw_mousecursor();
void gfx_blarg();
void gfx_magnify();

// screen blits & other management
void prep_screen(); // call before drawing stuff to *screen
void free_screen(); // call after drawing, before blit
void ik_blit();         // blit from memory to hardware
int gfx_checkswitch();  // check for gfx mode switch
void halfbritescreen();
void reshalfbritescreen();
void resallhalfbritescreens();

// palette handling
void update_palette();  // blit palette entries to hardware
void set_palette_entry(int n, int r, int g, int b);
int get_palette_entry(int n);
int32 get_rgb_color(int32 r, int32 g, int32 b);
void calc_color_tables(uint8 *pal);
void del_color_tables();

// misc
int get_direction(int32 dx, int32 dy);
int get_distance(int32 dx, int32 dy);

void gfx_initmagnifier();
void gfx_deinitmagnifier();



// ------------------------
//         FONT.CPP
// ------------------------

t_ik_font *ik_load_font(char *fname, uint8 w,  uint8 h);
void ik_del_font(t_ik_font *fnt);

void ik_print(t_ik_image *img, t_ik_font *fnt, int32 x, int32 y, uint8 co, char *ln, ...);
void ik_printbig(t_ik_image *img, t_ik_font *fnt, int32 x, int32 y, uint8 co, char *ln, ...);
//void ik_text_input(int x, int y, int l, t_ik_font *fnt, char *tx);
void ik_text_input(int x, int y, int l, t_ik_font *fnt, char *pmt, char *tx, int bg=0, int co=0);
void ik_hiscore_input(int x, int y, int l, t_ik_font *fnt, char *tx);

// ------------------------
//      SPRITES.CPP
// ------------------------

// sprite management
t_ik_sprite *			new_sprite(int32 w, int32 h);
void							free_sprite(t_ik_sprite *spr);

t_ik_sprite *			get_sprite(t_ik_image *img, int32 x, int32 y, int32 w, int32 h);
int32							calc_sprite_color(t_ik_sprite *spr);

t_ik_spritepak *	new_spritepak(int32 num);
void							free_spritepak(t_ik_spritepak *pak);

t_ik_spritepak *	load_sprites(char *fname);
void							save_sprites(char *fname, t_ik_spritepak *pak);


// sprite drawing
void ik_dsprite(t_ik_image *img, int32 x, int32 y, t_ik_sprite *spr, int32 flags=0);
void ik_drsprite(t_ik_image *img, int32 x, int32 y, int32 r, int32 s, t_ik_sprite *spr, int32 flags=0);
void ik_dspriteline(t_ik_image *img, int32 xb, int32 yb, int32 xe, int32 ye, int32 s, 
										int32 offset, int32 ybits, t_ik_sprite *spr, int32 flags=0);
