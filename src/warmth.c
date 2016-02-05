#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

bool 
check_body_temp (CHAR_DATA * ch)
{
  int warmth;
  SINGLE_OBJECT *obj;
  int temp = cur_temp (ch);
  if (!pow.weather || number_range (1, 3) != 2)
    return FALSE;
  if (IS_MOB (ch) || !ch->in_room || ch->in_room->sector_type == SECT_INSIDE ||
      (!IS_OUTSIDE(ch) || ch->in_room->sector_type >= SECT_CAVE || !IS_SET(ch->in_room->room_flags, ROOM_INDOORS)) || IS_SET (ch->act, PLR_HOLYWALK))
    return FALSE;
  if (ch->in_room->vnum < 1000)
    return FALSE;
  warmth = ch->pcdata->warmth;
  if (temp < 40 && warmth + temp < 45)
    {
      if (is_member(ch, GUILD_RANGER) && warmth+temp > 30)
	return FALSE;
      if (ch->position == POSITION_SLEEPING)
	{
	  send_to_char ("You awaken, wracked by shivering spasms from the cold...\n\r", ch);
	  NEW_POSITION(ch, POSITION_STANDING);
	}
      if (temp + warmth > 25)
	{
	  send_to_char ("It's quite chilly out, considering your attire...\n\r", ch);
	}
      else if (temp + warmth < 25)
	{
	  if (is_member (ch, GUILD_RANGER) && temp + warmth > 0)
	     return FALSE;
	  send_to_char ("You are FREEZING!! brrrrrr...\n\r", ch);
	  if (number_range(1,3) == 2)
	    if (SEX (ch) == SEX_MALE)
	      send_to_char ("Your manhood shrinks into boyhood as the cold grabs at your groin.\n\r", ch);
	    else
	      send_to_char ("The freezing air hits your bare legs and buttocks, causing shivering spasms.\n\r", ch);
	  if (number_range(1,3) == 2)
	    send_to_char ("Your nipples are all prickly in the freezing cold!\n\r", ch);
	  if (number_range(1,3) == 2)
	    send_to_char ("Your feet are completely numb and sore from the intense cold!\n\r", ch);
	}
      else
        return FALSE;
    SUBHIT(ch, number_range(6, (warmth-temp))); 
    return TRUE; 
    }  
  else if (temp > 95 && temp + warmth > 120)
    {
      if (is_member(ch, GUILD_RANGER) && temp + warmth < 150)
	return false;
      send_to_char ("You are HOT... Sweat covers your body, and your vision becomes fuzzy...\n\r", ch);
      SUBHIT(ch, number_range(7,(temp+warmth)));
      if (ch->move > 8)
	SUBMOVE(ch, number_range (5,15));
    }
  if (ch->hit < 0)
    {
      send_to_char ("Your vision blurs, and the world slips away in front of you...\n\r", ch);
      raw_kill (ch, FALSE);
      return TRUE;
    }
  return FALSE;
}
