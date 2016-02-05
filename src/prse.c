
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

struct int_apply_type int_app[37];
struct wis_apply_type wis_app[37];
struct con_apply_type con_app[37];

int lne = 1;
char line[40960];

int 
grab_num_2 (char *txt)
{
  int n = 0;
  char *t;
  for (t = txt; *t != '\0'; t++)
    {
      if (*t >= '0' && *t <= '9')
	{
	  n *= 10;
	  n += (*t) - '0';
	  continue;
	}
    }
  return n;
}

bool prmpt=FALSE;

char *
interpret_line (char *inlne, CHAR_DATA * ch)
{
  char tmpcd[100];
  int gpos;
  int pos;
  char *t;
  line[0] = '\0';
  lne = 1;
  gpos = 0;
  if (inlne == NULL)
    return "(type prompt all)";
  for (t = inlne; *t != '\0'; t++)
    {
      tmpcd[0] = '\0';
      pos = 0;
      if (*t == '\n')
	lne++;
      if (*t == '*')
	{
	  for (t++; *t != '*'; t++)
	    {
	      if (pos >= 10)
		{
		  fprintf (stderr, "Bad interpret * code.. '%s'.\n", inlne);
		  line[0] = '\0';
		  return line;
		}
	      tmpcd[pos] = *t;
	      pos++;
	    }
	  tmpcd[pos] = '\0';
	  line[gpos] = '\0';
	  strcat (line, parse_txt (tmpcd, ch));
	  gpos = strlen (line);
	  continue;
	}
      line[gpos] = *t;
      gpos++;
    }
  line[gpos] = '\0';
  return line;
}



