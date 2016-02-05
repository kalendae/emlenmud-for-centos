#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

static bool used_spell[SKILL_COUNT];
static bool vert_line[100];


void do_new_prereq (CHAR_DATA *ch, char *argy)
{
  SPELL_DATA *spell;
  int i;

  DEFINE_COMMAND ("zprerequisite", do_new_prereq, POSITION_DEAD, 0, LOG_NORMAL, "Allows you to see prerequisites to the skill-big tree format.")
    
    if (IS_MOB (ch))
      return;
  for (i = 0; i < 100; i ++)
    vert_line[i] = FALSE;
   for (i = 0; i < SKILL_COUNT; i++)
    used_spell[i] = FALSE;
 
  
  if (argy[0] == '\0' || (spell = skill_lookup (argy, -1)) == NULL)
    {
      send_to_char ("Syntax: Prereq <skill/spell/prof name>\n\r", ch);
      return;
    }
  xprereq(ch, spell);
  

  /* Now clean up our static bool */
 
  return;
}

void 
xprereq (CHAR_DATA *ch, SPELL_DATA *spell)
{
  static int deppy = 0;
  int jj;
  int diff = 0;
  int len = 3*deppy+3;
  char buf[STD_LENGTH];
  char buff[100];
  if (used_spell[spell->gsn] == TRUE)
    return;
  sprintf(buf, " ");
  if(deppy > 0)
    {
      for (jj = 0; jj < deppy; jj++)
	{
	  if (!vert_line[jj])
	    strcat(buf, "...");
	  else
	    strcat(buf, "|..");
	}
    }
  strcat(buf, "\\__ ");
  strcat(buf, spell->spell_name);
  len += strlen(spell->spell_name);
  diff = 70-len;
  if (diff > 0)
    {
      for (jj = 0; jj < diff; jj++)
	{
	  strcat(buf, ".");
	}
    }
  sprintf(buff, "[%d]\n\r", spell->num_prereqs);
  strcat(buf, buff);
  send_to_char(buf, ch);
  deppy++;
  used_spell[spell->gsn] = TRUE;
  vert_line[deppy] = FALSE;
  if (spell->pre1 != NULL && spell->pre2 != NULL && !used_spell[spell->pre1->gsn] && !used_spell[spell->pre2->gsn])
    {
      vert_line[deppy] = spell->go_to_second;
    }
  
  if (spell->pre1 != NULL )
    xprereq(ch, spell->pre1);
  vert_line[deppy] = FALSE;
  if (spell->pre2 != NULL)
    xprereq(ch, spell->pre2);
  deppy--;
  return;
}

bool
check_prereq_depth(SPELL_DATA *spell, SPELL_DATA *prereq)
{
  static int depthy = 0;
  if (depthy > 13) return TRUE;
  if (prereq == spell->pre1 || (prereq == spell->pre2 && depthy > 0))
    return FALSE;
  depthy++;
  if (spell->pre1 != NULL && !check_prereq_depth(spell->pre1, prereq))
    {
      depthy--;
      return FALSE;
    }
  if (spell->pre2 != NULL && !check_prereq_depth(spell->pre2, prereq))
    {
      depthy--;
      return FALSE;
    }
  depthy--;
  
  return TRUE;
}

char *
how_good (CHAR_DATA *ch, int gsn)
{
  static char bf[256];
  int i;
  int learned = 0;
  SPELL_DATA *spl;
  bf[0] = '\0';
  if (IS_MOB(ch)) 
    {
      strcpy (bf, "No Mobs Plz");
      return bf;
    }
  
  if ((spl = skill_lookup(NULL, gsn)) == NULL)
    {
      strcpy (bf, "No spell!");
      return (bf);
    }
  else
    {
      if (learned != 100)
	learned = ch->pcdata->learned[gsn];
      for (i = 0; i < 20; i++)
	{
	  if (pow.learned[i] != NULL && learned >= pow.low_range[i] && learned <= pow.high_range[i])
	    {
	      strcpy (bf, pow.learned[i]);
	      return (bf);
	    }
	}
    }
return bf;
}


