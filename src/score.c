#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

bool wwrap = TRUE;

char *
string_mana_p (int mana)
{
  static char ret[500];
  ret[0] = '\0';
  if (mana < 2)
    sprintf (ret, "Empty");
  else if (mana < 11)
    sprintf (ret, "Icy");
  else if (mana < 26)
    sprintf (ret, "Cold");
  else if (mana < 84)
    sprintf (ret, "Warm");
  else if (mana < 121)
    sprintf (ret, "Hot");
  else
    sprintf (ret, "Burning");
  return ret;
}

char *
string_mana (int mana)
{
  static char ret[500];
  ret[0] = '\0';
  if (mana < 2)
    sprintf (ret, "Empty");
  else if (mana < 11)
    sprintf (ret, "Nearly Empty");
  else if (mana < 26)
    sprintf (ret, "Some Energy");
  else if (mana < 40)
    sprintf (ret, "Moderate Energy");
  else if (mana < 67)
    sprintf (ret, "Powerful");
  else if (mana < 92)
    sprintf (ret, "Very Powerful");
  else if (mana < 114)
    sprintf (ret, "Extremely Powerful");
  else if (mana < 144)
    sprintf (ret, "Sparking with Energy");
  else if (mana < 195)
    sprintf (ret, "Extreme Magical Presence");
  else
    sprintf (ret, "Incredible Magical Presence");
  return ret;
}

void 
do_weight (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  DEFINE_COMMAND ("weight", do_weight, POSITION_DEAD, 0, LOG_NORMAL, "This command will show you a summary of how much weight you are carrying, and how hard it is to carry this weight.")
    
    sprintf (buf,
	     "\x1B[37;0mYou are carrying %d items (Weight: %d Stones, %d Pebbles) - ",
	     ch->pcdata->carry_number, (ch->pcdata->carry_weight/WGT_MULT), (ch->pcdata->carry_weight % WGT_MULT));
  send_to_char (buf, ch);
  {
    int weightmult = (get_curr_str(ch)+
      (IS_AUGMENTED(ch, AUG_BODY_POWER) ? 2 : 0)+
      (IS_AUGMENTED(ch, AUG_LEG_POWER) ? 2 : 0)+ 
      (IS_AUGMENTED(ch, AUG_FOOT_POWER) ? 1 : 0))*WGT_MULT;
    
    if (ch->pcdata->carry_weight < weightmult)
      sprintf (buf, "You are unburdened.\n\r");
    else if (ch->pcdata->carry_weight < (weightmult * 2))
      sprintf (buf, "No problem.        \n\r");
    else if (ch->pcdata->carry_weight < (weightmult * 3))
      sprintf (buf, "It is a tad heavy. \n\r");
    else if (ch->pcdata->carry_weight < (weightmult * 4))
      sprintf (buf, "It is heavy now.   \n\r");
    else if (ch->pcdata->carry_weight < (weightmult * 5))
      sprintf (buf, "VERY heavy!        \n\r");
    else
      sprintf (buf, "YOU ARE IN PAIN!   \n\r");
    send_to_char (buf, ch);
    return;
  }
}
void 
do_eeffect (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("effect", do_affect, POSITION_DEAD, 0, LOG_NORMAL, "This command shows what current affects your character has.")
    return;
}

