// ----------------
//    CONSTANTS
// ----------------

#define STARMAP_FRAMERATE 50

#define SM_MAP_X 160
#define SM_MAP_Y 0
#define SM_SHIP_X 0
#define SM_SHIP_Y 0
#define SM_INV_X 0
#define SM_INV_Y 256
#define SM_SEL_X 0
#define SM_SEL_Y 384

//#define STARMAP_BUILD_HELP
//#define LOG_OUTPUT

//#define STARMAP_DEBUGINFO 
//#define STARMAP_DEBUGENEMIES
//#define STARMAP_DEBUGTANRU
//#define STARMAP_DEBUGALLIES
//#define STARMAP_DEBUGEVENTS
//#define STARMAP_DEBUGFOLD
//#define STARMAP_DEBUGDEVICES
//#define STARMAP_DEBUGCLOAK
//#define STARMAP_DEBUGRAREITEMS
//#define STARMAP_STEPBYSTEP
//#define STARMAP_DEBUGSYSTEMS
//#define STARMAP_KAWANGI

#define STARMAP_INTERFACE_COLOR 11

#define STARMAP_MAX_FLEETS 8

#ifndef DEMO_VERSION
#define RC_MUCRON 9
#define RC_PLANET 10
#define RC_FOMAX 11
#define RC_BLOWUP 12
#define RC_HOLED 13
#define RC_LOST 14
#else 
#define RC_MUCRON 4
#define RC_PLANET 4
#define RC_FOMAX 5
#define RC_BLOWUP 5
#define RC_HOLED 6
#define RC_LOST 5
#endif

#ifdef DEMO_VERSION
	#define NUM_STARSYSTEMS 8
	#define NUM_FLEETS		3
	#define NUM_EVENTS		1
	#define NUM_ALLIES		2
	#define NUM_ITEMS			2
	#define NUM_RAREITEMS	1
	#define NUM_LIFEFORMS	1
#else
	#define NUM_FLEETS		5
	#define NUM_STARSYSTEMS 16
#ifdef STARMAP_DEBUGEVENTS
	#define NUM_EVENTS		8 // 2
	#define NUM_ALLIES		2
	#define NUM_ITEMS			1	// 6
	#define NUM_RAREITEMS	1 // 2
	#define NUM_LIFEFORMS	3
#else 
#ifdef STARMAP_DEBUGRAREITEMS
	#define NUM_EVENTS		2
	#define NUM_ALLIES		2
	#define NUM_ITEMS			1
	#define NUM_RAREITEMS	7
	#define NUM_LIFEFORMS	3
#else
	#define NUM_EVENTS		2
	#define NUM_ALLIES		2
	#define NUM_ITEMS			6
	#define NUM_RAREITEMS	2
	#define NUM_LIFEFORMS	3
#endif
#endif
#endif


// ----------------
//     TYPEDEFS
// ----------------

typedef struct _t_hud
{
	int32 invslider;
	int32 invselect;
	int32 sysslider;
	int32 sysselect;
} t_hud;


typedef struct _t_starsystem
{
	char starname[16];
	char planetname[16];
	int32 x, y;
	int32 color;
	int32 planet;
	int32 planetgfx;
	int32 novadate;
	int32 novatype;
	int32 novatime;

	int32 card;
	int32 explored;
	int32 ds_x, ds_y;
} t_starsystem;

typedef struct _t_planettype
{
	char name[16];
	char text[172];
	int32 bonus;
} t_planettype;

typedef struct _t_startype
{
	char name[16];
	char text[176];
} t_startype;

typedef struct _t_blackhole
{
	char name[20];
	int32 x, y;
	int16 size;
	int16 explored;
} t_blackhole;

typedef struct _t_nebula
{
	int32 x, y;
	int32 sprite;
} t_nebula;

typedef struct _t_player
{
	char captname[32];
	char shipname[32];
	char deathmsg[64];

	int32 x,y,a;
	int32 system;
	int32 target;

	int32 distance;
	int32 nebula;
	int32 enroute;

	int32 engage;
	int32 fold;
	int32 hypdate;
	int32 foldate;
	int32 hyptime;

	int32 explore;
	int32 stardate;	
	int32 death;
	int32 deatht;
	int32 hole;

	int32 num_ships;
	int32 ships[8];
	int32 sel_ship;
	int32 sel_ship_time;

	int32 items[32];
	int32 itemflags[32];
	int32 num_items;
	int32 bonusdata;
} t_player;

typedef struct _t_itemtype
{
	char name[32];
	char text[256];
	char clas[32];
	int32 type;
	int32 cost;
	int32 index;
	int32 flag;
	int32 sound;
	int32 loopsnd;
} t_itemtype;

typedef struct _t_fleet
{
	int32 system;
	int32 target;
	int32 enroute;
	int32 distance;
	int32 num_ships;
	int32 ships[16];
	int32 race;
	int32 explored;
	int32 blowtime;
} t_fleet;				// enemy "fleet"

typedef struct _t_month
{
	char name[16];
	char longname[16];
	int32 sd, le;
} t_month;

typedef struct _t_race
{
	char name[16];
	char text[256];
	char text2[64];
	int32 met;
	int32 fleet;
} t_race;

