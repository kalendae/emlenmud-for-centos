



#define PIC_WAIT 15
//#define WINDOWS 1 

#ifndef WINDOWS
#define false    0
#define true     1

typedef int socket_t;


#else
#include "winsock.h"
#include "windows.h"

typedef SOCKET socket_t;
#define NOCRYPT
#endif

#define FALSE    0
#define TRUE     1
#define yes      1
#define no       0
#define YES      1
#define NO       0

/* Allow for boolean type... */
typedef unsigned char bool;
extern socket_t control;

typedef struct scriptassoc SA;
typedef struct social_type SOCIAL;
typedef struct fight_data FGT;
typedef struct dammsg_data DAM_MSG;
typedef struct power_data POWER;
typedef struct shell_command SHELL;
typedef struct channel_type CHANNEL;
typedef struct cmd_type COMMAND;
typedef struct to_free TO_FREE;
typedef struct item_herb I_HERB;
typedef struct item_wood I_WOOD;
typedef struct auct_data AUCTION;
typedef struct item_book I_BOOK;
typedef struct item_armor I_ARMOR;
typedef struct item_weapon I_WEAPON;
typedef struct item_drink_container I_DRINK;
typedef struct item_light I_LIGHT;
typedef struct item_tool I_TOOL;
typedef struct item_hide I_HIDE;
typedef struct item_raw I_RAW;
typedef struct item_cloth I_CLOTH;
typedef struct item_trap I_TRAP;
typedef struct item_vehicle I_VEHICLE;
typedef struct item_fountain I_FOUNTAIN;
typedef struct item_potion I_POTION;
typedef struct item_spray I_SPRAY;
typedef struct item_mineral I_MINERAL;
typedef struct item_food I_FOOD;
typedef struct item_gem I_GEM;
typedef struct item_wand I_WAND;
typedef struct item_furniture I_FURNITURE;
typedef struct item_container I_CONTAINER;
typedef struct item_climbable I_CLIMBABLE;
typedef struct item_ammo I_AMMO;
typedef struct affect_data AFFECT_DATA;
typedef struct area_data AREA_DATA;
typedef struct ban_data BAN_DATA;
typedef struct voting_data VOTING_DATA;
typedef struct spell_data SPELL_DATA;
typedef struct gld_data GUILD_DATA; /* Guild data struct */
typedef struct dam_moves PERCENT_STRUCT;
typedef struct damages_struct DAM_STRUCT;
typedef struct mna_data MANA_DATA;
typedef struct track_data TRACK_DATA;
typedef struct ptrack_data PTRACK_DATA; /* JRAJRA - New tracking stuff */
typedef struct char_data CHAR_DATA;
typedef struct online ONLINE; /* JRAJRA - keep track of who has been online */
typedef struct potion_ingredients POTION_INFO;
typedef struct topten_data TOPTEN_DATA;
typedef struct rating_data RATING_DATA;
typedef struct pk_data PK_DATA;
typedef struct bid_data BID_DATA;
typedef struct descriptor_data DESCRIPTOR_DATA;
typedef struct exit_data EXIT_DATA;
typedef struct ced CED_DATA;
typedef struct door_data DOOR_DATA;
typedef struct extra_descr_data DESCRIPTION_DATA;
typedef struct playerbase_data PLAYERBASE_DATA;
typedef struct variable_data VARIABLE_DATA;
typedef struct item_check ITEM_CHECK;
typedef struct help_data HELP_DATA;
typedef struct mob_proto_data MOB_PROTOTYPE;
typedef struct script_dat SCRIPT_DAT;
typedef struct note_data NOTE_DATA;
typedef struct obj_boat OBJ_BOAT;
typedef struct b_list BLIST;
typedef struct obj_data SINGLE_OBJECT;
typedef struct obj_proto_data OBJ_PROTOTYPE;
typedef struct temp_str TEMP_STR;
typedef struct pc_data REALCHAR_DATA;
typedef struct npc_data MOB_DATA;
typedef struct optional_data OPTIONAL_DATA;
typedef struct room_data ROOM_DATA;
typedef struct sector_data SECTOR_DATA;
typedef struct trap_data TRAP_DATA; /* JRAJRA */
typedef struct room_extended ROOM_MORE;
typedef struct reset_data RESET_DATA;
typedef struct shop_data SHOP_DATA;
typedef struct time_info_data TIME_INFO_DATA;
typedef struct weather_data WEATHER_DATA;
typedef struct clan_data CLAN_DATA;
typedef struct bank_item BANK_ITEM;

typedef void CHR_FUNCTION (CHAR_DATA *, CHAR_DATA *, char *argy);
typedef void DO_COMMAND (CHAR_DATA *, char *);
typedef bool SPEC_FUNCTION (CHAR_DATA * ch);


/* All game-related DEFINES */

#define HASH_MAX       1564     /* Max list heads for rooms, objs, mobs */
#define STD_LENGTH     3048     /* Standard length string, 2048 */
#define SML_LENGTH     1524     /* Small length string, 512 */

#define MAX_MOB_TRO    7000     /* Maximum mob trophies, up to vnum 7000 */
                                /* Increasing this adds mem usage. */

/* ------------------------------------------------------------------------- */
/* If you are in between battleground_start_vnum and the end vnum, you are
   auto-considered to be in the battleground, and the mud will behave as if
   you are in the battleground. */
/* ------------------------------------------------------------------------- */
#define BATTLEGROUND_START_VNUM       600  /* Start bground room vnum */
#define BATTLEGROUND_END_VNUM         650  /* End bground room vnum */

#define INDOOR_FLY_HEIGHT              24  /* Things above this height cannot
					     fly indoors - JRAJRA */
#define SKILL_COUNT                   600	/* Must be 600 or higher for the */
					        /* Dynamic skills system */
#define NUM_STATS                    5 /* 5 stats str int wis dex con */

#define NUM_ROLLS                    4 /* VERY unlikely to be changed */


#define DESC_CRASHER       0x00000001
#define DESC_ROLLING       0x00000002
#define DESC_ROLLED_YET    0x00000004
#define DESC_TPCOMMAND     0x00000008
#define DESC_WRONG_PWD     0x00000010
#define DESC_INSERT        0x00000020
#define DESC_BAD_LETTER    0x00000040

#define STR                           0
#define INT                           1
#define WIS                           2
#define DEX                           3
#define CON                           4

#define NUM_ALIGNMENTS                4     /* Coding for 4+newbie
                                               aligments atm*/
#define NUM_ALIGN                     4     /* Cuz I forget what I call it
                                               sometimes */
#define TOP_LEVEL                     110       /* The maximum level. */
#define MAX_NUM_BG_PRIZES             10      /* Max num of bg prizes */
#define MAX_COLORS                    8		/* Maximum player configurable colors */
#define RACE_COUNT                    25	/* Max races is 25. Don't change! */
#define MAX_CLAN_STORAGE              30	/* How many items can a clan
						   have in their clan storage? */
#define MAXST                         15	/* How many items can person
						   have in person storage? */


#define MAX_IGNORE		      20        /* Max number of ignore
					           slots a player has */

#define MAX_TROPHY		      20        /* Max number of trophies
				                   that someone has */
#define MAX_SCRIPT_FLAGS              500       /* Max number of scriptflags
                                                   a person can have */

#define MAP_WIDTH                     35        /* Overhead mapping width*/
#define MAP_HEIGHT                    11       /* Overhead mapping height*/

#define MAX_TOPTEN                    10        /* Max num on topten */

#define MAX_RATING                    10        /* Max num on rating */

#define MAX_PKDATA                    20        /* Max number listed for pk */
#define PKN                           12       /* Types of pk info */

#define WGT_MULT                      10       /* The number of pebbles per
                                                  stone in weight ratios. */
#define SKILL                         0     /* Spell slot == 0 is skill */
#define SPELL                         1     /* Spell slot == 1 is spell */
#define PROF                          2     /* Spell slot == 2 is prof */


#define MAX_GROUP_SIZE		      0       /* 0 == off, anything else is checked */
#define GROUP_POINTS_ENABLED	      0       /* 1 == TRUE, 0 == FALSE, duh!        */
#define MAX_GROUP_FOLLOWERS	      0       /* 0 == off, anything else is checked */

#define BASE_ROLLING_POINTS	      27	/* base points used to roll a character */

#define PK_QUIT_TIMER		      8         /* Number of hours before
						   you can quit to pk */
#define MOB_QUIT_TIMER		      1         /* Number of hours before
						   you can quit after
						   fighting a mob */
#define SCRY_TIMER		      8         /* Time between scrys   */
#define LOCATE_PERSON_TIMER           6         /* Time between locates */
#define IDENTIFY_TIMER		      0         /* Time between idents  */
#define SUMMON_TIMER                  6         /* Time between summons */
#define RECALL_TIMER                  1         /* Time between recalls */
#define TELEPORT_TIMER                7         /* Time between teleps  */
#define MESSAGE_TIMER                 6         /* Time between message */

#define PK_RECALL_CHANCE	      3         /* 1 in this number chance
                                                   of a person recalling
                                                   with no_quit_pk >0 */
#define PK_SUMMON_CHANCE              2         /* 1 in this number chance
                                                   of summoning someone
                                                   with a pkill timer */
#define PK_TELEPORT_CHANCE            2         /* 1 in this number chance
				 		   of a person teleporting
						   while in pk noquit.  */
#define E_LOCATE_ROOM             12300         /* Evils loc from here */
#define G_LOCATE_ROOM             13307         /* Goods loc from here */

#define E_REMORT_ROOM             12300         /* Evils type remort here */
#define G_REMORT_ROOM             13307         /* Goods type remort here */
#define REMORT_ROOM                1200         /* All must type remort here */

#define COLOR_CHAT                    0
#define COLOR_CLANTALK                1
#define COLOR_IMMTALK                 6
#define COLOR_YELL                    2
#define COLOR_SAY                     3
#define COLOR_TELL                    4
#define COLOR_GODTALK                 5

#define MAX_TRADE                       5	/* Don't change this. */

#define STOREDIV                       14	/*item cost is divided by this to
						  determine the cost it will be to
						  store the item */
#define MAX_LEVEL		   110		/* max possible level */
#define LEVEL_BUILDER              103		/* Builder level */
#define LEVEL_IMMORTAL             100		/* Minimum immortal level */
#define IMM_LEVEL                  100          /* New Min immortal level */
/* ---------------------- */
/* Do not change these... */
/* ---------------------- */
#define PULSE_PER_SECOND             10
#define PULSE_VIOLENCE               15
#define PULSE_MOBILE            /*6*/  (6 * PULSE_PER_SECOND)
#define PULSE_TICK               /* 35*/ (35 * PULSE_PER_SECOND)
#define PULSE_AREA              /* 120 */  (120 * PULSE_PER_SECOND)

/* ------------------------------------------------------ */
/* Time of day defines.  Compatable with diku style muds. */
/* ------------------------------------------------------ */

#define SUN_DARK                    0
#define SUN_RISE                    1
#define SUN_MORNING                 2
#define SUN_MIDDAY                  3
#define SUN_AFTERNOON               4
#define SUN_EVENING                 5
#define SUN_SET                     6


/* ----------------------------------------------------------------------- */
/* Weather system model from Theisles, although I cannot guarentee similar
   implimentation... */
/* ----------------------------------------------------------------------- */
#define SKY_CLOUDLESS               0
#define SKY_CLOUDY                  1
#define SKY_RAINING                 2
#define SKY_LIGHTNING               3
#define SKY_FOGGY                   4

/* ------------------------------------------------------------------------- */
/* Fight option defines.  Can be toggled on/off through the 'fight' command. */
/* ------------------------------------------------------------------------- */
#define F_HEADBUTT      0x00000001
#define F_JUJI          0x00000002
#define F_ELBOW         0x00000004
#define F_KNEE          0x00000008
#define F_BITE          0x00000010
#define F_CHOKE         0x00000020

#define F_AGGRESSIVE    0x00010000
#define F_POWER         0x00020000
#define F_GROUND        0x00040000

/* --------------------------------------- */
/* POSITIONS for ground_pos.  Many unused. */
/* --------------------------------------- */
#define FP_CHOKE_1      0x00000001
#define FP_CHOKE_2      0x00000002
#define FP_CHOKE_3      0x00000004
#define FP_STUNNED      0x00000008
#define FP_BLEEDING     0x00000010
#define FP_TOO_HEAVY    0x00000020
#define FP_PUNCHING     0x00000040

/* --------------------------------------------------------------------- */
/* These bits describe a spell's behavior, for use with the online spell
   creation/editing system. */
/* --------------------------------------------------------------------- */

#define SPELL_ALL_IN_ROOM     0x00000001	/* Affects everyone in the room */
#define SPELL_HURT_OPP_ALIGN  0x00000004	/* Works only on players of diff aligns */
#define SPELL_HURT_UNDEAD     0x00000010	/* Harms only undead */
#define SPELL_TWO_SAVES       0x00000040	/* Victim gets 2 saves, so the possibility of resisting is greatly increased. */
#define SPELL_ONLY_MULTI_MOBS 0x00000080	/* Only affects multiple mobs. for use in conjunction with all_in_room */
#define SPELL_HALVED_SAVE     0x00000100	/* Spell damage is divided in half if the victim makes a saving throw. */
#define SPELL_NOT_SELF        0x00000200	/* Spell cannot be cast on the caster him/herself. */
#define SPELL_ONLY_GROUP      0x00000400	/* Only affects people of your  alignment in the room.  For use with all_in_room */
#define SPELL_ONLY_NOT_GROUP  0x00000800	/* Only affects people not of your alignment in the room. For use with all_in_room. */
#define SPELL_SUMMON          0x00001000 	/* Summons a mob/character. mob summoning can be toggled on/off in power.dat */
#define SPELL_TELEPORT        0x00002000	/* Teleports the victim */
#define SPELL_RAISE_UNDEAD    0x00004000	/* Raise a corpse as a slave */
#define SPELL_HEALS_DAM       0x00008000	/* Instead of removing damage on the victim in the dam field, the spell will heal this dam. */
#define SPELL_MESSAGE         0x00010000	/* Sends a message to another player */
#define SPELL_DISPEL_MAGIC    0x00020000	/* Dispels magic */
#define SPELL_IDENTIFY        0x00040000	/* Behaves as an identify spell */
#define SPELL_LOCATE_OBJECT   0x00080000	/* Behaves as locate object spell */
#define SPELL_SCRY            0x00100000	/* Behaves as scry spell - used to look into the room where someone else in the game is. */
#define SPELL_LOCATE_PERSON   0x00200000	/* Behaves as locate spell - used to see the room name where someone is in. */
#define SPELL_RECALL          0x00400000	/* The recall spell */
#define SPELL_MASS_DISTORTION 0x00800000    	/* Mass distortion...half */
#define SPELL_ADD_MOVE        0x01000000	/* Adds the 'dam' field to the victim's moves instead of doing damage to the hps. */
#define SPELL_REMOVES_BIT     0x02000000	/* Removes a bit (like poison) */
#define SPELL_OBJ_TO_ROOM     0x04000000	/* If the spell creates an object, it will go into the room instead of the character's */
#define SPELL_MODIFY_OBJECT   0x08000000	/* NOT DONE YET */
#define SPELL_LIFE_TAP	      0x10000000	/* Sucks the life out of a victim */
#define SPELL_ICE             0x00000001    	/* Ice spell */
#define SPELL_SHOCK           0x00000002    	/* Electric spell */
#define SPELL_WIND            0x00000004    	/* Wind spell */
#define SPELL_RANGED          0x00000008    	/* Ranged Spell */
#define SPELL_AREA            0x00000010    	/* Multiroom spell */
#define SPELL_SHARD           0x00000020    	/* Shard breath */
#define SPELL_VAMPIRIC        0x00000040	/* Drains hps to caster */
#define SPELL_ACID            0x00000080	/* Eats equipment */
#define SPELL_FIRE            0x00000100	/* Burns equipment */


/* --------------------------------------------------------------------------- */
/* Connected states, similar to MERC's, used to define what state a connection
   is in. */
/* --------------------------------------------------------------------------- */
#define CON_PLAYING                      0	/* Playing the game */
#define CON_STATE_ONE                    1	/* At login screen */
#define CON_GET_OLD_PASSWORD             2	/* Entering password */
#define CON_CONFIRM_NEW_NAME             3	/* New user, confirming new name */
#define CON_GET_NEW_PASSWORD             4	/* Asking for new password */
#define CON_CONFIRM_NEW_PASSWORD         5	/* Confirming new password */
#define CON_GET_NEW_SEX                  6	/* Get sex of new user */
#define CON_GET_NEW_RACE                 7	/* Get race of new user */
#define CON_CONFIRM_NEW_RACE             8	/* Confirm this race */
#define CON_READ_MOTD                   10	/* Reading the message of the day */
#define CON_CONFIRM_ROLL                11	/* Confirming stat roll */
#define CON_GET_NEW_ANSI                12	/* Asking for ANSI */
#define CON_ASK_MENU                    13	/* Login Menu.  Unused. */
#define CON_SHOW_TITLE                  14	/* Showing the title screen. */
#define CON_GET_PAGELEN                 15	/* Asking page length */
#define CON_CHANNELD                    16	/* Special for mudlink */
#define CON_RWHO			17	/* Executing remote who */
#define CON_ASCENDING			18	/* Player is ascending */
#define CON_FINISHED_ASCENDING		19	/* Player is done ascending */
#define CON_GET_HARDCORE                20      /* Asking if they are HC */
#define CON_AEDITOR                     -1	/* Area Editor */
#define CON_REDITOR                     -2	/* Room Editor */
#define CON_MEDITOR                     -3	/* Mob Editor */
#define CON_OEDITOR                     -4	/* Object Editor */
#define CON_CEDITOR                     -5	/* Clan Editor */
#define CON_FSEDITOR                    -7	/* Fullscreen Editor */
#define CON_PEDITOR                     -8	/* Player editor */
#define CON_SEDITOR                     -9	/* Spell Editor */
#define CON_GODMENU                    -10	/* God Menu */
#define CON_SOCEDIT                    -11	/* Social Editor */
#define CON_SCRIPTEDITOR               -12	/* Script/trigger Editor */
#define CON_SHELL		       -13	/* Shell */

/* ----------------------------------------------- */
/* Used for ACT calls, similar to diku style 'act' */
/* ----------------------------------------------- */
#define TO_ROOM             0	/* Display to the room, not to char */
#define TO_NOTVICT          1	/* Display to the room, not char or victim */
#define TO_VICT             2	/* Display to the victim */
#define TO_CHAR             3	/* Display to the character */
#define TO_ROOM_SPAM        4	/* Display TO_ROOM only if spam mode off */
#define TO_NOTVICT_SPAM     5	/* Display TO_NOTVICT only if spam mode off */

#define TO_VICT_SPAM        6	/* Display TO_VICT only if spam mode off */
#define TO_CHAR_SPAM        7	/* Display TO_CHAR only if spam mode off */

#define RC_GOODRACE     0x04000000	/* Mask for good race */
#define RC_EVILRACE     0x08000000	/* Mask for evil race */
#define RC_NEUTRALRACE  0x10000000	/* Unused. */

/* ---------- */
/* Mob types. */
/* ---------- */
#define MOB_HUMAN               0
#define MOB_ELF                 1
#define MOB_DWARF               2
#define MOB_GNOME               3
#define MOB_GIANT               4
#define MOB_DRAGON              5
#define MOB_CANINE              6
#define MOB_RODENT              7
#define MOB_INSECT              8
#define MOB_UNDEAD              9
#define MOB_GOLEM               10
#define MOB_DEMON               11
#define MOB_TROLL               12
#define MOB_BIRD                13
#define MOB_FISH                14
#define MOB_STATUE              15
#define MOB_FELINE              16
#define MOB_PLANT               17
#define MOB_GENERAL_ANIMAL      18
#define MOB_FAERIE              19
#define MOB_REPTILE             20
#define MOB_GHOST               21
#define MOB_OTHER               22
#define MOB_OGRE                23
#define MOB_ELEMENTAL           24
#define MOB_ORC                 25
#define MOB_DUMMY               27
#define MOB_ARACHNID            28
#define MOB_GNOLL               29
#define MOB_BUGBEAR             30
#define MOB_SKAVEN              31
#define MOB_TROGLODYTE          32
#define MOB_GOBLIN              33
#define MOB_ILLITHID            34
#define MOB_KENDER              35
#define MOB_MINOTAUR            36
#define MOB_SPRITE              37
#define MOB_DRACONIAN           38
#define MOB_HOBGOBLIN           39
#define MOB_HOBBIT              40
#define MOB_CENTAUR             41
#define MOB_MUTANT              42
#define MOB_HORSE               43
#define MAX_MOB_TYPE            44

extern char *const mob_type[MAX_MOB_TYPE];
extern char *const mob_attack[MAX_MOB_TYPE];
/* --------------------------------------------------------------------- */
/* Hit types for weapons.  Some unused.  Tried to maintain compatability
   with diku type muds... */
/* --------------------------------------------------------------------- */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000	/* Max 1000 dynamic spells */
#define TYPE_SLASH                   1001
#define TYPE_PIERCE                  1002
#define TYPE_WHIP                    1003
#define TYPE_POLEARM                 1004
#define TYPE_POUND                   1005
#define TYPE_SUCTION                 1006
#define TYPE_SHOT                    1090
#define MAX_ATTACK                   17		/* Unused! (?) max index+1 */
#define MAX_ATTACK_TYPES             8

/* -------------------------------------------------- */
/* Maximum player defined aliases a person can set up */
/* -------------------------------------------------- */
#define MAXALIAS      50


/* ----------------------------------- */
/* Color defines and other ANSI codes. */
/* ----------------------------------- */
#define BLACK      "\x1b[30m"
#define RED        "\x1b[31m"
#define GREEN      "\x1b[32m"
#define YELLOW     "\x1b[33m"
#define BLUE       "\x1b[34m"
#define PURPLE     "\x1b[35m"
#define CYAN       "\x1b[36m"
#define GREY       "\x1b[37m"

#define B_BLACK    "\x1b[40m"
#define B_RED      "\x1b[41m"
#define B_GREEN    "\x1b[42m"
#define B_YELLOW   "\x1b[43m"
#define B_BLUE     "\x1b[44m"
#define B_PURPLE   "\x1b[45m"
#define B_CYAN     "\x1b[46m"
#define B_GREY     "\x1b[47m"

#define CLRSCR     "\x1b[2J"
#define CLREOL     "\x1b[K"

#define DOWNARROW    "\x1b[B"
#define RIGHTARROW   "\x1b[C"
#define UPARROW      "\x1b[A"
#define LEFTARROW    "\x1b[D"
#define HOMEPOS      "\x1b[H"

#define BOLD       "\x1b[1m"
#define FLASH      "\x1b[5m"
#define INVERSE    "\x1b[7m"
#define NTEXT      "\x1b[0m"

#define TAB        "\x9"

/* ----------------------------------------- */
/* The color for items laying on the ground. */
/* ----------------------------------------- */
#define ITEMCOLOR  "\x1b[0;36m"

#define ACT_CRASHER         0x00000001

/* --------------------------------------------------------------------------- */
/* Tried to keep things similar to Diku/Merc, for area conversion, but I don't
   think it's very possible.. sigh */
/* --------------------------------------------------------------------------- */
#define ACT_STAY_AREA        0x00000001		/* Mob stays in it's area vnum */
#define ACT_SENTINEL         0x00000002		/* Mob doesn't move */
#define ACT_SCAVENGER        0x00000004		/* Picks things up off ground */
#define ACT_AGGRESSIVE       0x00000008		/* Attacks players on sight */

#define ACT_WIMPY            0x00000010		/* Mob tries to flee when
						   low on hit points */
#define ACT_MOBSWITCH        0x00000020  	/* Switches opponents in combat */
#define ACT_PET              0x00000040		/* Is this a 'pet' mob? */
#define ACT_UNTOUCHABLE	     0x00000080		/* Only harmable by magic */
#define ACT_PRACTICE         0x00000100		/* This mob can teach things */
#define ACT_UNVAL            0x00000200		/* Unvalidated player! */
#define ACT_FASTHUNT         0x00000400  	/* Hunts victims FAST */
#define ACT_SCRAMBLE         0x00001000		/* Used for rushing/running */
#define ACT_MOUNTABLE        0x00002000		/* Can the mob be mounted? */
#define ACT_ANGRY            0x00008000		/* Similar to aggressive, but
						   doesn't attack so fast. */
#define ACT_STAY_SECTOR	     0x00004000		/* Stays in the sector type
						   it repop'd in */
#define ACT_BANK_TELLER      0x00040000		/* Is this a bank teller or
						   clan banker? */
#define ACT_SOLOYELL         0x00010000	/* Will yell solo if attacked */
#define ACT_STAYOFFROAD      0x00020000	/* Won't go onto city/road sectors */
/* ---------------------------------------------------------------------- */
/* On all flags from here on, there was no attempt to keep other codebase
   value flags */
/* ---------------------------------------------------------------------- */

#define REPAIRS_FREE         0x00080000		/* Repairs items for free */
#define REPAIRS_ONE          0x00100000		/* repairs 1/8 cost */
#define REPAIRS_TWO          0x00200000		/* ditto except 1/4 cost */

#define ACT_ATTITUDE         0x04000000		/* attacks if looked at */
#define ACT_AVENGED          0x08000000         /* Avengers come if killed */
#define ACT_REBOOT_ONLY      0x10000000         /* Dont repop after killed */





#define ACT3_CLAN_GUARD       0x00000001        /* Clan Guard */
#define ACT3_GUARD_DOOR       0x00000002	/* Set to guard a door */
#define ACT3_SECT_GUARD       0x00000004        /* Sect Guard */
#define ACT3_GUARD_DOOR_ALIGN 0x00000008        /* Guards door from 'good'*/
#define ACT3_GUARD_MOB        0x00000010	/* Set to guard anothermob */
#define ACT3_WARRIOR          0x00000020	/* Warrior's guildmaster */
#define ACT3_THIEFG           0x00000040        /* Battlemasterguildmaster*/
#define ACT3_HEALER           0x00000080	/* Healer's guildmaster */
#define ACT3_WIZARD           0x00000100	/* Wizard's guildmaster */
#define ACT3_RANGER           0x00000200	/* Ranger's guildmaster */
#define ACT3_ROGUE            0x00000400	/* Rogue's guildmaster */
#define ACT3_TINKER           0x00000800	/* Tinker's guildmaster */
#define ACT3_CONJURER         0x00008000        /* Conjurer's guildmaster*/
#define ACT3_MUTE             0x00010000	/* Can't yell */
#define ACT3_RIDE             0x00020000	/* Rides prev mob reset */
#define ACT3_FOLLOW           0x00040000	/* Will follow " " " " */
#define ACT3_REGENERATE       0x00080000	/* Regenerates fast rate */
#define ACT3_MYSTIC           0x00100000        /* Mystic GM */
#define ACT3_BATTLEMASTER     0x00200000        /* Battlemaster GM */
#define ACT3_NECROMANCER      0x00800000        /* Necromancers GM */
#define ACT3_MONK             0x01000000        /* Monks GM */

