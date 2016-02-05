#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
#define CTRL_R ""
char update_char[400];

void 
do_afk (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("afk", do_afk, POSITION_DEAD, 0, LOG_NORMAL, "This command logs you as away from the keyboard.")
    
    send_to_char ("You have been flagged as AFK. Typing any input will turn this off.\n\r", ch);
  send_to_char ("If you are away for too long, your character *will* autosave and autoquit.\n\r", ch);
  ch->timer = 50;
  return;
}


void
info_remort (CHAR_DATA * ch, char * argy)
{
  send_to_char("This command does not actually remort your character.\n\r", ch);
  send_to_char("It checks to see if you have the requirements for remorting.\n\r", ch);
  send_to_char("Type remort requirements to see the requrements for remorting.\n\r", ch);
  send_to_char("Type REMORT REMORT to actually remort your character.\n\r", ch);
}

void
show_remort_requirements(CHAR_DATA * ch, char *argy)
{
  char buf[500];
  int remort = 0;
  for (remort = 0; remort < pow.max_remorts; ++remort)
    {
      if((remort == ch->pcdata->remort_times))
	sprintf(buf,"-->");
      else
	sprintf(buf, "   ");
      send_to_char(buf, ch);
      //Non Ascended Remort Requirements
      if (ch->pcdata->race < 9)
         sprintf(buf, "Remort: %2d, Lvl: %2d, Wps Req/Lost: %5d/%5d, Kps Req/Lost: %5d/%5d\n\r", (remort+1), pow.remort_req[remort][0], pow.remort_req[remort][1], pow.remort_req[remort][2],pow.remort_req[remort][3], pow.remort_req[remort][4]);
      //Ascended Remort Requirements
      if (ch->pcdata->race > 8)
         sprintf(buf, "Remort: %2d, Lvl: %2d, Wps Req/Lost: %5d/%5d, Kps Req/Lost: %5d/%5d\n\r", (remort+1), pow.ascended_remort_req[remort][0], pow.ascended_remort_req[remort][1], pow.ascended_remort_req[remort][2], pow.ascended_remort_req[remort][3], pow.ascended_remort_req[remort][4]);
      send_to_char(buf, ch);
      if ((remort == ch->pcdata->remort_times))
	{
	  sprintf(buf, "-->You are level %d, you have %ld warpoints, and have %d killpoints.\n\r", LEVEL(ch), ch->pcdata->warpoints, ch->pcdata->killpoints);
	  send_to_char(buf, ch);
	}
    }
   if (ch->pcdata->remort_times >= pow.max_remorts)
      send_to_char("You have the maximum number of remorts.\n\r", ch);
return;
}

