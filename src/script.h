typedef struct single_trigger SINGLE_TRIGGER;	/* Holds triggers */
typedef struct code_segment CODE;		/* Holds trigger code */
typedef struct script_info SCRIPT_INFO;		/* Holds info of a single trigger running */

/* M = Mob, R = Room, O = Object */

#define TENTERS			 1	/* MR */
#define TLEAVES			 2	/* MR */
#define TCREATED		 3	/* M */
#define TCOMMAND		 4	/* MRO */
#define TMOVES			 5	/* MO */
#define TDESTROYED		 6	/* MO */
#define TSAYS			 7	/* MOR */
#define TTELLS	 		 8	/* M */
#define TEVERY_HOUR		 9	/* MOR */
#define TEVERY_QUICKTICK	10	/* MOR */
#define TEVERY_REGENTICK	11	/* MR */
#define TAT_HOUR		12	/* MOR */
#define TGIVEN			13	/* MOR */
#define TDROPPED		14	/* RO */

/* New below */

#define TEQUIPPED		15	/* O */
#define TATTACKED		16	/* M */
#define TOBJ_SACCED		17	/* OR */
#define TGIVEN_MONEY		18	/* M */

#define TMAX_TRIGGERS		19

/* Script_info are running scripts.. we need to update these often */

struct script_info
  {
    SCRIPT_INFO *next;
    SINGLE_TRIGGER *called_by;	/* Trigger it was called by */
    CHAR_DATA *current;		/* Person currently being looked at by the script */
    CHAR_DATA *mob;		/* If a mob is executing this is the mob */
    ROOM_DATA *room;		/* If a room is executing this is the room */
    SINGLE_OBJECT *obj;		/* If an obj is executing this is the obj */
    short delay_ticks;
    char tick_type;		/* 1=quick, 2=heartbeat, 3=hour, 4=superquick */
    short current_line;		/* Line of code it will execute next */
    char code_seg[10];		/* Current label of code we're in */
    int accumulator;		/* Single register process */
  };

/*Hashed by first letter of label */
struct code_segment
  {
    CODE *next;
    char label[10];
    char *code;
  };

struct single_trigger
  {
    SINGLE_TRIGGER *next;
    char trigger_id[20];
    int trigger_type;		/* Trigger type */
    char *keywords;		/* General keywords */
    int attached_to_room;
    int attached_to_mob;
    int attached_to_obj;
    char interrupted;		/* Will the trigger reset if it is interrupted? */
    char leaves_room;		/* Will the script stop if the player leaves the room? */
    char players_only;		/* Will this script trigger on players only? */
    char code_label[10];	/* Label of code to execute */
    char handle_leave[10];	/* Code label to jump to if char leaves (for ROOM AND MOB only) */
    SCRIPT_INFO *running_info;
  };

extern CHAR_DATA *debugger;
extern CODE *code_list[256];	/* hashed by starting letter */
extern SCRIPT_INFO *info_list;	/* vnum % 256 for hash table */
extern SINGLE_TRIGGER *trigger_list[TMAX_TRIGGERS];	/* Hashed by trigger type */

void load_triggers(void);
void load_code(void);
void end_script(SCRIPT_INFO *);
void execute_code(SCRIPT_INFO *);