void 
do_learn (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  int sn;
  int skl, spl, prf, i;
  SPELL_DATA *spell;
  int cntt;
  CHAR_DATA *mob;
  DEFINE_COMMAND ("learn", do_learn, POSITION_STANDING, 0, LOG_NORMAL, "This command allows you to learn a skill or spell.  You must be at a practitioner.")

    cntt = 0;
  if (IS_MOB (ch))
    return;
  
  if (!IS_AWAKE (ch))
    {
      send_to_char ("In your dreams, or what?\n\r", ch);
      return;
    }

  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_MOB (mob) && IS_SET (mob->act, ACT_PRACTICE) && mob->pIndexData->opt)
	break;
    }

  if (mob == NULL || !IS_SET (mob->act, ACT_PRACTICE) || IS_PLAYER (mob))
    {
      send_to_char ("You can't learn anything here!\n\r", ch);
      return;
    }

  skl = tally_spsk (ch, 0);
  spl = tally_spsk (ch, 1);
  prf = tally_spsk (ch, 2);
  if (mob != NULL && (IS_MOB (mob)) && argy[0] == '\0')
    {
      int col;
      send_to_char ("\x1B[1;37m        You may learn these skills (that you do not already know):\x1B[37;0m\n\r", ch);
      send_to_char ("\x1B[1;34m---------------------------------------------------------------------------\x1B[37;0m\n\r", ch);
      col = 0;
      for (sn = 0; sn < 28; sn++)
	{
	  if (mob->pIndexData->opt->skltaught[sn] != 0 &&
	  ch->pcdata->learned[mob->pIndexData->opt->skltaught[sn]] == -100 &&
	      (spell = skill_lookup (NULL, mob->pIndexData->opt->skltaught[sn])) != NULL &&
	      LEVEL (ch) >= spell->spell_level)
	    {
	     
	      sprintf (buf, "[Level \x1B[1;37m%-2d\x1B[0;37m]\x1b[1;30m (\x1b[1;31m%2s\x1b[0;31m)\x1B[1;34m%18s\x1B[37;0m     ", spell->spell_level,(spell->slot == 0 ? "Sk" : (spell->slot == 1 ? "Sp" : "Pr")),(spell->spell_funky_name));
	      send_to_char (buf, ch);
	      if (++col % 2 == 0)
		send_to_char ("\n\r", ch);
	    }
	}
      sprintf (buf, "\n\r\n\rYou have \x1B[1;37m%d\x1B[37;0m learning sessions left.\n\rYou may learn \x1B[1;33m%d\x1B[37;0m more skills,\x1B[1;33m%d\x1B[37;0m more proficiencies and \x1B[1;33m%d\x1B[37;0m more spells.\n\r",
	       ch->pcdata->learn, 
	       (race_info[ch->pcdata->race].max_skills + align_info[ch->pcdata->alignment].max_skills +(ch->pcdata->remort_times +1)/2 - skl),
	       ( race_info[ch->pcdata->race].max_profs + align_info[ch->pcdata->alignment].max_profs + (ch->pcdata->remort_times +1)/2 - prf), 
	       (race_info[ch->pcdata->race].max_spells + align_info[ch->pcdata->alignment].max_spells +(ch->pcdata->remort_times)-spl));
      send_to_char (buf, ch);
      return;
    }

  if (ch->pcdata->learn <= 0)
    {
      send_to_char ("You have no learning sessions left.\n\r", ch);
      return;
    }

  if ((spell = skill_lookup (argy, -1)) == NULL
      || IS_MOB (ch))
    {
      send_to_char ("I know nothing about that.\n\r", ch);
      return;
    }
  
  if (spell->slot == 0 && skl >= (race_info[ch->pcdata->race].max_skills + align_info[ch->pcdata->alignment].max_skills +
(ch->pcdata->remort_times +1)/2))
    {
      send_to_char ("You have already learned the maximum skills for your race.\n\r", ch);
      return;
    }
  if (spell->slot == 2 && prf >= (race_info[ch->pcdata->race].max_profs + align_info[ch->pcdata->alignment].max_profs +
(ch->pcdata->remort_times +1)/2))
    {
      send_to_char ("You have already learned the maximum proficiencies for your race.\n\r", ch);
      return;
    } 
  if (spell->slot == 1 && spl >= (race_info[ch->pcdata->race].max_spells + align_info[ch->pcdata->alignment].max_spells + (ch->pcdata->remort_times)))
    {
      send_to_char ("You have already learned the maximum spells for your race.\n\r", ch);
      return;
    }

  if (mob == NULL || IS_PLAYER (mob))
    return;
  while (cntt <= 28)
    {
      if (spell->gsn == mob->pIndexData->opt->skltaught[cntt])
	{
	  cntt = 50;
	  break;
	}
      cntt++;
    }

  if (cntt != 50)
    {
      send_to_char ("I have no knowledge of that to teach you.\n\r", ch);
      return;
    }

  if (LEVEL (ch) < spell->spell_level)
    {
      send_to_char ("You find it too difficult to understand.\n\r", ch);
      return;
    }
  if (IS_PLAYER (ch))
    {
      SPELL_DATA *spl;
      if (spell->prereq_1 != NULL && (spl = skill_lookup(spell->prereq_1, -1)) != NULL && ch->pcdata->learned[spl->gsn] < pow.prereq)
	    {
	      sprintf (buf,
		       "You need to be skilled in %s first.\n\r", spl->spell_funky_name);
	      send_to_char (buf, ch);
	      return;
	    }
      if (spell->prereq_2 != NULL && (spl = skill_lookup(spell->prereq_2, -1)) != NULL && ch->pcdata->learned[spl->gsn] < pow.prereq)
	    {
	      sprintf (buf,
		       "You need to be skilled in %s first.\n\r", spl->spell_funky_name);
	      send_to_char (buf, ch);
	      return;
	    }


    }

  if (ch->pcdata->learned[spell->gsn] != -100)
    {
      sprintf (buf, "You already know %s.\n\r",
	       spell->spell_funky_name);
      send_to_char (buf, ch);
      return;
    }
  if (spell->guilds != 0)
    {
      int i;
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if(IS_SET(spell->guilds, guild_data[i].mob_guildmaster_bit) && !IS_SET(ch->pcdata->guilds, guild_data[i].mob_guildmaster_bit))
	    {
	      send_to_char("You do not have the proper guilds to learn this!\n\r", ch);
	      return;
	    }
	}
    }
  

    {
      ch->pcdata->learn--;
      ch->pcdata->learned[spell->gsn] = 1;
      act ("You learn $T.",
	   ch, NULL, spell->spell_funky_name, TO_CHAR);
/*act( "$n learns $T.",
   ch, NULL,spell->spell_funky_name,TO_ROOM); */
    }

  return;
}

