#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

void 
allocate_correct_datatype (SINGLE_OBJECT * obj, int *val)
{
  OBJ_PROTOTYPE *oi = obj->pIndexData;

  if (oi == NULL)
    {
      fprintf (stderr, "Error with null prototype in allocating object datatype.\n");
      exit (99);
    }
  switch (oi->item_type)
    {
    case ITEM_WEAPON:
      
      {
	I_WEAPON *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->firstdice = val[7];
	data->seconddice = val[8];
	data->attack_type = val[3];
	data->damage_p = (val[0] == 0 ? 100 : val[0]);
	data->strength = val[1];
	data->ammo_type = val[4];
	obj->more = data;
	break;
      }
    case ITEM_ARMOR:
      {
	I_ARMOR *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->warmth = val[9];
	data->protects_legs = val[0];
	data->protects_body = val[1];
	data->protects_head = val[2];
	data->protects_arms = val[3];
	data->max_condition = val[6];
	data->condition_now = val[7];
	obj->more = data;
	break;
      }
    case ITEM_DRINK_CON:
      {
	I_DRINK *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->max_liquid = val[1];
	data->liquid_now = val[0];
	data->liquid_type = val[2];
	data->not_poison = val[3];
	obj->more = data;
	break;
      }
    case ITEM_AMMO:
      {
	I_AMMO *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->firstdice = val[0];
	data->seconddice = val[1];
	data->ammo_type = val[4];
	obj->more = data;
	break;
      }
    case ITEM_LIGHT:
      {
	I_LIGHT *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->light_fillable = val[2];
	data->light_lit = val[3];
	data->light_now = val[0];
	data->max_light = val[1];
	obj->more = data;
	break;
      }
    


    case ITEM_TRAP:
      {
	I_TRAP *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->material = val[0];
	data->spell1 = val[1];
	data->spell2 = val[2];
	data->spell3 = val[3];
	obj->more = data;
	break;
      }
    case ITEM_TOOL:
      {
	I_TOOL *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->tool_type = val[0];
	data->uses = val[1];
	data->max_uses = val[2];
	obj->more = data;
	break;
      }
    
    case ITEM_VEHICLE:
      {
	I_VEHICLE *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->sector_use = val[0];
	obj->more = data;
	break;
      }
    case ITEM_MINERAL:
      {
	I_MINERAL *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->type = val[0];
	obj->more = data;
	break;
      }
    case ITEM_FOUNTAIN:
      {
	I_FOUNTAIN *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->liquid = val[0];
	data->spells[0] = val[1];
	data->spells[1] = val[2];
	data->spells[2] = val[3];
	obj->more = data;
	break;
      }
    case ITEM_POTION:
    case ITEM_PILL:
    case ITEM_SCROLL:
      {
	I_POTION *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->spell_level = val[0];
	data->spells[0] = val[1];
	data->spells[1] = val[2];
	data->spells[2] = val[3];
	obj->more = data;
	break;
      }
    case ITEM_FOOD:
      {
	I_FOOD *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->hours = val[0];
	data->not_poison = val[3];
	obj->more = data;
	break;
      }
    case ITEM_GEM:
      {
	I_GEM *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->gem_type = val[0];
	data->mana_now = val[1];
	data->max_mana = val[2];
	data->max_level = val[3];
	obj->more = data;
	break;
      }
    case ITEM_WAND:
    case ITEM_STAFF:
      {
	I_WAND *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->spell_level = val[0];
	data->max_charges = val[1];
	data->current_charges = val[2];
	data->spells[0] = val[3];
	data->spells[1] = val[4];
	data->spells[2] = val[5];
	obj->more = data;
	break;
      }
    case ITEM_FURNITURE:
      {
	I_FURNITURE *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->to_room_vnum = val[1];
	data->type = val[0];
	data->strip_possessions = val[5];
	obj->more = data;
	break;
      }
    case ITEM_CONTAINER:
    case ITEM_CORPSE_PC:
    case ITEM_CORPSE_NPC:
      
      {
	I_CONTAINER *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->max_weight = val[0];
	data->flags = val[1];
	data->key_vnum = val[2];
	data->money = val[8];
	data->reason_for_death[0] = '\0';
	data->looted_by[0] = '\0';
	data->looted_by_two[0] = '\0';
	if (number_range(1,6) == 3 && 
	    IS_SET(data->flags, CONT_LOCKED) &&
	    IS_SET(data->flags, CONT_CLOSED))
	  SET_BIT(data->flags, CONT_HASTRAP);
	obj->more = data;
	break;
      }
    case ITEM_CLIMBABLE:
      {
	I_CLIMBABLE *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->up_down = val[0];
	data->above = val[2];
	data->below = val[3];
	obj->more = data;
	break;
      }
    
    case ITEM_BOOK:
      {
	I_BOOK *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->teaches = val[0];
	obj->more = data;
	break;
      }
    case ITEM_REPELLANT:
      {
	I_SPRAY *data;
	int i;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->sprays = val[0];
	for (i = 1; i < 10; i++)
	  data->repels[i - 1] = val[i];
	obj->more = data;
	break;
      }
    case ITEM_HERB:
      {
	I_HERB *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->amount = val[0];
	data->potency = val[1];
	data->type = val[2];
	obj->more = data;
	break;
      }
    case ITEM_HIDE:
      {
	I_HIDE *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->type = val[0];
	obj->more = data;
	break;
      }
    case ITEM_WOOD:
      {
	I_WOOD *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->type = val[0];
	obj->more = data;
	break;
      }
    case ITEM_RAW:
      {
	I_RAW *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->type = val[0];
	obj->more = data;
	break;
      }
    case ITEM_CLOTH:
      {
	I_CLOTH *data;
	data = mem_alloc (sizeof (*data));
	data->tag = oi->item_type;
	data->type = val[0];
	obj->more = data;
      break;
      }
    default:
      obj->more = NULL;
      break;
    }
  return;
}