void
do_raceinfo (CHAR_DATA *ch, char *argy)
{
char buf[SML_LENGTH];
int iRace;
int i;
SPELL_DATA *spl;
DEFINE_COMMAND("raceinfo", do_raceinfo, POSITION_DEAD, 0, LOG_NORMAL, "This command lists the basic information about the races in the game.")

if (!str_cmp(argy, "all") || argy[0] == '\0' || argy == "" || argy == "" || argy[0] == '\0' || !str_cmp(argy, "blank") || !str_cmp(argy, "remort") || !str_cmp(argy, "ascend") || !str_cmp(argy, "evolve"))
  {
    send_to_char("          Name     St In Wi De Co  Sk/Sp Rgn H/M   Ac  Specials\n\r", ch);
    for(iRace = 0; iRace < RACE_COUNT; iRace++)
      {
		if ((str_cmp(race_info[iRace].name, "Blank") && 
		    (
		     ((race_info[iRace].remort_race == 0) && 
 		     str_cmp(argy,"ascend") && str_cmp(argy,"evolve")) ||
		     ((race_info[iRace].remort_race == 1) && 
		      !str_cmp(argy, "evolve")) ||
		     ((race_info[iRace].remort_race == 2) && 
		      !str_cmp(argy, "ascend")))
		    )
		   )
	  {
	    bool before = FALSE;
	    sprintf(buf,"[%2d]\x1b[1;37m %12s: \x1b[1;37m%2d %2d %2d %2d %2d\x1b[0;37m \x1b[1;31m %2d/%-2d/%-2d\x1b[1;34m%3d %3d\x1b[1;37m  %3d ",(iRace+1),race_info[iRace].name ,race_info[iRace].limits[0], race_info[iRace].limits[1],  race_info[iRace].limits[2], race_info[iRace].limits[3], race_info[iRace].limits[4], race_info[iRace].max_skills, race_info[iRace].max_spells, race_info[iRace].max_profs, race_info[iRace].regen_hit_percent, race_info[iRace].regen_move_percent, race_info[iRace].natarmor);
	    send_to_char(buf, ch);
	    if (race_info[iRace].affect_bits != 0)
	      {
		before = TRUE;
		sprintf(buf, "%s", natural_abilities (race_info[iRace].affect_bits));
		send_to_char(buf, ch);
	      }
	    if (race_info[iRace].affect_bits2 != 0)
	      {
		if (before == TRUE)
		  send_to_char(",", ch);
		before = TRUE;
		sprintf(buf, "%s", natural_abilities2 (race_info[iRace].affect_bits2));
		send_to_char(buf, ch);
	      }
	    if (race_info[iRace].nat_augments != 0)
	      {
		if (before == TRUE)
		  send_to_char(",", ch);
		before = TRUE;
		sprintf(buf, "%s", augmentss (race_info[iRace].nat_augments));
		send_to_char(buf, ch);
	      }
	    send_to_char("\x1b[0;37m\n\r", ch);
	  }
      }
    return;
  }
for (iRace = 0; iRace < RACE_COUNT; iRace++) 
  {
    if (!str_cmp(argy, race_info[iRace].name) && str_cmp(argy, "blank"))
      break;
  }
if (is_number(argy))
  iRace = atoi(argy)-1;
if (iRace >= -1 && iRace < (RACE_COUNT) &&
str_cmp(race_info[iRace].name,"blank"))
  {
    do_help (ch, (char *) race_info[iRace].name);
    
    send_to_char ("--------------------------------------------------------------------------\n\r", ch);
    sprintf (buf, "People of this race usually range in height from %d'%d\" to %d' %d\".\n\r",
	     race_info[iRace].height_min / 12, race_info[iRace].height_min % 12,
	     race_info[iRace].height_max / 12, race_info[iRace].height_max % 12);
    send_to_char(buf, ch);
    sprintf (buf, "Max Skills: \x1B[1;32m%d\x1B[0;37m. Max Spells:\x1B[1;32m%d\x1B[0;37m. Max Profs:\x1B[1;32m%d\x1B[0;37m.\n\r", race_info[iRace].max_skills,
	     race_info[iRace].max_spells, race_info[iRace].max_profs);
    send_to_char(buf, ch);
    
    if (pow.show_race_select_info || ch->pcdata->level > 0)
      {
	sprintf (buf, "Natural Armor Bonus:\x1B[1;34m%d\x1B[0;37m.\n\rStat limits: \x1B[1;35mS: %d  D: %d  C: %d  I: %d W: %d\x1B[0;37m.\n\r",
		 (100 - race_info[iRace].natarmor),
		 race_info[iRace].limits[0], race_info[iRace].limits[3],
		 race_info[iRace].limits[4], race_info[iRace].limits[1],
		 race_info[iRace].limits[2]);
	send_to_char(buf, ch);
	sprintf (buf, "Natural abilities: \x1B[1;36m%s\x1B[0;37m.\n\r",
		 natural_abilities (race_info[iRace].affect_bits));
	send_to_char(buf, ch);
	sprintf (buf, "Natural abilities2: \x1B[1;36m%s\x1B[0;37m.\n\r",
		 natural_abilities2 (race_info[iRace].affect_bits2));
	send_to_char(buf, ch);
	sprintf (buf, "Natural Enhancements: \x1B[1;36m%s\x1B[0;37m.\n\r",
		 augmentss (race_info[iRace].nat_augments));
	send_to_char(buf, ch);
	if (race_info[iRace].regen_hit_percent != 100)
	  {
	    sprintf (buf, "Hitpoint regeneration is \x1B[1;37m%d\x1B[0;37m percent of normal.\n\r", race_info[iRace].regen_hit_percent);
	    send_to_char(buf, ch);
	  }
	if (race_info[iRace].regen_move_percent != 100)
	  {
	    sprintf (buf, "Movement regeneration is \x1B[1;37m%d\x1B[0;37m percent of normal.\n\r", race_info[iRace].regen_move_percent);
	    send_to_char(buf, ch);
	  }
	if (race_info[iRace].attack_spell_percent != 100)
	  {
	    sprintf (buf, "This race casts attack spells at \x1B[1;37m%d\x1B[0;37m percent of normal power.\n\r",
		     race_info[iRace].attack_spell_percent);
	    send_to_char(buf, ch);
	      }
	if (race_info[iRace].combat_damage_bonus != 0)
	  {
	    sprintf (buf, "This race has a natural attack power %s of \x1B[1;37m%d\x1B[0;37m.\n\r", (race_info[iRace].combat_damage_bonus > 0 ?
												     "bonus" : "penalty"), race_info[iRace].combat_damage_bonus);
	    send_to_char(buf, ch);
	  }
	if (race_info[iRace].backstab_percent != 100)
	  {
	    sprintf (buf, "This race backstabs at \x1B[1;37m%d\x1B[0;37m percent of normal.\n\r", race_info[iRace].backstab_percent);
	    send_to_char(buf, ch);
	  }
	if (race_info[iRace].heal_percent != 100)
	  {
	    sprintf (buf, "This race casts healing spells at \x1B[1;37m%d\x1B[0;37m percent of normal power.\n\r",
			 race_info[iRace].heal_percent);
	    send_to_char(buf, ch);
	  }
	if (race_info[iRace].hps_bonus != 0)
	  {
	    sprintf (buf, "This race %s a %s \x1B[1;37m%d\x1B[0;37m hit points gained per level.\n\r",
		     (race_info[iRace].hps_bonus > 0 ? "gains" : "has"),
		     (race_info[iRace].hps_bonus > 0 ? "bonus" : "penalty"),
		     (race_info[iRace].hps_bonus));
	    send_to_char(buf, ch);
	  }
	if (race_info[iRace].mana_percent != 100)
	  {
	    sprintf (buf, "This race stores \x1B[1;37m%d\x1B[0;37m percent of the normal amount of mana.\n\r", race_info[iRace].mana_percent);
	    send_to_char(buf, ch);
	  }
	for (i = 0; i < 3; i ++)
	  {
	    if (race_info[iRace].nat_skills[i] > 0)
	      {
		if ((spl = skill_lookup (NULL,race_info[iRace].nat_skills[i])) != NULL) 
		  {
		    sprintf(buf, "This race can use %s naturally.\n\r", spl->spell_funky_name);
		    send_to_char(buf, ch);
		  }
	      }
	  }
      }
    send_to_char ("--------------------------------------------------------------------------\n\r", ch);
  }
else
  {
    send_to_char("raceinfo or raceinfo <racename> or raceinfo <race_number>.\n\r", ch);
  }
return;
}

