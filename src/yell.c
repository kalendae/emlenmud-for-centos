#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

void 
undo_it (CHAR_DATA * ch, ROOM_DATA * rid, int strength, short came_from)
{
  int i = 0;
  if (strength < 1)
    return;
  if (rid->y == 0)
    return;
  rid->y = 0;
  for (i = 0; i < 6; i ++)
    {
      if (rid->exit[i] && rid->exit[i]->to_room)
	{
	  undo_it (ch, rid->exit[i]->to_room, strength--, i);
	}
    }
  return;
}

void 
do_it (CHAR_DATA * ch, ROOM_DATA * rid, int strength, short came_from)
{
  CHAR_DATA *fch;
  char buffr[1200];
  int ctr = 0;
  int i = 0;
  if (strength < 1)
    return;
  if (IS_SET (rid->room_flags, ROOM_NOHEAR))
    return;
  if (rid->y == TRUE)
    return;
  rid->y = TRUE;
  check_room_more(rid);
  for (fch = rid->more->people; fch != NULL; fch = fch->next_in_room)
    {
      if (fch == ch) continue;
      if (IS_MOB(fch)) continue;
      if (ignore(ch,fch)) continue;
      if (IS_SET (fch->pcdata->deaf, CHANNEL_YELL))
	continue;
      sprintf (buffr, "\x1b[1;31m%s yells, '%s'\x1b[0;37m\n\r", capitalize(rNAME (ch, fch)), yellbuff);
      send_to_char (buffr, fch);
    }
  for (i = 0; i < 6; i++)
    {
      if (rid->exit[i] && rid->exit[i]->to_room  && came_from != rev_dir[i])
	{
	  if (IS_SET (rid->room_flags, ROOM_NOISY))
	    ctr = 1;
	  else
	    ctr = 0;
	  if (rid->exit[i]->d_info)
	    if(IS_SET (rid->exit[i]->d_info->exit_info, EX_HIDDEN))
	      ctr = 9999;
	    else if (IS_SET (rid->exit[i]->d_info->exit_info, EX_CLOSED))
	      ctr++;
	  do_it (ch, rid->exit[i]->to_room, (strength - ctr - 1), i);
	}
    }
  
  return;
}

void 
find_people_n (CHAR_DATA * ch, int n)
{
  if (!ch->in_room)
    {
      return;
    }
  if (n > 25)
    {
      send_to_char ("Error in find_people_n: Max depth is 15.\n\r", ch);
      return;
    }
  do_it (ch, ch->in_room, n, 9999);
  undo_it (ch, ch->in_room, n, 9999);
  return;
}
