// ----------------
//    CONSTANTS
// ----------------

// for interface.cpp / interface_initsprites()
#define IF_BORDER_TRANS 0
#define IF_BORDER_SOLID 9
#define IF_BORDER_PORTRAIT 18
#define IF_BORDER_RADAR 19
#define IF_BORDER_FLAT 20
#define IF_BORDER_SMALL 21

enum tutorial_pages
{
	tut_starmap = 0,
	tut_explore,
	tut_upgrade,
	tut_device,
	tut_treasure,
	tut_ally,
	tut_encounter,
	tut_combat,
	tut_trading,
	tut_max,
};

// ----------------
//     TYPEDEFS
// ----------------

// ----------------
// GLOBAL VARIABLES
// ----------------

extern t_ik_spritepak *spr_titles;

extern t_ik_spritepak *spr_IFborder;
extern t_ik_spritepak *spr_IFbutton;
extern t_ik_spritepak *spr_IFslider;
extern t_ik_spritepak *spr_IFarrows;
extern t_ik_spritepak *spr_IFsystem;
extern t_ik_spritepak *spr_IFtarget;
extern t_ik_spritepak *spr_IFdifnebula;
extern t_ik_spritepak *spr_IFdifenemy;

extern t_ik_font	    *font_4x8;
extern t_ik_font	    *font_6x8;

// ----------------
//    PROTOTYPES
// ----------------

void interface_init();
void interface_deinit();

void interface_drawborder(t_ik_image *img, 
													int32 left, int32 top, int32 right, int32 bottom,
													int32 fill, int32 color,
													char *title);
void interface_thinborder(t_ik_image *img, 
													int32 left, int32 top, int32 right, int32 bottom,
													int32 color, int32 fill = -1);
int32 interface_textbox(t_ik_image *img, t_ik_font *fnt,
											 int32 left, int32 top, int32 w, int32 h,
											 int32 color, 
											 char *text);
int32 interface_textboxsize(t_ik_font *fnt,
														int32 w, int32 h,
														char *text);
int32 interface_popup(t_ik_font *fnt, 
										 int32 left, int32 top, int32 w, int32 h,
										 int32 co1, int32 co2, 
										 char *label, char *text,
										 char *button1 = NULL, char *button2 = NULL, char *button3 = NULL);
void interface_drawslider(t_ik_image *img, int32 left, int32 top, int32 a, int32 l, int32 rng, int32 val, int32 color);
void interface_drawbutton(t_ik_image *img, int32 left, int32 top, int32 l, int32 color, char *text);

void interface_cleartuts();
void interface_tutorial(int n);