#define ACT3_SLEEP           0x00400000		/* Sleeps sometimes*/
#define ACT3_DISABLED        0x01000000         /* Mapping disabled */
#define ACT3_STAYROAD        0x02000000  	/* Stays on road */
#define ACT3_GUARD_ITEM      0x04000000		/* Set to guard an item */

#define ACT4_ENTERSCRIPT      0x00000001   /* Mob has an assoc enter script */
#define ACT4_LEAVESCRIPT      0x00000002   /* Mob has an assoc leave script */
#define ACT4_COMMANDSCRIPT    0x00000004  /* Mob has command script on it. */
#define ACT4_QUICKTICK        0x00000008 /* Mob has a script every quicktick */
#define ACT4_MOVESCRIPT       0x00000010  /* Mob has a script when it moves. */
/* These are the flags to check if the mob attacks anyone */


#define ACT4_KILL_OPP         0x00004000		/* spams  */
#define ACT4_KILL_SAME        0x00008000		/* spams KILL EVIL */
#define ACT4_KILL_CLAN        0x00010000        /* Kills members asshole clans */
#define ACT4_PROTECTOR        0x00020000        /* Casts spells at "enemy" */
#define ACT4_KILLALLONE       0x00040000	/* Kills everything flag one */
#define ACT4_KILLALLTWO       0x00080000	/* Kills everything flag two */
#define ACT4_KILLMOPP         0x00100000	/* Kills mobs of diff align */
#define ACT4_KILLMGOOD        0x00200000	/* Kills good mobs */
#define ACT4_SUMMONER         0x00400000    /* Summons help while in combat */
#define ACT4_KILLGLADIATOR    0x00800000		/* kills 'gladiator' */
#define ACT4_SLEEPAGGRO       0x01000000		/* aggro towards sleeping players */
#define ACT4_ASSISTONE        0x02000000		/* assists mobs with name AONE */
#define ACT4_ASSISTTWO        0x04000000		/* assists mobs with name ATWO */
#define ACT4_REST             0x08000000		/* mob rests when no PC's in room */
#define ACT4_RANGER           0x10000000		/* attacks "beast" */
#define ACT4_ASSISTALL        0x20000000		/* Will assist all fighting mobs in room */
#define ACT4_RESCUEONE        0x40000000		/* rescue mobs with name PRTCT */
#define ACT4_RESCUETWO        0x80000000		/* rescue mobs with name CODEONE */

/* ----------------------------------------------------------------------------- */
/* For all AFF_ flags, I tried to follow as closely as I could to Theisles/Merc
   standards!  Sorry if some are off... some are unused. */
/* ----------------------------------------------------------------------------- */

#define AFF_BLIND            0x00000001 	/* Blind */
#define AFF_INVISIBLE        0x00000002		/* Invisible */
#define AFF_DETECT_EVIL      0x00000004		/* People can tell evil objs/mobs */
#define AFF_DETECT_INVIS     0x00000008		/* See invis */
#define AFF_DETECT_MAGIC     0x00000010		/* People can tell magic items */
#define AFF_DETECT_HIDDEN    0x00000020		/* See hidden */
#define AFF_WOUND            0x00000040         /* Loses hps fast */
#define AFF_SANCTUARY        0x00000080		/* Absorbs 0 to 1/2 of damage */
#define AFF_FAERIE_FIRE      0x00000100		/* Lowers armor */
#define AFF_INFRARED         0x00000200		/* See in the dark */
#define AFF_CURSE            0x00000400		/* Cannot recall... */
#define AFF_CONFUSE          0x00000800         /* Player is confused */
#define AFF_POISON           0x00001000		/* Poisoned */


/* ------------------------------------------------------ */
/* From here down, I tried to keep the NiMud/Theisles std */
/* ------------------------------------------------------ */
#define AFF_PROTECT          0x00002000	/* Absorbs 1/2 damage done */
#define AFF_LETHARGY         0x00004000 /* Makes player lose moves */
#define AFF_SNEAK            0x00008000	/* Sneaking.. */
#define AFF_HIDE             0x00010000	/* Hidden.. */
#define AFF_SLEEP            0x00020000	/* Forced sleep */
#define AFF_CHARM            0x00040000	/* Charmed */
#define AFF_FLYING           0x00080000	/* Flying */
#define AFF_PASS_DOOR        0x00100000	/* Pass through unlocked doors. */
#define AFF_REGENERATE       0x00200000 /* Regenerates hps quickly */

/* ------------------------------------------------- */
/* These flags are non-compat with any other mudbase */
/* ------------------------------------------------- */
#define AFF_PLAGUE           0x00400000	/* Plagued.. plague spreads. */
#define AFF_BREATH_WATER     0x00800000	/* Breathe underwater */
#define AFF_PROT_ALIGN       0x01000000	/* Protection from other aligns */

#define AFF_MINDSAP          0x04000000 /* Player loses mana fast */
#define AFF_AWARENESS        0x08000000	/* See people enter while asleep */
#define AFF_NOXIOUS          0x10000000	/* Noxious fumes.  Unused. */
#define AFF_DETECT_GOOD      0x20000000	/* Detect good items/mobs */
#define AFF_NOCHARM          0x40000000	/* Not charmable */
#define AFF_SAFETY           0x80000000	/* Exempt from being attacked by
					   aggro mobs of much lower level */

#define AFF_PROT_FIRE           0x00000001	/* Protection from fire */
#define AFF_PROT_GAS            0x00000002	/* Protection from gas */
#define AFF_MINDSHIELD          0x00000004	/* Spell protection */
#define AFF_SPEED               0x00000008	/* Double attacks */
#define AFF_PARALYZE            0x00000010	/* Paralysis */
#define AFF_PROT_SHOCK          0x00000020      /* Prot from shock */
#define AFF_PROT_WIND           0x00000040      /* Prot from wind */
#define AFF_PROT_WOUND          0x00000080      /* Prot from wounds */
#define AFF_PROT_PARALYZE       0x00000100      /* Prot from Paralyze */
#define AFF_PROT_LOCATE         0x00000200      /* Prot from locate */
#define AFF_PROT_ICE            0x00000400      /* Prot from ice */
#define AFF_PROT_ACID           0x00000800      /* Prot from acid */
#define AFF_PROT_ASTRAL         0x00001000      /* Prot from Astral */
#define AFF_PROT_LAVA           0x00002000      /* Prot from laval */
#define AFF_PROT_SHARD          0x00004000      /* Prot from shards */
#define AFF_SPELL_REFLECT       0x00008000      /* Reflects spells back */
#define AFF_NOTRACKS            0x00010000      /* They don't leave tracks */
#define AFF_NOBLOOD             0x00020000      /* They don't leave blood */
#define AFF_CHAMELEON           0x00040000      /* Mobs cannot see person
                                                   wears off if anything
                                                   but movement done */
#define AFF_DET_CHAMELEON       0x00080000      /* Lets you see cammed folx */
#define AFF_DET_FOG             0x00100000      /* Lets you see thru fog. */
#define AFF_PROT_POISON         0x00200000      /* No Poison effects */
#define AFF_PROT_CURSE          0x00400000      /* Cannot be cursed */
#define AFF_PROT_BLIND          0x00800000      /* Cannot be blinded */
#define AFF_PROT_PLAGUE         0x01000000      /* Cannot be Plagued */
#define AFF_FIRESHIELD          0x02000000      /* Caster is protected by fireshield */
#define AFF_MANASHIELD	        0x04000000	/* Mana Shield */

/* -------------------------------------------- */
/* Sexflags - specifically follow Diku standard */
/* -------------------------------------------- */
#define SEX_NEUTRAL                   0
#define SEX_MALE                      1
#define SEX_FEMALE                    2


/* --------------------------------------------------------- */
/* These loosely follow theisles std and mortal conquest std */
/* --------------------------------------------------------- */
#define OBJ_VNUM_CORPSE_NPC           10	/* vnum of generic mob corpse */
#define OBJ_VNUM_CORPSE_PC            11	/* vnum of generic player corpse */
#define OBJ_VNUM_SEVERED_HEAD         12	/* vnum of severed head */
#define OBJ_VNUM_TORN_HEART           13	/* torn heart */
#define OBJ_VNUM_SLICED_ARM           14	/* sliced arm */
#define OBJ_VNUM_SLICED_LEG           15	/* sliced leg */
#define OBJ_VNUM_INTESTINES           16	/* intestines */
#define OBJ_VNUM_BLOOD                40	/* Blood trails */
#define ROOM_VNUM_ARENA             504		/* Arena room number */
#define VNUM_WAITONE                508		/* Waiting room 1 for arena */
#define VNUM_WAITTWO                509		/* Waiting room 2 for arena */

#define ARENA_WAIT_TICK               2		/* Ticks in waiting room
						   before the arena battle
						   begins */


#define SEARCH_INV_FIRST	1	/* Search inventory first, for use
					   with find_item_here. */
#define SEARCH_ROOM_FIRST	2	/* Search room first. */


/* ---------------------------------------------------------------------- */
/* Full compatibility with Theisles, but possible additions.  I can't
   guarentee all flags do exactly the same things.  Ranged weapons are
   completely disabled, as I lifted the ranged weapon code from an old
   version of mortal conquest, and feel bad about using it.. I'll code my
   own in due time. */
/* ---------------------------------------------------------------------- */
#define ITEM_LIGHT                  1
#define ITEM_SCROLL                 2
#define ITEM_WAND                   3
#define ITEM_STAFF                  4
#define ITEM_WEAPON                 5
#define ITEM_AMMO                   7	/* unused */
#define ITEM_TREASURE               8
#define ITEM_ARMOR                  9
#define ITEM_POTION                10
#define ITEM_BOOK                  11
#define ITEM_FURNITURE             12
#define ITEM_TRASH                 13
#define ITEM_CONTAINER             15
#define ITEM_THROWN                16	/* unused */
#define ITEM_DRINK_CON             17
#define ITEM_KEY                   18
#define ITEM_FOOD                  19
#define ITEM_GEM                   21
#define ITEM_VEHICLE               22
#define ITEM_CORPSE_NPC            23
#define ITEM_CORPSE_PC             24
#define ITEM_FOUNTAIN              25
#define ITEM_PILL                  26
#define ITEM_TOOL                  27
#define ITEM_LIST                  28
#define ITEM_HERB                  29
#define ITEM_CLIMBABLE             30
#define ITEM_SCALABLE              31
#define ITEM_SCALE_DEV             32
#define ITEM_MAP                   33
#define ITEM_TICKET                34
#define ITEM_BOAT                  35
#define ITEM_REPELLANT             36
#define ITEM_TRAP                  37
#define ITEM_MINERAL               38
#define ITEM_HIDE                  39
#define ITEM_RAW                   40
#define ITEM_CLOTH                 41
#define ITEM_WOOD                  42
#define ITEM_MAX                   45


/* Pseudo oop - identifies a structure */
#define K_OBJECT         1	/* structure is an object */
#define K_OBJ            1	/* structure is an object */
#define K_CHAR           2	/* structure is a character */
#define K_ROOM           3	/* structure is a room */
#define K_PC             4	/* structure is a player structure */



/* ------------------------------------- */
/* These values follow Theisles standard */
/* ------------------------------------- */
#define CONT_CLOSEABLE                0x01	/* Can container be opened/closed? */
#define CONT_PICKPROOF                0x02	/* Is the container pickproof? */
#define CONT_CLOSED                   0x04	/* Does the container default to closed? */
#define CONT_LOCKED                   0x08	/* Is the container locked? */
#define CONT_INDESTRUCTABLE           0x10      /* This item cannot be
                                                   destroyed by fire */
#define CONT_HASTRAP                  0x20     /* This item has a trap */
#define CONT_MORTAR                   0x40      /* This item is a mortar for brewing */
#define LIGHT_LIT                     0x01	/* Is the light lit? */
#define LIGHT_FILLABLE                0x02	/* Is the light refillable? */
#define TOOL_TINDERBOX                0x01	/* Is this a tinderbox? */
#define TOOL_LOCKPICK                 0x02	/* Is this a lockpick? */
#define TOOL_FLASH_POWDER             0x04      /* Is this flash powder? */
#define TOOL_FORGING                  0x08      /* This is a forging hammer */
#define TOOL_PICKAXE                  0x10      /* This is pickaxe for mining*/
#define TOOL_NEEDLE                   0x20      /* This is a needle */
#define TOOL_PESTLE                   0x40      /* This is a pestle for brewing */

/* ----------------------------------------------------------------------- */
/* For portals.  value 4 to retain compatibility with mortal conquest code */
/* ----------------------------------------------------------------------- */
#define FURN_EXIT                     4
#define FURN_FORGE                    8 /* This is a forge. */
#define FURN_ANVIL                   16 /* This is an anvil */
#define FURN_LOOM                    32    /* This is a loom for weaving */
/* ------------------------------------------------- */
/* I tried to keep compatibility with merc/theisles. */
/* ------------------------------------------------- */
#define ITEM_GLOW            0x0000000000000001	/* Can hit ghosts if on a weapon */
#define ITEM_HUM             0x0000000000000002	/* Can hit ghosts if on a weapon */
#define ITEM_COMMANDSCRIPT   0x0000000000000004 /* Item has command script on it */
#define ITEM_DROPSCRIPT      0x0000000000000008 /* Item has a script when dropped */
#define ITEM_EVIL            0x0000000000000010	/* Registers with detect evil */
#define ITEM_INVIS           0x0000000000000020	/* Item is invisible */
#define ITEM_MAGIC           0x0000000000000040	/* Item registers with detect magic */
#define ITEM_NODROP          0x0000000000000080	/* Can't drop this item */
#define ITEM_GIVESCRIPT      0x0000000000000100 /* Item has a script when given */
#define ITEM_RANGED          0x0000000000000200 /* Item is a ranged wpn */
#define ITEM_NOREMOVE        0x0000000000001000	/* Can't unequip this item */
#define ITEM_INVENTORY       0x0000000000002000	/* For shop use, internal */
#define ITEM_NOSAVE          0x0000000000004000	/* Item doesn't save when you quit */
#define ITEM_POWER           0x0000000000008000	/* Item of power.. Eats up sanct */
#define ITEM_UNSEEN          0x0000000000010000 /* Hidden. - cannot search */
#define ITEM_PIERCE          0x0000000000020000	/* Gives a bonus for piercing skill */
#define ITEM_SLASHING        0x0000000000040000	/* slashing */
#define ITEM_CONCUSSION      0x0000000000080000	/* pounding/concussion */
#define ITEM_WHIP            0x0000000000100000	/* whipping */
#define ITEM_HEAVY_SLASH     0x0000000000200000	/* VERY HEAVY - hitroll */
#define ITEM_LIGHT_SLASH     0x0000000000400000	/* Gives a large slashing bonus */
#define ITEM_GOOD            0x0000000000800000 /* Registers with detect good */
#define ITEM_SAVE            0x0000000001000000	/* Item stays on death. */
#define ITEM_NOSTORE         0x0000000002000000	/* Cannot store this item  */
#define ITEM_POLEARM         0x0000000004000000	/* Polearm */
#define ITEM_ELEMENTAL       0x0000000008000000 /* Item causes elemental effects */
#define ITEM_WOUND           0x0000000010000000 /* Wounding item */
#define ITEM_BLIND           0x0000000020000000 /* Blinding item */
#define ITEM_POISON          0x0000000040000000 /* Poison item */
#define ITEM_GUILD	     0x0000000080000000 /* Guild Item */
#define ITEM_FLAME           0x0000000100000000 /* Item is flaming */
#define ITEM_HOLY            0x0000000200000000 /* Item is Holy */
#define ITEM_UNHOLY          0x0000000400000000 /* Item is Unholy */
/* Proc Effects.  Small chance of going off in combat */
#define ITEM_KNOCKDOWN	     0x0000001000000000 /* knockdown */
#define ITEM_VAMPIRIC	     0x0000002000000000 /* drain life */
#define ITEM_SILENCE	     0x0000004000000000 /* silencing */
#define ITEM_MANARESTORE     0x0000008000000000 /* restore a % of dme as mana */
#define ITEM_CRUSADER	     0x0000010000000000 /* greatly increases strength */
#define ITEM_MANABURN	     0x0000020000000000 /* also burns off mana */
#define ITEM_GUARDIAN	     0x0000040000000000 /* increases defense */
#define ITEM_REFLECT	     0x0000080000000000 /* bounce a spell back */
#define ITEM_SPELLSHIELD     0x0000100000000000 /* gives spell shield effect */
#define ITEM_WEAKEN	     0x0000200000000000 /* weaken effect */
#define ITEM_OVERPOWER	     0x0000400000000000 /* double damage */

/* Place Holders - Rename define and add to end of extra_flagss
 * structure in def.c before the end_of_list marker */
/*      	
#define ITEM_FREE	     0x0000800000000000 
#define ITEM_FREE	     0x0001000000000000 
#define ITEM_FREE	     0x0002000000000000 
#define ITEM_FREE	     0x0004000000000000 
#define ITEM_FREE	     0x0008000000000000 
#define ITEM_FREE	     0x0010000000000000 
#define ITEM_FREE	     0x0020000000000000 
#define ITEM_FREE	     0x0040000000000000 
#define ITEM_FREE	     0x0080000000000000 
#define ITEM_FREE	     0x0100000000000000 
#define ITEM_FREE	     0x0200000000000000 
#define ITEM_FREE	     0x0400000000000000 
#define ITEM_FREE	     0x0800000000000000 
#define ITEM_FREE	     0x1000000000000000 
#define ITEM_FREE	     0x2000000000000000 
#define ITEM_FREE	     0x4000000000000000 
#define ITEM_FREE	     0x8000000000000000 
*/


/* ------------------------------------------------- */
/*   		Item Guild Flags		     */
/* ------------------------------------------------- */
#define ITEM_ANTI_TINKER       0x00000001	/* Item cannot be used by Tinkers */
#define ITEM_ANTI_WARRIOR      0x00000002	/* Item cannot be used by Warriors */
#define ITEM_ANTI_THIEF        0x00000004	/* Item cannot be used by Thieves */
#define ITEM_ANTI_HEALER       0x00000008	/* Item cannot be used by Healers */
#define ITEM_ANTI_WIZARD       0x00000010	/* Item cannot be used by Wizards */
#define ITEM_ANTI_RANGER       0x00000020	/* Item cannot be used by Rangers */
#define ITEM_ANTI_ROGUE        0x00000040	/* Item cannot be used by Rogue */
#define ITEM_ANTI_CONJURER     0x00000080	/* Item cannot be used by Conjurers */
#define ITEM_ANTI_MYSTIC       0x00000100	/* Item cannot be used by Mystics */
#define ITEM_ANTI_BATTLEMASTER 0x00000200	/* Item cannot be used by BMasters */
#define ITEM_ANTI_NECROMANCER  0x00000400	/* Item cannot be used by Necromancers */
#define ITEM_ANTI_MONK         0x00000800	/* Item cannot be used by Monks */

/* --------------------------------------------------------------------------- */
/* Fully Theisles compatible.  I duplicated the follow flags with the idea
   in mind of someone eventually making a theisles --> thismud object converter */
/* --------------------------------------------------------------------------- */
#define ITEM_TAKE            0x00000001
#define ITEM_WEAR_FINGER     0x00000002
#define ITEM_WEAR_NECK       0x00000004
#define ITEM_WEAR_BODY       0x00000008
#define ITEM_WEAR_HEAD       0x00000010
#define ITEM_WEAR_LEGS       0x00000020
#define ITEM_WEAR_FEET       0x00000040
#define ITEM_WEAR_HANDS      0x00000080
#define ITEM_WEAR_ARMS       0x00000100
#define ITEM_WEAR_SHIELD     0x00000200
#define ITEM_WEAR_ABOUT      0x00000400
#define ITEM_WEAR_WAIST      0x00000800
#define ITEM_WEAR_WRIST      0x00001000
#define ITEM_WEAR_BELT       0x00002000
#define ITEM_WIELD           0x00004000
#define ITEM_HOLD            0x00004000

/* -------------------------------- */
/* Mortal conquest compatible flags */
/* -------------------------------- */
#define ITEM_WEAR_SHOULDER   0x00008000
#define ITEM_WEAR_FACE       0x00010000
#define ITEM_FLOATING        0x00020000
#define ITEM_WEAR_EAR1       0x00040000
#define ITEM_WEAR_ANKLE      0x00080000
#define ITEM_WEAR_ELBOW      0x00100000
#define ITEM_WEAR_KNEE       0x00200000
#define ITEM_WEAR_BACK       0x00400000
#define ITEM_WEAR_EYES       0x00800000
#define ITEM_WEAR_TATTOO     0x01000000

/* --------------------------------- */
/*     Elemental Alignment Flags     */
/* --------------------------------- */
#define ELEMENTAL_ALIGN_FIRE        0x00000001	
#define ELEMENTAL_ALIGN_WATER       0x00000002	
#define ELEMENTAL_ALIGN_AIR         0x00000004	
#define ELEMENTAL_ALIGN_EARTH       0x00000008	
#define ELEMENTAL_ALIGN_GOOD        0x00000010
#define ELEMENTAL_ALIGN_EVIL        0x00000020	


/* -------------------------------------------------- */
/* Theisles/merc compatible, although many are unused */
/* -------------------------------------------------- */
#define APPLY_NONE                    0
#define APPLY_STR                     1
#define APPLY_DEX                     2
#define APPLY_INT                     3
#define APPLY_WIS                     4
#define APPLY_CON                     5
#define APPLY_HIT                    13
#define APPLY_MOVE                   14
#define APPLY_MANA                   15
#define APPLY_AC                     17	/* Negative is good */
#define APPLY_HITROLL                18	/* Positive is good */
#define APPLY_DAMROLL                19	/* Positive is good */
#define APPLY_SAVING_THROW           20	/* Negative is good */

/* ------------- */
/* Code specific */
/* ------------- */
#define APPLY_RESIST_SUMMON          21
#define APPLY_KICK_DAMAGE            22
#define APPLY_SNEAK                  23
#define APPLY_HIDE                   24
#define APPLY_WARMTH                 25


/* ------------------------------------------------------------------- */
/* Once again, the room flags keep semi-compatibility, although manny are
   unused, and some are very different */
/* ------------------------------------------------------------------- */
#define ROOM_DARK            0x00000001	/* Room is dark */
#define ROOM_NO_MOB          0x00000002	/* No mobs will wander in. */
#define ROOM_INDOORS         0x00000004	/* Room is indoors */
#define ROOM_LOWMAGIC        0x00000008 /* Room suppresses magic */
#define ROOM_HIGHMAGIC       0x00000010 /* Extra magic power here...*/
#define ROOM_UNDERGROUND     0x00000020 /* This room is underground */
#define ROOM_PET_SHOP        0x00000040	/* Is a pet shop */
#define ROOM_NOHEAR          0x00000080 /* cannot hear from here */
#define ROOM_NOISY           0x00000100 /* Noisy; yells don't go as far */
#define R_MARK               0x00000200	/* Mark a room */
#define ROOM_NOSUMMON        0x00000400	/* Can't summon TO/FROM this room */
#define ROOM_UNTRACKABLE     0x00000800	/* Can't track through this room */
#define ROOM_NOMAGIC         0x00001000	/* No spells allowed here */
#define ROOM_SILENCE         0x00002000	/* Nobody can talk from here */
#define ROOM_NO_RECALL       0x00004000	/* Can't recall from here */
#define ROOM_CLANSTORE       0x00008000	/* This is a clan storage room */
#define ROOM_CANTELEPORT     0x00010000 /* Can teleport to this room */

#define ROOM_COMMANDSCRIPT  0x08000000 /* Room has command script. */
#define ROOM_ENTERSCRIPT    0x10000000 /* Room has an enter script. */
#define ROOM_LEAVESCRIPT    0x20000000	/* Room has a leave script. */
#define ROOM_DROPSCRIPT     0x40000000	/* Room has a dropped item script */
#define ROOM_GETSCRIPT      0x80000000 /* Room has a get item script */





// These are for the transport update routine...to split them off from
// The rest of the types of things which are not checked each second or 2.



#define ROOM2_FALLSFAST       0x00000001	/* Person falls down if they are in*/

#define ROOM2_EXTRAHEAL       0x00000002	/* Slight healing bonus in this room2 */
#define ROOM2_EXTRAMANA       0x00000004 /* Slight mana bonus in this room2.. buggy */
#define ROOM2_MOVING          0x00000008	/* Room2 has a 'current' */
#define ROOM2_MANADRAIN       0x00000010	/* Drains all mana */
#define ROOM2_FIRETRAP        0x00000020	/* Fire burns characters */
#define ROOM2_POISONTRAP      0x00000040	/* Poison arrows shoot characters */
#define ROOM2_ARROWTRAP       0x00000080	/* Arrows shoot characters */
#define ROOM2_ROCKTRAP        0x00000100	/* Rocks fall on characters */
#define ROOM2_SPIKETRAP       0x00000200	/* Spikes stab characters */
#define ROOM2_GASTRAPONE      0x00000400	/* Unused, not working */
#define ROOM2_GASTRAPTWO      0x00000800	/* Noxious gas poisons characters */

#define ROOM2_GEYSERTRAP       0x00001000	/* Gyser scalds */

#
#define ROOM2_AVALANCHE       0x00002000	/* Avalanche... goes off once (buggy) */
#define ROOM2_ACIDSPRAY       0x00004000	/* Sprays player with acid */





/* ------------------------------------------------ */
/* Direction compatibility with all diku-based muds */
/* ------------------------------------------------ */
#define DIR_NORTH                     0
#define DIR_EAST                      1
#define DIR_SOUTH                     2
#define DIR_WEST                      3
#define DIR_UP                        4
#define DIR_DOWN                      5
#define MAX_DIR                       6


/* ------------------------------------- */
/* Mortal Conquest compatible exit flags */
/* ------------------------------------- */
#define EX_ISDOOR            0x00000001	/* This IS a door */
#define EX_CLOSED            0x00000002	/* Door is closed */
#define EX_LOCKED            0x00000004 /* Door is locked */
#define EX_PICKPROOF         0x00000010	/* Door cannot be picked */
#define EX_BASHPROOF         0x00000020	/* Door cannot be broken */
#define EX_HIDDEN            0x00000100	/* Hidden door */
#define EX_RESET             0x00000200	/* Resets on repop if broken */
#define EX_HASTRAP           0x00000400 /* This exit has a trap */
/* ---------------------------------------- */
/* Small compatibility with Mortal Conquest */
/* ---------------------------------------- */
#define SECT_INSIDE                   0	/* Inside, set INDOORS flag too! */
#define SECT_CITY                     1	/* City, easy moving */
#define SECT_TOWN                     2 /* Town area */
#define SECT_ROAD                     3 /* Road sector */
#define SECT_TRAIL                    4 /* Trail Sector */
#define SECT_PATH                     5 /* A trail through wilderness */
#define SECT_BRIDGE		      6 /* Bridge Sector */
#define SECT_FIELD                    7	/* Field, easy moving */
#define SECT_GRASSLANDS               8 /* Grasslands - open rich */
#define SECT_SAVANNAH                 9 /* Warm, lush grasslands */


