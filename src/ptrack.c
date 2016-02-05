#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"



void  /* JRAJRA - Player track command */
do_ptrack (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  DEFINE_COMMAND ("track", do_ptrack, POSITION_STANDING, 0, LOG_NORMAL, "Allows you to search for player tracks...even after they log out.")

  if (ch->position == POSITION_FIGHTING)
    return;
if (ch->pcdata->voting_on == 999 || HUNTING(ch) != NULL)
  {
    send_to_char("You can only track one thing at a time!", ch);
    return;
  }
if (IS_PLAYER (ch) && ch->pcdata->learned[gsn_track] < 3 && !is_member (ch, GUILD_RANGER))
    {
      send_to_char ("You really have no clue what you are doing. It would be helpful to\n\rlearn the PTRACK skill first!\n\r", ch);
      return;
    }
one_argy (argy, arg);
if (IS_MOB(ch))
  return;
if (arg[0] == '\0' && PHUNTING (ch) == NULL)
  {
    send_to_char ("Whom are you trying to track?\n\r", ch);
    ch->pcdata->voting_on = 0;
    return;
  }
if (arg[0] == '\0' && PHUNTING (ch) != NULL)
    {
      do_ptrack (ch, "continue");
      ch->pcdata->voting_on = 0;
      return;
    }

if (strcmp (arg, "continue"))
    {
      if (!str_cmp (arg, NAME (ch)) || !str_cmp(arg, "me"))
	{
	  send_to_char ("Strange... the tracks seem to end right here.\n\r", ch);
	  if (PHUNTING (ch) != NULL)
	    {
	      free_string (ch->fgt->phunting);
	      ch->fgt->phunting = NULL;
	      ch->pcdata->voting_on = 0;
	    }
	  return;
	}
      if (PHUNTING (ch) != NULL)
	{
	  send_to_char (
			"Type TRACK and your name to stop tracking your quarry.\n\r", ch);
	  ch->pcdata->voting_on = 0;
	  return;
	}
      send_to_char ("You begin to look for tracks...\n\r", ch);
      if (HUNTING (ch) == NULL)
	{
	  check_fgt (ch);
	  if (strlen (arg) < 16)
	    ch->fgt->phunting = str_dup (arg);
	  ch->pcdata->tickcounts = 9;
          if (ch->position == POSITION_CASTING) {
            NEW_POSITION(ch, POSITION_STANDING); // prevent casting
            send_to_char("You stop casting your spell.\n\r", ch);
          }
	  ch->pcdata->voting_on = 998;
	  return;
	}
    }
else
    {
      if (PHUNTING (ch) == NULL)
	{
	  send_to_char ("You aren't tracking anyone.\n\r", ch);
	  ch->pcdata->voting_on = 0;
	  return;
	}
      send_to_char ("You begin to look for tracks...\n\r", ch);
      ch->pcdata->tickcounts = 9;
          if (ch->position == POSITION_CASTING) {
            NEW_POSITION(ch, POSITION_STANDING); // prevent casting
            send_to_char("You stop casting your spell.\n\r", ch);
          }
      ch->pcdata->voting_on = 998;
    }
  return;
}

/*******
Actual track command where it is accessed from comm.c
******/


void 
do_ptrack_t (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  PTRACK_DATA *ptr;
  int dir;
  if (ch->position == POSITION_FIGHTING)
    return;
  if (IS_MOB(ch))
    return;
  if (IS_PLAYER (ch) && ch->pcdata->learned[gsn_track] < 3 && !is_member (ch, GUILD_RANGER))
    {
      send_to_char ("You really have no clue what you are doing. It would be helpful to\n\rlearn the TRACK skill first!\n\r", ch);
      return;
    }
  one_argy (argy, arg);
  if (arg[0] == '\0' && PHUNTING (ch) == NULL)
    {
      send_to_char ("Whom are you trying to track?\n\r", ch);
      return;
    }
  if (!strcmp (arg, NAME(ch)) || !strcmp (arg, "me"))
      {
	send_to_char ("Strange, the tracks seem to end here.", ch);
	ch->pcdata->voting_on = 0;
	return;
      }
  if (arg[0] == '\0' && PHUNTING (ch) != NULL)
    {
      do_ptrack (ch, "continue");
      return;
    }
  if (PHUNTING (ch) == NULL)
    {
      send_to_char ("You aren't tracking anyone.\n\r", ch);
      return;
    }
  if (IS_PLAYER (ch) &&
      (ch->pcdata->learned[gsn_track] < number_range (0, 50) && !is_member (ch, GUILD_RANGER)))
    {
      send_to_char ("You find some footprints of some sort, but aren't good enough to determine\n\r", ch);
      send_to_char ("if they are your victim's, how old they are, or where they lead...\n\r", ch);
      ch->pcdata->voting_on=0;
      return;
    }
  for (ptr = ch->in_room->ptracks; ptr != NULL; ptr = ptr->next_track_in_room)
    {
      if (!str_cmp (PHUNTING (ch), ptr->online->name))
	{
	  if (ptr->dir_left == 10 || !strcmp(NAME(ch), ptr->online->name))
	    {
	      send_to_char ("Strange... the tracks seem to end right here.\n\r", ch);
	      if (ch->fgt->phunting != NULL )
		{
		  free_string (ch->fgt->phunting);
		  ch->fgt->phunting = NULL;
		}
	      ch->pcdata->voting_on = 0;
	      return;
	    }
	  
	  dir = ptr->dir_came;
	  if (dir != 10)
	    {
	      if (dir >= 4)
		sprintf (buf, "The tracks suggest that they entered the area from %s.\n\r", dir == 4 ? "above" : "below");
	      else
		sprintf (buf, "The tracks suggest that they entered the area from the %s.\n\r", dir_name[dir]);
	      send_to_char (buf, ch);
	    }
	  dir = ptr->dir_left;
	  if (dir >= 4)
	    sprintf (buf, "The tracks appear to leave the area right %s you!\n\r",  dir == 4 ? "above" : "below");
	  else
	    sprintf (buf, "The tracks appear to leave the area heading %s!\n\r", dir_name[dir]);
	  send_to_char (buf, ch);
	  if (PHUNTING (ch) == NULL)
	    {
	      check_fgt (ch);
	      if (strlen (arg) < 16)
		ch->fgt->phunting = str_dup (arg);
	    }
	  if (number_range (1, 3) == 2)
	    skill_gain (ch, gsn_track, TRUE);
	  return;
	}
    }
  send_to_char ("You find some footprints of some sort, but aren't good enough to determine\n\r", ch);
  send_to_char ("if they are your victim's, how old they are, or where they lead...\n\r", ch);
  return;
}