void 
do_affect (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *tvict;
  AFFECT_DATA *paf;
  char buf[STD_LENGTH];
  SPELL_DATA *spell;
  bool enlarge_yn;
  DEFINE_COMMAND ("affect", do_affect, POSITION_DEAD, 0, LOG_NORMAL, "This command shows what current affects your character has.")

    enlarge_yn = FALSE;
  if (argy == "" || argy[0] == '\0' || LEVEL (ch) < MAX_LEVEL)
    tvict = ch;
  else if ((tvict = get_char_world (ch, argy)) == NULL)
    tvict = ch;
  if (tvict->affected != NULL)
    {
      send_to_char ("\x1B[1;37m---> \x1B[34mCurrent Affects \x1B[37m<---\x1B[37;0m\n\r\n\r", ch);
      for (paf = tvict->affected; paf != NULL; paf = paf->next)
	{
	  if ((spell = skill_lookup (NULL, paf->type)) == NULL)
	    continue;
	  sprintf (buf, "%s", capitalize (spell->spell_funky_name));
	  send_to_char (buf, ch);
	  if (IS_MOB (ch) || (LEVEL (ch) > 100))
	    sprintf (buf,
		     " (modifies %s by %d for %d mud hours)\n\r",
		     affect_loc_name (paf->location),
		     paf->modifier,
		     paf->duration);
	  else
	    {
	      if (paf->duration > 50)
		sprintf (buf, ", which won't wear off for many days!\n\r");
	      else if (paf->duration > 30)
		sprintf (buf, ", which will wear off in a day or two.\n\r");
	      else if (paf->duration > 20)
		sprintf (buf, ", which will wear off in about a day.\n\r");
	      else if (paf->duration > 10)
		sprintf (buf, ", which will wear off in about half a day.\n\r");
	      else if (paf->duration > 6)
		sprintf (buf, ", which will wear off in about a quarter of a day.\n\r");
	      else if (paf->duration > 3)
		sprintf (buf, ", which will wear off in a few hours.\n\r");
	      else if (paf->duration > 1)
		sprintf (buf, ", which will wear off soon.\n\r");
	      else if (paf->duration == 1)
		sprintf (buf, ", which will wear off very soon.\n\r");
	      else
		sprintf (buf, ", which will wear off at any moment.\n\r");
	    }
	  send_to_char (buf, ch);
	}
      send_to_char ("\n\r", ch);
      return;
    }
  else
    {
      send_to_char ("No affects present.\n\r", ch);
      return;
    }
  return;
}

int 
rating (CHAR_DATA * ch)
{
  int h = 0;
  int wdam = 0;
  int bonushit = 0;
  SINGLE_OBJECT *w_one;
  if (IS_MOB (ch))
    return 0;
  check_ced (ch);
  if ((w_one = ch->hold1) == NULL)
    wdam += 1;
  else
    {
      if (w_one->pIndexData->item_type == ITEM_WEAPON)
	{
	  wdam += (FIRSTDICE (w_one) * SECONDDICE (w_one)) / 2;
	  bonushit += addoffense2(ch, w_one);
	}
      else if (w_one->pIndexData->item_type == ITEM_GEM)
	{
	  int color = TYPEMANA(w_one);
	  wdam += (TOPMANA(w_one))/10;
	  wdam += (TOPLEVEL(w_one))/10;
	  if (IS_SET(color, MANA_AIR)) wdam++;
	  if (IS_SET(color, MANA_FIRE)) wdam++;
	  if (IS_SET(color, MANA_EARTH)) wdam++;
	  if (IS_SET(color, MANA_WATER)) wdam++;
	  if (IS_SET(color, MANA_SPIRIT)) wdam++;
	}
      else wdam +=1;
    }
  if ((w_one = ch->hold2) ==NULL)
    wdam += 1;
  else
     {
      if (w_one->pIndexData->item_type == ITEM_WEAPON)
       {
        wdam += (FIRSTDICE (w_one) * SECONDDICE (w_one)) / 2;
        bonushit += addoffense2(ch, w_one);
       }
      else if (w_one->pIndexData->item_type == ITEM_GEM) 
      {
         int color = TYPEMANA(w_one);
         wdam += (TOPMANA(w_one))/10;
         wdam += (TOPLEVEL(w_one))/10;
         if (IS_SET(color, MANA_AIR)) wdam++;
         if (IS_SET(color, MANA_FIRE)) wdam++;
         if (IS_SET(color, MANA_EARTH)) wdam++;
         if (IS_SET(color, MANA_WATER)) wdam++;
         if (IS_SET(color, MANA_SPIRIT)) wdam++;
       }
      else wdam +=1;
    }
  h = ((GET_AC (ch)) / 3) + get_curr_str (ch) + get_curr_int (ch) +
get_curr_dex (ch) + get_curr_con (ch) + get_curr_wis (ch) +	
(wdam * 4) + (ch->max_hit / 18) + (max_mana (ch) / 4) + ((LEVEL(ch)*3)/2) +
bonushit + GET_HITROLL(ch) + (GET_DAMROLL (ch)) - (ch->pcdata->deaths);
 return h;

}

