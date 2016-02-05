#include <sys/types.h> 
#include <ctype.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "emlen.h"

#ifdef WINDOWS

 
#include <winsock.h>
#include <sys\types.h>
#include <direct.h>
#else
#include <sys/errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/time.h>
#include <dirent.h>
#endif


extern int _filbuf (FILE *);

char cl[10024];


void load_scriptassoc (void);

SINGLE_TRIGGER *trigger_list[TMAX_TRIGGERS];
SCRIPT_INFO *info_list;
CODE *code_list[256];
SA *sa_head[27];

CHAR_DATA *aggro_check = NULL;
time_t bt_tm;
time_t startup_time;
AUCTION *auction_list = NULL;
int auction_number = 0;
int ti = 5;
DAM_MSG dmsg;
CHR_FUNCTION *cf = NULL;
COMMAND *command_hash[256];
POWER pow;
bool alliance[NUM_ALIGN][NUM_ALIGN];
int commands = 0;
extern char str_boot_time[];
bool allow_anything = FALSE;
CHAR_DATA *pedit_found;
int min_level;
int max_level;
int bg_multiplier;
CHANNEL *chan_first = NULL;
char tmpt[2048];
CHAR_DATA *f_first = NULL;
long day_counter;
char hugebuf_o[70000];
char *questname[100];
int cons_damage;
bool builder_port = FALSE;
char reasonone[30];
int arena_timelimit;
HELP_DATA *help_first;
HELP_DATA *help_last;
int desc_rooms;
SHOP_DATA *shop_first;
SHOP_DATA *shop_last;
char *score = NULL;
char *sh_score = NULL;
char *info = NULL;
int current_vnum = 0;
int mob_chunk = 0;
int room_chunk = 0;
int obj_chunk = 0;
bool fLogAll = FALSE;
TEMP_STR *temp_first = NULL;
TEMP_STR *temp_free = NULL;
BLIST *base_blist = NULL;
CHAR_DATA *auction_char[NUM_ALIGN];
CHAR_DATA *auction_tochar[NUM_ALIGN];
bool pet_flag = TRUE;
bool refresh_oldchars = FALSE;
DAM_STRUCT damt;
bool got = TRUE;
int max_connected;
int sac_vnums[30];
int sac_points[30];
/*short combat_state;*/
int ticks_to_reboot;
int ticks_to_battlefield;
long chars_sent;
int meg_sent;
bool changed_spells;
AREA_DATA *saving = NULL;

CHAR_DATA *char_free = NULL;
DESCRIPTION_DATA *extra_descr_free = NULL;
NOTE_DATA *note_free = NULL;
SINGLE_OBJECT *obj_free = NULL;
SINGLE_OBJECT *bg_prize[MAX_NUM_BG_PRIZES];
REALCHAR_DATA *pcdata_free = NULL;
MOB_DATA *npcdata_free = NULL;
char bug_buf[2 * SML_LENGTH];
CHAR_DATA *char_list;
CED_DATA *ced_first;
CED_DATA *ced_free;
SPELL_DATA *spell_list[SPELL_HASH];
SPELL_DATA *spell_list_2[SPELL_HASH];
short top_ced = 0;
short top_temp = 0;
bool state_var_1;
char *help_greeting;
char *help_greeting_a;
char log_buf[2 * SML_LENGTH];
PLAYERBASE_DATA *playerbase_zero;
NOTE_DATA *note_list;
SINGLE_OBJECT *object_list;
TIME_INFO_DATA time_info;
WEATHER_DATA weather_info;
CLAN_DATA *clan_list = NULL;
CLAN_DATA *clan_list_2 = NULL;

REALCHAR_DATA base_pc;
char condition_var[500];
int auction_minbid[NUM_ALIGN];
int auction_bid[NUM_ALIGN];
char auction_item_name[NUM_ALIGN][500];
SINGLE_OBJECT *auction_item[NUM_ALIGN];
short auction_going[NUM_ALIGN];
CHAR_DATA *auction_lastbidder[NUM_ALIGN];
int pulse_timer;
BANK_ITEM align_bank[NUM_ALIGN];
BANK_ITEM clan_bank[30];
int entlab;
short gsn_critical;
short gsn_throw;
short gsn_shoot;
short gsn_ranged;
short gsn_chop;
short gsn_build;
short gsn_enchant;
short gsn_brew;
short gsn_skin;
short gsn_tanning;
short gsn_stitch;
short gsn_weave;
short gsn_mine;
short gsn_lore;
short gsn_patch;
short gsn_forge;
short gsn_detrap;
short gsn_locate_trap;
short gsn_awareness;
short gsn_block;
short gsn_invest;
short gsn_riding;
short gsn_circle;
short gsn_penetration;
short gsn_grapple;
short gsn_swim;
short gsn_flail;
short gsn_butcher;
short gsn_warcry;
short gsn_backstab;
short gsn_dodge;
short gsn_evade;
short gsn_hide;
short gsn_peek;
short gsn_pick_lock;
short gsn_sneak;
short gsn_steal;
short gsn_bash;
short gsn_track;
short gsn_trapset;
short gsn_fireshield;
short gsn_manashield;
short gsn_disarm;
short gsn_sixth_sense;
short gsn_enhanced_damage;
short gsn_resilience;
short gsn_accuracy;
short gsn_kick;
short gsn_tornado;
short gsn_double_kick;
short gsn_smash;
short gsn_impale;
short gsn_charge;
short gsn_flurry;
short gsn_tackle;
short gsn_flash;
short gsn_parry;
short gsn_blink;
short gsn_steadiness;
short gsn_shield_block;
short gsn_rescue;
short gsn_second_attack;
short gsn_third_attack;
short gsn_dual_wield;
short gsn_pierce;
short gsn_unarmed;
short gsn_spiritregen;
short gsn_buddha;
short gsn_nerve;
short gsn_slashing;
short gsn_concussion;
short gsn_polearm;
short gsn_guard;
short gsn_heavy_slash;
short gsn_light_slash;
short gsn_whip;
short gsn_search;
char max_remort_times[NUM_ALIGN];
int bytes_saved = 0;
TOPTEN_DATA topten;
RATING_DATA ratingd;
PK_DATA pkdata[MAX_PKDATA][PKN];
BID_DATA biddata;
short gsn_blindness;
short gsn_chameleon;
short gsn_spell_reflect;
short gsn_raise_undead;
short gsn_life_tap;
short gsn_curse;
short gsn_invis;
short gsn_poison;
short gsn_plague;
short gsn_sleep;
short gsn_paralyze; /* JRAJRA */
short gsn_wound; /* JRAJRA */
short gsn_confuse; /* JRAJRA */
short gsn_lethargy; /* JRAJRA */
short gsn_mindsap; /* JRAJRA */
MOB_PROTOTYPE *mob_index_hash[HASH_MAX];
OBJ_PROTOTYPE *obj_index_hash[HASH_MAX];
ROOM_DATA *room_hash[HASH_MAX];
CLAN_DATA *clan_index_hash[20];
CLAN_DATA *clan_index_hash_2[20];
char *string_hash[HASH_MAX];
int wear_flag (int loc);
int wear_location (int loc);
int exit_flag (int loc);
char *add_newline (char *str);
char *get_name (char *str);
int room_flag (int loc);

AREA_DATA *area_first;
AREA_DATA *area_last;
char *string_space;
char *top_string;
int top_spell;
char str_empty[1];
ONLINE *been_online = NULL;

int LOG_LEVEL = LEVEL_IMMORTAL;

int top_clan = 0;
int top_clan_2 = 0;
int top_affect = 0;
int top_area = 0;
int top_ed = 0;
int top_exit = 0;
int top_help = 0;
int top_mob_index = 0;
int top_obj_index = 0;
int top_reset = 0;
int top_room = 0;
int top_shop = 0;

int top_vnum_mob = 0;
int top_vnum_obj = 0;
int top_vnum_room = 0;

int num_areas = 0;
int num_mobs = 0;
int num_objs = 0;
int num_rooms = 0;
int num_shops = 0;
int i;

int nAllocString;
int sAllocString;

bool fBootDb;
FILE *fpArea;
char strArea[SML_LENGTH];
void init_mm (void);
void load_playerbase (void);
void load_area (FILE * fp);
void load_helps (FILE * fp);
void load_mobiles (FILE * fp);
void load_objects (FILE * fp);
void load_rooms (FILE * fp);
void load_specials (FILE * fp);
void load_notes (void);
void load_clans (void);
void load_clans_2 (void);
void load_topten (void);
void load_rating (void);
void load_pkdata (void);
void load_bansites (void);
void fix_exits (void);
char *fread_and_compress_string (FILE * fp, CHAR_DATA * ch);
void reset_room (ROOM_DATA * pRoom);
void set_mob_xp (CHAR_DATA * mob);

void
cleanup_system (void)
{
#ifndef WINDOWS
  DIR *d;
  struct dirent *e;
  char *t;

  if ((d = opendir ("../plr")) == NULL)
    return;
  while ((e = readdir (d)) != NULL)
    {
      for (t = e->d_name; *t != '\0'; t++)
	{
	  if (*t == '.')
	    goto tte;
	}
      fcheck_pbase (e->d_name);
    tte:
      (void)0;
    }
#endif
  return;
}


void
area_sort (AREA_DATA * pArea)
{
  AREA_DATA *fArea;
  area_last = pArea;
  if (!area_first)
    {
      area_first = pArea;
      return;
    }

  for (fArea = area_first; fArea; fArea = fArea->next)
    {
      if (pArea->lvnum == fArea->lvnum ||
	  (pArea->lvnum > fArea->lvnum &&
	   (!fArea->next || pArea->lvnum < fArea->next->lvnum)))
	{
	  pArea->next = fArea->next;
	  fArea->next = pArea;
	  return;
	}
    }
  pArea->next = area_first;
  area_first = pArea;
  return;
}



#ifdef SLOW_BOOTUP
void
sleepy_time (void)
{
  long secDelta;
  long usecDelta;
  struct timeval stall_time;

  secDelta = 1;
  usecDelta = 0;

  stall_time.tv_usec = usecDelta;
  stall_time.tv_sec = secDelta;
  if (select (0, NULL, NULL, NULL, &stall_time) < 0)
    {
      perror ("Game_loop: select: stall");
      exit (1);
    }

  return;
}
#endif


void
set_mob_xp (CHAR_DATA * mob)
{
  mob->exp = translate (pow.mob_exp, LEVEL (mob), mob);
  if (LEVEL (mob) < 7)
    mob->exp /= 2;
  if (mob->armor < 70)
    mob->exp += LEVEL (mob) * 40;
  if (mob->armor < 0)
    mob->exp += LEVEL (mob) * 40;
  if (mob->armor < -55)
    mob->exp += LEVEL (mob) * 40;

  if (mob->pIndexData->spec_fun)
    if (mob->pIndexData->spec_fun == spec_breath_any)
      mob->exp += LEVEL (mob) * 200;
    else if (mob->pIndexData->spec_fun == spec_breath_acid
	     || mob->pIndexData->spec_fun == spec_breath_fire
	     || mob->pIndexData->spec_fun == spec_breath_frost
	     || mob->pIndexData->spec_fun == spec_breath_gas
	     || mob->pIndexData->spec_fun == spec_breath_lightning)
      mob->exp += LEVEL (mob) * 200;
    else if (mob->pIndexData->spec_fun == spec_poison)
      mob->exp += LEVEL (mob) * 300;
  if (mob->pIndexData->hpmult >= 1000)
    {
      mob->exp = (mob->exp * (mob->pIndexData->hpmult / 1000)) / 10;
    }



  if (mob->exp <= 0)
    mob->exp = 1;
  return;
}


void
save_day_counter (void)
{
  FILE *ddd;
  if ((ddd = fopen ("dayct.dat", "w")) == NULL)
    {
      fprintf (stderr, "Error in day counter write.\n");
      exit (20);
      return;
    }
  fprintf (ddd, "%ld\nDF_EOF\n", day_counter);
  fclose (ddd);
  return;
}

void
load_day_counter (void)
{
  FILE *dct;
  if ((dct = fopen ("dayct.dat", "r")) == NULL)
    {
      day_counter = 105;
      save_day_counter ();
      return;
    }
  day_counter = fread_number (dct);
  fclose (dct);
  return;
}

void
write_damages ()
{
  FILE *cr;
  if ((cr = fopen ("dam.dat", "w")) == NULL)
    {
      fprintf (stderr, "You're screwed... couldn't write a dam.dat file!!\n");
      exit (1);
    }
  fprintf (cr, "Kickd %s\n", damt.kick_dam);
  fprintf (cr, "END\n");
  fclose (cr);
  return;
}



void
reset_world (void)
{
  int iHash;
  ROOM_DATA *rid;
  for (iHash = 0; iHash < HASH_MAX; iHash++)
    {
      for (rid = room_hash[iHash]; rid != NULL; rid = rid->next)
	{
	  reset_room (rid);
	}
    }
  return;
}

void
add_to_list (char *nm, DO_COMMAND * cmd, int pos, int lev, int log_lev, char *help)
{
  COMMAND *newc;
  newc = mem_alloc (sizeof (*newc));
  newc->name = str_dup (nm);
  newc->do_fun = cmd;
  newc->position = pos;
  newc->level = lev;
  newc->log = log_lev;
  newc->help = str_dup (help);
  newc->next = command_hash[UPPER (nm[0])];
  command_hash[UPPER (nm[0])] = newc;
  return;
}

int seg_count=0;

void
segfault_handler ()
{
  DESCRIPTOR_DATA *d;
  AUCTION *auct;
 seg_count++;
  fprintf (stderr, "Last marked label was %s.\n", commandlog);
  fprintf (stderr, "Trapped segfault successfully.  This makes %d.\n",seg_count);
  
  fprintf(stderr, "Last move label was %d.\n", entlab);
  if (seg_count > 3)
    {
      fprintf(stderr, "Repeating segfault...nosave crash.\n");
      exit(5);
    }
  do_asave (char_list, "changed");
for (auct = auction_list; auct != NULL; auct = auct->next)
    {
      if (auct->item)
        {
          obj_to(auct->item, auct->bychar);
          auct->item = NULL;
        }
    }
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (!d->character || !d->character->pcdata)
        continue;
      fprintf(stderr, "%3s %20s at %5d with command %s.\n",(IS_SET(d->character->act, ACT_CRASHER) ? "-->" : "   "), NAME(d->character),
(d->character->in_room ? d->character->in_room->vnum : d->connected), (d->incomm[0] == '\0' ? d->inlast : d->incomm));
     if (d->connected != CON_PLAYING)
	continue;
      write_to_descriptor (d->descriptor, "Game Crash detected...  Saving your character's current state.\n\r", 0);
      save_char_obj (d->character);
    }
  exit(5);

  return;
}