/* End of easy movement sectors begin special sectors which need stuff.*/

#define SECT_WATER_SWIM              10	/* Swimable water */
#define SECT_WATER_NOSWIM            11	/* Unswimable water */
#define SECT_UNDERWATER              12	/* Underwater */
#define SECT_MOUNTAIN                13	/* Need mountain boots-vehicle	 */
#define SECT_AIR                     14	/* Air sector */
#define SECT_CLOUDS                  15 /* Up in the clouds */
#define SECT_SNOW                    16	/* Hard moving, cold */


/* End special sectors that need stuff, now onto the rest. */

#define SECT_ICE		     17 /* Ice sector.. slippery footing */
#define SECT_ARCTIC                  18 /* Cold like sect_snow... */
#define SECT_TUNDRA                  19 /* Cold and barren wastes */
#define SECT_TROPICAL                20	/* Tropical, hot */
#define SECT_MARSH		     21 /* Marsh sector.. no blood trails */
#define SECT_SWAMP                   22 /* Swamp area..yeah im scraping */
#define SECT_HOLLOW                  23 /* Cool and dark marsh... */
#define SECT_STEPPE                  24 /* Cool, dry grasslands */
#define SECT_PLAINS                  25 /* Plains sector (big field) */
#define SECT_BRUSH                   26 /* Brush sector... */
#define SECT_FOREST                  27	/* Forest, tough going */
#define SECT_WOODS                   28 /* Another forest type */
#define SECT_CANYON                  29 /* Canyon--dark alot */
#define SECT_HILLS                   30	/* Hills, tough going */
#define SECT_ROCKY                   31 /* Rocky, hard to keep footing */
#define SECT_WASTELAND               32 /* Almost as bad as desert, dry */
#define SECT_DESERT                  33	/* Desert, -2 thirst, hot */
#define SECT_RUINS                   34 /* Ancient, abandoned place */
#define SECT_LAVA                    35 /* Lava Area */
#define SECT_ASTRAL                  36 /* Astral sector...need spell */
/* These are the underground sectors! */



#define SECT_CAVE                    37 /* Underground cave */
#define SECT_HOLE                    38 /* A Place over a hole */
#define SECT_CAVERN                  39 /* BIG cave underground */
#define SECT_PASSAGE                 40 /* A type of tunnel */
#define SECT_ALCOVE                  41 /* A Small chamber */
#define SECT_CHAMBER                 42 /* An underground opening */
#define SECT_VAULT                   43 /* MASSIVE cave..can fly */
#define SECT_SUBTERRANIAN            44 /* under the earth area */
#define SECT_SHAFT                   45 /* A shaft...dunno */
#define SECT_PIT                     46 /* Above or in a pit */
#define SECT_TUNNEL                  47 /* Underground path */
#define SECT_TUBE                    48 /* Round formed tunnel */
#define SECT_CAPTURE		     49 /* Caputurable Sector */
#define SECT_MAX                     50


/* ------------------------------------------------------------------------- */
/* Wear locations: NO compatibility with any other code base, but it is so   */
/* easy to change these things.... there is no reason to requyire that */
/* ------------------------------------------------------------------------- */
#define WEAR_NONE                    -1
#define WEAR_FLOATING_1               0
#define WEAR_FLOATING_2               1
#define WEAR_HEAD                     2
#define WEAR_EYES                     3
#define WEAR_EAR_1                    4
#define WEAR_EAR_2                    5
#define WEAR_FACE                     6
#define WEAR_NECK_1                   7
#define WEAR_NECK_2                   8
#define WEAR_BODY                     9
#define WEAR_ABOUT                   10
#define WEAR_BACK                    11
#define WEAR_SHOULDER_1              12
#define WEAR_SHOULDER_2              13
#define WEAR_SHIELD                  14
#define WEAR_ARMS                    15
#define WEAR_TATTOO                  16
#define WEAR_ELBOW_1                 17
#define WEAR_ELBOW_2                 18
#define WEAR_WRIST_1                 19
#define WEAR_WRIST_2                 20
#define WEAR_HANDS                   21
#define WEAR_FINGER_1                22
#define WEAR_FINGER_2                23
#define WEAR_HOLD_1                  24
#define WEAR_HOLD_2                  25
#define WEAR_BELT_1                  26
#define WEAR_BELT_2                  27
#define WEAR_BELT_3                  28
#define WEAR_BELT_4                  29
#define WEAR_BELT_5                  30
#define WEAR_WAIST                   31
#define WEAR_LEGS                    32
#define WEAR_KNEE_1                  33
#define WEAR_KNEE_2                  34
#define WEAR_ANKLE_1                 35
#define WEAR_ANKLE_2                 36
#define WEAR_FEET                    37
#define MAX_WEAR                     38



/* ----------------- */
/* Cool extra skills */
/* ----------------- */

/* ----------------- */
/* Cool skill checks */
/* ----------------- */


/* -------------------------------------- */
/* Conditions compat with mortal conquest */
/* -------------------------------------- */
#define COND_DRUNK                    0
#define COND_FULL                     1
#define COND_THIRST                   2
#define COND_STARVING                 3

/* ---------------------- */
/* Internal use for ident */
/* ---------------------- */
#define WRITE_AUTH 1
#define AUTH       2

/* -------------------- */
/* Guild flags and info */
/* -------------------- */
/* These values MUST match those set 	*/
/* in by ACT3_ 				*/

#define GUILD_TINKER        0x00000800
#define GUILD_WARRIOR       0x00000020
#define GUILD_THIEFG        0x00000040
#define GUILD_HEALER        0x00000080
#define GUILD_WIZARD        0x00000100
#define GUILD_RANGER        0x00000200
#define GUILD_ROGUE         0x00000400
#define GUILD_CONJURER      0x00008000
#define GUILD_MYSTIC        0x00100000
#define GUILD_BATTLEMASTER  0x00200000
#define GUILD_NECROMANCER   0x00800000
#define GUILD_MONK          0x01000000




/* ------------------------------------------------------------------------ */
/* Positions... the first few positions, I tried to maintain compatibility
   with mortal conquest/theisles. After that, anything goes. */
/* ------------------------------------------------------------------------ */
#define POSITION_DEAD                      0
#define POSITION_MORTAL                    1	/* Mortally wounded */
#define POSITION_INCAP                     2	/* Incapacitated */
#define POSITION_STUNNED                   3	/* Stunned */
#define POSITION_SLEEPING                  4	/* Sleeping */
#define POSITION_RESTING                   5	/* Resting */
#define POSITION_BASHED                    6	/* Bashed to ground */
#define POSITION_GROUNDFIGHTING            7	/* Groundfighting */
#define POSITION_FIGHTING                  8	/* Fighting */
#define POSITION_CIRCLE                    9	/* Circling */
#define POSITION_FALLING                  10	/* Falling! */
#define POSITION_MEDITATING               11
#define POSITION_STANDING                 12	/* Standing, not fighting */
#define POSITION_RUNNING                  13	/* Running */
#define POSITION_SEARCHING                14	/* Searching for exit/item */
#define POSITION_BACKSTAB                 15	/* Backstab preparation */
#define POSITION_INVEST                   16	/* Investigating */
#define POSITION_CASTING                  17	/* Casting a spell */
#define POSITION_SHOOTING                 18    /* Shooting a wpn */
#define POSITION_MINING                   19    /* Mining for chunks */
#define POSITION_CHARGING                 20    /* Mining for chunks */
#define MAX_POSITIONS                     21    /* MAX num positions. */
/* ---------------- */
/* Player set bits. */
/* ---------------- */
#define PLR_HOLYLIGHT        0x00000004	/* Can see in the dark, invis, etc */
#define PLR_HARDCORE         0x00000008	/* 1 pdeath and you're gone */
#define PLR_MAPPING          0x80000000 /* is the player mapping */
#define PLR_NOIBM            0x40000000 /* Plr does not use ibm for map */
#define PLR_VIEWMOBS         0x20000000 /* Plr views mobs on map */
#define PLR_VIEWPLAYERS      0x10000000 /* Plr views players on map. */
#define PLR_MAPSPAM          0x08000000 /* Plr gets spammy updates */
#define PLR_LOG              0x00000010	/* Keep logs of this player's commands */
#define PLR_DENY             0x00000020	/* Deny this character from logging in */
#define PLR_FREEZE           0x00000040	/* Freeze this character */
#define PLR_HOLYPEACE        0x00010000	/* Nobody can attack anyone while in the
					   room with this immortal. */
#define PLR_HOLYWALK         0x00020000	/* Can walk through anything, anywhere */
#define PLR_HOLYLAG          0x00100000	/* Is this immortal delayed on delayed commands like mortals? */
#define PLR_NOCLAN           0x00200000 /* Is this person forbidden to join a clan? */
#define PLR_LAMER            0x00400000 /* Is this person a lamer? */
#define PLR_NOAUCTION        0x00800000 /* Is this person forbidden to auction? */
#define PLR_SAME_SIDE_PK     0x01000000 /* Can this person same side pk */
#define PLR_PKED_THIS_MORT   0x02000000 /* Has this remort pkilled enough
                                           this remort..vs tough opps. */
#define PLR_HAS_SHOES        0x04000000 /* Players has shoes on */
#define PLR_HAS_MTN_BOOTS    0x08000000 /* Players has mtn boots on*/
#define PLR_HAS_SCUBA        0x10000000 /* Players has scuba on */
#define PLR_HAS_BOAT         0x20000000 /* Players has a boat */
#define PLR_HAS_SNOWSHOES    0x40000000 /* Players snowshoes  */
#define PLR_HAS_SHIELD       0x80000000 /* This player is using a shield */





/* -------------------------------------- */
/* Bits for immort notification on events */
/* -------------------------------------- */
#define WIZ_NOTIFY           0x00000080
#define WIZ_NOTIFY_1         0x00000100
#define WIZ_NOTIFY_2         0x00000200
#define WIZ_NOTIFY_LOGIN     0x00000400
#define WIZ_NOTIFY_DEATH     0x00000800
#define WIZ_NOTIFY_BUG       0x00001000
#define WIZ_NOTIFY_LOG       0x00002000
#define WIZ_NOTIFY_SCRIPT    0x00004000

/* --------------------- */
/* Internal channel ID's */
/* --------------------- */
#define CHANNEL_AUCTION      0x00000001
#define CHANNEL_CHAT         0x00000002
#define CHANNEL_CLANTALK     0x00000004
#define CHANNEL_IMMTALK      0x00000008
#define CHANNEL_NOTIFY       0x00000010
#define CHANNEL_YELL         0x00000080
#define CHANNEL_GODTALK      0x00000100

/* --------------------------------------------------------------- */
/* Auto's conforming to theisles 1.0 and mortal conquest standards */
/* --------------------------------------------------------------- */
#define PLR_AUTOGOLD         0x00000100	/* Gets gold from kills */
#define PLR_AUTOEXIT         0x00000200	/* Displays exits in room desc */
#define PLR_AUTOLOOT         0x00000400	/* Loots all kills */
#define PLR_AUTOSAC          0x00000800	/* Sacs corpses of all kills */
#define PLR_BLANK            0x00001000	/* Blank line before prompt */
#define PLR_BRIEF            0x00002000	/* Brief mode - no long room descs */
#define PLR_COMBINE          0x00004000	/* Combine object listings */
#define PLR_PROMPT           0x00008000	/* Prompt is on/off */
#define PLR_TELNET_GA        0x00010000 /* Non-compat, but same name!  Warning */
#define PLR_ASSIST           0x00020000	/* Auto-assists people in group against
					   mobs.  To assist against other players,
					   you must manually type assist. */
#define PLR_AUTOSPLIT        0x00040000	/* auto-splits any cash looted */

#define PLR_SILENCE          0x00080000	/* Player cannot use channels */
#define PLR_AUTOSCAN         0x00100000 /* You get the scan messages from
                                           players of opp align in adjacent
                                           rooms */
#define PLR_AUTOBLINK        0x00200000 /* Player uses blink skill. */
#define PLR_ANSI             0x00400000	/* Ansi is on/off for this player? */
#define PLR_VT100            0x00800000	/* Masks PLR_ANSI */
#define PLR_PROMPT_UP        0x08000000	/* Prompt with map or down below? */
#define PLR_NOSUMMON         0x10000000 /* Plr cannot be summoned */



/* ---------------------------------------------------------------------- */
/* Mana types retain full compatibility with Mortal conquest and Theisles */
/* ---------------------------------------------------------------------- */
#define MANA_ANY           0x80
#define MANA_NONE          0x00
#define MANA_EARTH         0x01
#define MANA_AIR           0x02
#define MANA_FIRE          0x04
#define MANA_WATER         0x08
#define MANA_SPIRIT        0x10

/* ------------------- */
/* Spelltype targets.. */
/* ------------------- */
#define TAR_IGNORE                  0x00000000	/* No target needed */
#define TAR_CHAR_OFFENSIVE          0x00000001	/* Offensive spell */
#define TAR_CHAR_DEFENSIVE          0x00000002	/* Defensive spell */
#define TAR_CHAR_SELF               0x00000004	/* Self only */
#define TAR_OBJ_INV                 0x00000008	/* Object in inventory */
#define TAR_ALL_IN_ROOM             0x00000010	/* Everyone in room */
#define TAR_OBJ_ROOM		    0x00000080  /* Object in the room */

#ifdef WINDOWS
#define PLAYER_DIR      "..\\plr\\"
#define PLAYER_DIR_2    "..\\plr\\"
#define NULL_FILE       "..\\src\\LoC"
#else
#define PLAYER_DIR      "../plr/"
#define PLAYER_DIR_2    "../plr/"
#define NULL_FILE       "/dev/null"
#endif

#define AREA_LIST       "playarea.lst"
#define NOTE_FILE       "plnotes.txt"

#define MAX_DISABLED_COMMANDS 500

extern char *globalalias_name[200];
extern char *globalalias_expansion[200];

struct scriptassoc {
	SA	*next;
	char	*flagname;
	char	*flagtext;
};

extern SA *sa_head[27];  /* 1=A, 26=Z */

struct auct_data
{
  CHAR_DATA *bychar;
  CHAR_DATA *tochar;
  short going;
  long minbid;
  long bid;
  int number;
  char item_name[SML_LENGTH];
  SINGLE_OBJECT *item;
 AUCTION *next;
};









/* ----------------------------------------------------------------- */
/* This reads from dam.dat.  Allows for configurable damage messages */
/* ----------------------------------------------------------------- */
struct dammsg_data
  {
    short generic_range[20][2];
    short generic_percent[20][2];
    char *gen_m[20];
    char *gen_m1[20];
    char *gen_m2[20];
  };

/* ----------------------------------------------------------------------- */
/* This reads from power.dat.  It can be used to configure the entire mud. */
/* ----------------------------------------------------------------------- */
struct power_data
  {
    bool all_newbies_good;	/* Do all newbies start as good? */
    int level_to_choose;        /* Level newbies choose their side */
    int choose_in_room;         /* Which room people choose sides in */
    short initial_hp[2];	/* Initial hit points for newbies */
    short remort_hp[2];		/* Remort initial hit points */
    short initial_mv[2];	/* Initial move points for newbies */
    short remort_mv[2];		/* Remort initial move points */
    short hitgain[10][5];	/* Hit gains in a given level range */
    short rem_hitgain[10][5];	/* Remort hit gains in a given level range */
    short movegain[10][4];  	/* Move gains in a given level range */
    short rem_movegain[10][4];	/* Remort move gains in a given level range */
    bool remort_bonus_fixed;    /* Tells if remort bonus is fixed JRAJRA */
    int remort_in_room[11];         /* Must be here to "remort" */
    int ascend_remort_in_room[11];         /* Must be here to "remort" */
    int transform_in_room;	/* Must be in this room to transform */
    int asctransform_in_room;   /* Must be in this room to transform if ascended */
    bool add_to_stats;		/* Remorts add stats? */
    bool augmentremortonly;	/* Can you only augment at remort? */
    short max_remorts;		/* Maximum remorts */
    short remort_level;		/* Level you must be to remort */
    bool vaporize_inv;		/* Vaporize inventory when remorting? */
    bool vaporize_all;		/* Vaporize everything when remorting? */
    int beams_to_room;		/* Jump to this room after remorting? */
    bool weather;		/* Weather effects on/off */
    bool player_prompt;		/* Player defined prompts? */
    bool skill_loss_on_mob_death; /* skills lost on mob death? */
    bool skill_loss_on_pk_death;  /* skills lost on pk death? */
    int amount_lost_on_death;	  /* Actual skill amount lost on death */
    bool arena_on;		/* Arena active? */
    bool arena_good_evil;	/* Good/Evil fights in arena? */
    bool validation;		/* Validation system on/off */
    char email_to[50];		/* Email to here for validation */
    int space_allowed;          /* Space allowed for bootup */
    bool easier_moves;		/* Easier movement method */
    int max_prac_spells;	/* Maximum spell percentage you can practice to */
    int max_prac_skills;	/* Maximum skill percentage you can practice to */
    int newbie_object[10];	/* Newbies start with these objects */
    bool allow_where;		/* Allow use of 'where' command? */
    int space_needed;		/* Shared string space needed on bootup. */
    bool evil_can_cast;		/* Evil can cast spells? */
    short practice_percent;	/* Use to scale practices per level */
    short repair_weapons[3];	/* Costs to repair weapons */
    short repair_armors;	/* Costs to repair armor */
    short tinker_discount;	/* Discount for tinker guild members */
    short resize_divisor;	/* Resize divisor for resize cost */
    short groundfight_stand;	/* Chance of standing in a groundfight */
    bool equip_in_combat;	/* Equip armor in combat? */
    bool remove_while_fighting;	/* Remove armor while fighting? */
    bool score_while_fighting;	/* Use score while fighting? */
    bool give_while_fighting;	/* Give objects while fighting? */
    char *god_names[5];		/* Holds the name of mudgods. */
    bool auto_auction;		/* Allow auto-auction? */
    int level_to_see_numeric_stats; /* Level to see numeric stats, unused. */
    short base_hp_regen;	/* Base hp regen per heartbeat */
    short base_mp_regen;	/* Base mp regen per heartbeat */
    short hp_bonus_resting;	/* Hp regen bonus for resting */
    short hp_bonus_sleeping;	/* Hp regen bonus for sleeping */
    short mp_bonus_resting;	/* Move bonus for resting */
    short mp_bonus_sleeping;	/* Move bonus for sleeping */
    short med_mana_regen_bonus;	/* Meditating regen for mana */
    short base_mana_regen_bonus; /* Non-meditating regen for mana */
    bool tackle_with_weapon;	/* Allow tackling with a weapon? */
    bool tackle_person_with_weapon; /* Allow tackling a person with a weapon? */
    bool allow_rerolls;		/* Allow rerolls in old creation method? */
    short flurry_mps;		/* Move point cost for flurry */
    char *learned[20];		/* Description of how well you know spells/skills */
    short low_range[20];	/* Range for learned */
    short high_range[20];	/* Range for learned */
    char *exp_to_level;		/* Interpretted exp to level. */
    char *highlev_exp_to_level; /* Interpretted highlevel exp to level */
    short prereq;		/* Minimum you must know a given skill for it
				   to count as a satisfied prerequisite. */
    char *mob_exp;		/* Mob exp to level, interpretted. */
    short armor_lev_min[20];	/* Armor level range */
    short armor_lev_max[20];	/* Armor level range */
    char *armor_lev_word[20];	/* Armor level range descriptions. */
    bool restore_when_level;	/* Free restore when you level? */
    bool can_summon_mobs;	/* Does the game allow mob summoning? */
    bool morts_use_color_chat;	/* Can morts use colors in channels? */
    bool show_race_select_info;	/* Shows race stats to newbies? */
    char flee_chance;		/* Percent chance that fleeing will fail */
    bool bash_slip_to_tackle;	/* Can a bash slip into a groundfight? */
    bool penalize_evil_in_sun;	/* Penalize evil in direct sunlight? */
    bool get_from_corpse_while_in_combat; /* Loot corpses while fighting? */
    char plague_chance;		/* Chance others in the room will catch
				   plague from you, if infected. */
    char *gods;			/* Gods who run the place */

    int  max_level;		/* maximum level, cannot exceed 90 */
    bool can_fight_self;	/* Can fight yourself? */
    char loot_level;		/* What level can you loot a corpse? */
    bool auction_lag;
    bool good_kill_good;
    bool evil_kill_evil;
    int ascension_req[5];
    int forsake_req[4];
    int remort_req[13][8];
    int ascended_remort_req[13][8];
    int guild_info[8][7];
    int augment_cost[12][7];

};

/* ------------------------------- */
/* For use with syslag, sysfinger. */
/* ------------------------------- */
struct shell_command
  {
    SHELL *next;
    char caller[50];
    char fname[20];
    int wait;
  };

/* ---------------- */
/* Banned site data */
/* ---------------- */
struct ban_data
  {
    BAN_DATA *next;
    char *name;		/* Numeric site ban */
    bool newbie;	/* Newbie banned?  If so, people from this site
			   must go through email validation to play! */
  };

/* ------------------ */
/* Bank item data     */
/* ------------------ */
struct bank_item
   {
     int warpoints;
     int gold;
   };


/* ---------------------- */
/* Combat Field Structure */
/* ---------------------- */

/* -------------------------------- */
/* Damages.. partially implimented. */
/* -------------------------------- */
struct damages_struct
  {
    char *kick_dam;
  };

/* ------------------------------ */
/* The key to the spell editor... */
/* ------------------------------ */
struct spell_data
  {
    SPELL_DATA *next;		/* Next spell, hashed by number */
    SPELL_DATA *next_a;		/* Next spell, hashed by spell name */
    int slot;			/* 0 = skill, 1 = spell, 2 = prof */
    int gsn;			/* General slot number.  internal use */
    int spell_type;		/* Offensive, defensive, etc... */
    int spell_bits;		/* SPELL_SUMMON, etc.. */
    int spell_bits2;            /* More spell bits :P */
    char *spell_name;		/* The spell's name */
    char *spell_funky_name;	/* Should match spell_name. Unused */
    int spell_level;		/* Minimum level to cast this spell */
    char *prereq_1;		/* Spell prereq #1 */
    char *prereq_2;		/* Spell prereq #2 */
    SPELL_DATA *pre1;           /* Prereq1 to be less cpu intensive */
    SPELL_DATA *pre2;           /* Prereq2 to be less cpu intensive */
    int num_prereqs;            /* Number of prereqs for spell */
    bool go_to_second;          /* Is there vert line to second prereq? */
    char *noun_damage;		/* Noun damage for offensive combat spells */
    char *act_to_ch_1;		/* Shown to character when he/she casts */
    char *act_to_ch_2;		/* Just another line to show to char */
    char *act_to_vict_1;	/* To victim */
    char *act_to_vict_2;
    char *act_to_notvict_1;	/* To everyone in room besides char + vict */
    char *act_to_notvict_2;
    int location[5];		/* Modifies this location (str, dex, etc) */
    char *duration;		/* Needs to be a string so we can have vars */
    char *modifier[5];		/* Amount to modify location */
    int bitvector;		/* Bitvector for modification */
    char *damage;		/* If combat spell, how much dam (w/vars) */
    int mana;			/* Minimum Mana cost */
    int mana_type;		/* Mana type */
    int casting_time;		/* Casting delay time. 10 = 1 combat round */
    int position;		/* Position you must be in to cast */
    char *wear_off_msg;		/* When the spell wears off, this msg is disp'd */
    int creates_obj;		/* Creates this object vnum */
    int values[10];		/* Uses these values on creates_obj... UNUSED! */
    				/* -9 means use creates_obj base value */
    unsigned char min_int;	/* Minimum int to learn the spell fully */
    unsigned char min_wis;	/* Minimum wis to learn the spell fully */
				/* These flags are used as con/str for skills */
    char *linked_to;		/* Linked to this spell; when you cast spell
				   A, spell B is also cast.  */
    int bitvector2;		/* More effects */
    int guilds;                 /* Guilds required to cast this spell */
    int in_room_good;           /* Good aligns must be here to cast this */
    int in_room_evil;           /* Evil aligns must be here to cast this */
    int spell_lag;              /* Number of hours between castings */
    int fail_chance;            /* percent chance it fails  */
    int pk_fail_chance;       /* percent chance it succeeds if person has pk noquit timer set > 2 */

  };


/* ------------------------- */
/* Where a blow is struck... */
/* ------------------------- */
#define STRIKE_BODY   1
#define STRIKE_HEAD   2
#define STRIKE_LEGS   3
#define STRIKE_ARMS   4

/* ------------------------- */
/* Structure that armor has. */
/* ------------------------- */
struct item_armor
  {
    char tag;
    signed char warmth;		/* v9 */
    signed char protects_legs;	/* v0 */
    signed char protects_body;	/* v1 */
    signed char protects_head;	/* v2 */
    signed char protects_arms;	/* v3 */
    signed char max_condition;	/* v6 */
    signed char condition_now;	/* v7 */
  };

/* ---------------------------- */
/* Weapons have these values.   */
/* ---------------------------- */
struct item_weapon
  {
    char tag;
    signed char firstdice;	/* v7 */
    signed char seconddice;	/* v8 */
    signed char attack_type;	/* v3 */
    signed char damage_p;	/* v0 */
    signed char strength;	/* v1 */
    signed char ammo_type;   /* v4 */
  };

/* ---------------------------- */
/* Herbs have these values      */
/* ---------------------------- */


struct item_herb
{
  char tag;
  signed char amount;        /* v0 */
  signed char potency;       /* v1 */
  signed char type;          /* v2 */
};

struct item_ammo
{
char tag;
signed char firstdice;      /* v0 */
signed char seconddice;     /* v1 */
signed char ammo_type;      /* v4 shortbow 1, longbow 2, crossbow 3... */
};


struct item_wood
{
char tag;
short type;
};


/* ---------------------------- */
/* Drink containers' values...  */
/* ---------------------------- */
struct item_drink_container
  {
    char tag;
    signed char max_liquid;	/* v1 */
    signed char liquid_now;	/* v0 */
    signed char liquid_type;	/* v2 */
    signed char not_poison;	/* v3 */
  };

/* ------------------- */
/* A light's values... */
/* ------------------- */
struct item_light
  {
    char tag;
    signed char light_lit;	/* v3 */
    signed char light_now;	/* v0 */
    signed char max_light;	/* v1 */
    signed char light_fillable;	/* v2 */
  };

/* etc.. */

struct item_tool
  {
    char tag;
    signed char tool_type;	/* v0 */
    signed char uses;		/* v1 */
    signed char max_uses;	/* v2 */
  };


struct item_hide
{
  char tag;
  char type;  /* v0 */
};

struct item_raw
{
  char tag;
  char type;   /* v0 */
};

struct item_cloth
{
  char tag;
  char type; /* v0 */
};