SINGLE_OBJECT *
new_obj (void)
{
  SINGLE_OBJECT *obj;
  if (obj_free == NULL)
    {
      obj = mem_alloc (sizeof (*obj));
      obj->data_type = K_OBJ;
    }
  else
    {
      obj = obj_free;
      obj_free = obj_free->next;
      obj->data_type = K_OBJ;
    }
  clear_data (obj);
  return obj;
}



void 
obj_to (SINGLE_OBJECT * obj, void *thing)
{
  ROOM_DATA *in_room;
  SINGLE_OBJECT *obj_t;
  CHAR_DATA *ch;
  char *t = thing;
  if (!obj || !thing)
    return;
  if (*t == K_OBJ)
    {
      obj_t = (SINGLE_OBJECT *) thing;
      if (obj->carried_by && obj->wear_loc != -1)
	unequip_char (obj->carried_by, obj);
      obj->next_content = obj_t->contains;
      obj_t->contains = obj;
      obj->in_obj = obj_t;
      obj->in_room = NULL;
      obj->carried_by = NULL;
      obj->wear_loc = WEAR_NONE;
      for (; obj_t != NULL; obj_t = obj_t->in_obj)
	{
	  if (obj_t->carried_by != NULL && IS_PLAYER (obj_t->carried_by))
	    {
	      obj_t->carried_by->pcdata->carry_number += get_obj_number (obj);
	      obj_t->carried_by->pcdata->carry_weight += get_obj_weight (obj);
	    }
	}
    }
  else if (*t == K_ROOM)
    {
      in_room = (ROOM_DATA *) thing;
      check_room_more (in_room);

     if (obj->pIndexData->item_type == ITEM_LIGHT && IS_LIT (obj))
	++in_room->light;
     if (IS_SET(obj->pIndexData->extra_flags, ITEM_COMMANDSCRIPT))
       in_room->command_objs++;
      obj->next_content = in_room->more->contents;
      in_room->more->contents = obj;
      obj->in_room = in_room;
      obj->carried_by = NULL;
      obj->in_obj = NULL;
      obj->wear_loc = WEAR_NONE;
      
      return;
      
    }
  else if (*t == K_CHAR)
    {
      ch = (CHAR_DATA *) thing;
      obj->next_content = ch->carrying;
      ch->carrying = obj;
      obj->carried_by = ch;
      obj->in_room = NULL;
      obj->in_obj = NULL;
      if (IS_PLAYER (ch))
	{
	  ch->pcdata->carry_number += get_obj_number (obj);
	  ch->pcdata->carry_weight += get_obj_weight (obj);
	  if (IS_SET(obj->pIndexData->extra_flags, ITEM_COMMANDSCRIPT))
	    ch->pcdata->command_objs++;
	}
     
    }
  return;
}