void
do_alignmentinfo (CHAR_DATA *ch, char *argy)
{
char buf[SML_LENGTH];
int Align, i;
SPELL_DATA *spl;
DEFINE_COMMAND("aligninfo", do_alignmentinfo, POSITION_DEAD, 0, LOG_NORMAL, "This command lists the basic information about the alignments in the game.")

if (!str_cmp(argy, "all") || argy[0] == '\0' || argy == "" || argy == "" || argy[0] == '\0' || !str_cmp(argy, "blank"))
  {
    send_to_char("         Bonuses to all of the following above and beyond racial bonuses:\n\r", ch);
    send_to_char("          Name    St In Wi De Co  Sk/Sp/Pr Rgn H/M  Ac Specials\n\r", ch);
    for(Align = 0; Align < NUM_ALIGN; Align++)
      {
	if (str_cmp(race_info[Align].name, "Blank"))
	  {
	    bool before = FALSE;
	    sprintf(buf,"[%2d]\x1b[1;37m%12s: \x1b[1;37m%2d %2d %2d %2d %2d\x1b[0;37m \x1b[1;31m %2d/%1d/%-2d\x1b[1;34m%3d %3d\x1b[1;37m  %3d ",(Align),align_info[Align].name ,align_info[Align].bonus[0], align_info[Align].bonus[1],  align_info[Align].bonus[2], align_info[Align].bonus[3], align_info[Align].bonus[4], align_info[Align].max_skills, align_info[Align].max_spells, align_info[Align].max_profs, align_info[Align].regen_hit_percent, align_info[Align].regen_move_percent, align_info[Align].natarmor);
	    send_to_char(buf, ch);
	    if (align_info[Align].affect_bits != 0)
	      {
		before = TRUE;
		sprintf(buf, "%s", natural_abilities (align_info[Align].affect_bits));
		send_to_char(buf, ch);
	      }
	    if (align_info[Align].affect_bits2 != 0)
	      {
		if (before == TRUE)
		  send_to_char(",", ch);
		before = TRUE;
		sprintf(buf, "%s", natural_abilities2 (align_info[Align].affect_bits2));
		send_to_char(buf, ch);
	      }
	    if (align_info[Align].nat_augments != 0)
	      {
		if (before == TRUE)
		  send_to_char(",", ch);
		before = TRUE;
		sprintf(buf, "%s", augmentss (align_info[Align].nat_augments));
		send_to_char(buf, ch);
	      }
	    send_to_char("\x1b[0;37m\n\r", ch);
	  }
      }
    return;
  }
for (Align = 0; Align < NUM_ALIGN; Align++) 
  {
    if (!str_cmp(argy, align_info[Align].name) && str_cmp(argy, "blank"))
      break;
  }
if (is_number(argy))
  Align = atoi(argy);
if (Align >= 0 && Align < (NUM_ALIGN) && str_cmp(align_info[Align].name,"blank"))
  {
    do_help (ch, (char *) align_info[Align].name);
    
    send_to_char ("--------------------------------------------------------------------------\n\r", ch);
    
    sprintf (buf, "Skill %s: \x1B[1;32m%d\x1B[0;37m. Spell %s:\x1B[1;32m%d\x1B[0;37m. Profs %s:\x1B[1;32m%d\x1B[0;37m. \n\r", 
	     (align_info[Align].max_skills > 0 ? "Bonus" : "Penalty"),align_info[Align].max_skills,
	     (align_info[Align].max_spells > 0 ? "Bonus" : "Penalty"),align_info[Align].max_spells,
	     (align_info[Align].max_profs > 0 ? "Bonus" : "Penalty"),align_info[Align].max_profs);
    send_to_char(buf, ch);
    sprintf (buf, "Natural Armor Bonus:\x1B[1;34m%d\x1B[0;37m.\n\rStat Limit Modifiers: \x1B[1;35mS: %d  D: %d  C: %d  I: %d W: %d\x1B[0;37m.\n\r",
	     (align_info[Align].natarmor),
	     align_info[Align].limits[0], align_info[Align].limits[3],
	     align_info[Align].limits[4], align_info[Align].limits[1],
	     align_info[Align].limits[2]);
    send_to_char(buf, ch);
    
    sprintf (buf, "Natural abilities: \x1B[1;36m%s\x1B[0;37m.\n\r",
	     natural_abilities (align_info[Align].affect_bits));
    send_to_char(buf, ch);
    sprintf (buf, "Natural abilities2: \x1B[1;36m%s\x1B[0;37m.\n\r",
	     natural_abilities2 (align_info[Align].affect_bits2));
    send_to_char(buf, ch);
    sprintf (buf, "Natural Enhancements: \x1B[1;36m%s\x1B[0;37m.\n\r",
	     augmentss (align_info[Align].nat_augments));
    send_to_char(buf, ch);
    
    if (align_info[Align].regen_hit_percent != 0)
      {
	sprintf (buf, "Hitpoint regeneration %s is \x1B[1;37m%d\x1B[0;37m percent from normal.\n\r", 
		 (align_info[Align].regen_hit_percent > 0 ? "bonus" : "penalty"), align_info[Align].regen_hit_percent);
	send_to_char(buf, ch);
      }
    if (align_info[Align].regen_move_percent != 0)
      {
	sprintf (buf, "Movement regeneration %s is \x1B[1;37m%d\x1B[0;37m percent from normal.\n\r", 
		 (align_info[Align].regen_move_percent > 0 ? "bonus" : "penalty"),align_info[Align].regen_move_percent);
	send_to_char(buf, ch);
      }
    if (align_info[Align].attack_spell_percent != 00)
      {
	sprintf (buf, "Attack spell %s is \x1B[1;37m%d\x1B[0;37m from normal.\n\r",
		 (align_info[Align].attack_spell_percent > 0 ? "bonus" : "penalty"),align_info[Align].attack_spell_percent);
	send_to_char(buf, ch);
      }
    if (align_info[Align].combat_damage_bonus != 0)
      {
	sprintf (buf, "This alignment has a natural attack power %s of \x1B[1;37m%d\x1B[0;37m from normal.\n\r", 
		 (align_info[Align].combat_damage_bonus > 0 ? "bonus" : "penalty"), align_info[Align].combat_damage_bonus);
	send_to_char(buf, ch);
      }
    if (align_info[Align].backstab_percent != 0)
      {
	sprintf (buf, "Backstab %s of \x1B[1;37m%d\x1B[0;37m from normal.\n\r",
		 (align_info[Align].backstab_percent > 0 ? "bonus" : "penalty"), align_info[Align].backstab_percent);  
	send_to_char(buf, ch);
      }
    if (align_info[Align].heal_percent != 0)
      {
	sprintf (buf, "Healing spell %s is \x1B[1;37m%d\x1B[0;37m percent from normal.\n\r",
		 (align_info[Align].heal_percent > 0 ? "bonus" : "penalty"),align_info[Align].heal_percent); 
	send_to_char(buf, ch);
      }
    if (align_info[Align].hps_bonus != 0)
      {
	sprintf (buf, "This race %s a %s \x1B[1;37m%d\x1B[0;37m hit points gained per level.\n\r",
		 (align_info[Align].hps_bonus > 0 ? "gains" : "has"),
		 (align_info[Align].hps_bonus > 0 ? "bonus" : "penalty"),
		 (align_info[Align].hps_bonus));
	send_to_char(buf, ch);
      }
    if (align_info[Align].mana_percent != 0)
      {
	sprintf (buf, "This race stores \x1B[1;37m%d\x1B[0;37m percent %s mana than normal.\n\r", align_info[Align].mana_percent,
          (align_info[Align].mana_percent > 0 ? "more" : "less"));
	send_to_char(buf, ch);
      }
    for (i = 0; i < 3; i ++)
	  {
	    if (align_info[Align].nat_skills[i] > 0)
	      {
		if ((spl = skill_lookup (NULL,align_info[Align].nat_skills[i])) != NULL) 
		  {
		    sprintf(buf, "This alignment can use %s naturally.\n\r", spl->spell_funky_name);
		    send_to_char(buf, ch);
		  }
	      }
	  }
  
    send_to_char ("--------------------------------------------------------------------------\n\r", ch);
  }
else
  {
    send_to_char("aligninfo or aligninfo <alignname> or aligninfo <align_number>.\n\r", ch);
  }
return;
}

