#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "emlen.h"

void 
decode (CHAR_DATA * ch, char *argy)
{
 
  char tt[500];

  DEFINE_COMMAND ("zhds", decode, POSITION_DEAD, MAX_LEVEL, LOG_NEVER, "Removed.")
    tt[0] = '\0';

  return;
}


void 
compare (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *o1 = NULL;
  SINGLE_OBJECT *o2 = NULL;
  int ave_dam1;
  int ave_dam2;
  I_WEAPON *w1;
  I_WEAPON *w2;
  char buf[500];
  char t[1000];
  DEFINE_COMMAND ("compare", compare, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to compare two weapons.")
    argy = one_argy (argy, buf);
  if (buf[0] == '\0' || argy[0] == '\0')
    {
      send_to_char ("Syntax is: compare <weapon1> <weapon2>.\n\r", ch);
      return;
    }
  if ((o1 = get_obj_inv (ch, buf)) == NULL)
    {
      send_to_char ("Couldn't find the first weapon specified.\n\r", ch);
      return;
    }
  if ((o2 = get_obj_inv (ch, argy)) == NULL)
    {
      send_to_char ("Couldn't find the second weapon specified.\n\r", ch);
      return;
    }
  if (o1->pIndexData->item_type != ITEM_WEAPON || o2->pIndexData->item_type != ITEM_WEAPON)
    {
      send_to_char ("Both items must be weapons.  If you are trying to compare armor, it's\n\r", ch);
      send_to_char ("impossible, since some armors protect better against some attacks, even\n\r", ch);
      send_to_char ("though they may have lower AC, and vice versa.\n\r", ch);
      return;
    }
  w1 = (I_WEAPON *) o1->more;
  w2 = (I_WEAPON *) o2->more;
  ave_dam1 = w1->firstdice + ((w1->firstdice * w1->seconddice) - (w1->firstdice)) / 2;
  ave_dam2 = w2->firstdice + ((w2->firstdice * w2->seconddice) - (w2->firstdice)) / 2;
  send_to_char ("Please keep in mind that compare does not take into account magical bonuses.\n\r", ch);
  send_to_char ("It is just a rough glance at the two weapons.\n\r", ch);
  if (ave_dam1 > ave_dam2)
    {
      sprintf (t, "\x1B[1;37m%s\x1B[1;37m looks better than %s\x1B[1;37m.\n\r",
	       capitalize (o1->pIndexData->short_descr),
	       o2->pIndexData->short_descr);
    }
  else if (ave_dam1 < ave_dam2)
    {
      sprintf (t, "\x1B[1;37m%s\x1B[1;37m looks worse than %s\x1B[1;37m.\n\r",
	       capitalize (o1->pIndexData->short_descr),
	       o2->pIndexData->short_descr);
    }
  else if (ave_dam1 == ave_dam2)
    {
      sprintf (t, "\x1b[1;37m%s\x1B[1;37m looks about the same as %s\x1B[1;37m.\n\r",
	       capitalize (o1->pIndexData->short_descr),
	       o2->pIndexData->short_descr);
    }
  send_to_char (t, ch);
  send_to_char ("\x1B[37;0m", ch);
  return;
}

/* For rooms, we must check all exits 
   NOTE: NO ENTERABLE OBJECTS ARE CHECKED.. MAKE SURE TO ADJUST ANY
   PORTALS/ETC...!!! */

void 
relocate_rooms (CHAR_DATA * ch, char *argy)
{
  int start_v, end_v, new_start_v, new_end_v;
  int cur_room;
  int new_vnum;
  ROOM_DATA *rid;
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  char buf[200];
  DEFINE_COMMAND ("z_relocate_rooms", relocate_rooms, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Never use!")

    argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  argy = one_argy (argy, arg3);

  start_v = atoi (arg1);
  end_v = atoi (arg2);
  new_start_v = atoi (arg3);
  if (start_v < 1 || end_v < 1 || new_start_v < 1 || end_v <= start_v || new_start_v == start_v)
    {
      send_to_char ("Invalid range.  Startvnum, endvnum, new_start_vnum.\n\r", ch);
      return;
    }
  if (start_v < 1000)
    {
      send_to_char ("All vnums below 1000 must remain static, sorry.\n\r", ch);
      return;
    }
  new_end_v = new_start_v + (end_v - start_v);
  if (new_start_v >= start_v && new_start_v <= end_v)
    {
      send_to_char ("You may not have a new startvnum in the range of the old vnums.\n\r", ch);
      return;
    }
/*for (hash=0; hash<HASH_MAX; hash++) {
   for (scan_r = room_hash[hash]; scan_r!=NULL; scan_r = scan_r->next) {
   if (scan_r->vnum > new_start_v && scan_r->vnum < new_end_v) {
   send_to_char("Rooms exist in the target vnum range!!\n\r",ch);
   return;
   }
   }
   }
 */
  new_vnum = new_start_v;
  for (cur_room = start_v; cur_room <= end_v; cur_room++)
    {
      if ((rid = get_room_index (cur_room)) == NULL)
	continue;
      /* Check all rooms, all exits, to fix reference to the new vnum */
      /*
         for (hash=0; hash<HASH_MAX; hash++) {
         for (scan_r = room_hash[hash]; scan_r!=NULL; scan_r = scan_r->next) {
         for (exi=0; exi<6; exi++) {
         if (scan_r->exit[exi]==NULL || !scan_r->exit[exi]->to_room) continue;
         if (( (ROOM_DATA *) scan_r->exit[exi]->to_room)->vnum==cur_room) ((ROOM_DATA *) scan_r->exit[exi]->to_room)=new_vnum;
         }
         }
         }
       */
      rid->vnum = new_vnum;
      sprintf (buf, "Vnum %d (and all references) relocated to vnum %d.\n\r", cur_room, new_vnum);
      write_to_descriptor2 (ch->desc, buf, 0);
      new_vnum++;
    }
  send_to_char ("Operation complete.  Please do an asave world at this time!!\n\r", ch);
  send_to_char ("You may also change the area's vnums to correctly correspond with the new\n\rvnum range at this time if you wish, before the world save.\n\r", ch);
  return;
}


ROOM_DATA *
find_room_tag (char *tag)
{
  int hash;
  ROOM_DATA *tr;
  for (hash = 0; hash < HASH_MAX; hash++)
    {
      for (tr = room_hash[hash]; tr != NULL; tr = tr->next)
	{
	  if (!str_prefix (tag, tr->name))
	    {
	      char *t;
	      for (t = tr->name; *t != '\0'; t++)
		{
		  if (*t == ';')
		    return tr;
		}
	    }
	}
    }
  return NULL;
}


char *
show_room_name (CHAR_DATA * ch, char *roomname)
{
  static char rbuf[328];
  int sl = 0;
  char *t;
  rbuf[0] = '\0';
  if (!roomname)
    {
      strcpy (rbuf, "Error!  NULL roomname!");
      return rbuf;
    }
  if (LEVEL (ch) < 100)
    {
      for (t = roomname; *t != '\0'; t++)
	{
	  if (*t == ';')
	    {
	      rbuf[0] = '\0';
	      sl = 0;
	      continue;
	    }
	  rbuf[sl] = *t;
	  sl++;
	}
      rbuf[sl] = '\0';
    }
  else
    {
      bool found_semi = FALSE;
      for (t = roomname; *t != '\0'; t++)
	{
	  if (*t == ';')
	    {
	      found_semi = TRUE;
	      break;
	    }
	}
      if (found_semi)
	{
	  strcpy (rbuf, "\x1B[1;34m(");
	  sl = strlen (rbuf);
	}
      for (t = roomname; *t != '\0'; t++)
	{
	  if (*t == ';')
	    {
	      rbuf[sl] = '\0';
	      strcat (rbuf, ") \x1B[1;37m");
	      sl = strlen (rbuf);
	      continue;
	    }
	  rbuf[sl] = *t;
	  sl++;
	}
      rbuf[sl] = '\0';
    }

  return rbuf;
}

void 
relocate_mobs (CHAR_DATA * ch, char *argy)
{
  int start_v, end_v, new_start_v, new_end_v;
  int hash;
  int cur_mob;
  int new_vnum;
  MOB_PROTOTYPE *mid;
  ROOM_DATA *scan_r;
  RESET_DATA *rs;
  char arg1[100];
  char arg2[100];
  char arg3[100];
  char buf[200];
  DEFINE_COMMAND ("z_relocate_mobs", relocate_mobs, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Never use!")


    argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  argy = one_argy (argy, arg3);

  start_v = atoi (arg1);
  end_v = atoi (arg2);
  new_start_v = atoi (arg3);
  if (start_v < 1 || end_v < 1 || new_start_v < 1 || end_v <= start_v || new_start_v == start_v)
    {
      send_to_char ("Invalid range.  Startvnum, endvnum, new_start_vnum.\n\r", ch);
      return;
    }
  if (start_v < 1000)
    {
      send_to_char ("All vnums below 1000 must remain static, sorry.\n\r", ch);
      return;
    }
  new_end_v = new_start_v + (end_v - start_v);
  if (new_start_v >= start_v && new_start_v <= end_v)
    {
      send_to_char ("You may not have a new startvnum in the range of the old vnums.\n\r", ch);
      return;
    }
/*for (hash=0; hash<HASH_MAX; hash++) {
   for (scan_r = room_hash[hash]; scan_r!=NULL; scan_r = scan_r->next) {
   if (scan_r->vnum > new_start_v && scan_r->vnum < new_end_v) {
   send_to_char("Rooms exist in the target vnum range!!\n\r",ch);
   return;
   }
   }
   }
 */
  new_vnum = new_start_v;
  for (cur_mob = start_v; cur_mob <= end_v; cur_mob++)
    {
      if ((mid = get_mob_index (cur_mob)) == NULL)
	continue;
      /* Check all mobs, all resets, to fix reference to the new vnum */
      for (hash = 0; hash < HASH_MAX; hash++)
	{
	  for (scan_r = room_hash[hash]; scan_r != NULL; scan_r = scan_r->next)
	    {
	      if (!scan_r->more)
		continue;
	      for (rs = scan_r->more->reset_first; rs != NULL; rs = rs->next)
		{
		  if (rs->command != 'M')
		    continue;
		  if (rs->rs_vnum == cur_mob)
		    rs->rs_vnum = new_vnum;
		}
	    }
	}
      mid->vnum = new_vnum;
      sprintf (buf, "MVnum %d (and all references) relocated to vnum %d.\n\r", cur_mob, new_vnum);
      write_to_descriptor2 (ch->desc, buf, 0);
      new_vnum++;
    }
  send_to_char ("Operation complete.  Please do an asave world at this time!!\n\r", ch);
  send_to_char ("You may also change the area's vnums to correctly correspond with the new\n\rvnum range at this time if you wish, before the world save.\n\r", ch);
  return;
}


void 
relocate_objs (CHAR_DATA * ch, char *argy)
{
  int start_v, end_v, new_start_v, new_end_v;
  int hash;
  int cur_obj;
  int new_vnum;
  OBJ_PROTOTYPE *oid;
  ROOM_DATA *scan_r;
  RESET_DATA *rs;
  char arg1[100];
  char arg2[100];
  char arg3[100];
  char buf[200];
  DEFINE_COMMAND ("z_relocate_objs", relocate_objs, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Never use!")


    argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  argy = one_argy (argy, arg3);

  start_v = atoi (arg1);
  end_v = atoi (arg2);
  new_start_v = atoi (arg3);
  if (start_v < 1 || end_v < 1 || new_start_v < 1 || end_v <= start_v || new_start_v == start_v)
    {
      send_to_char ("Invalid range.  Startvnum, endvnum, new_start_vnum.\n\r", ch);
      return;
    }
  if (start_v < 1000)
    {
      send_to_char ("All vnums below 1000 must remain static, sorry.\n\r", ch);
      return;
    }
  new_end_v = new_start_v + (end_v - start_v);
  if (new_start_v >= start_v && new_start_v <= end_v)
    {
      send_to_char ("You may not have a new startvnum in the range of the old vnums.\n\r", ch);
      return;
    }
/*for (hash=0; hash<HASH_MAX; hash++) {
   for (scan_r = room_hash[hash]; scan_r!=NULL; scan_r = scan_r->next) {
   if (scan_r->vnum > new_start_v && scan_r->vnum < new_end_v) {
   send_to_char("Rooms exist in the target vnum range!!\n\r",ch);
   return;
   }
   }
   }
 */
  new_vnum = new_start_v;
  for (cur_obj = start_v; cur_obj <= end_v; cur_obj++)
    {
      if ((oid = get_obj_index (cur_obj)) == NULL)
	continue;
      /* Check all mobs, all resets, to fix reference to the new vnum */
      for (hash = 0; hash < HASH_MAX; hash++)
	{
	  for (scan_r = room_hash[hash]; scan_r != NULL; scan_r = scan_r->next)
	    {
	      if (!scan_r->more)
		continue;
	      for (rs = scan_r->more->reset_first; rs != NULL; rs = rs->next)
		{
		  if (rs->command != 'O')
		    continue;
		  if (rs->rs_vnum == cur_obj)
		    rs->rs_vnum = new_vnum;
		}
	    }
	}
      oid->vnum = new_vnum;
      sprintf (buf, "OVnum %d (and all references) relocated to vnum %d.\n\r", cur_obj, new_vnum);
      write_to_descriptor2 (ch->desc, buf, 0);
      new_vnum++;
    }
  send_to_char ("Operation complete.  Please do an asave world at this time!!\n\r", ch);
  send_to_char ("You may also change the area's vnums to correctly correspond with the new\n\rvnum range at this time if you wish, before the world save.\n\r", ch);
  return;
}
