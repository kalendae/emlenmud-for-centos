#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

bool 
locate_person (CHAR_DATA * ch, int level, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  bool found;
  CHAR_DATA *victim;
  one_argy (argy, arg);
  found = FALSE;
  for (victim = char_list; victim != NULL; victim = victim->next)
    {
      if (IS_MOB(victim)) continue;
      if (victim->in_room != NULL
	  && !IS_AFFECTED (victim, AFF_HIDE)
	  && (!IS_AFFECTED_EXTRA(victim, AFF_PROT_LOCATE))
	  && !IS_SET (victim->in_room->room_flags, ROOM_UNTRACKABLE)
	  && can_see (ch, victim)
	  && is_name (arg, NAME (victim)) && level + 40 >= LEVEL (victim))
	{
	  found = TRUE;
	  sprintf (buf, "%-28s %s\n\r",
		   PERS (victim, ch), show_room_name (victim, victim->in_room->name));
	  send_to_char (buf, ch);
	  
	  if (number_range (1, 3) == 1)
	    {
	      sprintf (buf, "\x1b[1;31mYou feel as if someone might have pin-pointed your location!\n\r", NAME (ch));
	      send_to_char (buf, victim);	      
	    }
	  else
	    if (number_range(1,2) ==1 )
	      {
		sprintf (buf, "\x1b[1;31mYou feel as if someone is pin-pointing your location...\n\r");
		send_to_char (buf, victim);

	      }
	    else
	      {
		sprintf(buf, "\x1b[1;31mSomeone has just pin-pointed your exact location!\n\r",
			NAME(ch));
		send_to_char(buf, victim);		
	      }
	  {
	    CHAR_DATA *rch;
	    check_room_more(victim->in_room);
	    for (rch = victim->in_room->more->people; rch != NULL; rch = rch->next_in_room)
	      {
		if(rch == victim || IS_MOB(victim)) continue;
		send_to_char("\x1b[1;31mAll of a sudden, you feel a ripple in the \x1b[1;30mVoid\x1b[1;31m, and then nothing.\n\r", rch);
	      }
	  }
	  break;
	}
    }
  return found;
}

  bool 
scry (CHAR_DATA * ch, int level, char *argy)
{
  char arg[SML_LENGTH];
  char buf[STD_LENGTH];
  bool found;
  CHAR_DATA *victim;
  one_argy (argy, arg);
  if (LEVEL (ch) > 100)
    return FALSE;
  found = FALSE;
  for (victim = char_list; victim != NULL; victim = victim->next)
    {
      if (victim->in_room != NULL
	  && !IS_AFFECTED (victim, AFF_HIDE)
	  && !IS_AFFECTED_EXTRA (victim, AFF_PROT_LOCATE)
	  && !IS_SET (victim->in_room->room_flags, ROOM_UNTRACKABLE)
	  && can_see (ch, victim)
	  && is_name (arg, NAME (victim)) &&
	  level + 35 >= LEVEL (victim))
	{
	  found = TRUE;
	  sprintf (buf, "%s look", NAME (victim));
	  do_at (ch, buf);
	  if (number_range (1, 3) == 1)
	    {
	      sprintf (buf, "\x1b[1;31mYou feel as if %s is watching you!\n\r", NAME (ch));
	      send_to_char (buf, victim);
	      send_to_char (buf, victim);
	      send_to_char (buf, victim);
	      
	    }
	  else
	    if (number_range(1,2) ==1 )
	      {
		sprintf (buf, "\x1b[1;31mYou feel as if someone is watching you..\n\r");
		send_to_char (buf, victim);
		send_to_char (buf, victim);
		send_to_char (buf, victim);
		
	      }
	    else
	      {
		sprintf(buf, "\x1b[1;31m%s is coming to kill you!\n\r",
			NAME(ch));
		send_to_char(buf, victim);
		send_to_char (buf, victim);
		send_to_char (buf, victim);
		
	      }
	  {
	    CHAR_DATA *rch;
	    check_room_more(victim->in_room);
	    for (rch = victim->in_room->more->people; rch != NULL; rch = rch->next_in_room)
	      {
		if(rch == victim || IS_MOB(victim)) continue;
		send_to_char("\x1b[1;31mAll of a sudden, you feel a ripple in the \x1b[1;30mVoid\x1b[1;31m, and then nothing.\n\r", rch);
	      }
	  }
	  
	  break;
	}
    }
  
  return found;
}