struct item_trap
{
  char tag;
  signed char material;     /* v0 */
  signed char spell1;       /* v1 */
  signed char spell2;       /* v2 */
  signed char spell3;       /* v3 */
};

struct item_vehicle
  {
    char tag;
    signed char sector_use;	/* v0 */
  };

struct item_fountain
  {
    char tag;
    signed short liquid;	/* v0 */
    signed short spells[3];	/* v1-> */
  };

struct item_potion
  {				/* Scrolls/pills/potions */
    char tag;
    unsigned char spell_level;	/* v0 */
    signed short spells[3];	/* v1-> */
  };


/* ---------------------------- */
/* For spray bottles/repellants */
/* ---------------------------- */
struct item_spray
  {
    char tag;
    signed char sprays;	/* How many charges left */
    int repels[9];	/* Repels mobs of these vnums */
  };


struct item_mineral
{
char tag;
  signed char type;
};


struct item_food
  {
    char tag;
    signed char hours;		/* v0 */
    signed char not_poison;	/* v3 */
  };

struct item_gem
  {
    char tag;
    signed short gem_type;	/* v0 */
    signed short mana_now;	/* v1 */
    signed short max_mana;	/* v2 */
    unsigned char max_level;	/* v3 */
  };

struct item_wand
  {				/* Wands/staffs */
    char tag;
    unsigned char spell_level;	/* v0 */
    signed char max_charges;	/* v1 */
    signed char current_charges;	/* v2 */
    signed short spells[3];	/* v3-> */
  };

struct item_furniture
  {
    char tag;
    signed char type;		/* v0 */
    int to_room_vnum;		/* v1 */
    signed char strip_possessions;	/* v5 */ /* OR!!!! if it is an anvil,
                                         this number is the maximum rank of
                                         item that can be forged on it! */

  };

struct item_container
  {
    char tag;
    int max_weight;		/* v0 */
    int flags;			/* v1 */
    int key_vnum;		/* v2 */
    int money;			/* v8 */
    char reason_for_death[30];
    char looted_by[30];
    char looted_by_two[30];
  };

struct item_climbable
  {
    char tag;
    signed char up_down;	/* v0 */
    int above;			/* v2 */
    int below;			/* v3 */
  };

struct item_book
  {
    char tag;
    signed short teaches;	/* v0 */
  };

struct time_info_data
  {
    short year;
    short month;
    short hour;
    short day;
  };



/* ---------------------------------------------------------------------- */
/* Weather compatibility followed from Theisles.. may be different than
   expected though... warning! :) */
/* ---------------------------------------------------------------------- */
struct weather_data
  {
    int mmhg;		/* Mercury mm */
    short temperature;	/* Farenheight temp */
    int change;		/* Last change */
    short sky;		/* Sky value */
    short sunlight;	/* Sunlight value */
    short winddir;	/* Wind direction */
    short windspeed;	/* Wind speed */
  };


/* ----------------------------------------------------------------------- */
/* The bidding is messy, but it's a port from long ago when I wrote it for
   midpoint void... */
/* ----------------------------------------------------------------------- */
struct bid_data
  {
    char fighters[2][17];	/*[0] and [1] hold the two fighters' names */
    char bidders[50][17];	/*Character names of bidders (didn't to structs cuz
				   of quitting/etc */
    long bids[50];		/*actually monetary bid */
    bool bidfor[50];		/*FALSE for fighter 0, TRUE for fighter 1 */
    bool taking_bids;		/*Are we still up for bidding or has the fight begun? */
    int waitticks;
  };

/* --------------------------------------- */
/* Topten structure holds top ten pkillers */
/* --------------------------------------- */
struct topten_data
  {
    char topten_name[MAX_TOPTEN][17];
    long topten_warpoints[MAX_TOPTEN];
    int topten_alignment[MAX_TOPTEN];
  };

/* --------------------------------------- */
/* Rating data holds top 20 rated players  */
/* --------------------------------------- */
struct rating_data
  {
    char rating_name[MAX_RATING][17];
    long rating_rating[MAX_RATING];
    int rating_alignment[MAX_RATING];
  };


struct pk_data
{
  char name[17];
  float value;
  int alignment;
};



/* --------------------------------------------------------- */
/* Semi-compatibility with Theisles, but nothing guarenteed! */
/* --------------------------------------------------------- */
struct descriptor_data
  {
    socket_t descriptor;		/* Actual file descriptor number */
    DESCRIPTOR_DATA *next;
    DESCRIPTOR_DATA *snoop_by;	/* Snooping by another descriptor */
    CHAR_DATA *original;	/* Original, for use in switching into mobs */
    CHAR_DATA *character;	/* player this descriptor is attached to */
    int timeout_counter;	/* Login screen timeout counter */
    short connected;		/* Connect state */
    int atimes;			/* Authorization ident attempts */
    socket_t auth_fd;		/* Authorization ident file desc */
    int auth_state;		/* Authorization state */
    int auth_inc;
    char abuf[100];		/* Authorization buffer for in/out */
    char host[30];		/* Hostname this player comes from */
    char hosttwo[30];		/* Host prefix for extra checking */
    char *hostname;		/* Hostname.  Unused due to slow dns lookups */
    char *username;		/* Username result from ident */
    int max_outbuf;		/* Maximum outbuffer size used so far.  Outbuffer
				   is allocated in 256 byte chunks, and can
				   grow depending on how much text needs to
				   be in the buffer at once. */
    char inbuf[2002];		/* Input buffer */
    char incomm[2004];		/* Input command buffer */
    char inlast[2004];		/* Last Input buffer contents */
    int repeat;			/* Repeat count; catch spammers */
    char *outbuf;		/* Output buffer pointer */
    char *showstr_head;		/* Head of string pausing */
    char *showstr_point;	/* Pointer to showstring string pausing */
    void *pEdit;		/* Pointer to structure currently editing */
    int outsize;		/* Output size */
    int outtop;			/* Output max */
    int location;		/* Location in fullscreen editor */
    char **pString;		/* Pointer to string currently editing */
    int fdpair[2];		/* File desc pair for shelling */
    short pic_wait;		/* Picture wait ticks */
    short roll[NUM_ROLLS];        /* Rolling choice for each option (current = 4) */
    int act;                    /* Combines a lot of other stuff */
  };


/* --------------------------------------------------------------------- */
/* Theisles and mortal conquest compat.  Yes, I copied their file parsing
   and general helpfile format ideas!  However, since it's not the exact
   same code, I can't guarentee files will be compat... */
/* --------------------------------------------------------------------- */

struct help_data
  {
    HELP_DATA *next;
    short level;	/* Minimum level to see this helpfile */
    char *keyword;	/* Keyword list to get this helpfile */
    char *text;		/* Actual help text */
  };

/* ----------------- */
/* CHANNELS follow.. */
/* ----------------- */
#define TO_ALL -2
#define TO_SAME -1

struct channel_type
  {
    CHANNEL *next;
    bool clan;		/* Clan channel? */
    bool clan2;		/* Sect channel? */
    char *channel_name;	/* Channel name */
    char *commands[3];	/* Commands to use this channel */
    int level;		/* Level to use this channel */
    int to_level;	/* Channel is seen by all higher than this level */
    char *color;	/* Color of this channel for newbies */
    int moves;		/* Takes this amount of moves to use channel */
    char to_align;		/* 1 = same align as you  2 = good  3 = evil
				   4 = both aligns */
  };

/* ---------- */
/* Non-compat */
/* ---------- */
struct shop_data
  {
    SHOP_DATA *next;
    int keeper;			/* Vnum of shopkeeper */
    short buy_type[MAX_TRADE]; 	/* What a shopkeeper will buy */
    short profit_buy;		/* Buying profit, in percent */
    short decrease;		/* Decrease percent for multiple items */
    short items_until_decrease;	/* Decrease percent after this many items */
    short profit_sell;		/* Selling profit, in percent */
    unsigned char open_hour;	/* Time the shop opens */
    unsigned char close_hour;	/* Time the shop closes */
    int creates_vnum;		/* Shopkeeper makes this object */
    short creates_hours;	/* ever creates_hours hours. */
    char *creates_message;	/* display this message when he/she creates */
  };


struct clan_data
  {
    CLAN_DATA *next;		/* Next Clan in List */
    short vnum;			/* Clan vnum */
    char leader[20];		/* Name of Clan's Leader/Owner */
    char name[60];		/* Name of Clan Itself */
    int start_vnum;		/* Starting vnum of where clanhouse begins */
    short alignment;		/* 1=good,2=evil,0=neut,3=special */
    short minlev;		/* Minlevel of new members */
    char *members[100];		/* Clan members */
    SINGLE_OBJECT *storage[MAX_CLAN_STORAGE]; /* Clan storage */
    bool shitlist;
  };


/* -------------------------- */
/* Race data for player races */
/* And now player aligns too  */
/* -------------------------- */
struct race_data
  {
    char name[40];	/* Name of the race */
    short bonus[5];	/* Initial stat bonuses */
    short limits[5];	/* Stat limits */
    int start_room;	/* Starting room,100-110 or so 100 = newbie */
    int max_skills;	/* Max skills */
    int max_spells;	/* Max spells */
    int max_profs;      /* Max proficiencies */
    short height_min;	/* Minimum height */
    short height_max;	/* Maximum height */
    int affect_bits;	/* Natural abilities */
    int affect_bits2;   /* More natural abilities */
    int nat_augments;   /* Natural augments for this race...*/
    /* This is set only when the player enters the game the first time. */
    int natarmor;	/* Natural skin armor class */
    int regen_hit_percent; /* Hit point regeneration percent */
    int regen_move_percent; /* Move point regenration percent */
    bool switch_aligns;   /* Can this race switch alignment? */

    /* These following things are to differentiate races so that they are not
       all clones. The 100 = normal means its a percent of normal...
       0 = normal means its a straight number. */

    int attack_spell_percent;     /* Spell power bonus or penalty... 100 = normal */
    int combat_damage_bonus; /* Natural APB 0 = normal */
    int backstab_percent;    /* Bonus to backstab damage 100 = normal*/
    int heal_percent;    /* Bonus to healing 100 = normal*/
    int hps_bonus;       /* Extra or negative hps per level 0 = normal */
    int mana_percent;      /* Natural extra or less mana 100 = normal */
    int nat_skills[3];   /* Each race has up to three natural skills. */
    int remort_race; /* Is this race 0:normal 1:evolved 2:ascended? */
  };

/* --------------------------- */
/* Compat with mortal conquest */
/* --------------------------- */
struct color_data
  {
    char code[10];
    char act_code[5];
    char name[15];
    int number;
  };

struct material_data
  {
    char *name;
    char *colorname;
    short weight;  /* /4 for body, /5 for legs, /6 for arms, /7 for head,
		     /7 for feet, /9 for gloves */
    char strength; /* the armor durability/hp value */
    short armor;
    int cost;	/* /3 for body, /5 for legs, /5 for arms, /6 for head
		   /7 for feet, /8 for gloves, etc */
    short wpn_start_location;
    bool nametype;
    int initflags;
    short bonushit;
    short bonushp;
    short bonusmove;
    short bigbonus;
    short kickbonus;

  };

struct forged_items
{
char *name; /* For forging of first type */
char *name2; /* For items of second type */
int wear_loc; /* Not including item_take */
int item_type; /* armor wpns tool etc */
int chunk_cost; /* How many chunks to forge this? */
int weight_divisor; /* Divide chunk wgt by this to make the item */
int cost_divisor; /* Divide cost by this */
int num_wear_slots; /* 1 or 2 */
int location;   /* Upper = 0; middle = 1; lower = 2 */
int adjust; /* Are armor or wpn values of this type adjusted up or down? */
int armor_location; /* Where does this armor give ac bonus? 4 = all */
};



/* ----------------------------------- */
/* Flag data for online creation flags */
/* ----------------------------------- */
struct flag_data
  {
    bool shown_in_help;		/* Should this flag be available in helpfiles? */
    char what_you_type[50];	/* What you type to get the flag toggled */
    char how_it_appears[50];	/* How the flag display appears */
    long long flagname;		/* The actual FLAG */
    char what_it_does[255];	/* Help for what it does. */
  };


struct mna_data
{
   char what_you_type[20];          /* The name of the mana type */
   char how_it_appears[20];      /* What actually shows up */
   char color[20];       /* This is the color seen when accessed */
   char letter[3];           /* The single letter used for this */
   int flagname;          /* The flagname associated with this mana */
};

/* ------------------------------------------------- */
/* Note data base is compatible with mortal conquest */
/* ------------------------------------------------- */
struct note_data
  {
    NOTE_DATA *next;
    char *sender;	/* Note's sender */
    char *date;		/* Date it was sent */
    char *to_list;	/* List of people it is to */
    char *subject;	/* Subject of the note */
    char *text;		/* The actual note text */
    int lines;		/* Lines of text in the note */
    time_t date_stamp;	/* Date stamp for unread notes */
    int board_num;	/* Game Board number this note is located on */
  };


/* ----------------------------------------------------------------------- */
/* Affect data - I tried to keep it compat with diku, however no code was
   copied, and I'm only human!  I take no responsibilities for general
   differences that may cause problems in porting code back and forth!! */
/* ----------------------------------------------------------------------- */
struct affect_data
  {
    AFFECT_DATA *next;
    short type;		/* Affect type (gsn) */
    int bitvector;	/* bitvector affects #1 */
    int bitvector2;	/* bitvector affects #2 */
    short modifier;	/* numeric modified */
    short location;	/* locatino modified */
    short duration;	/* duration of affect */
  };


/* Sector data...to keep things neater. */

struct sector_data
{
  int moves;
  char name_seen[40];
  char  name_typed[40];
  char  desc[200];
  char color[40];
  char colort;
};



/* ------------------------------------------------------------------- */
/* Compat with mortal conquest/Theisles, but not meant for use in this
   codebase */
/* ------------------------------------------------------------------- */
struct attack_type
  {
    char *name;
    int hit_type;
  };

struct int_apply_type {
short learn;
};

struct con_apply_type {
short hit;
};

struct str_apply_type {
short dam;
};

struct wis_apply_type {
short practice;
};

struct weapon_damage
{
  short firstdice;
  short seconddice;
};

/* --------------- */
/* Useful macros!! */
/* --------------- */

#ifdef NEW_WORLD
#define IS_IN_BOAT(ch)		FALSE
#else
#define IS_IN_BOAT(ch)        ( (ch)->ced->in_boat != NULL )
#endif

/* -------------------------------------------- */
/* Is the character builder in a specific area? */
/* -------------------------------------------- */
#define IS_BUILDER(ch, area)  ( (LEVEL(ch)>=LEVEL_IMMORTAL && area->open!=1 && \
				(ch->pcdata->security <= area->security \
				|| strstr( area->builders, ch->pcdata->name ) != NULL \
				|| strstr( area->builders, "All" ) != NULL \
				)) \
				|| LEVEL(ch)>=MAX_LEVEL)

#define HUNTING(ch)           ((ch)->fgt ? (ch)->fgt->hunting : NULL)
#define PHUNTING(ch)          ((ch)->fgt ? (ch)->fgt->phunting : NULL)


/* ------------------------------------------- */
/* Basic mob prototype used to create mobiles! */
/* ------------------------------------------- */
struct mob_proto_data
  {
    MOB_PROTOTYPE *next;
    char *name;			/* Keywords used to talk about this mob */
    char *short_descr;		/* short description "a giant cow", etc.. */
    char *long_descr;		/* Long description, shown to room */
    char *description;    	/* Full textual description */
    SPEC_FUNCTION *spec_fun;	/* Special function pointer, compat with diku */
    char *spec_name;		/* Special function name */
    SHOP_DATA *pShop;		/* Shop data, if attached to mob */
    AREA_DATA *area;		/* The area this mob saves in */
    OPTIONAL_DATA *opt;		/* Optional mob data */
    char *attackname;		/* Attack name, NULL for default attack type */
    short max_in_world;		/* Maximum of this mob in the world at once */
    int yeller_number;		/*This is a unique number that
				   can be used to match a help-yeller
				   with its helper(s) in the area.
				 */
    char will_help;		/*Will this mob set it's hunting
				   string to the char name if another
				   mob with the same yeller_number in
				   the area recieves a yell_message?
				 */
    int vnum;			/* The vnum of this mob */
    unsigned char sex;		/* Mob's sex */
    short count;		/* How many mobs are there of this type? */
    short killed;		/* How many times has this mob been killed since reboot? */
    short level;		/* Mob's level */
    short timer;		/* Mob's timer.  Disappears after this many ticks */
    char clan_guard_1;		/* Guards for this clan */
    char clan_guard_2;		/* Guards for this sect */
    int act;			/* Action template */
    int act3;			/* Action template for act3 bit */
    int act4;			/* Action template for act4 bit */
    int affected_by;		/* Affected by these bits */
    int more_affected_by;	/* More affected by bits */
    short alignment;		/* Alignment of the mob */
    short money;		/* The amount of copper this mob has */
    short height;		/* The height of this mob, or 0 for
				   default height for mob type */
    signed char mobtype;	/* Mob type for this mob */
    short hpmult;		/* Hit point and experience multiplier. */
    short armcls;		/* Armor class. 101 = default for level */
    int race_hate;		/* Hate bits for race specific attacks */
    int alt_vnum;		/* Alters into this vnum.. UNUSED! */
    int guard;			/* Guards this direction, this mob vnum, or
			 	   this obj vnum, depends on flags. */
    int hitroll;	        /* Extra hitroll. */
    int damroll;	        /* Extra damroll.  Can be - or + */
    unsigned char dodge;	/* Extra dodge bonus. */
    unsigned char parry;	/* Extra parry bonus, for mobs with weapons */
    int points;			/* Points for obj creation. */
  };

/* --------------------------------- */
/* Character extended data structure */
/* --------------------------------- */
struct ced
  {
    CED_DATA *next;
    CHAR_DATA *attackers[4];	/* Melee attackers */

#ifndef NEW_WORLD
    SINGLE_OBJECT *in_boat;	/* In an OLD format boat? */
    SINGLE_OBJECT *came_from_boat; /* Came from this OLD format boat? */
#endif

    int wasroom;		/* Was in room before arena */
    NOTE_DATA *pnote;		/* Writing this temp note */
    CHAR_DATA *original;	/* Original data for switching */
    bool has_yelled;		/* TRUE if victim has yelled already */
    bool scanning;		/* Internal use with scan function */
    ROOM_DATA *was_in_room;	/* Was in this room before battleground */
    CHAR_DATA *reply;		/* Last person who sent him/her a tell */
    int fight_ops;		/* Fighting options, via 'fight' command */
    short ground_pos;		/* Groundfight position */
    short wimpy;		/* Flee at this many hit points */

/* OLD script format */

    int cur_line;		/*The scriptline that just was run */
    short scr_wait;		/*Wait for the script to continue */
    signed char wait_type;	/*How "wait" is counted down */

  };

/* OLD script wait types */
#define WAIT_TYPE_CHAR_TICK    1
#define WAIT_TYPE_COMBAT_TICK  2
#define WAIT_TYPE_SMALL_TICK   3
#define WAIT_TYPE_PULSE        4


/* -------------------------------- */
/* Track data for tracking, hunting */
/* -------------------------------- */
struct track_data
{
  CHAR_DATA *ch;			/* Character whose tracks these are */
  signed char dir_came;		/* Came from direction */
  signed char dir_left;		/* Left in this direction */
  TRACK_DATA *next_track_in_room;	/* Linked list in room */
};


/**********************************************************
JRAJRA - Player track data...pointer to online instead of ch struct.
**********************************************************/

struct ptrack_data
{
  ONLINE *online;
  signed char dir_came;
  signed char dir_left;
  PTRACK_DATA *next_track_in_room;
};



/*-------------------------------------*/
/* Trap data to make nasty things in   */
/* rooms for unsuspecting players ;)   */
/*-------------------------------------*/


struct trap_data
{
  int level;
  int affects;
  int material;
};


/* ---------------------------------------------------------------- */
/* Temporary fight structure, for use with players or fighting mobs */
/* ---------------------------------------------------------------- */
struct fight_data
  {
    short ears;			/* Ears left, and internally used */
    bool attacker_turn;		/* Internal use */
    char attacks;		/* Internal use */
    int wasroomtwo;		/* Internal use */
    bool upd;			/* Internal use */
    bool in_battle;		/* Is this guy in the battleground? */
    signed char challenge;	/* Has this bloke been challenged to arena fight? */
    CHAR_DATA *leader;		/* Leader, for following */
    CHAR_DATA *master;		/* Master, for grouping, charming */
    CHAR_DATA *riding;		/* Riding for mounts */
    CHAR_DATA *mounted_by;	/* Mounted by */
    char *hunting;		/* Hunting for/tracking */
    char *phunting;              /* Used for ptracking JRAJRA */
    CHAR_DATA *fighting;	/* Currently fighting */
    short combat_delay_count;	/* Delay ticks, internal use */

    /* Built-in script stuff below - goes in fgt.  OOOLD format! */
    short script_delay;
    CHR_FUNCTION *running_char_function;
    short tag_in_proc;
    CHAR_DATA *caller;
    char *argy;
    char *next_command;
    char auto_toggles;
  };

/* ------------------------------------------------ */
/* Internal use flags for list maintenance/checking */
/* ------------------------------------------------ */
#define ISMOB                 0x01	/* Is this a mob? */
#define IS_IN_FIGHTING_LIST   0x02	/* Is this person in the fighting list? */
#define IS_IN_AGGRO_LIST      0x04	/* Is this mob in the aggro list? */
#define CHAR_MODE             0x10	/* Is this player in character mode? */

/* ----------------------------------------------------------------- */
/* The main character data for mob characters and player characters! */
/* ----------------------------------------------------------------- */
struct char_data
  {
    signed char data_type;
    char special;		/* Contains ismob, is_in_fighting_list, etc */
    CHAR_DATA *next_fighting;	/* Next in fighting list */
    DESCRIPTOR_DATA *desc;	/* Descriptor for players, NULL for mobs */
    CHAR_DATA *gen_next;	/* General next, for AGGRO list */
    CED_DATA *ced;		/* Character extended data */
    CHAR_DATA *next;		/* Next in global character list */
    CHAR_DATA *next_in_room;	/* Next person in room list */
    REALCHAR_DATA *pcdata;	/* Player Character Data pointer */
    MOB_PROTOTYPE *pIndexData;	/* Mob prototype reference.  Kept it like DIKU
				   pIndexData so people can port code easier!
				   grr.. how annoying. */
    AFFECT_DATA *affected;	/* Linked list of affects */
    SINGLE_OBJECT *carrying;	/* Objects carrying */
    SINGLE_OBJECT *hold1;       /* Item held in first hand */
    SINGLE_OBJECT *hold2;       /* Item held in second hand */
    ROOM_DATA *in_room;		/* Current room character is in */
    FGT *fgt;			/* Fight data */
    int act;			/* Action bits 1 */
    int affected_by;		/* Affected bits 1 */
    int more_affected_by;	/* Affected bits 2 */
    short height;		/* Height */
    short timer;		/* Timer, AFK for players, destroy for mobs */
    short wait;			/* Wait state */
    long max_hit;		/* Maximum hit points */
    long hit;			/* Hit points now */
    short max_move;		/* Maximum move points */
    short move;			/* Move points now */
    int hitroll;		/* Hitroll */
    int damroll;		/* Damroll */
    short armor;		/* General skin armor */
    signed char position;	/* Physical position */
    int copper;			/* Amount of copper */
    int gold;			/* Amount of gold */
    unsigned long exp;		/* Experience, or experience worth for mobs */
    int number_of_mob_followers;	/* Number of Mobs following this character */
    bool is_undead;
  };

/* Is the person in the battleground? */
#define IN_BATTLE(ch)           ((ch)->fgt ? (ch)->fgt->in_battle : FALSE)

/* Is the person in the arena? */
#define CHALLENGE(ch)           ((ch)->fgt ? (ch)->fgt->challenge : 0)

/* Long description of this char? */
#define LD(ch)                  (IS_PLAYER((ch)) ? (ch)->pcdata->long_descr : (ch)->pIndexData->long_descr)

/* Short description of this char? */
#define SD(ch)                  (IS_PLAYER((ch)) ? (ch)->pcdata->short_descr : (ch)->pIndexData->short_descr)

/* Description of this char? */
#define DSC(ch)                 (IS_PLAYER((ch)) ? (ch)->pcdata->description : (ch)->pIndexData->description)

/* -------------------------------------------------------------------------- */
/* Temp_str holds delayed command targets for spells, searching, backstabbing,
   etc... */
/* -------------------------------------------------------------------------- */
struct temp_str
  {
    TEMP_STR *next;		/* List for free temp_str */
    char temp_string[1024];	/* The actual string */
  };

/***************************************************************
JRAJRA - Thing to keep track of who has been online since the mud
rebooted. The header will be set at bootup.


***************************************************************/

struct online
{
  char *name;
  ONLINE *next;
  int mined;
  int chopped;
  bool located;
  int pk_since_locate;
  int notes_this_reboot;
};

struct potion_ingredients
{
  int gsn;
  int ingredients[10];
  int timer;
  int equipment_level;
};