/*
   The attribute command
 */
void 
do_attribute (CHAR_DATA * ch, char *argy)
{
  char buf[500];
  DEFINE_COMMAND ("attribute", do_attribute, POSITION_DEAD, 0, LOG_NORMAL, "This command shows you various info about your character.")
    if (IS_MOB (ch))
	return;
  if (LEVEL (ch) < pow.level_to_see_numeric_stats)
    {
      char strg[40];
      char con[40];
      char dex[40];
      char wis[40];
      char inte[40];
      if (get_curr_str (ch) < 10)
	sprintf (strg, "Weakling");
      else if (get_curr_str (ch) < 15)
	sprintf (strg, "Fairly Weak");
      else if (get_curr_str (ch) < 18)
	sprintf (strg, "Well-Toned");
      else if (get_curr_str (ch) < 21)
	sprintf (strg, "Very Strong");
      else if (get_curr_str (ch) < 24)
	sprintf (strg, "Extremely Strong");
      else if (get_curr_str (ch) < 26)
	sprintf (strg, "Incredibly Strong");
      else
	sprintf (strg, "Superhuman");
      if (get_curr_int (ch) < 10)
	sprintf (inte, "Brainless ");
      else if (get_curr_int (ch) < 15)
	sprintf (inte, "Pretty Dumb");
      else if (get_curr_int (ch) < 18)
	sprintf (inte, "Moderately Intelligent");
      else if (get_curr_int (ch) < 21)
	sprintf (inte, "Very Smart");
      else if (get_curr_int (ch) < 24)
	sprintf (inte, "Extremely Smart");
      else
	sprintf (inte, "Genius ");
      if (get_curr_con (ch) < 10)
	sprintf (con, "Flabby");
      else if (get_curr_con (ch) < 15)
	sprintf (con, "Overweight");
      else if (get_curr_con (ch) < 18)
	sprintf (con, "Average Constitution");
      else if (get_curr_con (ch) < 21)
	sprintf (con, "Tough");
      else if (get_curr_con (ch) < 24)
	sprintf (con, "Very Tough");
      else
	sprintf (con, "Amazingly Durable");
      if (get_curr_wis (ch) < 10)
	sprintf (wis, "No Common Sense");
      else if (get_curr_wis (ch) < 15)
	sprintf (wis, "Ignorant");
      else if (get_curr_wis (ch) < 18)
	sprintf (wis, "Good Common Sense");
      else if (get_curr_wis (ch) < 21)
	sprintf (wis, "Very Wise");
      else if (get_curr_wis (ch) < 24)
	sprintf (wis, "Extremely Wise ");
      else
	sprintf (wis, "Sage-Like ");
      if (get_curr_dex (ch) < 10)
	sprintf (dex, "Snail-Like");
      else if (get_curr_dex (ch) < 15)
	sprintf (dex, "Lathargic");
      else if (get_curr_dex (ch) < 18)
	sprintf (dex, "Average Dexterity");
      else if (get_curr_dex (ch) < 21)
	sprintf (dex, "Above Average Dexterity");
      else if (get_curr_dex (ch) < 24)
	sprintf (dex, "Very Quick");
      else
	sprintf (dex, "Speed Demon");
      
      send_to_char("\x1b[1;32m+\x1b[1;34m---------------------------------------\x1b[1;32m+\x1b[0;37m\n\r", ch);
      sprintf (buf, " Strength      : \x1b[1;37m%-10s\x1b[0;37m\n\r",strg);
      send_to_char(buf, ch);
      sprintf (buf, " Constitution  : \x1b[1;32m%-10s\x1b[0;37m\n\r",con);
      send_to_char(buf, ch);
      sprintf (buf, " Dexterity     : \x1b[1;35m%-10s\x1b[0;37m\n\r",dex);
      send_to_char(buf, ch);
      sprintf (buf, " Intelligence  : \x1b[1;33m%-10s\x1b[0;37m\n\r",inte);
      send_to_char(buf, ch);
      sprintf (buf, " Wisdom        : \x1b[1;31m%-10s\x1b[0;37m\n\r", wis);
      send_to_char(buf, ch);
      send_to_char("\x1b[1;32m+\x1b[1;34m---------------------------------------\x1b[1;32m+\x1b[0;37m\n\r", ch); 
   
    }
  else
    {
       send_to_char("\x1b[1;32m+\x1b[1;34m---------------------------------------\x1b[1;32m+\x1b[0;37m\n\r", ch);
      sprintf (buf, " Strength      : \x1b[1;37m%d\x1b[0;37m\n\r",get_curr_str(ch));
      send_to_char(buf, ch);
      sprintf (buf, " Constitution  : \x1b[1;32m%d\x1b[0;37m\n\r",get_curr_con(ch));
      send_to_char(buf, ch);
      sprintf (buf, " Dexterity     : \x1b[1;35m%d\x1b[0;37m\n\r",get_curr_dex(ch));
      send_to_char(buf, ch);
      sprintf (buf, " Intelligence  : \x1b[1;33m%d\x1b[0;37m\n\r",get_curr_int(ch));
      send_to_char(buf, ch);
      sprintf (buf, " Wisdom        : \x1b[1;31m%d\x1b[0;37m\n\r", get_curr_wis(ch));
      send_to_char(buf, ch);
      send_to_char("\x1b[1;32m+\x1b[1;34m---------------------------------------\x1b[1;32m+\x1b[0;37m\n\r", ch);  
     
     
    }
  return;
}


