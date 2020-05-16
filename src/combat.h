//#define DEBUG_COMBAT

//#define COMBAT_BUILD_HELP

// ----------------
//		CONSTANTS
// ----------------

enum combat_tactics
{
	tac_pursue=0,
	tac_attack,
	tac_move,
	tac_flee,
};

#define MAX_COMBAT_SHIPS 32
#define MAX_COMBAT_PROJECTILES 256
#define MAX_COMBAT_BEAMS 256
#define MAX_COMBAT_EXPLOS 256

#ifndef MOVIE
#define COMBAT_FRAMERATE 50
#else
#define COMBAT_FRAMERATE 17
#endif

#define COMBAT_INTERFACE_COLOR (11+simulated)

// ----------------
//     TYPEDEFS
// ----------------

typedef struct _t_hardpoint
{
	int8 type;
	int8 size;
	int8 x;
	int8 y;
	int16 a;		// angle
	int16 f;		// field of vision / fire
} t_hardpoint;

typedef struct _t_hull
{
	char name[32];
	int32 size;			// length in meters
	int32 hits;
	int32 mass;
	int32 numh;			// number of hardpoints
	t_ik_sprite *sprite;
	t_ik_sprite *silu;
	t_hardpoint hardpts[32];
} t_hull;

typedef struct _t_shiptype
{
	char name[32];
	int32 race;
	int32 flag;
	int32 hull;
//	int32 shield;
	int32 hits;
	int32 engine;
	int32 thrust;
	int32 speed;
	int32 turn;
	int32 sensor;
	int32 num_systems;
	int32 sys_eng, sys_thru;
//	int32 weapon[8];
	int16 system[16];
	int16 sysdmg[16];
} t_shiptype;

typedef struct _t_ship
{
	char name[32];
	int32 type;
	int32 hits;

	int32 shld;
	int32 shld_type;
	int32 shld_time;
	int32 shld_charge;

	int32 damage_time;
	int32	dmgc_type;
	int32 dmgc_time;

	int32 cpu_type;
	int32 ecm_type;
	int32 clo_type;

	int32 sys_thru;
	int32 sys_shld;
	int32 sys_dmgc;
	int32 sys_cpu;
	int32 sys_ecm;
	int32 sys_clo;

	int32 speed, turn;

	int32 wepfire[8];
	int32 syshits[16];
	int32 own;

	int32 x, y, a;			// location
	int32 vx, vy, va;		// movement

	int32 ds_x, ds_y, ds_s;		// display x, y, size (for mouse clicking)
	int32 wp_x, wp_y, escaped, wp_time, flee;
	int32 patx, paty;	// patrol

	int32 cloaked, cloaktime;	// cloaktime is last time you cloaked/decloaked

	int32 tel_x, tel_y, teltime;	// teleportation of zorg

	int32 active;	// for spacehulk
	int32 aistart;

	int32 target;
	int32 tac;
	int32 angle;
	int32 dist;

	int32 launchtime;	// for carrier
	int32 frange;

	int32 bong_start, bong_end;	// for babulon's bong artifact

} t_ship;

typedef struct _t_shipweapon
{
	char name[32];
	int32 stage;
	int32 type;
	int32 flags;
	t_ik_sprite *sprite;
	int32 size;
	int32 sound1;
	int32 sound2;
	int32 rate;
	int32 speed;
	int32 damage;
	int32 range;
	int32 item;
} t_shipweapon;

typedef struct _t_shipsystem
{
	char name[32];
	int16 type;
	int16 size;
	int32 par[4];
	int32 item;
} t_shipsystem;

typedef struct _t_wepbeam
{
	t_shipweapon *wep;
	t_ship *src;
	t_ship *dst;

	int32 stg;					// if staged from a projectile
	int32 ang, len;			// angle and length if missed
	int32 stp, dsp;			// start, destination hardpoint (-1 = hull / shield)
	int32 str, dmt, end;			// start, damage, expire time
} t_wepbeam;

typedef struct _t_wepproj
{
	t_shipweapon *wep;
	t_ship *src;
	t_ship *dst;				// target (for missiles)

	int32 str, end;			// start, expire time

	int32 x, y, a;			// location
	int32 vx, vy, va;		// movement
	int32 hits;					// used for dispersing weapons
} t_wepproj;

typedef struct _t_explosion
{
	t_ik_spritepak *spr;

	int32 x, y, a;
	int32 vx, vy, va;
	int32 str, end;
	int32 size, zoom;
	int32 spin, fade;
	int32 anim;
	int32 cam;
} t_explosion;

typedef struct _t_combatcamera
{
	int32 x, y, z;
	int32 ship_sel;
	int32 ship_trg;
	int32 time_sel;
	int32 time_trg;
	int32 drag_trg;
} t_combatcamera;


enum hull_keyids
{
	hlkBegin,
	hlkName,
	hlkSize,
	hlkHits,
	hlkMass,
	hlkSprite,
	hlkSilu,
	hlkWeapon,
	hlkEngine,
	hlkThruster,
	hlkFighter,
	hlkEnd,
	hlkMax,
};

enum hull_hardpttypes
{
	hdpWeapon,
	hdpEngine,
	hdpThruster,
	hdpFighter,
	hdpMax,
};