void 
do_skill (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char hg[300];
  SPELL_DATA *spell;
  int sn;
  int col;
  int oldtrack;
  int oldtrap;
  int oldpick;
  DEFINE_COMMAND ("skills", do_skill, POSITION_DEAD, 0, LOG_NORMAL, "This command shows you what skills you know, if any.")

    if (IS_MOB (ch))
    return;
  hugebuf_o[0] = '\0';
  send_to_char ("\x1B[1;37mYou have knowledge in the following skills:\x1B[37;0m\n\r", ch);
  send_to_char ("\x1B[1;34m-------------------------------------------\x1B[37;0m\n\r", ch);
  col = 0;
  oldtrack = ch->pcdata->learned[gsn_track];
  oldtrap = ch->pcdata->learned[gsn_trapset];
  oldpick = ch->pcdata->learned[gsn_pick_lock];
  if (is_member (ch, GUILD_THIEFG))
    ch->pcdata->learned[gsn_pick_lock] = 100;
  if (is_member (ch, GUILD_RANGER) && is_member(ch, GUILD_THIEFG) &&
is_member(ch, GUILD_TINKER))
    ch->pcdata->learned[gsn_trapset] = 100;
 if (is_member (ch, GUILD_RANGER))
    ch->pcdata->learned[gsn_track] = 100; 
  ch->pcdata->learned[gsn_sneak] += ch->pcdata->plus_sneak;
  ch->pcdata->learned[gsn_hide] += ch->pcdata->plus_hide;
  for (sn = 0; sn < SKILL_COUNT; sn++)
    {
      spell = skill_lookup (NULL, sn);
      if (spell == NULL)
	continue;
      if ((LEVEL (ch) < spell->spell_level)
	  || (ch->pcdata->learned[sn] == -100) || spell->slot == 1)
	continue;
      strcpy (hg, how_good (ch, sn));
      if (LEVEL (ch) >= 100)
	sprintf (buf, "%s%22s %3d%%\x1b[0;37m ",(spell->slot == 2 ? "\x1b[1;36m" : ""), spell->spell_funky_name, ch->pcdata->learned[sn]);
      else
	sprintf (buf, "%s%22s %10s",(spell->slot == 2 ? "\x1b[1;36m" :
"\x1b[0;37m"), spell->spell_funky_name, hg);
      strcat (hugebuf_o, buf);
      if (++col % 2 == 0)
	strcat (hugebuf_o, "\n\r");
    }

  ch->pcdata->learned[gsn_track] = oldtrack; 
  ch->pcdata->learned[gsn_trapset] = oldtrap;
  ch->pcdata->learned[gsn_pick_lock] = oldpick;
  ch->pcdata->learned[gsn_sneak] -= ch->pcdata->plus_sneak;
  ch->pcdata->learned[gsn_hide] -= ch->pcdata->plus_hide;
  if (col % 2 != 0)
    strcat (hugebuf_o, "\n\r");
  sprintf (buf, "\n\rYou have %d practice sessions left.\n\r",
	   ch->pcdata->practice);
  strcat (hugebuf_o, buf);
  page_to_char (hugebuf_o, ch);
  return;
}