void
do_ascend (CHAR_DATA * ch, char * argy)
{
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *next_obj;
  int k;
  char workstr[STD_LENGTH];
  char buf4[SML_LENGTH];

  DEFINE_COMMAND ("ascend", do_ascend, POSITION_DEAD, 0, LOG_NORMAL, "This command causes your character to assume a more powerful race")
  
  if (IS_MOB(ch))
    return;
  if (ch->pcdata->race == 0)
  {
    send_to_char("Humans may never ascend!!\n\r", ch);
    return;
  }
  if (!str_cmp (argy, "cost"))
  {
    send_to_char ("Ascension Requirements:\n\r", ch);
    sprintf (buf4, "Lvl: %2d Remorts: %2d Wpts: %4d Ttl Wpts: %4d\n\r", pow.ascension_req[0], pow.ascension_req[1], pow.ascension_req[2], pow.ascension_req[3]);
    send_to_char (buf4, ch);
    return;
  }
  if (pow.ascension_req[4] > 0 && get_room_index(pow.ascension_req[4]) != NULL
      && ch->in_room->vnum != pow.ascension_req[4] )
  {
    send_to_char ("You are not in the proper place to Ascend!\n\r", ch);
    return;
  }
  if (str_cmp (argy, "character"))
  {
    send_to_char ("You must type ascend character to ascend.\n\r", ch);
    return;
  }
  if (ch->position != POSITION_STANDING)
  {
    send_to_char ("You must be standing to ascend!\n\r", ch);
    return;
  }
  if (LEVEL(ch) < pow.ascension_req[0])
  {
    sprintf (buf4, "You must be at least level %d in order to ascend.\n\r", pow.ascension_req[0]);
    send_to_char (buf4, ch);
    return;
  }
  if (ch->pcdata->remort_times < pow.ascension_req[1])
  {
    sprintf (buf4, "You must have remorted at least %d times in order to ascend.\n\r", pow.ascension_req[1]);
    send_to_char (buf4, ch);
    return;
  }
  if (ch->pcdata->warpoints < pow.ascension_req[2])
  {
    sprintf (buf4, "You must have at least %d current warpoints in order to ascend.\n\r", pow.ascension_req[2]);
    send_to_char (buf4, ch);
    return;
  }
  if (ch->pcdata->total_wps < pow.ascension_req[3])
  {
    sprintf (buf4, "You must have accumulated at least %d total warpoints in order to ascend.\n\r", pow.ascension_req[3]);
    send_to_char (buf4, ch);
    return;
  }
  if (ch->pcdata->race > 16)
  {
    send_to_char ("You are unable to refine yourself any further through ascension.\n\r", ch);
    return;
  }
  
  //Do Actual Ascension here
/*Begin transforming the character */
  for (obj = ch->carrying; obj != NULL; obj = next_obj)
    {
      next_obj = obj->next_content;
      if (IS_SET (obj->pIndexData->extra_flags, ITEM_SAVE))
	continue;
      if (pow.vaporize_inv && (obj->wear_loc == -1 || obj->contains != NULL))
	{
	  sprintf (workstr, "%s just vaporized!!\n\r", capitalize (obj->pIndexData->short_descr));
	  send_to_char (workstr, ch);
	  free_it (obj);
	  continue;
	}
      else if (obj->wear_loc != -1)
	unequip_char (ch, obj);
      if (pow.vaporize_all)
	{
	  sprintf (workstr, "%s just vaporized!!\n\r", capitalize (obj->pIndexData->short_descr));
	  send_to_char (workstr, ch);
	  free_it (obj);
	  continue;
	}
    }
  send_to_char ("Your worn items were removed to inventory.\n\r", ch);
  ch->pcdata->level = 1;
  ch->exp = 1;
  check_fgt(ch);
  free_string(ch->fgt->phunting);
  ch->fgt->phunting = NULL;
  free_string(ch->fgt->hunting);
  ch->fgt->hunting = NULL;
  ch->pcdata->voting_on = 0;
  ch->pcdata->deaths =0;
  REMOVE_BIT(ch->act, PLR_PKED_THIS_MORT);
  ch->pcdata->practice = 10;
  ch->pcdata->learn = 2;
	  //Reset All Important Character Data
          ch->pcdata->remort_times = 0;
/*	  ch->pcdata->total_wps = 0; */
/*	  ch->pcdata->warpoints = 0; KEEP THIS FOR RECORDS */
	  ch->pcdata->guilds = 0;
/*	  ch->pcdata->augments = 0; */ /* STRUCTURE fixed augs being 
removed */
	  ch->pcdata->totalkills = 0;
	  ch->pcdata->killpoints = 0;
	  ch->pcdata->pklevels = 0;
	  ch->pcdata->pkhelpers = 0;
	  ch->pcdata->hard_pkills = 0;
	  ch->pcdata->pkills = 0;
	  ch->pcdata->pkilled = 0;
	  ch->pcdata->pkers = 0;
	  ch->pcdata->ascend_times++;
          for (k = 0; k < MAX_TROPHY; k++)
	  {
		  ch->pcdata->trophy_name[k][0] = '\0';
                  ch->pcdata->trophy_times[k] = 0;
                  ch->pcdata->trophy_level[k] = 0;
	  }
 	  
	  //Do A Global Echo to impress people!
	  {
		  CHAR_DATA *sch;
				
		  sch = create_mobile(get_mob_index(1));					  
		  char_to_room(sch, get_room_index(2));
				  		
		  sprintf(buf4, "$B$5M$1y$N$1t$5hi$1c$Ba$B$5l $0power surges from the astral plane rushing towards the room of");				
		  do_echo(sch, buf4);
				
		  sprintf(buf4, "$B$0transformation!", NAME(ch));				
		  do_echo(sch, buf4);
				
		  sprintf(buf4, "$B$1%s $0utters a scream heard throughout the land!", NAME(ch));			  
		  do_echo(sch, buf4);

		  //Send Message to Character
		  send_to_char("Your body is wracked with pain as you undergo a metamorphosis.\n\r", ch);

		  //Do Actual Race Change
		  raceswitch(ch, ch->pcdata->race + 8);
	  //Reset Stats to what they were rolled at
	  for (k = 0; k < NUM_STATS;k ++)
	  {
		  ch->pcdata->stat[k] = ch->pcdata->start_stat[k];
	  }
	  //Add stats depending on your race for ascending -MATUSE
          if (ch->pcdata->race == 9)
	  {
            ch->pcdata->stat[2] += 2;
	    ch->pcdata->stat[1] += 1;
	  }
          else if (ch->pcdata->race == 10)
	  {
            ch->pcdata->stat[2] += 1;
	    ch->pcdata->stat[1] += 2;
	  }
          else if (ch->pcdata->race == 11)
	  {
            ch->pcdata->stat[0] += 1;
	    ch->pcdata->stat[3] += 2;
	  }
          else if (ch->pcdata->race == 12)
	  {
            ch->pcdata->stat[4] += 1;
	    ch->pcdata->stat[3] += 2;
	  }
          else if (ch->pcdata->race == 13)
	  {
            ch->pcdata->stat[0] += 1;
	    ch->pcdata->stat[4] += 2;
	  }
          else if (ch->pcdata->race == 14)
	  {
            ch->pcdata->stat[4] += 1;
	    ch->pcdata->stat[0] += 2;
	  }
          else if (ch->pcdata->race == 15)
	  {
            ch->pcdata->stat[0] += 1;
	    ch->pcdata->stat[1] += 1;
	    ch->pcdata->stat[4] += 1;
	  }
          else if (ch->pcdata->race == 16)
	  {
            ch->pcdata->stat[0] += 1;
	    ch->pcdata->stat[2] += 1;
	    ch->pcdata->stat[4] += 1;
	  }
          else if (ch->pcdata->race == 17)
	  {
            ch->pcdata->stat[2] += 4;
	    ch->pcdata->stat[1] += 2;
	  }
          else if (ch->pcdata->race == 18)
	  {
            ch->pcdata->stat[2] += 2;
	    ch->pcdata->stat[1] += 4;
	  }
          else if (ch->pcdata->race == 19)
	  {
            ch->pcdata->stat[3] += 4;
	    ch->pcdata->stat[0] += 2;
	  }
          else if (ch->pcdata->race == 20)
	  {
            ch->pcdata->stat[3] += 4;
	    ch->pcdata->stat[4] += 2;
	  }
          else if (ch->pcdata->race == 21)
	  {
            ch->pcdata->stat[0] += 4;
	    ch->pcdata->stat[4] += 2;
	  }
          else if (ch->pcdata->race == 22)
	  {
            ch->pcdata->stat[4] += 4;
	    ch->pcdata->stat[0] += 2;
	  }
          else if (ch->pcdata->race == 23)
	  {
            ch->pcdata->stat[0] += 2;
	    ch->pcdata->stat[1] += 2;
	    ch->pcdata->stat[4] += 2;
	  }
          else if (ch->pcdata->race == 24)
	  {
            ch->pcdata->stat[0] += 1;
	    ch->pcdata->stat[2] += 1;
	    ch->pcdata->stat[4] += 1;
	  }

		  set_initial_hp(ch); 

		  sprintf(buf4, "A pulse of power surges throughout the lands as a new %s rises!", race_info[ch->pcdata->race].name);
		  do_echo(sch, buf4);		 

		  //Do Global Ascension Message
		  sprintf(buf4, "$B$1%s$B$0 has $B$4AsCeNdEd!$N", NAME(ch));
		  do_echo(sch, buf4);
			
		  //Get rid of that sexy reboot mob!
		  extract_char(sch, TRUE);
	  }
  for (k = 0; k < SKILL_COUNT; k++)
    {
      ch->pcdata->learned[k] = -100;
    }
  natural_mana (ch);
  if (pow.beams_to_room > 0)
    {
      int to_room;
      if (pow.beams_to_room == 1)
	{
	  to_room = (ALIGN(ch) + 100);
	}
      else
	to_room = pow.beams_to_room;
      char_from_room (ch);
      char_to_room (ch, get_room_index (to_room));
      do_look (ch, "auto");
    }
	  send_to_char ("\x1B[1;37mMetamorphosis complete!!!\x1B[37;0m\n\r", ch);
	  send_to_char ("You have purged yourself of your previous life and transcended.\n\r",ch);

  max_remort_times[ch->pcdata->alignment] = 
   UMAX(max_remort_times[ch->pcdata->alignment], ch->pcdata->remort_times);
  
  fix_char(ch);
  do_save (ch, "xx2xx11");
  return; 
}