void 
do_info (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  int ii;
  char tmpbuf[500];
  CHAR_DATA *tvict;
  int bonushit;
  DEFINE_COMMAND ("information", do_info, POSITION_DEAD, 0, LOG_NORMAL, "This command shows you an info page about your character.")

    if (LEVEL (ch) < MAX_LEVEL && FIGHTING (ch) != NULL && !pow.score_while_fighting)
    {
      send_to_char ("You cannot info while fighting!\n\r", ch);
      return;
    }
  ansi_color (NTEXT, ch);
  bonushit = 0;
  if (argy == "" || argy[0] == '\0' || LEVEL (ch) < MAX_LEVEL)
    tvict = ch;
  else
    {
      if ((tvict = get_char_world (ch, argy)) == NULL)
	tvict = ch;
    }
/*if (IS_MOB(tvict)) 
   {
   send_to_char("You may not INFO NPC's.\n\r",ch);
   return;
   }
 */
  check_ced (tvict);
  check_ced (ch);
  sprintf (buf, "\x1B[1;32m%d \x1B[37m[%s %s\x1B[37m] \x1B[0m%s %s\x1B[37;0m\n\r",
	   LEVEL (tvict), align_info[tvict->pcdata->alignment].name,
   race_info[tvict->pcdata->race].name, NAME (tvict), tvict->pcdata->title);
  send_to_char (buf, ch);
  send_to_char ("+--------------------------------------------------------------------------+\n\r", ch);
  sprintf (buf, "\x1B[1;34m Hitpts: \x1B[37m%5ld\x1B[34m/\x1B[36m%-5ld  \x1B[34mMovement: \x1B[37m%5d\x1B[34m/\x1B[36m%-5d\x1B[34m   Mana: \x1B[37m%s\x1B[0m\n\r",
	   tvict->hit, tvict->max_hit, tvict->move, tvict->max_move, string_mana (mana_now (tvict)));
  send_to_char (buf, ch);
  sprintf (buf, "\x1B[1;34m TKills: \x1B[31m%-12d \x1B[34mKillPnts: \x1B[31m%-12d \x1B[34mWarPts: \x1B[31m%ld\n\r",
	   tvict->pcdata->totalkills, tvict->pcdata->killpoints, tvict->pcdata->warpoints);
  send_to_char (buf, ch);
  sprintf (buf, "\x1B[1;34m InBank: \x1B[37m%-12ld \x1B[34mMoneyNow: \x1B[37m%-12d \x1B[34mPlrAge: \x1B[37m%d\n\r",
	   tvict->pcdata->bank, tally_coins (tvict), get_age (tvict));
  send_to_char (buf, ch);
  sprintf (buf, "\x1B[1;34m CWimpy: \x1B[37m%-12d \x1B[34mTRemorts: \x1B[37m%-12d \x1B[34mPagLen: \x1B[37m%d\x1B[0m\n\r",
	   tvict->ced->wimpy, tvict->pcdata->remort_times,
	   tvict->pcdata->pagelen);
  send_to_char (buf, ch);
  sprintf (tmpbuf, "\x1B[1;34mHeight: \x1B[1;37m%d' %d\"\x1B[0m", tvict->height / 12, tvict->height % 12);
  sprintf (buf, "\x1B[1;34m Learns: \x1B[37m%-12d \x1B[34mPractces: \x1B[37m%-12d \x1B[37;0m%s\n\r",
	   tvict->pcdata->learn, tvict->pcdata->practice, tmpbuf);
  send_to_char (buf, ch);
  sprintf (buf, " %s %s %s\n\r",
	   (tvict->pcdata->condition[COND_FULL] < 7 ? "\x1B[1;37m\x1B[5mHungry\x1B[0m" : " "),
	   (tvict->pcdata->condition[COND_THIRST] < 7 ? "\x1B[1;37m\x1B[5mThirsty\x1B[0m" : " "),
	   (tvict->pcdata->condition[COND_DRUNK] > 10 ? "\x1B[1;37m\x1B[5mDrunk\x1B[0m" : " "));
  send_to_char (buf, ch);
  send_to_char ("+--------------------------------------------------------------------------+\n\r", ch);
  if (LEVEL(ch) >= pow.level_to_see_numeric_stats){
  sprintf (buf, "\x1B[1;34m Str: \x1B[37m%-2d \x1B[34mInt: \x1B[37m%-2d \x1B[34mWis: \x1B[37m%-2d \x1B[34mDex: \x1B[37m%-2d \x1B[34mCon: \x1B[37m%-2d\x1B[37;0m Alignmt: \x1B[1;37m%d\n\r",
	   get_curr_str (tvict), get_curr_int (tvict), get_curr_wis (tvict),
	   get_curr_dex (tvict), get_curr_con (tvict), ALIGN (tvict));
  send_to_char (buf, ch);}
  sprintf (buf,
	   "\x1B[1;36m You are carrying \x1B[1;36m%d\x1B[0;37m Stones and \x1b[1;36m%d\x1b[0;37m Pebbles \x1B[0;37m(",
	   (tvict->pcdata->carry_weight/WGT_MULT), (tvict->pcdata->carry_weight % WGT_MULT));
  send_to_char (buf, ch);
  {
    int weightmult = (get_curr_str(tvict)+
      (IS_AUGMENTED(tvict, AUG_BODY_POWER) ? 2 : 0)+
      (IS_AUGMENTED(tvict, AUG_LEG_POWER) ? 2 : 0)+ 
      (IS_AUGMENTED(tvict, AUG_FOOT_POWER) ? 1 : 0))*WGT_MULT;
    if (tvict->pcdata->carry_weight < weightmult)
      sprintf (buf, "You are unburdened.)\n\r");
    else if (tvict->pcdata->carry_weight < (weightmult * 2))
      sprintf (buf, "No problem.)        \n\r");
    else if (tvict->pcdata->carry_weight < (weightmult * 3))
      sprintf (buf, "It is a tad heavy.) \n\r");
    else if (tvict->pcdata->carry_weight < (weightmult * 4))
      sprintf (buf, "It is heavy now.)   \n\r");
    else if (tvict->pcdata->carry_weight < (weightmult * 5))
      sprintf (buf, "VERY heavy!)        \n\r");
    else
      sprintf (buf, "YOU ARE IN PAIN!)  \n\r");

   
    send_to_char (buf, ch);
  }

  sprintf(buf,"Elemental Shards: %s\n\r",elemental_alignment_bit_name(tvict->pcdata->elemental_alignment));
  send_to_char(buf,ch);

  ii = clan_number (tvict);
  if (ii > 0)
    {
      sprintf (buf, " \x1B[37;0mMember of \x1B[1;36m%s\x1B[0;37m clan.\n\r", get_clan_index (ii)->name);
      send_to_char (buf, ch);
    }
  if (tvict->pcdata->guilds != 0)
    {
      show_guilds(ch, tvict->pcdata->guilds);
      
    }
  return;
}

