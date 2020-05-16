// ----------------
//     TYPEDEFS
// ----------------

typedef struct _t_eventcard
{
	char name[32];
	char text[256];
	char text2[256];
	int32 type;
	int32 parm;
} t_eventcard;

enum ecard_keyids
{
	eckBegin,
	eckName,
	eckText,
	eckText2,
	eckType,
	eckParam,
	eckEnd,
	eckMax,
};

enum ecard_types
{
	card_event,
	card_ally,
	card_item,
	card_rareitem,
	card_lifeform,
	card_max,
};

// ----------------
// GLOBAL VARIABLES
// ----------------

extern t_eventcard		*ecards;
extern int32					num_ecards;

// ----------------
//    PROTOTYPES
// ----------------

void cards_init();
void cards_deinit();
void card_display(int n);