enum shiptype_keyids
{
	shkBegin,
	shkName,
	shkRace,
	shkFlag,
	shkHull,
	shkArmor,
	shkSystem,
	shkEngine,
	shkThruster,
	shkWeapon,
	shkEnd,
	shkMax,
};

enum weapon_keyids
{
	wpkBegin,
	wpkName,
	wpkStage,
	wpkType,
	wpkFlag,
	wpkSprite,
	wpkSize,
	wpkSound1,
	wpkSound2,
	wpkRate,
	wpkSpeed,
	wpkDamage,
	wpkRange,
	wpkEnd,
	wpkMax,
};

enum weapon_flagids
{
	wpfTrans = 1,
	wpfSpin = 2,
	wpfDisperse = 4,
	wpfImplode = 8,
	wpfHoming = 16,
	wpfSplit = 32,
	wpfShock1 = 64,
	wpfShock2 = 128,
	wpfNova = 256,
	wpfWiggle = 512,
	wpfStrail = 1024,
	wpfNoclip = 2048,
	wpfMax = 12,
};

enum system_keyids
{
	sykBegin,
	sykName,
	sykType,
	sykSize,
	sykParam1,
	sykParam2,
	sykParam3,
	sykParam4,
	sykEnd,
	sykMax,
};

enum system_types
{
	sys_weapon,
	sys_thruster,
	sys_engine,
	sys_shield,
	sys_computer,
	sys_ecm,
	sys_sensor,
	sys_damage,
	sys_misc,
};

enum race_keyids
{
	rckBegin,
	rckName,
	rckText,
	rckText2,
	rckEnd,
	rckMax,
};

#ifndef DEMO_VERSION
enum race_ids
{
	race_none,
	race_terran,
	race_klakar,
	race_zorg,
	race_muktian,
	race_garthan,
	race_tanru,
	race_urluquai,
	race_kawangi,
	race_unknown,
	race_drone,
	race_max,
};
#else
enum race_ids
{
	race_none,
	race_terran,
	race_klakar,
	race_garthan,
	race_unknown,
	race_drone,
	race_max,
};
#endif


// ----------------
// GLOBAL VARIABLES
// ----------------

extern int IsMinimized;


extern t_ik_image			*combatbg1;
extern t_ik_image			*combatbg2;

extern t_ik_spritepak *spr_ships;
extern t_ik_spritepak *spr_shipsilu;
extern t_ik_spritepak *spr_weapons;
extern t_ik_spritepak *spr_explode1;
extern t_ik_spritepak *spr_shockwave;
extern t_ik_spritepak	*spr_shield;

extern t_hull					*hulls;
extern int						num_hulls;

extern t_shiptype			*shiptypes;
extern int						num_shiptypes;

extern t_shipweapon		*shipweapons;
extern int						num_shipweapons;

extern t_shipsystem		*shipsystems;
extern int						num_shipsystems;

extern t_ship					*ships;		// in combat
extern int						num_ships;

extern char						racename[16][32];
extern int						num_races;
extern int						enemies[16];
extern int						num_enemies;

extern t_combatcamera		camera;
extern t_ship						cships[MAX_COMBAT_SHIPS];
extern t_wepbeam					cbeams[MAX_COMBAT_BEAMS];
extern t_wepproj					cprojs[MAX_COMBAT_PROJECTILES];
extern t_explosion				cexplo[MAX_COMBAT_EXPLOS];

extern int32 numships;
extern int32 playership;
extern int32 sortship[MAX_COMBAT_SHIPS];

extern int32 t_move, t_disp, pause;

extern int32 nebula;
extern int32 retreat;
extern int32 klaktime;
extern int32 klakavail;
extern int32 gongavail;

extern int32 simulated;

#ifdef DEBUG_COMBAT
extern char combatdebug[64];
#endif

// ----------------
//    PROTOTYPES
// ----------------

// combat_init.cpp

void combat_init();
void combat_deinit();
void sort_shiptype_systems(int32 num);

// combat.cpp

int32 combat(int32 flt, int32 sim);
void select_ship(int32 s, int32 t);
void combat_updateshipstats(int32 s, int32 t);
void combat_findstuff2do(int32 s, int32 t);
void combat_help_screen();
void combat_SoundFX(int id, int srcx = camera.x, int volume = -1, int rate = -1);

// combat_sim.cpp

void combat_sim();
void combat_sim_end();

// combat_display.cpp

void combat_autocamera(int32 t);
void combat_display(int32 t);

// combat_weapons.cpp

int32 combat_findtarget(t_ship *ship, int32 hdp);
void combat_fire(t_ship *src, int32 hdp, t_ship *trg, int32 start);
int32 combat_addbeam(t_shipweapon *wep, t_ship *src, int32 hdp, t_ship *trg, int32 start, int32 stg = -1);
int32 combat_addproj(t_ship *src, int32 hdp, t_ship *trg, int32 start);
void combat_launchstages(int32 p, int32 num, int32 start);
int32 combat_addexplo(int32 x, int32 y, t_ik_spritepak *spr, int32 spin, int32 size, int32 zoom, int32 start, int32 end, int32 anim=-1, int32 cam=1);
void combat_damageship(int32 s, int32 src, int32 dmg, int32 t, t_shipweapon *wep, int32 deb=0);
void combat_gethardpoint(t_ship *ship, int32 hdp, int32 *rx, int32 *ry);
void combat_killship(int32 s, int32 t, int32 quiet=0);
