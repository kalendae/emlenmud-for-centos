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

bool 
spec_barbarian (CHAR_DATA * ch)
{
  CHAR_DATA *victim;
  return FALSE;
  if (ch->position != POSITION_FIGHTING)
    return FALSE;
  /*  for (victim = ch->in_room->more->people; victim != NULL; victim = v_next)
    {
      v_next = victim->next_in_room;
      if (FIGHTING (victim) == ch && number_range (1, 2) == 1)
	break;
    }*/
  if (victim == NULL)
    return FALSE;
  if (ch->fgt && ch->fgt->ears > 9)
    return FALSE;
}

SPEC_FUNCTION *
spec_lookup (const char *name)
{
  if (!str_cmp (name, "spec_breath_any"))
    return spec_breath_any;
  if (!str_cmp (name, "spec_breath_acid"))
    return spec_breath_acid;
  if (!str_cmp (name, "spec_breath_fire"))
    return spec_breath_fire;
  if (!str_cmp (name, "spec_breath_frost"))
    return spec_breath_frost;
  if (!str_cmp (name, "spec_breath_gas"))
    return spec_breath_gas;
  if (!str_cmp (name, "spec_breath_lightning"))
    return spec_breath_lightning;
  if (!str_cmp (name, "spec_janitor"))
    return spec_janitor;
  if (!str_cmp (name, "spec_eatcorpse"))
    return spec_eatcorpse;
  if (!str_cmp (name, "spec_poison"))
    return spec_poison;
  if (!str_cmp (name, "spec_thief"))
    return spec_thief;
  if (!str_cmp (name, "spec_warrior"))
    return spec_warrior;
  if (!str_cmp (name, "spec_barbarian"))
    return spec_barbarian;
  return NULL;
}



bool 
spec_warrior (CHAR_DATA * ch)
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  if (ch->position != POSITION_FIGHTING)
    return FALSE;
  for (victim = ch->in_room->more->people; victim != NULL; victim = v_next)
    {
      v_next = victim->next_in_room;
      if (FIGHTING (victim) == ch && number_range (1, 2) == 1)
	break;
    }
  if (victim == NULL)
    return FALSE;
  if (ch->fgt && ch->fgt->ears > 9)
    return FALSE;
  if (number_range (1, 5) == 1)
    {
      do_kick (ch, "");
      return TRUE;
    }
  else if (number_range (1, 5) == 1)
    {
      do_bash (ch, "");
      return TRUE;
    }
  else if (ch->move > 45)
    do_flurry (ch, "");
  return TRUE;
}

/*
   * Core procedure for dragons.
 */
bool 
dragon (CHAR_DATA * ch, char *spell_name)
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  SPELL_DATA *spll;
  if (ch->position != POSITION_FIGHTING)
    return FALSE;
  for (victim = ch->in_room->more->people; victim != NULL; victim = v_next)
    {
      v_next = victim->next_in_room;
      if (FIGHTING (victim) == ch && number_bits (2) == 0)
	break;
    }
  if (victim == NULL)
    return FALSE;
  if ((spll = skill_lookup (spell_name, -1)) == NULL)
    return FALSE;
  general_spell (spll, LEVEL (ch), ch, victim);
  return TRUE;
}

/*
   * Special procedures for mobiles.
 */
bool 
spec_breath_any (CHAR_DATA * ch)
{
  if (ch->position != POSITION_FIGHTING)
    return FALSE;
  switch (number_bits (3))
    {
    case 0:
      return spec_breath_fire (ch);
    case 1:
    case 2:
      return spec_breath_lightning (ch);
    case 3:
      return spec_breath_gas (ch);
    case 4:
      return spec_breath_acid (ch);
    case 5:
    case 6:
    case 7:
      return spec_breath_frost (ch);
    }
  return FALSE;
}

bool 
spec_breath_acid (CHAR_DATA * ch)
{
  return dragon (ch, "acid breath");
}