void 
do_practice (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char hg[500];
  bool found;
  int sn;
  int cnttt;
  SPELL_DATA *spell;
  CHAR_DATA *mob;
  int adept;
  DEFINE_COMMAND ("practice", do_practice, POSITION_STANDING, 0, LOG_NORMAL, "This command can be used to list all skills/spells you know, or, at a practitioner, can be used to practice a spell or skill.")
    hugebuf_o[0] = '\0';
  cnttt = 0;
  if (IS_MOB (ch))
    return;
  
  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_MOB (mob) && IS_SET (mob->act, ACT_PRACTICE) && mob->pIndexData->opt)
	break;
    }

  if (mob != NULL && (!IS_SET (mob->act, ACT_PRACTICE) || IS_PLAYER (mob)))
    {
      if (argy[0] != '\0')
	{
	  send_to_char ("You cannot practice here!\n\r", ch);
	  return;
	}
    }
  if (mob == NULL && argy[0] == '\0')
    {
      int col;
      int oldtrack;
      int oldtrap;
      int oldpick;
      sprintf (hugebuf_o, "You have knowledge in the following:\n\r\n\r");
      sprintf (hugebuf_o,"\x1B[1;37mYou have knowledge in the following:\x1B[37;0m\n\r");
      sprintf (hugebuf_o+strlen(hugebuf_o),"\x1B[1;34m------------------------------------\x1B[37;0m\n\r");
      col = 0;
      
      oldtrack = ch->pcdata->learned[gsn_track]; 
      if (is_member (ch, GUILD_RANGER))
	ch->pcdata->learned[gsn_track] = 100;
      oldtrap = ch->pcdata->learned[gsn_trapset]; 
      if (is_member (ch, GUILD_RANGER) && is_member(ch, GUILD_THIEFG) &&
	  is_member(ch, GUILD_TINKER))
	ch->pcdata->learned[gsn_trapset] = 100;
      oldpick = ch->pcdata->learned[gsn_pick_lock];
      if (is_member (ch, GUILD_THIEFG))
	ch->pcdata->learned[gsn_pick_lock] = 100;
      ch->pcdata->learned[gsn_sneak] += ch->pcdata->plus_sneak;
      ch->pcdata->learned[gsn_hide] += ch->pcdata->plus_hide;
      for (sn = 0; sn < SKILL_COUNT; sn++)
	{
	  if ((spell = skill_lookup (NULL, sn)) == NULL)
	    continue;
	  if ((LEVEL (ch) < spell->spell_level)
	      || (ch->pcdata->learned[sn] < -1))
	    continue;
	  strcpy (hg, how_good (ch, sn));
	  if (LEVEL (ch) >= 100)
	    sprintf (buf, "%22s %3d%% ", spell->spell_funky_name,
		     ch->pcdata->learned[sn]);
	  else
	    sprintf (buf, " %22s %10s", spell->spell_funky_name, hg);
	  strcat (hugebuf_o, buf);
	  if (++col % 2 == 0)
	    strcat (hugebuf_o, "\n\r");
	}
      ch->pcdata->learned[gsn_track] = oldtrack;
      ch->pcdata->learned[gsn_trapset] = oldtrap;
      ch->pcdata->learned[gsn_pick_lock] = oldpick;
      ch->pcdata->learned[gsn_sneak] -= ch->pcdata->plus_sneak;
      ch->pcdata->learned[gsn_hide] -= ch->pcdata->plus_hide;
      if (col % 2 != 0)
	strcat (hugebuf_o, "\n\r");
      sprintf (buf, "\n\rYou have %d practices and %d learns left.\x1B[0m\n\r",
	       ch->pcdata->practice, ch->pcdata->learn);
      strcat (hugebuf_o, buf);
      page_to_char (hugebuf_o, ch);
      return;
    }

  if (!IS_AWAKE (ch))
    {
      send_to_char ("In your dreams?  Good luck!\n\r", ch);
      return;
    }

  if (argy[0] == '\0')
    {
      int col;
      int oldtrack; 
      int oldtrap;
      int oldpick;
      hugebuf_o[0] = '\0';
      send_to_char("\x1B[1;37mYou have knowledge in the following:\x1B[37;0m\n\r", ch);
      send_to_char("\x1B[1;34m------------------------------------\x1B[37;0m\n\r", ch);
      found = FALSE;
      col = 0;
      if (mob == NULL || IS_PLAYER (mob))
	return;
      oldpick = ch->pcdata->learned[gsn_pick_lock];
      if (is_member (ch, GUILD_THIEFG))
	ch->pcdata->learned[gsn_pick_lock] = 100;
      oldtrack = ch->pcdata->learned[gsn_track];
      if (is_member (ch, GUILD_RANGER) && is_member(ch, GUILD_THIEFG) &&
is_member(ch, GUILD_TINKER))
	ch->pcdata->learned[gsn_trapset] = 100;
      oldtrap = ch->pcdata->learned[gsn_trapset]; 
      if (is_member (ch, GUILD_RANGER))
	ch->pcdata->learned[gsn_track] = 100;
      ch->pcdata->learned[gsn_sneak] += ch->pcdata->plus_sneak;
      ch->pcdata->learned[gsn_hide] += ch->pcdata->plus_hide;
      for (sn = 0; sn <= 28; sn++)
	{
	  if (mob->pIndexData->opt->skltaught[sn] > 0 &&
	      (spell = skill_lookup (NULL, mob->pIndexData->opt->skltaught[sn])) != NULL)
	    {
	    
	      if (LEVEL (ch) >= spell->spell_level)
		{
		  char hg[300];
		  found = TRUE;
		  strcpy (hg, how_good (ch, spell->gsn));
		  sprintf (buf, " %22s %10s", spell->spell_funky_name, hg);
		  strcat (hugebuf_o, buf);
		  if (++col % 2 == 0)
		    strcat (hugebuf_o, "\n\r");
		}
	    }
	}
      ch->pcdata->learned[gsn_trapset] = oldtrap;
      ch->pcdata->learned[gsn_track] = oldtrack;
      ch->pcdata->learned[gsn_pick_lock] = oldpick;
      ch->pcdata->learned[gsn_sneak] -= ch->pcdata->plus_sneak;
      ch->pcdata->learned[gsn_hide] -= ch->pcdata->plus_hide;
      if (!found)
	strcat (hugebuf_o, "Nothing.\n\r");
      if (col % 2 != 0)
	strcat (hugebuf_o, "\n\r");
      sprintf (buf, "\n\rYou have %d practice sessions left.\n\r",
	       ch->pcdata->practice);
      strcat (hugebuf_o, buf);
      page_to_char (hugebuf_o, ch);
      return;
    }

  spell = skill_lookup (argy, -1);
  if (spell == NULL)
    {
      send_to_char ("What's that?\n\r", ch);
      return;
    }

  if (spell == NULL
      || (IS_PLAYER (ch)
	  && (LEVEL (ch) < spell->spell_level ||
	      ch->pcdata->learned[spell->gsn] == -100)))
    {
      if (spell->gsn > 0 && spell->gsn < SKILL_COUNT && ch->pcdata->learned[spell->gsn] == -100)
	{
	  do_learn (ch, argy);
	  if (ch->pcdata->learned[spell->gsn] == -100)
	    {
	      send_to_char ("For some reason or another, you could not LEARN this spell/skill.\n\r", ch);
	      send_to_char ("Perhaps you are out of learns?  Type 'learn' for more information.\n\r", ch);
	      return;
	    }
	  goto lerned_it;
	}

      send_to_char ("I have no knowledge in that area.\n\r", ch);
      return;
    }