/* ----------------------------------------- */
/* The main player character data structure! */
/* This is the place to SPLURGE on memory... */
/* ----------------------------------------- */
struct pc_data
  {
    signed char data_type;
    REALCHAR_DATA *next;	/* Next player character data */
    char *pwd;			/* Password */
    char *rpwd;                 /* Real password. */
    char *title;		/* Character's title */
    SINGLE_OBJECT *storage[MAXST]; /* Storage objects in the bank */
    char *rprompt;		/* Real string prompt */
    char *ignore[MAX_IGNORE];	/* Ignored character list */
    char *beamin;		/* Beam in message (immorts) */
    char *beamout;		/* Beam out message (immorts) */
    char *oldgrep;		/* Last grep'd command (immorts) */
    short security;		/* Security level.. UNUSED */
    short wizinvis;		/* WizInvis level */
    short deaf;			/* Deaf to these channels */
    int chan_ignore;		/* Deaf to these new style channels */
    char chan_color[30][10];	/* Channel colors */
    short pray_points;		/* Pray points - used in rolling code */
    bool kk;			/* Show descripted rooms on maps */
    bool mb;			/* Show mobs on maps */
    bool noibm;			/* No IBM chars on maps */
    bool dbs;			/* Double space on maps? */
    char maxx;			/* Max X display on maps */
    long played;		/* Second played */
    short n_mana;		/* Natural mana */
    short n_max_mana;		/* Natural Max mana */
    short saving_throw;		/* Saving throw for spells */
    int cool_skills;		/* Cool extra skills/abilities */
    char *rank;                   /* Kilith '05 */
    signed char resist_summon;	/* Resistance to summon value */
    int plus_kick;		/* Extra kick bonus value */
    int plus_sneak;		/* Extra sneak bonus value */
    int plus_hide;		/* Extra hide bonus value */

    /* ------  New stuff for combat shortening --------  */
    int armor[4];      		 /* The four armor spots for chars... */
    /* ------------------------------------------------- */

    int donated;		/* Donated value */
    int transport_quest;	/* Unused!!  Used internally */
    bool listen_auction;	/* Listening to the current auction bidding? */
    short colors[MAX_COLORS];	/* Colors for old style channels */
    short practice;		/* Practice sessions left. */
    short learn;		/* Learn points left. */
    bool new_exp;		/* New experience format?  Internal use */
    int voting_on;		/* Unused.. internal use */
    short nat_armor;		/* Natural armor class for skin */
    int nat_abilities;		/* Natural abilities */
    int nat_abilities2;         /* More Natural Abilities */
    time_t logon;		/* Logon time */
    time_t last_note;		/* Last note read date-time stamp */
    char cursor;                /* Cursor position */
    short pet_move[20];		/* Pets' move points */
    short pet_hps[20];		/* Pets' hit points */
    short pet_temp[20];		/* Pet's temp values */
    char clan;                  /* Clan number */
    char clan2;                 /* Sect number */
    bool bgf;			/* Battleground flag, internal use */

    char *script_flags[MAX_SCRIPT_FLAGS];/* Actual new script language flags. */

    char email[45];		/* Email address */
    int questflag;		/* Questflag, internal use */
    char remort_times;		/* Remort times.. */
    int ascend_times;		/* Ascend times.. */
    short tickcounts;		/* Spell delay, internal use */
    short auction_count;        /* How much have they auctioned lately. */
    short speedwalk;            /* To stop speedwalking */
    long warpoints;   		/* War points */
    long total_wps;   		/* Total Wps */
    unsigned long pklevels;    	/* Total levels pked */
    unsigned long pkhelpers;   	/* Total number of helpers used to pk */
    unsigned long pkers;      	/* Total number of people req to pkill */
    int pkills;      		/* Total number of pkills */
    int pkilled;     		/* Total number of times pked */
    int hard_pkills;   		/* Total number of HARD pkills.  person has
                          		higher levels AND remorts >= yours*/
    int warmth;                 /* Warmth of the char */
    bool challenge_on;		/* Arena challenges on/off? */
    int arena_msg;		/* Arena message status */
    bool challenged;		/* Has been challenged? */
    char *challenged_by;	/* Challenged by whom? */
    short challenge_time;	/* Challenge time counter */
    TEMP_STR *temp;		/* Temp string for delay command use */
    char number_pets;		/* Number of current pets */
    short pagelen;		/* Physical page length */
    int guilds;			/* Guild flags */
    char no_quit;		/* No_quit timer values */
    char no_quit_pk;		/* No_quit timer values for PK fight */
    char trophy_name[MAX_TROPHY][18];		/* PKill trophy storage */
    short trophy_times[MAX_TROPHY];		/* Times PKilled these people */
    unsigned char trophy_level[MAX_TROPHY];
    char stat[NUM_STATS]; 			/* str, int, wis, dex, con */
    signed char stat_mod[NUM_STATS]; 		/* Str, int, wis, dex, con */
    char start_stat[NUM_STATS]; 		/*str, int, wis, dex, con */
    short condition[3];				/* Drunk,Thirsty,Hungry stats */
    signed char learned[SKILL_COUNT];		/* Learned level for skills and spells */
    unsigned long bank;		/* Money in the bank, in copper */
    int totalkills;		/* Total kills */
    int killpoints;		/* Kill points */
    int deaths;			/* Times the player has died */
    signed char no_spam;	/* NOspam setting */
    short quiet;		/* In quite mode/busy mode/normal mode */
    bool show_lev;		/* Show level in 'who' list? */
    char *aliasname[MAXALIAS];	/* Alias names */
    char *alias[MAXALIAS];	/* Alias expansion */

    /* ------  Unused or disabled for the moment ---- */

    char *wvar;			/* Word variable */
    int nvar;			/* Number variable */

    /* ------  Additions ---------------------------- */
    char *name;			/* Name of player */
    char *short_descr;		/* Short description of player */
    char *long_descr;		/* Long description of player */
    char *description;		/* Textual description of player */
    short alignment;		/* Player's alignment */
    int act2;			/* Player's act2, special bits */
    int act3;			/* act3 */
    int act4;			/* act4 */
    short level;		/* Player's level */
    short carry_weight;		/* Player's current carrying weight */
    short carry_number;		/* Carrying this amount of items */
    char sex;			/* Male/female */
    short race;			/* Player race this character is */
    char *last_tells[15];	/* Circular list of last tells */
    short tell_counter;		/* Tell counter "head" */
    char *last_gtells[15];      /* Circular list of last group tells */
    short gtell_counter;        /* Group tell counter */
    char *last_ctells[15];	/* List of last clan tells */
    short ctell_counter;	/* Clan tell counter */

    ROOM_DATA *temp_room;	/* Temporary room, internal use */
    char maxy;			/* Max Y display on maps */
    ONLINE *online_spot;	/* JRAJRA - Players place in online list */
    char light;                 /* Amt of light the person has. */
    char command_objs;          /* Number of objects the player has that are
                                    command objects (they start command driven
                                    scripts) */
    CHAR_DATA *guarding;        /* The person this one is guarding */
    CHAR_DATA *guarded_by[4];   /* The people who are guarding this person. */
    int faction[MAX_MOB_TYPE];  /* This determines how mobs view you */
    int x;                      /* Plr location in the overhead map */
    int y;                      /* Plr location in the overhead map */
    int augments;               /* Extra augmentations */
    time_t created;             /* When was the player created? */
    int died_recently;          /* How manyt times have you died recent*/
    int mana_shield_strength;   /* Number of hit points your mana shield can take */
    int elemental_alignment;	/* The element to which you are aligned */
    int stance;                 /* Monk stance variable */
    /* ------ New Rolling Code Variables ------- */
    int perm_con;
    int perm_str;
    int perm_dex;
    int perm_int;
    int perm_wis;
    int tmp_con;
    int tmp_str;
    int tmp_dex;
    int tmp_int;
    int tmp_wis;

  };

#define PKF(ch)  (FIGHTING((ch)) && (IS_PLAYER(FIGHTING((ch)))))

#define BLOOD_NORTH      0x00000001
#define BLOOD_EAST       0x00000002
#define BLOOD_SOUTH      0x00000004
#define BLOOD_WEST       0x00000008
#define BLOOD_UP         0x00000010
#define BLOOD_DOWN       0x00000020



/* -------- */
/* Augments */
/* -------- */

#define AUG_LEG_PLATING   0x00000001
#define AUG_BODY_PLATING  0x00000002
#define AUG_HEAD_PLATING  0x00000004
#define AUG_ARM_PLATING   0x00000008
#define AUG_BODY_POWER    0x00000010
#define AUG_HAND_POWER    0x00000020
#define AUG_LEG_POWER     0x00000040
#define AUG_ARM_POWER     0x00000080
#define AUG_MIND_POWER    0x00000100
#define AUG_MIND_FOCUS    0x00000200
#define AUG_FACE_SPIKE    0x00000400
#define AUG_HAND_SPD      0x00000800
#define AUG_FOOT_SPD      0x00001000
#define AUG_LEG_SPD       0x00002000
#define AUG_FOOT_POWER    0x00004000
#define AUG_FOOT_PLATING  0x00008000
#define AUG_MANA_BOOST    0x00010000
#define AUG_CHANNEL       0x00020000
#define AUG_MENTAL_ARMOR  0x00040000
#define AUG_FOCUS         0x00080000

/*------------------------------------*/
/* This number tells varios functions */
/* where to stop counting.            */
/*------------------------------------*/

#define MAX_AUGMENT 17



/* These are some minerals flags which tell what kind of mineral we have
found */

#define MINERAL_BRASS       1
#define MINERAL_BRONZE      2
#define MINERAL_COPPER      3
#define MINERAL_QUARTZ      4
#define MINERAL_IRON        5
#define MINERAL_SILVER      6
#define MINERAL_STEEL       7
#define MINERAL_GOLD        8
#define MINERAL_PLATINUM    9
#define MINERAL_OBSIDIAN    10
#define MINERAL_ONYX        11
#define MINERAL_MOLYBDENUM  12
#define MINERAL_MITHRIL     13
#define MINERAL_ADAMANTIUM  14
#define MINERAL_DIAMOND     15
/* These are the various types of hide players can use to make stuff. */


#define HIDE_SQUIRREL     1
#define HIDE_RABBIT       2
#define HIDE_RACCOON      3
#define HIDE_COW          4
#define HIDE_FOX          5
#define HIDE_DEER         6
#define HIDE_BOAR         7
#define HIDE_WOLF         8
#define HIDE_BEAR         9
#define HIDE_CAT         10
#define HIDE_DRAGON      11

/* These are the various herbs and things you can find. */



/* -------------------------------- */
/* Optional data for mob_prototypes */
/* -------------------------------- */
struct optional_data
{
  short skltaught[30];	        /* Skills this mob teaches */
  char *cast_spells[10];	/* Casts up to 10 spells */
  signed char cast_percent;	/* Percent he/she will cast a spell */
  signed char warrior_percent;  /* Percent the mob does warrior stuff */
  int summons;                  /* Mob number this one summons */
  int avenger;                  /* Mob that appears when this one is killed */
};


/* ------------------------------------------------------------------ */
/* Liquid idea from mortal conquest, and basic compatibility was kept */
/* ------------------------------------------------------------------ */
#define LIQ_WATER        0
#define LIQ_MAX         16

struct liq_type
  {
    char *liq_name;
    char *liq_color;
    short liq_affect[3];	/* 1=drunk 2=food 3=thirst */
  };

/* ----------------------------------------- */
/* Extra descriptions for objects and rooms! */
/* ----------------------------------------- */
/* Once again.. I detest structure names like extra_descr_data, but
   for diku coders' sake, I made it the same as diku since the structure
   is the same (I think).  I woulda called it edd :) */

struct extra_descr_data
  {
    DESCRIPTION_DATA *next;
    char *description;		/* The actual extra description */
    char *keyword;		/* Keyword list to see this desc */
  };

/*------------------------------------- */
/* Evil bits to set.. Aturion muds only */
/*------------------------------------- */
#define SUCK_ALL_MANA            1
#define SUCK_HALF_MANA           2
#define REMOVE_ALL_SPELLS        4
#define SUCK_HALF_HITS           8
#define SUCK_HALF_MOVES         16
#define DESTROY_OBJECT          32
#define DESTROY_RANDOM_OBJECT   64

/* ------------------------------- */
/* The main object prototype data! */
/* ------------------------------- */
struct obj_proto_data
  {
    OBJ_PROTOTYPE *next;
    DESCRIPTION_DATA *extra_descr;	/* Extra description list */
    AFFECT_DATA *affected;		/* Affected by these extra affects */
    AREA_DATA *area;			/* Saves in this area */
    char *name;				/* Keyword list to access object */
    char *short_descr;			/* Short description "a longsword" */
    char *description;			/* Description seen when on ground */
    int vnum;				/* Object vnum */
    int item_type;			/* The item type */
    short max_in_room;			/* Maximum that will pop in 1 room */
    short timer;			/* Object timer for disappearing objs */
    long long extra_flags;	        /* Extra object flags, bonuses, etc */
    int guild_flags;			/* Guild flags */
    int wear_flags;			/* Where can this be worn? */
    int cost;				/* Cost of the object */
    short count;			/* internal use */
    short weight;			/* Weight of the object */
    int value[11];


    char made_of;	/* Material made of */
  };


/* --------------------------------------------------------- */
/* Playerbase data, to finger old chars, player cleanup, etc */
/* --------------------------------------------------------- */
struct playerbase_data
  {
    PLAYERBASE_DATA *next;
    char player_name[17];	/* Player's name */
    time_t player_last_login;	/* Last login date-time stamp */
    int player_align;	/* What alignment is the player? */
    short player_level;		/* Current player level */
    bool player_nodelete;	/* Nodelete flag? */
    char email_address[45];	/* Player's email addy */
    short player_remort_times;   /* Number of remorts the player has */
    time_t player_created;      /* Time stamp when player created. */
  };

/* ------------------------------ */
/* Linked boat list for old boats */
/* ------------------------------ */
struct b_list
  {
    int vnum;
    BLIST *next;
  };

/* ------------- */
/* OLD boat type */
/* ------------- */
struct obj_boat
  {
    CHAR_DATA *in_boat[8];
    CHAR_DATA *navigator;
  };

/* ----------------------- */
/* A single object's data. */
/* ----------------------- */
struct obj_data
  {
    signed char data_type;
    SINGLE_OBJECT *next;
    SINGLE_OBJECT *next_content; /* Generic 'next'/containers, etc */
    SINGLE_OBJECT *contains;	/* For containers */
    SINGLE_OBJECT *in_obj;	/* Inside of object reference */
    CHAR_DATA *carried_by;	/* Carried by this player */
    OBJ_BOAT *boat;		/* OLD boat data */
    OBJ_PROTOTYPE *pIndexData;	/* Reference to object prototype */
    ROOM_DATA *in_room;		/* On the ground in this room? */
    char *short_descr;		/* Short description of the object */
    char *description;		/* 'On ground' description of the object */
    long long extra_flags;		/* Extra object flags */
    int guild_flags;		/* Guild related flags */
    int cost;			/* Object cost.  Change to ref back to proto */
    int wear_loc;		/* Location object is being worn */
    short timer;		/* Timer on this specific object */
    unsigned char size;		/* Size of this specific object */
    void *more;			/* More data.. armor/weapon data, etc */
    AFFECT_DATA *affected;   /* This lets people enchant objects. */
  };


/* ------------------------------------------ */
/* An exit.  Very small for low memory usage. */
/* ------------------------------------------ */
struct exit_data
  {
    int vnum;		/* Vnum this exit leads to */
    ROOM_DATA *to_room;	/* Room data this exit leads to */
    DOOR_DATA *d_info;	/* Optional door info structure */
    bool blood;         /* is there blood this way? */
  };

/* -------------------------------------- */
/* Door info structure, optional on exits */
/* -------------------------------------- */
struct door_data
  {
    short exit_info;	/* Exit flags, mostly compat with diku */
    long key;		/* Key required to open the door */
    char *keyword;	/* keyword for exit description */
    char *description;	/* Exit description */
    signed char maxstr; /* Maximum door strength */
    signed char str;	/* Door strength */
    int rs_flags;	/* Reset Flags for doors, etc */
  };


#define BLOOD_NORTH    0x00000001
#define BLOOD_EAST     0x00000002
#define BLOOD_SOUTH    0x00000004
#define BLOOD_WEST     0x00000008
#define BLOOD_UP       0x00000010
#define BLOOD_DOWN     0x00000020



/* ------------------------------------------------------------------ */
/* A single 'reset'.  SHOULD be compat with merc.. no promises though */
/* ------------------------------------------------------------------ */
struct reset_data
  {
    RESET_DATA *next;
    char command;	/* command (Object Mob, etc) */
    int rs_vnum;	/* Reset vnum */
    int loc;		/* Location or amount for shopkeeprs */
    short percent;	/* Percent chance of pop */
    int vnum;		/* Vnum it pops */
    short arg1;
    short arg2;
    short arg3;
  };


/* Guild data structure....this is so it is easier to add new guilds. */


struct gld_data
  {
    char what_you_see[50];      /* Name you see on score */
    char what_you_type[50];     /* What you type in to join/leave */
    int mob_guildmaster_bit;    /* The bit for the act3 guildmaster */
    int player_guild_bit;       /* The flag set in pcdata->guilds */
    int stat_modified;          /* Which stat is modified based on
				   STR INT WIS DEX CON */
  };
/* -------------------------------------------------------------------------- */
/* All reset values modelled from mortal conquest/theisles values, as to allow
   compatibility in converting areas, if annyone so desires to one day write
   the program the converts */
/* -------------------------------------------------------------------------- */

#define RESET_LOC_INOBJ    -3
#define RESET_LOC_ONOBJ    -4
#define RESET_LOC_INROOM   -5
#define RESET_LOC_PREVOBJ  -6

/* ------------------- */
/* The basic AREA data */
/* ------------------- */
struct area_data
  {
    AREA_DATA *next;
    RESET_DATA *reset_first;	/* First reset in this area */
    RESET_DATA *reset_last;	/* Last reset in this area */
    char *name;			/* Area name */
    char *repop;		/* Repop message */
    char can_quit;		/* Can_quit status; quit rules */
    short desc_rooms;		/* Descripted room count */
    bool has_popped_this_reboot; /* Has repop'd this reboot */
    bool clanzone;		/* Is this a clan zone?  unused */
    short weather_change;	/* Weather change.  unused */
    bool more_rainy;		/* Not implimented */
    bool more_dry;		/* Not implimented */
    bool more_fog;		/* Not implimented */
    short open;			/* Open to mortals? */
    int area_flags;		/* Area flags, changed, etc */
    short security;		/* Security, unused */
    char *builders;		/* Builder list */
    int lvnum;			/* Lower vnum */
    int uvnum;			/* Upper vnum */
    short nplayer;		/* players in area */
    char *filename;		/* Area filename */
    int vnum;			/* Vnum of area */
    int room_count;		/* Rooms in area */
    int mob_count;		/* Mobs in area */
    int obj_count;		/* Objs in area */
    int repop_rate;		/* Repop rate in hours */
    int repop_counter;		/* Current repop counter */
    int wps_value[NUM_ALIGN];   /* How good is this area for pkilling for this align? 10 = normal, 0 = crappy (defensive homelands) */
    /* These last two flags are such that 10 is normal and they are set
       so that they determine the numbers of warpoints awarded for
       pkilling in certain areas vs others. 10 = regular, 20 = double
       etc... */
    bool homeland;
  };

/* ----------------------- */
/* A single room structure */
/* ----------------------- */
struct room_data
  {
    signed char data_type;
    bool shade;			/* Is this room shaded? */
    ROOM_DATA *next;		/* Next in hashed room list */
    AREA_DATA *area;		/* Room this area is in */
    EXIT_DATA *exit[MAX_DIR];	/* Exits from this room */
    int vnum;			/* The room's vnum */
    char *name;			/* Name of the room */
    char *description;		/* The room description */
    int room_flags;		/* Room flags */
    int room_flags2;		/* More Room flags and dynamic desc flags */
    short blood;                  /* Blood info for room... */
    short minerals;             /* Amt of minerals left in the room..set on
				   reboot. */
    short light;		/* Light value of the room */
    short sector_type;		/* The type of sector this room is */
    TRACK_DATA *tracks;		/* Tracks in this room */
    PTRACK_DATA *ptracks;
    ROOM_MORE *more;		/* more room data */
    short command_objs;          /* number of command obj/mob in room */
    char a;
    char c;

    char ibm[2];		/* Overhead mapping information ibm */
    char noibm[2];              /* Overhead mapping information noibm */
    char color;                 /* Room color from the color table. */
    ROOM_DATA *mapexit[4];      /* Whether or not this room is considered
				   to map to an adjacent room setup on
				   REBOOT ONLY!!! */
    int cvnumh;			/* Capture Vnum High Value */
    int cvnuml;			/* Capture Vnum Low Value */
    int calign;			/* Align value */
    char *cbonusstr;		/* Align Bonus String */

    bool y;                     /* Yeller bit */
  };



/* ----------------------- */
/* Extended data for ROOMS */
/* ----------------------- */
struct room_extended
  {
    RESET_DATA *reset_first;	/* First reset in room */
    RESET_DATA *reset_last;	/* Last reset in room */
    SINGLE_OBJECT *contents;	/* Object contents of room */
    short pcs;			/* Player characters in room */
    CHAR_DATA *people;		/* People list in room */
    signed char move_dir;	/* Moving direction for 'currents' */
    char *move_message;		/* moving message for 'currents' */
    int copper;			/* Copper in room */
    int gold;			/* Gold in room */
    DESCRIPTION_DATA *extra_descr;	/* Extra descriptions in room */
    TRAP_DATA *trap;            /* Info on a trap in the room (if any) */
/* --------------------------- */
/* Below are for room VEHICLES */
/* --------------------------- */

  };

/* --------------------- */
/* Is a room descripted? */
/* --------------------- */
#define DESCRIPTED(room)   ((room->description && room->description[0]!='\0') )

/* --------------------- */
/* A single command link */
/* --------------------- */
struct cmd_type
  {
    char *name;		/* Name of the command */
    char *help;		/* Help text on the command */
    DO_COMMAND *do_fun; /* Command executes this function */
    short level;	/* Minimum level you must be to execute command */
    short position;	/* Minimum physical position to execute command */
    short log;		/* Log command status */
    COMMAND *next;	/* hashed by starting letter */
  };


/* --------------------------------------------------------------------- */
/* Social structure compat with theisles.. please note that the way they
   are saved/edited dynamically is VERY different!!! */
/* --------------------------------------------------------------------- */
struct social_type
  {
    char name[20];
    char *char_no_arg;
    char *others_no_arg;
    char *char_found;
    char *others_found;
    char *vict_found;
    char *char_not_found;
    char *char_auto;
    char *others_auto;
    SOCIAL *next;
    SOCIAL *next_hashed;
  };


/* ------------------------------------------------------------------- */
/* Maximum scrollback for channels, and maximum random pop structures. */
/* ------------------------------------------------------------------- */
#define MAX_SCROLLBACK 30

/* Some ideas below from theisles, but I can't guarentee they are exactly
   the same! */

#define RIDING(ch)              ((ch)->fgt ? (ch)->fgt->riding : NULL)
#define MOUNTED_BY(ch)          ((ch)->fgt ? (ch)->fgt->mounted_by : NULL)
#define MASTER(ch)              ((ch)->fgt ? (ch)->fgt->master : NULL)
#define LEADER(ch)              ((ch)->fgt ? (ch)->fgt->leader : NULL)

#define LEVEL(ch)               (IS_MOB((ch)) ? (ch)->pIndexData->level : (ch)->pcdata->level)
#define UMIN(a, b)              ((a)<(b) ? (a):(b))
#define UMAX(a, b)              ((a)>(b) ? (a):(b))
#define URANGE(a, b, c)         ((b)<(a) ? (a):((b)>(c) ? (c) : (b)))
#define LOWER(c)                ((c)>='A'&&(c)<='Z' ? (c)+'a'-'A' :(c))
#define UPPER(c)                ((c)>='a'&&(c)<='z' ? (c)+'A'-'a' :(c))
#define IS_SET(flag, bit)       ((flag)&(bit))
#define SET_BIT(var, bit)       ((var)|=(bit))
#define REMOVE_BIT(var, bit)    ((var)&=~(bit))
#define TOGGLE_BIT(var, bit)    ((var)^=(bit))
#define RNAME(ch)               (IS_PLAYER((ch)) ? (ch)->pcdata->name : (ch)->pIndexData->name)
#define OOSTR(dat, field)       (( (dat)->field != NULL            \
					  ? (dat)->field                \
					  : (dat)->pIndexData->field ))
#define STRR(dat, field)         (( (dat)->field != NULL                    \
						  ? (dat)->field                \
						  : (dat)->pIndexData->field ))
#define NST(pointer)            (pointer == NULL ? "" : pointer)
#define MARK( room )          ( SET_BIT( ( room )->room_flags, R_MARK))
#define UNMARK( room )        ( REMOVE_BIT( ( room )->room_flags, R_MARK))
#define IS_MARKED( room )     ( IS_SET( ( room )->room_flags, R_MARK))
#define TOROOM( room, y )     ( ((ROOM_DATA *) ( room )->exit[ ( y ) ]->to_room ))
#define IS_CLOSED( room, y )  ( IS_SET( ( room )->exit[( y )]->exit_info, \
				EX_CLOSED))

#define VALID_EDGE( room, y )                                              \
			      ( ( room )->exit[( y )] &&                   \
					( TOROOM(( room ), ( y ) ) != NULL ) &&   \
			      ( !IS_MARKED( TOROOM( ( room ), ( y ) ) ) ) )


/* These are the same as found in theisles.  Act codes were designed to be
   almost exactly the same as to allow easier code porting from base to base. */

#define SEX(ch)              (IS_PLAYER((ch)) ? (ch)->pcdata->sex : (ch)->pIndexData->sex)
#define HE_SHE(ch)           ((SEX((ch)) == SEX_MALE  ) ? "he"   :  \
				(  (SEX((ch)) == SEX_FEMALE) ? "she"  : "it" ) )
#define HIS_HER(ch)          ((SEX((ch)) == SEX_MALE  ) ? "his"  :  \
				(  (SEX((ch)) == SEX_FEMALE) ? "her"  : "its" ) )
#define HIM_HER(ch)          ((SEX((ch)) == SEX_MALE  ) ? "him"  :  \
				(  (SEX((ch)) == SEX_FEMALE) ? "her"  : "it" ) )
#define IN_SUN(ch) (IS_SUNNY && (ch)->in_room && (!IS_SET((ch)->in_room->room_flags,ROOM_INDOORS))&& ((ch)->in_room->sector_type != SECT_INSIDE) && ((ch)->in_room->sector_type< SECT_CAVE) && !IS_SET((ch)->in_room->room_flags, ROOM_UNDERGROUND))








#define MASTER(ch)              ((ch)->fgt ? (ch)->fgt->master : NULL)
#define FIGHTING(ch)            ((ch)!=NULL ? ((ch)->fgt ? (ch)->fgt->fighting : NULL) : NULL)
#define IS_AWAKE(ch)            ((ch)->position > POSITION_SLEEPING || (ch)->position<0)
#define IS_MOB(ch)              (IS_SET((ch)->special,ISMOB))
#define IS_PLAYER(ch)           (!IS_SET((ch)->special,ISMOB))
#define IS_IMMORTAL(ch)         (LEVEL((ch)) >= LEVEL_IMMORTAL)
#define IS_GOD(ch)              (LEVEL((ch)) == TOP_LEVEL)
#define IS_AFFECTED(ch, sn)     ((IS_SET((ch)->affected_by, (sn))))
#define IS_AUGMENTED(ch, sn)    ((IS_MOB((ch)) ? TRUE : (IS_SET((ch)->pcdata->augments, (sn)))))
#define IS_AFFECTED_EXTRA(ch, sn)     ((IS_SET((ch)->more_affected_by, (sn))))
#define ALIGN(ch)               (IS_MOB((ch)) ? (ch)->pIndexData->alignment : (ch)->pcdata->alignment)
#define MAP_INROOM (ch)     (((ch)->in_room))


     /* Alignment totally revamped for "kingdoms" code */


#define GET_AC(ch)              ((ch)->armor - (IS_MOB((ch)) ? 0 : (100 - (ch)->pcdata->nat_armor)))

#define IS_OUTSIDE(ch)          (!IS_SET((ch)->in_room->room_flags, ROOM_INDOORS) && ch->in_room->sector_type !=SECT_INSIDE && ch->in_room->sector_type < SECT_CAVE)
#define WAIT_STATE(ch, npulse)  ((ch)->wait = ( ((ch)->position==POSITION_CASTING) ? ((ch)->wait): (UMAX((ch)->wait, (npulse)) ) ))
#define STRING_HITS(ch)         (percent_hit[ (URANGE(0,(PERCENTAGE((ch)->hit, (ch)->max_hit)),101))/10 ]  )