void
boot_db (void)
{
  FILE *fttr;
  ONLINE *online_new;
  int hsh;
  struct timeval now_time;
  time_t timr;
  int i, jj;
  fBootDb = TRUE;
  cl[0] = '\0';
  online_new = (ONLINE *) mem_alloc ((sizeof (*online_new)));
  online_new->name = "Lonath";
  online_new->next = NULL;
  been_online = online_new;
	  
for (i=0;i<MT;i++) {
	average_time[i]=0;
	times_through[i]=0;
	total_time[i]=0;
	this_time[i]=0;
	temp_time[i]=0;
	}
#ifndef WINDOWS
  signal (SIGSEGV, segfault_handler);
#endif
  bzero (&outgoing, sizeof (outgoing));
  bzero (&connection_names, sizeof (connection_names));
  connectiond[0] = NULL;
  connectiond[1] = NULL;
  connectiond[2] = NULL;
  connectiond[3] = NULL;
  connectiond[4] = NULL;
  for (jj=0; jj<27; jj++) sa_head[jj]=NULL;
  bzero (&chan, sizeof (chan));
  bzero (&chan_count, sizeof (chan_count));
  reasonfd[0] = '\0';
  bzero (&command_hash, sizeof (command_hash));
  day_counter = 0;
  bzero (&questname, sizeof (questname));
  for (hsh = 0; hsh < HASH_MAX; hsh++)
    {
      obj_index_hash[hsh] = NULL;
      mob_index_hash[hsh] = NULL;
      room_hash[hsh] = NULL;
    }
  
  max_connected = 0;
  {
    jj = 0;
    for (jj = 0; jj < NUM_ALIGN; jj++)
      {
	max_remort_times[jj] = 0;
      }
  }
  for (jj = 0; jj < MAX_NUM_BG_PRIZES; jj++)
      bg_prize[jj] = NULL;
  bzero (&spell_list, sizeof (spell_list));
  bzero (&spell_list_2, sizeof (spell_list_2));
  top_spell = 0;
  /*combat_state = 0;*/
  fprintf (stderr, "Reading power.dat...\n");
  read_power_dat ();
  fprintf (stderr, "Reading races.dat...\n");
  read_race_info ();
  fprintf(stderr, "Reading align.dat...\n");
  read_align_info ();
  fprintf (stderr, "Reading channel.dat...\n");
  read_channel_data ();
  fprintf (stderr, "Loading day/year counter...\n");
  load_day_counter ();
  fprintf (stderr, "Reading dam.dat...\n");
  read_damages ();
  fprintf (stderr, "Reading triggers and script code...\n");
  load_triggers ();
  load_code ();
  fprintf (stderr, "Reading script association strings...\n");
  fprintf(stderr, "Loading original game startup time...\n");
  fread_startup_time();
  load_scriptassoc ();
  {
    int jj;
    for (jj = 0; jj < NUM_ALIGN; jj++)
      {
	fread_align_bank(jj);
      }
  }
  meg_sent = 0;
  chars_sent = 0;
  pulse_timer = 5;
  pedit_found = NULL;
  desc_rooms = 0;
  {
    int jj;
    for (jj = 0; jj  < NUM_ALIGN; jj++)
      {
	auction_minbid[jj] = 0;
	auction_bid[jj] = 0;
	auction_item_name[jj][0] = '\0';
	auction_item[jj] = NULL;
	auction_char[jj] = NULL;
	auction_tochar[jj] = NULL;
	auction_lastbidder[jj] = NULL;
	auction_going[jj] = 0;
      }
  }
  changed_spells = FALSE;
  ticks_to_reboot = 0;
  ticks_to_battlefield = 0;
  gettimeofday (&now_time, NULL);
  timr = (time_t) now_time.tv_sec;
  fprintf (stderr, "LoC began bootup proc %s", ((char *) ctime (&timr)));

  /*
   * Init random number generator.
   */
  {
    init_mm ();
  }




  /*
   * Set time and weather.
   */
  {
    long lhour, lday, lmonth;

    lhour = (current_time - 650336715)
      / (PULSE_TICK / PULSE_PER_SECOND);
    if (lhour < 24)
      lhour = 5000;
    time_info.hour = lhour % 24;
    lday = day_counter;
    time_info.day = day_counter % 35;
    lmonth = day_counter / 35;
    time_info.month = number_range(1,16);
    time_info.year = lmonth / 17;

    if (time_info.hour < 5)
      weather_info.sunlight = SUN_DARK;
    else if (time_info.hour < 6)
      weather_info.sunlight = SUN_RISE;
    else if (time_info.hour < 11)
      weather_info.sunlight = SUN_MORNING;
     else if (time_info.hour < 14)
      weather_info.sunlight = SUN_MIDDAY;
    else if (time_info.hour < 17)
      weather_info.sunlight = SUN_AFTERNOON;
     else if (time_info.hour < 20)
      weather_info.sunlight = SUN_EVENING;
    else if (time_info.hour < 20)
      weather_info.sunlight = SUN_SET;
    else
      weather_info.sunlight = SUN_DARK;

    weather_info.change = 0;
    weather_info.winddir = 0;
    if (time_info.month <= 4 || time_info.month >= 17)
      weather_info.temperature = number_fuzzy (20);
    else
      weather_info.temperature = number_fuzzy (50);
    weather_info.windspeed = number_range (1, 10);
    weather_info.mmhg = 960;
    if (time_info.month >= 7 && time_info.month <= 12)
      weather_info.mmhg += number_range (1, 50);
    else
      weather_info.mmhg += number_range (1, 80);

    if (weather_info.mmhg <= 980)
      weather_info.sky = SKY_LIGHTNING;
    else if (weather_info.mmhg <= 1000)
      weather_info.sky = SKY_RAINING;
    else if (weather_info.mmhg <= 1020)
      weather_info.sky = SKY_CLOUDY;
    else
      weather_info.sky = SKY_CLOUDLESS;

  }

  /*
   * Assign gsn's for skills which have them.
   */

  if ((fttr = fopen ("reason.fi", "r")) != NULL)
    {
      strcpy (reasonone, fread_word (fttr));
      fclose (fttr);
      unlink ("reason.fi");
    }
  else
    strcpy (reasonone, "Game-Crash");


  /*
   * Read in all the area files.
   */
  {
    FILE *fpList;
    int areanumber =0;
    if ((fpList = fopen (AREA_LIST, "r")) == NULL)
      {
	perror (AREA_LIST);
	exit (1);
      }

    for (;;)
      {
	strcpy (strArea, fread_word (fpList));
	if (strArea[0] == '$')
	  break;

	if (strArea[0] == '-')
	  {
	    fpArea = stdin;
	  }
	else
	  {
	    if ((fpArea = fopen (strArea, "r")) == NULL)
	      {
		perror (strArea);
		exit (1);
	      }
            areanumber++;
	  }
	for (;;)
	  {
	    char *word;
            fprintf(stderr, "Area %d\n", areanumber);
	    if (fread_letter (fpArea) != '#')
	      {
		bug ("Boot_db: # not found.", 0);
		exit (1);
	      }

	    word = fread_word (fpArea);

	    if (word[0] == '$')
	      break;
	    else if (!str_cmp (word, "SPECIALS"))
	      load_specials (fpArea);

	    else if (!str_cmp (word, "HELPS"))
	      {
		load_helps (fpArea);
	      }
	    else if (!str_cmp (word, "AREADATA"))
	      {
		load_area (fpArea);
#ifdef SLOW_BOOTUP
		sleepy_time ();
#endif
	      }
	    else if (!str_cmp (word, "MOBDATA"))
	      load_mobiles (fpArea);
	    else if (!str_cmp (word, "OBJDATA"))
	      load_objects (fpArea);
	    else if (!str_cmp (word, "ROOMDATA"))
	      load_rooms (fpArea);
	    else
	      {
		bug ("Bootup: $bad section name", 0);
		exit (1);
	      }
	  }

	if (fpArea != stdin)
	  fclose (fpArea);
	fpArea = NULL;
      }
    fclose (fpList);
  }

  gettimeofday (&now_time, NULL);
  timr = (time_t) now_time.tv_sec;
  fprintf (stderr, "LoC finished reading area files %s", ((char *) ctime
(&timr)));


  min_level = 0;
  max_level = 99;
  bg_multiplier = 0;
  free_arena ();
  dummy_pc ();

  fix_exits ();
  fBootDb = FALSE;
  fprintf (stderr, "Loading notes...\n");
  load_notes ();
  fprintf (stderr, "Loading clans...\n");
  load_clans ();
  fprintf (stderr, "Loading clans 2...\n");
  load_clans_2 ();
  fprintf (stderr, "Loading topten and rating and pk data...\n");
  load_topten ();
  load_rating ();
  load_pkdata ();
  fprintf (stderr, "Loading playerbase into memory...\n");
  load_playerbase ();
  fprintf (stderr, "Loading bansites...\n");
  load_bansites ();
  fprintf (stderr, "Loading score.dat, looking for sh_score.dat...\n");
  read_score (NULL, "");
  fprintf (stderr, "Loading all spells and skills...\n");
  load_spells ();
  fprintf (stderr, "Placing all commands in the hash table...\n");
  scanthru_commands ();
  fprintf (stderr, "Reading commands.dat for command level overrides...\n");
  read_command_levels ();
  fprintf (stderr, "Loading socials...\n");
  load_new_socials ();
  fprintf (stderr, "Loading alliance data...\n");
  fread_alliance_data();
  fprintf (stderr, "Performing complete world repop...\n");
  reset_world ();
  fprintf (stderr, "Checking all playerbase entries and pfiles for confirmation...\n");
  cleanup_system ();
  fBootDb = FALSE;
  fprintf (stderr, "Saved %d bytes from string compression...\n", bytes_saved);

  {
    SPELL_DATA *spp;
    int sn;
    
    gsn_spell_reflect = -1;
    gsn_chameleon = -1;
    gsn_blindness = -1;
    gsn_raise_undead = -1;
	gsn_life_tap = -1;
    gsn_curse = -1;
    gsn_invis = -1;
    gsn_poison = -1;
    gsn_plague = -1;
    gsn_sleep = -1;
    gsn_wound = -1;
    gsn_confuse = -1;
    gsn_lethargy = -1;
    gsn_mindsap = -1;
    gsn_fireshield = -1;
    gsn_manashield = -1;
    for (sn = 0; sn < SKILL_COUNT; sn++)
      {
	if ((spp = skill_lookup (NULL, sn)) == NULL)
	  continue;
	
	if (!str_prefix ("blind", spp->spell_name))
	  gsn_blindness = spp->gsn;
	if (!str_prefix ("Spell Reflect", spp->spell_name))
	  gsn_spell_reflect = spp->gsn;
	if (!str_prefix ("Fireshield", spp->spell_name))
	  gsn_fireshield = spp->gsn;
	if (!str_prefix ("Mana Shield", spp->spell_name))
	  gsn_manashield = spp->gsn;
	if (!str_prefix ("Chameleon", spp->spell_name))
	  gsn_chameleon = spp->gsn;
	if (!str_prefix ("raise", spp->spell_name))
	  gsn_raise_undead = spp->gsn;
	if (!str_prefix ("life", spp->spell_name))
	  gsn_life_tap = spp->gsn;
	if (!str_prefix ("curse", spp->spell_name))
	  gsn_curse = spp->gsn;
	if (!str_prefix ("invis", spp->spell_name))
	  gsn_invis = spp->gsn;
	if (!str_prefix ("poison", spp->spell_name))
	  gsn_poison = spp->gsn;
	if (!str_prefix ("plague", spp->spell_name))
	  gsn_plague = spp->gsn;
	if (!str_prefix ("sleep", spp->spell_name))
	  gsn_sleep = spp->gsn;
	if (!str_prefix ("wound", spp->spell_name))
	  gsn_wound = spp->gsn;
	if (!str_prefix ("confus", spp->spell_name))
	  gsn_confuse = spp->gsn;
	if (!str_prefix ("paral", spp->spell_name))
	  gsn_paralyze = spp->gsn;
	if (!str_prefix ("leth", spp->spell_name))
	  gsn_lethargy = spp->gsn;
	if (!str_prefix ("mindsap", spp->spell_name))
	  gsn_mindsap = spp->gsn;
	
      }
  }

/*
  if(gsn_manashield == -1)
    {
      fprintf(stderr,"Unable to set GSN_ for '%s'\n","Mana Shield");
    }
  if (gsn_chameleon==-1) 
    {
      fprintf(stderr,"Unable to set GSN_ for '%s'\n","chameleon");
    }
  if (gsn_blindness==-1) 
    {
      fprintf(stderr,"Unable to set GSN_ for '%s'\n","blind");
    }
  if (gsn_confuse==-1) 
    {
      fprintf(stderr,"Couldn't find spell '%s'\n","confusion");
    }
  if (gsn_wound==-1) 
    {
      fprintf(stderr,"Couldn't find spell '%s'\n","wound");
    }
  if (gsn_raise_undead ==-1) 
    {
      fprintf(stderr,"Couldn't find spell '%s'\n","raise_undead");
    }
	 if (gsn_life_tap ==-1) 
    {
      fprintf(stderr,"Couldn't find spell '%s'\n","life_tap");
    }
  if (gsn_curse ==-1) 
    {
      fprintf(stderr,"Couldn't find spell '%s'\n","curse");
    }
  if (gsn_invis ==-1) {
    
    fprintf(stderr,"Couldn't find spell '%s'\n","invis");
  }
  if (gsn_plague ==-1) 
    {
      fprintf(stderr,"Couldn't find spell '%s'\n","plague");
    }
  if (gsn_sleep ==-1) 
    {
      fprintf(stderr,"Couldn't find spell '%s'\n","sleep");
    }
  if (gsn_lethargy ==-1) 
    {
      fprintf(stderr,"Couldn't find spell '%s'\n","lethargy");
    }
  if (gsn_mindsap ==-1) 
    {
      fprintf(stderr,"Couldn't find spell '%s'\n","mindsap");
    }

*/


  gettimeofday (&now_time, NULL);
  timr = (time_t) now_time.tv_sec;
  fprintf (stderr, "EmlenMUD finished bootup initialization %s", ((char *) ctime (&timr)));
  bt_tm = (time_t) now_time.tv_sec;

  strcpy(cl,"");
#ifdef NEW_WORLD
  assign_coordinates (NULL, "");
#endif
  return;
}

#define CALCULATE_HEAD(s)	(UPPER((s)[0])-'A')
void new_assoc(char *s, char *s2) {
SA *temp;
temp=mem_alloc(sizeof(*temp));
temp->next=sa_head[CALCULATE_HEAD(s)];
sa_head[CALCULATE_HEAD(s)]=temp;
temp->flagname=str_dup(s);
temp->flagtext=s2;/*str_dup(s2);*/
return;
}

void load_scriptassoc (void) {
FILE *fp;
char assoc1[500];
if ((fp=fopen("sassoc.dat", "r")) == NULL ) {
	return;
	}
for ( ; ; ) {
strcpy(assoc1,fread_word(fp));
if (!str_cmp(assoc1,"END")) break;
new_assoc(assoc1,fread_string_eol(fp));
}
fclose(fp);
return;
}

void do_quests(CHAR_DATA *ch, char *argy) {
int i;
SA *temp;
DEFINE_COMMAND("quests",do_quests,POSITION_DEAD,0,LOG_NORMAL,"Views quest information.")
hugebuf_o[0]='\0';
sprintf(hugebuf_o,"\x1B[1;37m--- Quest Information/Status ---\x1B[0;37m\n\r");
for (i=0; i<MAX_SCRIPT_FLAGS; i++) {
	if (!ch->pcdata->script_flags[i]) continue;
	for (temp=sa_head[CALCULATE_HEAD(ch->pcdata->script_flags[i])]; temp!=NULL; temp=temp->next) {
		if (!str_cmp(temp->flagname,ch->pcdata->script_flags[i])) {
			sprintf(hugebuf_o+strlen(hugebuf_o),"%s\n\r",
				temp->flagtext);
			}
		}
	}
page_to_char(hugebuf_o,ch);
return;
}

