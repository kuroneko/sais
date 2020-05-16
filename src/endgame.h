// ----------------
//    CONSTANTS
// ----------------

// ----------------
//     TYPEDEFS
// ----------------

typedef struct _t_job
{
	char name[64];	
	int32 value;
} t_job;

typedef struct _t_score
{
	char cname[16], sname[16];
	char deathmsg[64];
	int32 score;
	int32 date;
} t_score;

// ----------------
// GLOBAL VARIABLES
// ----------------

extern t_job		*jobs;
extern int32		num_jobs;

extern int32    num_scores;
extern t_score  scores[20];

extern int32		got_hiscore;

// ----------------
//    PROTOTYPES
// ----------------

void game_over();

void endgame_init();
void endgame_deinit();