#define STRING_MOVES(ch)        (percent_tired[ (URANGE(0,(PERCENTAGE((ch)->move, (ch)->max_move)),101))/10 ]  )

#define FIND_EXP(lv, r)         (translate(pow.exp_to_level,lv,NULL))

#define DIFF_ALIGN(ch, victim)  (!alliance[ALIGN((ch))][ALIGN((victim))])

#define FIRSTDICE(obj)          ( (obj)->pIndexData->value[7] )
#define SECONDDICE(obj)         ( (obj)->pIndexData->value[8] )
#define MANANOW(obj)            ( ((I_GEM *) (obj)->more)->mana_now )
#define TOPMANA(obj)            ( ((I_GEM *) (obj)->more)->max_mana )
#define TOPLEVEL(obj)            ( ((I_GEM *) (obj)->more)->max_level )
#define TYPEMANA(obj)           ( ((I_GEM*) (obj)->more)->gem_type)
#define CHAR_OBJ_SIZE(ch)       ( (ch)->height<40 ? 1 :                  \
				  ( (ch)->height<55 ? 2 :                \
				    ( (ch)->height<66 ? 3 :              \
				      ( (ch)->height<75 ? 4 :            \
					( (ch)->height<88 ? 5 :          \
					  ( (ch)->height<114 ? 6 : 7 )   \
					 )                               \
				       )                                 \
				     )                                   \
				   )                                     \
				 )

#define SET_OBJ_SIZE(ch,obj)    ( (obj)->size = CHAR_OBJ_SIZE(ch) )
#define GET_OBJ_SIZE(ch)        ( (obj)->size )
#define OBJ_TOO_BIG(obj,ch)     ( (obj)->size > CHAR_OBJ_SIZE(ch) )
#define OBJ_TOO_SMALL(obj,ch)   ( (obj)->size < CHAR_OBJ_SIZE(ch) )
#define OBJ_FITABLE(obj)        ( (obj)->size == 0 )
#define OBJ_ANYSIZE(obj)        ( (obj)->size == 8 )
#define OBJ_FITS(obj,ch)        ( ((obj)->size   == CHAR_OBJ_SIZE(ch) || \
				   (obj)->size+1 == CHAR_OBJ_SIZE(ch) || \
				   (obj)->size-1 == CHAR_OBJ_SIZE(ch) || \
				  OBJ_ANYSIZE((obj))) )
#define CAN_WEAR(obj, part)     (IS_SET((obj)->pIndexData->wear_flags,  part))
#define IS_OBJ_STAT(obj, stat)  (IS_SET((obj)->pIndexData->extra_flags, stat))
#define IS_PLR_ELEM_ALIGN(ch, stat)  (IS_SET((ch)->pcdata->elemental_alignment, stat))
#define IS_OBJ_GUILD_STAT(obj, stat)  (IS_SET((obj)->pIndexData->guild_flags, stat))
#define IS_REAL_GOD(ch)         ((IS_PLAYER((ch)) && LEVEL((ch))>109) )

#define IS_IN_MELEE(ch,victim)  (!FIGHTING((ch)) || (ch)->position==POSITION_STANDING || (victim)->ced->attackers[0]==(ch) || (victim)->ced->attackers[1]==(ch) || (victim)->ced->attackers[2]==(ch) || (victim)->ced->attackers[3]==(ch))

#define NOT_WORN( obj )         ((obj)->wear_loc == WEAR_NONE   ||     \
				 (obj)->wear_loc == WEAR_BELT_1 ||     \
				 (obj)->wear_loc == WEAR_BELT_2 ||     \
				 (obj)->wear_loc == WEAR_BELT_5    )
#define IS_LIT( obj )           ( (obj)->pIndexData->item_type==ITEM_LIGHT && ((I_LIGHT *) (obj)->more)->light_now > 0 &&  \
				 IS_SET( ((I_LIGHT *) (obj)->more)->light_lit, LIGHT_LIT) )
#define NAME( ch )              (( IS_PLAYER( ch ) ? (ch)->pcdata->name : ch->pIndexData->short_descr ))
#define PERS(ch, looker)     ( (can_see_nextdoor( looker, (ch) ) || can_see( looker, (ch) )) ? NAME(ch) : "someone" )
#define TPERS(ch, looker)   (can_see(looker, (ch) ) ? NAME(ch) : "Someone")
#define NOTIFY(buf, lvl, tog)       ( do_global( buf, lvl, tog | WIZ_NOTIFY ) )


/* Old Theisles and MC flag compatability... for the most part */

#define         AREA_NONE       0
#define         SEX_NONE        4
#define         AREA_CHANGED    1
#define         AREA_ADDED      2
#define         ACT_NONE        0
#define         DIR_MAX         6
#define         ROOM_NONE       0
#define         EXTRA_NONE      0
#define         EX_NONE         0
#define         ITEM_WEAR_NONE  0
#define         ITEM_NONE       0
#define         AFFECT_NONE     0

#define SPELL_HASH 24

#define LOG_ALWAYS 0
#define LOG_NEVER  1
#define LOG_NORMAL 2

/* Allows for module interface without 'command' recompile!!!!!!!! */

#ifdef WINDOWS
#define DEFINE_COMMAND(chars,function,pos,lvl,loglvl,help) \
		static bool dun=FALSE; \
		if (!dun) { \
			add_to_list((chars),(function),(pos),(lvl),(loglvl),(help)); \
			dun=TRUE; return; \
			} \
/*		strcpy(cl,(chars)); strcat(cl,":"); \
		if (ch) { \
			strcat(cl,NAME(ch)); \
			}	\
		if (argy) {strcat(cl,":");strcat(cl,argy);}*/
#else
#define DEFINE_COMMAND(chars,function,pos,lvl,loglvl,help) static bool dun=FALSE; \
		if (!dun) { \
			add_to_list((chars),(function),(pos),(lvl),(loglvl),(help)); \
			dun=TRUE; return; \
		}
/*
		strcpy(cl,(chars)); strcat(cl,":"); \
		if (ch) { \
			strcat(cl,NAME(ch)); \
			if (argy) { \
				strcat(cl,":"); \
				strcat(cl,argy); \
			} \
		if (ch->in_room) strcat(cl,itoa(ch->in_room->vnum)); \
			else strcat(cl,"NULLROOM"); \
		}
*/
#endif

/*
#define MARK_DEBUG(chars) strcpy(cl,(chars));
#define MARK_CHAR_DEBUG(chars,ch) \
	strcpy(cl,(chars)); strcat(cl,":"); \
	if (ch) {strcat(cl,NAME(ch)); strcat(cl,":");}
*/
#define MARK_DEBUG(c)
#define MARK_CHAR_DEBUG(c,ch)

/* Short configurability macros!! */

/* #define ALLOW_WHERE */

#define CAN_DROP_ITEMS_IN_ARENA(yn)    if ( !(yn) && CHALLENGE(ch)==10) {send_to_char("You may not drop anything in the arena.\n\r",ch); return;}
#define CAN_GIVE_ITEMS_IN_ARENA(yn)    if ( !(yn) && CHALLENGE(ch)==10) {send_to_char("You may not give anything to anyone in the arena.\n\r",ch); return;}
#define CAN_DROP_ITEMS_IN_BGROUND(yn)  if ( !(yn) && IN_BATTLE(ch)) {send_to_char("You may not drop items in the battleground.\n\r",ch); return;}
#define CAN_GIVE_ITEMS_IN_BGROUND(yn)  if ( !(yn) && IN_BATTLE(ch)) {send_to_char("You may not give annything to anyone inn the battleground.\n\r",ch); return;}

#define BUILDERS_CAN_GIVE(yn)        if ( !(yn) && IS_PLAYER(ch) && ch->pcdata->level>99 && ch->pcdata->level<110 ) {send_to_char("Builders cannot use the 'give' command, sorry.\n\r",ch); return;}
#define CAN_FILL_LIGHT_WHILE_LIT(yn) if ( !(yn) && IS_SET(lg->light_lit, LIGHT_LIT) ) {send_to_char("That's not a very good idea when it's lit...\n\r",ch); return;}

#define ALLOW_UNMOTIVATED_PK(yn)  return TRUE;

#define PLAYERS_DONT_GET_A_CORPSE(yn)  if ( (yn) && IS_PLAYER(ch) )

/* OF INTEREST TO SCRIPTWRITERS, SEE ALSO script.doc!!! */





#define STOP_FIGHTING(ch)             ( stop_fighting( (ch), FALSE ) )
#define STOP_BOTH_FIGHTING(ch)        ( stop_fighting( (ch), TRUE ) )
#define STOP_ALL_IN_ROOM_FIGHTING(ch) ( do_peace( (ch), "") )
#define START_FIGHTING(ch, opponent)  ( multi_hit( (ch), (opponent), TYPE_UNDEFINED) )
#define FLEE(ch)                      ( do_flee( (ch), "") )
#define DELAY_CHAR(ch, val)           ( (ch)->wait+=(val) )
#define DSAY(n, ch, text)             MARK_CHUNK((n)){do_say((ch),(text));DELAY_SCRIPT(2);}
#define SAY(ch, text)                 ( do_say( (ch), (text) ) )
#define DO_TELL(ch, recipient, text)  sprintf(tmpt,"%s %s", NAME( (recipient) ), (text)); do_tell( (ch), tmpt )
#define TELL(ch, recipient, text)     sprintf(tmpt,"%s %s", NAME( (recipient) ), (text)); do_tell( (ch), tmpt )
#define DTELL(n, ch, recipient, text) MARK_CHUNK((n)){TELL((ch),(recipient),(text));DELAY_SCRIPT(2);}
#define SOCIAL_TO(ch, to, social)     sprintf(tmpt,"%s %s", (social), NAME( (to) )); interpret( (ch), tmpt )
#define SOCIAL(ch, social)            interpret( (ch), (social) )
#define SET(thing,value)              ( (thing) = (value) )
#define ONE_COMBAT_ROUND              (  3 )
#define ONE_SECOND                    (  1 )

#define WORKS_ON_MOBS_ONLY     if (IS_PLAYER(ch)) return
#define WORKS_ON_PLAYERS_ONLY  if (IS_MOB(ch)) return

#define INTEGER(name)        static int (name)
#define TEMPSTRING(name)     char (name)[512]
#define STRING(name)         static char (name)[1024]
#define OBJECT(name)         static SINGLE_OBJECT (name)
#define PLAYER(name)         static CHAR_DATA (name)
#define MOB(name)            static CHAR_DATA (name)

#define WRITELN(text,ch)       send_to_char((text),(ch)); send_to_char("\n\r",(ch))
#define WRITE(text,ch)         send_to_char((text),(ch))
#define SHOW_TO_ROOM(text,ch)  do_recho( (ch), (text) )
#define SHOW_TO_OTHERS(text,ch) act((text),mob,NULL,ch,TO_NOTVICT)
#define EXECUTE_COMMAND(ch,text) interpret( (ch), (text) )
#define TEMPFLAG(num)           (ch->pcdata->tempflag[(num)])
#define FLAG(num)               (ch->pcdata->complete[(num)])
#define DESTROY(ch,vnum)        do_destroy( (ch), (vnum) )
#define NUM(str)                (atoi((str)))
#define AWARDXP(ch,amt)         sprintf(tmpt,"pset %s addxp %d", NAME( (ch) ), (amt)); interpret( findmob(), tmpt )
#define COINS(ch,amt)           sprintf(tmpt,"coins %d", (amt) ); interpret(ch,tmpt)
#define GIVECOINS(from,ch,amt)       sprintf(tmpt,"give %s %s", (amt), capitalize(NAME( (ch) )) ); interpret(from,tmpt)
#define GIVEITEM(from,to,item)  sprintf(tmpt,"give %s %s",(item), NAME( (to) )); interpret(from,tmpt)
#define CONTAINS(looking,str)   (!str_infix( (looking), (str) ) )
#define AWARDSKILL(ch,skill,amt) sprintf(tmpt,"award %s '%s' %d",NAME( (ch) ), (skill), (amt) ); interpret(findmob(), tmpt )
#define RAND(min,max)           ( number_range( (min), (max) ) )
#define IS_WINTER               ((time_info.month<=3 || time_info.month>19))
#define IS_SPRING               ((time_info.month>3 && time_info.month<=7))
#define IS_SUMMER               ((time_info.month>7 && time_info.month<=12))
#define IS_AUTUMN               ((time_info.month>12 && time_info.month<=19))
#define IS_RAINING              ((weather_info.sky==SKY_RAINING))
#define IS_CLEARSKY             ((weather_info.sky==SKY_CLOUDLESS))
#define IS_CLOUDY               ((weather_info.sky==SKY_CLOUDY))
#define IS_STORM                ((weather_info.sky==SKY_LIGHTNING))
#define IS_NIGHT                (weather_info.sunlight == SUN_DARK)
#define IS_DAY                  (!IS_NIGHT)
#define IS_SUNNY                (weather_info.sky == SKY_CLOUDLESS && weather_info.sunlight > SUN_DARK)
/* Utility Defines */

/* With loop, 'next' argument must be like "obj->next", or "obj_next" */
/* Example: LOOP(ch,char_list,ch->next) { blah } */

#define LOOP(thing,initial,next)  for ((thing)=(initial);(thing)!=NULL;(thing)=(next))


/* For timing; defines */
#define FIGHT_LOOP	0
#define CHAR_ONLY_UPD	1
#define MOBILE_UPD	2
#define FIGHT_UPD	3
#define TRANSPORT_UPD	4
#define CHAR_UPDATE	5
#define OBJ_UPDATE	6
#define AGGRO_UPDATE	7
#define INTERPRETTED	8
#define SCRIPT_INT	9
#define ALL_UPD		10
#define MT 		20
/* ------------------- */

/* ------------------- */
/* All globals below!! */
/* ------------------- */
extern long average_time[MT];
extern long times_through[MT];
extern long total_time[MT];
extern long temp_time[MT];
extern long this_time[MT];
extern bool hitop;
extern bool big_group_pkill_whore;
extern char commandlog[3000];
extern int main_control;

extern int roll_values[NUM_ROLLS][6][NUM_STATS][2]; /* 4 steps, 6 letter choices for each stat, lower and upper bounds for each stat..makes editing easier :)  it looks ugly but its not so bad. */
extern bool kk;
extern bool noibm;
extern bool mb;
extern bool dbs;
extern int entlab;
extern SOCIAL *social_list;
extern SOCIAL *social_hashed[255];
extern int outgoing[5];
extern char connection_names[5][5];
extern char *chan[NUM_ALIGN][30][MAX_SCROLLBACK];
extern short chan_count[NUM_ALIGN][30];
extern bool supress_prompt_newline;
extern bool allow_anything;
extern bool alliance[NUM_ALIGN][NUM_ALIGN];
extern char grep[500];
extern int scrn[80][25][9];
extern int total_fgt;
extern BANK_ITEM align_bank[NUM_ALIGN];
extern BANK_ITEM clan_bank[30];
extern FILE *plrchecker;
extern FILE *corpsefile;
extern CHAR_DATA *f_first;
extern SINGLE_OBJECT *bg_prize[MAX_NUM_BG_PRIZES];
extern char *string_space;
extern char *top_string;
extern CHAR_DATA *aggro_check;
extern int ti;
extern bool oldflag;
extern DAM_MSG dmsg;
extern POWER pow;
extern SHELL *shell_first;
extern bool fLogAll;
extern CHANNEL *chan_first;
extern bool wwrap;
extern char *score;
extern char *sh_score;
extern char *info;
extern char reasonfd[30];
extern CHAR_DATA *last_pkiller;
extern TO_FREE *head;
extern bool is_equipwipe;
extern CHR_FUNCTION *cf;
extern int room_chunk;
extern int obj_chunk;
extern int mob_chunk;
extern char tmpt[2048];
extern bool quit_flag;
extern bool pet_flag;
extern bool got;
extern char hugebuf_o[70000];
extern bool refresh_oldchars;
extern int current_vnum;
extern bool builder_port;
extern AREA_DATA *saving;
extern int max_connected;
extern short top_temp;
extern short top_ced;
extern CED_DATA *ced_first;
extern CED_DATA *ced_free;
extern bool translate_on;
extern short combat_state;
extern PERCENT_STRUCT prcnt;
extern DAM_STRUCT damt;
extern long chars_sent;
extern bool setskills;
extern bool state_var_1;
extern char *target_name;
extern bool fBootDb;
extern int meg_sent;
extern CHAR_DATA *pedit_found;
extern int area_this_reboot;
extern int pulse_timer;
extern int min_level;
extern int max_level;
extern int bg_multiplier;
extern char max_remort_times[NUM_ALIGN];
extern long day_counter;
extern int cons_damage;
extern char *const color_name[];
extern char *const where_name[];
extern char *const dist_name[];
extern char *const percent_ded[];
extern int top_vnum_room;
extern int arena_timelimit;
extern REALCHAR_DATA base_pc;
extern PLAYERBASE_DATA *playerbase_zero;
extern ITEM_CHECK itemcheck;
extern char condition_var[500];
extern int ticks_to_reboot;
extern int ticks_to_battlefield;
extern bool changed_spells;
extern const char position_name[MAX_POSITIONS][20];
extern BID_DATA biddata;
extern TOPTEN_DATA topten;
extern RATING_DATA ratingd;
extern PK_DATA pkdata[MAX_PKDATA][PKN];


extern CLAN_DATA *clan_index_free_2;
extern CLAN_DATA *clan_index_free;
extern AUCTION *auction_list;
extern int auction_number;
extern short gsn_build;
extern short gsn_ranged;
extern short gsn_shoot;
extern short gsn_throw;
extern short gsn_chop;
extern short gsn_enchant;
extern short gsn_weave;
extern short gsn_stitch;
extern short gsn_mine;
extern short gsn_skin;
extern short gsn_tanning;
extern short gsn_patch;
extern short gsn_detrap;
extern short gsn_locate_trap;
extern short gsn_awareness;
extern short gsn_block;
extern short gsn_flail;
extern short gsn_critical;
extern short gsn_guard;
extern short gsn_warcry;
extern short gsn_invest;
extern short gsn_riding;
extern short gsn_backstab;
extern short gsn_penetration;
extern short gsn_grapple;
extern short gsn_swim;
extern short gsn_butcher;
extern short gsn_dodge;
extern short gsn_evade;
extern short gsn_hide;
extern short gsn_peek;
extern short gsn_pick_lock;
extern short gsn_sneak;
extern short gsn_steal;
extern short gsn_bash;
extern short gsn_track;
extern short gsn_trapset;
extern short gsn_circle;
extern short gsn_fireshield;
extern short gsn_manashield;
extern short gsn_sixth_sense;
extern short gsn_disarm;
extern short gsn_enhanced_damage;
extern short gsn_resilience;
extern short gsn_accuracy;
extern short gsn_kick;
extern short gsn_tornado;
extern short gsn_double_kick;
extern short gsn_smash;
extern short gsn_impale;
extern short gsn_charge;
extern short gsn_flurry;
extern short gsn_tackle;
extern short gsn_flash;
extern short gsn_parry;
extern short gsn_blink;
extern short gsn_steadiness;
extern short gsn_shield_block;
extern short gsn_rescue;
extern short gsn_second_attack;
extern short gsn_third_attack;
extern short gsn_dual_wield;
extern short gsn_pierce;
extern short gsn_unarmed;
extern short gsn_spiritregen;
extern short gsn_buddha;
extern short gsn_nerve;
extern short gsn_light_slash;
extern short gsn_concussion;
extern short gsn_polearm;
extern short gsn_whip;
extern short gsn_heavy_slash;
extern short gsn_slashing;
extern short gsn_search;
extern short gsn_forge;
extern short gsn_brew;
extern short gsn_lore;
extern short gsn_blindness;
extern short gsn_spell_reflect;
extern short gsn_chameleon;
extern short gsn_raise_undead;
extern short gsn_life_tap;
extern short gsn_curse;
extern short gsn_invis;
extern short gsn_poison;
extern short gsn_sleep;
extern short gsn_plague;
extern short gsn_paralyze; /* JRAJRA */
extern short gsn_wound; /* JRAJRA */
extern short gsn_confuse; /* JRAJRA */
extern short gsn_lethargy; /* JRAJRA */
extern short gsn_mindsap; /* JRAJRA */
extern char yellbuff[1000];
extern const struct attack_type attack_table[];
extern const struct liq_type liq_table[];
extern char *const dir_name[];
extern const short rev_dir[];
extern const int blood_number[];
extern char reasonone[30];
extern HELP_DATA *help_first;
extern HELP_DATA *help_last;
extern SHOP_DATA *shop_first;
extern SHOP_DATA *shop_last;
extern TEMP_STR *temp_first;
extern TEMP_STR *temp_free;
extern BLIST *base_blist;
extern BAN_DATA *ban_list;
extern CHAR_DATA *char_list;
extern SPELL_DATA *spell_list[SPELL_HASH];
extern SPELL_DATA *spell_list_2[SPELL_HASH];
extern int top_spell;
extern DESCRIPTOR_DATA *descriptor_list;
extern NOTE_DATA *note_list;
extern SINGLE_OBJECT *object_list;
extern AFFECT_DATA *affect_free;
extern RESET_DATA *reset_free;
extern BAN_DATA *ban_free;
extern CHAR_DATA *char_free;
extern DESCRIPTOR_DATA *descriptor_free;
extern DESCRIPTION_DATA *extra_descr_free;
extern NOTE_DATA *note_free;
extern SINGLE_OBJECT *obj_free;
extern REALCHAR_DATA *pcdata_free;
extern MOB_DATA *npcdata_free;
extern char bug_buf[];
extern time_t current_time;
extern time_t startup_time;
extern bool fLogAll;
//extern FILE *fpReserve;
extern char log_buf[];
extern TIME_INFO_DATA time_info;
extern WEATHER_DATA weather_info;
extern MOB_PROTOTYPE *mob_index_hash[HASH_MAX];
extern OBJ_PROTOTYPE *obj_index_hash[HASH_MAX];
extern ROOM_DATA *room_hash[HASH_MAX];
extern CLAN_DATA *clan_index_hash[20];
extern CLAN_DATA *clan_index_hash_2[20];
extern char *string_hash[HASH_MAX];
extern AREA_DATA *area_first;
extern AREA_DATA *area_last;
extern ONLINE *been_online; /* JRAJRA - been online checker list */
extern int top_affect;
extern int top_area;
extern int top_ed;
extern int top_exit;
extern int top_help;
extern int top_clan;
extern int top_clan_2;
extern int top_mob_index;
extern int top_obj_index;
extern int top_reset;
extern int top_room;
extern int top_shop;
extern DESCRIPTOR_DATA *connectiond[5];
extern int top_vnum_mob;
extern int top_vnum_obj;
extern int top_vnum_room;
extern char str_empty[1];
extern int FORCE_LEVEL;
extern int LOG_LEVEL;
extern char *disabled_commands[MAX_DISABLED_COMMANDS];

/* All functions below */

void showRoller(DESCRIPTOR_DATA *,CHAR_DATA *);
char* get_main_guild(CHAR_DATA *);
char* get_main_elemental_align(CHAR_DATA *);
void remove_from_fighting_list (CHAR_DATA *);
void clear_fgt (CHAR_DATA *);
SOCIAL * new_social (void);
bool one_is_in_list_two (char *, char *);
bool one_is_of_two (char *, char *);
void load_random_pops(void);
void load_new_socials(void);
void show_social (SOCIAL *, CHAR_DATA *);
void social_update_hash(void);
void xprereq (CHAR_DATA *, SPELL_DATA *);
void save_new_socials (void);
void auction_update (void);
void menu_interp(CHAR_DATA *,char *);
void check_background_processes(void);
void save_char_tro(CHAR_DATA *);
void view_scriptassoc (CHAR_DATA *, char *);
void do_quests (CHAR_DATA *, char *);
void reset_data (void);
void go_display(CHAR_DATA *, ROOM_DATA *, int, int, int, int);
void draw_data (CHAR_DATA *, int);
void draw_small (CHAR_DATA *);
void sml_noibm(CHAR_DATA *, ROOM_DATA *, int, int);
void sml_ibm (CHAR_DATA *, ROOM_DATA *, int, int);
void save_code (void);
void save_triggers (void);
void show_guilds (CHAR_DATA *, int);
void do_spico_help (CHAR_DATA *, char *);
void do_shell (CHAR_DATA *, char *);
void flags (CHAR_DATA *, char *);
void debug (CHAR_DATA *, char *);
void online_coding (CHAR_DATA *, char *);
void list_code (CHAR_DATA *, char *);
void do_build (CHAR_DATA *, char *);
void do_chop (CHAR_DATA *, char *);
void list_triggers (CHAR_DATA *, char *);
void script_editor (CHAR_DATA *, char *);
void do_mat (CHAR_DATA *, char *);
void do_soc_edit (CHAR_DATA *, char *);
void z_areachange (CHAR_DATA *, char *);
void do_setmiw (CHAR_DATA *, char *);
void relocate_objs (CHAR_DATA *, char *);
void relocate_mobs (CHAR_DATA *, char *);
void do_augment (CHAR_DATA *, char *);
void do_ocopy (CHAR_DATA *, char *);
void do_mcopy (CHAR_DATA *, char *);
void do_rcopy (CHAR_DATA *, char *);
void decode (CHAR_DATA *, char *);
void compare (CHAR_DATA *, char *);
void do_repop (CHAR_DATA *, char *);
void clean_oldchars (CHAR_DATA *, char *);
void clean_up (CHAR_DATA *, char *);
void p_refresh (CHAR_DATA *, char *);
void reload_pbase (CHAR_DATA *, char *);
void replace (CHAR_DATA *, char *);
void grab_hostnames (CHAR_DATA *, char *);
void reload_power_dat (CHAR_DATA *, char *);
void do_dir (CHAR_DATA *, char *);
void do_vequipment (CHAR_DATA *, char *);
void create_standards (CHAR_DATA *, char *);
void do_impale (CHAR_DATA *, char *);
void do_charge (CHAR_DATA *, char *);
void do_actual_charge (CHAR_DATA *, char *);
void do_flash (CHAR_DATA *, char *);
void do_bs (CHAR_DATA *, char *);
void do_cedit2 (CHAR_DATA *, char *);
void do_say2 (CHAR_DATA *, char *);
void do_capture (CHAR_DATA *, char *);
void do_emote2 (CHAR_DATA *, char *);
void do_gtell2 (CHAR_DATA *, char *);
void finger (CHAR_DATA *, char *);
void do_color (CHAR_DATA *, char *);
void do_eeffect (CHAR_DATA *, char *);
void do_vscore (CHAR_DATA *, char *);
void do_quickscore2 (CHAR_DATA *, char *);
void do_take (CHAR_DATA *, char *);
void do_swap (CHAR_DATA *, char *);
void do_wield (CHAR_DATA *, char *);
void do_hold (CHAR_DATA *, char *);
void do_bury (CHAR_DATA *, char *);
void do_read (CHAR_DATA *, char *);
void do_clan_2 (CHAR_DATA *, char *);

