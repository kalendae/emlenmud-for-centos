
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
/* climb any item of type ITEM_CLIMBABLE 
   Some of this code was borrowed from the previous mud code 
*/
void 
do_climb (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *fch, *fch_next;
  SINGLE_OBJECT *obj;
  ROOM_DATA *to_room;
  ROOM_DATA *in_room;
  I_CLIMBABLE *c;
  char *arg;
  char buf[SML_LENGTH];
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  int dir;
  DEFINE_COMMAND ("climb", do_climb, POSITION_STANDING, 0, LOG_NORMAL, "Allows you to climb up <object> or climb down <object>.")


    arg = one_argy (argy, arg2);
  one_argy (arg, arg1);
  if (arg1[0] == '\0')
    {
      send_to_char ("Climb what?\n\r", ch);
      return;
    }
  if ((obj = get_obj_here (ch, arg1, SEARCH_ROOM_FIRST)) == NULL)
    {
      send_to_char ("There's nothing here to climb.\n\r", ch);
      return;
    }
  if (obj->pIndexData->item_type == ITEM_SCALABLE)
    {
      if (get_item_held (ch, ITEM_SCALE_DEV) == NULL)
	{
	  send_to_char ("You need something to scale it with.\n\r", ch);
	  return;
	}
    }
  else if (obj->pIndexData->item_type != ITEM_CLIMBABLE)
    {
      send_to_char ("You can't climb that!\n\r", ch);
      return;
    }
  c = (I_CLIMBABLE *) obj->more;
  if (RIDING (ch) != NULL)
    {
      send_to_char ("You're mounted; you can't climb!\n\r", ch);
      return;
    }
  if (MOUNTED_BY (ch) != NULL)
    {
      send_to_char ("You can't climb while mounted.\n\r", ch);
      return;
    }
  dir = c->up_down;
  if (arg2[0] == '\0' && dir == 2)
    {
      send_to_char ("Which way? Up or down?\n\r", ch);
      return;
    }
  in_room = ch->in_room;
  switch (arg2[0])
    {
    case 'U':
    case 'u':
      if (dir == 0)
	{
	  send_to_char ("It doesn't lead anywhere.\n\r", ch);
	  return;
	}
      else
	{
	  if ((to_room = get_room_index (c->above)) == NULL)
	    {
	      send_to_char ("It doesn't lead anywhere.\n\r", ch);
	      bug ("Do_climb: bad vnum up %d.",
		   c->above);
	      return;
	    }
	  dir = 1;
	}
      break;
    case 'D':
    case 'd':
      if (dir == 1)
	{
	  send_to_char ("It doesn't lead anywhere.\n\r", ch);
	  return;
	}
      else
	{
	  if ((to_room = get_room_index (c->below)) == NULL)
	    {
	      send_to_char ("It doesn't lead anywhere.\n\r", ch);
	      bug ("Do_climb: bad vnum down %d.",
		   c->below);
	      return;
	    }
	  dir = 0;
	}
      break;
    default:
      if (dir == 0)
	{
	  if ((to_room = get_room_index (c->below)) == NULL)
	    {
	      send_to_char ("It doesn't lead anywhere.\n\r", ch);
	      bug ("Do_climb: bad vnum down %d.",
		   c->below);
	      return;
	    }
	}
      else
	{
	  if ((to_room = get_room_index (c->above)) == NULL)
	    {
	      send_to_char ("It doesn't lead anywhere.\n\r", ch);
	      bug ("Do_climb: bad vnum up %d.",
		   c->above);
	      return;
	    }
	  dir = 1;
	}
      break;
    }
  if (IS_PLAYER (ch))
    {
      int move = 0;
      move -= ch->pcdata->carry_number - get_curr_str (ch);
      if (move < 2)
	move = 2;
      if (ch->move < move)
	{
	  send_to_char ("You are too exhausted.\n\r", ch);
	  return;
	}
      WAIT_STATE (ch, 1);
      SUBMOVE(ch,move);
    }
  if (MOUNTED_BY (ch) != NULL)
    sprintf (buf, "You climb %s %s, rode by %s.",
	     (dir == 0 ? "down" : "up"), obj->pIndexData->short_descr,
	     PERS (MOUNTED_BY (ch), ch));
  else if (IS_AFFECTED (ch, AFF_SNEAK))
    sprintf (buf, "You sneak %s %s.",
	     (dir == 0 ? "down" : "up"), obj->pIndexData->short_descr);
  else if (IS_AFFECTED (ch, AFF_FLYING))
    sprintf (buf, "You fly %s %s.",
	     (dir == 0 ? "down" : "up"), obj->pIndexData->short_descr);
  else
    sprintf (buf, "You climb %s %s.",
	     (dir == 0 ? "down" : "up"), obj->pIndexData->short_descr);
  buf[0] = UPPER (buf[0]);
  send_to_char (buf, ch);
  send_to_char ("\n\r", ch);
  check_room_more (in_room);
  for (fch = in_room->more->people; fch != NULL; fch = fch_next)
    {
      fch_next = fch->next_in_room;
      if (ch == fch || MOUNTED_BY (ch) == fch)
	continue;
      if (!can_see (fch, ch))
	continue;
      if (MOUNTED_BY (ch) != NULL)
	break;
      else if (IS_AFFECTED (ch, AFF_SNEAK))
	{
	  if (IS_SET (fch->act, PLR_HOLYLIGHT))
	    sprintf (buf, "%s sneaks %s %s.", PERS (ch, fch),
		     (dir == 0 ? "down" : "up"),
		     obj->pIndexData->short_descr);
	  else
	    buf[0] = '\0';
	}
      else if (IS_MOB (ch) && IS_SET (ch->act, ACT_SCRAMBLE))
	sprintf (buf, "%s rushes %s %s.", PERS (ch, fch),
		 (dir == 0 ? "down" : "up"),
		 obj->pIndexData->short_descr);
      else if (IS_AFFECTED (ch, AFF_FLYING))
	sprintf (buf, "%s flies %s %s.", PERS (ch, fch),
		 (dir == 0 ? "down" : "up"),
		 obj->pIndexData->short_descr);
      else
	sprintf (buf, "%s climbs %s %s.", PERS (ch, fch),
		 (dir == 0 ? "down" : "up"),
		 obj->pIndexData->short_descr);
      buf[0] = UPPER (buf[0]);
      send_to_char (buf, fch);
      if (buf[0] != '\0')
	send_to_char ("\n\r", fch);
    }
  char_from_room (ch);
  char_to_room (ch, to_room);
  if (MOUNTED_BY (ch) != NULL && ch->in_room != MOUNTED_BY (ch)->in_room)
    {
      do_climb (MOUNTED_BY (ch), argy);
    }
  check_room_more (in_room);
  for (fch = to_room->more->people; fch != NULL; fch = fch_next)
    {
      fch_next = fch->next_in_room;
      if (ch == fch || MOUNTED_BY (ch) == fch)
	continue;
      if (!can_see (fch, ch))
	continue;
      if (MOUNTED_BY (ch) != NULL)
	break;
      else if (IS_AFFECTED (ch, AFF_SNEAK))
	{
	  if (IS_SET (fch->act, PLR_HOLYLIGHT))
	    sprintf (buf, "%s sneakily climbs %s from %s.", PERS (ch, fch),
		     (dir == 0 ? "down" : "up"),
		     (dir == 0 ? "above" : "below"));
	  else
	    buf[0] = '\0';
	}
      else if (IS_MOB (ch) && IS_SET (ch->act, ACT_SCRAMBLE))
	sprintf (buf, "%s rushes %s from %s.", PERS (ch, fch),
		 (dir == 0 ? "down" : "up"),
		 (dir == 0 ? "above" : "below"));
      else if (IS_AFFECTED (ch, AFF_FLYING))
	sprintf (buf, "%s flies %s from %s.", PERS (ch, fch),
		 (dir == 0 ? "down" : "up"),
		 (dir == 0 ? "above" : "below"));
      else
	sprintf (buf, "%s climbs %s from %s.", PERS (ch, fch),
		 (dir == 0 ? "down" : "up"),
		 (dir == 0 ? "above" : "below"));
      buf[0] = UPPER (buf[0]);
      if (IS_AWAKE (fch))
	send_to_char (buf, fch);
      if (buf[0] != '\0')
	send_to_char ("\n\r", fch);
    }
  do_look (ch, "auto");
  do_silentscan(ch, 0);
  if (HUNTING (ch) != NULL)
    do_track (ch, "continue");
  if (PHUNTING (ch) !=NULL)
    do_ptrack (ch, "continue");
  check_room_more (in_room);
  for (fch = in_room->more->people; fch != NULL; fch = fch_next)
    {
      fch_next = fch->next_in_room;
      if (MASTER (fch) == ch && fch->position >= POSITION_STANDING)
	{
	  act ("You follow $N $t.", fch,
	       (dir == 0 ? "down" : "up"), ch, TO_CHAR);
	  do_climb (fch, argy);
	}
    }
 
  return;
}