bool 
spec_breath_fire (CHAR_DATA * ch)
{
  return dragon (ch, "fire breath");
}

bool 
spec_breath_frost (CHAR_DATA * ch)
{
  return dragon (ch, "frost breath");
}

bool 
spec_breath_gas (CHAR_DATA * ch)
{
  SPELL_DATA *spll;
  if (ch->position != POSITION_FIGHTING)
    return FALSE;
  if ((spll = skill_lookup ("gas breath", -1)) == NULL)
    return FALSE;
  general_spell (spll, LEVEL (ch), ch, NULL);
  return TRUE;
}

bool 
spec_breath_lightning (CHAR_DATA * ch)
{
  return dragon (ch, "lightning breath");
}

bool 
spec_executioner (CHAR_DATA * ch)
{
  
  /*  char buf[STD_LENGTH];
      CHAR_DATA *victim;
      CHAR_DATA *v_next;
      if (!IS_AWAKE (ch) || FIGHTING (ch) != NULL)
      return FALSE;
      for (victim = ch->in_room->more->people; victim != NULL; victim = v_next)
      {
      v_next = victim->next_in_room;
      
      break;
      }
      if (victim == NULL)
      return FALSE;
      sprintf (buf, "%s is a outlaw! I am the judge and jury! Penalty is death!",
      NAME (victim));
      do_yell (ch, buf);
      multi_hit (ch, victim, TYPE_UNDEFINED);*/
  return TRUE;
}

bool 
spec_eatcorpse (CHAR_DATA * ch)
{
  SINGLE_OBJECT *corpse;
  SINGLE_OBJECT *c_next;
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj_next;
  if (!IS_AWAKE (ch) || FIGHTING (ch) != NULL)
    return FALSE;
  for (corpse = ch->in_room->more->contents; corpse != NULL; corpse = c_next)
    {
      c_next = corpse->next_content;
      if (corpse->pIndexData->item_type != ITEM_CORPSE_NPC)
	continue;
      act ("$n begins munching away at the corpse!!", ch, NULL, NULL, TO_ROOM);
      for (obj = corpse->contains; obj; obj = obj_next)
	{
	  obj_next = obj->next_content;
	  obj_from (obj);
	  obj_to (obj, ch->in_room);
	}
      free_it (corpse);
      return TRUE;
    }
  return FALSE;
}

bool 
spec_guard (CHAR_DATA * ch)
{
 
  return FALSE;
}

bool 
spec_janitor (CHAR_DATA * ch)
{
  SINGLE_OBJECT *trash;
  SINGLE_OBJECT *trash_next;
  if (!IS_AWAKE (ch))
    return FALSE;
  for (trash = ch->carrying; trash != NULL; trash = trash_next)
    {
      trash_next = trash->next_content;
      switch (trash->pIndexData->item_type)
	{
	default:
	  continue;
	case ITEM_DRINK_CON:
	  {
	     free_it (trash);
	     break;
	  }
	case ITEM_FOOD:
	  act ("$n eats $p.", ch, trash, NULL, TO_ROOM);
	  free_it (trash);
	  break;
	}
    }
  for (trash = ch->in_room->more->contents; trash != NULL; trash = trash_next)
    {
      trash_next = trash->next_content;
      if (!IS_SET (trash->pIndexData->wear_flags, ITEM_TAKE))
	continue;
      if (trash->pIndexData->item_type == ITEM_DRINK_CON
	  || trash->pIndexData->item_type == ITEM_TRASH
	  || trash->cost < 10)
	{
	  act ("$n picks up some trash.", ch, NULL, NULL, TO_ROOM);
	  if (trash->in_room == NULL)
	    continue;
	  obj_from (trash);
	  obj_to (trash, ch);
	  return TRUE;
	}
    }
  return FALSE;
}