char *
parse_txt (char *code, CHAR_DATA * ch)
{
  static char bf[5550];
  int sl = 0;
  int pad = 0;
  int i;
  char *tt;
  bf[0] = '\0';
  pad = grab_num_2 (code);
  if (pad > 20) pad = 20;
  for (tt = code; *tt != '\0'; tt++)
    if (!is_digit(*tt))
      break;
  
  switch (UPPER(*tt))
    {
    case 'A':
      if (!str_infix ("aff", code))
	{
	  if (ch->affected != NULL)
	    {
	      SPELL_DATA *spell;
	      AFFECT_DATA *paf;
	      char buf[500];
	      strcat (bf, "\n\rYou are affected by:");
	      lne++;
	      for (paf = ch->affected; paf != NULL; paf = paf->next)
		{
		  if ((spell = skill_lookup (NULL, paf->type)) == NULL)
		    continue;
		  lne++;
		  sprintf (buf, "\n\r%s", capitalize (spell->spell_funky_name));
		  strcat (bf, buf);
		  if (IS_MOB (ch) || (LEVEL (ch) > 100))
		    sprintf (buf,
			     " (modifies %s by %d for %d mud hours)",
			     affect_loc_name (paf->location),
			     paf->modifier,
			     paf->duration);
		  else
		    {
		      if (paf->duration > 50)
			sprintf (buf, ", which won't wear off for many days!");
		      else if (paf->duration > 30)
			sprintf (buf, ", which will wear off in a day or two.");
		      else if (paf->duration > 20)
			sprintf (buf, ", which will wear off in about a day.");
		      else if (paf->duration > 10)
			sprintf (buf, ", which will wear off in about half a day.");
		      else if (paf->duration > 6)
			sprintf (buf, ", which will wear off in about a quarter of a day.");
		      else if (paf->duration > 3)
			sprintf (buf, ", which will wear off in a few hours.");
		      else if (paf->duration > 1)
			sprintf (buf, ", which will wear off soon.");
		      else if (paf->duration == 1)
			sprintf (buf, ", which will wear off very soon.");
		      else
			sprintf (buf, ", which will wear off at any moment.");
		    }
		  strcat (bf, buf);
		}
	    }
	} 
      else if (!str_infix ("al", code))
	{
	  sprintf (bf, "%d", ALIGN (ch));
	} 
      else if (!str_infix ("ap", code))
	{
	  if (!ch->ced)
	    sprintf (bf, "0");
	  else
	    sprintf (bf, "%d", GET_DAMROLL (ch)); 
	}
      else if (!str_infix ("a", code))
	{
	  sprintf (bf, "%d", get_age (ch));
	}
      break;
    case 'C':
      if (!str_infix ("con", code))
	{
	  if( LEVEL(ch) >= pow.level_to_see_numeric_stats)
	    sprintf (bf, "%d", get_curr_con (ch));
	  else
	    sprintf(bf, "XX");
	  
	}
      else if (!str_infix ("cond", code))
	{
	  char tbf[100];
	  tbf[0] = '\0';
	  if (ch->pcdata->condition[COND_DRUNK] > 10)
	    strcat (tbf, "You are drunk.\n\r");
	  if (ch->pcdata->condition[COND_FULL] < 10 &&
	      ch->pcdata->condition[COND_FULL] > 5)
	    strcat (tbf, "You are starting to get hungry.\n\r");
	  if (ch->pcdata->condition[COND_THIRST] < 10 &&
	      ch->pcdata->condition[COND_THIRST] > 5)
	    strcat (tbf, "You are beginning to get thirsty.\n\r");
	  if (ch->pcdata->condition[COND_FULL] < 5 &&
	      ch->pcdata->condition[COND_FULL] > 0)
	    strcat (tbf, "You are hungry.\n\r");
	  if (ch->pcdata->condition[COND_THIRST] < 5 &&
	      ch->pcdata->condition[COND_THIRST] > 0)
	    strcat (tbf, "You are thirsty.\n\r");
	  if (ch->pcdata->condition[COND_THIRST] < 1)
	    strcat (tbf, "You are totally dehydrated!\n\r");
	  if (ch->pcdata->condition[COND_FULL] < 1)
	    strcat (tbf, "You are STARVING!\n\r");
	  if (tbf[0] != '\0')
	    {
	      lne++;
	      strcat (bf, "\n\r");
	      strcat (bf, tbf);
	    }
	}
      else if (!str_infix("cskl", code))
	{
  	  sprintf(bf, "%d", (race_info[ch->pcdata->race].max_skills +
align_info[ch->pcdata->alignment].max_skills+
(ch->pcdata->remort_times+1)/2) - tally_spsk(ch, 0));
	}
      else if (!str_infix("cspl", code))
	{
 	  sprintf(bf, "%d", (race_info[ch->pcdata->race].max_spells +
align_info[ch->pcdata->alignment].max_spells+ (ch->pcdata->remort_times)- tally_spsk(ch, 1)));
	}
      else if (!str_infix("cprf", code))
	{
       sprintf(bf, "%d", (race_info[ch->pcdata->race].max_profs +
align_info[ch->pcdata->alignment].max_profs+ (ch->pcdata->remort_times+1)/2) - tally_spsk(ch, 2));
	} 
      else if (!str_infix ("clan", code))
	{
	  int ii;
	  if (IS_PLAYER (ch))
	    {
	      ii = clan_number (ch);
	      if (ii > 0)
		{
		  sprintf (bf, "\n\rMember of \x1B[1;36m%s\x1B[37;0m clan.", get_clan_index (ii)->name);
		  lne++;
		}
	    }
	}
      else if (!str_infix ("cv", code))
	{
	  if (LEVEL (ch) > 100)
	    sprintf (bf, "%d", ch->in_room->vnum);
	  else
	    bf[0] = '\0';
	} 
      else if (!str_infix ("cr", code))
	{
	  sprintf (bf, "%d", ch->pcdata->carry_number);
	}
      else if (!str_infix ("cn", code))
	{
	  sprintf (bf, "%d", (ch->gold * 100) + ch->copper);
	}
      else if (!str_infix ("cb", code))
	{
	  sprintf (bf, "%ld", ch->pcdata->bank);
	}
      break;
    case 'D':
      if (!str_infix ("dex", code))
	{
	  if( LEVEL(ch) >= pow.level_to_see_numeric_stats)
	    sprintf (bf, "%d", get_curr_dex (ch));
	  else 
	    sprintf(bf, "XX");
	}
      else if (!str_infix ("dth", code))
	{
	  sprintf(bf, "%d", ch->pcdata->deaths);
	}
      break;
    case 'E':
      if (!str_infix ("eb", code))
	{
	  int evasion;
	  if (!ch->ced)
	    sprintf (bf, "0");
	  else
	    {
	      evasion = get_curr_dex (ch) + (LEVEL (ch) / 5) + (is_member (ch, GUILD_THIEFG) ? 5 : 0) +
		(ch->pcdata->learned[gsn_parry] == -100 ? 0 : ch->pcdata->learned[gsn_parry] / 7) +
		(ch->pcdata->learned[gsn_dodge] == -100 ? 0 : ch->pcdata->learned[gsn_dodge] / 6) +
		(ch->pcdata->learned[gsn_evade] == -100 ? 0 : ch->pcdata->learned[gsn_evade] / 6) +
		(ch->pcdata->learned[gsn_shield_block] == -100 ? 0 : ch->pcdata->learned[gsn_shield_block] / 9) +
		(IS_SET (ch->ced->fight_ops, F_AGGRESSIVE) ? 0 : 4);
	      sprintf (bf, "%d", evasion);
	    }
	}
      break;
    case 'F':
      if (!str_infix ("fi", code))
	{
	  if (!FIGHTING (ch))
	    bf[0] = '\0';
	  else
	    sprintf (bf, "[Oppnt: %s] ", capitalize (STRING_HITS (FIGHTING (ch))));
	}
      break;
    case 'G':
      if (!str_infix ("guil", code) && ch->pcdata->guilds != 0 && IS_PLAYER (ch))
	{
	  int i=0;
	  bool prev= FALSE;
	  lne++;
	  strcat(bf, "\n\rGuild(s): \x1b[1;37m");
	  for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	    {
	      if (is_member(ch, guild_data[i].mob_guildmaster_bit))
		{
		  if (prev)
		    strcat(bf, ",");
		  strcat(bf, guild_data[i].what_you_see);
		  prev = TRUE;
		}
	    }
	  
	  strcat (bf, "\x1B[0;37m.");
	}
      else if (!str_infix ("g", code))
	{
	  int i, j;
	  char *t;
	  i = 0;
	  for (t = line + (strlen (line)); t != line && *t != '\r' && *t != '\n'; t--)
	    {
	      i++;
	    }
	  for (j = pad - i; j > 0; j--)
	    {
	      strcat (bf, " ");
	      if (strlen(bf)>80) break;
	    }
	  return bf;
	}
      break;
    case 'H':
      if (!str_infix ("hn", code))
	{
	  sprintf (bf, "%ld", ch->hit);
	}
      else if (!str_infix ("hm", code))
	{
	  sprintf (bf, "%ld", ch->max_hit);
	}
      else if (!str_infix ("hw", code))
	{
	  sprintf (bf, "%s", capitalize (STRING_HITS (ch)));
	}
      else if (!str_infix ("ht", code))
	{
	  sprintf (bf, "%d' %2d\"",
		   ch->height / 12, ch->height % 12);
	}
      
      break;
    case 'I':
      if (!str_infix ("int", code))
	{
	  if( LEVEL(ch) >= pow.level_to_see_numeric_stats)
	    sprintf (bf, "%d", get_curr_int (ch));
	  else 
	    sprintf(bf, "XX");
	}
      break;
    case 'J':
      
      break;  
    case 'K':
      if (!str_infix ("kw", code))
	{
	  sprintf (bf, "%d Stn., %d Peb.", ch->pcdata->carry_weight/WGT_MULT, ch->pcdata->carry_weight % WGT_MULT);
	}
      else if (!str_infix ("kp", code))
	{
	  sprintf (bf, "%d", ch->pcdata->killpoints);
	}
      
      break;
    case 'L':
      if (!str_infix ("lea", code))
	{
	  sprintf (bf, "%d", (IS_MOB (ch) ? 0 : ch->pcdata->learn));
	}
      else if (!str_infix ("lvl", code))
	{
	  sprintf (bf, "%d", LEVEL (ch));
	}
      else if (!str_infix ("lnme", code))
	{
	  int offset = strlen(NAME(ch))/2;
	  int backset = (strlen(NAME(ch))+1)/2;
	  char buff[100];
	  int i;
	  sprintf(buff, " ");
	  for (i = 0; i < (20-offset); i++)
	    strcat(buff, " ");
	  strcat(buff, NAME(ch));
	  for (i = 0; i < (20-backset); i++)
	    strcat(buff, " "); 
	  
	  sprintf (bf, buff);
	}
      
      break;
    case 'M':
      if (!str_infix ("mnn", code))
	{
	  sprintf (bf, "%d", mana_now (ch));
	} 
      else if (!str_infix ("mmm", code))
	{
	  sprintf (bf, "%d", max_mana (ch));
	} 
      else if (!str_infix ("mn", code))
	{
	  sprintf (bf, "%d", ch->move);
	}
      else if (!str_infix ("mm", code))
	{
	  sprintf (bf, "%d", ch->max_move);
	}
      else if (!str_infix ("mw", code))
	{
	  sprintf (bf, "%s", capitalize (STRING_MOVES (ch)));
	}
      break;
    case 'N':
      if (!str_infix ("npkl", code))
	{
	  sprintf (bf, "%ld", ch->pcdata->pklevels);
	}
      else if (!str_infix ("npkh", code))
	{
	  sprintf (bf, "%ld", ch->pcdata->pkhelpers);
	}
      else if (!str_infix ("npks", code))
	{
	  sprintf (bf, "%d", ch->pcdata->pkills);
	}
      else if (!str_infix ("npkd", code))
	{
	  sprintf (bf, "%d", ch->pcdata->pkilled);
	} 
      else if (!str_infix ("npkr", code))
	{
	  sprintf (bf, "%ld", ch->pcdata->pkers);
	} 
      break;
    case 'O':
      if (!str_infix ("ob", code))
	{
	  int bonushit, offensive;
	  bonushit = 0;
	  if (!ch->ced)
	    sprintf (bf, "0");
	  else
	    {
	      SINGLE_OBJECT *objt;
	      objt = get_item_held(ch, ITEM_WEAPON);
	      if (objt != NULL)
		bonushit = addoffense2 (ch, objt);
	      offensive = bonushit + GET_HITROLL (ch);
	      sprintf (bf, "%d", offensive);
	    }
	}
      break;  
    case 'P':
      if (!str_infix ("pos", code))
	{
	  char tbf[100];
	  tbf[0] = '\0';
	  switch (ch->position)
	    {
	    case POSITION_DEAD:
	      strcpy (tbf, "You are DEAD!!           ");
	      break;
	    case POSITION_MORTAL:
	      strcpy (tbf, "You are mortally wounded.");
	      break;
	    case POSITION_INCAP:
	      strcpy (tbf, "You are incapacitated.   ");
	      break;
	    case POSITION_STUNNED:
	      strcpy (tbf, "You are stunned.         ");
	      break;
	    case POSITION_SLEEPING:
	      strcpy (tbf, "You are sleeping.        ");
	      break;
	    case POSITION_RESTING:
	      strcpy (tbf, "You are resting.         ");
	      break;
	    case POSITION_MEDITATING:
	      strcpy (tbf, "You are meditating.      ");
	      break;
	    }
	  if (tbf[0] != '\0')
	    {
	      strcat (bf, "\n\r");
	      lne++;
	      strcat (bf, tbf);
	    }
	}
      else if (!str_infix ("pra", code))
	{
	  sprintf (bf, "%d", (IS_MOB (ch) ? 0 : ch->pcdata->practice));
	}
      
      else if (!str_infix ("pt", code))
	{
	  int hours = ((ch->pcdata->played) + (((int) (current_time - ch->pcdata->logon)))) / 3600;
	  sprintf (bf, "%d", hours);
	}
      break;
    case 'Q':
      if (!str_infix ("qstr", code))
	{
	  if( LEVEL(ch) >= pow.level_to_see_numeric_stats)
	    sprintf (bf, "%d", ch->pcdata->stat[0]);
	  else 
	    sprintf(bf, "XX");  
	}
      else if (!str_infix ("qint", code))
	{
	  if( LEVEL(ch) >= pow.level_to_see_numeric_stats)
	    sprintf (bf, "%d", ch->pcdata->stat[1] );
	  else 
	    sprintf(bf, "XX");  
	}
      else if (!str_infix ("qwis", code))
	{
	  if( LEVEL(ch) >= pow.level_to_see_numeric_stats)
	    sprintf (bf, "%d", ch->pcdata->stat[2]);
	  else 
	    sprintf(bf, "XX");  
	}
      else if (!str_infix ("qdex", code))
	{
	  if( LEVEL(ch) >= pow.level_to_see_numeric_stats)
	    sprintf (bf, "%d", ch->pcdata->stat[3]);
	  else 
	    sprintf(bf, "XX");
     }
      else if (!str_infix ("qcon", code))
	{
	  if( LEVEL(ch) >= pow.level_to_see_numeric_stats)
	    sprintf (bf, "%d", ch->pcdata->stat[4]);
	  else 
	    sprintf(bf, "XX");  
	}
      break;
    case 'R':
      if (!str_infix ("rank", code))
	{
	  if (LEVEL (ch) < 4)
	    sprintf (bf, "Inexperienced");
	  else if (LEVEL (ch) < 12)
	    sprintf (bf, "Knowledgable ");
	  else if (LEVEL (ch) < 27)
	    sprintf (bf, "Experienced  ");
	  else if (LEVEL (ch) < 45)
	    sprintf (bf, "Well-Known   ");
	  else if (LEVEL (ch) < 70)
	    sprintf (bf, "Famous       ");
	  else if (LEVEL (ch) < 100)
	    sprintf (bf, "LEGEND       ");
	  else
	    sprintf (bf, "A God        ");
	}
      else if (!str_infix ("rmt", code))
	{
	  sprintf (bf, "%d", ch->pcdata->remort_times);
	}
      else if (!str_infix ("r", code))
	{
	  sprintf (bf, "%s", race_info[ch->pcdata->race].name);
	}
      break;
      
    case 'S':
      if (!str_infix ("str", code))
	{
	  if( LEVEL(ch) >= pow.level_to_see_numeric_stats)
	    sprintf (bf, "%d", get_curr_str (ch));
	  else 
	    sprintf(bf, "XX");
	}
      else if (!str_infix ("sex", code))
	{
	  if (ch->pcdata->sex == SEX_MALE) sprintf (bf, "Male   ");
	  else if (ch->pcdata->sex == SEX_FEMALE) sprintf (bf, "Female ");
	  else 
	    {
	      ch->pcdata->sex = SEX_NEUTRAL;
	      sprintf(bf, "Neutral");
	    }
	} 
      else if (!str_infix("snme", code))
	{
	  sprintf (bf, NAME(ch));
	}
      else if (!str_infix ("sect", code))
	{
	  int ii;
	  if (IS_PLAYER (ch))
	    {
	      ii = clan_number_2 (ch);
	      if (ii > 0)
		{
		  sprintf (bf, "\n\rMember of \x1B[1;36m%s\x1B[37;0m sect.", get_clan_index_2 (ii)->name);
		  lne++;
		}
	    }
	}
      break;
    case 'T':
      if (!str_infix("tskl", code))
	{
	  sprintf(bf, "%d", (race_info[ch->pcdata->race].max_skills + align_info[ch->pcdata->alignment].max_skills + (ch->pcdata->remort_times+1)/2));
	}
      else if (!str_infix("tspl", code))
	{
 	  sprintf(bf, "%d", (race_info[ch->pcdata->race].max_spells +
align_info[ch->pcdata->alignment].max_spells+
(ch->pcdata->remort_times)));
	} 
      else if (!str_infix("tprf", code))
	{
	  sprintf(bf, "%d", (race_info[ch->pcdata->race].max_profs + align_info[ch->pcdata->alignment].max_profs+ (ch->pcdata->remort_times+1)/2));
	} 
      
      else if (!str_infix ("tk", code))
	{
	  if (!FIGHTING (ch) || !FIGHTING (FIGHTING (ch)) || FIGHTING (FIGHTING (ch)) == ch)
	    bf[0] = '\0';
	  else
	    sprintf (bf, "[%s: %s] ", NAME (FIGHTING (FIGHTING (ch))), capitalize (STRING_HITS (FIGHTING (FIGHTING (ch)))));
	}
      else if (!str_infix ("twp", code))
	{
	  sprintf (bf, "%ld", ch->pcdata->total_wps);
	} 
      else if (!str_infix ("tc", code))
	{
	  sprintf (bf, "%d", ch->pcdata->totalkills);
        }
   /*  Heres what structure tried to add else if (!str_infix ("sha", 
code))
        { 
        sprintf (bf, "%sd", ch->pcdata->elemental_alignment);
	} */
      break;
    case 'W':
      if (!str_infix ("wis", code))
	{
	  if( LEVEL(ch) >= pow.level_to_see_numeric_stats)
	    sprintf (bf, "%d", get_curr_wis (ch));
	  else 
	    sprintf(bf, "XX");
	}
      else if (!str_infix ("warm", code) && IS_PLAYER (ch))
	{
	  if (cur_temp (ch) < 50)
	    {
	      if (ch->pcdata->warmth > 0)
		{
		  switch (ch->pcdata->warmth / 10)
		    {
		    case 0:
		    case 1:
		      strcat (bf, "\n\rYour clothing doesn't protect you too well in cold weather.");
		      break;
		    case 2:
		    case 3:
		      strcat (bf, "\n\rYour clothing protects you moderately well in cold weather.");
		      break;
		    case 4:
		      strcat (bf, "\n\rYour clothing protects you quite well in cold weather.");
		      break;
		    case 5:
		      strcat (bf, "\n\rYour clothing would keep you warm in some of the most frigid conditions.");
		      break;
		    default:
		      strcat (bf, "\n\rYour clothing would keep you very warm even in the middle of a blizzard!");
		      break;
		    }
		  lne++; 
		}
	    }
	}
      else if (!str_infix ("wim", code))
	{
	  sprintf (bf, "%d", (!ch->ced ? 0 : ch->ced->wimpy));
	}
      else if (!str_infix ("wm", code))
	{
	  sprintf (bf, "%s", capitalize (string_mana (mana_now (ch))));
	}
      else if (!str_infix ("ww", code))
	{
	  int weightmult = (get_curr_str(ch)+
			    (IS_AUGMENTED(ch, AUG_BODY_POWER) ? 2 : 0)+
			    (IS_AUGMENTED(ch, AUG_LEG_POWER) ? 2 : 0)+ 
			    (IS_AUGMENTED(ch, AUG_FOOT_POWER) ? 1 : 0))* WGT_MULT;
	  
	  if (ch->pcdata->carry_weight < weightmult *1)
	    sprintf (bf, "You are unburdened.");
	  else if (ch->pcdata->carry_weight < (weightmult * 2))
	    sprintf (bf, "No problem.        ");
	  else if (ch->pcdata->carry_weight < (weightmult * 3))
	    sprintf (bf, "It is a tad heavy. ");
	  else if (ch->pcdata->carry_weight < (weightmult * 4))
	    sprintf (bf, "It is heavy now.   ");
	  else if (ch->pcdata->carry_weight < (weightmult * 5))
	    sprintf (bf, "VERY heavy!        ");
	  else
	    sprintf (bf, "YOU ARE IN PAIN!   ");
	  
	}
      else if (!str_infix ("wp", code))
	{
	  sprintf (bf, "%ld", ch->pcdata->warpoints);
	}
      break;
      
    case 'X':
      if (!str_infix ("xp", code))
	{
	  sprintf (bf, "%ld", ch->exp);
	}
      else if (!str_infix ("xl", code))
	{
	  sprintf (bf, "%ld", ((FIND_EXP (LEVEL (ch), ch->race)) - ch->exp));
	}
      break;
    }
  
  
  sl = strlen (bf);
  if (pad > sl)
    {
      for (i = pad - sl; i > 0; i--)
	{
	  strcat (bf, " ");
	  if (strlen(bf)>80) break;
	}
    }
  
  if (pad > 1 && pad < sl)
    {
      bf[pad] = '\0';
    }
  
  return bf;
}