void 
do_quickscore2 (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("statistics", do_quickscore, POSITION_DEAD, 0, LOG_NORMAL, "This command shows a brief summary of your character.")
    return;
}

void 
do_quickscore (CHAR_DATA * ch, char *argy)
{
  int pos;
  DEFINE_COMMAND ("sco", do_quickscore, POSITION_DEAD, 0, LOG_NORMAL, "This command shows a brief summary of your character.")
    if (FIGHTING (ch) && !pow.score_while_fighting)
    {
      send_to_char ("You cannot use this command during combat.\n\r", ch);
      return;
    }


  pos = ch->position;
  ch->position = POSITION_STANDING;
  wwrap = FALSE;
  if (sh_score != NULL)
    act (interpret_line (sh_score, ch), ch, NULL, ch, TO_CHAR);
  else
    act (interpret_line (score, ch), ch, NULL, ch, TO_CHAR);
  wwrap = TRUE;
  ch->position = pos;

  return;
}


void 
do_vscore (CHAR_DATA * ch, char *argy)
{
  int pos;
  DEFINE_COMMAND ("vscore", do_vscore, POSITION_DEAD, 0, LOG_NORMAL, "This command shows you complete information about your character.")

    if (FIGHTING (ch) && !pow.score_while_fighting)
    {
      send_to_char ("You cannot use this command during combat.\n\r", ch);
      return;
    }

  pos = ch->position;
  ch->position = POSITION_STANDING;
  wwrap = FALSE;
  act (interpret_line (score, ch), ch, NULL, ch, TO_CHAR);
  wwrap = TRUE;
  ch->position = pos;

  return;
}