void view_scriptassoc(CHAR_DATA *ch, char *argy) {
int i;
SA *temp;
DEFINE_COMMAND("view_associations",view_scriptassoc,POSITION_DEAD,MAX_LEVEL,LOG_NORMAL,"Views all script associations (sassoc.dat).")
hugebuf_o[0]='\0';
sprintf(hugebuf_o,"----- Script flag associations -----\n\r");
for (i=0; i<27; i++) {
	for (temp=sa_head[i]; temp!=NULL; temp=temp->next) {
		sprintf(hugebuf_o+strlen(hugebuf_o),"%s: %s\n\r",temp->flagname,temp->flagtext);
		}
	}
page_to_char(hugebuf_o,ch);
return;
}

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )    if ( !str_cmp( word, literal ) )  {field  = value; fMatch = TRUE; break;}

#define SKEY( string, field )     if ( !str_cmp( word, string ) )   {free_string( field );  field =  fread_string( fp, NULL ); fMatch = TRUE; break;}
#ifdef NEW_WORLD
#define SCKEY( string, field )     if ( !str_cmp( word, string ) )   {free_string( field );  field =  fread_and_compress_string( fp, NULL ); fMatch = TRUE; break;}
#else
/*#define SCKEY( string, field )     if ( !str_cmp( word, string ) )   {free_string( field );  field =  fread_and_compress_string( fp, NULL ); fMatch = TRUE; break;}*/
#define SCKEY( string, field )     if ( !str_cmp( word, string ) )   {free_string( field );  field =  fread_string( fp, NULL ); fMatch = TRUE; break;}
#endif

void
load_area (FILE * fp)
{
  AREA_DATA *pArea;
  char *word;
  bool fMatch;
  pArea = mem_alloc (sizeof (*pArea));
  pArea->has_popped_this_reboot = FALSE;
  pArea->open = 0;
  pArea->desc_rooms = 0;
  pArea->weather_change = 0;
  pArea->can_quit = FALSE;
  pArea->more_rainy = FALSE;
  pArea->more_dry = FALSE;
  pArea->more_fog = FALSE;
  pArea->homeland = FALSE;
  pArea->nplayer = 0;
  pArea->filename = str_dup (strArea);
  pArea->vnum = top_area;
  pArea->name = &str_empty[0];
  pArea->builders = &str_empty[0];
  pArea->repop = &str_empty[0];
  pArea->security = 1;
  pArea->clanzone = FALSE;
  pArea->lvnum = 0;
  pArea->uvnum = 0;
  pArea->area_flags = 0;
  pArea->room_count = 0;
  pArea->mob_count = 0;
  pArea->obj_count = 0;
  pArea->repop_rate = 55;
  pArea->repop_counter = number_range (30, 55);

  for (;;)
    {
      word = feof (fp) ? "End" : fread_word (fp);
      fMatch = FALSE;

      switch (UPPER (word[0]))
	{
	case 'N':
	  SKEY ("Name", pArea->name);
	  KEY ("NQuit", pArea->can_quit, fread_number (fp));
	  break;
	case 'S':
	  KEY ("Security", pArea->security, fread_number (fp));
	  KEY ("SRepop", pArea->repop_rate, fread_number (fp));
	
	  KEY ("SCZone", pArea->clanzone, fread_number (fp));
	 
	  break;
	case 'H':
	  KEY("Homeland", pArea->homeland, fread_number(fp));
	  break;
	case 'V': 
	  if (!str_cmp (word, "VNUMs"))
	    {
	      pArea->lvnum = fread_number (fp);
	      pArea->uvnum = fread_number (fp);
	    }
	  break;
	case 'E':
	  if (!str_cmp (word, "End"))
	    {
	      fMatch = TRUE;
	      area_sort (pArea);
	      top_area++;
	      if (pArea->open)
		num_areas++;
	      pArea->repop_counter = number_range (1, pArea->repop_rate);
	      return;
	    }
	  break;
	case 'B':
	  SKEY ("Builders", pArea->builders);
	  break;
	case 'O':
	  KEY ("Open", pArea->open, fread_number (fp));
	  break;
	case 'R':
	  KEY ("Rcount", pArea->room_count, fread_number (fp));
	  SKEY ("Repop", pArea->repop);
	  if (!str_cmp (word, "Reset"))
	    {
	      fread_to_eol (fp);
	      fMatch = TRUE;
	    }
	  break;
	}
    }
  pArea->repop_counter = number_range (1, pArea->repop_rate);
  return;
}

void
load_helps (FILE * fp)
{
  HELP_DATA *pHelp;
  for (;;)
    {
      pHelp = mem_alloc (sizeof (*pHelp));
      pHelp->level = fread_number (fp);
      pHelp->keyword = fread_string (fp, NULL);

      if (pHelp->keyword[0] == '$')
	break;
#ifdef NEW_WORLD
      strcpy (hugebuf_o, fread_string2 (fp, NULL));
      i = strlen (hugebuf_o);
      pHelp->text = str_dup (compress (hugebuf_o, i));
      bytes_saved += i - strlen (pHelp->text);
#else
      pHelp->text = fread_string (fp, NULL);
#endif

      if (!str_cmp (pHelp->keyword, "greeting"))
	help_greeting = pHelp->text;
      if (!str_cmp (pHelp->keyword, "greeting_raw"))
	help_greeting_a = pHelp->text;
      if (help_first == NULL)
	help_first = pHelp;
      if (help_last != NULL)
	help_last->next = pHelp;

      help_last = pHelp;
      pHelp->next = NULL;
      top_help++;
    }
  if (!help_greeting_a)
    help_greeting_a = help_greeting;
  return;
}

SOCIAL *social_list;
SOCIAL *social_hashed[255];

void
save_new_socials (void)
{
  FILE *fp;
  SOCIAL *s;
  if ((fp = fopen ("socials.dat", "w+")) == NULL)
    {
      fprintf (stderr, "Error on social write.\n\r");
      return;
    }
  for (s = social_list; s != NULL; s = s->next)
    {
      fprintf (fp, "SocialName  %s~\n", s->name);
      fprintf (fp, "CharNoArg   %s~\n", (s->char_no_arg ? s->char_no_arg : "none"));
      fprintf (fp, "OtherNoArg  %s~\n", (s->others_no_arg ? s->others_no_arg : "none"));
      fprintf (fp, "CharFound   %s~\n", (s->char_found ? s->char_found : "none"));
      fprintf (fp, "OtherFound  %s~\n", (s->others_found ? s->others_found : "none"));
      fprintf (fp, "VictFound   %s~\n", (s->vict_found ? s->vict_found : "none"));
      fprintf (fp, "CharNFound  %s~\n", (s->char_not_found ? s->char_not_found : "none"));
      fprintf (fp, "CharSelf    %s~\n", (s->char_auto ? s->char_auto : "none"));
      fprintf (fp, "OtherSelf   %s~\n\n", (s->others_auto ? s->others_auto : "none"));
    }
  fprintf (fp, "END\n");
  fclose (fp);
  return;
}

void
social_update_hash (void)
{
  SOCIAL *s;
  bzero (&social_hashed, sizeof (social_hashed));
  for (s = social_list; s != NULL; s = s->next)
    {
      s->next_hashed = social_hashed[UPPER (s->name[0])];
      social_hashed[UPPER (s->name[0])] = s;
    }
  return;
}

SOCIAL *
new_social (void)
{
  SOCIAL *soc;
  soc = mem_alloc (sizeof (*soc));
  bzero (soc, sizeof (*soc));
  soc->next = social_list;
  social_list = soc;
  return soc;
}

void
load_new_socials (void)
{
  FILE *fp;
  SOCIAL *s;
  char tempbuf[1500];

  if ((fp = fopen ("socials.dat", "r")) == NULL)
    {
      fprintf (stderr, "Error on social read!\n");
      return;
    }

  for (;;)
    {
      strcpy (tempbuf, fread_word (fp));
      if (!str_cmp (tempbuf, "END"))
	break;
      if (!str_cmp (tempbuf, "SocialName"))
	{
	  s = new_social ();
	  strcpy (s->name, fread_string2 (fp, NULL));
	  continue;
	}
      if (!str_cmp (tempbuf, "CharNoArg"))
	{
	  strcpy (tempbuf, fread_string2 (fp, NULL));
	  if (!str_cmp (tempbuf, "none"))
	    continue;
	  s->char_no_arg = str_dup (tempbuf);
	  continue;
	}
      if (!str_cmp (tempbuf, "OtherNoArg"))
	{
	  strcpy (tempbuf, fread_string2 (fp, NULL));
	  if (!str_cmp (tempbuf, "none"))
	    continue;
	  s->others_no_arg = str_dup (tempbuf);
	  continue;
	}
      if (!str_cmp (tempbuf, "CharFound"))
	{
	  strcpy (tempbuf, fread_string2 (fp, NULL));
	  if (!str_cmp (tempbuf, "none"))
	    continue;
	  s->char_found = str_dup (tempbuf);
	  continue;
	}
      if (!str_cmp (tempbuf, "OtherFound"))
	{
	  strcpy (tempbuf, fread_string2 (fp, NULL));
	  if (!str_cmp (tempbuf, "none"))
	    continue;
	  s->others_found = str_dup (tempbuf);
	  continue;
	}
      if (!str_cmp (tempbuf, "VictFound"))
	{
	  strcpy (tempbuf, fread_string2 (fp, NULL));
	  if (!str_cmp (tempbuf, "none"))
	    continue;
	  s->vict_found = str_dup (tempbuf);
	  continue;
	}
      if (!str_cmp (tempbuf, "CharNFound"))
	{
	  strcpy (tempbuf, fread_string2 (fp, NULL));
	  if (!str_cmp (tempbuf, "none"))
	    continue;
	  s->char_not_found = str_dup (tempbuf);
	  continue;
	}
      if (!str_cmp (tempbuf, "CharSelf"))
	{
	  strcpy (tempbuf, fread_string2 (fp, NULL));
	  if (!str_cmp (tempbuf, "none"))
	    continue;
	  s->char_auto = str_dup (tempbuf);
	  continue;
	}
      if (!str_cmp (tempbuf, "OtherSelf"))
	{
	  strcpy (tempbuf, fread_string2 (fp, NULL));
	  if (!str_cmp (tempbuf, "none"))
	    continue;
	  s->others_auto = str_dup (tempbuf);
	  continue;
	}
    }
  social_update_hash ();
  fclose (fp);
}

void
fread_clan (FILE * fp, int vnum)
{
  CLAN_DATA *clan;
  char *word;
  bool fMatch;
  int iHash;
  int iAttack;

  clan = mem_alloc (sizeof (*clan));
  clan->vnum = vnum;
  clan->alignment = 0;
  clan->leader[0] = '\0';
  clan->name[0] = '\0';
  clan->minlev = 0;
  clan->start_vnum = 0;
  clan->shitlist = FALSE;
  clan_bank[vnum].gold = 0;
  clan_bank[vnum].warpoints = 0;
  for (iAttack = 0; iAttack < 100; iAttack++)
    clan->members[iAttack] = NULL;

  for (;;)
    {
      word = feof (fp) ? "End" : fread_word (fp);
      fMatch = FALSE;

      switch (UPPER (word[0]))
	{
	case '*':
	  fread_to_eol (fp);
	  break;
	case 'E':
	  if (!str_cmp (word, "End"))
	    {
	      iHash = vnum % 20;
	      clan->next = clan_index_hash[iHash];
	      clan_index_hash[iHash] = clan;
	      top_clan++;
	      return;
	    }
	  break;
	case 'A':
	  KEY ("Alignment", clan->alignment, fread_number (fp));
	  break;
	case 'C':
	  if (!str_cmp (word, "CMember"))
	    {
	      int ii;
	      ii = clan_free (clan);
	      clan->members[ii] = str_dup (fread_word (fp));
	      fMatch = TRUE;
	    }
	  break;
	case 'L':
	  if (!str_cmp ("Leader", word))
	    {
	      strcpy (clan->leader, fread_word (fp));
	      fMatch = TRUE;
	    }
	  break;
	case 'M':
	  KEY ("MStartV", clan->start_vnum, fread_number (fp));
	  KEY ("Minlev", clan->minlev, fread_number (fp));
	  break;
	case 'N':
	  if (!str_cmp ("Name", word))
	    {
	      strcpy (clan->name, fread_string (fp, NULL));
	      fMatch = TRUE;
	    }
	  break;
	case 'S':
	  KEY("ShitList", clan->shitlist, fread_number(fp));
	  break;
	}
      if (!fMatch)
	{
	  char buf[80];
	  sprintf (buf, "fread_clan: %d incorrect: %s",
		   vnum, word);
	  bug (buf, 0);
	  fread_to_eol (fp);
	}

    };
  return;
}

void
fread_clan_2 (FILE * fp, int vnum)
{
  CLAN_DATA *clan;
  char *word;
  bool fMatch;
  int iHash;
  int iAttack;

  clan = mem_alloc (sizeof (*clan));
  clan->vnum = vnum;
  clan->alignment = 0;
  clan->leader[0] = '\0';
  clan->name[0] = '\0';
  clan->minlev = 0;
  clan->start_vnum = 0;
  for (iAttack = 0; iAttack < 100; iAttack++)
    clan->members[iAttack] = NULL;

  for (;;)
    {
      word = feof (fp) ? "End" : fread_word (fp);
      fMatch = FALSE;

      switch (UPPER (word[0]))
	{
	case '*':
	  fread_to_eol (fp);
	  break;
	case 'E':
	  if (!str_cmp (word, "End"))
	    {
	      iHash = vnum % 20;
	      clan->next = clan_index_hash_2[iHash];
	      clan_index_hash_2[iHash] = clan;
	      top_clan_2++;
	      return;
	    }
	  break;
	case 'A':
	  KEY ("Alignment", clan->alignment, fread_number (fp));
	  break;
	case 'C':
	  if (!str_cmp (word, "CMember"))
	    {
	      int ii;
	      ii = clan_free (clan);
	      clan->members[ii] = str_dup (fread_word (fp));
	      fMatch = TRUE;
	    }
	  break;
	case 'L':
	  if (!str_cmp ("Leader", word))
	    {
	      strcpy (clan->leader, fread_word (fp));
	      fMatch = TRUE;
	    }
	  break;
	case 'M':
	  KEY ("MStartV", clan->start_vnum, fread_number (fp));
	  KEY ("Minlev", clan->minlev, fread_number (fp));
	  break;
	case 'N':
	  if (!str_cmp ("Name", word))
	    {
	      strcpy (clan->name, fread_string (fp, NULL));
	      fMatch = TRUE;
	    }
	  break;

	}
      if (!fMatch)
	{
	  char buf[80];
	  sprintf (buf, "fread_clan_2: %d incorrect: %s",
		   vnum, word);
	  bug (buf, 0);
	  fread_to_eol (fp);
	}

    };
  return;
}



