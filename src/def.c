#include <sys/types.h> 
#include <stdio.h> 
#include <time.h>
#include "emlen.h"


const char position_name[MAX_POSITIONS][20] = 
{
   {"dead"},
   {"mortally wounded"},
   {"incapacitated"},
   {"stunned"},
   {"sleeping"},
   {"resting"},
   {"bashed"},
   {"groundfighting"},
   {"fighting"},
   {"circling"},
   {"falling"},
   {"meditating"},
   {"standing"},
   {"running"},
   {"searching"},
   {"backstabbing"},
   {"investigating"},
   {"casting"},
   {"shooting"},
   {"mining"},
 };

char *const color_name[] =
{
  "OOC",
  "CLANTALK",
  "YELL",
  "SAY",
  "TELL",
  "UNUSED",
  "IMMTALK",
  "GODTALK",
  "NONE",
  "NONE",
  "NONE",
  "NONE",
  "NONE",
  "NONE",
  "NONE"
};


const struct str_apply_type str_app[37] =
{
  {-4},
  {-4},
  {-2},
  {-1},
  {-1},
  {-1},
  {0},
  {0},
  {0},
  {0},
  {0},
  {0},
  {0},
  {0},
  {0},
  {1},
  {1},
  {1},
  {2},
  {2},
  {2},
  {3},
  {3},
  {4},
  {4},
  {5},
  {5},
  {5},
  {6},
  {6},
  {7},
  {7},
  {8},
  {8},
  {9},
  {10},
  {11}
};

char *const where_name[] =
{
  "\x1b[0;37m<\x1b[1;30mfloating nearby\x1b[0;37m>   ",
  "\x1b[0;37m<\x1b[1;30mfloating nearby\x1b[0;37m>   ",
  "\x1b[0;37m<\x1b[1;30mworn on head\x1b[0;37m>      ",
  "\x1b[0;37m<\x1b[1;30mworn on eyes\x1b[0;37m>      ",
  "\x1b[0;37m<\x1b[1;30mworn in ear\x1b[0;37m>       ",
  "\x1b[0;37m<\x1b[1;30mworn in ear\x1b[0;37m>       ",
  "\x1b[0;37m<\x1b[1;30mworn on face\x1b[0;37m>      ",
  "\x1b[0;37m<\x1b[1;30mworn around neck\x1b[0;37m>  ",
  "\x1b[0;37m<\x1b[1;30mworn around neck\x1b[0;37m>  ",
  "\x1b[0;37m<\x1b[1;30mworn on body\x1b[0;37m>      ",
  "\x1b[0;37m<\x1b[1;30mworn about body\x1b[0;37m>   ",
  "\x1b[0;37m<\x1b[1;30mworn on back\x1b[0;37m>      ",
  "\x1b[0;37m<\x1b[1;30mworn on shoulder\x1b[0;37m>  ",
  "\x1b[0;37m<\x1b[1;30mworn on shoulder\x1b[0;37m>  ",
  "\x1b[0;37m<\x1b[1;30mstrapped to arm\x1b[0;37m>   ",
  "\x1b[0;37m<\x1b[1;30mworn on arms\x1b[0;37m>      ",
  "\x1b[0;37m<\x1b[1;30mtattoo\x1b[0;37m>            ",
  "\x1b[0;37m<\x1b[1;30mworn on elbow\x1b[0;37m>     ",
  "\x1b[0;37m<\x1b[1;30mworn on elbow\x1b[0;37m>     ",
  "\x1b[0;37m<\x1b[1;30mworn on wrist\x1b[0;37m>     ",
  "\x1b[0;37m<\x1b[1;30mworn on wrist\x1b[0;37m>     ",
  "\x1b[0;37m<\x1b[1;30mworn on hands\x1b[0;37m>     ",
  "\x1b[0;37m<\x1b[1;30mworn on finger\x1b[0;37m>    ",
  "\x1b[0;37m<\x1b[1;30mworn on finger>\x1b[0;37m    ",
  "\x1b[0;37m<\x1b[1;30mprimary hand\x1b[0;37m>      ",
  "\x1b[0;37m<\x1b[1;30msecondary hand\x1b[0;37m>    ",
  "\x1b[0;37m<\x1b[1;30mthrust thru belt\x1b[0;37m>  ",
  "\x1b[0;37m<\x1b[0;37m\x1b[1;30mthrust thru belt\x1b[0;37m>  ",
  "\x1b[0;37m<\x1b[1;30mthrust thru belt\x1b[0;37m>  ",
  "\x1b[0;37m<\x1b[1;30mthrust thru belt\x1b[0;37m>  ",
  "\x1b[0;37m<\x1b[1;30mthrust thru belt\x1b[0;37m>  ",
  "\x1b[0;37m<\x1b[1;30mworn about waist\x1b[0;37m>  ",
  "\x1b[0;37m<\x1b[1;30mworn on legs\x1b[0;37m>      ",
  "\x1b[0;37m<\x1b[1;30mworn on knee\x1b[0;37m>      ",
  "\x1b[0;37m<\x1b[1;30mworn on knee\x1b[0;37m>      ",
  "\x1b[0;37m<\x1b[1;30mworn on ankle\x1b[0;37m>     ",
  "\x1b[0;37m<\x1b[1;30mworn on ankle\x1b[0;37m>     ",
  "\x1b[0;37m<\x1b[1;30mworn on feet\x1b[0;37m>      "
};


char *const dist_name[8] =
{
  "pretty close by",
  "fairly far off",
  "a long ways off",
  "fairly far off",
  "fairly far off",
  "fairly far off",
  "fairly far off",
  "fairly far off"
};


char *const mob_type[MAX_MOB_TYPE] =
{
  "human",
  "elf",
  "dwarf",
  "gnome",
  "giant",
  "dragon",
  "canine",
  "rodent",
  "insect",
  "undead",
  "golem",
  "demon",
  "troll",
  "bird",
  "fish",
  "statue",
  "feline",
  "plant",
  "general_animal",
  "faerie",
  "reptile",
  "ghost",
  "other",
  "ogre",
  "elemental",
  "orc",
  "dummy",
  "arachnid",
  "gnoll",
  "bugbear", 
  "skaven", 
  "troglodyte",
  "goblin",
  "illithid",
  "kender", 
  "minotaur", 
  "sprite",
  "draconian",
  "hobgoblin",
  "hobbit", 
  "centaur", 
  "mutant",
  "horse",
};
char *const mob_attack[MAX_MOB_TYPE] =
{
  "punch",
  "punch",
  "punch",
  "punch",
  "punch",
  "claw",
  "bite",
  "bite",
  "sting",
  "touch",
  "crush",
  "claw",
  "claw",
  "peck",
  "bite",
  "pound",
  "scratch",
  "scratch",
  "bite",
  "touch",
  "bite",
  "grasp",
  "scratch",
  "pound",
  "drain",
  "punch",
  "nothing",
  "bite",
  "punch",
  "punch", 
  "punch", 
  "punch",
  "punch",
  "punch",
  "punch", 
  "gore", 
  "touch",
  "claw",
  "punch",
  "punch", 
  "kick", 
  "punch",
  "kick",
};

char *const percent_ded[] =
{
  "is at the brink of death",
  "is majorly wounded",
  "is majorly wounded",
  "is wounded",
  "is wounded",
  "is wounded",
  "has several wounds",
  "is minorly wounded",
  "has a few scratches",
  "has a few bruises",
  "is in excellent shape",
  "is in amazing shape",
  "is in incredible shape",
  "is incredibly healthy",
  "is incredibly healthy",
  "is incredibly healthy",
  "is incredibly healthy",
  "is incredibly healthy",
  "is incredibly healthy",
  "is incredibly healthy",
  "is incredibly healthy",
  "is incredibly healthy",
  "is incredibly healthy"

};


/* This table determines how races interact and how they 
view you when you kill some of their members...9 = really
 really likes that race, -9 = really really hates that race. 
When you kill something of that race, it changes their view of you..
stored in the factions code of your race. Then things will
automatically attack you if they don't like you. */


const int factions[MAX_MOB_TYPE][MAX_MOB_TYPE] =

{
  {9,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,9,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,9,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,9,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,9, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 9,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,9,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,9,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,9,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,9, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
   
  {0,0,0,0,0, 0,0,0,0,0, 9,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,9,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,9,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,9,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,9, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 9,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,9,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,9,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,9,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,9, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 9,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,9,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,9,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,9,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,9, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 9,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,9,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,9,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,9,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,9, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},

  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 9,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,9,0,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,9,0,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,9,0, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,9, 0,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 9,0,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,9,0,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,9,0,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,9,0, 0,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,9, 0,0,0,0},

  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 9,0,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,9,0,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,9,0},
  {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0},
 
};




