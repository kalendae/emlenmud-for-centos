#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
static char bah[500];
extern char *const dir_rev[];

void 
do_drag (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char buf[500];
  SINGLE_OBJECT *obj;
  CHAR_DATA *rch;
  char arg2[SML_LENGTH];
  int dir;
  ROOM_DATA *oldroom;
  DEFINE_COMMAND ("drag", do_drag, POSITION_STANDING, 0, LOG_NORMAL, "This command allows you to drag a corpse.")

    arg1[0] = '\0';
  arg2[0] = '\0';
  if (strlen (argy) >= 50)
    return;
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  if (arg1[0] == '\0')
    {
      send_to_char ("Drag what?\n\r", ch);
      return;
    }
  if (arg2[0] == '\0')
    {
      send_to_char ("In which direction?\n\r", ch);
      return;
    }
  if ((obj = get_obj_here (ch, arg1, SEARCH_ROOM_FIRST)) == NULL || obj->carried_by != NULL || obj->in_room == NULL)
    {
      send_to_char ("I don't see that object here....\n\r", ch);
      return;
    }
  if (obj->pIndexData->item_type != ITEM_CORPSE_PC && obj->pIndexData->item_type != ITEM_CORPSE_NPC &&
      !CAN_WEAR (obj, ITEM_TAKE))
    {
      send_to_char ("You can't drag that object...\n\r", ch);
      return;
    }
  if (ch->move < 10)
    {
      send_to_char ("You are too exhausted to drag anything...\n\r", ch);
      return;
    }
  dir = -1;
  if (arg2[0] == 'n' || arg2[0] == 'N')
    dir = DIR_NORTH;
  if (arg2[0] == 'w' || arg2[0] == 'W')
    dir = DIR_WEST;
  if (arg2[0] == 'e' || arg2[0] == 'E')
    dir = DIR_EAST;
  if (arg2[0] == 's' || arg2[0] == 'S')
    dir = DIR_SOUTH;
  if (arg2[0] == 'u' || arg2[0] == 'U')
    dir = DIR_UP;
  if (arg2[0] == 'd' || arg2[0] == 'D')
    dir = DIR_DOWN;
  if (dir == -1)
    {
      send_to_char ("Which direction?\n\r", ch);
      return;
    }
  oldroom = ch->in_room;
  if (!move_char (ch, dir))
    {
      send_to_char ("How can you drag something to someplace you can't even go!\n\r", ch);
      return;
    }
  sprintf (buf, "%s drags %s %s.\n\r", NAME (ch),
       (obj->short_descr ? obj->short_descr : obj->pIndexData->short_descr),
	   dir_name[dir]);
  if (oldroom->more)
    for (rch = oldroom->more->people; rch != NULL; rch = rch->next_in_room)
      {
	if (rch == ch)
	  continue;
	if (!IS_AWAKE (rch))
	  continue;
	send_to_char (buf, rch);
      }
  obj_from (obj);
  obj_to (obj, ch->in_room);
  sprintf (buf, "You drag %s into the room.\n\r",
      (obj->short_descr ? obj->short_descr : obj->pIndexData->short_descr));
  send_to_char (buf, ch);
  sprintf (buf, "%s drags %s in from %s.\n\r", NAME (ch),
       (obj->short_descr ? obj->short_descr : obj->pIndexData->short_descr),
	   dir_rev[rev_dir[dir]]);
  for (rch = ch->in_room->more->people; rch != NULL; rch = rch->next_in_room)
    {
      if (rch == ch)
	continue;
      if (!IS_AWAKE (rch))
	continue;
      send_to_char (buf, rch);
    }
  SUBMOVE(ch, number_range (8, 10));
  return;
}