void 
do_finger (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  PLAYERBASE_DATA *playerbase;
  DEFINE_COMMAND ("finger", do_finger, POSITION_DEAD, 0, LOG_NORMAL, "This command can be used to lookup information on the specified player.")

  one_argy (argy, arg);

  /* ------------------------- */
  /* if no argument was passed */
  /* ------------------------- */
  if (arg[0] == '\0')
    {
      send_to_char ("Finger whom?\n\r", ch);
      return;
    }
  /* --------------------- */
  /* is victim online now? */
  /* --------------------- */
  if ((victim = get_char_world (ch, arg)) == NULL)
    {
      bool ffund;
      ffund = FALSE;
      for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
	{
	  if ( !str_cmp (argy, playerbase->player_name)
 	       && (playerbase->player_level == MAX_LEVEL || LEVEL (ch) > 100 
		|| (playerbase->player_align == ALIGN(ch))
		|| (playerbase->player_align == 0)))  { /* Neutral Align */
	      char bufr[500];
	      ffund = TRUE;

	      /* ----------------------------------------------------- */
	      /* Do not show any information to morals about immortals */
	      /* ----------------------------------------------------- */
  	      if (playerbase->player_level > 100 && LEVEL(ch) < 100) {
		    send_to_char ("Strain as you might, that knowledge is beyond your understanding.\n\r",ch);
		    return;
	      }

	      sprintf (bufr, "\x1B[37;0m[\x1B[1m%s\x1B[0m] Logged on last about \x1B[1m%ld\x1B[0m hours ago.\n\r",
		       playerbase->player_name,
		       (((current_time) - playerbase->player_last_login) / 3600));
	      send_to_char (bufr, ch);
	      if (LEVEL (ch) > MAX_LEVEL-3)
		{
		  sprintf (bufr, "GOD INFO ONLY: Fingered character is level %d with %d remorts.\n\r",
			   (playerbase->player_level), playerbase->player_remort_times);
		  send_to_char (bufr, ch);
		  sprintf(bufr, "Player was created %d days, %d hours ago.\n\r", (current_time - playerbase->player_created)/(3600*24),
				((current_time - playerbase->player_created)/(3600)) % 24);
		  send_to_char(bufr, ch);
		}
	      break;
	    }
	}
      if (!ffund)
	{
	  send_to_char ("Player not found or not visible.\n\r", ch);
	}
      return;
     }

  /* ----------------------------------------------------- */
  /* Do not show any information to morals about immortals */
  /* ----------------------------------------------------- */
  if (LEVEL(victim) > 100 && LEVEL(ch) < 100)
  {
    send_to_char ("Strain as you might, that knowledge is beyond your understanding.\n\r",ch);
    return;
  }
  
  if ((LEVEL(victim) < 100 && LEVEL (ch) < 100 && ALIGN(victim) != 0 && DIFF_ALIGN(ch, victim)) || !can_see (ch, victim) || IS_MOB (victim))
    {
      send_to_char ("Player not found or not visible.\n\r", ch);
      return;
    }

  sprintf (buf, "\x1B[1;34m%s\x1B[37;0m the %s %s is currently playing.\n\r", NAME (victim),
	   (SEX (victim) == SEX_MALE ? "male" :
	    SEX (victim) == SEX_FEMALE ? "female" : "sexless"),
	   race_info[victim->pcdata->race].name);
  send_to_char (buf, ch);
  if (DSC (victim) && DSC (victim)[0] != '\0')
    {
      sprintf (hugebuf_o, "Description:\n\r%s", DSC (victim));
      send_to_char (hugebuf_o, ch);
    }
  if (LEVEL (ch) > (MAX_LEVEL -3)&& victim->pcdata->email[0] != '\0')
    {
      sprintf (buf, "God Info: Email: %s.\n\r", victim->pcdata->email);
      send_to_char (buf, ch);
    }

  return;
}