void
do_forsake (CHAR_DATA * ch, char * argy)
{
  char buf4[SML_LENGTH];
  
  DEFINE_COMMAND ("forsake", do_forsake, POSITION_DEAD, 0, LOG_NORMAL, "This command allows one to forsake their alignment")
  
  if (IS_MOB(ch))
    return;
  if(!str_cmp (argy, "cost"))
  {
	send_to_char ("Forsake Requirements:\n\r", ch);
	sprintf (buf4, "Lvl: %2d Remorts: %2d Wpts: %4d\n\r", pow.forsake_req[0], pow.forsake_req[1], pow.forsake_req[2]);
	send_to_char (buf4, ch);
	return;
  }
  if(str_cmp (argy, "alignment yes"))
  {
	  send_to_char ("You must type forsake alignment yes to forsake your alignment.\n\r", ch);
	  return;
  }
  if(ch->position != POSITION_STANDING)
  {
    send_to_char("You must be standing to forsake!\n\r", ch);
    return;
  }
  
  if (ch->pcdata->remort_times < pow.forsake_req[1])
  {
	  sprintf (buf4, "You must have remorted at least %d times in order to forsake.\n\r", pow.forsake_req[1]);
	  send_to_char(buf4, ch);
	  return;
  }

  if (LEVEL(ch) < pow.forsake_req[0])
  {
    sprintf (buf4, "You must be at least level %d in order to forsake.\n\r", pow.forsake_req[0]);
    send_to_char (buf4, ch);
    return;
  }

  if (ch->pcdata->warpoints < pow.forsake_req[2])
  {
    sprintf (buf4, "You must have at least %d current warpoints to forsake your alignment.\n\r", pow.forsake_req[2]);
    send_to_char (buf4, ch);
    return;
  }
  if (pow.forsake_req[4] > 0 && get_room_index(pow.forsake_req[4]) != NULL && ch->in_room->vnum != pow.forsake_req[4] )
    {
      send_to_char("You are not in the proper place to forsake!\n\r", ch);
      return;
    }
  {
  CHAR_DATA *sch;
  sch = create_mobile(get_mob_index(1));
  char_to_room(sch, get_room_index(2));

  ch->pcdata->warpoints = ch->pcdata->warpoints - 100;
  send_to_char ("You abandon your alignment to the whims of fate.", ch);
  alignswitch(ch, 3);
  char_from_room(ch);
  char_to_room(ch, get_room_index(103));

  sprintf(buf4, "$B$0A clap of $6thunder$0 echos throughout the land as");				
  do_echo(sch, buf4);
				
  sprintf(buf4, "$N$B%s $B$0has chosen to walk the path of the traitor and abandon his", NAME(ch));
  do_echo(sch, buf4);
				
  sprintf(buf4, "$B$0alignment to the winds of fate.", NAME(ch));				
  do_echo(sch, buf4);
  
  sprintf(buf4, "$B$0Another soul joins the ranks of the forsaken!$N"); 				
  do_echo(sch, buf4);  

  extract_char(sch, TRUE);
  }
  return;
}  

 void