typedef struct _t_racefleet
{
	int32 race;
	int32 num_fleets;
	int32 stype[3];
	int32 fleets[9][3];
	int32 diff[3][10];
} t_racefleet;

enum planet_keyids
{
	plkBegin,
	plkName,
	plkText,
	plkBonus,
	plkEnd,
	plkMax,
};

enum star_keyids
{
	stkBegin,
	stkName,
	stkText,
	stkEnd,
	stkMax,
};

enum item_keyids
{
	itkBegin,
	itkName,
	itkType,
	itkText,
	itkClass,
	itkCost,
	itkFlag,
	itkSound,
	itkEnd,
	itkMax,
};

enum item_flags
{
	item_broken = 1,
	item_owed = 2,
};

enum item_deviceflags
{
	device_beacon = 4,
	device_probe = 8,
	device_collapser = 16,
	device_mirror = 32,
	device_bauble = 64,
	device_gong = 128,
	device_mantle = 256,
	device_torc = 512,
	device_conograph = 1024,
	lifeform_hard = 2048,
	lifeform_ambassador = 4096,
};

enum item_types
{
	item_weapon = 0,
	item_system,
	item_device,
	item_lifeform,
	item_treasure,
};

enum raceflt_keyids
{
	rflBegin,
	rflRace,
	rflShip1,
	rflShip2,
	rflShip3,
	rflFleet,
	rflEasy,
	rflMedium,
	rflHard,
	rflEnd,
	rflMax,
};

// ----------------
// GLOBAL VARIABLES
// ----------------

extern t_month				months[12];

extern t_race					races[16];
extern int32					num_races;

extern t_racefleet		racefleets[16];
extern int32					num_racefleets;

extern t_planettype		*platypes;
extern int32					num_platypes;

extern t_startype			*startypes;
extern int32					num_startypes;

extern t_itemtype			*itemtypes;
extern int32					num_itemtypes;

extern t_starsystem		*sm_stars;
extern int32					num_stars;

extern t_blackhole		*sm_holes;
extern int32					num_holes;

extern t_nebula				*sm_nebula;
extern int32					num_nebula;

extern t_fleet				sm_fleets[STARMAP_MAX_FLEETS];

extern t_ik_spritepak *spr_SMstars;
extern t_ik_spritepak *spr_SMstars2;
extern t_ik_spritepak *spr_SMplanet;
extern t_ik_spritepak *spr_SMplanet2;
extern t_ik_spritepak *spr_SMnebula;
extern t_ik_spritepak *spr_SMraces;

extern uint8					*sm_nebulamap;
extern t_ik_image			*sm_nebulagfx;
extern t_ik_image			*sm_starfield;

extern int32					star_env[8][8];
//extern char						pltype_name[10][32];
extern int32					plgfx_type[256];
extern int32					num_plgfx;

extern int32					homesystem;
extern t_player				player;
extern t_hud					hud;

extern int32					kla_items[32];
extern int32					kla_numitems;

extern char	captnames[64][16];
extern int32 num_captnames;
extern char	shipnames[64][16];
extern int32 num_shipnames;

extern int starmap_tutorialtype;

// ----------------
//    PROTOTYPES
// ----------------

void player_init();
void allies_init();

void starmap_init();
void starmap_create();
void starmap_createnebulagfx();

void starmap_deinit();

void starmap();
void starmap_display(int32 t);


void starmap_removeship(int32 n);

void starmap_advancedays(int32 n);

int starmap_stardist(int32 s1, int32 s2);
int starmap_nebuladist(int32 s1, int32 s2);
void starmap_sensefleets();


// ---------------------
// starmap_encounters.cpp
// ---------------------

// normal exploration
int32 starmap_entersystem();
void starmap_exploreplanet();
int32 starmap_explorehole(int32 h, int32 t);

// alien encounters
int32 fleet_encounter(int32 flt, int32 inc=0);
void starmap_meetrace(int32 r);
void klakar_encounter();
void enemy_encounter(int32 r);

#ifndef DEMO_VERSION
void starmap_mantle(int32 flt);
int32 muktian_encounter();

// kawangi encounter
void kawangi_warning();
void kawangi_message(int32 flt, int32 m);
void starmap_kawangimove(int flt);
#endif

// ---------------------
// starmap_inventory.cpp
// ---------------------

// regular inventory management
void starmap_installitem(int32 n);
void starmap_uninstallsystem(int32 n, int32 brk);
void starmap_destroysystem(int32 n);
void starmap_additem(int32 it, int32 brk);
void starmap_removeitem(int32 n);
int32 ally_install(int32 s, int32 it, int32 pay);
int32 select_weaponpoint();
int32 item_colorcode(int32 it);

// klakar
void klakar_trade();

// mercenary
int32 pay_item(char *title, char *text, int r, char klak = 0);

// use artifact
#ifndef DEMO_VERSION
int32 use_vacuum_collapser(char *title);
void vacuum_collapse(int st);
void eledras_mirror(char *title);
int32 eledras_bauble(char *title);
void use_conograph(char *title);
#endif

int32 probe_fleet_encounter(int32 flt);
void probe_exploreplanet(int32 probe);
int32 stellar_probe(char *title);