void do_answer (CHAR_DATA *, char *);
void do_mplay (CHAR_DATA *, char *);
void do_sing (CHAR_DATA *, char *);
void do_flail (CHAR_DATA *, char *);
void do_investigate (CHAR_DATA *, char *);
void do_mobscatter (CHAR_DATA *, char *);
void do_scatter (CHAR_DATA *, char *);
void relocate_rooms (CHAR_DATA *, char *);
void do_reload_help (CHAR_DATA *, char *);
void do_anon (CHAR_DATA *, char *);
void do_gic (CHAR_DATA *, char *);
void do_eic (CHAR_DATA *, char *);
void do_ic (CHAR_DATA *, char *);
void do_list_done_quests (CHAR_DATA *, char *);
void do_drag (CHAR_DATA *, char *);
void do_yes (CHAR_DATA *, char *);
void do_dam (CHAR_DATA *, char *);
void do_beep (CHAR_DATA *, char *);
void do_levels (CHAR_DATA *, char *);
void disengage (CHAR_DATA *, char *);
void do_disembark (CHAR_DATA *, char *);
void do_board (CHAR_DATA *, char *);
void drawit (CHAR_DATA *, char *);
void do_drawmort (CHAR_DATA *, char *);
void do_drawmini (CHAR_DATA *, char *);
void do_drawtop (CHAR_DATA *, char *);
void do_drawrooms (CHAR_DATA *, char *);
void do_resize (CHAR_DATA *, char *);
void clean_blank_rooms (CHAR_DATA *, char *);
void do_setlevels (CHAR_DATA *, char *);
void do_battleground (CHAR_DATA *, char *);
void do_clanbuy (CHAR_DATA *, char *);
void do_purgearea (CHAR_DATA *, char *);
void do_mreset (CHAR_DATA *, char *);
void do_oreset (CHAR_DATA *, char *);
void do_topten (CHAR_DATA *, char *);
void do_rating (CHAR_DATA *, char *);
void do_pkstat (CHAR_DATA *, char *);
void do_clanstore (CHAR_DATA *, char *);
void do_prereqs (CHAR_DATA *, char *);
void do_clanunstore (CHAR_DATA *, char *);
void do_store (CHAR_DATA *, char *);
void do_unstore (CHAR_DATA *, char *);
void do_world (CHAR_DATA *, char *);
void do_preset (CHAR_DATA *, char *);
void do_trophy (CHAR_DATA *, char *);
void do_cadd (CHAR_DATA *, char *);
void do_butcher (CHAR_DATA *, char *);
void do_scan (CHAR_DATA *, char *);
void do_silentscan (CHAR_DATA *, int);
void do_cremove (CHAR_DATA *, char *);
void do_clan (CHAR_DATA *, char *);
void do_clan_notify (CHAR_DATA *, char *);
void do_clantalk (CHAR_DATA *, char *);
void do_tackle (CHAR_DATA *, char *);
void do_repair (CHAR_DATA *, char *);
void do_guild (CHAR_DATA *, char *);
void show_guild_costs(CHAR_DATA *, char*);
void show_augment_costs(CHAR_DATA *, char*);
void mob_act_bits (CHAR_DATA *, char *);
void show_augment_list(CHAR_DATA *, char*);
void show_affect_list(CHAR_DATA *, char*);
void show_affect_list2(CHAR_DATA *, char*);
void check_stats (CHAR_DATA *);

bool is_elemental_aligned(CHAR_DATA *,SPELL_DATA *);
int check_group_pts (CHAR_DATA *);
int get_num_augments(CHAR_DATA *);
int get_num_guilds (CHAR_DATA *);
int get_num_followers (CHAR_DATA *);
void check_same_side_pk (CHAR_DATA *, CHAR_DATA *);
void try_prompt (char *, CHAR_DATA *);
int get_guild_num(CHAR_DATA *);
void do_clear (CHAR_DATA *, char *);
void do_skill (CHAR_DATA *, char *);
void do_flurry (CHAR_DATA *, char *);
void syslag (CHAR_DATA *, char *);
void searchlog (CHAR_DATA *, char *);
void do_delete (CHAR_DATA *, char *);
void do_empty (CHAR_DATA *, char *);
void do_bid (CHAR_DATA *, char *);
void do_surrender (CHAR_DATA *, char *);
void do_bbounty (CHAR_DATA *, char *);
void do_2forward (CHAR_DATA *, char *);
void do_2backward (CHAR_DATA *, char *);
void do_forward (CHAR_DATA *, char *);
void do_backward (CHAR_DATA *, char *);
void do_exchange (CHAR_DATA *, char *);
void do_meditate (CHAR_DATA *, char *);
void do_random_split(CHAR_DATA *, char *);
void do_bash (CHAR_DATA *, char *);
void do_coins (CHAR_DATA *, char *);
void do_ansi (CHAR_DATA *, char *);
void do_fight (CHAR_DATA *, char *);
void do_fire (CHAR_DATA *, char *);
void do_advance (CHAR_DATA *, char *);
void do_allow (CHAR_DATA *, char *);
void do_areas (CHAR_DATA *, char *);
void do_at (CHAR_DATA *, char *);
void do_tauction (CHAR_DATA *, char *);
void do_break (CHAR_DATA *, char *);
void do_alias (CHAR_DATA *, char *);
void do_auto (CHAR_DATA *, char *);
void do_autosac (CHAR_DATA *, char *);
void do_assist (CHAR_DATA *, char *);
void do_autosplit (CHAR_DATA *, char *);
void do_autoloot (CHAR_DATA *, char *);
void do_autoblink (CHAR_DATA *, char *);

void do_autoscan (CHAR_DATA *, char *);
void do_autoexit (CHAR_DATA *, char *);
void do_autoassist (CHAR_DATA *, char *);
void do_autogold (CHAR_DATA *, char *);
void do_backstab (CHAR_DATA *, char *);
void do_circle (CHAR_DATA *, char *);
void do_beamin (CHAR_DATA *, char *);
void do_beamout (CHAR_DATA *, char *);
void do_ban (CHAR_DATA *, char *);
void do_nban (CHAR_DATA *, char *);
void do_blank (CHAR_DATA *, char *);
void do_bounty (CHAR_DATA *, char *);
void do_brief (CHAR_DATA *, char *);
void do_brandish (CHAR_DATA *, char *);
void do_buck (CHAR_DATA *, char *);
void do_buy (CHAR_DATA *, char *);
void do_cast (CHAR_DATA *, char *);
void do_unload(CHAR_DATA *, char *);
void do_load (CHAR_DATA *, char *);
void do_shoot (CHAR_DATA *, char *);
void do_channels (CHAR_DATA *, char *);
void do_gen_gos (CHAR_DATA *, char *);
void do_chat (CHAR_DATA *, char *);
void do_climb (CHAR_DATA *, char *);
void do_close (CHAR_DATA *, char *);
void do_commands (CHAR_DATA *, char *);
void do_compare (CHAR_DATA *, char *);
void do_config (CHAR_DATA *, char *);
void do_consider (CHAR_DATA *, char *);
void do_group_consider (CHAR_DATA *, char *);
void do_count (CHAR_DATA *, char *);
void do_credits (CHAR_DATA *, char *);
void do_deny (CHAR_DATA *, char *);
void do_description (CHAR_DATA *, char *);
void do_disarm (CHAR_DATA *, char *);
void do_disconnect (CHAR_DATA *, char *);
void do_dismount (CHAR_DATA *, char *);
void do_down (CHAR_DATA *, char *);
void do_draw (CHAR_DATA *, char *);
void do_drink (CHAR_DATA *, char *);
void do_drop (CHAR_DATA *, char *);
void do_east (CHAR_DATA *, char *);
void do_eat (CHAR_DATA *, char *);
void do_echo (CHAR_DATA *, char *);
void do_emote (CHAR_DATA *, char *);
void do_enter (CHAR_DATA *, char *);
void do_equipment (CHAR_DATA *, char *);
void do_examine (CHAR_DATA *, char *);
void do_extinguish (CHAR_DATA *, char *);
void do_fill (CHAR_DATA *, char *);
void do_flee (CHAR_DATA *, char *);
void do_fly (CHAR_DATA *, char *);
void do_follow (CHAR_DATA *, char *);
void do_sforce (CHAR_DATA *, char *);
void do_force (CHAR_DATA *, char *);
void do_freeze (CHAR_DATA *, char *);
void do_noclan (CHAR_DATA *, char *);
void do_lamer (CHAR_DATA *, char *);
void do_shitlist (CHAR_DATA *, char *);
void do_finger (CHAR_DATA *, char *);
void do_get (CHAR_DATA *, char *);
void do_give (CHAR_DATA *, char *);
void do_godtalk (CHAR_DATA *, char *);
void do_goto (CHAR_DATA *, char *);
void do_group (CHAR_DATA *, char *);
void do_gtell (CHAR_DATA *, char *);
void do_help (CHAR_DATA *, char *);
void do_help_two (CHAR_DATA *, char *);
void do_events (CHAR_DATA *, char *);
void do_news (CHAR_DATA *, char *);
void do_changes (CHAR_DATA *, char *);
void do_forgecost (CHAR_DATA *, char *);
void do_hide (CHAR_DATA *, char *);
void do_hire (CHAR_DATA *, char *);
void do_rawclear (CHAR_DATA *);
void do_holypeace (CHAR_DATA *, char *);
void do_mapping (CHAR_DATA *, char *);
void do_mapspam (CHAR_DATA *, char *);
void do_noibm (CHAR_DATA *, char *);
void do_viewmobs (CHAR_DATA *, char *);
void do_viewplayers (CHAR_DATA *, char *);
void do_holylag (CHAR_DATA *, char *);
void do_holyear (CHAR_DATA *, char *);
void do_holylight (CHAR_DATA *, char *);
void do_holywalk (CHAR_DATA *, char *);
void do_idea (CHAR_DATA *, char *);
void do_immtalk (CHAR_DATA *, char *);
void do_inventory (CHAR_DATA *, char *);
void do_invis (CHAR_DATA *, char *);
void do_kick  (CHAR_DATA *, char *);
void do_tornado  (CHAR_DATA *, char *);
void do_raceinfo (CHAR_DATA *, char *);
void do_alignmentinfo (CHAR_DATA *, char *);
void do_kill (CHAR_DATA *, char *);
void do_language (CHAR_DATA *, char *);
void do_learn (CHAR_DATA *, char *);
void do_unlearn (CHAR_DATA *, char *);
void do_levels (CHAR_DATA *, char *);
void do_list (CHAR_DATA *, char *);
void do_light (CHAR_DATA *, char *);
void do_lock (CHAR_DATA *, char *);
void do_log (CHAR_DATA *, char *);
void do_memory (CHAR_DATA *, char *);
void do_mfind (CHAR_DATA *, char *);
void do_mload (CHAR_DATA *, char *);
void do_mount (CHAR_DATA *, char *);
void do_mset (CHAR_DATA *, char *);
void do_mstat (CHAR_DATA *, char *);
void do_mwhere (CHAR_DATA *, char *);
void do_murde (CHAR_DATA *, char *);
void do_murder (CHAR_DATA *, char *);
void do_pray (CHAR_DATA *, char *);
void do_noemote (CHAR_DATA *, char *);
void do_north (CHAR_DATA *, char *);
void do_note (CHAR_DATA *, char *);
void do_notell (CHAR_DATA *, char *);
void do_notify (CHAR_DATA *, char *);
void do_ofind (CHAR_DATA *, char *);
void do_oload (CHAR_DATA *, char *);
void do_open (CHAR_DATA *, char *);
void do_order (CHAR_DATA *, char *);
void do_oset (CHAR_DATA *, char *);
void do_ostat (CHAR_DATA *, char *);
void do_owhere (CHAR_DATA *, char *);
void do_pagelen (CHAR_DATA *, char *);
void do_password (CHAR_DATA *, char *);
void no_spam (CHAR_DATA *, char *);
void spam (CHAR_DATA *, char *);
void do_glance (CHAR_DATA *, char *);
void do_prompt (CHAR_DATA *, char *);
void do_peace (CHAR_DATA *, char *);
void do_pick (CHAR_DATA *, char *);
void do_practice (CHAR_DATA *, char *);
void do_pset (CHAR_DATA *, char *);
void do_pstat (CHAR_DATA *, char *);
void do_purge (CHAR_DATA *, char *);
void do_put (CHAR_DATA *, char *);
void do_quaff (CHAR_DATA *, char *);
void do_qui (CHAR_DATA *, char *);
void do_quit (CHAR_DATA *, char *);
void do_reboo (CHAR_DATA *, char *);
void do_reboot (CHAR_DATA *, char *);
void do_recall (CHAR_DATA *, char *);
void do_recho (CHAR_DATA *, char *);
void do_recite (CHAR_DATA *, char *);
void do_remove (CHAR_DATA *, char *);
void do_rent (CHAR_DATA *, char *);
void do_reply (CHAR_DATA *, char *);
void do_report (CHAR_DATA *, char *);
void do_rescue (CHAR_DATA *, char *);
void do_guard (CHAR_DATA *, char *);
void do_rest (CHAR_DATA *, char *);
void do_return (CHAR_DATA *, char *);
void do_rfind (CHAR_DATA *, char *);
void do_rstat (CHAR_DATA *, char *);
void do_run (CHAR_DATA *, char *);
void do_rank (CHAR_DATA *, char *); /* Kilith 05' */
void do_unguard (CHAR_DATA *, char *);
void do_sacrifice (CHAR_DATA *, char *);
void do_save (CHAR_DATA *, char *);
void do_say (CHAR_DATA *, char *);
void do_quickscore (CHAR_DATA *, char *);
void do_purse (CHAR_DATA *, char *);
void do_score (CHAR_DATA *, char *);
void do_info (CHAR_DATA *, char *);
void do_affect (CHAR_DATA *, char *);
void do_search (CHAR_DATA *, char *);
void do_sell (CHAR_DATA *, char *);
void do_sheath (CHAR_DATA *, char *);
void do_shout (CHAR_DATA *, char *);
void do_shutdow (CHAR_DATA *, char *);
void do_shutdown (CHAR_DATA *, char *);
void do_silence (CHAR_DATA *, char *);
void do_sit (CHAR_DATA *, char *);
void do_sla (CHAR_DATA *, char *);
void gotoxy (CHAR_DATA *, int, int);
void do_slay (CHAR_DATA *, char *);
void do_sleep (CHAR_DATA *, char *);
void do_slist (CHAR_DATA *, char *);
void do_slookup (CHAR_DATA *, char *);
void do_sneak (CHAR_DATA *, char *);
void do_snoop (CHAR_DATA *, char *);
void do_socials (CHAR_DATA *, char *);
void do_south (CHAR_DATA *, char *);
void do_speak (CHAR_DATA *, char *);
void do_armor (CHAR_DATA *, char *);
void do_split (CHAR_DATA *, char *);
void do_spells (CHAR_DATA *, char *);
void do_sset (CHAR_DATA *, char *);
void do_allsave (CHAR_DATA *, char *);
void do_stand (CHAR_DATA *, char *);
void do_steal (CHAR_DATA *, char *);
void do_string (CHAR_DATA *, char *);
void do_switch (CHAR_DATA *, char *);
void do_tell (CHAR_DATA *, char *);
void do_time (CHAR_DATA *, char *);
void do_title (CHAR_DATA *, char *);
void do_track (CHAR_DATA *, char *);
void do_track_t (CHAR_DATA *, char *);
void do_ptrack_t (CHAR_DATA *, char *); /* JRAJRA - New track */
void do_ptrack (CHAR_DATA *, char *); /* JRAJRA - new track command */
void do_trapset (CHAR_DATA *, char *); /* JRAJRA - Set up a trap */
void do_detrap (CHAR_DATA *, char *); /* remove traps from stuff */
void do_patch (CHAR_DATA *, char *); /* repair wpns and armor */
void do_train (CHAR_DATA *, char *);
void do_transfer (CHAR_DATA *, char *);
void do_trust (CHAR_DATA *, char *);
void do_unlock (CHAR_DATA *, char *);
void do_up (CHAR_DATA *, char *);
void do_users (CHAR_DATA *, char *);
void do_value (CHAR_DATA *, char *);
void do_visible (CHAR_DATA *, char *);
void do_wake (CHAR_DATA *, char *);
void do_wear (CHAR_DATA *, char *);
void do_headbutt (CHAR_DATA *, char *);
void do_bite (CHAR_DATA *, char *);
void do_warcry (CHAR_DATA *, char *);
void do_weather (CHAR_DATA *, char *);
void do_west (CHAR_DATA *, char *);
void do_where (CHAR_DATA *, char *);
void do_who (CHAR_DATA *, char *);
void do_wimpy (CHAR_DATA *, char *);
void do_wizhelp (CHAR_DATA *, char *);
void do_wizify (CHAR_DATA *, char *);
void do_wizlock (CHAR_DATA *, char *);
void do_wizlist (CHAR_DATA *, char *);
void do_wizprompt (CHAR_DATA *, char *);
void do_wp (CHAR_DATA *, char *);
void do_yell (CHAR_DATA *, char *);
void do_study (CHAR_DATA *, char *);
void do_zap (CHAR_DATA *, char *);
void do_spray (CHAR_DATA *, char *);
void do_donate ( CHAR_DATA *, char *);
void do_alliance (CHAR_DATA *, char *);
void do_towers (CHAR_DATA *, char *);
void do_bank (CHAR_DATA *, char *); /* JRAJRA */
void do_bankset (CHAR_DATA *, char*);
void show_donate (CHAR_DATA *);
void do_deposit (CHAR_DATA *, char *);
void do_withdraw (CHAR_DATA *, char *);
void do_balance (CHAR_DATA *, char *);
void do_quiet (CHAR_DATA *, char *);
void do_convert (CHAR_DATA *, char *);
void do_award_skill (CHAR_DATA *, char *);
void do_asave (CHAR_DATA *, char *);
void do_astat (CHAR_DATA *, char *);
void do_mindex (CHAR_DATA *, char *);
void do_oindex (CHAR_DATA *, char *);
void do_cindex (CHAR_DATA *, char *);
void do_aedit (CHAR_DATA *, char *);
void do_redit (CHAR_DATA *, char *);
void do_sedit (CHAR_DATA *, char *);
void do_oedit (CHAR_DATA *, char *);
void do_medit (CHAR_DATA *, char *);
void do_set_fighters (CHAR_DATA *, char *);
void do_cancel (CHAR_DATA *, char *);
void do_challenge (CHAR_DATA *, char *);
void do_acceptc (CHAR_DATA *, char *);
void do_decline (CHAR_DATA *, char *);
void do_bet (CHAR_DATA *, char *);
void do_forge (CHAR_DATA *, char *);
void do_weave (CHAR_DATA *, char *);
void do_enchant (CHAR_DATA *, char *);
void do_unenchant (CHAR_DATA *, char *);
void do_stitch (CHAR_DATA *, char *);
void do_mine (CHAR_DATA *, char *);
void do_actual_mine (CHAR_DATA *, char*);
void do_skin (CHAR_DATA *, char *);
void do_mine (CHAR_DATA *, char *);
void do_actual_mine (CHAR_DATA *, char*);
void do_skin (CHAR_DATA *, char *);
void do_tan (CHAR_DATA *, char *);
void do_brew (CHAR_DATA *, char *);
void do_lore (CHAR_DATA *, char *);
void do_arena (CHAR_DATA *, char *);
void do_ignore (CHAR_DATA *, char *);
void do_clan_quit (CHAR_DATA *, char *);
void do_attribute (CHAR_DATA *, char *);
void do_ggossip (CHAR_DATA *, char *);
void do_egossip (CHAR_DATA *, char *);
void do_weight (CHAR_DATA *, char *);
void do_alien (CHAR_DATA *, char *);
void do_choosesides(CHAR_DATA *, char *); /* JRAJRA */
void do_remort (CHAR_DATA *, char *); /* JRAJRA */
void do_forsake (CHAR_DATA *, char *);
void do_ascend (CHAR_DATA *, char *);
void info_remort (CHAR_DATA *, char *); /* JRAJRA */
void show_remort_requirements (CHAR_DATA *, char *); /* JRAJRA */
void do_nodelete (CHAR_DATA *, char *);
void do_minmana (CHAR_DATA *, char *);
void do_noaffect (CHAR_DATA *, char *);
void do_results (CHAR_DATA *, char *);
void do_afk (CHAR_DATA *, char *);
void do_busy (CHAR_DATA *, char *);
void do_ditch (CHAR_DATA *, char *);
void do_query (CHAR_DATA *, char *);
void do_pedit (CHAR_DATA *, char *);
void do_cedit (CHAR_DATA *, char *);
void mana (CHAR_DATA *, char *);
void do_tan (CHAR_DATA *, char *);
void do_brew (CHAR_DATA *, char *);
void do_lore (CHAR_DATA *, char *);
void do_arena (CHAR_DATA *, char *);
void do_ignore (CHAR_DATA *, char *);
void do_clan_quit (CHAR_DATA *, char *);
void do_attribute (CHAR_DATA *, char *);
void do_ggossip (CHAR_DATA *, char *);
void do_egossip (CHAR_DATA *, char *);
void do_weight (CHAR_DATA *, char *);
void do_alien (CHAR_DATA *, char *);
void do_choosesides(CHAR_DATA *, char *); /* JRAJRA */
void do_remort (CHAR_DATA *, char *); /* JRAJRA */
void info_remort (CHAR_DATA *, char *); /* JRAJRA */
void show_remort_requirements (CHAR_DATA *, char *); /* JRAJRA */
void do_nodelete (CHAR_DATA *, char *);
void do_minmana (CHAR_DATA *, char *);
void do_noaffect (CHAR_DATA *, char *);
void do_results (CHAR_DATA *, char *);
void do_afk (CHAR_DATA *, char *);
void do_busy (CHAR_DATA *, char *);
void do_ditch (CHAR_DATA *, char *);
void do_query (CHAR_DATA *, char *);
void do_pedit (CHAR_DATA *, char *);
void do_cedit (CHAR_DATA *, char *);
void mana (CHAR_DATA *, char *);
void do_startup_time (CHAR_DATA *, char *);
void alignswitch(CHAR_DATA *, int);
void raceswitch (CHAR_DATA *, int);
void do_players (CHAR_DATA *, char *);
void do_pdelete (CHAR_DATA *, char *);
void do_resets (CHAR_DATA *, char *);
void do_scriptr (CHAR_DATA *, char *);
void do_timers (CHAR_DATA *,char *);
void do_mark (CHAR_DATA *, char *);
void spell_locate_person (int, int, CHAR_DATA *, void *);
void spell_scry (int sn, int level, CHAR_DATA * ch, void *vo);
//void spell_raise_undead (int sn, int level, CHAR_DATA * ch, void *vo);
void spell_raise_undead (SPELL_DATA * sd, int level, CHAR_DATA * ch, void *vo);
void spell_life_tap (int sn, int level, CHAR_DATA * ch, void *vo);
void spell_control_weather (int sn, int level, CHAR_DATA * ch, void *vo);
void spell_identify (int sn, int level, CHAR_DATA * ch, void *vo);
void spell_locate_object (int sn, int level, CHAR_DATA * ch, void *vo);
void spell_summon (int sn, int level, CHAR_DATA * ch, void *vo);
void spell_acid_breath (int level, CHAR_DATA * ch, void *vo);
void spell_fire_breath (int level, CHAR_DATA * ch, void *vo);
void spell_ice_breath (int level, CHAR_DATA * ch, void *vo);
void spell_shard_breath (int level, CHAR_DATA * ch, void *vo);
void spell_wind_breath (int level, CHAR_DATA * ch, void *vo);
void spell_shock_breath (int level, CHAR_DATA * ch, void *vo);
void spell_message (int sn, int level, CHAR_DATA * ch, void *vo);
void set_off_trap (CHAR_DATA *, ROOM_DATA *, bool);
char *star_a (int, int);
char *star_b (int, int);
void do_disable(CHAR_DATA *,char *);

/* I included many standard prototypes here, although not all are implimented
   as of now... */

bool spec_breath_any (CHAR_DATA * ch);
bool spec_breath_acid (CHAR_DATA * ch);
bool spec_breath_fire (CHAR_DATA * ch);
bool spec_breath_frost (CHAR_DATA * ch);
bool spec_breath_gas (CHAR_DATA * ch);
bool spec_breath_lightning (CHAR_DATA * ch);
bool spec_eatcorpse (CHAR_DATA * ch);
bool spec_guard (CHAR_DATA * ch);
bool spec_janitor (CHAR_DATA * ch);
bool spec_poison (CHAR_DATA * ch);
bool spec_thief (CHAR_DATA * ch);
bool spec_warrior (CHAR_DATA * ch);


char *crypt (const char *key, const char *salt);

/* Thanks to Cym of delta.misha.net for helping me make the mud SYSV/SUN
   compatible */

#if     defined(sun)
int fclose (FILE * stream);
int fprintf (FILE * stream, const char *format,...);
#if     defined(SYSV)
size_t fread (void *ptr, size_t size, size_t n, FILE * stream);
#else
int fread (void *ptr, int size, int n, FILE * stream);
#endif
int fseek (FILE * stream, long offset, int ptrname);
void perror (const char *s);
int ungetc (int c, FILE * stream);
#endif

#if     defined(NOCRYPT)
#define crypt(s1, s2)   (s1)
#endif


void add_follower (CHAR_DATA *, CHAR_DATA *);
bool is_note_to (CHAR_DATA *, NOTE_DATA *);
void set_title (CHAR_DATA *, char *);
bool move_char (CHAR_DATA * ch, int door);
void make_blood (CHAR_DATA * ch, int door);
CHAR_DATA *find_keeper (CHAR_DATA * ch);
CHAR_DATA *find_repair (CHAR_DATA * ch, int rnum);
int get_cost (CHAR_DATA *, SINGLE_OBJECT *, bool);
void wear_obj (CHAR_DATA *, SINGLE_OBJECT *, bool, int);
ROOM_DATA *find_location (CHAR_DATA *, char *);
void die_follower (CHAR_DATA *);
char *get_ansi_spaces (char *);
int find_first_step (ROOM_DATA *, ROOM_DATA *);
void do_chan_notify (CHAR_DATA *, char *);
void do_restore (CHAR_DATA *, char *);
void stop_follower (CHAR_DATA *);
bool is_same_group (CHAR_DATA *, CHAR_DATA *);
void do_exits (CHAR_DATA *, char *);
void do_land (CHAR_DATA *, char *);
void do_look (CHAR_DATA *, char *);
void close_socket (DESCRIPTOR_DATA * dclose);
SHELL *new_shell (void);
void write_to_buffer (DESCRIPTOR_DATA *, const char *,
		      int);