do_remort (CHAR_DATA * ch, char * argy)
{
  char buf[SML_LENGTH];
  int remort = 0;
  bool ready = TRUE;
  DEFINE_COMMAND ("remort", do_remort, POSITION_DEAD, 0, LOG_NORMAL, "This command transfers worthy players to the remort room where they can TRANSFORM.") 
    if (IS_MOB(ch))
      return;
  if(ch->position !=POSITION_STANDING)
    {
      send_to_char("You must be standing to remort!!!\n\r", ch);
      return;
    }
  if (argy == "" || argy == '\0' || !str_cmp(argy, "help"))
    {
      info_remort(ch, argy);
      return;
    }
  if (!str_cmp (argy, "requirements") || !str_cmp(argy, "cost") ||
!str_cmp(argy, "costs"))
    {
      if ((ch->pcdata->race > 10 &&
	   pow.asctransform_in_room > 0) ||
	  (ch->pcdata->race < 11 &&
	   pow.transform_in_room > 0))
      {
        show_remort_requirements (ch, argy);
        return;
      }
      else
      {
	if (number_range(0,1) == 1)
	send_to_char ("Huh?\n\r", ch);
	else if (number_range(0,1) == 1)
	send_to_char ("What? (Type HELP for help)\n\r", ch);
	else
	send_to_char ("Unrecognized Command.\n\r", ch);
	return;
      }
    }
  if (str_cmp (argy, "remort"))
    {
      info_remort(ch, "");
      return;
    }
   if (ch->pcdata->race < 11 && pow.remort_in_room[ch->pcdata->remort_times] > 0 &&
       get_room_index(pow.remort_in_room[ch->pcdata->remort_times]) != NULL && 
       ch->in_room->vnum != pow.remort_in_room[ch->pcdata->remort_times] )
    {
      send_to_char("You are not in the proper place to remort!\n\r", ch);
      return;
    }
  if (ch->pcdata->race > 10 && pow.ascend_remort_in_room[ch->pcdata->remort_times] > 0 && get_room_index(pow.ascend_remort_in_room[ch->pcdata->remort_times]) != NULL && ch->in_room->vnum != pow.ascend_remort_in_room[ch->pcdata->remort_times] )
    {
      send_to_char("You are not in the proper place to remort!\n\r", ch);
      return;
    }
  remort = ch->pcdata->remort_times;
  if ((ch->pcdata->race < 11 && (LEVEL(ch) < pow.remort_req[remort][0])) 
	|| (ch->pcdata->race > 10 && (LEVEL(ch) < pow.ascended_remort_req[remort][0]))) 
    {
      send_to_char("You need to gain more levels before you can remort!\n\r", ch);
      ready = FALSE;
    }
  if ((ch->pcdata->race < 11 && (ch->pcdata->warpoints < pow.remort_req[remort][1])) || (ch->pcdata->race > 10 && (ch->pcdata->warpoints < pow.ascended_remort_req[remort][1])))
    {
      send_to_char("You need to pkill more before we will allow you to remort!\n\r", ch);
      ready = FALSE;
    }
  if ((ch->pcdata->race < 11 && (ch->pcdata->killpoints < pow.remort_req[remort][3])) || (ch->pcdata->race > 10 && (ch->pcdata->killpoints < pow.ascended_remort_req[remort][3]))) 
    {
      send_to_char("You need to kill more tough monsters before you can remort!\n\r", ch);
      ready = FALSE;
    }
  if (!ready)
    return;
  char_from_room(ch);
  if ((pow.transform_in_room > 0 && 
	get_room_index(pow.transform_in_room) != NULL && 
	ch->pcdata->race < 11) ||
	(pow.asctransform_in_room > 0 &&
	get_room_index(pow.asctransform_in_room) != NULL &&
	ch->pcdata->race > 10))
    char_to_room(ch, get_room_index(pow.transform_in_room));
  {
    CHAR_DATA *sch;
    sch=create_mobile(get_mob_index(1));
    char_to_room(sch,get_room_index(2));
    sprintf(buf, "We, the immortals, decree that %s is fit to be reborn!\n\r", NAME(ch));
    do_echo(sch, buf);
    extract_char(sch, TRUE);
  }
  if (ch->pcdata->race < 11)
   {
  	ch->pcdata->warpoints -= pow.remort_req[remort][2];
  	ch->pcdata->killpoints -= pow.remort_req[remort][4];
   }
  else
   {
	ch->pcdata->warpoints -= pow.ascended_remort_req[remort][2];
	ch->pcdata->killpoints -= pow.ascended_remort_req[remort][4];
   }
}

