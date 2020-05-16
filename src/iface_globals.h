#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define ABS(x)			 (((x) > 0) ? (x) : (0-x))

#define SAIS_VERSION_NUMBER "v1.5"

//#define WINDOWED_MODE

typedef struct _t_gamesettings
{
	int32 dif_nebula;
	int32 dif_enemies;
	int32 dif_ship;
	int32 random_names;
	int8 opt_timerwarnings;
	int8 opt_mucrontext;
	int8 opt_timeremaining;
	int8 opt_mousemode;
	int8 opt_smoketrails;
	int8 opt_lensflares;
	int16 opt_volume;
	char captname[32];
	char shipname[32];
} t_gamesettings;

extern t_gamesettings settings;

// ******** GENERAL STUFF *******

int my_main();
int ik_eventhandler();
int Game_Init(void *parms=NULL);
int Game_Shutdown(void *parms=NULL);

// inputs
int key_pressed(int vk_code);  // FIXME: GET RID OF VK CODES!
int ik_inkey();  // returns ascii
void ik_showcursor();
void ik_hidecursor();
int ik_mclick(); // returns flags when mbutton down

// timers
void start_ik_timer(int n, int f);
void set_ik_timer(int n, int v);
int get_ik_timer(int n);
int get_ik_timer_fr(int n);

// INTERFACE GLOBALS
extern int ik_mouse_x;
extern int ik_mouse_y;
extern int ik_mouse_b;
extern int ik_mouse_c;
extern int must_quit;
extern int wants_screenshot;

extern int key_left;
extern int key_right;
extern int key_up;
extern int key_down;
extern int key_f[10];
extern int key_fire1;
extern int key_fire2;
extern int key_fire2b;