lerned_it:
  if (mob == NULL)
    return;
  while (cnttt <= 30)
    {
      if (IS_MOB (mob) && mob->pIndexData->opt->skltaught[cnttt] == spell->gsn)
	{
	  cnttt = 50;
	  break;
	}
      cnttt++;
    }

  if (cnttt != 50)
    {
      send_to_char ("I have too little knowledge in that area to help you practice.\n\r", ch);
      return;
    }

  
  if (ch->pcdata->practice < 1)
    {
      send_to_char ("You have no practice sessions left!\n\r", ch);
      return;
    }

  if (spell->slot != 0)
    adept = pow.max_prac_spells;
  else
    adept = pow.max_prac_skills;
  if (spell->slot == 0)
    {
      if(get_curr_dex (ch) < spell->min_wis)
	{
	  adept -= (spell->min_wis - get_curr_dex (ch)) * 8;
	}
      if(get_curr_str (ch) < spell->min_int)
	{
	  adept -= (spell->min_int - get_curr_str (ch)) * 8;
	  if (adept < 20)
	    adept = 20;
	}
    }
  else if (spell->slot == 1)
    { 
      if (get_curr_wis (ch) < spell->min_wis)
	{
	  adept -= (spell->min_wis - get_curr_wis (ch)) * 8;
	}
      if (get_curr_int (ch) < spell->min_int)
	{
	  adept -= (spell->min_int - get_curr_int (ch)) * 8;
	  if (adept < 20)
	    adept = 20;
	}
    }
  else if (spell->slot == 2)
    {
      if (get_curr_dex (ch) < spell->min_wis)
	{
	  adept -= (spell->min_wis - get_curr_dex (ch)) * 8;
	}
      if (get_curr_int (ch) < spell->min_int)
	{
	  adept -= (spell->min_int - get_curr_int (ch)) * 8;
	  if (adept < 20)
	    adept = 20;
	}
    }
  
  if (IS_PLAYER (ch))
    {
      SPELL_DATA *spl;
      if (spell->prereq_1 != NULL && (spl = skill_lookup(spell->prereq_1, -1)) != NULL && ch->pcdata->learned[spl->gsn] < pow.prereq)
	{
	  sprintf (buf, "You need to be skilled in %s first.\n\r", spl->spell_funky_name);
	  send_to_char (buf, ch);
	  return;
	}
      if (spell->prereq_2 != NULL && (spl = skill_lookup(spell->prereq_2, -1)) != NULL && ch->pcdata->learned[spl->gsn] < pow.prereq)
	{
	  sprintf (buf, "You need to be skilled in %s first.\n\r", spl->spell_funky_name);
	  send_to_char (buf, ch);
	  return;
	}
    }
  
  if (ch->pcdata->learned[spell->gsn] >= adept)
    {
      sprintf (buf, "You are already quite good at %s.\n\r", spell->spell_funky_name);
      send_to_char (buf, ch);
      return;
    }
  if (spell->guilds != 0)
    {
      int i;
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if(IS_SET(spell->guilds, guild_data[i].mob_guildmaster_bit) && !IS_SET(ch->pcdata->guilds, guild_data[i].mob_guildmaster_bit))
	    {
	      send_to_char("You do not have the proper guilds to practice this!\n\r", ch);
	      return;
	    }
	}
    }

    
      ch->pcdata->practice--;
      if (spell->slot == 1)
	ch->pcdata->learned[spell->gsn] += int_app[get_curr_int (ch) - 1].learn;
      if (spell->slot != 1)
	ch->pcdata->learned[spell->gsn] += int_app[get_curr_int (ch) - 1].learn - 1;
      if (ch->pcdata->learned[spell->gsn] < adept)
	{
	  act ("You practice $T.",
	       ch, NULL, spell->spell_funky_name, TO_CHAR);
	}
      else
	{
	  ch->pcdata->learned[spell->gsn] = adept;
	  act ("You now have a good understanding of $T.",
	       ch, NULL, spell->spell_funky_name, TO_CHAR);
	  if (spell->gsn > 549 && IS_PLAYER(ch) && !str_cmp(race_info[ch->pcdata->race].name, "orc") && adept == pow.max_prac_skills)
	    ch->pcdata->learned[spell->gsn]+= 15;
	}
    

  return;
}