void
do_choosesides (CHAR_DATA * ch, char *argy)
{
  int num;
  char buf[200];

DEFINE_COMMAND ("choosesides", do_choosesides, POSITION_DEAD, 0, LOG_NORMAL, "This command is the one that lets newbies choose sides.")

  if (IS_MOB(ch)) return;

if (pow.choose_in_room > 0 && (!ch->in_room || ch->in_room->vnum != pow.choose_in_room))
  {
    send_to_char("You are not in the room of choosing.\n\r", ch);
    return;
  }

if(IS_PLAYER(ch)) {
  if(LEVEL(ch) < 0 && ch->pcdata->remort_times == 0) {
    sprintf(buf, "You still need %d %s before you can choose an alignment.\n\r",40-LEVEL(ch),40-LEVEL(ch) == 1 ? "level" : "levels");
    send_to_char(buf,ch);
    return;
  }
}

if (is_number(argy))
  {
    num = atoi(argy);
    if (num > 0 && num < NUM_ALIGN && num != 3)
      {
	sprintf(buf, "\n\r\n\r\n\r\x1b[1;31m Ok, you choose side %d.\n\r\n\r\n\r", num);
	send_to_char(buf, ch);
	alignswitch(ch, num);
	char_from_room(ch);
	char_to_room(ch, get_room_index(100+num));
	return;
      }
  }
sprintf(buf, "Please type aligninfo and choose a number from 1 to %d.\n\r", (NUM_ALIGN-2));
send_to_char(buf, ch);
return;
}



void 
write_playerbase_to_file (void)
{
  FILE *fff;
  char write_one_line[500];
  PLAYERBASE_DATA *playerbase;
  int  tps, np, rp, average_level, level_tot;
  tps = 0;
  np = 0;
  rp = 0;
  average_level = 0;
  level_tot = 0;
  if ((fff = fopen ("player.txt", "w+")) == NULL)
    {
      fprintf (stderr, "The write failed! Bad news!! VERY bad news!!!!\n\r");
      return;
    }
  for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
    {
      if (playerbase->player_level != -1)
	{
	  tps++;
	  if(playerbase->player_level <= pow.level_to_choose && playerbase->player_remort_times == 0)
	    np++;
	  if (playerbase->player_remort_times > 0)
	    rp++;
	  level_tot += (playerbase->player_level);
	  sprintf (write_one_line, "%-17s R: %d Lvl: %3d Last_on: %4ld hrs NoD: %s\n",
		   playerbase->player_name,
		   (playerbase->player_remort_times),
		   (playerbase->player_level),
		   ((current_time - playerbase->player_last_login) / 3600),
		   (playerbase->player_nodelete ? "y" : "n"));
	  fprintf (fff, write_one_line);
	}
    }
  if (tps == 0)
    average_level = 0;
  else
    average_level = level_tot / tps;
  fprintf (fff, "Total Playerbase size: %d. Max Playerbase size: Unlimited.\n\r",
	   tps);
  fprintf (fff, "Newbies: %d. Remorted players: %d. Average Lvl: %d.\n\r",
	   np, rp, average_level);
  fclose (fff);
  return;
}

void 
do_players (CHAR_DATA * ch, char *argy)
{
  int num_total, level_total, average_level;
  int average_lev[NUM_ALIGN];
  int level_tot[NUM_ALIGN];
  int num_tot[NUM_ALIGN];
  int num_remorts[NUM_ALIGN][pow.max_remorts+1];
  int maximum_remorts[NUM_ALIGN];
  int i, jj; 
 
  PLAYERBASE_DATA *playerbase;
  DEFINE_COMMAND ("players", do_players, POSITION_DEAD, 0, LOG_NORMAL, "This command lists the playerbase size, and gives some general info.")
    
    num_total = 0;
  level_total = 0;
  average_level = 0;
  for (jj = 0; jj < NUM_ALIGN; jj++)
    {
      average_lev[jj] = 0;
      level_tot[jj] = 0;
      num_tot[jj] = 0;
      maximum_remorts[jj] = 0;
      for (i = 0; i < (pow.max_remorts+1); i++)
	{
	  num_remorts[jj][i] = 0;
	}
    }
  if (!str_cmp (argy, "file") && ch->pcdata->level == MAX_LEVEL)
    write_playerbase_to_file ();
  for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
    {
      if (playerbase->player_level > 91)
	continue;
      num_total++;
      level_total += playerbase->player_level;
      level_tot[playerbase->player_align] += playerbase->player_level;
      num_tot[playerbase->player_align]++;
      num_remorts[playerbase->player_align][playerbase->player_remort_times]++;
      if (playerbase->player_remort_times > maximum_remorts[playerbase->player_align])
	maximum_remorts[playerbase->player_align] = playerbase->player_remort_times;
    }
  
  if (num_total == 0)
    average_level = 0;
  else
    average_level = level_total / num_total;
  for (jj = 0; jj < NUM_ALIGN; jj++)
    {
      if (num_tot[jj] == 0)
	average_lev[jj] = 0;
      else
	average_lev [jj] = level_tot[jj]/num_tot[jj];
    }
  sprintf (update_char, "Total Playerbase size: \x1B[1;37m%d\x1B[0m. Average Playerbase Level: \x1b[1;37m%d\x1b[0;37m.\n\r",
	   num_total, average_level);
  send_to_char (update_char, ch);
  for (jj = 0; jj < NUM_ALIGN; jj ++)
    {
      if (num_tot[jj] > 0)
        {
	sprintf(update_char, "Total members of %s: %d, Average Level: %d, Max Remort: %d\n\r", align_info[jj].name, num_tot[jj], average_lev[jj],maximum_remorts[jj]);
      send_to_char(update_char, ch);
        }
    }
  if (!str_cmp(argy, "full") && LEVEL(ch) == MAX_LEVEL)
    {
      for (i=0; i < (pow.max_remorts+1); i++)
	{
	  update_char[0] = '\0';
	  for (jj = 0; jj < NUM_ALIGN; jj++)
	    {
	      
	      sprintf(update_char, "%4d", num_remorts[jj][i]);
	      send_to_char(update_char,ch);
	    }
	  send_to_char("\n\r", ch);
	}
    }
  
  return;
}