void
fread_mobile (FILE * fp, int vnum)
{
  MOB_PROTOTYPE *pMobIndex;
  char *word;
  bool fMatch;
  int iHash;

  pMobIndex = mem_alloc (sizeof (*pMobIndex));
  pMobIndex->points=0;
  pMobIndex->vnum = vnum;
  pMobIndex->area = area_last;
  pMobIndex->attackname = NULL;
  pMobIndex->name = NULL;
  pMobIndex->short_descr = NULL;
  pMobIndex->long_descr = NULL;
  pMobIndex->description = NULL;
  pMobIndex->mobtype = -1;
  pMobIndex->clan_guard_1 = 0;
  pMobIndex->clan_guard_2 = 0;
  pMobIndex->hpmult = 10;
  pMobIndex->yeller_number = 0;
  pMobIndex->max_in_world = 5;
  pMobIndex->height = -1;
  pMobIndex->will_help = 0;
  pMobIndex->guard = -1;
  pMobIndex->act = 0;
  pMobIndex->act3 = 0;
  pMobIndex->affected_by = 0;
  pMobIndex->more_affected_by = 0;
  pMobIndex->alignment = 0;
  pMobIndex->money = 0;
  pMobIndex->sex = 0;
  pMobIndex->act4 = 0;
  pMobIndex->armcls = 101;
  pMobIndex->race_hate = 0;
  pMobIndex->alt_vnum = 0;
  pMobIndex->guard = -1;
  pMobIndex->opt = NULL;
  pMobIndex->hitroll = 0;
  pMobIndex->damroll = 0;
  pMobIndex->dodge = 0;
  pMobIndex->parry = 0;

  for (;;)
    {
      word = feof (fp) ? "End" : fread_word (fp);
      fMatch = FALSE;

      switch (UPPER (word[0]))
	{
	case '*':
	  fread_to_eol (fp);
	  break;
	case 'E':
	  if (!str_cmp (word, "End"))
	    {
	      iHash = vnum % HASH_MAX;
	      pMobIndex->next = mob_index_hash[iHash];
	      mob_index_hash[iHash] = pMobIndex;
	      top_mob_index++;
	      if (!pMobIndex->name)
		pMobIndex->name = str_dup ("(no name)");
	      if (!pMobIndex->short_descr)
		pMobIndex->short_descr = str_dup ("(no short desc)");
	      if (!pMobIndex->long_descr)
		pMobIndex->long_descr = str_dup ("(no long desc)\n\r");
	      if (!pMobIndex->description)
		pMobIndex->description = str_dup ("(no description)");
	      return;
	    }
	  break;
	case 'A':
	  SKEY ("Attackmsg", pMobIndex->attackname);
	  KEY ("AClan2", pMobIndex->clan_guard_2, fread_number (fp));
	  KEY ("AClan", pMobIndex->clan_guard_1, fread_number (fp));
	  KEY ("ActBits", pMobIndex->act, fread_number (fp));
	  KEY ("ActBits3", pMobIndex->act3, fread_number (fp));
	  KEY ("AYeller", pMobIndex->yeller_number, fread_number (fp));
	  KEY ("AMax", pMobIndex->max_in_world, fread_number (fp));
	  KEY ("AWillhelp", pMobIndex->will_help, fread_number (fp));
	  KEY ("ActBits4", pMobIndex->act4, fread_number (fp));
	  KEY ("Affect_By", pMobIndex->affected_by, fread_number (fp));
	  KEY ("AffectTWO", pMobIndex->more_affected_by, fread_number (fp));
	  if (!str_cmp(word, "Alignment"))
	    {
	      pMobIndex->alignment = fread_number(fp);
	      if (pMobIndex->alignment > NUM_ALIGN) pMobIndex->alignment = 2;
	      if (pMobIndex->alignment < 0) pMobIndex->alignment = 1;
              fMatch = TRUE;
	    }
	  KEY ("Alt_vnum", pMobIndex->alt_vnum, fread_number (fp));
          if (!str_cmp (word, "ArmorClass"))
	    {
	      fMatch = TRUE;
	      pMobIndex->armcls = fread_number (fp);
	    }
	  if (!str_cmp (word, "Avngr"))
	    {
	      int num;
	      fMatch = TRUE;
	      if (!pMobIndex->opt)
		pMobIndex->opt = new_optional ();
	      num = fread_number (fp);
	      pMobIndex->opt->avenger = num;
	    }
	  if (!str_cmp (word, "ASklTaught"))
	    {
	      int num;
	      int num2;
	      fMatch = TRUE;
	      if (!pMobIndex->opt)
		pMobIndex->opt = new_optional ();
	      num = fread_number (fp);
	      num2 = fread_number (fp);
	      if (num != 30)
		pMobIndex->opt->skltaught[num] = num2;

	    }
	  break;
	case 'B':
	  if (!str_cmp (word, "BCasts"))
	    {
	      int num = 0;
	      fMatch = TRUE;
	      if (!pMobIndex->opt)
		pMobIndex->opt = new_optional ();
	      while (pMobIndex->opt->cast_spells[num] != NULL)
		num++;
	      pMobIndex->opt->cast_spells[num] = fread_string (fp, NULL);
	    }
	  if (!str_cmp (word, "BCast_P"))
	    {
	      fMatch = TRUE;
	      if (!pMobIndex->opt)
		pMobIndex->opt = new_optional ();
	      pMobIndex->opt->cast_percent = fread_number (fp);
	    }
	  if (!str_cmp (word, "BWarrior_P"))
	    {
	      fMatch = TRUE;
	      if (!pMobIndex->opt)
		pMobIndex->opt = new_optional ();
	      pMobIndex->opt->warrior_percent = fread_number (fp);
	    }
	  break;
	case 'G':
	  KEY ("Guard", pMobIndex->guard, fread_number (fp));
	  KEY ("GHitR", pMobIndex->hitroll, fread_number (fp));
	  KEY ("GDamR", pMobIndex->damroll, fread_number (fp));
	  KEY ("GDod", pMobIndex->dodge, fread_number (fp));
	  KEY ("GPar", pMobIndex->parry, fread_number (fp));
	  break;
	case 'H':
	  KEY ("Hp", pMobIndex->hpmult, fread_number (fp));
	  break;
	case 'J':
	  KEY ("JPts", pMobIndex->points, fread_number(fp));
	  break;
	case 'M':
	  KEY ("MobType", pMobIndex->mobtype, fread_number (fp));
	  KEY ("Money", pMobIndex->money, fread_number (fp));
	  break;
	case 'N':
	  KEY ("NHeight", pMobIndex->height, fread_number (fp));
	  SKEY ("Name", pMobIndex->name);
	  break;
	case 'R':
	  KEY ("RaceHate", pMobIndex->race_hate, fread_number (fp));
	case 'S':
	  KEY ("Sex", pMobIndex->sex, fread_number (fp));
	  SKEY ("Short", pMobIndex->short_descr);
          if (!str_cmp (word, "Spec"))
	    {
	      pMobIndex->spec_fun = NULL;
	      pMobIndex->spec_name = str_dup (fread_word (fp));
	      pMobIndex->spec_fun = spec_lookup (pMobIndex->spec_name);
	      fMatch = TRUE;
	    }
	  
	  KEY ("Shopd2", pMobIndex->pShop->decrease, fread_number (fp));
	  KEY ("Shopd3", pMobIndex->pShop->items_until_decrease, fread_number (fp));

	  KEY ("ShopCr", pMobIndex->pShop->creates_vnum, fread_number (fp));
	  KEY ("ShCrh", pMobIndex->pShop->creates_hours, fread_number (fp));
	  SKEY ("ShopCS", pMobIndex->pShop->creates_message);


	  if (!str_cmp (word, "Shop"))
	    {
	      SHOP_DATA *pShop;
	      int iTrade;

	      pShop = mem_alloc (sizeof (*pShop));

	      pShop->keeper = vnum;

	      for (iTrade = 0; iTrade < MAX_TRADE; iTrade++)
		pShop->buy_type[iTrade] = fread_number (fp);

	      pShop->profit_buy = fread_number (fp);
	      pShop->profit_sell = fread_number (fp);
	      pShop->creates_hours = 1;
	      pShop->open_hour = fread_number (fp);
	      pShop->close_hour = fread_number (fp);
	      pShop->decrease = 5;
	      pShop->items_until_decrease = 4;
	      pShop->creates_vnum = 0;
	      pShop->creates_message = NULL;
	      pMobIndex->pShop = pShop;

	      if (shop_first == NULL)
		shop_first = pShop;
	      if (shop_last != NULL)
		shop_last->next = pShop;

	      shop_last = pShop;
	      pShop->next = NULL;
	      top_shop++;
	      fMatch = TRUE;
	    }
	  if (!str_cmp (word, "Smns"))
	    {
	      int num;
	      fMatch = TRUE;
	      if (!pMobIndex->opt)
		pMobIndex->opt = new_optional ();
	      num = fread_number (fp);
	      pMobIndex->opt->summons = num;
	    }
	  
	  break;
	case 'L':
	  SKEY ("Long", pMobIndex->long_descr);
	  KEY ("Level", pMobIndex->level, fread_number (fp));
	  break;
	case 'D':
	  SCKEY ("Descr", pMobIndex->description);
	  break;
	}
      if (!fMatch)
	{
	  char buf[80];
	  sprintf (buf, "fread_mobile: %d incorrect: %s",
		   vnum, word);
	  bug (buf, 0);
	  fread_to_eol (fp);
	}
    };
  return;
}


void
load_mobiles (FILE * fp)
{
  area_last->mob_count = 0;
  for (;;)
    {
      int vnum;
      char letter;

      letter = fread_letter (fp);
      if (letter != '#')
	{
	  bug ("Load_mobiles: '#' not found.", 0);
	  exit (1);
	}

      vnum = fread_number (fp);
      if (vnum == 0)
	{
	  /*bug( "found vnum zero", 0 ); */
	  break;
	}
      fBootDb = FALSE;
      if (get_mob_index (vnum) != NULL)
	{
	  bug ("Load_mobiles: vnum %d duplicated.", vnum);
	  continue;
	}
      fBootDb = TRUE;

      fread_mobile (fp, vnum);
      area_last->mob_count++;
      if (area_last->open)
	num_mobs++;
      top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;
    }
  return;
}

void
load_clans (void)
{
  FILE *fp;
  if ((fp = fopen ("pclans.dat", "r")) == NULL)
    return;

  for (;;)
    {
      int vnum;
      char letter;

      letter = fread_letter (fp);
      if (letter != '#')
	{
	  bug ("Load_clan: '#' not found.", 0);
	  exit (1);
	}

      vnum = fread_number (fp);
      if (vnum == 0)
	{
	  break;
	}
      fBootDb = FALSE;
      if (get_clan_index (vnum) != NULL)
	{
	  bug ("Load_clans: vnum %d duplicated.", vnum);
	  continue;
	}
      fBootDb = TRUE;

      fread_clan (fp, vnum);
      fread_clan_bank (vnum);
      if (vnum > 0)
	fread_clan_storage (vnum);
      top_clan = top_clan < vnum ? vnum : top_clan;
    }
  fclose (fp);
  return;
}

void
load_clans_2 (void)
{
  FILE *fp;
  if ((fp = fopen ("pclans2.dat", "r")) == NULL)
    return;

  for (;;)
    {
      int vnum;
      char letter;

      letter = fread_letter (fp);
      if (letter != '#')
	{
	  bug ("Load_clan_2: '#' not found.", 0);
	  exit (1);
	}

      vnum = fread_number (fp);
      if (vnum == 0)
	{
	  break;
	}
      fBootDb = FALSE;
      if (get_clan_index_2 (vnum) != NULL)
	{
	  bug ("Load_clans_2: vnum %d duplicated.", vnum);
	  continue;
	}
      fBootDb = TRUE;

      fread_clan_2 (fp, vnum);
      top_clan_2 = top_clan_2 < vnum ? vnum : top_clan_2;
    }
  fclose (fp);
  return;
}



void
load_topten (void)
{
  FILE *fp;
  int k;
  if ((fp = fopen ("topten.dat", "r")) == NULL)
    {
      for (k = 0; k < MAX_TOPTEN; k++)
	{
	  strcpy (topten.topten_name[k], "Free-Slot");
	  topten.topten_warpoints[k] = 0;
	  topten.topten_alignment[k] = TRUE;
	}
      save_topten ();
      return;
    }

  for (k = 0; k < MAX_TOPTEN; k++)
    {
      strcpy (topten.topten_name[k], fread_word (fp));
      topten.topten_warpoints[k] = fread_number (fp);
      topten.topten_alignment[k] = fread_number (fp);
    }

  fclose (fp);
  return;
}


void
load_pkdata (void)
{
  FILE *fp;
  int k, i;
  if ((fp = fopen ("pkdata.dat", "r")) == NULL)
    {
      for (k = 0; k < MAX_PKDATA; k++)
	{
	  for (i=0; i < PKN; i++)
	    {
	      strcpy (pkdata[k][i].name, "Free-Slot");
	      pkdata[k][i].value = 0.0;
	      pkdata[k][i].alignment = TRUE;
	    }
	}
      save_pkdata ();
      return;
    }

for (k = 0; k < MAX_PKDATA; k++)
	{
	  for (i=0; i < PKN; i++)
	    { 
              int tempval;
	      strcpy (pkdata[k][i].name, fread_word (fp));
	      tempval = fread_number (fp);
	      pkdata[k][i].value = (float) ((float) tempval/ (float) 10000);
              pkdata[k][i].alignment = fread_number (fp);
	    }
	}
  fclose (fp);
  return;
}





void
load_rating (void)
{
  FILE *fp;
  int k;
  if ((fp = fopen ("rating.dat", "r")) == NULL)
    {
      for (k = 0; k < MAX_RATING; k++)
	{
	  strcpy (ratingd.rating_name[k], "Free-Slot");
	  ratingd.rating_rating[k] = 0;
	  ratingd.rating_alignment[k] = TRUE;
	}
      save_rating ();
      return;
    }

  for (k = 0; k < MAX_RATING; k++)
    {
      strcpy (ratingd.rating_name[k], fread_word (fp));
      ratingd.rating_rating[k] = fread_number (fp);
      ratingd.rating_alignment[k] = fread_number (fp);
    }

  fclose (fp);
  return;
}



void
load_bansites (void)
{
  FILE *fp;
  bool newb;
  char templine[500];
  if ((fp = fopen ("bansite.dat", "r")) == NULL)
    {
      return;
    }
  strcpy (templine, "NOEND");
  while (str_cmp (templine, "END"))
    {
      strcpy (templine, fread_word (fp));
      if (str_cmp (templine, "END"))
	{
	  newb = fread_number (fp);
	  if (!newb)
	    bansite (templine);
	  else
	    nbansite (templine);
	}
    }

  fclose (fp);
  return;
}


void
fix_object (OBJ_PROTOTYPE * pObjIndex)
{
  return;
}