void 
do_prereqs (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  SPELL_DATA *spell;
  DEFINE_COMMAND ("prereqs", do_prereqs, POSITION_DEAD, 0, LOG_NORMAL, "Allows you to see prerequisites to the specified skill or spell.")

    if (IS_MOB (ch))
    return;
  if (argy[0] == '\0' || (spell = skill_lookup (argy, -1)) == NULL)
    {
      send_to_char (
		     "Syntax: Prereq <skill/spell/prof name>\n\r", ch);
      return;
    }

  if (spell->prereq_1 != NULL && spell->prereq_2 != NULL)
    {
      sprintf (buf,
	       "\x1B[1m%s\x1B[0m has two prerequisites: \x1B[1;36m%s\x1B[37;0m and \x1B[1;36m%s\x1B[37;0m.\n\r",
	       spell->spell_funky_name, spell->prereq_1, spell->prereq_2);
      send_to_char (buf, ch);
      return;
    }

  if (spell->prereq_1 != NULL)
    {
      sprintf (buf,
      "\x1B[1m%s\x1B[0m has only one prerequisite: \x1B[1;36m%s\x1B[0m.\n\r",
	       spell->spell_funky_name, spell->prereq_1);
      send_to_char (buf, ch);
      return;
    }

  sprintf (buf,
	   "\x1B[1m%s\x1B[0m has no prerequisites whatsoever.\n\r",
	   spell->spell_funky_name);
  send_to_char (buf, ch);
  return;
}



void 
do_spells (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char bbb[50];
  char hg[300];
  SPELL_DATA *spell;
  bool fnd = FALSE;
  int sn;
  int col;
  DEFINE_COMMAND ("spells", do_spells, POSITION_DEAD, 0, LOG_NORMAL, "This command shows you all the spells you currently know.")
    hugebuf_o[0] = '\0';
  if (IS_MOB (ch))
    {
      send_to_char ("You do not know how to cast spells!\n\r", ch);
      return;
    }

  hugebuf_o[0] = '\0';
  col = 0;
  for (sn = 0; sn < SKILL_COUNT; sn++)
    {
      if ((spell = skill_lookup (NULL, sn)) == NULL)
	continue;
      if ((LEVEL (ch) < spell->spell_level)
	  || (spell->spell_level > 80))
	continue;
      if (spell->slot != 1)
	continue;
      if (ch->pcdata->learned[sn] == -100)
	continue;
      fnd = TRUE;
     
      {
	bool found = FALSE;
	int i;
	for (i = 0; str_cmp(mana_data[i].what_you_type, "end_of_list") && !found; i ++)
	  {
	    if (spell->mana_type == mana_data[i].flagname)
	      {
		char buf[50];
		sprintf (buf, "%s%s\x1b[0;37m", mana_data[i].color, mana_data[i].letter);
		strcpy(bbb, buf);
		found = TRUE;
	      }
	  }
	if (!found)
	  strcpy(bbb, " ");
      }
      
      strcpy (hg, how_good (ch, sn));
      if (LEVEL (ch) >= 100)
	sprintf (buf, "%s %3dM (%3d) %-20s ",
	    bbb, ((2 + LEVEL (ch) - spell->spell_level == 0) ? spell->mana :
		  (UMAX (spell->mana,
			 100 / (2 + LEVEL (ch) - spell->spell_level)))),
		 ch->pcdata->learned[sn], spell->spell_funky_name);
      else
	sprintf (buf, "%s %3dM %10s %-20s ",
	    bbb, ((2 + LEVEL (ch) - spell->spell_level == 0) ? spell->mana :
	 (UMAX (spell->mana, 100 / (2 + LEVEL (ch) - spell->spell_level)))),
		 hg, spell->spell_funky_name);
      strcat (hugebuf_o, buf);
      if (++col % 2 == 0)
	strcat (hugebuf_o, "\n\r");
    }

  if (col % 2 != 0)
    strcat (hugebuf_o, "\n\r");
  if (!fnd)
    strcpy (hugebuf_o, "You know no spells at the moment.\n\r");
  page_to_char (hugebuf_o, ch);
  return;
}