void 
do_pdelete (CHAR_DATA * ch, char *argy)
{
  PLAYERBASE_DATA *playerbase;
  CHAR_DATA *ccc;
  DEFINE_COMMAND ("pdelete", do_pdelete, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command is used to delete characters.")

/*    if (IS_MOB (ch))
    {
      do_echo(ch, "This didnt work cuz you are a mob!!\n\r");
      return;
    }
*/
  if (IS_PLAYER(ch) && !IS_REAL_GOD (ch))
    {
      do_echo(ch, "This didnt work cuz you aren't a real god!!\n\r");
      return;
    }
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("Syntax is PDELETE <playername>.\n\r", ch);
      return;
    }
  ccc = get_char_world (ch, argy);
  if (ccc != NULL && IS_PLAYER (ccc))
    {
      do_delete (ccc, "character forever");
    }
  for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
    {
      if (!str_cmp (argy, playerbase->player_name))
	{
	  sprintf (update_char, "%s%s.cor", PLAYER_DIR, playerbase->player_name);
	  unlink (update_char);
	  
	  
	  sprintf (update_char, "%s%s", PLAYER_DIR, playerbase->player_name);
	  unlink (update_char);
	       /* sprintf(update_char,"rm \"%s%s\" &",PLAYER_DIR,jumble_name(playerbase->player_name,FALSE));
	     system(update_char); */
	  playerbase->player_level = -1;
	  send_to_char ("Character deleted!\n\r", ch);
	  save_playerbase ();
	  return;
	}
    }
  send_to_char ("Player not found.\n\r", ch);
  return;
}

void 
do_nodelete (CHAR_DATA * ch, char *argy)
{
  PLAYERBASE_DATA *playerbase;
  DEFINE_COMMAND ("z_nodelete", do_nodelete, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This toggles a no delete flag on/off for a given player.")

    if (argy == "" || argy[0] == '\0')
    {
      char line[3][17];
      char outbuf[200];
      line[0][0] = '\0';
      line[1][0] = '\0';
      line[2][0] = '\0';
      send_to_char ("Syntax is NODELETE <charname>.\n\r", ch);
      send_to_char ("\n\rCurrent no_delete flags on:\n\r", ch);
      for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
	{
	  if (playerbase->player_level == -1)
	    continue;
	  if (playerbase->player_nodelete)
	    {
	      if (line[0][0] == '\0')
		strcpy (line[0], playerbase->player_name);
	      else if (line[1][0] == '\0')
		strcpy (line[1], playerbase->player_name);
	      else if (line[2][0] == '\0')
		strcpy (line[2], playerbase->player_name);
	      else
		{
		  sprintf (outbuf, "%17s %17s %17s\n\r", line[0], line[1], line[2]);
		  send_to_char (outbuf, ch);
		  strcpy (line[0], playerbase->player_name);
		  line[1][0] = '\0';
		  line[2][0] = '\0';
		}
	    }
	}
      sprintf (outbuf, "%17s %17s %17s\n\r",
	       (line[0][0] == '\0' ? " " : line[0]),
	       (line[1][0] == '\0' ? " " : line[1]),
	       (line[2][0] == '\0' ? " " : line[2]));
      send_to_char (outbuf, ch);
      return;
    }
  for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
    {
      if (!str_cmp (argy, playerbase->player_name))
	{
	  if (playerbase->player_nodelete)
	    {
	      send_to_char ("PLAYER NOW CAN BE DELETED AFTER INACTIVITY!\n\r", ch);
	      playerbase->player_nodelete = FALSE;
	      save_playerbase ();
	      return;
	    }
	  else
	    {
	      send_to_char ("Player is now safe from inactivity deletion.\n\r", ch);
	      playerbase->player_nodelete = TRUE;
	      save_playerbase ();
	      return;
	    }
	}
    }
  send_to_char ("I could not find that player in the database.\n\r", ch);
  return;
}

void 
show_remort (CHAR_DATA * ch)
{
  send_to_char ("You should read help Transform first, before you go any further.\n\r", ch);
  send_to_char ("\n\rThe correct syntax for transforming is:\n\r", ch);
  send_to_char ("Transform <\x1B[1;37mstat\x1B[0m> <\x1B[1;37mstat\x1B[0m> <\x1B[1;37mstat\x1B[0m>\n\r", ch);
  send_to_char ("<stat> is STR, DEX, CON, WIS, INT or NONE to specify null.\n\r", ch);
  send_to_char ("\n\rOr:\n\r", ch);
  if (pow.augmentremortonly)
  {
  send_to_char ("Transform <\x1B[1;37mstat\x1B[0m> <\x1B[1;37mstat\x1B[0m> <\x1B[1;37maugment\x1B[0m>\n\r", ch);
  send_to_char ("To view a list of augments type augment list.\n\r", ch);
  send_to_char ("\n\rOr:\n\r", ch);
  }
  send_to_char ("Transform race <\x1B[1;37mnumber\x1B[0m> - Switches your race (REMORT 2 and BELOW)\n\r", ch);
  return;
}

void
show_donate (CHAR_DATA * ch)
{
  send_to_char ("You should read help Donate first, before continuing.\n\r", ch);
  send_to_char ("The proper syntax is:\n\n\r", ch);
  send_to_char ("Donate  <amount> <warpoints/gold> <city/clan>.\n\n\r", ch);
  send_to_char ("You must donate at least 100 gold or warpoints each time.\n\r", ch);
  return;
}