void
fread_object (FILE * fp, int vnum)
{
  OBJ_PROTOTYPE *pObjIndex;
  int iHash;
  char *word;
  bool fMatch;
  int tmpi;

  pObjIndex = mem_alloc (sizeof (*pObjIndex));
  pObjIndex->made_of=0;
  pObjIndex->max_in_room = 1;
  pObjIndex->vnum = vnum;
  pObjIndex->timer = 0;
  pObjIndex->area = area_last;
  pObjIndex->name = &str_empty[0];
  pObjIndex->short_descr = &str_empty[0];
  pObjIndex->description = &str_empty[0];
  pObjIndex->extra_flags = 0;
  pObjIndex->guild_flags = 0;
  for (tmpi = 0; tmpi < 11; tmpi++)
    pObjIndex->value[tmpi] = 0;
 

  for (;;)
    {
      word = feof (fp) ? "End" : fread_word (fp);
      fMatch = FALSE;

      switch (UPPER (word[0]))
	{
	case '*':
	  fread_to_eol (fp);
	  break;
	case 'E':
	  KEY ("Extra", pObjIndex->extra_flags, fread_number (fp));
	  if (!str_cmp (word, "ExtraDescr"))
	    {
	      DESCRIPTION_DATA *ed;

	      ed = mem_alloc (sizeof (*ed));
	      ed->keyword = fread_string (fp, NULL);
	      ed->description = fread_string (fp, NULL);
	      ed->next = pObjIndex->extra_descr;
	      pObjIndex->extra_descr = ed;
	      top_ed++;
	      fMatch = TRUE;
	    }
	  if (!str_cmp (word, "End"))
	    {
	      iHash = vnum % HASH_MAX;
	      pObjIndex->next = obj_index_hash[iHash];
	      obj_index_hash[iHash] = pObjIndex;
	      top_obj_index++;
	      fix_object (pObjIndex);
	      return;
	    }
	  break;
	case 'A':
	  if (!str_cmp (word, "Affect"))
	    {
	      AFFECT_DATA *paf;

	      paf = mem_alloc (sizeof (*paf));
	      paf->location = fread_number (fp);
	      paf->modifier = fread_number (fp);
	      paf->type = fread_number (fp);
	      paf->duration = fread_number (fp);
	      paf->bitvector = fread_number (fp);
	      paf->next = pObjIndex->affected;
	      pObjIndex->affected = paf;
	      top_affect++;
	      fMatch = TRUE;
	    }
	  break;
	case 'D':
           KEY ("DMat", pObjIndex->made_of, fread_number(fp));
	  SKEY ("Descr", pObjIndex->description);
	  break;
	
	case 'G':
	  KEY ("Guild", pObjIndex->guild_flags, fread_number (fp));
	  break;
	
	case 'T':
	  KEY ("Timer", pObjIndex->timer, fread_number (fp));
	  KEY ("Type", pObjIndex->item_type, fread_number (fp));
	  break;
	case 'M':
	  KEY ("Maxinroom", pObjIndex->max_in_room, fread_number (fp));
	  break;
	case 'N':
	  SKEY ("Name", pObjIndex->name);
	  break;
	case 'S':
	  SKEY ("Short", pObjIndex->short_descr);
	  break;
	case 'W':
	  KEY ("Wear", pObjIndex->wear_flags, fread_number (fp));
	  KEY ("Weight", pObjIndex->weight, fread_number (fp));
	  break;
	case 'V':
	  if (!str_cmp (word, "Values"))
	    {
	      fMatch = TRUE;
	      for (iHash = 0; iHash < 11; iHash++)
		{
		  pObjIndex->value[iHash] = fread_number (fp);
		};
	    }
	  

	  break;
	case 'C':
	  KEY ("Cost", pObjIndex->cost, fread_number (fp));
	  break;

	}
      if (!fMatch)
	{
	  sprintf (log_buf, "fread_object: vnum %d incorrect:  '%s'",
		   vnum, word);
	  bug (log_buf, 0);
	  fread_to_eol (fp);
	}
    };
  if (pObjIndex->item_type == ITEM_ARMOR && pObjIndex->weight < 11)
    {
      if (pObjIndex->weight == 0) pObjIndex->weight++;
      pObjIndex->weight *= 10;
      }
  if (pObjIndex->item_type == ITEM_WEAPON && pObjIndex->weight < 11)
    {
      pObjIndex->weight *= 10;
      }
 
  return;
}

DOOR_DATA *
new_door (void)
{
  DOOR_DATA *ped;
  ped = mem_alloc (sizeof (*ped));
  ped->key = 0;
  ped->exit_info = 0;
  ped->rs_flags = 0;
  ped->str = 30;
  ped->maxstr = 30;
  ped->keyword = &str_empty[0];
  ped->description = &str_empty[0];
  return ped;
}


void
load_objects (FILE * fp)
{
  area_last->obj_count = 0;
  for (;;)
    {
      int vnum;
      char letter;

      letter = fread_letter (fp);
      if (letter != '#')
	{
	  bug ("Load_objects: # not found.", 0);
	  exit (1);
	}

      vnum = fread_number (fp);
      if (vnum == 0)
	break;

      fBootDb = FALSE;
      if (get_obj_index (vnum) != NULL)
	{
	  bug ("Load_objects: vnum %d duplicated.", vnum);
	  continue;
	}
      fBootDb = TRUE;

      fread_object (fp, vnum);
    
      area_last->obj_count++;
      if (area_last->open)
	num_objs++;
      top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;
    }

  return;
}

void
fread_room (FILE * fp, int vnum)
{
  ROOM_DATA *oneroom;
  int iHash;
  int door;
  char *word;
  bool fMatch;

  oneroom = new_room ();
  oneroom->area = area_last;
  oneroom->vnum = vnum;
  for (;;)
    {
      word = feof (fp) ? "End" : fread_word (fp);
      fMatch = FALSE;

      switch (UPPER (word[0]))
	{
	case '*':
	  fread_to_eol (fp);
	  break;
	case 'C':
	  KEY ("Cvnumh", oneroom->cvnumh, fread_number (fp));
	  KEY ("Cvnuml", oneroom->cvnuml, fread_number (fp));
	  KEY ("Calign", oneroom->calign, fread_number (fp));
	  KEY ("Cbonusstr", oneroom->cbonusstr, fread_string (fp, NULL));
	  break;
	case 'E':
	  if (!str_cmp (word, "End"))
	    {

	      if (DESCRIPTED (oneroom))
		{
		  desc_rooms++;
		  oneroom->area->desc_rooms++;
		}
	      iHash = vnum % HASH_MAX;
	      oneroom->next = room_hash[iHash];
	      room_hash[iHash] = oneroom;
	      top_room++;
	      return;
	    }
	  if (!str_cmp (word, "ExtraDescr"))
	    {
	      DESCRIPTION_DATA *ed;
	      check_room_more (oneroom);
	      ed = mem_alloc (sizeof (*ed));
	      ed->keyword = fread_string (fp, NULL);
	      ed->description = fread_string (fp, NULL);
	      ed->next = oneroom->more->extra_descr;
	      oneroom->more->extra_descr = ed;
	      top_ed++;
	      fMatch = TRUE;
	    }
	  break;
	case 'R':
	  if (!str_cmp (word, "Reset"))
	    {
	      RESET_DATA *pReset;
	      check_room_more (oneroom);
	      pReset = mem_alloc (sizeof (*pReset));
	      pReset->command = fread_letter (fp);
	      pReset->rs_vnum = fread_number (fp);
	      pReset->loc = fread_number (fp);
	      pReset->percent = fread_number (fp);
	      fread_to_eol (fp);

	      if (oneroom->more->reset_first == NULL)
		oneroom->more->reset_first = pReset;
	      if (oneroom->more->reset_last != NULL)
		oneroom->more->reset_last->next = pReset;

	      oneroom->more->reset_last = pReset;
	      pReset->next = NULL;
	      top_reset++;
	      fMatch = TRUE;
	    }
	  break;
	case 'D':
	  /* Might want descriptions NOT to be compressed... */
	  SCKEY ("Descr", oneroom->description);
	  if (!str_cmp (word, "Door"))
	    {
	      EXIT_DATA *pexit;
	      
              char tmpp[5000];

	      int tmp;
	      door = fread_number (fp);

	      if (door < 0 || door > 5)
		{
		  bug ("Fread_rooms: vnum %d has bad door number.", vnum);
		  exit (1);
		}

	      pexit = new_exit ();
	      pexit->d_info = NULL;
	      tmp = fread_number (fp);
	      if (tmp != 0)
		{
		  if (pexit->d_info == NULL)
		    {
		      pexit->d_info = new_door ();
		    }
		  pexit->d_info->rs_flags = tmp;
		}
	      tmp = fread_number (fp);
	      if (tmp != 0)
		{
		  if (pexit->d_info == NULL)
		    {
		      pexit->d_info = new_door ();
		    }
		  pexit->d_info->key = tmp;
		}
	      /*pexit->rs_flags     = fread_number( fp );
	         pexit->key          = fread_number( fp ); */
	      pexit->vnum = fread_number (fp);
	      tmp = fread_number (fp);
	      tmp = fread_number (fp);
	      if (tmp != 0 && tmp != 30)
		{
		  if (pexit->d_info == NULL)
		    {
		      pexit->d_info = new_door ();
		    }
		  pexit->d_info->maxstr = tmp;
		  pexit->d_info->str = tmp;
		}
	      tmpp[0] = '\0';
	      strcpy (tmpp, fread_string2 (fp, NULL));
	      if (tmpp[0] != '\0')
		{
		  if (pexit->d_info == NULL)
		    {
		      pexit->d_info = new_door ();
		    }
		  pexit->d_info->description = str_dup (tmpp);
		}
	      /*pexit->description  = fread_string( fp,NULL ); */
	      tmpp[0] = '\0';
	      strcpy (tmpp, fread_string2 (fp, NULL));
	      if (str_cmp (tmpp, "none") && str_cmp (tmpp, "d") && tmpp[0] != '\0')
		{
		  if (pexit->d_info == NULL)
		    {
		      pexit->d_info = new_door ();
		    }
		  pexit->d_info->keyword = str_dup (tmpp);
		}
	      oneroom->exit[door] = pexit;
	      fMatch = TRUE;
	    }
	  break;
	case 'N':
	  KEY ("NAP", oneroom->a, fread_number (fp));
	  KEY ("NAC", oneroom->c, fread_number (fp)); 
	  SKEY ("Name", oneroom->name);
	  break;
	case 'M':
	  {
	    check_room_more (oneroom);
	    
	    KEY ("Move_dir", oneroom->more->move_dir, fread_number (fp));
	    SKEY ("Movemess", oneroom->more->move_message);
	  }
	  break;
	case 'F':
	  KEY ("Flags", oneroom->room_flags, fread_number (fp));
	  KEY ("Flags2", oneroom->room_flags2, fread_number (fp));
	  break;
	case 'S':
	  KEY ("Shd", oneroom->shade, fread_number (fp));
	  KEY ("Sector", oneroom->sector_type, fread_number (fp));
	  break;
	}
      if (!fMatch)
	{
	  char buf[80];
	  sprintf (buf, "fread_rooms: incorrect titler %s on v%d",
		   word, vnum);
	  bug (buf, 0);
	  fread_to_eol (fp);
	}
    }
  return;
}

void
load_rooms (FILE * fp)
{
  if (area_last == NULL)
    {
      bug ("Load_rooms: no #AREA seen yet.", 0);
      exit (1);
    }
  area_last->room_count = 0;
/*if (builder_port && area_last->open) return; */
  for (;;)
    {
      int vnum;
      char letter;
      letter = fread_letter (fp);
      if (letter != '#')
	{
	  bug ("Load_rooms: # not found.", 0);
	  exit (1);
	}

      vnum = fread_number (fp);
      if (vnum == 0)
	break;

      fBootDb = FALSE;
      if (get_room_index (vnum) != NULL)
	{
	  bug ("Load_rooms: vnum %d duplicated.", vnum);
	  continue;
	}
      fBootDb = TRUE;

      fread_room (fp, vnum);
      area_last->room_count++;
      if (area_last->open)
	num_rooms++;
      top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room;
    }

  return;
}

void
load_specials (FILE * fp)
{
  for (;;)
    {
      MOB_PROTOTYPE *pMobIndex;
      char letter;

      switch (letter = fread_letter (fp))
	{
	default:
	  bug ("Load_specials: letter '%c' not *MS.", letter);
	  exit (1);

	case 'S':
	  return;

	case '*':
	  break;

	case 'M':
	  pMobIndex = get_mob_index (fread_number (fp));
	  pMobIndex->spec_name = str_dup (fread_word (fp));
	  pMobIndex->spec_fun = spec_lookup (pMobIndex->spec_name);
	  if (pMobIndex->spec_fun == 0)
	    {
	      bug ("Load_specials: 'M': vnum %d.", pMobIndex->vnum);
	      exit (1);
	    }
	  break;
	}

      fread_to_eol (fp);
    }
  return;
}



void
load_notes (void)
{
  FILE *fp;
  NOTE_DATA *pnotelast;
  char *word;
  bool fMatch;
  bool endinterior;

  if ((fp = fopen (NOTE_FILE, "r")) == NULL)
    return;

  pnotelast = NULL;
  for (;;)
    {
      NOTE_DATA *pnote;
      char letter;

      do
	{
	  letter = getc (fp);
	  if (feof (fp))
	    {
	      fclose (fp);
	      return;
	    }
	}
      while (isspace (letter));
      ungetc (letter, fp);

      pnote = mem_alloc (sizeof (*pnote));
      pnote->board_num = 0;
      endinterior = FALSE;

      for (; !endinterior;)
	{
	  word = feof (fp) ? "End" : fread_word (fp);
	  fMatch = FALSE;

	  switch (UPPER (word[0]))
	    {
	    case 'T':
	      KEY ("TBoard", pnote->board_num, fread_number (fp));
	      KEY ("Text", pnote->text, fread_string (fp, NULL));
	      KEY ("To", pnote->to_list, fread_string (fp, NULL));
	      break;
	    case 'S':
	      KEY ("Sender", pnote->sender, fread_string (fp, NULL));
	      KEY ("Stamp", pnote->date_stamp, fread_number (fp));
	      KEY ("Subject", pnote->subject, fread_string (fp, NULL));
	      break;
	    case 'E':
	      if (!str_cmp (word, "End"))
		{
		  fMatch = TRUE;
		  endinterior = TRUE;
		  pnote->next = NULL;
		  if (pnotelast != NULL)
		    pnotelast->next = pnote;
		  else
		    note_list = pnote;
		}
	      break;
	    case 'D':
	      KEY ("Date", pnote->date, fread_string (fp, NULL));
	      break;
	    }
	  if (!fMatch)
	    {
	      sprintf (log_buf, "load_notes: bad keyword '%s'", word);
	      bug (log_buf, 0);
	      fread_to_eol (fp);
	    }
	};

      pnotelast = pnote;
    }

  strcpy (strArea, NOTE_FILE);
  fpArea = fp;
  bug ("Load_notes: bad key word.", 0);
  exit (1);
  return;
}