void 
do_info_sp_sk (CHAR_DATA * ch, SPELL_DATA * spell)
{
char buffy[500];
char hg[300];
bool prereq = FALSE;
bool found_mana = FALSE;
if (IS_MOB (ch))
  {
    send_to_char ("NPC's cannot do this!\n\r", ch);
    return;
  }

if (spell == NULL)
  {
    send_to_char ("Does not exist.\n\r", ch);
    return;
  }
sprintf(buffy,"\x1b[0;35m-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\x1b[0;37m\n\r");
send_to_char(buffy, ch);
sprintf(buffy, "\x1b[1;30mLevel \x1b[1;34m%2d\x1b[0;37m:\n\r",spell->spell_level);
send_to_char(buffy, ch);
sprintf(buffy, "\x1b[1;36m%s   ", capitalize(spell->spell_funky_name));
send_to_char(buffy, ch);
if (spell->slot == 0)
  {
    sprintf(buffy, "    \x1b[1;30m(\x1b[1;36mSkill\x1b[1;30m)\n\r");
    send_to_char(buffy, ch);
  }
else if (spell->slot == 2)
  {
    sprintf(buffy, "    \x1b[1;30m(\x1b[1;36mProficiency\x1b[1;30m)\n\r");
    send_to_char(buffy, ch);
  }
else
  { 
    sprintf(buffy, "\x1b[1;30m(");
    send_to_char(buffy, ch);
    if (spell->spell_type == TAR_CHAR_OFFENSIVE)
      send_to_char("\x1b[1;36mOffensive ", ch);
    else if (spell->spell_type == TAR_CHAR_DEFENSIVE)
      send_to_char("\x1b[1;36mDefensive ", ch);
    else if (spell->spell_type == TAR_CHAR_SELF)
      send_to_char("\x1b[1;36mSelf Only ", ch);
    else
      send_to_char("\x1b[1;36mSpecial ", ch);
    if (IS_SET(spell->spell_bits, SPELL_ALL_IN_ROOM))
      send_to_char("Area ", ch);
    send_to_char("Spell\x1b[1;30m)\n\r", ch);
    sprintf(buffy, "\x1b[0;32m{\x1b[1;37m%d\x1b[0;32m} ",spell->mana);
    send_to_char(buffy, ch);
    {
      int i;
      for (i = 0; str_cmp(mana_data[i].what_you_type, "end_of_list") && !found_mana; i ++)
	{
	  if (spell->mana_type == mana_data[i].flagname)
	    {
	      sprintf(buffy, "%s%s\x1b[0;37m", mana_data[i].color, mana_data[i].what_you_type);
	      found_mana = TRUE;
	      
	    }
	}
    }
    if (!found_mana)
      sprintf(buffy, "Unknown");
    send_to_char (buffy, ch);
    send_to_char("\x1b[0;32m mana is required to cast this spell.\x1b[0;37m\n\r", ch);
  }  
send_to_char("\x1b[1;30mPrerequisites\x1b[0;37m:  ", ch);
if (spell->prereq_1 && str_cmp(spell->prereq_1, "none") &&str_cmp(spell->prereq_1, "None"))
  {
    sprintf (buffy, "\x1b[0;36m%s", spell->prereq_1);
    send_to_char(buffy, ch); 
    prereq = TRUE;
  }
if (spell->prereq_2 && str_cmp(spell->prereq_2, "none") &&str_cmp(spell->prereq_2,"None"))
  {
    if (prereq)
      send_to_char(", ", ch);
    sprintf (buffy, "\x1b[0;36m%s", spell->prereq_2);
    send_to_char(buffy, ch);
    prereq = TRUE;
  }
if (!prereq)
  send_to_char("\x1b[0;36mNone", ch);
send_to_char(".\n\r",ch);  


if (spell->slot == 0)
  {
    strcpy (hg, how_good (ch, spell->gsn));
    sprintf (buffy, "Your proficiency in this skill is \x1B[1;37m%s\x1B[0m.\n\r", hg);
  }
else if (spell->slot == 2)
    {
    strcpy (hg, how_good (ch, spell->gsn));
    sprintf (buffy, "Your ability in this proficiency is \x1B[1;37m%s\x1B[0m.\n\r", hg);
  }
else
  {
    strcpy (hg, how_good (ch, spell->gsn));
    sprintf (buffy, "Your proficiency in this spell is \x1B[1;37m%s\x1B[0;37m.\n\n\r", hg);
  }
send_to_char(buffy, ch);
if (spell->slot == 0)
  {
    sprintf(buffy, "This skill requires a minimum \x1b[1;34m%d\x1b[0;37m strength and \x1b[1;32m%d\x1b[0;37m dexterity to practice fully.\n\r", spell->min_int, spell->min_wis);
    send_to_char(buffy, ch);
  }
else if (spell->slot == 2)
  {
    sprintf(buffy, "This proficiency requires a minimum \x1b[1;34m%d\x1b[0;37m intelligence and \x1b[1;32m%d\x1b[0;37m dexterity to practice fully.\n\r", spell->min_int, spell->min_wis);
    send_to_char(buffy, ch);
  }
else
  {
    sprintf(buffy, "This skill requires a minimum \x1b[1;34m%d\x1b[0;37m intelligence and \x1b[1;32m%d\x1b[0;37m wisdom to practice fully.\n\r", spell->min_int, spell->min_wis);
    send_to_char(buffy, ch);
  }
   /*
if (spell->slot == 1)
  {
    if (spell->spell_type == TAR_CHAR_OFFENSIVE)
      {
	sprintf(buffy, "This spell requires a minimum of 
\x1b[1;34m%d\x1b[0;37m intelligence to cast effectively.\n\r", (17 + 
(spell->spell_level)/5));
	send_to_char(buffy, ch);
      } 
    else 
      if (IS_SET(spell->spell_bits, SPELL_HEALS_DAM) || IS_SET(spell->spell_bits, SPELL_ADD_MOVE))
	{
	  sprintf(buffy, "This spell requires a minimum of \x1b[1;34m%d\x1b[0;37m wisdom to cast effectively.\n\r", (10+(spell->spell_level)*2/5));
	  send_to_char(buffy, ch);  
	} 
  }
*/
if (spell->guilds != 0)
  {
    int i=0;
    bool prev= FALSE; 
    sprintf(buffy, "This %s requires:\x1b[1;36m",(spell->slot == 0 ? "skill" :  (spell->slot == 1 ?  "spell" : "proficiency")));
    send_to_char(buffy, ch);
    for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
      {
	if (IS_SET(spell->guilds, guild_data[i].mob_guildmaster_bit))
	  {
	    if (prev)
	      send_to_char(",", ch);
	    sprintf(buffy, guild_data[i].what_you_see);
	    send_to_char(buffy, ch);
	    prev = TRUE;
	  }
      }
    send_to_char("\x1b[0;37m to learn and use.\n\r", ch);
  }
  
send_to_char("\n\r", ch);
do_help_two (ch, spell->spell_funky_name);
sprintf(buffy,"\x1b[0;35m-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\x1b[0;37m\n\r");
send_to_char(buffy, ch);
return;
}