void 
do_meditate (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("meditate", do_meditate, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to meditate, and focus on magical energy regeneration.")

    if (RIDING (ch) != NULL)
    {
      send_to_char ("If you wanted to sit and meditate, you would not be riding!\n\r", ch);
      return;
    }
  if (ch->fgt && ch->fgt->ears > 9)
    {
      send_to_char ("You are bashed; you can't meditate...\n\r", ch);
      return;
    }
if (FIGHTING(ch)) 
  {
     if (FIGHTING(ch)->in_room != ch->in_room)
       {
          ch->fgt->fighting = NULL;
          NEW_POSITION(ch, POSITION_STANDING);
       }
      else
       {
         act ("You must start meditation from a standing or resting position.", ch, NULL, NULL, TO_CHAR);
	 return;
	}
   }
  if (ch->position != POSITION_STANDING && ch->position != POSITION_RESTING && ch->position != POSITION_MEDITATING)
    {
      act ("You must start meditation from a standing or resting position.", ch, NULL, NULL, TO_CHAR);
      return;
    }
  if (ch->position == POSITION_MEDITATING)
    {
      NEW_POSITION(ch, POSITION_STANDING);
      act ("You stop meditating, and clamber to your feet.", ch, NULL, NULL, TO_CHAR);
      act ("$N stops meditating, and clambers to $s feet.", ch, NULL, ch, TO_ROOM);
      return;
    }
  NEW_POSITION(ch, POSITION_MEDITATING);
  act ("You sit down cross-legged and begin meditating.", ch, NULL, NULL, TO_CHAR);
  act ("$N sits down, clears $s mind, and begins meditating.", ch, NULL, ch, TO_ROOM);
  return;
}

void 
get_obj (CHAR_DATA * ch, SINGLE_OBJECT * obj, SINGLE_OBJECT * container, bool opp_align)
{
  CHAR_DATA *fch;
  CHAR_DATA *fch_next;
  if (!CAN_WEAR (obj, ITEM_TAKE))
    {
      send_to_char ("You can't take that.\n\r", ch);
      return;
    }
  for (fch = ch->in_room->more->people; fch != NULL; fch = fch_next)
    {
      fch_next = fch->next_in_room;
      if (IS_MOB (fch) && IS_SET (fch->pIndexData->act3, ACT3_GUARD_ITEM)
	  && obj->pIndexData->vnum == fch->pIndexData->guard)
	{
	  do_say (fch, "Don't touch that!");
	  multi_hit (fch, ch, TYPE_UNDEFINED);
	}
    }
  if (!opp_align)
    {
      if (container != NULL)
        {
	  act ("You get $p\x1B[37;0m from $P\x1B[0m.", ch, obj, container, TO_CHAR);
	  act ("$n gets $p\x1B[37;0m from $P\x1B[0m.", ch, obj, container, TO_ROOM);
        }
      else
        {
          act ("You get $p\x1B[37;0m.", ch, obj, container, TO_CHAR);
          act ("$n gets $p\x1B[37;0m.", ch, obj, container, TO_ROOM);
        }
    }
  if (obj->pIndexData->vnum > 90000 &&
   (obj->pIndexData->item_type == ITEM_FURNITURE ||
    obj->pIndexData->item_type == ITEM_TOOL))
  {
    fprintf (stderr, "FORGECRAP %s gotten by %s at %d.\n",
    OOSTR(obj, short_descr), NAME(ch), ch->in_room->vnum);
    }

  obj_from (obj);
  obj->wear_loc = -1;
  obj_to (obj, ch);
  if (obj->pIndexData->item_type == ITEM_VEHICLE && IS_PLAYER(ch))
    {
      I_VEHICLE *veh = (I_VEHICLE *) obj->more;
      if (veh->sector_use == SECT_WATER_SWIM || 
	  veh->sector_use == SECT_WATER_NOSWIM)
	SET_BIT(ch->act, PLR_HAS_BOAT);
    }
  return;
}

void 
do_take (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("take", do_get, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to get an object from an object, or from the ground.")
    return;
}

void 
do_get (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char buffy[SML_LENGTH];
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj_next;
  SINGLE_OBJECT *container;
  CHAR_DATA *fch;
  I_CONTAINER *cnt;
  bool found = FALSE;
  bool gonnasave = FALSE;
  bool opp_align = FALSE;
  DEFINE_COMMAND ("get", do_get, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to get an object from an object, or from the ground.")

    check_room_more (ch->in_room);
  
  if(IS_PLAYER(ch))
    {
      for(fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
	{
	  if (IS_PLAYER(fch) && DIFF_ALIGN(ch, fch))
	    {
	      opp_align = TRUE;
	      break;
	    }
	}
    }

  if (!str_cmp (argy, "all corpse"))
    {
      argy = one_argy (argy, arg1);
      argy = one_argy (argy, arg2);
    }
  else
    {
      argy = one_argy (argy, arg1);
      while (str_prefix ("from", argy) && argy != "" && argy[0] != '\0')
	argy = one_argy (argy, arg1);
      argy = one_argy (argy, arg2);
      while (argy != "" && argy[0] != '\0')
	argy = one_argy (argy, arg2);
    }

  if (arg1[0] == '\0')
    {
      send_to_char ("Get what?\n\r", ch);
      return;
    }
  if (arg2[0] == '\0')
    {
      if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
	{
	  if (!str_cmp (arg1, "coins") || !str_cmp (arg1, "coin"))
	    {
	      found = FALSE;
	      if (ch->in_room->more->gold > 0)
		{
		  found = TRUE;
		  sprintf(bah, "You pick up \x1b[1;33m%d\x1b[0;37m gold.\n\r", ch->in_room->more->gold);
		  send_to_char(bah, ch);
		  ch->gold += ch->in_room->more->gold;
		  ch->in_room->more->gold = 0;
		}
	      if (ch->in_room->more->copper > 0)
		{
		  found = TRUE;
		  sprintf(bah, "You pick up \x1b[33m%d\x1b[0;37m copper.\n\r",
ch->in_room->more->copper);
		  send_to_char(bah, ch);
		  ch->copper += ch->in_room->more->copper;
		  ch->in_room->more->copper = 0;
		}
	      if (!found)
		send_to_char("There are no coins here to pick up.\n\r", ch);
	      return;
	    }
	  obj = get_obj_list (ch, arg1, ch->in_room->more->contents);
	  if (obj == NULL || IS_SET (obj->extra_flags, ITEM_UNSEEN))
	    {
	      act ("It isn't here.", ch, NULL, NULL, TO_CHAR);
	      return;
	    }
	  get_obj (ch, obj, NULL, opp_align);
          if (opp_align)
            {
	      act("You pick up $p.", ch, obj,NULL , TO_CHAR);
	      act("$n picks up $p.", ch, obj, NULL, TO_ROOM);
            }
	}
      else
	{
	  found = FALSE;
	  if (ch->in_room->more->gold > 0)
	    {
	      found = TRUE;
	      sprintf(bah, "You pick up \x1b[1;33m%d\x1b[0;37m gold.\n\r", ch->in_room->more->gold);
	      send_to_char(bah, ch);
	      ch->gold += ch->in_room->more->gold;
	      ch->in_room->more->gold = 0;
	    }
	  if (ch->in_room->more->copper > 0)
	    {
	      found = TRUE;
	      sprintf(bah, "You pick up \x1b[33m%d\x1b[0;37m copper.\n\r",
ch->in_room->more->copper);
	      send_to_char(bah, ch);
	      ch->copper += ch->in_room->more->copper;
	      ch->in_room->more->copper = 0;
	    }
	  for (obj = ch->in_room->more->contents; obj != NULL; obj = obj_next)
	    {
	      obj_next = obj->next_content;
	      if ((arg1[3] == '\0' || is_name (&arg1[4], obj->pIndexData->name))
		  && can_see_obj (ch, obj) && !IS_SET (obj->extra_flags, ITEM_UNSEEN))
		{
		  found = TRUE;
		  get_obj (ch, obj, NULL, opp_align);
		}
	    }
	  if (!found)
	    {
	      if (arg1[3] == '\0')
		send_to_char ("I see nothing here.\n\r", ch);
	      else
		act ("I don't see that here.", ch, NULL, NULL, TO_CHAR);
	    }
          else if (opp_align)
            {
	      act("You pick up several items.", ch, NULL,NULL , TO_CHAR);
	      act("$n gathers up several things off the ground.", ch, NULL, NULL, TO_ROOM);
            }

	}
    }
  else
    {
      if (!str_cmp (arg2, "all") || !str_prefix ("all.", arg2))
	{
	  send_to_char ("You can't do that.\n\r", ch);
	  return;
	}
      if ((container = (!str_prefix ("cor", arg2) ? get_obj_here (ch, arg2, SEARCH_ROOM_FIRST) : get_obj_here (ch, arg2, SEARCH_INV_FIRST))) == NULL)
	{
	  send_to_char("That does not appear to be here.\n\r", ch);
	  return;
	}
      switch (container->pIndexData->item_type)
	{
	default:
	  send_to_char ("You can't look inside that...\n\r", ch);
	  return;
	case ITEM_CONTAINER:
	case ITEM_CORPSE_NPC:
	  {
	    cnt = (I_CONTAINER *) container->more;
	    if ((cnt->money > 0 || container->contains) && strlen (NAME (ch)) < 30)
	      {
		if (cnt->looted_by[0] == '\0')
		  {
		    strcpy (cnt->looted_by, NAME (ch));
		  }
		else if (cnt->looted_by_two[0] == '\0' && str_cmp (NAME (ch), cnt->looted_by))
		  {
		    strcpy (cnt->looted_by_two, NAME (ch));
		  }
	      }
	  }
	  break;
	case ITEM_CORPSE_PC:
	  {
	    char name[SML_LENGTH];
	    char *pd;
	    if (!pow.get_from_corpse_while_in_combat && FIGHTING (ch))
	      {
		send_to_char ("You don't have time to loot the corpse while you are fighting!\n\r", ch);
		return;
	      }
	    cnt = (I_CONTAINER *) container->more;
	    pd = OOSTR (container, short_descr);
	    pd = one_argy (pd, name);
	    pd = one_argy (pd, name);
	    pd = one_argy (pd, name);
	    pd = one_argy (pd, name);
	    /*name now equals the name of the person */
	    if (IS_PLAYER(ch) && LEVEL(ch)<=pow.loot_level && str_cmp(name,NAME(ch))) 
	      {
		send_to_char("You cannot loot another person's corpse at that low level.\n\r",ch);
		return;
	      }
	    if ((container->contains || cnt->money > 0) && str_cmp (NAME (ch), capitalize (name)))
	      {
		if (strlen (NAME (ch)) < 30)
		  {
		    if (cnt->looted_by[0] == '\0')
		      {
			strcpy (cnt->looted_by, NAME (ch));
		      }
		    else if (cnt->looted_by_two[0] == '\0' && str_cmp (NAME (ch), cnt->looted_by))
		      {
			strcpy (cnt->looted_by_two, NAME (ch));
		      }
		  }
		fprintf (stderr, "%s just took something from %s's corpse.\n",
			 NAME (ch), capitalize (name));
	      }
	    if (container->contains || cnt->money > 0)
	      {
		sprintf (buffy, "%s%s.cor", PLAYER_DIR_2, capitalize (name));
		unlink (buffy);
	      }
	    gonnasave = TRUE;
	    if (IS_PLAYER (ch) && str_cmp (NAME (ch), capitalize (name)))
	      {
               if (ch->pcdata->no_quit_pk < 4);
		ch->pcdata->no_quit_pk = 4;/*JRAJRA*/
	      } /* JRAJRA -noquit stuff */
	  }
	}
      cnt = (I_CONTAINER *) container->more;
      if (IS_SET (cnt->flags, CONT_CLOSED))
	{
	  act ("The $d is closed.", ch, NULL, container->pIndexData->name, TO_CHAR);
	  return;
	}
      if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
	{
	  cnt = (I_CONTAINER *) container->more;
	  if (!str_cmp (arg1, "coins"))
	    {
	      if (cnt->money < 1)
		{
		  send_to_char ("I see no coins in there.\n\r", ch);
		  return;
		}
	      sprintf (bah, "You get %d copper from %s\x1B[0;37m.\n\r", cnt->money, OOSTR (container, short_descr));
	      ch->copper += cnt->money;
	      cnt->money = 0;
	      send_to_char (bah, ch);
	      return;
	    }
	  obj = get_obj_list (ch, arg1, container->contains);
	  if (obj == NULL)
	    {
	      act ("There is nothing like that in the $T\x1B[0m.", ch, NULL, arg2, TO_CHAR);
	      return;
	    }
	  get_obj (ch, obj, container, opp_align);
          if (opp_align)
            {
	      act("You get $p from $P.", ch, obj, container, TO_CHAR);
	      act("$n reaches in and gets $p out of $P.", ch, obj, container, TO_ROOM);
            }
	}
      else
	{
	  cnt = (I_CONTAINER *) container->more;
	  found = FALSE;
	  if (cnt->money > 0)
	    {
	      sprintf (bah, "You get %s from %s.\x1B[0;37m\n\r", name_amount (cnt->money), OOSTR (container, short_descr));
	      ch->copper += cnt->money;
	      cnt->money = 0;
	      send_to_char (bah, ch);
	      found = TRUE;
	    }
	  for (obj = container->contains; obj != NULL; obj = obj_next)
	    {
	      obj_next = obj->next_content;
	      if ((arg1[3] == '\0' || is_name (&arg1[4], obj->pIndexData->name))
		  && can_see_obj (ch, obj))
		{
		  found = TRUE;
		  get_obj (ch, obj, container, opp_align);
		}
	    }
	  if (!found)
	    {
	      if (arg1[3] == '\0')
		act ("I see nothing in $T\x1B[0m.", ch, NULL, arg2, TO_CHAR);
	      else
		act ("I see nothing like that in $T\x1B[0m.", ch, NULL, arg2, TO_CHAR);
  	    }
          else 
	    {
	      if (opp_align)
		{
		  if (arg1[3] == '\0')
		    {
		      act ("You get some things from $P\x1B[0m.", ch, NULL,container,TO_CHAR);
		      act ("$n gets some things from $P\x1b[0;37m.", ch, NULL,container,TO_ROOM); 
		    }
		  else
		    {
		      act ("You get some things from $P\x1B[0m.", ch,NULL,container,TO_CHAR);
		      act ("$n gets some stuff from $P\x1b[0;37m.", ch, NULL,container, TO_ROOM);
		    }
		}
	    }
	}
    }
  if (IS_PLAYER (ch))
    update_weight_num (ch);
  check_clear_more (ch->in_room);
  return;
}

void 
do_put (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  SINGLE_OBJECT *container;
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj_next;
  I_CONTAINER *cnt;
  DEFINE_COMMAND ("put", do_put, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to put an object in something.")

    argy = one_argy (argy, arg1);
  while (str_prefix ("in", argy) && argy != "" && argy[0] != '\0')
    argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  while (argy != "" && argy[0] != '\0')
    argy = one_argy (argy, arg2);

  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
      send_to_char ("Put what in what?\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "all.coin") || !str_cmp (arg1, "coin") || !str_cmp (arg1, "all.coins") || !str_cmp (arg1, "coins"))
    {
      char buffu[500];
      if ((container = get_obj_here (ch, arg2, SEARCH_INV_FIRST)) == NULL)
	{
	  act ("I see no $T here.", ch, NULL, arg2, TO_CHAR);
	  return;
	}
      if (container->pIndexData->item_type != ITEM_CONTAINER)
	{
	  send_to_char ("You can't put anything inside of that.\n\r", ch);
	  return;
	}
      cnt = (I_CONTAINER *) container->more;
      cnt->money += ch->copper;
      sprintf (buffu, "You put your copper in %s.  (Gold pieces can't go inside containers).\n\r", OOSTR (container, short_descr));
      act (buffu, ch, NULL, ch, TO_CHAR);
      ch->copper = 0;
      return;
    }
  if (!str_cmp (arg2, "all") || !str_prefix ("all.", arg2))
    {
      send_to_char ("You can't do that.\n\r", ch);
      return;
    }
  if ((container = get_obj_here (ch, arg2, SEARCH_INV_FIRST)) == NULL)
    {
      act ("I see no $T here.", ch, NULL, arg2, TO_CHAR);
      return;
    }
  if (container->pIndexData->item_type != ITEM_CONTAINER)
    {
      send_to_char ("That's not a container.\n\r", ch);
      return;
    }
 

 cnt = (I_CONTAINER *) container->more;
  if (IS_SET (cnt->flags, CONT_CLOSED))
    {
      act ("The $d is closed.", ch, NULL, container->pIndexData->name, TO_CHAR);
      return;
    }
  if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
    {
      if ((obj = get_obj_inv (ch, arg1)) == NULL)
	{
	  send_to_char ("You do not have that item.\n\r", ch);
	  return;
	}
  if (obj->wear_loc!=WEAR_NONE && IS_SET (obj->extra_flags, ITEM_NOREMOVE)) {
	send_to_char("You can't remove that item!\n\r",ch);
	return;
	}
      if (obj == container)
	{
	  send_to_char ("Now that would be a trick, eh?\n\r", ch);
	  return;
	}
      if (!can_drop_obj (ch, obj))
	{
	  send_to_char ("You can't let go of it.\n\r", ch);
	  return;
	}
      if ((cnt->max_weight + container->pIndexData->weight)
	  < (get_obj_weight (obj) + get_obj_weight (container)))
	{
	  send_to_char ("It won't fit.\n\r", ch);
	  return;
	}
      obj_from (obj);
      obj_to (obj, container);
      if (obj->pIndexData->item_type == ITEM_VEHICLE && IS_PLAYER(ch))
	{
	  I_VEHICLE *veh = (I_VEHICLE *) obj->more;
	  if (veh->sector_use == SECT_WATER_SWIM || 
	      veh->sector_use == SECT_WATER_NOSWIM)
	    REMOVE_BIT(ch->act, PLR_HAS_BOAT);
	}
      act ("$n puts $p\x1B[0m in $P\x1B[0m.", ch, obj, container, TO_ROOM);
      act ("You put $p\x1B[0m in $P\x1B[0m.", ch, obj, container, TO_CHAR);
    }
  else
    {
      cnt = (I_CONTAINER *) container->more;
      for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
	  obj_next = obj->next_content;
	  if ((arg1[3] == '\0' || is_name (&arg1[4], obj->pIndexData->name))
	      && can_see_obj (ch, obj)
	      && obj->wear_loc == WEAR_NONE
	      && obj != container
	      && can_drop_obj (ch, obj)
	      && get_obj_weight (obj) + get_obj_weight (container)
	      <= cnt->max_weight)
	    {
	      obj_from (obj);
	      obj_to (obj, container);
	      if (obj->pIndexData->item_type == ITEM_VEHICLE && IS_PLAYER(ch))
		{
		  I_VEHICLE *veh = (I_VEHICLE *) obj->more;
		  if (veh->sector_use == SECT_WATER_SWIM || 
		      veh->sector_use == SECT_WATER_NOSWIM)
		    REMOVE_BIT(ch->act, PLR_HAS_BOAT);
		}
	      act ("$n puts $p\x1B[0m in $P\x1B[0m.", ch, obj, container, TO_ROOM);
	      act ("You put $p\x1B[0m in $P\x1B[0m.", ch, obj, container, TO_CHAR);
	    }
	}
    }
  return;
}

void 
do_drop (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  char buf[SML_LENGTH];
  CHAR_DATA *fch;
  bool opp_align = FALSE;
  int amount;
  SINGLE_OBJECT *obj_next;
  bool found = FALSE;
  char arg2[SML_LENGTH];
  DEFINE_COMMAND ("drop", do_drop, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to drop an object.")

    argy = one_argy (argy, arg);
  if (!is_number (arg))
    {
      while (argy != "" && argy[0] != '\0')
	argy = one_argy (argy, arg);
    }
  argy = one_argy (argy, arg2);
  if (!ch->in_room || !ch->in_room->more) return;
  if (IS_PLAYER(ch) && ch->pcdata->no_quit_pk >= 6)
    {
      bool opp_align_in_room = FALSE;
      CHAR_DATA *rch;
      for (rch = ch->in_room->more->people; rch != NULL; rch =
rch->next_in_room)
         {
           if (IS_PLAYER(rch) && DIFF_ALIGN(ch, rch))
             {
               opp_align_in_room = TRUE;
               break;
             }
         }
/*       if (!opp_align_in_room)
        {
          send_to_char("You cannot drop things now! Your heart is racing too much!\n\r", ch);
          return;
        } */
    }
  CAN_DROP_ITEMS_IN_BGROUND (no)
    CAN_DROP_ITEMS_IN_ARENA (no)
  if (IS_PLAYER(ch))
  for (fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
    {
      if (IS_PLAYER(fch) && DIFF_ALIGN(ch, fch))
        { 
           opp_align = TRUE;
           break;
        } 
    }



    if (arg[0] == '\0')
    {
      send_to_char ("Drop what?\n\r", ch);
      return;
    }
  if (!str_cmp (arg, "coin") || !str_cmp (arg, "coins") || !str_cmp (arg, "all.coin") || !str_cmp (arg, "all.coins"))
    {
      char bufff[500];
      if (tally_coins (ch) < 0)
	{
	  send_to_char ("You have no coins in your inventory!\n\r", ch);
	  return;
	}
      if (LEVEL (ch) < 100 || IS_REAL_GOD (ch))
	{
	  ch->in_room->more->copper += ch->copper;
	  ch->in_room->more->gold += ch->gold;
	}
      sprintf (bufff, "You drop your coins.\n\r");
      act("$n drops some coins.", ch, NULL,NULL, TO_ROOM);
      send_to_char (bufff, ch);
      ch->gold = 0;
      ch->copper = 0;
      return;
    }
  if (IS_PLAYER (ch))
    {
      update_weight_num (ch);
      /*save_char_obj(ch); */
    }

  if ((!str_cmp (arg2, "gold"))
      && (is_number (arg) && atoi (arg) > 0))
    {
      amount = atoi (arg);
      if (amount < 1)
	return;
      if (ch->gold < amount)
	{
	  send_to_char ("You don't have that many \x1B[1;33mgold\x1B[37;0m coins.\n\r", ch);
	  return;
	}
      ch->gold -= amount;
      if (ch->in_room->sector_type != SECT_WATER_SWIM && ch->in_room->sector_type != SECT_WATER_NOSWIM &&
	  ((LEVEL (ch) < 100) || IS_REAL_GOD (ch)))
	ch->in_room->more->gold += amount;
      sprintf (buf, "You drop %d \x1B[1;33mgold\x1B[37;0m coins.\n\r", amount);
      send_to_char (buf, ch);
      act ("$n drops some \x1B[1;33mgold\x1B[37;0m coins.", ch, NULL, NULL, TO_ROOM);
      return;
    }

  if ((!str_cmp (arg2, "copper"))
      && (is_number (arg) && atoi (arg) > 0))
    {
      amount = atoi (arg);
      if (amount < 1)
	return;
      if (ch->copper < amount)
	{
	  send_to_char ("You don't have that many \x1B[33mcopper\x1B[37;0m coins.\n\r", ch);
	  return;
	}
      ch->copper -= amount;
      if (ch->in_room->sector_type != SECT_WATER_SWIM && ch->in_room->sector_type != SECT_WATER_NOSWIM &&
	  ((LEVEL (ch) < 100) || IS_REAL_GOD (ch)))
	ch->in_room->more->copper += amount;
      sprintf (buf, "You drop %d \x1B[33mcopper\x1B[37;0m coins.\n\r", amount);
      send_to_char (buf, ch);
      act ("$n drops some \x1B[33mcopper\x1B[37;0m coins.", ch, NULL, NULL, TO_ROOM);
      return;
    }
  if ((str_cmp (arg, "all") && str_prefix ("all.", arg)) || (ch->fgt && ch->fgt->combat_delay_count == 9))
    {
      if ((obj = get_obj_inv (ch, arg)) == NULL)
	{
	  send_to_char ("You do not have that item.\n\r", ch);
	  return;
	}
      else if ((!NOT_WORN (obj))	/*|| (obj->wear_loc==WEAR_HOLD_1) ||
					   (obj->wear_loc==WEAR_HOLD_2) */ )
	{
	  send_to_char ("You will have to remove it first.\n\r", ch);
	  return;
	}
      if (!can_drop_obj (ch, obj))
	{
	  send_to_char ("You can't let go of it.\n\r", ch);
	  return;
	}

      if (obj->wear_loc == -1 && !IS_SET (obj->extra_flags, ITEM_UNSEEN) &&
	  (ch->fgt && ch->fgt->combat_delay_count == 9))
	{
	  act ("You hide $p\x1B[0m in the room.", ch, obj, NULL, TO_CHAR);
	  ch->fgt->combat_delay_count = 0;
	  obj->extra_flags ^= ITEM_UNSEEN;
	  obj_from (obj);
	  if (ch->in_room->sector_type == SECT_WATER_SWIM || ch->in_room->sector_type == SECT_WATER_NOSWIM)
	    {
	      act ("The waters cause $p to sink beneath them; lost forever.", ch, obj, NULL, TO_CHAR);
	      free_it (obj);
	      return;
	    }
	  obj_to (obj, ch->in_room);
	  if (obj->pIndexData->item_type == ITEM_VEHICLE && IS_PLAYER(ch))
	    {
	      I_VEHICLE *veh = (I_VEHICLE *) obj->more;
	      if (veh->sector_use == SECT_WATER_SWIM || 
		  veh->sector_use == SECT_WATER_NOSWIM)
		REMOVE_BIT(ch->act, PLR_HAS_BOAT);
	    }
	  if (number_percent () > ch->pcdata->learned[gsn_hide] + ch->pcdata->plus_hide)
	    act ("$n hides $p\x1B[0m.", ch, obj, NULL, TO_ROOM);
	  return;
	}

          act ("You drop $p\x1B[37;0m.", ch, obj, NULL, TO_CHAR);
          act ("$n drops $p\x1B[37;0m.", ch, obj, NULL, TO_ROOM);
      obj_from (obj);
      if (obj->pIndexData->item_type != ITEM_BOAT && (ch->in_room->sector_type == SECT_WATER_SWIM || ch->in_room->sector_type == SECT_WATER_NOSWIM))
	{
	  act ("The waters cause $p to sink beneath them; lost forever.", ch, obj, NULL, TO_CHAR);
	  free_it (obj);
	}
      else
	{
	  obj_to (obj, ch->in_room);
	  if (obj->pIndexData->item_type == ITEM_VEHICLE && IS_PLAYER(ch))
	    {
	      I_VEHICLE *veh = (I_VEHICLE *) obj->more;
	      if (veh->sector_use == SECT_WATER_SWIM || 
		  veh->sector_use == SECT_WATER_NOSWIM)
		REMOVE_BIT(ch->act, PLR_HAS_BOAT);
	    }
	  if (IS_OBJ_STAT(obj, ITEM_DROPSCRIPT))
	    {
	    again_8:
	      /* Dropped item, check for triggers on object! */
	      {
		SINGLE_TRIGGER *tr;
		SCRIPT_INFO *s;
		for (tr = trigger_list[TDROPPED]; tr != NULL; tr = tr->next)
		  {
		    if (obj->pIndexData->vnum == tr->attached_to_obj)
		      {
			if (tr->running_info && !tr->interrupted)
			  continue;	/* Already running, interrupted, but script says not to allow interruptions. */
			if (tr->running_info && tr->interrupted != 2)
			  {
			    end_script (tr->running_info);
			    goto again_8;
			  }
			{
			  char tmp[500];
			  sprintf(tmp,"%d",ch->in_room->vnum);
			  if (tr->keywords[0] != '\0' && !one_is_in_list_two (tmp, tr->keywords))
			    continue;
			}
			/* ----------------- */
			/* Start the script! */
			/* ----------------- */
			tr->running_info = mem_alloc (sizeof (*tr->running_info));
			s = tr->running_info;
			bzero (s, sizeof (*s));
			s->current = ch;
			s->obj = obj;
			strcpy (s->code_seg, tr->code_label);
			s->current_line = 0;
			s->called_by = tr;
			s->next = info_list;
			info_list = s;
			execute_code (s);
			/* ----------------- */
		      }
		  }
	      }
	    }
	      /* End trigger check! */
	  if (IS_SET(ch->in_room->room_flags, ROOM_DROPSCRIPT))
	    {
	    again_9:
	      /* Dropped item, check for triggers on room! */
	      {
		SINGLE_TRIGGER *tr;
		SCRIPT_INFO *s;
		for (tr = trigger_list[TDROPPED]; tr != NULL; tr = tr->next)
		  {
		    if (ch->in_room->vnum == tr->attached_to_room)
		      {
			if (tr->running_info && !tr->interrupted)
			  continue;	/* Already running, interrupted, but script says not to allow interruptions. */
			if (tr->running_info && tr->interrupted != 2)
			  {
			    end_script (tr->running_info);
			    goto again_9;
			  }
			{
			  char tmp[50];
			  sprintf(tmp,"%d",obj->pIndexData->vnum);
			  if (tr->keywords[0] != '\0' && !one_is_in_list_two (tmp, tr->keywords))
			    continue;
			}
			/* ----------------- */
			/* Start the script! */
			/* ----------------- */
			tr->running_info = mem_alloc (sizeof (*tr->running_info));
			s = tr->running_info;
			bzero (s, sizeof (*s));
			s->current = ch;
			s->room = ch->in_room;
			s->obj = obj;
			strcpy (s->code_seg, tr->code_label);
			s->current_line = 0;
			s->called_by = tr;
			s->next = info_list;
			info_list = s;
			execute_code (s);
			/* ----------------- */
		      }
		  }
	      }
	    }
	  /* End trigger check! */
	  
	  
	  
	  
	  
	}
    }
  else
    {
      if (!str_cmp (arg, "all"))
	{
	  found = FALSE;
	  if (tally_coins (ch) > 0)
	    {
	      char bunl[500];
	      if (ch->in_room->sector_type != SECT_WATER_SWIM && ch->in_room->sector_type != SECT_WATER_NOSWIM &&
		  (LEVEL (ch) < 100 || IS_REAL_GOD (ch)))
		{
		  ch->in_room->more->gold += ch->gold;
		  ch->in_room->more->copper += ch->copper;
		}
	      sprintf (bunl, "You drop your coins.\n\r");
	      send_to_char (bunl, ch);
	      found = TRUE;
	      ch->gold = 0;
	      ch->copper = 0;
	    }
	}



      for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
	  if (!obj->next_content)
	    obj_next = NULL;
	  else
	    obj_next = obj->next_content;
	  if ((arg[3] == '\0' || is_name (&arg[4], obj->pIndexData->name))
	      && can_see_obj (ch, obj)
	      && (obj->wear_loc == WEAR_NONE)
	      && can_drop_obj (ch, obj))
	    {
	      found = TRUE;
              if (IS_PLAYER(ch) && !opp_align)
                {	      
	      act ("$n drops $p\x1B[37;0m.", ch, obj, NULL, TO_ROOM);
	      act ("You drop $p\x1B[37;0m.", ch, obj, NULL, TO_CHAR);
                 }
	      obj_from (obj);
	      if (obj->pIndexData->item_type != ITEM_BOAT && (ch->in_room->sector_type == SECT_WATER_SWIM || ch->in_room->sector_type == SECT_WATER_NOSWIM))
		{
		  act ("The waters cause $p to sink beneath them; lost forever.", ch, obj, NULL, TO_CHAR);
		  free_it (obj);
		}
	      else
		{
		  if (IS_OBJ_STAT(obj, ITEM_DROPSCRIPT))
		    {
		    again_10:
		      /* Dropped item, check for triggers on object! */
		      {
			SINGLE_TRIGGER *tr;
			SCRIPT_INFO *s;
			for (tr = trigger_list[TDROPPED]; tr != NULL; tr = tr->next)
			  {
			    if (obj->pIndexData->vnum == tr->attached_to_obj)
			      {
				if (tr->running_info && !tr->interrupted)
				  continue;		/* Already running, interrupted, but script says not to allow interruptions. */
				if (tr->running_info && tr->interrupted != 2)
				  {
				    end_script (tr->running_info);
				    goto again_10;
				  }
				{
				  char tmp[50];
				  sprintf(tmp,"%d",ch->in_room->vnum);
				  if (tr->keywords[0] != '\0' && !one_is_in_list_two (tmp, tr->keywords))
				    continue;
				}
				/* ----------------- */
				/* Start the script! */
				/* ----------------- */
				tr->running_info = mem_alloc (sizeof (*tr->running_info));
				s = tr->running_info;
				bzero (s, sizeof (*s));
				s->current = ch;
				s->obj = obj;
				strcpy (s->code_seg, tr->code_label);
				s->current_line = 0;
				s->called_by = tr;
				s->next = info_list;
				info_list = s;
				execute_code (s);
				/* ----------------- */
			      }
			  }
		      }
		    }
/* End trigger check! */
		  if (IS_SET(ch->in_room->room_flags, ROOM_DROPSCRIPT))
		    {
		    again_11:
		      /* Dropped item, check for triggers on room! */
		      {
			SINGLE_TRIGGER *tr;
			SCRIPT_INFO *s;
			for (tr = trigger_list[TDROPPED]; tr != NULL; tr = tr->next)
			  {
			    if (ch->in_room->vnum == tr->attached_to_room)
			      {
				if (tr->running_info && !tr->interrupted)
				  continue;		/* Already running, interrupted, but script says not to allow interruptions. */
				if (tr->running_info && tr->interrupted != 2)
				  {
				    end_script (tr->running_info);
				    goto again_11;
				  }
				{
				  char tmp[50];
				  sprintf(tmp,"%d",obj->pIndexData->vnum);
				  if (tr->keywords[0] != '\0' && !one_is_in_list_two (tmp, tr->keywords))
				    continue;
				}
				/* ----------------- */
				/* Start the script! */
				/* ----------------- */
				tr->running_info = mem_alloc (sizeof (*tr->running_info));
				s = tr->running_info;
				bzero (s, sizeof (*s));
				s->current = ch;
				s->room = ch->in_room;
				s->obj = obj;
				strcpy (s->code_seg, tr->code_label);
				s->current_line = 0;
				s->called_by = tr;
				s->next = info_list;
				info_list = s;
				execute_code (s);
				/* ----------------- */
			      }
			  }
		      }
		    }
		  /* End trigger check! */
		  
		  
		  obj_to (obj, ch->in_room);
		  if (obj->pIndexData->item_type == ITEM_VEHICLE && IS_PLAYER(ch))
		    {
		      I_VEHICLE *veh = (I_VEHICLE *) obj->more;
		      if (veh->sector_use == SECT_WATER_SWIM || 
			  veh->sector_use == SECT_WATER_NOSWIM)
			REMOVE_BIT(ch->act, PLR_HAS_BOAT);
		    }
		}
	    }
	}
      if (!found)
	{
	  if (arg[3] == '\0')
	    act ("You are not carrying anything.",
		 ch, NULL, arg, TO_CHAR);
	  else
	    act ("You are not carrying $T.",
		 ch, NULL, &arg[4], TO_CHAR);
	}
      else if (IS_PLAYER(ch) && opp_align)
        {
          act("$n drops some stuff.", ch, NULL, NULL, TO_ROOM);
          act("You drop some stuff.", ch, NULL, NULL, TO_CHAR);
        }
    }
  return;
}

void 
do_give (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  CHAR_DATA *victim;
  SINGLE_OBJECT *obj;
  int amount;
  char buf[STD_LENGTH];
  DEFINE_COMMAND ("give", do_give, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to give something to someone.")

    if (FIGHTING (ch) && !pow.give_while_fighting)
    {
      send_to_char ("You may not give objects while fighting!\n\r", ch);
      return;
    }

  CAN_GIVE_ITEMS_IN_ARENA (no)
    CAN_GIVE_ITEMS_IN_BGROUND (no)

    argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  argy = one_argy (argy, arg3);
  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
      send_to_char ("Give what to whom?\n\r", ch);
      return;
    }

  if ((!str_cmp (arg2, "gold"))
      && (is_number (arg1) && atoi (arg1) > 0))
    {
      amount = atoi (arg1);
      if (amount < 1)
	return;
      if (ch->gold < amount)
	{
	  send_to_char ("You don't have that many \x1B[1;33mgold\x1B[37;0m coins.\n\r", ch);
	  return;
	}
      if ((victim = get_char_room (ch, arg3)) == NULL)
	{
	  send_to_char ("You don't see them here.\n\r", ch);
	  return;
	}
      if (IS_PLAYER(victim) && IS_PLAYER(ch) && DIFF_ALIGN(ch, victim))
        {
          send_to_char("You cannot give anything to enemies!\n\r", ch);
          return;
        }
      if (victim == ch)
	return;
      ch->gold -= amount;
      victim->gold += amount;
      sprintf (buf, "You give %d \x1B[1;33mgold\x1B[37;0m coins to %s.\n\r",
	       amount, NAME (victim));
      send_to_char (buf, ch);
      act ("$n gives some \x1B[1;33mgold\x1B[37;0m to $N.", ch, NULL, victim, TO_NOTVICT);
      sprintf (buf, "%s gives you %d \x1B[1;33mgold\x1B[37;0m coins.\n\r", NAME (ch), amount);
      send_to_char (buf, victim);
/*     if ( IS_MOB(victim) && victim->npcdata->hire == ch )
   {
   victim->npcdata->paid += (amount*100);
   if ( victim->npcdata->wage > victim->npcdata->paid )
   {
   sprintf(buf,"%s You still owe me %s.", NAME(ch),
   name_amount(victim->npcdata->wage - victim->npcdata->paid ));
   }
   else
   {
   sprintf(buf,"%s Thanks.", NAME(ch));
   victim->leader = ch;
   victim->master = ch;
   act("$N joins your group.",ch,NULL,victim,TO_CHAR);
   }
   do_tell(victim,buf);
   } */
      return;
    }

  if ((!str_cmp (arg2, "copper"))
      && (is_number (arg1) && atoi (arg1) > 0))
    {
      amount = atoi (arg1);
      if (amount < 1)
	return;
      if (ch->copper < amount)
	{
	  send_to_char ("You don't have that many \x1B[33mcopper\x1B[37;0m coins.\n\r", ch);
	  return;
	}
      if ((victim = get_char_room (ch, arg3)) == NULL)
	{
	  send_to_char ("You don't see them here.\n\r", ch);
	  return;
	}
      if (IS_PLAYER(victim) && IS_PLAYER(ch) && DIFF_ALIGN(ch, victim))
        {
          send_to_char("You cannot give anything to enemies!\n\r", ch);
          return;
        }

      if (victim == ch)
	return;
      ch->copper -= amount;
      victim->copper += amount;
      sprintf (buf, "You give %d \x1B[33mcopper\x1B[37;0m coins to %s.\n\r",
	       amount, NAME (victim));
      send_to_char (buf, ch);
      act ("$n gives some \x1B[33mcopper\x1B[37;0m to $N.", ch, NULL, victim, TO_NOTVICT);
      sprintf (buf, "%s gives you %d \x1B[33mcopper\x1B[37;0m coins.\n\r", NAME (ch), amount);
      send_to_char (buf, victim);
/*     if ( IS_MOB(victim) && victim->npcdata->hire == ch )
   {
   victim->npcdata->paid += (amount);
   if ( victim->npcdata->wage > victim->npcdata->paid )
   {
   sprintf(buf,"%s You still owe me %s.", NAME(ch),
   name_amount(victim->npcdata->wage - victim->npcdata->paid ));
   }
   else
   {
   sprintf(buf,"%s Thanks.", NAME(ch));
   victim->leader = ch;
   victim->master = ch;
   ch->num_in_group++;
   act("$N joins your group.",ch,NULL,victim,TO_CHAR);
   }
   do_tell(victim,buf);
   } */
      return;
    }

  if ((victim = get_char_room (ch, arg2)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
      if (IS_PLAYER(victim) && IS_PLAYER(ch) && DIFF_ALIGN(ch, victim))
        {
          send_to_char("You cannot give anything to enemies!\n\r", ch);
          return;
        }

  if ((obj = get_obj_inv (ch, arg1)) == NULL)
    {
      send_to_char ("You do not have that item.\n\r", ch);
      return;
    }
  if (!NOT_WORN (obj))
    {
      send_to_char ("You must remove it first.\n\r", ch);
      return;
    }
  if ((victim = get_char_room (ch, arg2)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (!can_drop_obj (ch, obj))
    {
      send_to_char ("You can't let go of it.\n\r", ch);
      return;
    }
  if (!can_see_obj (victim, obj))
    {
      act ("$N can't see it.", ch, NULL, victim, TO_CHAR);
      return;
    }
  if (obj->pIndexData->vnum > 90000 &&
   (obj->pIndexData->item_type == ITEM_FURNITURE ||
    obj->pIndexData->item_type == ITEM_TOOL))
  {
    fprintf (stderr, "FORGECRAP %s given by %s to %s at %d.\n",
    OOSTR(obj, short_descr), NAME(ch), NAME(victim), ch->in_room->vnum);
    }
  obj_from (obj);
  obj_to (obj, victim);
  if (obj->pIndexData->item_type == ITEM_VEHICLE && IS_PLAYER(ch))
    {
      I_VEHICLE *veh = (I_VEHICLE *) obj->more;
      if (veh->sector_use == SECT_WATER_SWIM || 
	  veh->sector_use == SECT_WATER_NOSWIM)
	REMOVE_BIT(ch->act, PLR_HAS_BOAT);
    }
  if (obj->pIndexData->item_type == ITEM_VEHICLE && IS_PLAYER(victim))
    {
      I_VEHICLE *veh = (I_VEHICLE *) obj->more;
      if (veh->sector_use == SECT_WATER_SWIM || 
	  veh->sector_use == SECT_WATER_NOSWIM)
	REMOVE_BIT(victim->act, PLR_HAS_BOAT);
    }
  obj->wear_loc = -1;
  if (IS_SET (obj->extra_flags, ITEM_INVENTORY))
    obj->extra_flags ^= ITEM_INVENTORY;
  act ("$n gives $p\x1B[0m to $N.", ch, obj, victim, TO_NOTVICT);
  act ("$n gives you $p\x1B[0m.", ch, obj, victim, TO_VICT);
  act ("You give $p\x1B[0m to $N.", ch, obj, victim, TO_CHAR);
  if (IS_OBJ_STAT(obj, ITEM_GIVESCRIPT))
    {
    again_12:
      /* Given item, check for triggers on mobbie! */
      if (IS_MOB (victim))
	{
	  SINGLE_TRIGGER *tr;
	  SCRIPT_INFO *s;
	  for (tr = trigger_list[TGIVEN]; tr != NULL; tr = tr->next)
	    {
	      if (victim->pIndexData->vnum == tr->attached_to_mob)
		{
		  if (tr->running_info && !tr->interrupted)
		    continue;	/* Already running, interrupted, but script says not to allow interruptions. */
		  if (tr->running_info && tr->interrupted != 2)
		    {
		      end_script (tr->running_info);
		      goto again_12;
		    }
		  {char tmp[50];
		  sprintf(tmp,"%d",obj->pIndexData->vnum);
		  if (tr->keywords[0] != '\0' && !one_is_in_list_two (tmp, tr->keywords))
		    continue;
		  }
		  /* ----------------- */
		  /* Start the script! */
		  /* ----------------- */
		  tr->running_info = mem_alloc (sizeof (*tr->running_info));
		  s = tr->running_info;
		  bzero (s, sizeof (*s));
		  s->current = ch;
		  s->mob = victim;
		  strcpy (s->code_seg, tr->code_label);
		  s->current_line = 0;
		  s->called_by = tr;
		  s->next = info_list;
		  info_list = s;
		  execute_code (s);
		  /* ----------------- */
		}
	    }
	
      
      /* End trigger check! */
      
    again_13:
      {
	SINGLE_TRIGGER *tr;
	SCRIPT_INFO *s;
	for (tr = trigger_list[TGIVEN]; tr != NULL; tr = tr->next)
	  {
	    if (obj->pIndexData->vnum == tr->attached_to_obj)
	      {
		if (tr->running_info && !tr->interrupted)
		  continue;	/* Already running, interrupted, but script says not to allow interruptions. */
		if (tr->running_info && tr->interrupted != 2)
		  {
		    end_script (tr->running_info);
		    goto again_13;
		  }
		{char tmp[50];
		sprintf(tmp,"%d",victim->pIndexData->vnum);
		if (tr->keywords[0] != '\0' && !one_is_in_list_two (tmp , tr->keywords))
		  continue;
		}
		/* ----------------- */
		/* Start the script! */
		/* ----------------- */
		tr->running_info = mem_alloc (sizeof (*tr->running_info));
		s = tr->running_info;
		bzero (s, sizeof (*s));
		s->current = ch;
		s->obj = obj;
		s->mob = victim;
		strcpy (s->code_seg, tr->code_label);
		s->current_line = 0;
		s->called_by = tr;
		s->next = info_list;
		info_list = s;
		execute_code (s);
		/* ----------------- */
	      }
	  }
      }
    }
  }
  /* End trigger check! */
  return;
}

void 
do_fill (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *fountain;
  I_DRINK *dr;
  bool found;
  DEFINE_COMMAND ("fill", do_fill, POSITION_STANDING, 0, LOG_NORMAL, "This command allows you to fill a container or lamp.")

    argy = one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Fill what?\n\r", ch);
      return;
    }
/*held */
  if ((obj = get_obj_inv (ch, arg)) == NULL)
    {
      send_to_char ("You do not have that item.\n\r", ch);
      return;
    }
  if (obj->pIndexData->item_type == ITEM_DRINK_CON)
    {
      found = FALSE;
      dr = (I_DRINK *) obj->more;
      for (fountain = ch->in_room->more->contents; fountain != NULL;
	   fountain = fountain->next_content)
	{
	  if (fountain->pIndexData->item_type == ITEM_FOUNTAIN)
	    {
	      found = TRUE;
	      break;
	    }
	}
      if (!found)
	{
	  send_to_char ("There is no fountain here!\n\r", ch);
	  return;
	}
      if (dr->liquid_now != 0 && dr->liquid_type != 0)
	{
	  send_to_char ("It already has some other liquid in it.\n\r", ch);
	  return;
	}
      if (IS_SET(dr->not_poison, 1))
	{
	  send_to_char("That container cannot be refilled.\n\r", ch);
	  return;
	}
      if (dr->liquid_now >= dr->max_liquid)
	{
	  send_to_char ("The container is full.\n\r", ch);
	  return;
	}
      act ("You fill $p\x1B[0m.", ch, obj, NULL, TO_CHAR);
      dr->liquid_type = 0;
      dr->liquid_now = dr->max_liquid;
    }
  else if (obj->pIndexData->item_type == ITEM_LIGHT)
    {
      SINGLE_OBJECT *obj2;
      I_LIGHT *lg = (I_LIGHT *) obj->more;
      I_DRINK *dr;
      /*get_item_hold */
      if ((obj2 = get_item_held (ch, ITEM_DRINK_CON)) == NULL)
	{
	  send_to_char ("You don't have anything in your hand to fill it with.\n\r", ch);
	  return;
	}
      dr = (I_DRINK *) obj2->more;
      if (!IS_SET (lg->light_lit, LIGHT_FILLABLE) && !IS_SET (lg->light_fillable, LIGHT_FILLABLE))
	{
	  send_to_char ("It can't be refueled.\n\r", ch);
	  return;
	}
      if (dr->liquid_type != -1)
	{
	  send_to_char ("There's no oil in that container.\n\r", ch);
	  return;
	}
      if (lg->light_now != 0)
	{
	  send_to_char ("There is already some oil in it.\n\r", ch);
	  return;
	}

      CAN_FILL_LIGHT_WHILE_LIT (yes);

      if (dr->liquid_now < 1)
	{
	  send_to_char ("There is no liquid left to fill it with.\n\r", ch);
	  return;
	}
      if (dr->liquid_now >= lg->max_light)
	{
	  act ("You fill $p\x1B[0m with $P\x1B[0m.", ch, obj, obj2, TO_CHAR);
	  lg->light_now = lg->max_light;
	  dr->liquid_now += -(lg->max_light);
	}
      else
	{
	  act ("You empty $P\x1B[0m into $p\x1B[0m.", ch, obj, obj2, TO_CHAR);
	  lg->light_now = lg->max_light;
	  dr->liquid_now = 0;
	}
    }
  else
    {
      send_to_char ("You can't fill that.\n\r", ch);
      return;
    }
  return;
}

void 
do_empty (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND ("empty", do_empty, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to empty a container of liquid.")

    argy = one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Empty what?\n\r", ch);
      return;
    }
/*held */
  if ((obj = get_obj_inv (ch, arg)) == NULL)
    {
      send_to_char ("You do not have that item.\n\r", ch);
      return;
    }
  if (obj->pIndexData->item_type == ITEM_DRINK_CON)
    {
      I_DRINK *dr = (I_DRINK *) obj->more;
      if (dr->liquid_now < 1)
	{
	  send_to_char ("Your container is empty.\n\r", ch);
	  return;
	}
      act ("You empty $p\x1B[0m.", ch, obj, NULL, TO_CHAR);
      dr->liquid_type = 0;
      dr->liquid_now = 0;
    }
  return;
}

void 
do_drink (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  int amount;
  bool thirsty;
  int liquid;
  DEFINE_COMMAND ("drink", do_drink, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to drink from something.")

    while (argy != "" && argy[0] != '\0')
    argy = one_argy (argy, arg);

  if (arg[0] == '\0')
    {
      for (obj = ch->in_room->more->contents; obj; obj = obj->next_content)
	{
	  if (obj->pIndexData->item_type == ITEM_FOUNTAIN)
	    break;
	}
      if (obj == NULL)
	{
	  send_to_char ("Drink what?\n\r", ch);
	  return;
	}
    }
  else
    {
      if ((obj = get_obj_here (ch, arg, SEARCH_ROOM_FIRST)) == NULL)
	{
	  send_to_char ("You can't find it.\n\r", ch);
	  return;
	}
    }
  if (IS_PLAYER (ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    {
      send_to_char ("You are too drunk, dazed, and confused to drink any more.\n\r", ch);
      return;
    }
  if (IS_PLAYER (ch) && ch->pcdata->condition[COND_THIRST] < 7)
    thirsty = TRUE;
  else
    thirsty = FALSE;
  if (IS_PLAYER (ch) && ch->pcdata->condition[COND_THIRST] > 40)
    {
      send_to_char ("If you drank any more, your bladder would explode.\n\r", ch);
      return;
    }
  switch (obj->pIndexData->item_type)
    {
    default:
      send_to_char ("You can't drink from that.\n\r", ch);
      break;
    case ITEM_FOUNTAIN:
      {
	I_FOUNTAIN *fn = (I_FOUNTAIN *) obj->more;
	if (IS_PLAYER (ch))
	  ch->pcdata->condition[COND_THIRST] = 48;
	act ("$n drinks from $p\x1B[0m.", ch, obj, NULL, TO_ROOM);
	act ("You drink from $p\x1B[0m.", ch, obj, NULL, TO_CHAR);
	if (fn->spells[0] > 0)
	  obj_cast_spell (fn->spells[0], fn->liquid, ch, ch, NULL);
	if (fn->spells[1] > 0)
	  obj_cast_spell (fn->spells[1], fn->liquid, ch, ch, NULL);
	if (fn->spells[2] > 0)
	  obj_cast_spell (fn->spells[2], fn->liquid, ch, ch, NULL);
	break;
      }
    case ITEM_DRINK_CON:
      {
	I_DRINK *dr = (I_DRINK *) obj->more;
	if (dr->liquid_now <= 0)
	  {
	    send_to_char ("It is empty.\n\r", ch);
	    return;
	  }
	if ((liquid = dr->liquid_type) >= LIQ_MAX)
	  {
	    bug ("Bad liquid number %d.", liquid);
	    liquid = dr->liquid_type = 0;
	  }
	if (liquid == -1)
	  {
	    act ("$n drinks lamp oil from $p\x1B[0m, and grimaces in pain.",
		 ch, obj, NULL, TO_ROOM);
	    act ("You drink oil from $p\x1B[0m.  Ugh!",
		 ch, obj, NULL, TO_CHAR);
	    amount = 0;
	  }
	else
	  {
	    act ("$n drinks $T\x1B[0m from $p\x1B[0m.",
		 ch, obj, liq_table[liquid].liq_name, TO_ROOM);
	    act ("You drink $T\x1B[0m from $p\x1B[0m.",
		 ch, obj, liq_table[liquid].liq_name, TO_CHAR);
	    amount = number_range (3, 10);
	    amount = UMIN (amount, dr->liquid_now);
	    gain_condition (ch, COND_DRUNK,
			 amount * liq_table[liquid].liq_affect[COND_DRUNK]);
	    gain_condition (ch, COND_FULL,
			  amount * liq_table[liquid].liq_affect[COND_FULL]);
	    gain_condition (ch, COND_THIRST,
			amount * liq_table[liquid].liq_affect[COND_THIRST]);
	    if (IS_PLAYER (ch) && ch->pcdata->condition[COND_DRUNK] > 10)
	      send_to_char ("You feel drunk.\n\r", ch);
	    if (IS_PLAYER (ch) && ch->pcdata->condition[COND_FULL] > 40)
	      send_to_char ("You are full.\n\r", ch);
	    if (IS_PLAYER (ch) && ch->pcdata->condition[COND_THIRST] > 40)
	      send_to_char ("You are no longer thirsty.\n\r", ch);
	    if (dr->not_poison >= 2)
	      {
		AFFECT_DATA af;
		bzero (&af, sizeof (af));
		act ("$n's face pales.", ch, NULL, NULL, TO_ROOM);
		send_to_char ("Your intestines feel like they are on fire!\n\r", ch);
		af.type = gsn_poison;
		af.duration = 3 * amount;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_POISON;
		affect_join (ch, &af);
	      }
	  }
	dr->liquid_now -= amount;
	if (dr->liquid_now <= 0)
	  {
	    dr->not_poison = 0;
	    dr->liquid_type = 0;
	  }
	break;
      }
    }

  return;
}

void 
do_eat (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND ("eat", do_eat, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to eat something.")

    argy = one_argy (argy, arg);

  if (!str_cmp (arg, "the") || !str_cmp (arg, "some"))
    argy = one_argy (argy, arg);

  if (arg[0] == '\0')
    {
      send_to_char ("Eat what?\n\r", ch);
      return;
    }
  if ((obj = get_obj_inv (ch, arg)) == NULL)
    {
      send_to_char ("You do not have that item.\n\r", ch);
      return;
    }
  if (!IS_IMMORTAL (ch))
    {
      if (obj->pIndexData->item_type != ITEM_FOOD && obj->pIndexData->item_type != ITEM_PILL)
	{
	  send_to_char ("That's not edible.\n\r", ch);
	  return;
	}
      if (IS_PLAYER (ch) &&  ch->pcdata->condition[COND_FULL] > 40)
	{
	  send_to_char ("You are too full to eat more.\n\r", ch);
	  return;
	}
    }
  else
    {
      if (obj->pIndexData->item_type == ITEM_LIGHT && IS_LIT (obj))
	--ch->in_room->light;
      if (ch->in_room->light < 0) ch->in_room->light = 0;
      if (obj->wear_loc != -1)
	remove_obj (ch, obj->wear_loc, TRUE);
    }
  act ("$n eats $p\x1B[0m.", ch, obj, NULL, TO_ROOM);
  act ("You eat $p\x1B[0m.", ch, obj, NULL, TO_CHAR);
  switch (obj->pIndexData->item_type)
    {
    case ITEM_FOOD:
      {
	I_FOOD *fd = (I_FOOD *) obj->more;
	if (IS_PLAYER (ch))
	  {
	    int conditiona;
	    conditiona = ch->pcdata->condition[COND_FULL];
	    gain_condition (ch, COND_FULL, fd->hours);
	    if (conditiona < 4 && ch->pcdata->condition[COND_FULL] > 4)
	      send_to_char ("You feel a bit less hungry.\n\r", ch);
	    else if (ch->pcdata->condition[COND_FULL] > 40)
	      send_to_char ("You are full.\n\r", ch);
	  }
	if (fd->not_poison != 0)
	  {
	    /* The shit was poisoned! */
	    AFFECT_DATA af;
	    bzero (&af, sizeof (af));
	    act ("$n's face pales, and $e clutches $s stomach.", ch, 0, 0, TO_ROOM);
	    send_to_char ("Ouch!  You feel very sick!\n\r", ch);
	    af.type = gsn_poison;
	    af.duration = fd->hours / 2;
	    af.location = APPLY_NONE;
	    af.modifier = 0;
	    af.bitvector = AFF_POISON;
	    affect_join (ch, &af);
	  }
	break;
      }
    case ITEM_PILL:
      {
	I_POTION *pl = (I_POTION *) obj->more;
	if (pl->spells[0] > 0)
	  obj_cast_spell (pl->spells[0], pl->spell_level, ch, ch, NULL);
	if (pl->spells[1] > 0)
	  obj_cast_spell (pl->spells[1], pl->spell_level, ch, ch, NULL);
	if (pl->spells[2] > 0)
	  obj_cast_spell (pl->spells[2], pl->spell_level, ch, ch, NULL);
	break;
      }
    }
  obj_from (obj);
  free_it (obj);
  return;
}

bool 
draw_obj (CHAR_DATA * ch, SINGLE_OBJECT * obj)
{
  if (obj->wear_loc != WEAR_BELT_1
      && obj->wear_loc != WEAR_BELT_2
      && obj->wear_loc != WEAR_BELT_3
      && obj->wear_loc != WEAR_BELT_4
      && obj->wear_loc != WEAR_BELT_5)
    return FALSE;
  if (hand_empty (ch) == WEAR_NONE)
    return FALSE;
  obj->wear_loc = -1;
  equip_char (ch, obj, hand_empty (ch));
  act ("$n draws $p\x1B[0m from $s belt.", ch, obj, NULL, TO_ROOM);
  act ("You draw $p\x1B[0m from your belt.", ch, obj, NULL, TO_CHAR);
  return TRUE;
}

bool 
sheath_obj (CHAR_DATA * ch, SINGLE_OBJECT * obj)
{
  if (obj == NULL)
    return FALSE;
  if (!IS_SET (obj->pIndexData->wear_flags, ITEM_WEAR_BELT))
    return FALSE;
  if (get_eq_char (ch, WEAR_WAIST) == NULL)
    {
      send_to_char ("You need a belt to sheath it in.\n\r", ch);
      return TRUE;
    }
 
  if (
       obj->wear_loc != WEAR_HOLD_1
       && obj->wear_loc != WEAR_HOLD_2)
    {
      send_to_char ("You must be holding the weapon to sheath it!\n\r", ch);
      return FALSE;
    }
 if (IS_OBJ_STAT(obj, ITEM_POLEARM) || IS_OBJ_STAT(obj, ITEM_RANGED))
  { 
   send_to_char("You cannot sheath polearms.\n\r", ch);
   return FALSE;
  } 
 if (belt_empty (ch) == WEAR_NONE)
    {
      send_to_char ("Remove something from your belt first.\n\r", ch);
      return TRUE;
    }
  unequip_char (ch, obj);
  obj->wear_loc = belt_empty (ch);
  act ("$n sheaths $p\x1B[0m in $s belt.", ch, obj, NULL, TO_ROOM);
  act ("You sheath $p\x1B[0m in your belt.", ch, obj, NULL, TO_CHAR);
  return TRUE;
}


bool 
remove_obj (CHAR_DATA * ch, int iWear, bool fReplace)
{
  SINGLE_OBJECT *obj;
  if ((obj = get_eq_char (ch, iWear)) == NULL)
    return TRUE;
  if (!fReplace)
    return FALSE;
  if (obj->wear_loc == WEAR_NONE)
    {
      return FALSE;
    }
  if (IS_SET (obj->extra_flags, ITEM_NOREMOVE) && !IS_IMMORTAL (ch))
    {
      act ("You can't remove $p\x1B[0m.", ch, obj, NULL, TO_CHAR);
      return FALSE;
    }
  if (obj->wear_loc == WEAR_WAIST)
    {
      SINGLE_OBJECT *obj2;
      bool fFound = FALSE;
      int curwear;
      for (curwear = WEAR_BELT_1; curwear <= WEAR_BELT_3; curwear++)
	{
	  obj2 = get_eq_char (ch, curwear);
	  if (obj2 != NULL)
	    {
	      fFound = TRUE;
	      obj2->wear_loc = -1;
	    }
	}
      if (fFound)
	{
	}
    }
  if (unequip_char (ch, obj))
    {
      act ("$n stops using $p\x1B[0m.", ch, obj, NULL, TO_ROOM);
      act ("You stop using $p\x1B[0m.", ch, obj, NULL, TO_CHAR);
    }
  else
    {
      send_to_char ("You've got your hands full.\n\r", ch);
      return FALSE;
    }
  return TRUE;
}


void 
wear_obj (CHAR_DATA * ch, SINGLE_OBJECT * obj, bool fReplace, int loc)
{
  SHOP_DATA *pShop;

  OBJ_PROTOTYPE *pObj = obj->pIndexData;
  SINGLE_OBJECT *obj1;
  SINGLE_OBJECT *obj2;
  SINGLE_OBJECT *obj3;

  pShop = NULL;
  if (IS_MOB (ch) && (pShop = ch->pIndexData->pShop) != NULL)
    return;
  else
    {
      /*OKAY, not a shopkeeper, go ahead and wear stuff */

      // Check to see if the person is in a guild that does not allow this item
      if(is_member(ch,GUILD_TINKER) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_TINKER) ||
	 is_member(ch,GUILD_WARRIOR) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_WARRIOR) ||
	 is_member(ch,GUILD_THIEFG) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_THIEF) ||
	 is_member(ch,GUILD_HEALER) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_HEALER) ||
	 is_member(ch,GUILD_WIZARD) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_WIZARD) ||
	 is_member(ch,GUILD_RANGER) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_RANGER) ||
	 is_member(ch,GUILD_ROGUE) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_ROGUE) ||
	 is_member(ch,GUILD_CONJURER) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_CONJURER) ||
	 is_member(ch,GUILD_MYSTIC) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_MYSTIC) ||
	 is_member(ch,GUILD_BATTLEMASTER) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_BATTLEMASTER) ||
	 is_member(ch,GUILD_NECROMANCER) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_NECROMANCER) ||
	 is_member(ch,GUILD_MONK) && IS_OBJ_GUILD_STAT(obj,ITEM_ANTI_MONK)){
	send_to_char("One of your guilds restricts you from using this item.\n\r",ch);
	return;
      }

      if (pObj != NULL)
      {
	int i;
	if (pObj->value[10] != 0)
	for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list");i++)
	{
	   if (IS_SET(pObj->value[10], guild_data[i].mob_guildmaster_bit) && !IS_SET(ch->pcdata->guilds, guild_data[i].mob_guildmaster_bit))
	   {
		send_to_char ("You do not have the proper training to wear this item!\n\r", ch);
		return;
	   }
	}
      }
      if (CAN_WEAR (obj, ITEM_WEAR_FINGER))
	{
	  if (loc == 0 || loc == ITEM_WEAR_FINGER)
	    {
	      if (get_eq_char (ch, WEAR_FINGER_1) != NULL
		  && get_eq_char (ch, WEAR_FINGER_2) != NULL
		  && !remove_obj (ch, WEAR_FINGER_1, fReplace)
		  && !remove_obj (ch, WEAR_FINGER_2, fReplace))
		return;
	      if (get_eq_char (ch, WEAR_FINGER_1) == NULL)
		{
		  act ("$n slips $p\x1B[0m onto $s left finger.", ch, obj, NULL, TO_ROOM);
		  act ("You slip $p\x1B[0m onto your left finger.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_FINGER_1);
		  return;
		}
	      if (get_eq_char (ch, WEAR_FINGER_2) == NULL)
		{
		  act ("$n slips $p onto $s right finger.", ch, obj, NULL, TO_ROOM);
		  act ("You slip $p onto your right finger.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_FINGER_2);
		  return;
		}
	      bug ("Wear_obj: no free finger.", 0);
	      send_to_char ("You already wear two rings.\n\r", ch);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_NECK))
	{
	  if (loc == 0 || loc == ITEM_WEAR_NECK)
	    {
	      if (get_eq_char (ch, WEAR_NECK_1) != NULL
		  && get_eq_char (ch, WEAR_NECK_2) != NULL
		  && !remove_obj (ch, WEAR_NECK_1, fReplace)
		  && !remove_obj (ch, WEAR_NECK_2, fReplace))
		return;
	      if (get_eq_char (ch, WEAR_NECK_1) == NULL)
		{
		  act ("$n clasps $p\x1B[0m around $s neck.", ch, obj, NULL, TO_ROOM);
		  act ("You clasp $p\x1B[0m around your neck.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_NECK_1);
		  return;
		}
	      if (get_eq_char (ch, WEAR_NECK_2) == NULL)
		{
		  act ("$n clasps $p\x1B[0m around $s neck.", ch, obj, NULL, TO_ROOM);
		  act ("You clasp $p\x1B[0m around your neck.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_NECK_2);
		  return;
		}
	      bug ("Wear_obj: no free neck.", 0);
	      send_to_char ("You already wear two neck items.\n\r", ch);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_FLOATING))
	{
	  if (loc == 0 || loc == ITEM_FLOATING)
	    {
	      if (get_eq_char (ch, WEAR_FLOATING_1) != NULL
		  && get_eq_char (ch, WEAR_FLOATING_2) != NULL
		  && !remove_obj (ch, WEAR_FLOATING_1, fReplace)
		  && !remove_obj (ch, WEAR_FLOATING_2, fReplace))
		return;
	      if (get_eq_char (ch, WEAR_FLOATING_1) == NULL)
		{
		  act ("$p starts floating around $n's head.", ch, obj, NULL, TO_ROOM);
		  act ("Your $p starts floating nearby your head.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_FLOATING_1);
		  return;
		}
	      if (get_eq_char (ch, WEAR_FLOATING_2) == NULL)
		{
		  act ("$p starts floating around $n's head.", ch, obj, NULL, TO_ROOM);
		  act ("Your $p starts floating nearby your head.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_FLOATING_2);
		  return;
		}
	      bug ("Wear_obj: no free floater.", 0);
	      send_to_char ("You already have two items floating nearby.\n\r", ch);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_ELBOW))
	{
	  if (loc == 0 || loc == ITEM_WEAR_ELBOW)
	    {
	      if (get_eq_char (ch, WEAR_ELBOW_1) != NULL
		  && get_eq_char (ch, WEAR_ELBOW_2) != NULL
		  && !remove_obj (ch, WEAR_ELBOW_1, fReplace)
		  && !remove_obj (ch, WEAR_ELBOW_2, fReplace))
		return;
	      if (get_eq_char (ch, WEAR_ELBOW_1) == NULL)
		{
		  act ("$n straps $p\x1B[0m onto $s elbow.", ch, obj, NULL, TO_ROOM);
		  act ("You strap $p\x1B[0m onto your elbow.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_ELBOW_1);
		  return;
		}
	      if (get_eq_char (ch, WEAR_ELBOW_2) == NULL)
		{
		  act ("$n strap $p\x1B[0m onto $s elbow.", ch, obj, NULL, TO_ROOM);
		  act ("You strap $p\x1B[0m onto your elbow.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_ELBOW_2);
		  return;
		}
	      bug ("Wear_obj: no free elbow.", 0);
	      send_to_char ("You already wear an item on each elbow.\n\r", ch);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_KNEE))
	{
	  if (loc == 0 || loc == ITEM_WEAR_KNEE)
	    {
	      if (get_eq_char (ch, WEAR_KNEE_1) != NULL
		  && get_eq_char (ch, WEAR_KNEE_2) != NULL
		  && !remove_obj (ch, WEAR_KNEE_1, fReplace)
		  && !remove_obj (ch, WEAR_KNEE_2, fReplace))
		return;
	      if (get_eq_char (ch, WEAR_KNEE_1) == NULL)
		{
		  act ("$n straps $p\x1B[0m onto $s knee.", ch, obj, NULL, TO_ROOM);
		  act ("You strap $p\x1B[0m onto your knee.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_KNEE_1);
		  return;
		}
	      if (get_eq_char (ch, WEAR_KNEE_2) == NULL)
		{
		  act ("$n strap $p\x1B[0m onto $s knee.", ch, obj, NULL, TO_ROOM);
		  act ("You strap $p\x1B[0m onto your knee.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_KNEE_2);
		  return;
		}
	      bug ("Wear_obj: no free knee.", 0);
	      send_to_char ("You already wear an item on each knee.\n\r", ch);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_BODY))
	{
	  if (loc == 0 || loc == ITEM_WEAR_BODY)
	    {
	      if (!remove_obj (ch, WEAR_BODY, fReplace))
		return;
	      act ("$n wears $p\x1B[0m on $s body.", ch, obj, NULL, TO_ROOM);
	      act ("You wear $p\x1B[0m on your body.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_BODY);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_HEAD))
	{
	  if (loc == 0 || loc == ITEM_WEAR_HEAD)
	    {
	      if (!remove_obj (ch, WEAR_HEAD, fReplace))
		return;
	      act ("$n dons $p\x1B[0m on $s head.", ch, obj, NULL, TO_ROOM);
	      act ("You don $p\x1B[0m on your head.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_HEAD);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_FACE))
	{
	  if (loc == 0 || loc == ITEM_WEAR_FACE)
	    {
	      if (!remove_obj (ch, WEAR_FACE, fReplace))
		return;
	      act ("$n straps $p\x1B[0m onto $s face.", ch, obj, NULL, TO_ROOM);
	      act ("You strap $p onto your face.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_FACE);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_LEGS))
	{
	  if (loc == 0 || loc == ITEM_WEAR_LEGS)
	    {
	      if (!remove_obj (ch, WEAR_LEGS, fReplace))
		return;
	      act ("$n wears $p\x1B[0m on $s legs.", ch, obj, NULL, TO_ROOM);
	      act ("You wear $p\x1B[0m on your legs.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_LEGS);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_FEET))
	{
	  if (loc == 0 || loc == ITEM_WEAR_FEET)
	    {
	      if (!remove_obj (ch, WEAR_FEET, fReplace))
		return;
	      act ("$n slips $s feet into $p\x1B[0m.", ch, obj, NULL, TO_ROOM);
	      act ("You slip your feet into $p\x1B[0m.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_FEET);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_HANDS))
	{
	  if (loc == 0 || loc == ITEM_WEAR_HANDS)
	    {
	      if (!remove_obj (ch, WEAR_HANDS, fReplace))
		return;
	      act ("$n wears $p\x1B[0m on $s hands.", ch, obj, NULL, TO_ROOM);
	      act ("You wear $p\x1B[0m on your hands.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_HANDS);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_ARMS))
	{
	  if (loc == 0 || loc == ITEM_WEAR_ARMS)
	    {
	      if (!remove_obj (ch, WEAR_ARMS, fReplace))
		return;
	      act ("$n wears $p\x1B[0m on $s arms.", ch, obj, NULL, TO_ROOM);
	      act ("You wear $p\x1B[0m on your arms.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_ARMS);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_ABOUT))
	{
	  if (loc == 0 || loc == ITEM_WEAR_ABOUT)
	    {
	      if (!remove_obj (ch, WEAR_ABOUT, fReplace))
		return;
	      act ("$n wears $p\x1B[0m about $s body.", ch, obj, NULL, TO_ROOM);
	      act ("You wear $p\x1B[0m about your body.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_ABOUT);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_EYES))
	{
	  if (loc == 0 || loc == ITEM_WEAR_EYES)
	    {
	      if (!remove_obj (ch, WEAR_EYES, fReplace))
		return;
	      act ("$n wears $p\x1B[0m on $s eyes.", ch, obj, NULL, TO_ROOM);
	      act ("You wear $p\x1B[0m on your eyes.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_EYES);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_BACK))
	{
	  if (loc == 0 || loc == ITEM_WEAR_BACK)
	    {
	      if (!remove_obj (ch, WEAR_BACK, fReplace))
		return;
	      act ("$n wears $p\x1B[0m on $s back.", ch, obj, NULL, TO_ROOM);
	      act ("You wear $p\x1B[0m on your back.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_BACK);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_WAIST))
	{
	  if (loc == 0 || loc == ITEM_WEAR_WAIST)
	    {
	      if (!remove_obj (ch, WEAR_WAIST, fReplace))
		return;
	      act ("$n fastens $p\x1B[0m around $s waist.", ch, obj, NULL, TO_ROOM);
	      act ("You fasten $p\x1B[0m around your waist.", ch, obj, NULL, TO_CHAR);
	      equip_char (ch, obj, WEAR_WAIST);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_EAR1))
	{
	  if (loc == 0 || loc == ITEM_WEAR_EAR1)
	    {
	      if ((get_eq_char (ch, WEAR_EAR_1) != NULL) && (get_eq_char (ch, WEAR_EAR_2) != NULL))
		return;
	      if (get_eq_char (ch, WEAR_EAR_1) == NULL)
		{
		  act ("$n wears $p\x1B[0m in $s right ear.", ch, obj, NULL, TO_ROOM);
		  act ("You wear $p\x1B[0m in your right ear.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_EAR_1);
		  return;
		}
	      if (get_eq_char (ch, WEAR_EAR_2) == NULL)
		{
		  act ("$n wears $p\x1B[0m in $s left ear.", ch, obj, NULL, TO_ROOM);
		  act ("You wear $p\x1B[0m in your left ear.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_EAR_2);
		  return;
		}
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_ANKLE))
	{
	  if (loc == 0 || loc == ITEM_WEAR_ANKLE)
	    {
	      if ((get_eq_char (ch, WEAR_ANKLE_1) != NULL) && (get_eq_char (ch, WEAR_ANKLE_2) != NULL))
		return;
	      if (get_eq_char (ch, WEAR_ANKLE_1) == NULL)
		{
		  act ("$n wears $p\x1B[0m around $s right ankle.", ch, obj, NULL, TO_ROOM);
		  act ("You wear $p\x1B[0m around your right ankle.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_ANKLE_1);
		  return;
		}
	      if (get_eq_char (ch, WEAR_ANKLE_2) == NULL)
		{
		  act ("$n wears $p\x1B[0m around $s left ankle.", ch, obj, NULL, TO_ROOM);
		  act ("You wear $p\x1B[0m around your left ankle.", ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_ANKLE_2);
		  return;
		}
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_WRIST))
	{
	  if (loc == 0 || loc == ITEM_WEAR_WRIST)
	    {
	      if (get_eq_char (ch, WEAR_WRIST_1) != NULL
		  && get_eq_char (ch, WEAR_WRIST_2) != NULL
		  && !remove_obj (ch, WEAR_WRIST_1, fReplace)
		  && !remove_obj (ch, WEAR_WRIST_2, fReplace))
		return;
	      if (get_eq_char (ch, WEAR_WRIST_1) == NULL)
		{
		  act ("$n wears $p\x1B[0m around $s left wrist.",
		       ch, obj, NULL, TO_ROOM);
		  act ("You wear $p\x1B[0m around your left wrist.",
		       ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_WRIST_1);
		  return;
		}
	      if (get_eq_char (ch, WEAR_WRIST_2) == NULL)
		{
		  act ("$n wears $p\x1B[0m around $s right wrist.",
		       ch, obj, NULL, TO_ROOM);
		  act ("You wear $p\x1B[0m around your right wrist.",
		       ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_WRIST_2);
		  return;
		}
	      bug ("Wear_obj: no free wrist.", 0);
	      send_to_char ("You already wear two wrist items.\n\r", ch);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_TATTOO))
	{
	  if (loc == 0 || loc == ITEM_WEAR_TATTOO)
	    {
	      if (get_eq_char (ch, WEAR_TATTOO) != NULL
		  && !remove_obj (ch, WEAR_TATTOO, fReplace))
		return;
	      if (get_eq_char (ch, WEAR_TATTOO) == NULL)
		{
		  act ("$n cuts $p\x1B[0m into $s arm.",
		       ch, obj, NULL, TO_ROOM);
		  act ("You cut $p\x1B[0m into your arm.",
		       ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_TATTOO);
		  return;
		}
	      bug ("Wear_obj: no free tattoo.", 0);
	      send_to_char ("You already wear a tattoo.\n\r", ch);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_SHOULDER))
	{
	  if (loc == 0 || loc == ITEM_WEAR_SHOULDER)
	    {
	      if (get_eq_char (ch, WEAR_SHOULDER_1) != NULL
		  && get_eq_char (ch, WEAR_SHOULDER_2) != NULL
		  && !remove_obj (ch, WEAR_SHOULDER_1, fReplace)
		  && !remove_obj (ch, WEAR_SHOULDER_2, fReplace))
		return;
	      if (get_eq_char (ch, WEAR_SHOULDER_1) == NULL)
		{
		  act ("$n slings $p\x1B[0m over $s left shoulder.",
		       ch, obj, NULL, TO_ROOM);
		  act ("You sling $p\x1B[0m over your left shoulder.",
		       ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_SHOULDER_1);
		  return;
		}
	      if (get_eq_char (ch, WEAR_SHOULDER_2) == NULL)
		{
		  act ("$n slings $p\x1B[0m over $s right shoulder.",
		       ch, obj, NULL, TO_ROOM);
		  act ("You sling $p\x1B[0m over your right shoulder.",
		       ch, obj, NULL, TO_CHAR);
		  equip_char (ch, obj, WEAR_SHOULDER_2);
		  return;
		}
	      bug ("Wear_obj: no free shoulder.", 0);
	      send_to_char ("You already wear two items on your shoulders.\n\r", ch);
	      return;
	    }
	}
      if (CAN_WEAR (obj, ITEM_WEAR_SHIELD))
	{
          if (IS_PLAYER(ch) && is_member(ch, GUILD_MONK))
      	  {
            act ("Monks of the divine order need to keep their arms free.", ch, NULL, NULL, TO_CHAR);
            return;
	  }
	  if (!remove_obj (ch, WEAR_SHIELD, fReplace))
	    return;
	  obj1 = get_eq_char (ch, WEAR_HOLD_1);
	  obj2 = get_eq_char (ch, WEAR_HOLD_2);
          if ((obj2 != NULL && (IS_OBJ_STAT(obj2, ITEM_POLEARM) ||IS_OBJ_STAT(obj2, ITEM_RANGED)))||
             (obj1 != NULL && (IS_OBJ_STAT(obj1, ITEM_POLEARM) || IS_OBJ_STAT(obj1, ITEM_RANGED))))
           {
             send_to_char("You may not wear a shield while you wield a polearm.\n\r", ch);
             return;
           }
	  if (obj->pIndexData->item_type == ITEM_ARMOR && (obj1 != NULL && obj1->pIndexData->item_type == ITEM_WEAPON) &&
	      (obj2 != NULL && obj2->pIndexData->item_type == ITEM_WEAPON))
	    {
	      send_to_char ("You can't use a shield while wielding two weapons!\n\r", ch);
	      return;
	    }
	  act ("$n straps $p\x1B[0m to $s arm.", ch, obj, NULL, TO_ROOM);
	  act ("You strap $p\x1B[0m to your arm.", ch, obj, NULL, TO_CHAR);
	  equip_char (ch, obj, WEAR_SHIELD);
	  return;
	}

      if ((CAN_WEAR (obj, ITEM_HOLD) || CAN_WEAR (obj, ITEM_WIELD))
      /*&& hand_empty( ch ) != WEAR_NONE */ )
	{
          if (IS_PLAYER(ch) && is_member(ch, GUILD_MONK) && obj->pIndexData->item_type == ITEM_WEAPON)
      	  {
            act ("Monks of the divine order may not use such brutal objects.", ch, NULL, NULL, TO_CHAR);
            return;
	  }
	  if (hand_empty (ch) == WEAR_NONE)
	    {
	      act ("Your hands are full! Remove something first.", ch, NULL, NULL, TO_CHAR);
	      return;
	    }
	  obj1 = get_eq_char (ch, WEAR_HOLD_1);
	  obj2 = get_eq_char (ch, WEAR_HOLD_2);
	  obj3 = get_eq_char (ch, WEAR_SHIELD);
         if ((obj2 != NULL && (IS_OBJ_STAT(obj2, ITEM_POLEARM) || IS_OBJ_STAT(obj2, ITEM_RANGED))) ||
             (obj1 != NULL && (IS_OBJ_STAT(obj1, ITEM_POLEARM) || IS_OBJ_STAT(obj1, ITEM_RANGED))))
           {
             send_to_char("You may not hold anything else while you wield a polearm or ranged weapon.\n\r", ch);
             return;
           }

          
	 if ((IS_OBJ_STAT(obj, ITEM_POLEARM) || IS_OBJ_STAT(obj, ITEM_RANGED)) &&
          (obj1 != NULL || obj2 != NULL || obj3 != NULL))
           {
             send_to_char("You may not wield a polearmor ranged weapon while you have anything in either hand, or while you wear a shield.\n\r", ch);
             return;
           }
	  if (obj3 && obj->pIndexData->item_type == ITEM_WEAPON &&
	    ((obj1 != NULL && obj1->pIndexData->item_type == ITEM_WEAPON) ||
	     (obj2 != NULL && obj2->pIndexData->item_type == ITEM_WEAPON)))
	    {
	      send_to_char ("You can't wield two weapons while using a shield!\n\r", ch);
	      return;
	    }
	  if (IS_PLAYER(ch))
	    {
	      int canwield = (get_curr_str(ch)*2 / 3);
	      if (IS_AUGMENTED(ch, AUG_ARM_POWER)) canwield +=2;
	      if (IS_AUGMENTED(ch, AUG_HAND_POWER)) canwield +=1;
	      if (IS_AUGMENTED(ch, AUG_BODY_POWER)) canwield +=2;
	      if   (obj->pIndexData->weight > canwield*WGT_MULT){
		act ("Unfortunately, $p is too heavy for you to use!", ch, obj, NULL, TO_CHAR);
                return;
		  }
	    }
	  act ("$n holds $p\x1B[0m in $s hand.", ch, obj, NULL, TO_ROOM);
	  act ("You hold $p\x1B[0m in your hand.", ch, obj, NULL, TO_CHAR);
	  equip_char (ch, obj, hand_empty (ch));
	  return;
	}
      if (fReplace)
	send_to_char ("You can't wear, wield, or hold that there.\n\r", ch);
      return;
      /*REMOVE this next 
         }
         if you remove the check for shopkeeper thingy */
    }

  return;
}
void 
do_swap (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("swap", do_exchange, POSITION_RESTING, 0, LOG_NORMAL, "This command exchanges what you have in your primary and secondary hand.")
    return;
}

void 
do_exchange (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *objh1;
  SINGLE_OBJECT *objh2;
  bool hone;
  bool htwo;
  DEFINE_COMMAND ("exchange", do_exchange, POSITION_FIGHTING, 0, LOG_NORMAL, "This command exchanges what you have in your primary and secondary hand.")

    hone = FALSE;
  htwo = FALSE;
  if ((objh1 = ch->hold1) != NULL)
    hone = TRUE;
  if ((objh2 = ch->hold2) != NULL)
    htwo = TRUE;
  if ((!hone) && (!htwo))
    {
      act ("You aren't holding anything in your hands!", ch, NULL, NULL, TO_CHAR);
      return;
    }

  if ((!hone) && (htwo))
    {
      objh2->wear_loc = WEAR_HOLD_1;
      act ("The object in your secondary hand is now in your primary hand.", ch, NULL, NULL, TO_CHAR);
      ch->hold2 = NULL;
      ch->hold1 = objh2;
      return;
    }

  if ((hone) && (!htwo))
    {
      objh1->wear_loc = WEAR_HOLD_2;
      act ("The object in your primary hand is now in your secondary hand.", ch, NULL, NULL, TO_CHAR);
      ch->hold1 = NULL;
      ch->hold2 = objh1;
      return;
    }

  if ((hone) && (htwo))
    {
      objh1->wear_loc = WEAR_HOLD_2;
      objh2->wear_loc = WEAR_HOLD_1;
      act ("You now hold $p\x1B[0m in your primary hand.", ch, objh2, NULL, TO_CHAR);
      act ("You now hold $p\x1B[0m in your secondary hand.", ch, objh1, NULL, TO_CHAR);
      ch->hold1 = objh2;
      ch->hold2 = objh1;
      return;

    }

  return;
}

void 
do_wield (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("wield", do_wear, POSITION_STANDING, 0, LOG_NORMAL, "Allows you to wear/wield or hold an object.")
    return;
}

void 
do_hold (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("hold", do_wear, POSITION_STANDING, 0, LOG_NORMAL, "Allows you to wear/wield or hold an object.")
    return;
}

void 
do_wear (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND ("wear", do_wear, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to wear/wield or hold an object.")

  argy = one_argy (argy, arg1);
  one_argy (argy, arg2);
  if (arg1[0] == '\0')
    {
      send_to_char ("Please specify what you want to wear/wield/hold.\n\r", ch);
      return;
    }


  if (!str_cmp (arg1, "all"))
    {
      SINGLE_OBJECT *obj_next;
      if (FIGHTING (ch) != NULL && !pow.equip_in_combat)
	{
	  send_to_char ("Not while you're fighting; you'd get creamed!\n\r", ch);
	  return;
	}
      for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
	  obj_next = obj->next_content;
	  if ((CAN_WEAR (obj, ITEM_HOLD)))
	    if (hand_empty (ch) == WEAR_NONE)
	      continue;
	  if ((obj->wear_loc == -1 || obj->wear_loc == WEAR_NONE	/*|| 
									   obj->wear_loc == WEAR_HOLD_1 ||
									   obj->wear_loc == WEAR_HOLD_2 */ 
		) && can_see_obj (ch, obj))
	    wear_obj (ch, obj, FALSE, 0);
	}
      return;
    }
  else
    {
/*held */
      if ((obj = get_obj_inv (ch, arg1)) == NULL)
	{
	  send_to_char ("You do not have that item.\n\r", ch);
	  return;
	}

      if ((CAN_WEAR (obj, ITEM_HOLD)))
	if (hand_empty (ch) == WEAR_NONE)
	  {
	    send_to_char ("Your hands are already full!\n\r", ch);
	    return;
	  }

      if (FIGHTING (ch) != NULL && !pow.equip_in_combat && !CAN_WEAR (obj, ITEM_HOLD))
	{
	  send_to_char ("Not while you're fighting; you'd get creamed!\n\r", ch);
	  return;
	}

      if (obj->wear_loc == -1)
	{
	  wear_obj (ch, obj, TRUE, wear_name_bit (arg2));
	}
    }

  return;
}
void 
do_sheath (CHAR_DATA * ch, char *argy)
{
  char arg1[STD_LENGTH];
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND ("sheath", do_sheath, POSITION_STANDING, 0, LOG_NORMAL, "Allows you to sheath a weapon in your belt.")

    argy = one_argy (argy, arg1);
/*get_obj_held */
  if ((obj = get_obj_inv (ch, arg1)) == NULL)
    {
/*item_held */
      if (get_item_held (ch, ITEM_WEAPON) != NULL)
	sheath_obj (ch, get_item_held (ch, ITEM_WEAPON));
      if (get_item_held (ch, ITEM_WEAPON) != NULL)
	sheath_obj (ch, get_item_held (ch, ITEM_WEAPON));
      return;
    }

  if (!sheath_obj (ch, obj))
    send_to_char ("You can't do that!\n\r", ch);
  return;
}
void 
do_draw (CHAR_DATA * ch, char *argy)
{
  char arg1[STD_LENGTH];
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj1; 
  SINGLE_OBJECT *obj2; 
  
  DEFINE_COMMAND ("draw", do_draw, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to draw a weapon from your belt.")
    
    obj2 = get_item_held(ch, ITEM_WEAPON);
  obj1 = get_eq_char(ch, WEAR_SHIELD);
  if ((obj2 != NULL) && (IS_OBJ_STAT(obj2, ITEM_POLEARM) || IS_OBJ_STAT(obj2, ITEM_RANGED)))
    {
      send_to_char("You cannot draw a weapon while wielding a polearm.\n\r", ch);
      return;
    }
  argy = one_argy (argy, arg1);
  if ((obj = get_obj_wear (ch, arg1)) == NULL)
    {
      send_to_char ("You do not have that item in your belt.\n\r", ch);
      return;
    }
  if (obj->pIndexData->item_type == ITEM_WEAPON && obj2 != NULL && obj1 != NULL)
    {
      send_to_char("You cannot wear two weapons and a shield!\n\r", ch);
      return;
    }
  if (!draw_obj (ch, obj))
    send_to_char ("You can't do that!\n\r", ch);
  return;
}

void 
do_remove (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND ("remove", do_remove, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to remove a weapon/armor/etc... that is worn.")

    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Remove what?\n\r", ch);
      return;
    }
  if (!str_cmp (argy, "all"))
    {
      SINGLE_OBJECT *obj_next;
      if (!pow.remove_while_fighting && FIGHTING (ch))
	{
	  send_to_char ("No way; you'd get creamed!\n\r", ch);
	  return;
	}
      for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
	  obj_next = obj->next_content;
	  /* if ( (obj->wear_loc == -1 || obj->wear_loc == WEAR_NONE || 
	     obj->wear_loc == WEAR_HOLD_1 ||
	     obj->wear_loc == WEAR_HOLD_2) && can_see_obj( ch, obj ) )
	     wear_obj( ch, obj, FALSE, 0 ); */
	  if (obj->wear_loc != WEAR_NONE && obj->wear_loc != -1)
	    remove_obj (ch, obj->wear_loc, TRUE);
	}
      if (ch->move > ch->max_move)
	MAXMOVE(ch);
      return;
    }

  if ((obj = get_obj_wear (ch, arg)) == NULL)
    {
      send_to_char ("You do not have that item.\n\r", ch);
      return;
    }

  if (!pow.remove_while_fighting && obj->wear_loc != WEAR_HOLD_1 &&
      obj->wear_loc != WEAR_HOLD_2 && FIGHTING (ch))
    {
      send_to_char ("No way; you'd get creamed!\n\r", ch);
      return;
    }

  if (obj->wear_loc != -1)
    {
      remove_obj (ch, obj->wear_loc, TRUE);
    }
  if (ch->move > ch->max_move)
    MAXMOVE(ch);
  if (ch->hit > ch->max_hit)
    ch->hit = ch->max_hit;
  return;
}

void 
do_bury (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("bury", do_sacrifice, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to sacrifice something to the gods... helps clean up the game.")
    return;
}

void 
do_sacrifice (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  const char *god;
  int n_god;
  char buf[STD_LENGTH];

  DEFINE_COMMAND ("sacrifice", do_sacrifice, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to sacrifice something to the gods... helps clean up the game.")

    if (argy == "" || argy[0] == '\0')
    return;
  one_argy (argy, arg);
  if (arg[0] == '\0' || !str_cmp (arg, NAME (ch)))
    {
      act ("$n offers $mself to the gods, who decline.",
	   ch, NULL, NULL, TO_ROOM);
      send_to_char (
	    "The gods appreciate your self-sacrifice, but refuse.\n\r", ch);
      return;
    }

  obj = get_obj_list (ch, arg, ch->in_room->more->contents);
  if (obj == NULL)
    {
      if (get_obj_inv (ch, arg) != NULL)
	{
	  send_to_char ("The object must be lying on the ground in order to sacrifice it.\n\r", ch);
	  return;
	}
      send_to_char ("You don't see that object here to sacrifice.\n\r", ch);
      return;
    }

  if (!CAN_WEAR (obj, ITEM_TAKE))
    {
      act ("$p\x1B[0m is not a sacrificable object.", ch, obj, 0, TO_CHAR);
      return;
    }

  if (obj->pIndexData->item_type == ITEM_BOAT)
    {
      send_to_char ("You cannot sacrifice something that large!\n\r", ch);
      return;
    }

  n_god = number_range (0, 4);
  god = pow.god_names[n_god];

//TEMPORARY TO TRY TO LET CASTERS EXP
//Lowered Wisdom Requirement - Sabelis 
  if (ch->pcdata->no_quit_pk < 5 && get_curr_int(ch) > 19 && get_curr_wis(ch) > 19
      && obj->pIndexData->item_type == ITEM_CORPSE_NPC)
  {
    int count;
    for (count = 0; count < 8; count++)
    {
      mana_gain(ch, TRUE);
    }
  }
  sprintf (buf, "$n sacrifices $p\x1B[0m to \x1B[1m%s\x1B[0m.", god);
  act (buf, ch, obj, NULL, TO_ROOM);
  sprintf (buf, "You sacrifice $p\x1B[0m to \x1B[1m%s\x1B[0m.", god);
  act (buf, ch, obj, NULL, TO_CHAR);
  if (LEVEL (ch) < 9 && obj->pIndexData->item_type == ITEM_CORPSE_NPC)
    {
      send_to_char ("You recieve a copper coin as a gift from the gods for this sacrifice!\n\r", ch);
      ch->copper++;
    }

  if (obj->carried_by != NULL)
    obj_from (obj);
  free_it (obj);
  return;
}

void 
do_quaff (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  I_POTION *pt;
  DEFINE_COMMAND ("quaff", do_quaff, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to quaff a potion.")

    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Quaff what?\n\r", ch);
      return;
    }

  if ((obj = get_obj_inv (ch, arg)) == NULL)
    {
      send_to_char ("You do not have that potion.\n\r", ch);
      return;
    }

  if (obj->pIndexData->item_type != ITEM_POTION)
    {
      send_to_char ("You can quaff only potions.\n\r", ch);
      return;
    }
  pt = (I_POTION *) obj->more;
  act ("$n quaffs $p\x1B[0m.", ch, obj, NULL, TO_ROOM);
  act ("You quaff $p\x1B[0m.", ch, obj, NULL, TO_CHAR);
  if (pt->spells[0] > 0)
    obj_cast_spell (pt->spells[0], pt->spell_level, ch, ch, NULL);
  if (pt->spells[1] > 0)
    obj_cast_spell (pt->spells[1], pt->spell_level, ch, ch, NULL);
  if (pt->spells[2] > 0)
    obj_cast_spell (pt->spells[2], pt->spell_level, ch, ch, NULL);
  obj_from (obj);
  free_it (obj);
  return;
}

void 
do_read (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("read", do_look, POSITION_RESTING, 0, LOG_NORMAL, "Same as look.")
    return;
}

void 
do_recite (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  CHAR_DATA *victim;
  I_POTION *sc;
  SINGLE_OBJECT *scroll;
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND ("recite", do_recite, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to recite a scroll.")

    argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  if ((scroll = get_obj_inv (ch, arg1)) == NULL)
    {
      send_to_char ("You do not have that scroll.\n\r", ch);
      return;
    }

  if (scroll->pIndexData->item_type != ITEM_SCROLL)
    {
      send_to_char ("You can recite only scrolls.\n\r", ch);
      return;
    }
  sc = (I_POTION *) scroll->more;
  obj = NULL;
  if (arg2[0] == '\0')
    {
      victim = ch;
    }

  else
    {
      if ((victim = get_char_room (ch, arg2)) == NULL
	  && (obj = get_obj_here (ch, arg2, SEARCH_INV_FIRST)) == NULL)
	{
	  send_to_char ("You can't find it.\n\r", ch);
	  return;
	}
    }

  act ("$n recites $p\x1B[0m.", ch, scroll, NULL, TO_ROOM);
  act ("You recite $p\x1B[0m.", ch, scroll, NULL, TO_CHAR);
  if (sc->spells[0] > 0)
    obj_cast_spell (sc->spells[0], sc->spell_level, ch, victim, obj);
  if (sc->spells[1] > 0)
    obj_cast_spell (sc->spells[1], sc->spell_level, ch, victim, obj);
  if (sc->spells[2] > 0)
    obj_cast_spell (sc->spells[2], sc->spell_level, ch, victim, obj);
  free_it (scroll);
  return;
}
void 
do_brandish (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  SINGLE_OBJECT *staff;
  SPELL_DATA *spell;
  I_WAND *stf;
  DEFINE_COMMAND ("brandish", do_brandish, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to brandish a staff.")

    if ((staff = get_item_held (ch, ITEM_STAFF)) == NULL)
    {
      send_to_char ("You don't have the staff in your hand.\n\r", ch);
      return;
    }
  stf = (I_WAND *) staff->more;
  if ((spell = skill_lookup (NULL, stf->spells[0])) == NULL)
    {
      bug ("Do_brandish: bad sn %d.", 9999);
      return;
    }

  WAIT_STATE (ch, 2 * PULSE_VIOLENCE);
  if (stf->current_charges > 0)
    {
      act ("$n raises $p\x1B[0m into the air and utters some mystical words under $s breath.",
	   ch, staff, NULL, TO_ROOM);
      act ("You brandish $p\x1B[0m.", ch, staff, NULL, TO_CHAR);
      for (vch = ch->in_room->more->people; vch; vch = vch_next)
	{
	  vch_next = vch->next_in_room;
	  switch (spell->spell_type)
	    {
	    default:
	      bug ("Do_brandish: bad target for sn %d.", spell->gsn);
	      return;
	    case TAR_IGNORE:
	      if (vch != ch)
		continue;
	      break;
	    case TAR_CHAR_OFFENSIVE:
	      if (IS_MOB (ch) ? IS_MOB (vch) : IS_PLAYER (vch))
		continue;
	      break;
	    case TAR_CHAR_DEFENSIVE:
	      if (IS_MOB (ch) ? IS_PLAYER (vch) : IS_MOB (vch))
		continue;
	      break;
	    case TAR_CHAR_SELF:
	      if (vch != ch)
		continue;
	      break;
	    }
	  if (stf->spells[0] > 0)
	    obj_cast_spell (stf->spells[0], stf->spell_level, ch, vch, NULL);
	}
    }

  if (--stf->current_charges <= 0)
    {
      act ("$n's $p\x1B[0m blazes bright and is gone.", ch, staff, NULL, TO_ROOM);
      act ("Your $p\x1B[0m blazes bright and is gone.", ch, staff, NULL, TO_CHAR);
      free_it (staff);
    }

  return;
}
void 
do_zap (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  SINGLE_OBJECT *wand;
  SINGLE_OBJECT *obj;
  SPELL_DATA *spell;
  I_WAND *wnd;
  DEFINE_COMMAND ("zap", do_zap, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to zap a wand.")

    one_argy (argy, arg);
  if (arg[0] == '\0' && FIGHTING (ch) == NULL)
    {
      send_to_char ("Zap whom or what?\n\r", ch);
      return;
    }

  if ((wand = get_item_held (ch, ITEM_WAND)) == NULL)
    {
      send_to_char ("You don't have a wand in your hand.\n\r", ch);
      return;
    }
  wnd = (I_WAND *) wand->more;
  if ((spell = skill_lookup (NULL, wnd->spells[0])) == NULL)
    {
      bug ("Do_zap: bad sn %d.", 0);
      return;
    }

  obj = NULL;
  if (arg[0] == '\0')
    {
      if (FIGHTING (ch) != NULL)
	{
	  victim = FIGHTING (ch);
	}
      else
	{
	  send_to_char ("Zap whom or what?\n\r", ch);
	  return;
	}
    }

  else
    {
      if ((victim = get_char_room (ch, arg)) == NULL
	  && (obj = get_obj_here (ch, arg, SEARCH_INV_FIRST)) == NULL)
	{
	  send_to_char ("You can't find it.\n\r", ch);
	  return;
	}
    }

  WAIT_STATE (ch, 2 * PULSE_VIOLENCE);
  if (wnd->current_charges > 0)
    {
      if (victim != NULL)
	{
	  act ("$n zaps $N with $p\x1B[0m.", ch, wand, victim, TO_ROOM);
	  act ("You zap $N with $p\x1B[0m.", ch, wand, victim, TO_CHAR);
	}
      else
	{
	  act ("$n zaps $P with $p\x1B[0m.", ch, wand, obj, TO_ROOM);
	  act ("You zap $P with $p\x1B[0m.", ch, wand, obj, TO_CHAR);
	}
      if (wnd->spells[0] > 0)
	obj_cast_spell (wnd->spells[0], wnd->spell_level, ch, victim, obj);
    }

  if (--wnd->current_charges <= 0)
    {
      act ("$n's $p\x1B[0m explodes into fragments.", ch, wand, NULL, TO_ROOM);
      act ("Your $p\x1B[0m explodes into fragments.", ch, wand, NULL, TO_CHAR);
      free_it (wand);
    }

  return;
}
void 
do_study (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *obj;
  SPELL_DATA *spell;
  I_BOOK *bk;
  DEFINE_COMMAND ("study", do_study, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to study a book.")

    if ((obj = get_obj_inv (ch, argy)) == NULL)
    {
      send_to_char ("You don't have that to study.\n\r", ch);
      return;
    }

  if (obj->pIndexData->item_type != ITEM_BOOK)
    {
      send_to_char ("That's not a book.. wouldn't want it to BURST into FLAMES! heh.\n\r", ch);
      return;
    }
  bk = (I_BOOK *) obj->more;
  if (bk->teaches == 0)
    {
      send_to_char ("You begin to read... Blah blah blah...\n\rYou soon discover that there is nothing you can learn from this book.\n\r", ch);
      return;
    }

  spell = skill_lookup (NULL, bk->teaches);
  if (spell == NULL)
    {
      send_to_char ("Some LAME builder created this book wrong - it doesn't do SHIT!\n\rHAHAHAHAHA!", ch);
      return;
    }
  
/*past this point the book FRIES.. */
  if (ch->pcdata->learned[(bk->teaches)] > 25)
    {
      send_to_char ("You read the book and a knowledge and understanding begins to\n\rmeld into your open mind... Unfortunately the book doesn't seem to\n\rteach you anything you don't already know! DOH!\n\r", ch);
    }

  else
    {
      char buf[STD_LENGTH];
      send_to_char ("You read the book and a knowledge and understanding begins to\n\rmeld into your open mind...\n\r", ch);
      sprintf (buf, "You now understand the very basics of %s!!\n\r", spell->spell_funky_name);
      send_to_char (buf, ch);
      ch->pcdata->learned[(bk->teaches)] = 26;
    }

  act ("$n reads $p\x1B[0m, and it bursts into flames!", ch, obj, NULL, TO_ROOM);
  act ("After you read $p\x1B[0m, it bursts into flames!", ch, obj, NULL, TO_CHAR);
  free_it (obj);
  return;
}

void 
do_steal (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  CHAR_DATA *victim;
  SINGLE_OBJECT *obj;
    DEFINE_COMMAND ("steal", do_steal, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to steal things from others.")
 
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  if (ch->pcdata->alignment == 0)
    {
      send_to_char ("Neutral eq whore?  Go join an align!\n\r", ch);
      return;
    }
  if (arg1[0] == '\0' || arg2[0] == '\0')
    {
      send_to_char ("Steal what from whom?\n\r", ch);
      return;
    }

  if (IS_PLAYER (ch) && (ch->fgt->in_battle || ch->fgt->challenge == 10))
    {
      send_to_char ("Not in the battleground or arena!\n\r", ch);
      return;
    }

  if ((victim = get_char_room (ch, arg2)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }

  if (LEVEL(victim) >= IMM_LEVEL)
    {
      send_to_char ("Forget it.\n\r", ch);
      return;
    }

  if (victim == ch)
    {
      send_to_char ("Like a dull sword, that's pointless.\n\r", ch);
      return;
    }
  if (victim->pcdata->alignment == 0)
    {
      send_to_char ("Stealing from Neuts?  Your kit must SUCK!!\n\r", ch);
      return;
    }
  if (ch->pcdata->alignment == victim->pcdata->alignment)
    {
      send_to_char ("You cannot steal from your own alignment.\n\r", ch);
      return;
    }

  /* ------------------------------------------------------ */
  /*  Do not allow stealing from the neutral alignment      */
  /* ------------------------------------------------------ */
  if (victim->pcdata->alignment == 0 && ch->pcdata->alignment != 0)
  {
    act("\x1B[1;32mA moral dilemma rages in your head!  Steal from the Protected!? *Tisk Tisk*\x1B[0m",ch,NULL,NULL,TO_CHAR);
    return;
  }
  if (ch->pcdata->alignment == 0 && victim->pcdata->alignment != 0)
  {
    act("\x1B[1;32mYour code of honor prohibits you from stealing from anyone other than your own kind.\x1B[0m",ch,NULL,NULL,TO_CHAR);
    return;
  }

  WAIT_STATE (ch, 200); 
  if (is_safe (ch, victim))
    {
      if (IS_PLAYER (ch))
	{
	  act ("\x1B[1;37mYou feel as if the gods are watching, and a hot pain spreads", ch, NULL, NULL, TO_CHAR);
	  act ("through your hand. OUCH! Perhaps you should not steal here...\x1B[0m", ch, NULL, NULL, TO_CHAR);
	}
      return;
    }
  
  else
    {
      if (victim->position == POSITION_FIGHTING 
	  || !is_member(ch, GUILD_THIEFG)
	  || !is_member(ch, GUILD_ROGUE)
	  || get_curr_dex(ch) < 28
	  || number_range(1,2) == 2
	  || (IS_PLAYER (ch) && ch->pcdata->learned[gsn_steal] < 60)
	  || (IS_MOB (ch) && (number_range (1, 20) < 19))
          || (IS_MOB (victim)))
	{ 
	  send_to_char ("Oops.\n\r", ch);
	  act ("$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT);
	  act ("$n tried to steal from $N.\n\r", ch, NULL, victim, TO_NOTVICT);
	  sprintf (buf, "%s is a bloody thief!", NAME (ch));
	  send_to_char (buf, victim);
	  if (IS_PLAYER (ch))
	    {
	      if (IS_MOB (victim))
		  multi_hit (victim, ch, TYPE_UNDEFINED);
              else
		  multi_hit (victim, ch, TYPE_UNDEFINED);

	    }
	  return;
	}
      else
	{
	  if ((obj = get_obj_inv (victim, arg1)) == NULL)
	    {
	      send_to_char ("You can't find it.\n\r", ch);
	      return;
	    }
	  if (obj->wear_loc != -1)
	    {
	      send_to_char ("You can't steal things people are WEARING or HOLDING!!!\n\r", ch);
	      return;
	    }
	  if (!can_drop_obj (ch, obj)
	      || IS_SET (obj->extra_flags, ITEM_INVENTORY))
	    {
	      send_to_char ("You can't pry it away.\n\r", ch);
	      return;
	    }
	  if (number_range (1, 5) == 2)
	    {
	      act ("$N's suddenly looks your way, and you whistle innocently.", ch, NULL, victim, TO_CHAR);
	      return;
	    }
	  
	  obj_from (obj);
	  obj_to (obj, ch);
	  if (obj->pIndexData->item_type == ITEM_VEHICLE && IS_PLAYER(ch))
	    {
	      I_VEHICLE *veh = (I_VEHICLE *) obj->more;
	      if (veh->sector_use == SECT_WATER_SWIM || 
		  veh->sector_use == SECT_WATER_NOSWIM)
		SET_BIT(ch->act, PLR_HAS_BOAT);
	    }
	  if (obj->pIndexData->item_type == ITEM_VEHICLE && IS_PLAYER(victim))
	    {
	      I_VEHICLE *veh = (I_VEHICLE *) obj->more;
	      if (veh->sector_use == SECT_WATER_SWIM || 
		  veh->sector_use == SECT_WATER_NOSWIM)
		REMOVE_BIT(victim->act, PLR_HAS_BOAT);
	    }
	  obj->wear_loc = -1;
	  send_to_char ("Stolen.\n\r", ch);
	  skill_gain (ch, gsn_steal, FALSE);
	}
      return;
    }

  return;
}

void 
do_light (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj2;
  I_LIGHT *lt;
  I_TOOL *tb;
  DEFINE_COMMAND ("light", do_light, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to light a light source.")

    argy = one_argy (argy, arg);
  if (arg[0] == '\0')
    obj = get_item_held (ch, ITEM_LIGHT);
  else
    obj = get_obj_inv (ch, arg);
  obj2 = get_obj_inv (ch, "tinderbox");
  if (obj == NULL)
    {
      send_to_char ("You don't have the light.\n\r", ch);
      return;
    }

  if (obj->pIndexData->item_type != ITEM_LIGHT)
    {
      send_to_char ("You can't light that!\n\r", ch);
      return;
    }
  lt = (I_LIGHT *) obj->more;
  if (IS_LIT (obj))
    {
      send_to_char ("It's lit already!\n\r", ch);
      return;
    }

  if (lt->light_now <= 0)
    {
      send_to_char ("It's out of fuel.\n\r", ch);
      return;
    }

  if (obj2 == NULL)
    {
      send_to_char ("You don't have anything to light it with!\n\r", ch);
      return;
    }
  tb = (I_TOOL *) obj2->more;
  if (use_tool (obj2, TOOL_TINDERBOX) && !IS_SET(lt->light_lit, LIGHT_LIT))
    {
      SET_BIT (lt->light_lit, LIGHT_LIT);
      act ("$n lights $p with $P.", ch, obj, obj2, TO_ROOM);
      act ("You light $p with $P.", ch, obj, obj2, TO_CHAR);
      if (obj->wear_loc != -1)
	{
	  if (ch->in_room->light < 0) ch->in_room->light = 0;
	  ch->in_room->light++;
	  if (IS_PLAYER(ch))
	    ch->pcdata->light++;
	}
      if (tb->uses == 1)
	{
	  send_to_char ("Your tinderbox is almost out of tinder, and the flint is starting to chip.\n\r", ch);
	}
      if (tb->uses == 0)
	{
	  send_to_char ("Your tinderbox is out of tinder and the flint is chipped...\n\rTime to buy a new one!\n\r", ch);
	  obj_from (obj2);
	  free_it (obj2);
	}
      return;
    }

  else
    {
      send_to_char ("You can't light it with that.\n\r", ch);
      return;
    }

  return;
}
void 
do_extinguish (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  SINGLE_OBJECT *obj;
  I_LIGHT *lt;
  DEFINE_COMMAND ("extinguish", do_extinguish, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to extinguish a light source.")

    argy = one_argy (argy, arg);
  if (arg[0] == '\0')
    obj = get_item_held (ch, ITEM_LIGHT);
/*held */
  else
    obj = get_obj_inv (ch, arg);
  if (obj == NULL)
    {
      send_to_char ("You don't have the light in your hands.\n\r", ch);
      return;
    }

  if (obj->pIndexData->item_type != ITEM_LIGHT)
    {
      send_to_char ("That's not a light source.\n\r", ch);
      return;
    }
  lt = (I_LIGHT *) obj->more;
  if (!IS_LIT (obj))
    {
      send_to_char ("It's extinguished already!\n\r", ch);
      return;
    }

  REMOVE_BIT (lt->light_lit, LIGHT_LIT);
  act ("$n extinguishes $p.", ch, obj, NULL, TO_ROOM);
  act ("You extinguish $p.", ch, obj, NULL, TO_CHAR);
  if (obj->wear_loc != -1)
    {
      ch->in_room->light--;
      if (ch->in_room->light < 0) ch->in_room->light = 0;
      if (IS_PLAYER(ch))
	{
	  ch->pcdata->light--;
	  if (ch->pcdata->light < 0) ch->pcdata->light = 0;
	}
    }
  return;
}
void 
do_butcher (CHAR_DATA * ch, char *argy)		/*By Arcane */
{
  SINGLE_OBJECT *obj, *weap, *steak;
  int count, x;
  char buf[80];
  DEFINE_COMMAND ("butcher", do_butcher, POSITION_STANDING, 0, LOG_NORMAL, "Allows you to butcher a corpse into steaks.  Based on your butcher skill.")

    if (argy[0] == '\0')
    {
      send_to_char ("What do you want to butcher?\n\r", ch);
      return;
    }

  if ((obj = get_obj_here (ch, argy, SEARCH_ROOM_FIRST)) == NULL)
    {
      send_to_char ("That's not here.\n\r", ch);
      return;
    }

  if (obj->pIndexData->item_type != ITEM_CORPSE_NPC)
    {
      send_to_char ("You can't butcher that!\n\r", ch);
      return;
    }

  if ((weap = get_item_held (ch, ITEM_WEAPON)) == NULL)
    {
      send_to_char ("You need something to cut with!\n\r", ch);
      return;
    }

  if (!IS_SET (weap->extra_flags, ITEM_PIERCE)
      && !IS_SET (weap->extra_flags, ITEM_SLASHING)
      && !IS_SET (weap->extra_flags, ITEM_HEAVY_SLASH)
      && !IS_SET (weap->extra_flags, ITEM_LIGHT_SLASH))
    {
      send_to_char ("You need something sharp to cut with!\n\r", ch);
      return;
    }

  if (number_range (0, 100) > ch->pcdata->learned[gsn_butcher])
    {
      send_to_char ("Your clumsy hands destroy the corpse.\n\r", ch);
      free_it (obj);
      return;
    }

  count = dice (1, 3);
  sprintf (buf, "You slice and dice the corpse into %d steak%s.\n\r",
	   count, (count > 1 ? "s" : ""));
  send_to_char (buf, ch);
  for (x = 0; x < count; x++)
    {
      steak = create_object (get_obj_index (110), 0);
      steak->timer = number_range (13, 20);
      if (dice (1, 100) == 13)
	{
	  /*sprintf(buf,"a greenish steak");
	     free_string( steak->short_descr );
	     steak->short_descr = str_dup( buf ); */
	  ((I_FOOD *) steak->more)->not_poison = 1;
	}
      obj_to (steak, ch->in_room);
    }

  skill_gain (ch, gsn_butcher, TRUE);
  free_it (obj);
  return;
}