void
clean_blank_rooms (CHAR_DATA * ch, char *argy)
{
  ROOM_DATA *oneroom;
  EXIT_DATA *pexit;
  int iHash;
  int door;
  bool allf = FALSE;
  DEFINE_COMMAND ("cleanblankrooms", clean_blank_rooms, POSITION_DEAD,MAX_LEVEL, LOG_ALWAYS, "For admin use only.")
    if (!str_cmp (argy, "all"))
    allf = TRUE;

  for (iHash = 0; iHash < HASH_MAX; iHash++)
    {
      for (oneroom = room_hash[iHash];
	   oneroom != NULL;
	   oneroom = oneroom->next)
	{
	  bool fexit;
	  if (!allf && oneroom->area != ch->in_room->area)
	    continue;
	  if (oneroom->vnum < 1202 || oneroom->area->open == 0)
	    continue;
	  fexit = FALSE;
	  for (door = 0; door <= 5; door++)	/*Does room have an exit? */
	    {
	      if (
		   ((pexit = oneroom->exit[door]) != NULL) &&
		   pexit->to_room != NULL)
		{
		  fexit = TRUE;
		  break;
		}
	    }

	  if (!fexit && (!oneroom->more || oneroom->more->reset_first == NULL) && (!pexit))
	    {			/*Room doesn't have an exit, and no resets! */
	      char tmpbuf[500];
	      sprintf (tmpbuf, "Room %d marked for deletion!\n\r", oneroom->vnum);
	      write_to_descriptor2 (ch->desc, tmpbuf, 0);
	      check_room_more (oneroom);
	    }
	}
    }

  return;
}




CHAR_DATA *
create_mobile (MOB_PROTOTYPE * pMobIndex)
{
  CHAR_DATA *mob;

  if (pMobIndex == NULL)
    {
      bug ("Create_mobile: NULL pMobIndex.", 0);
      pMobIndex = get_mob_index (1);
      /*exit( 1 ); */
    }

  mob = new_char ();
  mob->pIndexData = pMobIndex;


  mob->pcdata = &base_pc;

  mob->special = ISMOB;
  mob->position = POSITION_STANDING;

  if (!pMobIndex->spec_name)
    pMobIndex->spec_fun = NULL;

  // Set the name and such
  mob->pcdata->name = pMobIndex->name;
  mob->pcdata->short_descr = pMobIndex->short_descr;
  mob->pcdata->long_descr = pMobIndex->long_descr;


  mob->timer = pMobIndex->timer;
  mob->act = pMobIndex->act;
  mob->height = pMobIndex->height;
  mob->affected_by = pMobIndex->affected_by;
  mob->more_affected_by = pMobIndex->more_affected_by;
  if (pMobIndex->money != 0)
    create_amount (number_range (pMobIndex->money / 4, pMobIndex->money - 1) + 1, mob,
		   NULL, NULL);
  if (pMobIndex->armcls == 101)
    {
      mob->armor = 99 - (number_range (LEVEL (mob) / 3, LEVEL (mob))) - LEVEL (mob);
      if (mob->armor <= -30)
	mob->armor /= 2;
    }
  else
    mob->armor = pMobIndex->armcls;
  set_initial_hp (mob);

  add_level_bonuses (mob, mob->pIndexData->level - 1);

  mob->damroll = 4 + mob->hit / (number_range (35, 43));
/* Before the modifier! */
  mob->max_hit = (mob->max_hit * 10 * (pMobIndex->hpmult > 1000 ? (pMobIndex->hpmult % 1000) : pMobIndex->hpmult)) / 100;
  if (mob->max_hit <= 1)
    {
      mob->max_hit = 3;
    }

  /*  if (mob->max_hit < 100 && LEVEL (mob) > 50)
    {
      mob->max_hit = 30000;
      fprintf (stderr, "MOB hps wrapped on vnum %d.\n", pMobIndex->vnum);
    }*/

  mob->hit = mob->max_hit;

  set_mob_xp (mob);

  mob->hitroll = UMAX (7, number_fuzzy (LEVEL (mob) / 2));

  mob->next = char_list;
  char_list = mob;
  pMobIndex->count++;

  conv_height (mob);
  return mob;
}



SINGLE_OBJECT *
create_object (OBJ_PROTOTYPE * pObjIndex, int level)
{
  OBJ_PROTOTYPE *pIndex;
  SINGLE_OBJECT *obj;

  if (pObjIndex == NULL)
    {
      bug ("Create_object: NULL pObjIndex.", 0);
      exit (1);
    }

  if (pObjIndex->item_type == ITEM_LIST)
    {
      char *scanarg;
      char buf[20];
      int num;
      int d;

      scanarg = str_dup (pObjIndex->description);
      num = number_range (1, pObjIndex->value[0]);
      buf[0] = '\0';

      for (d = 0; d < num; d++)
	{
	  scanarg = one_argy (scanarg, buf);
	}
      if (!is_number (buf))
	pObjIndex = get_obj_index (37);
      else
	{
	  num = atoi (buf);
	  pIndex = get_obj_index (num);
	  if (pIndex != NULL)
	    pObjIndex = pIndex;
	  else
	    pObjIndex = get_obj_index (37);
	}
    }

  obj = new_obj ();
  obj->pIndexData = pObjIndex;
  allocate_correct_datatype (obj, pObjIndex->value);
  obj->in_room = NULL;
  obj->wear_loc = WEAR_NONE;
  if (obj->pIndexData->item_type == ITEM_BOAT)
    {
      OBJ_BOAT *boat;
      obj->boat = mem_alloc (sizeof (*boat));
      obj->boat->in_boat[0] = NULL;
      obj->boat->in_boat[1] = NULL;
      obj->boat->in_boat[2] = NULL;
      obj->boat->in_boat[3] = NULL;
      obj->boat->in_boat[4] = NULL;
      obj->boat->in_boat[5] = NULL;
      obj->boat->in_boat[6] = NULL;
      obj->boat->in_boat[7] = NULL;
      obj->boat->navigator = NULL;
    }
  obj->timer = pObjIndex->timer;
  obj->extra_flags = pObjIndex->extra_flags;
  if (pObjIndex->cost != 0)
    obj->cost = number_range (pObjIndex->cost, ((pObjIndex->cost * 3) / 2));
  if (pObjIndex->cost != 0 && obj->cost == 0)
    obj->cost = 1;
  obj->next = object_list;
  object_list = obj;
  pObjIndex->count++;

  return obj;
}

MOB_PROTOTYPE *
get_mob_index (int vnum)
{
  MOB_PROTOTYPE *pMobIndex;

  if (!fBootDb && vnum > top_vnum_mob)
    return NULL;

  for (pMobIndex = mob_index_hash[vnum % HASH_MAX];
       pMobIndex != NULL;
       pMobIndex = pMobIndex->next)
    {
      if (pMobIndex->vnum == vnum)
	return pMobIndex;
    }

  if (fBootDb)
    {
      bug ("Get_mob_index: bad vnum %d.", vnum);
    }

  return NULL;
}

CLAN_DATA *
get_clan_index (int vnum)
{
  CLAN_DATA *clanny;

  if (!fBootDb && vnum > top_clan)
    return NULL;

  for (clanny = clan_index_hash[vnum % 20];
       clanny != NULL;
       clanny = clanny->next)
    {
      if (clanny->vnum == vnum)
	return clanny;
    }

  if (fBootDb)
    {
      bug ("Get_clan_index: bad vnum %d.", vnum);
      exit (1);
    }

  return NULL;
}

CLAN_DATA *
get_clan_index_2 (int vnum)
{
  CLAN_DATA *clanny;

  if (!fBootDb && vnum > top_clan_2)
    return NULL;

  for (clanny = clan_index_hash_2[vnum % 20];
       clanny != NULL;
       clanny = clanny->next)
    {
      if (clanny->vnum == vnum)
	return clanny;
    }

  if (fBootDb)
    {
      bug ("Get_clan_index_2: bad vnum %d.", vnum);
      exit (1);
    }

  return NULL;
}


OBJ_PROTOTYPE *
get_obj_index (int vnum)
{
  OBJ_PROTOTYPE *pObjIndex;

  if (!fBootDb && vnum > top_vnum_obj)
    return NULL;

  for (pObjIndex = obj_index_hash[vnum % HASH_MAX];
       pObjIndex != NULL;
       pObjIndex = pObjIndex->next)
    {
      if (pObjIndex->vnum == vnum)
	return pObjIndex;
    }

  if (fBootDb)
    {
      bug ("Get_obj_index: bad vnum %d.", vnum);
    }

  return NULL;
}



/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_DATA *
get_room_index (int vnum)
{
  ROOM_DATA *oneroom;

  if (!fBootDb && vnum > top_vnum_room)
    return NULL;

  for (oneroom = room_hash[vnum % HASH_MAX];
       oneroom != NULL;
       oneroom = oneroom->next)
    {
      if (oneroom->vnum == vnum)
	return oneroom;
    }

  if (fBootDb)
    {
      bug ("Get_room_index: bad vnum %d.", vnum);
    }

  return NULL;
}



/*
 * Read a letter from a file.
 */
char
fread_letter (FILE * fp)
{
  char c;

  do
    {
      c = getc (fp);
    }
  while (isspace (c));

  return c;
}



/*
 * Read a number from a file.
 */
long long
fread_number (FILE * fp)
{
  long long number;
  bool sign;
  char c;
  int base;
  do
    {
      c = getc (fp);
    }
  while (isspace (c));

  number = 0;

  sign = FALSE;
  if (c == '+')
    {
      c = getc (fp);
    }
  else if (c == '-')
    {
      sign = TRUE;
      c = getc (fp);
    }

  if (!isdigit (c) && c != '.')
    {
      fprintf (stderr, "BAD CHARACTER READ FIRST: %c\n", c);
      ungetc (c, fp);
      return 0;
    }

/* New octal code */
  if (c == '.')			/* An octal value */
    {
      base = 8;
      c = getc (fp);
    }
  else
    base = 10;

  while (isdigit (c))
    {
      number = (number * base) + (c - '0');
      c = getc (fp);
    }

  if (sign)
    number = 0 - number;

  if (c == '|')
    number += fread_number (fp);
  else if (c != ' ')
    ungetc (c, fp);

  return number;
}

char *
fread_string2 (FILE * fp, CHAR_DATA * ch)
{
  char *plst;
  char c;
  static char bufr[10000];
  plst = bufr;
  do
    {
      c = getc (fp);
    }
  while (isspace (c));

  if ((*plst++ = c) == '~')
    {
      bufr[0] = '\0';
      return bufr;
    }

  for (;;)
    {
      switch (*plst = getc (fp))
	{
	default:
	  plst++;
	  break;

	case EOF:
	  {
	    bug ("Fread_string: EOF - POSSIBLE PFILE CORRUPTION", 0);
	    exit (1);
	  }
	  break;

	case '\n':
	  plst++;
	  *plst++ = '\r';
	  break;

	case '\r':
	  break;

	case '~':
	  *plst = '\0';
	  return bufr;
	  break;
	}
    }
}


char *
fread_and_compress_string (FILE * fp, CHAR_DATA * ch)
{
  char tmp[8000];
  char *pls = tmp;
  char *s;
  char *plast;
  int lo;
  int lt;
  int i = 0;
  char c;

  plast = top_string + sizeof (char *);
  if (plast > &string_space[pow.space_needed - STD_LENGTH])
    {
      bug ("Fread_string: SPACE_NEEDED %d exceeded.", pow.space_needed);
      exit (1);
    }

  do
    {
      c = getc (fp);
    }
  while (isspace (c));

  if ((*pls++ = c) == '~')
    return &str_empty[0];

  for (;;)
    {
      switch (*pls = getc (fp))
	{
	default:
	  pls++;
	  break;

	case EOF:
	  {
	    bug ("Fread_string: EOF - POSSIBLE PFILE CORRUPTION", 0);
	    exit (1);
	  }
	  break;

	case '\n':
	  pls++;
	  *pls++ = '\r';
	  break;

	case '\r':
	  break;

	case '~':
	  pls[0] = '\0';
	  lo = strlen (tmp);
	  /*tally_stuff(tmp); */
	  s = compress (tmp, lo);
	  lt = strlen (s);
	  for (i = 0; i < lt; i++)
	    {
	      *plast++ = s[i];
	    }
	  bytes_saved += lo - lt;
	  /*plast++; */
	  *plast = '\0';
	  plast++;
	  {
	    union
	      {
		char *pc;
		char rgc[sizeof (char *)];
	      }
	    u1;
	    int ic;
	    int iHash;
	    char *pHash;
	    char *pHashPrev;
	    char *pString;

	    iHash = UMIN (HASH_MAX - 1, plast - 1 - top_string);
	    for (pHash = string_hash[iHash]; pHash; pHash = pHashPrev)
	      {
		for (ic = 0; ic < sizeof (char *); ic++)
		  u1.rgc[ic] = pHash[ic];
		pHashPrev = u1.pc;
		pHash += sizeof (char *);

		if (top_string[sizeof (char *)] == pHash[0]
		    && !strcmp (top_string + sizeof (char *) + 1, pHash + 1))
		    return pHash;
	      }
	    if (fBootDb)
	      {
		pString = top_string;
		top_string = plast;
		u1.pc = string_hash[iHash];
		for (ic = 0; ic < sizeof (char *); ic++)
		  pString[ic] = u1.rgc[ic];
		string_hash[iHash] = pString;

		nAllocString += 1;
		sAllocString += top_string - pString;
		return pString + sizeof (char *);
	      }
	    else
	      {
		return str_dup (top_string + sizeof (char *));
	      }
	  }
	}
    }
}


char *
fread_string (FILE * fp, CHAR_DATA * ch)
{
  char *plast;
  char c;

  plast = top_string + sizeof (char *);
  if (plast > &string_space[pow.space_needed - STD_LENGTH])
    {
      bug ("Fread_string: SPACE_NEEDED %d exceeded.", pow.space_needed);
      exit (1);
    }

  do
    {
      c = getc (fp);
    }
  while (isspace (c));

  if ((*plast++ = c) == '~')
    return &str_empty[0];

  for (;;)
    {
      switch (*plast = getc (fp))
	{
	default:
	  plast++;
	  break;

	case EOF:
	  {
	    bug ("Fread_string: EOF - POSSIBLE PFILE CORRUPTION", 0);
	    exit (1);
	  }
	  break;

	case '\n':
	  plast++;
	  *plast++ = '\r';
	  break;

	case '\r':
	  break;

	case '~':
	  plast++;
	  {
	    union
	      {
		char *pc;
		char rgc[sizeof (char *)];
	      }
	    u1;
	    int ic;
	    int iHash;
	    char *pHash;
	    char *pHashPrev;
	    char *pString;

	    plast[-1] = '\0';

	    iHash = UMIN (HASH_MAX - 1, plast - 1 - top_string);
	    for (pHash = string_hash[iHash]; pHash; pHash = pHashPrev)
	      {
		for (ic = 0; ic < sizeof (char *); ic++)
		  u1.rgc[ic] = pHash[ic];
		pHashPrev = u1.pc;
		pHash += sizeof (char *);

		if (top_string[sizeof (char *)] == pHash[0]
		    && !strcmp (top_string + sizeof (char *) + 1, pHash + 1))
		    return pHash;
	      }
	    if (fBootDb)
	      {
		pString = top_string;
		top_string = plast;
		u1.pc = string_hash[iHash];
		for (ic = 0; ic < sizeof (char *); ic++)
		  pString[ic] = u1.rgc[ic];
		string_hash[iHash] = pString;

		nAllocString += 1;
		sAllocString += top_string - pString;
		return pString + sizeof (char *);
	      }
	    else
	      {
		return str_dup (top_string + sizeof (char *));
	      }
	  }
	}
    }
}