void 
do_slist (CHAR_DATA * ch, char *argy)
{
  SPELL_DATA *spellstuff[500];
  char buf[STD_LENGTH];
  SPELL_DATA *spell;
  int sn;
  int mnum = 0;
  int level;
  char bbb[40];
  short slot;
  DEFINE_COMMAND ("slist", do_slist, POSITION_DEAD, 1, LOG_NORMAL, "This command lists spells/skills.")

    if (argy[0] == '\0' || argy == "")
    {
      send_to_char ("'slist spells': list all spells in the game.\n\r'slist skills': list all skills in the game.\n\r'slist prof' lists all of the proficiencies in the game.\n\r", ch);
      return;
    }
  if (!str_cmp (argy, "spells"))
    slot = 1;
  else if (!str_cmp (argy, "skills"))
    slot = 0;
  else if (!str_cmp (argy, "prof") || !str_cmp(argy, "proficiencies"))
    slot = 2;
  else
    {
      if ((spell = skill_lookup (argy, -1)) == NULL)
	{
	  send_to_char ("Valid argys are 'spells' or 'skills' or 'prof' ...or slist <specific spell/skill>.\n\r", ch);
	  return;
	}
      do_info_sp_sk (ch, spell);
      return;
    }
  
  hugebuf_o[0] = '\0';
  switch(slot)
    {
    case 0:
      strcat(hugebuf_o, "Skills available:\n\rLv Skills\n\r----------\n\r");
      break;
    case 1:
      strcat(hugebuf_o, "Spells available:\n\rLv Spells\n\r----------\n\r");
      break;
    case 2:
      strcat(hugebuf_o, "Proficiencies available:\n\rLv Proficiencies\n\r----------\n\r");
      break;
    }

  for (sn = 0; sn < SKILL_COUNT; sn++)
    {
      if ((spell = skill_lookup (NULL, sn)) == NULL)
	continue;
      if (spell->slot != slot)
	continue;
      spellstuff[mnum] = spell;
      mnum++;
    }

  for (level = 0; level <= 110; level++)
    {
      for (sn = 0; sn < mnum; sn++)
	{
	  spell = spellstuff[sn];
	  if (spell->spell_level != level)
	    continue;
	      sprintf (buf, "\x1b[0;35m(\x1b[0;32m%2d\x1b[0;35m) ",level);
	      strcat (hugebuf_o, buf);
          if(IS_SET(spell->spell_bits, SPELL_ALL_IN_ROOM))
             sprintf(buf, "\x1b[1;31mA ");
          else
             sprintf(buf, "  ");
          strcat (hugebuf_o, buf);
	  {
	    bool found = FALSE;
	    int i;
	    for (i = 0; str_cmp(mana_data[i].what_you_type, "end_of_list") && !found; i ++)
	      {
		if (spell->mana_type == mana_data[i].flagname)
		  {
		    char bufff[50];
		    sprintf (bufff, "%s%s\x1b[0;37m", mana_data[i].color, mana_data[i].letter);
		    strcpy(bbb, bufff);
		    found = TRUE;
		  }
	      }
	    if (!found)
	      strcpy(bbb, " ");
	  }
	  if (slot == 1)
	    sprintf (buf, "\x1b[1;36m%-21s %s \x1b[1;30m(\x1b[1;37m%2d\x1b[1;30m)\x1b[0;37m ",    spell->spell_funky_name, bbb, spell->mana);
	  else
	    sprintf (buf, "\x1b[1;36m%-20s  ", spell->spell_funky_name);
	  strcat (hugebuf_o, buf);
	  if (spell->prereq_1 && str_cmp(spell->prereq_1, "none") &&
str_cmp(spell->prereq_1, "None"))
           {
             sprintf (buf, "\x1b[0;36m%-18s", spell->prereq_1);
             strcat (hugebuf_o, buf);
           }
	  if (spell->prereq_2 && str_cmp(spell->prereq_2, "none") &&
str_cmp(spell->prereq_2,"None"))
           {
             sprintf (buf, "\x1b[0;36m%-18s", spell->prereq_2);
             strcat (hugebuf_o, buf);
           }
        strcat (hugebuf_o, "\n\r");
       }
    }
  strcat (hugebuf_o, "\x1b[0;37m");
  page_to_char (hugebuf_o, ch);
  return;
}