void do_global (char *, int, int);
void send_to_char (const char *, CHAR_DATA * ch);
void page_to_char (char *, CHAR_DATA * ch);
char* add_color(char *);
char* add_color_limited (char*);
void ansi_color (const char *, CHAR_DATA * ch);
void show_string (DESCRIPTOR_DATA *, char *);
void act (const char *, CHAR_DATA *, const void *, const void *, int);
void act_dam (const char *, CHAR_DATA *, SINGLE_OBJECT *, CHAR_DATA *, int);
char *anseval (const char *, CHAR_DATA *);
void boot_db (void);
void area_update (void);
void room_update (void);
void reset_room (ROOM_DATA *);
CHAR_DATA *create_mobile (MOB_PROTOTYPE *);
SINGLE_OBJECT *create_object (OBJ_PROTOTYPE *, int);
void clear_data (void *thing);
REALCHAR_DATA *new_pc (void);
CHAR_DATA *new_char (void);
SINGLE_OBJECT *new_obj (void);
char *get_extra_descr (const char *, DESCRIPTION_DATA *);
int clan_number (CHAR_DATA *);
int clan_number_2 (CHAR_DATA *);
bool scan_clan (char *arg);
bool scan_clan_2 (char *arg);
CLAN_DATA *get_clan_index (int);
CLAN_DATA *get_clan_index_2 (int);
MOB_PROTOTYPE *get_mob_index (int);
OBJ_PROTOTYPE *get_obj_index (int);
ROOM_DATA *get_room_index (int);
SPELL_DATA *spell_lookup (char *txt, int sn);
void save_area (AREA_DATA *);
void save_boats (void);
void load_boats (void);
bool check_prereq_depth (SPELL_DATA *, SPELL_DATA*);
int lookup_gsn (char *nm);
void setup_skills (void);
int tally_spsk (CHAR_DATA *, int);
void general_spell (SPELL_DATA *, int, CHAR_DATA *, void *);
void general_spell2 (SPELL_DATA *, int, CHAR_DATA *, void *, bool,ROOM_DATA *);
void load_spells (void);
void do_new_prereq (CHAR_DATA *, char *);
void sedit (CHAR_DATA * ch, char *argy);
void write_spells (void);
void read_damages (void);
void write_damages (void);
void read_config (void);
void write_config (void);
SPELL_DATA *new_spell (void);
long translate (char *line, int level, CHAR_DATA * ch);


/* These functions work similarly to the old Mortal Conquest ones.  they
   may not behave exactly the same... */

char fread_letter (FILE *);
long long fread_number (FILE *);
char *fread_string2 (FILE *, CHAR_DATA *);
char *fread_string (FILE *, CHAR_DATA *);
char *fread_string_eol (FILE *);
void fread_to_eol (FILE *);
char *fread_word (FILE *);
void *alloc_mem (int);
void *alloc_perm (int);
void free_mem (void *, int);
char *str_dupn (const char *);
char *str_dup (const char *);
void free_string (char *);
int number_fuzzy (int);
int number_range (int, int);
int number_percent (void);
int number_door (void);
int number_bits (int);
int number_mm (void);
int dice (int, int);
void smash_tilde (char *);
bool str_cmp (const char *, const char *);
bool str_prefix (const char *, const char *);
bool str_infix (const char *, const char *);
bool str_suffix (const char *, const char *);
char *capitalize (const char *);
void append_file (CHAR_DATA *, char *, char *);
void bug (const char *, int);
void log_string (const char *);
bool is_safe (CHAR_DATA *, CHAR_DATA *);
void death_cry (CHAR_DATA * ch, bool flaggy);
void violence_update (void);
void multi_hit (CHAR_DATA *, CHAR_DATA *, int);
void ground_hit (CHAR_DATA *, CHAR_DATA *, int);
void damage (CHAR_DATA *, CHAR_DATA *, int, int);
ROOM_DATA *new_room (void);
void update_pos (CHAR_DATA *);
void update_prereq (void);
void check_prereq (SPELL_DATA *);
void stop_fighting (CHAR_DATA *, bool);
void raw_kill (CHAR_DATA *, bool);
void skill_gain (CHAR_DATA *, short, bool);
void do_mindexc (CHAR_DATA * ch, char *argy, bool windowed);
int get_age (CHAR_DATA * ch);
int get_stat(CHAR_DATA *, int);
int get_curr_str (CHAR_DATA *ch);
int get_curr_int (CHAR_DATA *ch);
int get_curr_wis (CHAR_DATA *ch);
int get_curr_dex (CHAR_DATA *ch);
int get_curr_con (CHAR_DATA *ch);

bool is_name (const char *, char *);
void affect_to_char (CHAR_DATA *, AFFECT_DATA *);
void affect_remove (CHAR_DATA *, AFFECT_DATA *);
void affect_strip (CHAR_DATA *, int);
void affect_strip_bits (CHAR_DATA *, int);
void affect_strip_bits2 (CHAR_DATA *, int);
bool is_affected (CHAR_DATA *, int);
bool is_affectedt (CHAR_DATA *, int);
void affect_join (CHAR_DATA *, AFFECT_DATA *);
void char_from_boat (CHAR_DATA *, SINGLE_OBJECT *);
void char_to_boat (CHAR_DATA *, SINGLE_OBJECT *);
bool has_boat_space (CHAR_DATA *, SINGLE_OBJECT *);
bool is_in_same_boat (CHAR_DATA *, CHAR_DATA *);
void char_from_room (CHAR_DATA *);
void char_to_room (CHAR_DATA *, ROOM_DATA *);
void u_char_to_room (CHAR_DATA *, ROOM_DATA *);
void obj_to (SINGLE_OBJECT *, void *);
void obj_from (SINGLE_OBJECT *);
void free_it (void *);
int apply_ac (SINGLE_OBJECT *, int);
SINGLE_OBJECT *get_eq_char (CHAR_DATA *, int);
void equip_char (CHAR_DATA *, SINGLE_OBJECT *, int);
SINGLE_OBJECT *get_item_held (CHAR_DATA *, int);

void NEW_POSITION(CHAR_DATA *, int);
void SUBHIT(CHAR_DATA *, int);
void ADDHIT(CHAR_DATA *, int);
void ADDMOVE(CHAR_DATA *, int);
void SUBMOVE(CHAR_DATA *, int);
void MAXHIT(CHAR_DATA *);
void MAXMOVE(CHAR_DATA *);
void check_fgt(CHAR_DATA *);

int hand_empty (CHAR_DATA *);
int belt_empty (CHAR_DATA *);
bool unequip_char (CHAR_DATA *, SINGLE_OBJECT *);
int count_obj_list (OBJ_PROTOTYPE *, SINGLE_OBJECT *);
void extract_char (CHAR_DATA *, bool);
CHAR_DATA *get_char_room (CHAR_DATA *, char *);
CHAR_DATA *get_char_room_near (CHAR_DATA *, ROOM_DATA *, int, char *);
CHAR_DATA *get_char_room_near2 (CHAR_DATA *, ROOM_DATA *, int, char *, int);

CHAR_DATA *get_char_room2 (CHAR_DATA *, ROOM_DATA *, char *);
CHAR_DATA *get_char_world (CHAR_DATA *, char *);
CHAR_DATA *get_char_world_2 (CHAR_DATA *, char *);
SINGLE_OBJECT *get_obj_type (OBJ_PROTOTYPE *);
SINGLE_OBJECT *get_obj_list (CHAR_DATA *, char *,
			     SINGLE_OBJECT *);
SINGLE_OBJECT *get_obj_carry (CHAR_DATA *, char *);
SINGLE_OBJECT *get_obj_wear (CHAR_DATA *, char *);
SINGLE_OBJECT *get_obj_inv (CHAR_DATA *, char *argy);
SINGLE_OBJECT *get_obj_held (CHAR_DATA *, char *argy);
SINGLE_OBJECT *get_obj_here (CHAR_DATA *, char *argy, int);
SINGLE_OBJECT *get_obj_world (CHAR_DATA *, char *argy);
void create_amount (int, CHAR_DATA *,
		    ROOM_DATA *, SINGLE_OBJECT *);
int tally_coins (CHAR_DATA *);
char *name_amount (int);
char *sub_coins (int, CHAR_DATA *);
void fread_startup_time (void);
int get_obj_number (SINGLE_OBJECT * obj);
int get_obj_weight (SINGLE_OBJECT * obj);
int room_is_dark (ROOM_DATA *);
bool room_is_private (ROOM_DATA *);
bool can_see (CHAR_DATA *, CHAR_DATA *);
bool can_see_nextdoor (CHAR_DATA *, CHAR_DATA *);
bool can_see_obj (CHAR_DATA *, SINGLE_OBJECT *);
bool can_drop_obj (CHAR_DATA *, SINGLE_OBJECT *);
char *mob_type_name (int);
char *mob_type_attack (int);
char *item_type_name (int);
char *affect_loc_name (int);
char *affect_bit_name (int);
char *augment_bit_name (int);
char *affect_bit_name_two (int);
void do_actual_investigate (CHAR_DATA *ch, char * argy);
void do_actual_circle (CHAR_DATA * ch, char *argy, bool);
void do_actual_backstab (CHAR_DATA * ch, char *argy);
void do_actual_search (CHAR_DATA * ch, char *argy);
void do_actual_cast (CHAR_DATA * ch, char *argy);
void do_actual_shoot (CHAR_DATA * ch, char *argy);
bool find_vehicle (CHAR_DATA *, int);
void set_fighting (CHAR_DATA *, CHAR_DATA *);
void clan_check_align (CHAR_DATA *);
void clan_check_align_2 (CHAR_DATA *);
int max_mana (CHAR_DATA *);
int mana_now (CHAR_DATA *);
void natural_mana (CHAR_DATA *);
char *compr (char *txt);
char *translate_output (char *inpt);
void clan_notify (char *argy, int clan_num);
void clan_notify_2 (char *argy, int clan_num);
void group_notify (char *argy, CHAR_DATA * ch);
void save_clans (void);
void save_clans_2 (void);
void dummy_mid (void);
void dummy_npc (void);
void dummy_pc (void);
void save_playerbase (void);
void save_itemcheck (void);
void update_pbase (CHAR_DATA * ch);
int itemcheck_forchar (CHAR_DATA * ch);
void update_itemcheck (CHAR_DATA * ch);
void save_topten (void);
void save_day_counter (void);
void save_rating (void);
void save_pkdata (void);
void save_banlist (void);
void update_weight_num (CHAR_DATA * ch);
void dummycall (void);
void check_add_trophy (CHAR_DATA * ch, CHAR_DATA * victim, int, int);
int free_trophy_slot (CHAR_DATA * ch, CHAR_DATA * victim);
void sort_trophy (CHAR_DATA *ch, char*);
int free_storage (CHAR_DATA * ch);
int free_clan_storage (CLAN_DATA * clan);
void load_corpse (CHAR_DATA * read_ch);
void save_corpse (CHAR_DATA * ch);
void check_topten (CHAR_DATA * ch);
void check_rating (CHAR_DATA * ch);
void check_pkdata (CHAR_DATA * ch);
void raw_purge (ROOM_DATA * rid);
void bust_a_prompt (CHAR_DATA * ch);
void fwrite_clan_storage (int clan_num);
void fread_clan_storage (int clan_num);
void fwrite_align_bank (int align); /* JRAJRA */
void fread_align_bank (int align);/* JRAJRA */
void fread_clan_bank (int clan_num);/* JRAJRA */
void fread_alliance_data (void);
void fwrite_alliance_data (void);

void fwrite_clan_bank (int clan_num);/* JRAJRA */
void get_condition (SINGLE_OBJECT * obj);
char *strip_ansi_codes (char *txty);
void free_arena (void);
bool write_to_descriptor2 (DESCRIPTOR_DATA *, char *, int);
int get_bet_slot (char *charname);
bool check_skill(CHAR_DATA *, int);
char *bet_on (char *charname);
void end_arena (CHAR_DATA * winner, CHAR_DATA * loser);
void arena (void);
bool ignore (CHAR_DATA * blabber_char, CHAR_DATA * sent_tot);
int chars_in_group (CHAR_DATA * ch);
int rchars_in_group (CHAR_DATA * ch);
int tchars_in_group (CHAR_DATA * ch);
void bansite (char *argy);
void nbansite (char *argy);
void do_arenaecho (CHAR_DATA * ch, char *argy, bool batspam);
void give_back_money (void);
void blank_prompt (CHAR_DATA * ch);

/* These should be compatable with theisles.  I can't guarentee they are
   implimented the same, but the passed parameters should be compat.. no
   guarentees!! */

char *format_obj_to_char (SINGLE_OBJECT *, CHAR_DATA *,
			  bool);
void show_list_to_char (SINGLE_OBJECT *, CHAR_DATA *,
			bool, bool);
void show_char_to_char_1 (CHAR_DATA *, CHAR_DATA *);
void show_char_to_char (CHAR_DATA *, CHAR_DATA *);
void show_char_to_char_0 (CHAR_DATA *, CHAR_DATA *);
void show_equipment (CHAR_DATA *, CHAR_DATA *);
bool check_blind (CHAR_DATA *);
int words (char *s);
char *ans_uppercase (const char *);
bool remove_obj (CHAR_DATA * ch, int, bool);

void syslag (CHAR_DATA * ch, char *argy);

bool check_defense (CHAR_DATA *, CHAR_DATA *);


void dam_message (CHAR_DATA *, CHAR_DATA *, int,
		  int, int);
bool can_groundfight (CHAR_DATA * ch);
void death_cry (CHAR_DATA * ch, bool flaggy);
void group_gain (CHAR_DATA * ch, CHAR_DATA * victim);
int xp_compute (CHAR_DATA * gch, CHAR_DATA * victim);
bool is_safe (CHAR_DATA * ch, CHAR_DATA * victim);
void make_corpse (CHAR_DATA * ch);
void *mem_alloc (int);
void one_hit (CHAR_DATA *, CHAR_DATA *, int, SINGLE_OBJECT *);
void raw_kill (CHAR_DATA * victim, bool used_sword);
void set_fighting (CHAR_DATA * ch, CHAR_DATA * victim);
void disarm (CHAR_DATA * ch, CHAR_DATA * victim);
void trip (CHAR_DATA * ch, CHAR_DATA * victim);
void display_toggles (CHAR_DATA * ch);
PLAYERBASE_DATA *new_player (void);
MOB_PROTOTYPE *new_mob_index (void);
DOOR_DATA *new_door (void);
CED_DATA *new_ced (void);
TEMP_STR *new_temp (void);
void free_temp (CHAR_DATA * ch);
void check_temp (CHAR_DATA * ch);
void free_ced (CHAR_DATA * ch);
void check_ced (CHAR_DATA * ch);
EXIT_DATA *new_exit (void);
ROOM_DATA *new_room_index (void);
AREA_DATA *get_vnum_area (int vnum);
int strtoint (char *string);
bool is_same_align (CHAR_DATA * ch, CHAR_DATA * victim);
bool is_cheating (CHAR_DATA * ch);
void hunt_victim (CHAR_DATA * ch);
CHAR_DATA *get_player_world (CHAR_DATA * ch, char *argy, bool need_see);
void do_auction (CHAR_DATA * ch, char *argy, int number);
CLAN_DATA *new_clan_index (void);
CLAN_DATA *new_clan_index_2 (void);
int get_sector_number (char *argy);
void free_exit (EXIT_DATA *);
int get_exit_flag_number (char *arg);

/* Write to descriptor... Do NOT use this for output.  I leave this in only
   so that it's compat with a diku comm package... ALWAYS use write_to_
   descriptor2() ... never use just plain ole write_to_descriptor! */

bool write_to_descriptor (int, char *, int);
/*int     read             ( int, char *, int ); */

void free_affect (AFFECT_DATA *);
AFFECT_DATA *new_affect (void);
OBJ_PROTOTYPE *new_obj_index (void);
DESCRIPTION_DATA *new_extra_descr (void);
void free_extra_descr (DESCRIPTION_DATA *);
OPTIONAL_DATA *new_optional (void);
void new_shop (MOB_PROTOTYPE *);
void free_shop (SHOP_DATA *);
int get_mob_sex_number (char *);
SPEC_FUNCTION *get_mob_spec (char *arg);
int arg_count (char *);
int get_area_flags_number (char *argy);
AREA_DATA *new_area (void);
char *string_replace (char *orig, char *old, char *new);
char *string_unpad (char *argy);
char *string_proper (char *argy);
void mindex_window (CHAR_DATA * ch, MOB_PROTOTYPE * victim, int update);
int add_damroll (CHAR_DATA * ch);
int add_hitroll (CHAR_DATA * ch);
int GET_HITROLL (CHAR_DATA * ch);
int GET_DAMROLL (CHAR_DATA * ch);
void conv_height (CHAR_DATA * ch);
char *jumble_name (char *st, bool unjumble);
void jumble (char *name);
void unjumble (char *name, bool convert);
char *string_mana (int mana);
char *string_mana_p (int mana);
void fix_char (CHAR_DATA * ch);

bool check_body_temp (CHAR_DATA * ch);
void do_fatal (CHAR_DATA * victim, CHAR_DATA * ch, int dt);
bool IS_CORPSE_MOB (CHAR_DATA * ch);
char *rNAME (CHAR_DATA * to, CHAR_DATA * ch);
void find_people_n (CHAR_DATA * ch, int n);
char *wordwrap (char *txt, int);
int lookup_questname (char *txt);
ROOM_DATA *find_room_tag (char *tag);
char *show_room_name (CHAR_DATA * ch, char *roomname);
void place_pets (CHAR_DATA * ch);
void clear_tracks (CHAR_DATA * ch);
CHAR_DATA *find_mob (void);
CHAR_DATA *findmob (void);
void createobject (CHAR_DATA * ch, int vnum);
void destroy (CHAR_DATA * mob, int i);
void do_destroy (CHAR_DATA * mob, int i);
void hardcode_give_scripts (CHAR_DATA *, CHAR_DATA *, char *);
void hardcode_move_scripts (CHAR_DATA *, CHAR_DATA *);
void hardcode_room_enter (CHAR_DATA *, ROOM_DATA *);
void hardcode_interpret_scripts (CHAR_DATA *, CHAR_DATA *, char *);
void hardcode_saytell_scripts (CHAR_DATA *, CHAR_DATA *, char *);
/*char *itoa(int); */
void allocate_correct_datatype (SINGLE_OBJECT *, int *);
void free_m (void *);
void print_reasons (CHAR_DATA * ch, SINGLE_OBJECT * container);
char *interpret_line (char *, CHAR_DATA *);
char *natural_abilities (int);
char *natural_abilities2 (int);
char *augmentss (int);
void show_remort (CHAR_DATA *);
void show_donate (CHAR_DATA *);
void set_initial_hp (CHAR_DATA *);
void remove_from_aggro_list (CHAR_DATA *);
void add_to_fighting_list (CHAR_DATA *);
void add_to_aggro_list (CHAR_DATA *);
void set_default_prompt (CHAR_DATA *, char);
void read_power_data (void);
void read_race_info (void);
void read_align_info (void);
void read_channel_data (void);
void read_score (CHAR_DATA *, char *);
bool check_pr (char *);
void read_power_dat (void);

void read_command_levels (void);
void add_level_bonuses (CHAR_DATA *, int);
int new_damage (CHAR_DATA *, int, int, int);
void channel_function (CHAR_DATA *, char *, CHANNEL *, int, char *);
void fcheck_pbase (char *);
void reset_small (void);
void area_sort (AREA_DATA *);
void save_area_list (void);
void undisplay(CHAR_DATA *, ROOM_DATA *, int);
void drawstuff(CHAR_DATA *, char *, int, int);
char *parse_txt (char *, CHAR_DATA *);
char *how_good (CHAR_DATA *, int);
char *format_obj_to (SINGLE_OBJECT *, CHAR_DATA *, bool);
void check_clear_more (ROOM_DATA * room);
void check_room_more (ROOM_DATA * room);
void display_room (CHAR_DATA * ch, ROOM_DATA * rid, int x, int y);
void strip_nonworn (CHAR_DATA * ch);
void pc_death_penalty (CHAR_DATA * ch, CHAR_DATA * victim);
int rating (CHAR_DATA * ch);
int free_bet_slot (void);
void make_corpse (CHAR_DATA * ch);
void racehate_message (CHAR_DATA * ch);
bool can_yell (CHAR_DATA * ch);
bool check_clan (CLAN_DATA * clan, char *argy);
bool check_clan_2 (CLAN_DATA * clan, char *argy);
int clan_free (CLAN_DATA * clan);
int clan_free_2 (CLAN_DATA * clan);
bool is_member (CHAR_DATA * ch, int guildflag);
bool locate_person (CHAR_DATA * ch, int level, char *argy);
bool scry (CHAR_DATA * ch, int level, char *argy);
int get_race_look (char *argy);
bool pkill (CHAR_DATA * ch, CHAR_DATA * victim);
void check_pkill (CHAR_DATA * ch, CHAR_DATA * victim);
void get_obj (CHAR_DATA * ch, SINGLE_OBJECT * obj, SINGLE_OBJECT * container, bool opp_align);
int addoffense (CHAR_DATA * ch, SINGLE_OBJECT * obj);
int addoffense2 (CHAR_DATA *ch, SINGLE_OBJECT *obj);
char *extra_bit_name (long long extra_flags);
char *room_bit_name (int room_flags);
char *act_bit_name (int act);
char *room_bit_name2 (int room_flags);
char *act_bit_name2 (int act);

char *act3_bit_name (int act);
char *act4_bit_name (int act);
char *race_bit_name (int);
char *plr_bit_name (int);
char *wear_bit_name (int);
char *wear_loc_name (int);

int mob_name_type (char *name);
int item_name_type (char *name);
int affect_name_loc (char *name);
int affect_name_bit (char *buf);
int affect_name_bit_two (char *buf);
int augment_name_bit (char *buf);
long long extra_name_bit (char *buf);
int item_guild_bit (char *buf);
int room_name_bit (char *buf);
int room_name_bit2 (char *buf);
int act_name_bit (char *buf);
int race_name_bit (char *buf);
int wear_name_bit (char *buf);
int wear_name_loc (char *buf);


char *is_are (char *);
char *format_string (char *);
char *replace_string (char *orig, char *old, char *new);
char *get_extra_descr (const char *name, DESCRIPTION_DATA * ed);
bool use_tool (SINGLE_OBJECT * obj, int bit);
int PERCENTAGE (int amount, int max);

void interpret (CHAR_DATA * ch, char *argy);

void super_interpret (CHAR_DATA *, char *);
bool is_number (char *arg);
int num_args (char *argy);
int number_argy (char *argy, char *arg);
char *one_argy (char *argy, char *arg_first);
char *one_argcase (char *argy, char *);
bool check_social (CHAR_DATA *, char *, char *);
SPELL_DATA *skill_lookup (char *, int);
int slot_lookup (int);
bool saves_spell (int, CHAR_DATA *);
void obj_cast_spell (int, int, CHAR_DATA *,
		     CHAR_DATA *, SINGLE_OBJECT *);
void aedit (CHAR_DATA * ch, char *argy);
void redit (CHAR_DATA * ch, char *argy);
void renew_affect (CHAR_DATA * ch, AFFECT_DATA * aff);
void oedit (CHAR_DATA * ch, char *argy);
void medit (CHAR_DATA * ch, char *argy);
void pedit (CHAR_DATA * ch, char *argy);
void cedit (CHAR_DATA * ch, char *argy);
void string_append (CHAR_DATA * ch, char **pString);
void fullscreen_editor (CHAR_DATA * ch, char **pString);
AREA_DATA *get_area_data (int vnum);
char *fix_string (const char *str);
void string_add (CHAR_DATA * ch, char *argy);
void character_add (CHAR_DATA * ch, char *to_add);
void save_char_obj (CHAR_DATA * ch);
bool load_char_obj (DESCRIPTOR_DATA * d, char *name, bool ped);
int cur_temp (CHAR_DATA * ch);
char *first_arg (char *, char *, bool);
SPEC_FUNCTION *spec_lookup (const char *);
void advance_level (CHAR_DATA *);
void gain_exp (CHAR_DATA *, int);
void gain_condition (CHAR_DATA *, int, int value);
void update_handler (void);
int get_direction_number (char *arg);
void load_helps (FILE * fp);
int system (const char *);
bool find_str (char *, char *);
bool process_output (DESCRIPTOR_DATA *, bool);
void strrepl (char *s, char new, char old);
char *first (char *s);
bool strsub (char *s, char *old, char *new);
char *ans_uppercase (const char *string);
bool aliasok (char *aliasname, char *s);
void add_to_list (char *, DO_COMMAND, int, int, int, char *);
void scanthru_commands (void);
char *decompress (char *s);
char *compress (char *s, long len);


// ARC: new functions as of June 2002

// for using 'an' vs 'a' in the right places
bool is_first_letter_vowel ( char* sample );



extern const int factions[MAX_MOB_TYPE][MAX_MOB_TYPE];
extern const struct material_data materials[];
extern const struct forged_items forged[];
extern const struct material_data skin_types[];
extern const struct material_data cloth_types[];
extern const struct str_apply_type str_app[37];
extern const struct weapon_damage wdt[];
extern struct int_apply_type int_app[37];
extern struct wis_apply_type wis_app[37];
extern struct con_apply_type con_app[37];
extern const struct color_data color_table[];
extern struct race_data race_info[RACE_COUNT];
extern struct race_data align_info[NUM_ALIGN];
extern const struct flag_data affect_flags_2[];
extern const struct potion_ingredients pot_info[];
extern const struct mna_data mana_data[];
extern const struct flag_data affect_flagss[];
extern const struct gld_data guild_data[];
extern const struct flag_data room_flagss[];
extern const struct flag_data room_flags2[];
extern const struct flag_data act_1[];
extern const struct flag_data act_3[];
extern const struct flag_data act_4[];
extern const struct flag_data extra_flagss[];
extern const struct flag_data item_guild_flags[];
extern const struct flag_data elemental_alignment_flags[];
extern const struct flag_data augment_flagss[];
extern const struct sector_data sectors[];
extern const struct flag_data spell_flagss[];
extern const struct flag_data spell_flags2[];
extern char *const percent_hit[];
extern char *const percent_tired[];



extern COMMAND *command_hash[256];
extern int commands;




#include "script.h"

#define FIXIT(ch)   if ((ch)->in_room&&(IS_SET((ch)->in_room->room_flags,ROOM_INDOORS) ||(ch)->in_room->sector_type >= SECT_CAVE)  && IS_SET((ch)->affected_by,AFF_FLYING)&& (ch)->height >INDOOR_FLY_HEIGHT)  (ch)->affected_by^=AFF_FLYING;

#define PLAYERSIZE1 "A"             // non-morted players
//#define PLAYERSIZE2 "A lesser"        // 1-3mort players
//#define PLAYERSIZE3 "A greater"        // 4-6mort players
//#define PLAYERSIZE4 "An elite"       // 7-9mort players
//#define PLAYERSIZE4F "An elite"    // 7-9mort female players
//#define PLAYERSIZE5 "A Champion"         // 10mort players
//#define PLAYERSIZE5F "A Champion"       // 10mort female players