char *const percent_hit[] =
{
  "awful",
  "bad",
  "bleeding",
  "wounded",
  "injured",
  "hurt",
  "hurt",
  "good",
  "good",
  "good",
  "fine",
  "fine",
  "fine",
  "fine",
  "fine",
  "fine",
  "fine",
  "fine"

};


char *const percent_tired[] =
{
  "exhausted",
  "fainty",
  "weak",
  "beat",
  "winded",
  "fatigued",
  "tired",
  "energetic",
  "energetic",
  "energetic",
  "energetic",
  "energetic",
  "fresh",
  "fresh",
  "fresh",
  "fresh",
  "fresh",
  "fresh",
  "fresh",
  "fresh",
  "fresh",
  "fresh",
  "fresh",
  "fresh"

};

char *const language_table[12] =
{
  "common",
  "elvish",
  "dark elvish",
  "dwarvish",
  "gnomish",
  "grunting",
  "kender",
  "shadaoslavian",
  "dragon",
  "buckawn",
  "skulk",
  "beastman"
};
/* Name            colorname                               weight wpn armor cost str  init hit hp mv dam kickdam */
const struct material_data materials[] ={
  {"copper",		"$4c$6o$4$Bpp$N$6e$4r$N"                ,20,  17, 1, 50,      1, 1, 0, 0,  0,   0, 0, 0},
  {"tin",			"tin"                                   ,10,  15, 1, 50,      2, 1, 0, 0,  0,   0, 0, 0},
  {"aluminum",		"a$Bl$Numin$Bu$Nm"					    ,10,  17, 1, 60,     10, 1, 0, 0,  0,   0, 0, 0},
  {"iron",			"$B$0iron$N"                            ,200, 20, 2, 1200,   20, 1, 0, 0,  0,   0, 0, 1},
  {"steel",			"$Bs$Nt$Be$Ne$Bl$N"                     ,150, 27, 4, 2000,   30, 1, 0, 0,  0,   0, 0, 1},
  {"electrum",		"$1e$Bl$7e$Nct$Br$1u$N$1m$N"            ,150, 17, 1, 5000,    2, 1, 0, 0,  0,   0, 0, 0},
  {"silver",		"$Bsilver$N"                            ,75,  15, 1, 6000,    1, 1, 0, 0,  0,   0, 0, 0},
  {"gold",			"$B$6gold$N"                            ,200, 17, 1, 20000,   2, 1, 0, 0,  0,   0, 0, 0},
  {"platinum",		"$3p$Bl$7a$Nti$Bn$3u$N$3m$N"            ,150, 17, 1, 30000,   2, 1, 0, 0,  0,   0, 0, 0},
  {"molybdenum",	"$B$1mo$Nly$Bbde$Nnu$1$Bm$N"            ,130, 25, 3, 3000,   25, 1, 0, 0,  0,   0, 0, 1},
  {"nickel",        "$B$0ni$Nck$B$0el$N"	                ,200, 35, 1, 3500,   45, 1, 0, 0,  0,   0, 0, 2},
  {"titanium",		"$3titanium$N"			                ,220, 45, 4, 4000,   70, 1, 0, 0,  0,   0, 0, 3},
  {"mithril",       "$B$0m$3i$Bt$7h$3r$N$3i$B$0l$N"         ,150, 27, 3, 45000,  30, 1, 0, 5,  5,   5, 0, 2},
  {"orichalicum",	"$6o$4r$Bic$N$4h$6a$4l$Bic$N$4u$6m$N"   ,290, 45, 4, 60000,  55, 1, 0,10, 10,  10, 1, 3}, 
  {"adamantium",	"$5a$B$5d$Nam$B$7an$Nti$B$5u$N$5m$N"    ,350, 60,10, 80000,  70, 1, 0, 0,  0,   0, 2, 5},
  {"END",			"",  0,  0, 0,     0,  0, 0, 0, 0, 0, 0, 0, 0},
};

const struct forged_items forged[] = {

  {"breastplate", "sturdy", ITEM_WEAR_BODY , ITEM_ARMOR ,13, 3 ,3 ,1 ,1 , 2 , 1},
  {"helmet","heavy" ,ITEM_WEAR_HEAD ,ITEM_ARMOR ,8 ,5 ,5 ,1 ,0 , 0, 2},
  {"greaves","dirty" ,ITEM_WEAR_LEGS ,ITEM_ARMOR ,10 ,5 ,5 , 1  ,2 ,1 ,0 },
  {"boots","dusty" ,ITEM_WEAR_FEET ,ITEM_ARMOR ,8 , 5 ,5 ,1 ,2 ,0 ,0 },
  {"gauntlets","thin" ,ITEM_WEAR_HANDS ,ITEM_ARMOR ,7 ,6 ,6 ,1 ,1 ,0 ,3 },
  {"armplates","thick" ,ITEM_WEAR_ARMS ,ITEM_ARMOR ,8 ,5 ,5 ,1 ,1 ,1 ,3 },
  {"belt","flimsy" ,ITEM_WEAR_WAIST ,ITEM_ARMOR ,7 ,7 ,7 ,1 ,1 ,0 ,1 },
  {"shield","solid" ,ITEM_WEAR_SHIELD ,ITEM_ARMOR ,11,4 ,4 ,1 ,1  ,0 ,4},
  {"mask","slitted" , ITEM_WEAR_FACE ,ITEM_ARMOR ,8 ,7 ,7 ,1 ,0  ,0 ,2},
  {"earring","dangling" ,ITEM_WEAR_EAR1 ,ITEM_ARMOR ,5 ,9 ,9 ,2, 0 ,0  ,2},
  {"bracelet","sparkling" ,ITEM_WEAR_WRIST ,ITEM_ARMOR ,5 ,9 ,9 ,2 ,1  ,0 ,3},
  {"anklet","spiked" ,ITEM_WEAR_ANKLE ,ITEM_ARMOR ,5 ,9 ,9 ,2 ,2  ,0 ,0},
  {"ring","carved" , ITEM_WEAR_FINGER ,ITEM_ARMOR ,5 ,9 ,9 ,2 ,1  ,0 ,3},
  {"necklace","flowing" , ITEM_WEAR_NECK ,ITEM_ARMOR ,5 ,9 ,9 ,2 ,1 ,0 ,1 },
  {"shoulderplate","layered" ,ITEM_WEAR_SHOULDER ,ITEM_ARMOR ,5  ,9 ,9 ,2 ,1 ,0 ,1 },
  {"kneepad", "reinforced", ITEM_WEAR_KNEE, ITEM_ARMOR, 5, 9, 9, 2, 2, 0, 0},
  {"elbowpad", "protective", ITEM_WEAR_ELBOW, ITEM_ARMOR, 5, 9, 9, 2, 1, 0, 3},
  {"hammer","weighty" ,ITEM_WIELD ,ITEM_TOOL ,20 ,2 ,2 ,1 ,1 ,TOOL_FORGING ,0 },
  {"anvil","huge" ,0 ,ITEM_FURNITURE ,35 ,1 ,1 ,1 ,1 ,FURN_ANVIL ,0 },
  {"chunk","rough" ,ITEM_WIELD ,ITEM_MINERAL ,0 ,20 ,20 ,1 ,0  ,0 ,0},
  {"needle","sharp" ,ITEM_WIELD ,ITEM_TOOL ,3 ,9 ,9 ,1 , 1 ,TOOL_NEEDLE ,0},
  {"mortar","small" , ITEM_WIELD ,ITEM_CONTAINER ,1 ,10 ,10 ,1 ,0  ,CONT_MORTAR ,0},
  {"pestle","knobbed" , ITEM_WIELD,ITEM_TOOL ,10 ,8 ,8 ,1 ,0  ,TOOL_PESTLE ,0},
  {"flail","razor-edged" ,ITEM_WIELD ,ITEM_WEAPON ,10 ,4 ,4 ,1 ,ITEM_WHIP ,4 ,4 },
  {"dagger","wicked" ,ITEM_WIELD+ITEM_WEAR_BELT ,ITEM_WEAPON ,8,5 ,5 ,1 ,ITEM_PIERCE ,-2 ,11},
  {"sword","serrated" ,ITEM_WIELD ,ITEM_WEAPON ,10 ,3 ,3 ,1 ,ITEM_SLASHING ,2 ,3 },
  {"axe","gigantic" ,ITEM_WIELD ,ITEM_WEAPON ,11 ,2 ,2,1,ITEM_HEAVY_SLASH,0 ,12},
  {"halberd","spiked" ,ITEM_WIELD ,ITEM_WEAPON ,14 ,2 ,2 ,1 ,ITEM_POLEARM  ,0 ,15},
  {"club","lead-shod" ,ITEM_WIELD ,ITEM_WEAPON ,11 ,3 ,3 ,1 ,ITEM_CONCUSSION ,3 ,7 },
  {"shortbow","light" ,ITEM_WIELD ,ITEM_WEAPON ,8 ,7 ,7 ,1 ,ITEM_RANGED,-10 ,7 },
  {"crossbow","heavy" ,ITEM_WIELD ,ITEM_WEAPON ,11 ,5 ,5 ,1 ,ITEM_RANGED,-8 ,7 },
  {"longbow","huge" ,ITEM_WIELD ,ITEM_WEAPON ,15 ,4 ,4 ,1 ,ITEM_RANGED,-7 ,7 },
  {"arrow","short" ,ITEM_WIELD ,ITEM_AMMO ,1 ,40 ,40 ,1, 0 ,-10 ,1 },
  {"bolt","crossbow" ,ITEM_WIELD ,ITEM_AMMO ,2 ,35 ,35 ,1, 0 ,-8 ,2 },
  {"shaft","clothyard" ,ITEM_WIELD ,ITEM_AMMO ,2 ,30 ,30 ,1, 0 ,-6 ,3 },
  {"end_of_list", "",0,0,0,0,0,0,0,0,0},
};