void 
do_score (CHAR_DATA * ch, char *argy)
{
  int pos;
  DEFINE_COMMAND ("score", do_score, POSITION_DEAD, 0, LOG_NORMAL, "This command shows you complete information about your character.")
    if (FIGHTING (ch) && !pow.score_while_fighting)
    {
      send_to_char ("You cannot use this command during combat.\n\r", ch);
      return;
    }

  pos = ch->position;
  ch->position = POSITION_STANDING;
  wwrap = FALSE;
  act (interpret_line (score, ch), ch, NULL, ch, TO_CHAR);
  wwrap = TRUE;
  ch->position = pos;

  return;
}

void 
read_score (CHAR_DATA * ch, char *argy)
{
  FILE *ff;
  DEFINE_COMMAND ("z_reload_score", read_score, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command rereads score.dat from disk.")

    if (score)
    free (score);
  if (sh_score)
    free (sh_score);
  score = NULL;
  sh_score = NULL;

  if ((ff = fopen ("score.dat", "r")) == NULL)
    {
      fprintf (stderr, "Error on score.dat read.\n");
      exit (20);
    }
  else
    {
      score = str_dup (fread_string2 (ff, NULL));
      fclose (ff);
    }

  if ((ff = fopen ("sh_score.dat", "r")) == NULL)
    {
      fprintf (stderr, "Error on sh_score.dat read... generalizing to score.dat.\n");
      sh_score = NULL;
    }
  else
    {
      sh_score = str_dup (fread_string2 (ff, NULL));
      fclose (ff);
    }

  return;
}