char *
fread_string_eol (FILE * fp)
{
  static bool char_special[256 - EOF];
  char *plast;
  char c;

  if (char_special[EOF - EOF] != TRUE)
    {
      char_special[EOF - EOF] = TRUE;
      char_special['\n' - EOF] = TRUE;
      char_special['\r' - EOF] = TRUE;
    }

  plast = top_string + sizeof (char *);
  if (plast > &string_space[pow.space_needed - STD_LENGTH])
    {
      bug ("Fread_string: SPACE_NEEDED %d exceeded.", pow.space_needed);
      exit (1);
    }

  /*
   * Skip blanks.
   * Read first char.
   */
  do
    {
      c = getc (fp);
    }
  while (isspace (c));

  if ((*plast++ = c) == '\n')
    return &str_empty[0];

  for (;;)
    {
      if (!char_special[(*plast++ = getc (fp)) - EOF])
	continue;

      switch (plast[-1])
	{
	default:
	  break;

	case EOF:
	  bug ("Fread_string_eol  EOF", 0);
	  exit (1);
	  break;

	case '\n':
	case '\r':
	  {
	    union
	      {
		char *pc;
		char rgc[sizeof (char *)];
	      }
	    u1;
	    int ic;
	    int iHash;
	    char *pHash;
	    char *pHashPrev;
	    char *pString;

	    plast[-1] = '\0';
	    iHash = UMIN (HASH_MAX - 1, plast - 1 - top_string);
	    for (pHash = string_hash[iHash]; pHash; pHash = pHashPrev)
	      {
		for (ic = 0; ic < sizeof (char *); ic++)
		  u1.rgc[ic] = pHash[ic];
		pHashPrev = u1.pc;
		pHash += sizeof (char *);

		if (top_string[sizeof (char *)] == pHash[0]
		    && !strcmp (top_string + sizeof (char *) + 1, pHash + 1))
		    return pHash;
	      }

	    if (fBootDb)
	      {
		pString = top_string;
		top_string = plast;
		u1.pc = string_hash[iHash];
		for (ic = 0; ic < sizeof (char *); ic++)
		  pString[ic] = u1.rgc[ic];
		string_hash[iHash] = pString;

		nAllocString += 1;
		sAllocString += top_string - pString;
		return pString + sizeof (char *);
	      }
	    else
	      {
		return str_dup (top_string + sizeof (char *));
	      }
	  }
	}
    }
}

/*
 * Read to end of line (for comments).
 */
void
fread_to_eol (FILE * fp)
{
  char c;

  do
    {
      c = getc (fp);
    }
  while (c != '\n' && c != '\r');

  do
    {
      c = getc (fp);
    }
  while (c == '\n' || c == '\r');

  ungetc (c, fp);
  return;
}



/*
 * Read one word (into static buffer).
 */
char *
fread_word (FILE * fp)
{
  static char word[SML_LENGTH];
  char general_use[STD_LENGTH];
  char *pword;
  char cEnd;
  pword = NULL;
  do
    {
      cEnd = getc (fp);
    }
  while (isspace (cEnd));

  if (cEnd == '\'' || cEnd == '"')
    {
      pword = word;
    }
  else
    {
      word[0] = cEnd;
      pword = word + 1;
      cEnd = ' ';
    }

  for (; pword < word + SML_LENGTH; pword++)
    {
      *pword = getc (fp);
      if (cEnd == ' ' ? isspace (*pword) : *pword == cEnd)
	{
	  if (cEnd == ' ')
	    ungetc (*pword, fp);
	  *pword = '\0';
	  return word;
	}
    }
  if (!pword)
    return NULL;
  if (strlen (pword) > 5000)
    {
      sprintf (general_use, "Fread_word: word too long - %s.", pword);
      bug (general_use, 0);
      exit (1);
      return NULL;
    }
  return "END";
}


void *
mem_alloc (int sz)
{
  void *t;
  t = malloc (sz + 1);
  bzero (t, sz + 1);
  return t;
}

char *
str_dup (const char *str)
{
  char *str_new;

  if (str[0] == '\0' || str == "")
    return &str_empty[0];
  if (str >= string_space && str < top_string)
    return (char *) str;

  str_new = mem_alloc (strlen (str) + 1);
  strcpy (str_new, str);
  return str_new;
}

void
free_m (void *p)
{
  free (p);
  return;
}

void
free_string (char *pstr)
{
  if (!pstr || pstr == NULL || pstr == ""
      || pstr == &str_empty[0]
      || (pstr >= string_space && pstr < top_string))
    return;
  free_m (pstr);
  pstr = NULL;
  return;
}

void
do_memory (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *oo;
  CHAR_DATA *md;
  REALCHAR_DATA *pcd;
  int aglist;
  int flist;
  AFFECT_DATA *aff;
  OBJ_PROTOTYPE *oid;
  MOB_PROTOTYPE *mid;
  EXIT_DATA *exd;
  AREA_DATA *ar;
  RESET_DATA *rsd;
  ROOM_DATA *rrr;
  SHOP_DATA *shd;
  CED_DATA *ced;
  int socnum = 0;
  TEMP_STR *tstr;
  SOCIAL *s;
  int ctobj;
  int ctmob;
  int frmob;
  int frobj;
  long estimate=0;
  FGT *ff;
  char general_use[STD_LENGTH];
  long workspace;
  DEFINE_COMMAND ("memory", do_memory, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "Shows memory info, structure sizes, and bandwidth use.")

    ctobj = 0;
  ctmob = 0;
  frmob = 0;
  frobj = 0;
  aglist = 0;
  flist = 0;
  if (!str_cmp(argy,"timing")) {
	sprintf(general_use,"Total cpu time from bootup: %.2f seconds.\n\r",
		((double) mytime()/1000000)); 
	send_to_char(general_use,ch);
	sprintf(general_use,"Commands: %.2f seconds,  Average: %ld microseconds\n\r",
		((double) total_time[INTERPRETTED]/1000000),
		total_time[INTERPRETTED]/times_through[INTERPRETTED]);
	send_to_char(general_use,ch);
	sprintf(general_use,"Update Loop: %.2f seconds,  Average: %ld microseconds\n\r",
		((double) total_time[ALL_UPD]/1000000),
		total_time[ALL_UPD]/times_through[ALL_UPD]);
	send_to_char(general_use,ch);
/*	sprintf(general_use,"Shared mem size: %ld, Unshared data size: %ld, Unshared stack size: %ld.\n\r",
		info_mem(1),info_mem(2),info_mem(3));
	send_to_char(general_use,ch);*/
	return;
	}
  /*for (f=field_list; f; f=f->next) fields++;
  for (f=free_field_list; f; f=f->next) ffields++;*/
  for (s = social_list; s != NULL; s = s->next)
    socnum++;
  for (md = aggro_check; md != NULL; md = md->gen_next)
    aglist++;
  for (md = f_first; md != NULL; md = md->next_fighting)
    {
      flist++;
    }
  for (oo = object_list; oo != NULL; oo = oo->next)
    ctobj++;
  for (md = char_list; md != NULL; md = md->next)
    ctmob++;
  for (oo = obj_free; oo != NULL; oo = oo->next)
    frobj++;
  for (md = char_free; md != NULL; md = md->next)
    frmob++;

  if (((long) current_time - (long) bt_tm) / 10 != 0)
    workspace = (((meg_sent * 104857) + (chars_sent / 10)) / (((long) current_time - (long) bt_tm) / 10));
  else
    workspace = 1024;
  /*
  sprintf (general_use, "Fields  %-5d [%5db]\t\t\t", fields, sizeof(*f));
  send_to_char (general_use, ch);
  sprintf (general_use, "FFields %-5d\n\r", ffields);
  send_to_char (general_use, ch);
  estimate+=(fields*sizeof(*f)) + (ffields*sizeof(*f));
  sprintf (general_use, "Affects %-5d [%5db]\t\t\t", top_affect, sizeof (*aff));
  send_to_char (general_use, ch);*/
  estimate+=top_affect*sizeof(*aff);
  sprintf (general_use, "Areas   %-5d [%5db]\n\r", top_area, sizeof (*ar));
  send_to_char (general_use, ch);
  estimate+=top_area*sizeof(*ar);
  sprintf (general_use, "Exits   %-5d [%5db]\t\t\t", top_exit, sizeof (*exd));
  send_to_char (general_use, ch);
  estimate+=top_exit*sizeof(*exd);
  sprintf (general_use, "MobInd  %-5d [%5db]\n\r", top_mob_index, sizeof (*mid));
  send_to_char (general_use, ch);
  estimate+=top_mob_index*sizeof(*mid);
  sprintf (general_use, "Mobs/Ch %-5d [%5db]\t\t\t", ctmob, sizeof (*md));
  send_to_char (general_use, ch);
  estimate+=ctmob*sizeof(*md);
  sprintf (general_use, "TempStr %-5d [%5db]\n\r", top_temp, sizeof (*tstr));
  send_to_char (general_use, ch);
  estimate+=top_temp*sizeof(*tstr);
  sprintf (general_use, "ObjInd  %-5d [%5db]\t\t\t", top_obj_index, sizeof (*oid));
  send_to_char (general_use, ch);
  estimate+=top_obj_index*sizeof(*oid);
  sprintf (general_use, "ObjsNow %-5d [%5db]\n\r", ctobj, sizeof (*oo));
  send_to_char (general_use, ch);
  estimate+=ctobj*sizeof(*oo);
  sprintf (general_use, "Resets  %-5d [%5db]\t\t\t", top_reset, sizeof (*rsd));
  send_to_char (general_use, ch);
  estimate+=top_reset*sizeof(*rsd);
  sprintf (general_use, "Rooms   %-5d [%5db]\n\r", top_room, sizeof (*rrr));
  send_to_char (general_use, ch);
  estimate+=top_room*sizeof(*rrr);
  sprintf (general_use, "Shops   %-5d [%5db]\t\t\t", top_shop, sizeof (*shd));
  send_to_char (general_use, ch);
  estimate+=top_shop*sizeof(*shd);
  sprintf (general_use, "Ced's   %-5d [%5db]\n\r", top_ced, sizeof (*ced));
  send_to_char (general_use, ch);
  estimate+=top_ced*sizeof(*ced);
  sprintf (general_use, "Plyrsiz %-5d [%5db]\t\t\t", max_connected, sizeof (*pcd));
  send_to_char (general_use, ch);
  estimate+=max_connected*(sizeof(*pcd)+512);
  sprintf (general_use, "FGTstrc %-5d [%5db]\n\r", total_fgt,sizeof(*ff));
  send_to_char (general_use, ch);
  estimate+=total_fgt*sizeof(*ff);
  sprintf (general_use, "\n\rSocials %-5d\t\t", socnum);
  send_to_char (general_use, ch);
  estimate+=socnum*128;
  sprintf (general_use, "Descrps %-5d\t\t", desc_rooms);
  send_to_char (general_use, ch);
  sprintf (general_use, "ObjFree %-5d\n\r", frobj);
  send_to_char (general_use, ch);
  estimate+=frobj*sizeof(*oo);
  sprintf (general_use, "AggSize %-5d\t\t", aglist);
  send_to_char (general_use, ch);
  sprintf (general_use, "FightLs %-5d\n\r", flist);
  send_to_char (general_use, ch);
  sprintf (general_use, "MobFree %-5d\t\t", frmob);
  send_to_char (general_use, ch);
  estimate+=frmob*sizeof(*md);
  sprintf (general_use, "ExDes   %-5d\t\t", top_ed);
  send_to_char (general_use, ch);
  sprintf (general_use, "Helps   %-5d\n\r", top_help);
  send_to_char (general_use, ch);
  sprintf (general_use, "\n\rTop Vnums: %d mobs  %d objs  %d rooms\n\r", top_vnum_mob,
	   top_vnum_obj,
	   top_vnum_room);
  send_to_char (general_use, ch);

  sprintf (general_use, "%d shared-string bytes used.\n\r",
	   sAllocString + 10);
  send_to_char (general_use, ch);
  estimate+=sAllocString+1024;
  sprintf (general_use, "Average chars/sec (bandwidth): %-5ld\n\r", workspace);
  send_to_char (general_use, ch);
/*  sprintf (general_use, "Sent %d Meg %ld Chrs\n\r\n\r", meg_sent, chars_sent);
  send_to_char (general_use, ch);*/
  sprintf(general_use,"Estimated Data Memory Usage: %ld (%ldK).\n\r",estimate,estimate/1024);
  send_to_char (general_use, ch);

/*
   sprintf( general_use, "Perms   %5d blocks  of %7d bytes.\n\r",
   nAllocPerm, sAllocPerm );
   send_to_char( general_use, ch ); */

  return;
}