const struct material_data skin_types[] = {

  { "squirrel",        "squirrel ",                 120, 15, 1, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "rabbit",          "rabbit",                    120, 15, 1, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "raccoon",         "raccoon",                   120, 15, 2, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "cow",             "cow",                       120, 15, 2, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "fox",             "fox",                       120, 15, 3, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "deer",            "deer",                      120, 15, 3, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "boar",            "boar",                      120, 15, 4, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "wolf",            "wolf",                      120, 15, 5, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "bear",            "bear",                      120, 15, 6, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "lion",            "lion",                      120, 15, 7, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "dragon",          "dragon",                    120, 15,10, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
   {"end_of_list",         "",                                0,  0, 0,   0,                   0, 0, 0, 0, 0, 0, 0, 0},
  
};

const struct material_data cloth_types[] = {
  { "cotton",      "cotton",               120, 15, 1, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "wool",        "wool",                 120, 15, 1, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  { "silk",        "silk",                 120, 15, 1, 200,                   0, 1, 0, 0, 0, 0, 0, 0},
  {"end_of_list",         "",                        0,  0, 0,   0,                   0, 0, 0, 0, 0, 0, 0, 0},
  
};


const struct gld_data guild_data[] =
{
{ " Ranger's Guild", "ranger", ACT3_RANGER, GUILD_RANGER, CON},
{ " Mystic's Guild", "mystic", ACT3_MYSTIC, GUILD_MYSTIC, WIS},
{ " Warrior's Guild", "warrior", ACT3_WARRIOR, GUILD_WARRIOR, STR},
{ " Healer's Guild", "healer", ACT3_HEALER, GUILD_HEALER, WIS},
{ " Wizards's Guild", "wizard", ACT3_WIZARD, GUILD_WIZARD, INT},
{ " Rogue's Guild", "rogue", ACT3_ROGUE, GUILD_ROGUE, DEX},
{ " Tinker's Guild", "tinker", ACT3_TINKER, GUILD_TINKER, CON},
{ " Battlemaster's Guild", "battlemaster", ACT3_BATTLEMASTER, GUILD_BATTLEMASTER, STR},
{ " Conjurer's Guild", "conjurer", ACT3_CONJURER, GUILD_CONJURER, INT},
{ " Thief's Guild", "thief", ACT3_THIEFG, GUILD_THIEFG, DEX},
{ " Necromancer's Guild", "necromancer", ACT3_NECROMANCER, GUILD_NECROMANCER, INT},
{ " Monk's Guild", "monk", ACT3_MONK, GUILD_MONK, DEX},
{ " None", "end_of_list", 0, -1}
};


const struct mna_data mana_data[] =
{
  {"Any",  " Any", "\x1b[1;37m" , "*", MANA_ANY}, /* Keep any mana as first */
  {"Air", " Air", "\x1b[1;36m" , "A", MANA_AIR},
  {"Earth", " Earth", "\x1b[0;32m" , "E", MANA_EARTH},
  {"Fire", " Fire", "\x1b[0;31m" , "F", MANA_FIRE},
  {"Water", " Water", "\x1b[1;34m" , "W", MANA_WATER},
  {"Spirit", " Spirit", "\x1b[1;33m" , "S", MANA_SPIRIT},
  {"end_of_list", "None ", "", "", 0},
};


const struct flag_data act_1[] =
{
{TRUE,"sentinel"," sentinel",ACT_SENTINEL,"The mob will not wander or hunt.\n\r"},
{TRUE,"scavenger"," scavenger",ACT_SCAVENGER,"The mob picks up stuff lying on the ground.\n\r"},
{TRUE,"aggressive"," aggressive",ACT_AGGRESSIVE,"The mob attacks all players it can see.\n\r"},
{TRUE,"stay_area"," stay_area",ACT_STAY_AREA,"The mob will not wander from the area it repop'd in.\n\r"},
{TRUE,"stay_sector"," stay_sector",ACT_STAY_SECTOR,"The mob will not wander from the sector type it repop'd in.\n\r"},
{TRUE,"wimpy"," wimpy",ACT_WIMPY,"The mob will try to flee if it gets really low on hit points.\n\r"},
{TRUE,"switch"," switch",ACT_MOBSWITCH,"Switches players it is currently fighting during combat.\n\r"},
{TRUE,"fasthunt"," fasthunt",ACT_FASTHUNT,"Mob hunts players FAST.\n\r"},
{TRUE,"pet"," pet",ACT_PET,"The mob can be bought as a pet if in a pet storage room.\n\r"},
{TRUE,"untouchable"," untouchable",ACT_UNTOUCHABLE,"The mob cannot be hurt by any physical means.. magic only.\n\r"},
{TRUE,"practitioner"," practitioner",ACT_PRACTICE,"Mob teaches things.  Teach <skill#> to set what is taught.\n\r"},
{FALSE,"scramble"," scramble",ACT_SCRAMBLE,"None.\n\r"},
{TRUE,"mountable"," mountable",ACT_MOUNTABLE,"This mob can be ridden/mounted.\n\r"},
{TRUE,"angry"," angry",ACT_ANGRY,"Mob attacks players, but not right away.\n\r"},
{TRUE,"teller"," teller",ACT_BANK_TELLER,"Mob is a bank teller.\n\r"},
{TRUE,"attitude"," attitude",ACT_ATTITUDE,"Attacks whoever looks at it.\n\r"},
{TRUE,"r_weapons"," r_weapons",REPAIRS_ONE,"Repairs weapons.\n\r"},
{TRUE,"r_armors"," r_armors",REPAIRS_TWO,"Repairs armors.\n\r"},
{TRUE,"avenged"," avenged",ACT_AVENGED,"Mob is avenged by other mobs when it dies.\n\r"},
{TRUE,"reboot_only"," reboot_only",ACT_REBOOT_ONLY,"Mob does not repop after it has been killed."},
{TRUE,"stay"," stayoffroad",ACT_STAYOFFROAD,"Mob stays out of city/road sectors.\n\r"},
{TRUE,"soloyell"," soloyell",ACT_SOLOYELL,"Mob will yell for help if yeller # set and alone.\n\r"},
{FALSE, "end_of_list", " nothing", 0, ""}
};



const struct flag_data act_3[] = {
{TRUE,"3clan_guard"," 3clan_guard",ACT3_CLAN_GUARD,"Mob guards the door specified by <door> vs non clannies-need to set clan_guard <number> for the clan.\n\r"},
{TRUE,"3guard_door"," 3guard_door",ACT3_GUARD_DOOR,"Mob guards the door specified by guarding <dir>.\n\r"},
{TRUE,"3sect_guard"," 3sect_guard",ACT3_SECT_GUARD,"Mob guards the door specified by <door> vs non sectnies-need to set sect_guard <number> for the sect.\n\r"},
{TRUE,"3guard_door_align"," 3guard_door_align",ACT3_GUARD_DOOR_ALIGN,"Mob guards door FROM other align players.\n\r"},
{TRUE,"3ride"," 3ride",ACT3_RIDE,"Mob mounts/rides the previously reset mob.\n\r"},
{TRUE,"3follower"," 3follower",ACT3_FOLLOW,"Mob follows the previously reset mob.\n\r"},
{TRUE,"3guard_item"," 3guard_item",ACT3_GUARD_ITEM,"Mob guards item specified by guarding #.\n\r"},
{TRUE,"3mute"," 3mute",ACT3_MUTE,"Mob cannot yell.\n\r"},
{TRUE,"3sleeper"," 3sleeper",ACT3_SLEEP,"Mob follows a regular day/night sleep pattern.\n\r"},
{TRUE,"3guard_mob"," 3guard_mob",ACT3_GUARD_MOB,"Guards mob vnum specified by guard #.\n\r"},
{TRUE,"3stayroad"," 3stayroad",ACT3_STAYROAD,"Mob stays ON road/city sectors.\n\r"},
{TRUE,"3tinkerg"," 3tinkerg",ACT3_TINKER,"Tinker guildmaster.\n\r"},
{TRUE,"3warriorg"," 3warriorg",ACT3_WARRIOR,"Warrior guildmaster.\n\r"},
{TRUE,"3thiefg"," 3thiefg",ACT3_THIEFG,"Thief guildmaster.\n\r"},
{TRUE,"3healerg"," 3healerg",ACT3_HEALER,"Healer guildmaster.\n\r"},
{TRUE,"3rangerg"," 3rangerg",ACT3_RANGER,"Ranger guildmaster.\n\r"},
{TRUE,"3wizardg"," 3wizardg",ACT3_WIZARD,"Wizard guildmaster.\n\r"},
{TRUE,"3rogueg"," 3rogueg",ACT3_ROGUE,"Rogue guildmaster.\n\r"},
{TRUE,"3conjurerg"," 3conjurerg",ACT3_CONJURER,"Conjurer guildmaster.\n\r"},
{TRUE,"3mysticg"," 3mysticg",ACT3_MYSTIC,"Mystic guildmaster.\n\r"},
{TRUE,"3battlemasterg"," 3battlemasterg",ACT3_BATTLEMASTER,"Battlemaster guildmaster.\n\r"},
{TRUE,"3necromancerg"," 3necromancerg",ACT3_NECROMANCER,"Necromancer guildmaster.\n\r"},
{TRUE,"3monkg"," 3monkg",ACT3_MONK,"Monk guildmaster.\n\r"},
{TRUE,"3regenerate"," 3regenerate",ACT3_REGENERATE,"Regenerates hit points at an increased rate.\n\r"},
{FALSE, "end_of_list", " nothing", 0, ""}
};


const struct flag_data act_4[] = {

{TRUE,"4enterscript"," 4enter",ACT4_ENTERSCRIPT,"The mob is checked for enter triggers when it is in the room.\n\r"},
{TRUE,"4leavescript"," 4leave",ACT4_LEAVESCRIPT,"The mob is checked for leave triggers when it is in the room.\n\r"},
{TRUE,"4commandscript"," 4command",ACT4_COMMANDSCRIPT,"The mob is checked for command triggers when it is in the room.\n\r"},
{TRUE,"4quicktick"," 4quicktick",ACT4_QUICKTICK,"The mob is checked for quicktrick triggers in transport_update.\n\r"},
{TRUE,"4movescript"," 4move",ACT4_MOVESCRIPT,"The mob is checked for move triggers when it is in the room.\n\r"},
{TRUE,"4rests"," 4rests",ACT4_REST,"Rests when nobody is around and injured.\n\r"},
{TRUE,"4killallone"," 4killallone",ACT4_KILLALLONE,"Kills mobs/players that aren't set KILLALLONE as well.\n\r"},
{TRUE,"4killalltwo"," 4killalltwo",ACT4_KILLALLTWO,"Kills mobs/players that aren't set KILLALLTWO as well.\n\r"},
{TRUE,"4sleepaggro"," 4sleepaggro",ACT4_SLEEPAGGRO,"Aggressive to sleeping players.\n\r"},
{TRUE,"4killopp"," 4killopp",ACT4_KILL_OPP ,"Kills mobs/players of opposite alignments.\n\r"},
{TRUE,"4protector"," 4protector",ACT4_PROTECTOR ,"Protects from players/mobs of opp align.\n\r"},
{TRUE,"4ranger"," 4ranger",ACT4_RANGER,"Kills mobs with BEAST in name field.\n\r"},
{TRUE,"4summoner"," 4summoner",ACT4_SUMMONER,"Mob summons help when in combat.\n\r"},
{TRUE,"4rescue_one"," 4rescue_one",ACT4_RESCUEONE,"Rescues mobs with PRTCT in name field.\n\r"},
{TRUE,"4rescue_two"," 4rescue_two",ACT4_RESCUETWO,"Rescues mobs with CODEONE in name field.\n\r"},
{FALSE,"4killgladiator"," 4killgladiator",ACT4_KILLGLADIATOR,"Kills mobs named 'gladiator'.\n\r"},
{TRUE,"4assistone"," 4assistone",ACT4_ASSISTONE,"Assists mobs with AONE in name field.\n\r"},
{TRUE,"4assisttwo"," 4assisttwo",ACT4_ASSISTTWO,"Assists mobs with ATWO in name field.\n\r"},
{TRUE,"4assistall"," 4assistall",ACT4_ASSISTALL,"Assists all other fighting mobs in the room.\n\r"},
{TRUE,"4kill_clan"," 4kill_clan",ACT4_KILL_CLAN,"Kills players whose clans are shitlisted.\n\r"},
{FALSE, "end_of_list", " nothing", 0, ""}
};

const struct flag_data affect_flags_2[] =
{
{TRUE, "prot_fire", " Prot-Fire", AFF_PROT_FIRE, "Resistant to fire.\n\r"},
{TRUE, "prot_gas", " Prot-Gas", AFF_PROT_GAS, "Resistant to gas.\n\r"},
{TRUE, "mindshield", " Mindshield", AFF_MINDSHIELD, "This protects you from some amagic.\n\r"},
{TRUE, "speed", " Speed", AFF_SPEED, "This can give you an extra attack.\n\r"},
{TRUE, "paralyze", " Paralyze", AFF_PARALYZE, "This paralyzes someone.\n\r"},
{TRUE, "prot_shock", " Prot-Shock", AFF_PROT_SHOCK, "resistant to shock.\n\r"},
{TRUE, "prot_wind", " Prot-Wind", AFF_PROT_WIND, "resistant to wind.\n\r"},
{TRUE, "prot_wound", " Prot-Wound", AFF_PROT_WOUND, "resistant to wounding.\n\r"},
{TRUE, "prot_paralyze", " Prot-Paralyze", AFF_PROT_PARALYZE, "resistant to paralyze.\n\r"},
{TRUE, "prot_locate", " Prot-Locate", AFF_PROT_LOCATE, "resistant to locate.\n\r"},
{TRUE, "prot_ice", " Prot-Ice", AFF_PROT_ICE, "resistant to ice.\n\r"},
{TRUE, "prot_acid", " Prot-Acid", AFF_PROT_ACID, "resistant to acid.\n\r"},
{TRUE, "prot_astral", " Prot-Astral", AFF_PROT_ASTRAL, "This wards off the effects of astral travel.\n\r"},
{TRUE, "prot_lava", " Prot-Lava", AFF_PROT_LAVA, "This wards off the effects of lava.\n\r"},
{TRUE, "prot_shard", " Prot-Shard", AFF_PROT_SHARD, "This wards off the effects of shards.\n\r"},
{TRUE, "spell_reflect", " Spell-Reflect", AFF_SPELL_REFLECT, "This reflects a spell back to its caster.\n\r"},
{TRUE, "noblood", " Noblood", AFF_NOBLOOD,	"Notracks: this person leaves no blood.\n\r"},
{TRUE, "chameleon", " Chameleon", AFF_CHAMELEON,	"Chameleon: this person has totally blended into the background.\n\r"},
{TRUE, "det_chameleon", "Det-Chameleon", AFF_DET_CHAMELEON, "This allows you to detect someone who is chameleoned.\n\r"},
{TRUE, "detect_fog", " Fog-Vision", AFF_DET_FOG, "This allows you to see perfectly in fog.\n\r"},
{TRUE, "prot_poison", " Prot-Poison", AFF_PROT_POISON, "This wards off the effects of poisons.\n\r"},
{TRUE, "prot_curse", " Prot-Curse", AFF_PROT_CURSE, "This wards off the effects of curses.\n\r"},
{TRUE, "prot_blind", " Prot-Blind", AFF_PROT_BLIND, "This wards off the effects of blindness.\n\r"},
{TRUE, "prot_plague", " Prot-Plague", AFF_PROT_PLAGUE, "This wards off the effects of plagues.\n\r"},
{TRUE, "fireshield", " Fireshield", AFF_FIRESHIELD, "This causes damage to people who hit you in melee.\n\r"},
{TRUE, "manashield", " Manashield", AFF_MANASHIELD, "This creates a shield to absorb some damage in melee.\n\r"},
{FALSE, "end_of_list", " nothing", 0, ""}
};

const struct flag_data augment_flagss[] = 
{

  {TRUE, "leg_plating", " Leg-Plating", AUG_LEG_PLATING, "This augmentation mounts armor plating to your legs.\n\r"},
  {TRUE, "body_plating", " Body-Plating", AUG_BODY_PLATING, "This augmentation mounts plating to your ribcage.\n\r"},
  {TRUE, "head_plating", " Head-Plating", AUG_HEAD_PLATING, "This augmentation mounts armor to your skull.\n\r"},
  {TRUE, "arm_plating", " Arm-Plating", AUG_ARM_PLATING, "This augmentation mounts armor to the skin on your arms.\n\r"},
  {TRUE, "body_power", " Body-Power", AUG_BODY_POWER, "This pumps you up.\n\r"},
  {TRUE, "hand_power", " Hand-Power", AUG_HAND_POWER, "This makes you able to keep a strong grip.\n\r"},
  {TRUE, "leg_power", " Leg-Power", AUG_LEG_POWER, "This makes your legs more durable.\n\r"},
  {TRUE, "arm_power", " Arm-Power", AUG_ARM_POWER, "This improves the power of your arms.\n\r"},
  {TRUE, "mind_power", " Mind-Power", AUG_MIND_POWER, "This improves your ability to focus on arkane powers.\n\r"},
  {TRUE, "mind_focus", " Mind-Focus", AUG_MIND_FOCUS, "This helps your ability to focus on the healing abilities.\n\r"},
  {TRUE, "face_spike", " Face-Spike", AUG_FACE_SPIKE, "This makes nasty spikes grow out of your face.\n\r"},  
  {TRUE, "hand_speed", " Hand-Speed", AUG_HAND_SPD, "This improves the quickness of your hands.\n\r"},
  {TRUE, "foot_speed", " Foot-Speed", AUG_FOOT_SPD, "This makes you more fleet of foot.\n\r"},
  {TRUE, "leg_speed", " Leg-Speed", AUG_LEG_SPD, "This improves how fast you can run and move.\n\r"},
  {TRUE, "foot_power", " Foot-Power", AUG_FOOT_POWER, "This makes your feet and calves stronger.\n\r"},
  {TRUE, "foot_plating", " Foot-Plating", AUG_FOOT_PLATING, "This toughens your feet.\n\r"},
  {TRUE, "mana_boost", " Mana-Boost", AUG_MANA_BOOST, "This gives a larger reserve of mana.\n\r"},
  {TRUE, "channeling", " Channeling", AUG_CHANNEL, "This increases your natural ability to cast.\n\r"},
  {TRUE, "mental_Armor", " Mental-Armor", AUG_MENTAL_ARMOR, "This helps you defend against hostile spells cast against you.\n\r"},
  {TRUE, "focus", " Focus", AUG_FOCUS, "This helps you keep your mind focused in combat.\n\r"}, 
  {FALSE, "end_of_list", " nothing", 0, ""}
};



const struct flag_data affect_flagss[] =
{

  {TRUE, "blind", " Blind", AFF_BLIND, "This toggles blindness.\n\r"},
  {TRUE, "invisible", " Invisible", AFF_INVISIBLE, "This toggles invisibility.\n\r"},
  {FALSE, "detect-good", " Detect-Good", AFF_DETECT_GOOD, "Tells if you can detect good.\n\r"},
  {FALSE, "detect-evil", " Detect-Evil", AFF_DETECT_EVIL, "Tells if you can detect evil.\n\r"},
  {TRUE, "detect-invis", " Detect-Invis", AFF_DETECT_INVIS, "This toggles detect-invisible.\n\r"},
  {FALSE, "detect-magic", " Detect-Magic", AFF_DETECT_MAGIC, "This shows magic auras on items.\n\r"},
  {TRUE, "detect-hidden", " Detect-Hidden", AFF_DETECT_HIDDEN, "This toggles whether the mob can see hidden.\n\r"},
  {TRUE, "wound", " Wound", AFF_WOUND, "This makes the mob lose hps rapidly..\n\r"},
  {TRUE, "sanctuary", " Sanctuary", AFF_SANCTUARY, "This affect flag toggles sanctuary.\n\r"},
  {FALSE, "faerie-fire", " Faerie-Fire", AFF_FAERIE_FIRE, "This makes the mob's armor much much worse.\n\r"},
  {TRUE, "infrared", " Infrared", AFF_INFRARED, "See in the dark.\n\r"},
  {FALSE, "curse", " Curse", AFF_CURSE, "This means the mob cannot recall.\n\r"},
  {TRUE, "confuse", " Confuse", AFF_CONFUSE, "Toggles if the mob is confused.\n\r"},
  {FALSE, "apoison", " Poison", AFF_POISON, "This toggles if the mob is poisoned.\n\r"},
  {FALSE, "plague", " Plague", AFF_PLAGUE, "Toggles if the mob has the plague or not.\n\r"},
  {TRUE, "protect", " Hardened Skin", AFF_PROTECT, "Toggles total protection.\n\r"},
  {TRUE, "lethargy", " Lethargy", AFF_LETHARGY, "This affect drains when the target moves.\n\r"},
  {TRUE, "sleep", " Sleep", AFF_SLEEP, "Toggles if someone isasleep.\n\r"},
  {TRUE, "sneak", " Sneak", AFF_SNEAK, "Toggles silent movement.\n\r"},
  {TRUE, "hide", " Hide", AFF_HIDE, "Toggles if the mob will hide when possible.\n\r"},
  {FALSE, "charm", " Charm", AFF_CHARM, "Toggles if the mob can be ordered.\n\r"},
  {TRUE, "flying", " Flying", AFF_FLYING, "Is the mob flying or not.\n\r"},
  {TRUE, "pass-door", " Pass-Door", AFF_PASS_DOOR, "This allows passage through closed doors.\n\r"},
  {TRUE, "regeneration", " Regeneration", AFF_REGENERATE, "This makes the mob regain hps fast.\n\r"},
  {TRUE, "nocharm", " Nocharm", AFF_NOCHARM, "Is it charmable?\n\r"},
  {TRUE, "protect_align", " Protect-Align", AFF_PROT_ALIGN, "This reduces damage taken by those who are not of your same alignment.\n\r"},
  {TRUE, "mindsap", " Mindsap", AFF_MINDSAP, "This affect drains mana from the target.\n\r"},
  {FALSE, "awareness", " Awareness", AFF_AWARENESS, "Allows sleeping people to see who enters room.\n\r"},
  {TRUE, "water-breath", " Water-Breath", AFF_BREATH_WATER, "This allows the mob to breath underwater.\n\r"},
  {TRUE, "safety", " Safety", AFF_SAFETY, "Lower level aggros don't attack.\n\r"},
  {FALSE, "end_of_list", " nothing", 0, ""}

};

const struct flag_data extra_flagss[] =
{

  {TRUE, "glow", " Glow", ITEM_GLOW, "This flag makes the item glow.\n\r"},
  {TRUE, "hum", " Hum", ITEM_HUM, "This flag makes the item hum.\n\r"},
  {TRUE, "elemental", " Elemental", ITEM_ELEMENTAL, "This flag makes the item cast elemental effects.\n\r"},
  {TRUE, "dropscript", "", ITEM_DROPSCRIPT, "This flag says the item has a script associated with it when dropped.\n\r"},
  {TRUE, "good", " Good", ITEM_GOOD, "This flag makes the item glow blue if you are affected by 'detect good'.\n\r"},
  {TRUE, "evil", " Evil", ITEM_EVIL, "This flag makes the item glow red if you are affected by 'detect evil'.\n\r"},
  {TRUE, "item_invis", " Item Invis", ITEM_INVIS, "This flag defaults the item to be invisible when it's created.\n\r"},
  {TRUE, "magic", " Magic", ITEM_MAGIC, "This flag makes the item seen as magic to those who have 'detect magic'.\n\r"},
  {TRUE, "nodrop", " Nodrop", ITEM_NODROP, "The item cannot be dropped... Use *very* sparingly.\n\r"}, 
  {TRUE, "givescript", "", ITEM_GIVESCRIPT, "This flag means the item has a script associated with it when given."},
  {TRUE, "ranged", " Ranged", ITEM_RANGED, "This is a type of bow that shoots things.\n\r"},
  {TRUE, "noremove", " Noremove", ITEM_NOREMOVE, "You can never remove this object until you die... Use sparingly!\n\r"},
  {FALSE, "inventory", " Inventory", ITEM_INVENTORY, ""},
  {TRUE, "nosave", " Nosave", ITEM_NOSAVE, "Object does not save when player quits.  NEVER use with wearable objects!\n\r"},
  {TRUE, "commandscript", "", ITEM_COMMANDSCRIPT, "This flag says the item has a script associated with it when commandped.\n\r"},
  {TRUE, "unseen", " Unseen", ITEM_UNSEEN, "Object cannot be seen.  Cannot be searched for if take flag isn't set.\n\r"},
  {TRUE, "piercing", " Piercing", ITEM_PIERCE, "Object gives a bonus to those with piercing weapons skill.\n\r"},
  {TRUE, "slashing", " Slashing", ITEM_SLASHING, "Object gives a bonus to those with slashing weapons skill.\n\r"},
  {TRUE, "heavy_slash", " Heavy_Slash", ITEM_HEAVY_SLASH, "Object gives a bonus to stronger people with slashing weapons skill.\n\r"},
  {TRUE, "light_slash", " Light_Slash", ITEM_LIGHT_SLASH, "Object gives a massive bonus to slashing since it's light.\n\r"},
  {TRUE, "whip", " Whip", ITEM_WHIP, "Object gives a bonus to those with whipping weapons skill.\n\r"},
  {TRUE, "concussion", " Concussion", ITEM_CONCUSSION, "Object gives a bonus to those with concussion weapons skill.\n\r"},
  {TRUE, "wounding", " Wounding", ITEM_WOUND, "The item has wounding effects.\n\r"}, 
  {TRUE, "blinding", " Blinding", ITEM_BLIND, "The item has blind effects.\n\r"},
  {TRUE, "poisoning", " Poisoning", ITEM_POISON, "The item has poison effects.\n\r"},
  {TRUE, "save", " Save", ITEM_SAVE, "Object saves on the character and does not go into his/her corpse when he/she dies.\n\r"},
  {TRUE, "nostore", " Nostore", ITEM_NOSTORE, "Object cannot be stored in a clan bank or personal bank.\n\r"},
  {TRUE, "polearm", " Polearm", ITEM_POLEARM, "Object is a polearm.\n\r"},
  {TRUE, "power", " Power", ITEM_POWER, "Object is a weapon of power.\n\r"},
  {TRUE, "flame", " Flame", ITEM_FLAME, "This flag makes the item flame.\n\r"},
  {TRUE, "iholy", " Holy", ITEM_HOLY, "This flag is for good only.\n\r"},
  {TRUE, "iunholy", " Unholy", ITEM_UNHOLY, "This flag is for evil only. \n\r"},
  {TRUE, "knockdown", " Knockdown", ITEM_KNOCKDOWN, "The item has a chance on hit of causing knockdown effects.\n\r"},
  {TRUE, "vampiric", " Vampiric", ITEM_VAMPIRIC, "The item has a chance on hit of giving a percentage of damage back to the wielder as health.\n\r"},
  {TRUE, "manarestore", " ManaRestoration", ITEM_MANARESTORE, "The item has a chance on hit of giving a percentage of damage back to the wielder as mana.\n\r"},
  {TRUE, "crusader", " Crusader", ITEM_CRUSADER, "The item has a chance on hit of giving the wielder the crusader effect.\n\r"},
  {TRUE, "manaburn", " ManaBurn", ITEM_MANABURN, "The item has a chance on hit of draining off a percentage of damage from mana.\n\r"},
  {TRUE, "guardian", " Guardian", ITEM_GUARDIAN, "The item has a chance on hit(or being hit) of giving it's wearer the guardian effect.\n\r"},
  {TRUE, "reflect", " Reflect", ITEM_REFLECT, "The item has a chance on spell hit of reflecting the spell back to the caster.\n\r"},
  {TRUE, "spellshield", " SpellShield", ITEM_SPELLSHIELD, "The item has a chance on spell hit of giving the weared the spell shield effect.\n\r"},
  {TRUE, "weaken", " Weakening", ITEM_WEAKEN, "The item has a chance on hit of draining a percentage of damage from stamina.\n\r"},
  {TRUE, "overpower", " Overpower", ITEM_OVERPOWER, "The item has a chance on hit of dealing double damage.\n\r"},
  {FALSE, "end_of_list", " nothing", 0, ""}
  
};


const struct flag_data elemental_alignment_flags[] = 
{
  {TRUE, "align_fire", " \x1B[1;31mRed Shard\x1B[0;37m", ELEMENTAL_ALIGN_FIRE, "Player is aligned with the Fire Element.\n\r"},
  {TRUE, "align_air", " \x1B[1;36mClear Shard\x1B[0;37m", ELEMENTAL_ALIGN_AIR, "Player is aligned with the Air Element.\n\r"},
  {TRUE, "align_water", " \x1B[1;34mBlue Shard\x1B[0;37m", ELEMENTAL_ALIGN_WATER, "Player is aligned with the Water Element.\n\r"},
  {TRUE, "align_earth", " \x1B[1;32mGreen Shard\x1B[0;37m", ELEMENTAL_ALIGN_EARTH, "Player is aligned with the Earth Element.\n\r"},
  {TRUE, "align_good", " \x1B[1;29mWhite Shard\x1B[0;37m", ELEMENTAL_ALIGN_GOOD, "Player is aligned with the Good Element.\n\r"},
  {TRUE, "align_evil", " \x1B[1;30mBlack Shard\x1B[0;37m", ELEMENTAL_ALIGN_EVIL, "Player is aligned with the Evil Element.\n\r"},
  {FALSE, "end_of_list", " nothing", 0, ""}
};


const struct flag_data item_guild_flags[] = 
{
  {TRUE, "antitinker", " Anti-Tinker", ITEM_ANTI_TINKER, "Object cannot be used by the Tinkers Guild.\n\r"},
  {TRUE, "antiwarrior", " Anti-Warrior", ITEM_ANTI_WARRIOR, "Object cannot be used by the Warriors Guild.\n\r"},
  {TRUE, "antithief", " Anti-Thief", ITEM_ANTI_THIEF, "Object cannot be used by the Thief Guild.\n\r"},
  {TRUE, "antihealer", " Anti-Healer", ITEM_ANTI_HEALER, "Object cannot be used by the Healers Guild.\n\r"},
  {TRUE, "antiwizard", " Anti-Wizard", ITEM_ANTI_WIZARD, "Object cannot be used by the Wizards Guild.\n\r"},
  {TRUE, "antiranger", " Anti-Ranger", ITEM_ANTI_RANGER, "Object cannot be used by the Rangers Guild.\n\r"},
  {TRUE, "antirogue", " Anti-Rogue", ITEM_ANTI_ROGUE, "Object cannot be used by the Rogues Guild.\n\r"},
  {TRUE, "anticonjurer", " Anti-Conjurer", ITEM_ANTI_CONJURER, "Object cannot be used by the Conjurers Guild.\n\r"},
  {TRUE, "antimystic", " Anti-Mystic", ITEM_ANTI_MYSTIC, "Object cannot be used by the Mystics Guild.\n\r"},
  {TRUE, "antibattlemaster", " Anti-Battlemaster", ITEM_ANTI_BATTLEMASTER, "Object cannot be used by the Battlemasters Guild.\n\r"},
  {TRUE, "antinecromancer", " Anti-Necromancer", ITEM_ANTI_NECROMANCER, "Object cannot be used by the Necromancers Guild.\n\r"},
  {TRUE, "antimonk", " Anti-Monk", ITEM_ANTI_MONK, "Object cannot be used by the Monk Guild.\n\r"},
  {FALSE, "end_of_list", " nothing", 0, ""}
};

const struct flag_data room_flags2[] =
{
 {TRUE, "fallsfast", " fallsfast", ROOM2_FALLSFAST , "Players tend to fall down from here.\n\r"},
 {TRUE, "extraheal", " extraheal", ROOM2_EXTRAHEAL , "Players are healed faster here.\n\r"},
 {TRUE, "extramana", " extramana",ROOM2_EXTRAMANA , "Players regen mana faster here.\n\r"},
 {TRUE, "moving", " moving",ROOM2_MOVING , "The room moves you to anothe room.\n\r"},
 {TRUE, "manadrain", " manadrain",ROOM2_MANADRAIN , "Mana gets drained here.\n\r"},
 {TRUE, "firetrap", " firetrap", ROOM2_FIRETRAP , "A fire lashes out at players.\n\r"},
 {TRUE, "poisontrap", " poisontrap", ROOM2_POISONTRAP , "Poison darts try to hit the players.\n\r"},
 {TRUE, "arrowtrap", " arrowtrap", ROOM2_ARROWTRAP, "Arrows are released to impale the players.\n\r"},
 {TRUE, "rocktrap", " rocktrap", ROOM2_ROCKTRAP, "Rocks fall on the players.\n\r"},
 {TRUE, "spiketrap", " spiketrap", ROOM2_SPIKETRAP, "Spikes appear to impale the players.\n\r"},
 {TRUE, "gastrapone", " gastrapone", ROOM2_GASTRAPONE, "A minor gas cloud appears to poison players.\n\r"},
 {TRUE, "gastraptwo", " gastraptwo", ROOM2_GASTRAPTWO, "A major gas cloud poisons players.\n\r"},
 {TRUE, "geysertrap", " geysertrap", ROOM2_GEYSERTRAP, "A stream of scalding hot liquid burns players.\n\r"},
 {TRUE, "avalanche", " avalanche", ROOM2_AVALANCHE, "A mound of snow falls on the players.\n\r"},
 {TRUE, "acidspray", " acidspray", ROOM2_ACIDSPRAY, "A stream of acid burns away the player equipment.\n\r"},
 {FALSE, "end_of_list", " nothing", 0, ""}
};

const struct flag_data room_flagss[] =
{

  {TRUE, "dark", " dark", ROOM_DARK, "Room is dark.  If it's inside, use 'indoors' toggle too.\n\r"},
  {TRUE, "no_mobs", " no_mobs", ROOM_NO_MOB, "Mobs in adjacent rooms will not enter this room when wandering.\n\r"},
  {TRUE, "indoors", " indoors", ROOM_INDOORS, "The room is indoors in general.  [Indoors].\n\r"},
  {TRUE, "lowmagic", " lowmagic", ROOM_LOWMAGIC, "Spells do not work as well here.\n\r"},
  {TRUE, "highmagic", " highmagic", ROOM_HIGHMAGIC, "Spells work better here.\n\r"},
  {TRUE, "underground", " underground", ROOM_UNDERGROUND, "These kinds of rooms are underground just as if they were set that way as sectors."},
  {TRUE, "pet_shop", " pet_shop", ROOM_PET_SHOP, "This room is a pet shop.  This vnum+1 is pet storage.\n\r"},
  {TRUE, "nohear", " nohear", ROOM_NOHEAR, "No sound can be heard here due to environment.\n\r"},
  {TRUE, "noisy", " noisy", ROOM_NOISY, "Sound doesn't travel well through this room.\n\r"},
  {TRUE, "no_summon", " no_summon", ROOM_NOSUMMON, "You cannot summon to or from this room.\n\r"},
  {TRUE, "untrackable", " untrackable", ROOM_UNTRACKABLE, "You cannot be tracked while in this room.\n\r"},
  {TRUE, "nomagic", " nomagic", ROOM_NOMAGIC, "All spells are absorbed by the ground in this room.\n\r"},
  {TRUE, "silence", " silence", ROOM_SILENCE, "Nothing can be said, gossip'd telled, etc.. in this room.\n\r"},
  {TRUE, "norecall", " norecall", ROOM_NO_RECALL, "You cannot recall from this room.\n\r"},
  {TRUE, "clanstore", " clanstore", ROOM_CLANSTORE, "Clan storage location.. Must be within clan start vnum + 20.\n\r"},
  {TRUE, "teleport", " teleport", ROOM_CANTELEPORT, "Accessable through the teleport spell."},
  {TRUE, "commandscript", " Commandscript", ROOM_COMMANDSCRIPT, "Commandscript...when things command, this trigger list is checked."},
  {TRUE, "enterscript", " Enterscript", ROOM_ENTERSCRIPT, "Enterscript...when things enter, this trigger list is checked."},
  {TRUE, "leavescript", " leavescript", ROOM_LEAVESCRIPT, "Leavescript...when things leave, this trigger list is checked."},
  {TRUE, "dropscript", " dropscript", ROOM_DROPSCRIPT, "Dropscript...when things are dropped, this trigger list is checked."},
  {TRUE, "getscript", " getscript", ROOM_GETSCRIPT, "Getscript...when things are gotten, this trigger list is checked."},
  {FALSE, "end_of_list", " nothing", 0, ""}

};




const struct flag_data spell_flagss[]=
{
  
  {TRUE, "summon", " Summon", SPELL_SUMMON , "This spell allows you to summon someone."},
  {TRUE, "teleport", " Teleport", SPELL_TELEPORT , "This spell transports you or the target to a random spot around the world."},
  {TRUE, "all_in_room", " All_In_Room", SPELL_ALL_IN_ROOM , "This spell will hit everyone in the room."},
  {TRUE, "remove_bit", " Removes_Bit", SPELL_REMOVES_BIT , "This spell removes the specific affects listed."},
  {TRUE, "heals_dam", " Heals_Dam", SPELL_HEALS_DAM , "This spell adds hps instead of removing them."},
  {TRUE, "message", " Message", SPELL_MESSAGE , "This is the message spell for cross align talk."},
  {TRUE, "dispel_magic", " Dispel_Magc", SPELL_DISPEL_MAGIC , "This spell removes spell affects from your enemies."},
  {TRUE, "hurt_opp_align", " Hurt_Opp_Align", SPELL_HURT_OPP_ALIGN , "This spell will only hurt evil creatures of the opposite alignment\n\r."},
  {TRUE,  "hurt_undead", " Hurt_Undead", SPELL_HURT_UNDEAD , "This spell only hurts undead and ghosts."},
  {TRUE, "modify_object", " Modify_Object", SPELL_MODIFY_OBJECT , "Unused-should change objects."},
  {TRUE, "identify", " Identify", SPELL_IDENTIFY , "This is the identify spell."},
  {TRUE, "locate_object", " Locate_Object", SPELL_LOCATE_OBJECT , "This spell lets you find objects in the world."},
  {TRUE, "scry", " Scry", SPELL_SCRY , "This spell allows the caster to do a look at some other creature."},
  {TRUE, "locate_person", " Locate_Person", SPELL_LOCATE_PERSON , "This spell gives the caster the room name of his target."},
  {TRUE, "recall", " Recall", SPELL_RECALL , "This spell returns the caster to his safe room."},
  {TRUE, "two_saves", " Two_Saves", SPELL_TWO_SAVES , "The target of this spell gets two chances to resist."},
  {TRUE, "only_multi_mobs", " Only_Multi_Mobs", SPELL_ONLY_MULTI_MOBS , "This spell will only hit if more than one mob is here."},
  {TRUE, "dam1/2", " Dam1/2", SPELL_HALVED_SAVE , "The target takes half damage if they save."},
  {TRUE, "not_self", " Not_Self", SPELL_NOT_SELF , "This spell may only be cast on others."},
  {TRUE, "raise_undead", " Raise_Undead", SPELL_RAISE_UNDEAD , "This spell causes something dead to follow you."},
  {TRUE, "life_tap", " Life_Tap", SPELL_LIFE_TAP , "This spell will suck the life out of your victim and into you."},
  {TRUE, "obj_to_room", " Obj_To_Room", SPELL_OBJ_TO_ROOM , "This spell is useful for making untakeable portals or things."},
  {TRUE, "add_move", " Add_Move", SPELL_ADD_MOVE , "This spell restores movements for the target."},
  {TRUE, "only_group", " Only_Group", SPELL_ONLY_GROUP , "This spell only affects the caster's group."},
  {TRUE, "only_not_group", "Only_Not_Group", SPELL_ONLY_NOT_GROUP , "This spell only affects opponents of the caster's group."},
  {TRUE, "mass_distortion", "Mass_Distortion", SPELL_MASS_DISTORTION, "This spell drains half the hps of the victim."},
  {FALSE, "end_of_list", " nothing", 0, ""}
};

const struct flag_data spell_flags2[]=
{
  {TRUE, "acid", " Acid", SPELL_ACID , "This spell eats away equipment."},
  {TRUE, "firespell", " Firespell", SPELL_FIRE , "This spell burns flammable materials."},
  {TRUE, "ice", " Ice", SPELL_ICE , "This spell freezes your enemies."},
  {TRUE, "shock", " Shock", SPELL_SHOCK , "This spell electrocutes your enemies."},
  {TRUE, "wind", " Wind", SPELL_WIND , "This spell ruins some affects dealing with concealment or movement."},
  {TRUE, "ranged", " Ranged", SPELL_RANGED , "This spell can hit targets in a different room."},
  {TRUE, "area", " Area", SPELL_AREA , "This spell affects people over several rooms."},
  {TRUE, "shard", " Shard", SPELL_SHARD , "This spell sends out tiny slivers of rock to cut your enemies."},
  {TRUE, "vampiric", " Vampiric", SPELL_VAMPIRIC , "This spell drains your enemies of health, and gives it to you."},
 {FALSE, "end_of_list", " nothing", 0, ""}
};



struct race_data race_info[RACE_COUNT];


/* Alignments will get bonuses just like races will...usually smaller amounts tho. */

struct race_data align_info[NUM_ALIGN];

const struct color_data color_table[] =
{
  {"\x1b[0;30m", "$R$0", "black", 0},
  {"\x1b[0;34m", "$R$1", "blue", 1},
  {"\x1b[0;32m", "$R$2", "green", 2},
  {"\x1b[0;36m", "$R$3", "cyan", 3},
  {"\x1b[0;31m", "$R$4", "red", 4},
  {"\x1b[0;35m", "$R$5", "purple", 5},
  {"\x1b[0;33m", "$R$6", "brown", 6},
  {"\x1b[0;37m", "$R$7", "grey", 7},
  {"\x1b[1;30m", "$B$0", "dark_grey", 8},
  {"\x1b[1;34m", "$B$1", "light_blue", 9},
  {"\x1b[1;32m", "$B$2", "light_green", 10},
  {"\x1b[1;36m", "$B$3", "light_cyan", 11},
  {"\x1b[1;31m", "$B$4", "light_red", 12},
  {"\x1b[1;35m", "$B$5", "magenta", 13},
  {"\x1b[1;33m", "$B$6", "yellow", 14},
  {"\x1b[1;37m", "$B$7", "white", 15},
  {"\x1b[1m", "$B", "bold", 16},
  {"\x1b[5m", "$F", "flashing", 17},
  {"\x1b[0m", "$R", "normal", 19}
};




const struct attack_type attack_table[] =
{
  {"hit", TYPE_HIT},		/*  0   */
  {"slice", TYPE_SLASH},	/*  1   */
  {"stab", TYPE_PIERCE},
  {"slash", TYPE_SLASH},
  {"whip", TYPE_WHIP},
  {"claw", TYPE_SLASH},		/*  5   */
  {"hammer", TYPE_POUND},
  {"pound", TYPE_POUND},
  {"crush", TYPE_POUND},
  {"gore", TYPE_PIERCE},
  {"bite", TYPE_PIERCE},	/*  10  */
  {"pierce", TYPE_PIERCE},
  {"cleave", TYPE_SLASH},
  {"stab", TYPE_POLEARM},
  {"slice", TYPE_POLEARM},    
  {"cleave", TYPE_POLEARM},    /* 15 */
  {"wail", TYPE_HIT},
  {"wail", TYPE_HIT},
  {"wail", TYPE_HIT},
  {"wail", TYPE_HIT},
  {"wail", TYPE_HIT},
  {"wail", TYPE_HIT},
  {"wail", TYPE_HIT},
  {"wail", TYPE_HIT}
};






const struct liq_type liq_table[LIQ_MAX + 4] =
{
  {"water", "clear",
   {0, 1, 10}},			/*  0 */
  {"beer", "amber",
   {1, 2, 5}},
  {"wine", "rose",
   {2, 2, 5}},
  {"ale", "brown",
   {2, 2, 5}},
  {"brandy", "dark",
   {5, 2, 5}},
  {"whisky", "golden",
   {6, 1, 4}},			/*  5 */
  {"everclear", "golden",
   {10, 0, 0}},
  {"rum", "brown",
   {3, 3, 3}},
  {"salt water", "clear",
   {0, 1, -2}},
  {"milk", "white",
   {0, 3, 6}},
  {"unused", "none",
   {-1, 1, 6}},			/* 10 */
  {"tea", "tan",
   {0, 1, 6}},
  {"blood", "red",
   {0, 2, -1}},
  {"unblood", "green",
   {0, 2, -2}},
  {"lemonade", "yellow",
   {0, 1, 8}},
  {"slime", "green",
   {0, 3, 3}},
  {"coffee", "black",
   {0, 1, 4}},
  {"cider", "brown",
   {0, 1, 6}},
  {"citrus", "yellow",
   {0, 1, 8}},
  {"citrus", "yellow",
   {0, 1, 8}}
};

int roll_values[NUM_ROLLS][6][NUM_STATS][2] =

/* Max of 24 points added per letter, min of 18 */
/* First Roll A-F = DOWN STR-INT-WIS-DEX-CON (lower, UPPER) across */
{        /*STR     INT     WIS     DEX     CON       MAX   MIN*/
  /*A*/	{{{6,6},  {3,3},  {3,3},  {6,6},  {6,6}},  //24    24
  /*B*/  {{3,3},  {6,6},  {5,5},  {6,6},  {4,4}},  //24    24
  /*C*/  {{3,3},  {5,5},  {6,6},  {5,5},  {5,5}},  //24    24
  /*D*/  {{4,4},  {4,4},  {3,3},  {6,6},  {7,7}},  //24    24
  /*E*/  {{4,4},  {4,4},  {4,4},  {6,6},  {6,6}},  //24    24
  /*F*/  {{6,6},  {3,3},  {4,4},  {6,6},  {5,5}}}, //24    24

  /* Second Roll */
         /*STR     INT     WIS     DEX     CON   */
  /*A*/ {{{5,5},  {4,4},  {4,5},  {4,5},  {5,5}},  //24    22
  /*B*/	 {{5,6},  {3,3},  {3,3},  {6,7},  {5,6}},  //24    22
  /*C*/  {{3,3},  {5,6},  {6,7},  {5,5},  {3,3}},  //24    22
  /*D*/	 {{3,3},  {6,7},  {5,6},  {4,4},  {4,4}},  //24    22
  /*E*/  {{5,6},  {3,3},  {4,4},  {5,6},  {4,5}},  //24    22
  /*F*/	 {{4,5},  {4,4},  {5,5},  {4,5},  {4,5}}}, //24    22
 
  /* Third Roll */
	     /*STR     INT     WIS     DEX     CON   */
  /*A*/ {{{5,5},  {4,4},  {4,5},  {4,5},  {5,5}},  //24    22
  /*B*/	 {{5,6},  {3,3},  {4,4},  {5,6},  {5,6}},  //24    22
  /*C*/  {{4,4},  {5,6},  {5,6},  {4,4},  {4,4}},  //24    22
  /*D*/	 {{3,3},  {5,6},  {5,6},  {5,5},  {4,4}},  //24    22
  /*E*/  {{5,6},  {3,3},  {4,4},  {5,6},  {4,5}},  //24    22
  /*F*/	 {{4,5},  {5,5},  {5,5},  {4,4},  {4,5}}}, //24    22

  /* Fourth Roll */
         /*STR     INT     WIS     DEX     CON   */ 
  /*A*/ {{{5,6},  {3,3},  {4,4},  {5,5},  {5,6}},  //24    22
  /*B*/  {{3,3},  {5,6},  {6,6},  {5,5},  {3,4}},  //24    22
  /*C*/  {{4,4},  {6,6},  {5,6},  {3,4},  {4,4}},  //24    22
  /*D*/	 {{5,6},  {4,4},  {3,3},  {5,6},  {4,5}},  //24    22
  /*E*/  {{4,4},  {4,5},  {4,5},  {6,6},  {4,4}},  //24    22
  /*F*/	 {{4,4},  {4,4},  {4,4},  {5,6},  {5,6}}}, //24    22

/*Min/Max (12,24) (12,25) (12,25) (12,25) (12,25)    96    90*/
};






/* Spell Gsn, Ingredients (560 + number = vnum of herb), timer, mortar equipment level */ 


/* ALWAYS ALWAYS ALWAYS USE 0 FOR THINGS THAT ARE NOT INGREDIENTS..NEVER
   NEGATIVE AND ALWAYS MAKE SURE THE POTION INGREDIENT NUMBER IS LESS
   THAN OR EQUAL TO 42 SINCE THAT IS THE MAX NUMBER OF HERBS WE HAVE!!! */
const struct potion_ingredients pot_info[] =
{
  {44, {1,21,34,4,9, 9,9,9,0,0},            20,  6}, /* Heal Crit */
  {60, {3,7,2,26,2, 0,0,0,0,0},        20,  2}, /* Refresh */
  {48, {3,7,2,26,2, 2,2,0,0,0},        20,  2}, /* Refresh */
  {45, {11,22,33,9,9, 9,9,0,0,0},      100, 8}, /* Healing */
  {44, {1,3,5,7,9, 9,9,0,0,0},            20,  4}, /* Heal serious */
  {44, {1,21,34,4,26, 0,0,0,0,0},            100,  8}, /* cureall */
  {45, {9,9,9,9,9, 9,9,9,9,9},      100, 8}, /* divine */
  {45, {11,22,33,9,9, 9,9,0,0,0},      100, 8}, /* purif */
  {6,  {8,0,31,23,3, 22,26,0,5,0},       20,  10}, /* Protection*/
  {10,  {0,0,5,15,25, 25,30,27,26,0},       100,  6}, /* D-invis */
  {11,  {3,8,17,26,33, 0,0,0,0,0},       100,  4}, /* infra */
  {12,  {6,12,18,26,30, 36,7,28,0,0},       50,  7}, /* d-hidden */
  {14,  {13,26,38,35,36, 32,0,0,0,0},       100,  3}, /* Fly */
  {15,  {26,33,41,42,40, 0,0,0,0,14},       10,  8}, /* water breath */
  {22,  {26,26,26,26,26, 26,26,26,26,26},       20,  12}, /* Passdoor */
  {29,  {31,37,41,5,8, 13,19,21,23,24},       20,  4}, /* bless */
  {30,  {2,7,13,20,28, 37,8,9,11,14},       10,  7}, /* war-bless */
  {32,  {22,23,25,28,32, 37,1,8,16,0},       10,  11}, /* haste */
  {33,  {32,4,6,31,40, 38,29,31,27,7},       10,  3}, /* battle */
  {36,  {13,19,0,0,0, 0,0,0,0,0},       100,  1}, /* minor */
  {37,  {13,19,27,0,0, 0,0,0,0,0},       80,  2}, /* Shield */
  {38,  {13,19,27,17,0, 0,0,0,0,0},       60,  3}, /* major */
  {39,  {13,19,27,17,21, 0,0,0,0,0},       40,  5}, /* Shell */
  {40,  {13,19,27,9,13, 18,0,0,0,0},       20,  7}, /* stone */
  {41,  {13,19,27,31,36, 42,7,0,0,0},       10,  9}, /* warding */
  {-1, {0,0,0,0,0,0,0,0,0,0},        0,    0}, /* End of List keep -1 there */
};