void 
obj_from (SINGLE_OBJECT * obj)
{
  CHAR_DATA *ch;
  ROOM_DATA *in_room;
  SINGLE_OBJECT *obj_frm;
  ch = obj->carried_by;
  in_room = obj->in_room;
  obj_frm = obj->in_obj;
  if (ch != NULL)
    {
      
      if (obj->wear_loc != WEAR_NONE)
	unequip_char (ch, obj);
      if (ch->carrying == obj)
	{
	  ch->carrying = obj->next_content;
	}
      else
	{
	  SINGLE_OBJECT *prev;
	  for (prev = ch->carrying; prev != NULL; prev = ((!prev->next_content) ? NULL : prev->next_content))
	    if (prev->next_content == obj)
	      {
		prev->next_content = obj->next_content;
		break;
	      }
	}
      obj->carried_by = NULL;
      obj->next_content = NULL;
      if (IS_PLAYER(ch))
	{
	  ch->pcdata->carry_number -= get_obj_number (obj);
	  ch->pcdata->carry_weight -= get_obj_weight (obj);
	  if (IS_SET(obj->pIndexData->extra_flags, ITEM_COMMANDSCRIPT))
	    ch->pcdata->command_objs--;
	}
    }
  else if (in_room != NULL)
    {
      if (obj == in_room->more->contents)
	{
	  in_room->more->contents = obj->next_content;
	}
      else
	{
	  SINGLE_OBJECT *prev;
	  for (prev = in_room->more->contents; prev; prev = prev->next_content)
	    if (prev->next_content == obj)
	      {
		prev->next_content = obj->next_content;
		break;
	      }
	}
      if (obj->pIndexData->item_type == ITEM_LIGHT && IS_LIT (obj))
	--obj->in_room->light;
       if (IS_SET(obj->pIndexData->extra_flags, ITEM_COMMANDSCRIPT))
	 in_room->command_objs--;
       if (in_room->command_objs < 0)
	 in_room->command_objs = 0;
      check_clear_more (obj->in_room);
      obj->in_room = NULL;
      obj->next_content = NULL;
    }
  else if (obj_frm != NULL)
    {
      if (obj == obj_frm->contains)
	{
	  obj_frm->contains = obj->next_content;
	}
      else
	{
	  SINGLE_OBJECT *prev;
	  for (prev = obj_frm->contains; prev; prev = prev->next_content)
	    if (prev->next_content == obj)
	      {
		prev->next_content = obj->next_content;
		break;
	      }
	}
      obj->next_content = NULL;
      obj->in_obj = NULL;
      for (; obj_frm != NULL; obj_frm = obj_frm->in_obj)
	if (obj_frm->carried_by != NULL && IS_PLAYER (obj_frm->carried_by))
	  {
	    obj_frm->carried_by->pcdata->carry_number -= get_obj_number (obj);
	    obj_frm->carried_by->pcdata->carry_weight -= get_obj_weight (obj);
	  }
    }
  return;
}
