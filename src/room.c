#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

#ifndef WINDOWS
#include <sys/time.h>
#endif


void 
z_areachange (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  int start_vnum = -1;
  int end_vnum = -1;
  int new_area_num = -1;
  int i;
  ROOM_DATA *room;
  AREA_DATA *a;
  AREA_DATA *destination_area = NULL;
  DEFINE_COMMAND ("z_areachange", z_areachange, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command moves vnums into a new area, without changing the vnums.")

    argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  start_vnum = atoi (arg1);
  end_vnum = atoi (arg2);
  new_area_num = atoi (argy);
  if (start_vnum <= 0 || end_vnum <= 0 || start_vnum > end_vnum ||
      new_area_num < 0)
    {
      send_to_char ("Invalid.  Syntax: z_areachange <min vnum> <max vnum> <new area>.\n\r", ch);
      return;
    }

  for (a = area_first; a != NULL; a = a->next)
    {
      if (a->vnum == new_area_num)
	{
	  destination_area = a;
	  break;
	}
    }

  if (!destination_area)
    {
      send_to_char ("Destination area vnum not found.\n\r", ch);
      return;
    }

/* Purge the world of mobs */
  for (i = 0; i < HASH_MAX; i++)
    {
      for (room = room_hash[i]; room != NULL; room = room->next)
	{
	  raw_purge (room);
	}
    }

  for (i = start_vnum; i < end_vnum; i++)
    {
      if ((room = get_room_index (i)) == NULL)
	continue;
      room->area = destination_area;
    }

  reset_world ();

  send_to_char ("Operation complete.  Please asave complete at this time.\n\r", ch);
  return;
}

void 
do_idea (CHAR_DATA * ch, char *argy)
{
  FILE *fpp;
  DEFINE_COMMAND ("idea", do_idea, POSITION_DEAD, 0, LOG_NORMAL, "Allows you to enter an idea for the admin to read later on.")
    if (!ch)
    return;
  if ((fpp = fopen ("ideas.txt", "a+")) == NULL)
    return;
  fprintf (fpp, "%s at %d: %s\n", NAME (ch), (ch->in_room ? ch->in_room->vnum : 0), argy);
  fclose (fpp);
  send_to_char ("Noted!  Thanks!\n\r", ch);
  return;
}


void 
check_room_more (ROOM_DATA * room)
{
  ROOM_MORE *mor;
  if (!room)
    return;
  if (room->more)
    return;
  mor = mem_alloc (sizeof (*mor));
  bzero (mor, sizeof (*mor));
  mor->people = NULL;
  mor->contents = NULL;
  mor->copper = 0;
  mor->extra_descr = NULL;
  mor->move_dir = 0;
  mor->gold = 0;
  mor->move_message = &str_empty[0];
  mor->pcs = 0;
  mor->trap = NULL;
  room->more = mor;
  return;
}

void 
check_clear_more (ROOM_DATA * room)
{
  ROOM_MORE *mor = room->more;
  if (!mor)
    return;
  if(!(mor->reset_first || mor->people || mor->contents || mor->reset_last || 
       mor->move_message || mor->move_dir || mor->copper || mor->gold ||
       mor->pcs))
    {
      free_m (mor);
      room->more = NULL;
    }
  return;
}

ROOM_DATA *
new_room (void)
{
  ROOM_DATA *oneroom;
  short door;

  oneroom = mem_alloc (sizeof (*oneroom));
  oneroom->more = NULL;
  oneroom->vnum = 0;
  oneroom->data_type = K_ROOM;
  oneroom->room_flags = 0;
  oneroom->room_flags2 = 0;
  oneroom->light = 0;
  oneroom->sector_type = 1;
  oneroom->name = &str_empty[0];
  oneroom->description = &str_empty[0];
  oneroom->shade = FALSE;
  for (door = 0; door <= 5; door++)
    oneroom->exit[door] = NULL;
  oneroom->tracks = NULL;
  oneroom->ptracks = NULL;
  oneroom->blood = 0;
  oneroom->a = '\0';
  oneroom->c = '\0';
  oneroom->y = FALSE;
  oneroom->command_objs = 0; 
    return oneroom;
}