void 
do_startup_time (CHAR_DATA *ch, char *argy)
{
  FILE *fp;
  DEFINE_COMMAND ("startuptime", do_startup_time, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Writes the original startup time of the game to a file.")
    
    if (str_cmp(argy, "Xaxglokmorg"))
      {
	send_to_char("Xaxglokmorg\n\r", ch);
	return;
      }
  if ((fp = fopen ("startup.time", "w")) == NULL)
    {
      bug ("Save_startup_time: fopen", 0);
      return;
    }
  startup_time = (current_time);
  fprintf(fp, "%d", startup_time);
  fclose(fp);
  send_to_char("Startup time of mud changed.\n\r", ch); 
  return;
}

void fread_startup_time (void)
{
  FILE *fp;
  if ((fp = fopen("startup.time", "r")) == NULL)
    {
      fprintf(stderr, "No startup time. Setting startup time to current_time.\n\r");
      startup_time = current_time;
      return;
    }
  startup_time = fread_number(fp);
  fclose (fp);
  return;
}





void
do_world (CHAR_DATA * ch, char *argy)
{
  int days;
  int hours;
  int minutes;
  long seconds;
  char general_use[STD_LENGTH];
  DEFINE_COMMAND ("world", do_world, POSITION_DEAD, 0, LOG_NORMAL, "Shows statistics about uptime and areas/rooms.")

    send_to_char ("\x1b[0;36m---------- [A Few Mud Statistics] ----------\x1b[0;37m\n\r\n\r", ch);
  sprintf (general_use, "\x1b[1;36mAr\x1b[1;36meas explorab\x1b[1;36mle\x1b[0;37m \x1b[1;30m(\x1b[1;37mOpen\x1b[0;37m\x1b[1;30m)\x1b[1;37m             [\x1B[1;36m%5d\x1B[1;37m]\n\r", num_areas);
  send_to_char (general_use, ch);
  sprintf (general_use, "\x1b[1;36mRooms explorable \x1b[1;30m(\x1b[1;37mAccessable\x1b[1;30m)\x1b[1;37m       [\x1B[1;36m%5d\x1B[1;37m]\n\r", num_rooms);
  send_to_char (general_use, ch);
  sprintf (general_use, "\n\rReason for last mud termination: \x1B[1;37m%s\x1B[0m\n\r", reasonone);
  send_to_char (general_use, ch);
  sprintf (general_use, "\x1b[0;37mCurrent System Time Now: %s\r", ((char*) ctime (&current_time)));
  send_to_char (general_use, ch);
  seconds = current_time - bt_tm;
  minutes = seconds / 60;
  seconds %= 60;
  hours = minutes / 60;
  minutes %= 60;
  days = hours / 24;
  hours %= 24;
  sprintf (general_use, "\x1b[0;37mCurrent Uptime: %d day%s, %d hour%s, %d minute%s, %ld second%s\n\r",
	   days, ((days == 0 || days > 1) ? "s" : ""),
	   hours, ((hours == 0 || hours > 1) ? "s" : ""),
	   minutes, ((minutes == 0 || minutes > 1) ? "s" : ""),
	   seconds, ((seconds == 0 || seconds > 1) ? "s" : ""));
  send_to_char (general_use, ch);
  return;
}



int
number_fuzzy (int number)
{
  switch (number_bits (2))
    {
    case 0:
      number -= 1;
      break;
    case 3:
      number += 1;
      break;
    }

  return UMAX (1, number);
}



int
number_range (int from, int to)
{
  unsigned int power;
  unsigned int number;

  if (from > 900000 || from < 0 || to > 900000 || to < 0)
    return 1;

  if ((to = to - from + 1) <= 1)
    return from;

  if (to < 2)
    return from;

  for (power = 2; power < to; power <<= 1)
    ;

  while ((number = number_mm () & (power - 1)) >= to)
    ;

  return from + number;
}



int
number_percent (void)
{
  int percent;

  while ((percent = number_mm () & (128 - 1)) > 99)
    ;

  return 1 + percent;
}



int
number_door (void)
{
  int door;

  while ((door = number_mm () & (8 - 1)) > 5)
    ;

  return door;
}



int
number_bits (int width)
{
  return number_mm () & ((1 << width) - 1);
}




static int rgiState[2 + 55];

void
init_mm ()
{
  int *piState;
  int iState;

  piState = &rgiState[2];

  piState[-2] = 55 - 55;
  piState[-1] = 55 - 24;

  piState[0] = ((int) current_time) & ((1 << 30) - 1);
  piState[1] = 1;
  for (iState = 2; iState < 55; iState++)
    {
      piState[iState] = (piState[iState - 1] + piState[iState - 2])
	& ((1 << 30) - 1);
    }
  return;
}



int
number_mm (void)
{
  int *piState;
  int iState1;
  int iState2;
  int iRand;

  piState = &rgiState[2];
  iState1 = piState[-2];
  iState2 = piState[-1];
  iRand = (piState[iState1] + piState[iState2])
    & ((1 << 30) - 1);
  piState[iState1] = iRand;
  if (++iState1 == 55)
    iState1 = 0;
  if (++iState2 == 55)
    iState2 = 0;
  piState[-2] = iState1;
  piState[-1] = iState2;
  return iRand >> 6;
}



/*
 * Roll some dice.
 */
int
dice (int number, int size)
{
  int idice;
  int sum;

  switch (size)
    {
    case 0:
      return 0;
    case 1:
      return number;
    }

  for (idice = 0, sum = 0; idice < number; idice++)
    sum += number_range (1, size);

  return sum;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void
smash_tilde (char *str)
{
  for (; *str != '\0'; str++)
    {
      if (*str == '~')
	*str = '-';
    }

  return;
}



bool
str_cmp (const char *astr, const char *bstr)
{

  if (!astr)
    return TRUE;

  if (!bstr)
    return TRUE;
  if (!*astr)
    return TRUE;
  if (!*bstr)
    return TRUE;

  for (; *astr || *bstr; astr++, bstr++)
    {
      if (LOWER (*astr) != LOWER (*bstr))
	return TRUE;
    }

  return FALSE;
}



bool
str_prefix (const char *astr, const char *bstr)
{
  if (!astr)
    {
      return TRUE;
    }

  if (!bstr)
    {
      return TRUE;
    }

  for (; *astr; astr++, bstr++)
    {
      if (LOWER (*astr) != LOWER (*bstr))
	return TRUE;
    }

  return FALSE;
}



bool
str_infix (const char *astr, const char *bstr)
{
  int sstr1;
  int sstr2;
  int ichar;

  if (!astr || !bstr)
    return TRUE;

  sstr1 = strlen (astr);
  sstr2 = strlen (bstr);

  for (ichar = 0; ichar <= sstr2 - sstr1; ichar++)
    {
      if (!str_prefix (astr, bstr + ichar))
	return FALSE;
    }

  return TRUE;
}



bool
str_suffix (const char *astr, const char *bstr)
{
  int sstr1;
  int sstr2;
  if (!astr || !bstr || astr == NULL || bstr == NULL)
    return TRUE;
  sstr1 = strlen (astr);
  sstr2 = strlen (bstr);
  if (sstr1 <= sstr2 && !str_cmp (astr, bstr + sstr2 - sstr1))
    return FALSE;
  else
    return TRUE;
}



char *
capitalize (const char *str)
{
  static char strcap[STD_LENGTH];
  int i;

  for (i = 0; str[i] != '\0'; i++)
    strcap[i] = LOWER (str[i]);
  strcap[i] = '\0';
  strcap[0] = UPPER (strcap[0]);
  return strcap;
}



/*
 * Append a string to a file.
 */
void
append_file (CHAR_DATA * ch, char *file, char *str)
{
  FILE *fp;

  if (IS_MOB (ch) || str[0] == '\0')
    return;
#ifndef WINDOWS
//  fclose (fpReserve);
#endif
  if ((fp = fopen (file, "a")) == NULL)
    {
      perror (file);
      send_to_char ("Could not open the file!\n\r", ch);
    }
  else
    {
      fprintf (fp, "[%5d] %s: %s\n",
	       ch->in_room ? ch->in_room->vnum : 0, NAME (ch), str);
      fclose (fp);
    }
#ifndef WINDOWS
//  fpReserve = fopen (NULL_FILE, "r");
#endif
  return;
}



void
bug (const char *str, int param)
{
  char general_use[STD_LENGTH];
  FILE *fp;

  if (fpArea != NULL)
    {
      int iLine;
      int iChar;

      if (fpArea == stdin)
	{
	  iLine = 0;
	}
      else
	{
	  iChar = ftell (fpArea);
	  fseek (fpArea, 0, 0);
	  for (iLine = 0; ftell (fpArea) < iChar; iLine++)
	    {
	      while (getc (fpArea) != '\n')
		;
	    }
	  fseek (fpArea, iChar, 0);
	}

      sprintf (general_use, "[<FBUG>] FILE: %s LINE: %d", strArea, iLine);
      log_string (general_use);

      sprintf (log_buf, "Notify> %s", general_use);
      do_global (log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_BUG);

      if ((fp = fopen ("shutdown.txt", "a")) != NULL)
	{
	  fprintf (fp, "[--!!--] %s\n", general_use);
	  fclose (fp);
	}
    }

  strcpy (general_use, "[*Loading*] "); 
  sprintf (general_use + strlen (general_use), str, param);
  log_string (general_use);

  return;
}



void
log_string (const char *str)
{
  char *strtime;
  char genn[700];
  /* can't log me */
  strtime = ctime (&current_time);
  strtime[strlen (strtime) - 1] = '\0';
  fprintf (stderr, "[%s] %s\n", strtime, str);

  sprintf (genn, "GodInfo> %s", str);
  NOTIFY (genn, LEVEL_IMMORTAL, WIZ_NOTIFY_LOG);
  LOG_LEVEL = LEVEL_IMMORTAL;

  return;
}


int
exit_flag (int flag)
{
  switch (flag)
    {
    case 0:
      return 1;
    case 1:
      return 3;
    case 2:
      return 7;
    default:
      return 0;
    }
  return 0;
}

char *
add_newline (char *str)
{
  char buf[STD_LENGTH];
  int i = 0;

  if (str == NULL)
    return (str_dup (""));

  while (*str != '~' && *str != '\0')
    {
      buf[i] = *str;
      i++;
      str++;
    }
  strcat (buf, "\n\r");

  return (str_dup (buf));
}

char *
get_name (char *str)
{
  if (str == NULL)
    return (str_dup (""));

  while (isspace (*str))
    str++;

  while (*str != '\0')
    {
      if (*str == '}')
	{
	  str++;
	  break;
	}
      str++;
    }

  while (isspace (*str))
    str++;

  return (str_dup (str));
}


void
fix_exits (void)
{
  ROOM_DATA *oneroom;
  EXIT_DATA *pexit;
  int iHash;
  int door;
  int i;
  ROOM_DATA *troom;
  EXIT_DATA *backexit;
  char out;
  bool dir[4]; 
  bool found;
  char outpt[20];
  char color[25]; 
  for (iHash = 0; iHash < HASH_MAX; iHash++)
    {
      for (oneroom = room_hash[iHash];
	   oneroom != NULL;
	   oneroom = oneroom->next)
	{
	  bool fexit;
	  fexit = FALSE;
	  for (door = 0; door <= 5; door++)
	    {
	      if ((pexit = oneroom->exit[door]) != NULL)
		{
		  fexit = TRUE;
		  if (pexit->vnum > 100000 || pexit->vnum == 0)
		    continue;
		  pexit->to_room = (ROOM_DATA *)get_room_index (pexit->vnum);
		}
	    }
	}
    }
	  /* This part is to make overhead mapping more efficient. */
  for (iHash = 0; iHash < HASH_MAX; iHash++)
    {
      for (oneroom = room_hash[iHash];
	   oneroom != NULL;
	   oneroom = oneroom->next)
	{
	  for (i = 0; i < 4; i++)
	    {
	      oneroom->mapexit[i] = NULL;
	      if (((pexit = oneroom->exit[i]) != NULL) &&  
		  ((troom = pexit->to_room) != NULL) && 
		  ((backexit = troom->exit[rev_dir[i]]) != NULL) && 
		  (backexit->to_room != NULL) &&
		  (!pexit->d_info || 
		   !IS_SET(pexit->d_info->rs_flags, EX_HIDDEN)) &&  
		  (!backexit->d_info || 
		   !IS_SET(backexit->d_info->rs_flags, EX_HIDDEN)))
		{
		  oneroom->mapexit[i] = troom;
		}
	    }
	  oneroom->ibm[0] = ' ';
	  oneroom->ibm[1] =  '\0';
	  oneroom->noibm[0] = ' ';
	  oneroom->noibm[1] = '\0';
	  oneroom->color = 0;
	  found = TRUE;
	  for (i = 0; i < 4; i ++)
	    {
	      dir[i] = FALSE;
	      if (oneroom->mapexit[i] != NULL)
		dir[i] = TRUE;
	    }
	  switch (oneroom->sector_type)
	    {
	    default: 
	      found = FALSE;
	      break;
	    case SECT_HILLS:
	    case SECT_MOUNTAIN:
	    case SECT_ROCKY:
	    case SECT_CANYON:
	      {
		oneroom->noibm[0] = '^';
		break;
	      }
            case SECT_ROAD:
            case SECT_TRAIL:
            case SECT_PATH:
              {
                 oneroom->noibm[0] = '+';
                 break;
              }
	    case SECT_FOREST:
	    case SECT_WOODS:
	    case SECT_BRUSH:
	      {
		oneroom->noibm[0] = '*';
		break;
	      }
	    case SECT_DESERT:
	    case SECT_WASTELAND:
	      {
		oneroom->noibm[0] = ',';
		break;
	      }
	    case SECT_WATER_SWIM:
	    case SECT_WATER_NOSWIM:
	    case SECT_UNDERWATER:
	      {
		oneroom->noibm[0] = '#';
		break;
	      }
	    case SECT_SWAMP:
	    case SECT_MARSH:
	    case SECT_HOLLOW:
	    case SECT_ICE:
	    case SECT_SNOW:
	    case SECT_ARCTIC:
	      {
		oneroom->noibm[0] = '"';
		break;
	      }
	    case SECT_FIELD:
	    case SECT_GRASSLANDS:
	    case SECT_PLAINS:
	    case SECT_SAVANNAH:
	    case SECT_STEPPE:
	      {
		oneroom->noibm[0] = '.';
		break;
	      }
	    }
	  if (dir[DIR_EAST])
	    {
	      if (dir[DIR_WEST])
		{
		  if (dir[DIR_NORTH])
		    {
		      if (dir[DIR_SOUTH]) /* N E W S */
			{
			  if (!found)
			    oneroom->noibm[0] = '+';
			  oneroom->ibm[0] = '\305';
			}
		      else /* N E W */
			{
			  if (!found)
			    oneroom->noibm[0] = '+';
			  oneroom->ibm[0] = '\301';
			}
		    }
		  else /* Not north */
		    {
		      if (dir[DIR_SOUTH]) /* Not North */
			{
			  if (!found) /* W E S */
			    oneroom->noibm[0] = '+';
			  oneroom->ibm[0] = '\302';
			}
		      else
			{
			  if(!found) /* E W */
			    oneroom->noibm[0] = '-';
			  oneroom->ibm[0] = '\304';
			}
		    }
		}
	      else /* East and Not West */
		{
		  if (dir[DIR_NORTH])
		    {
		      if(dir[DIR_SOUTH])
			{
			  if (!found) /* N E S */
			    oneroom->noibm[0] = '}';
			  oneroom->ibm[0] = '\303';
			}
		      else
			{
			  if (!found) /* E N */
			    oneroom->noibm[0] = '\\';
			  oneroom->ibm[0] = '\330';
			}
		    }
		  else /* East and not north and not west */
		    {
		      if (dir[DIR_SOUTH]) /* E S */
			{
			  if (!found)  /* E S */
			    oneroom->noibm[0] = '/';
			  oneroom->ibm[0] = '\332';
			}
		      else
			{
			  if (!found) /* E */
			     oneroom->noibm[0] = '-';
			  oneroom->ibm[0] = '\304';
			}
		    }
		}
	    }
	  else /* Not east */
	    {
	      if (dir[DIR_WEST]) /* West but not east */
		{
		  if (dir[DIR_NORTH])
		    {
		      if (dir[DIR_SOUTH])
			{
			  if (!found) /* W S N */
			    oneroom->noibm[0] = '{';
			  oneroom->ibm[0] = '\264';
			}
		      else
			{
			  if (!found) /* W N */
			    oneroom->noibm[0] = '/';
			  oneroom->ibm[0] = '\331';
			}
		    }
		  else 
		    {
		      if (dir[DIR_SOUTH]) /* S W */
			{
			  if (!found) /* W S */
			    oneroom->noibm[0] = '\\';
			  oneroom->ibm[0] = '\277';
			}
		      else
			{
			  if (!found) /* W */
			    oneroom->noibm[0] = '-';
			  oneroom->ibm[0] = '\304'; 
			}
		    }
		}
	      else /* Not east and Not west */
		{
		  if (dir[DIR_NORTH] ||  dir[DIR_SOUTH]) /* N, S, or N S */
		    {
		      if (!found)
			oneroom->noibm[0] = '|';
		      oneroom->ibm[0] = '\263';
		    }
		}
	    }
	  oneroom->color = sectors[oneroom->sector_type].colort;
	  oneroom->minerals = number_range(0, 6);
	}
    }
  return;
}
  