bool 
spec_poison (CHAR_DATA * ch)
{
  CHAR_DATA *victim;
  SPELL_DATA *spl;
  if (ch->position != POSITION_FIGHTING
      || !(victim = FIGHTING (ch)) || (FIGHTING (ch) == NULL)
      || number_percent () > 2 * LEVEL (ch))
    return FALSE;
  act ("You bite $N!", ch, NULL, victim, TO_CHAR);
  act ("$n bites $N!", ch, NULL, victim, TO_NOTVICT);
  act ("$n bites you!", ch, NULL, victim, TO_VICT);

/*MAKE IT POISON THE PERSON HERE!! */
  if ((spl = skill_lookup (NULL, gsn_poison)) == NULL)
    return FALSE;

  general_spell (spl, LEVEL (ch), ch, victim);

  return TRUE;
}

bool 
spec_thief (CHAR_DATA * ch)
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  SINGLE_OBJECT *obj, *o_next;
  if (ch->position != POSITION_STANDING)
    return FALSE;
  for (victim = ch->in_room->more->people; victim != NULL; victim = v_next)
    {
      v_next = victim->next_in_room;
      if (IS_MOB (victim))
	continue;
      if (LEVEL (victim) >= LEVEL_IMMORTAL
	  || number_bits (3) != 0
	  || !can_see (ch, victim))	/* Thx Glop */
	continue;
      if (is_safe (ch, victim))
	return FALSE;
      if (IS_SET (ch->act, ACT_ATTITUDE) && (!IS_AWAKE (victim) || number_range (1, 11) == 2))
	{
	  do_actual_backstab (ch, RNAME (victim));
	  return TRUE;
	}
      if (IS_AWAKE (victim)
	  && (dice (3, 6) < (get_curr_dex (ch) + get_curr_wis (ch)
			  - get_curr_wis (victim) - get_curr_dex (victim))))
	{
	  SINGLE_OBJECT *obj;
	  int chance;
	  if (victim->pcdata->carry_number == 0)
	    return FALSE;
	  chance = 100 / victim->pcdata->carry_number;
	  for (obj = victim->carrying; obj != NULL; obj = obj->next_content)
	    {
	      if ((obj->wear_loc != -1)
		  || !can_drop_obj (victim, obj))
		continue;
	      if (number_percent () < chance)
		{
		  obj_from (obj);
		  obj_to (obj, ch);
		  if (dice (1, 6) < (get_curr_dex (victim) - get_curr_dex (ch)))
		    send_to_char ("You feel less burdened.\n\r", victim);
		  return TRUE;
		}
	      chance += chance;
	    }
	}
      else
	{
	  act ("$n tried to steal from you!", ch, NULL, victim, TO_VICT);
	}
      return FALSE;
    }
  for (obj = ch->in_room->more->contents; obj != NULL; obj = o_next)
    {
      int chance;
      o_next = obj->next_content;
      if (ch->next_in_room == NULL)
	chance = 100;
      else
	chance = 50;
      if (obj->pIndexData->item_type == ITEM_CORPSE_NPC
	  || obj->pIndexData->item_type == ITEM_CORPSE_PC)
	{
	  SINGLE_OBJECT *obj2;
	  for (obj2 = obj->contains; obj2 != NULL; obj2 = obj2->next_content)
	    {
	      if (IS_SET (obj2->extra_flags, ITEM_NODROP))
		continue;
	      if (number_percent () < chance)
		{
		  obj_from (obj2);
		  obj_to (obj2, ch);
		  return TRUE;
		}
	    }
	}
      else
	{
	  if (!can_see_obj (ch, obj))
	    continue;
	  if (!CAN_WEAR (obj, ITEM_TAKE))
	    continue;
	  if (number_percent () < chance)
	    {
	      if (obj->in_room == NULL)
		continue;
	      obj_from (obj);
	      obj_to (obj, ch);
	      return TRUE;
	    }
	}
    }
  return FALSE;
}
