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
CHAR_DATA *
find_keeper (CHAR_DATA * ch)
{
  CHAR_DATA *keeper;
  SHOP_DATA *pShop;
  pShop = NULL;
  check_room_more (ch->in_room);
  for (keeper = ch->in_room->more->people; keeper; keeper = keeper->next_in_room)
    {
      if (IS_MOB (keeper) && (pShop = keeper->pIndexData->pShop) != NULL)
	break;
    }
  if (pShop == NULL)
    {
      send_to_char ("You can't do that here.\n\r", ch);
      return NULL;
    }
/*
   * Shop hours.
 */
  if (time_info.hour == 12)
    {
      char buff[500];
      sprintf(buff, "Ahh, welcome %s. It is so good to see you again. I am currently partaking of my midday meal. Perhaps if you would be so kind as to return in a little while, I will be able to assist you, as you are one of my most valued customers. We strive for one hundred percent customer satisfaction here at %s. However, I must eat my lunch, Thank you, come again.\n\r", NAME(ch),  ch->in_room->name);
      do_say (keeper, buff);
      return NULL;
    }
  if (time_info.hour < pShop->open_hour)
    {
      do_say (keeper, "Sorry, try again later.");
      return NULL;
    }
  if (time_info.hour > pShop->close_hour)
    {
      do_say (keeper, "Sorry, we are closed.");
      return NULL;
    }
/*
   * Invisible or hidden people.
 */
  if (!can_see (keeper, ch))
    {
      do_say (keeper, "What? Who said that?");
      return NULL;
    }
  return keeper;
}

CHAR_DATA *
find_repair (CHAR_DATA * ch, int rnum)
{
  CHAR_DATA *keeper;
  check_room_more (ch->in_room);
  for (keeper = ch->in_room->more->people; keeper; keeper = keeper->next_in_room)
    {
      if ((rnum == 1) && IS_MOB (keeper) && (IS_SET (keeper->act, REPAIRS_ONE)))
	break;
      if ((rnum == 2) && IS_MOB (keeper) && (IS_SET (keeper->act, REPAIRS_TWO)))
	break;
    }
  if (keeper == NULL)
    return NULL;
  if (!can_see (keeper, ch))
    {
      do_say (keeper, "What? Who said that?");
      return NULL;
    }
  return keeper;
}

int 
get_cost (CHAR_DATA * keeper, SINGLE_OBJECT * obj, bool fBuy)
{
  SHOP_DATA *pShop;
  int cost;
  if (obj == NULL || (pShop = keeper->pIndexData->pShop) == NULL)
    return 0;
  if (fBuy)
    {
      cost = obj->cost * pShop->profit_buy / 96;
    }
  else
    {
      SINGLE_OBJECT *obj2;
      int itype;
      int tally;
      int origcost;
      cost = 0;
      tally = 0;
      for (itype = 0; itype < MAX_TRADE; itype++)
	{
	  if (obj->pIndexData->item_type == pShop->buy_type[itype])
	    {
	      cost = obj->cost * pShop->profit_sell / 96;
	      break;
	    }
	}
      if (cost == 0)
	return 0;
      for (obj2 = keeper->carrying; obj2; obj2 = obj2->next_content)
	{
	  if (obj->pIndexData == obj2->pIndexData)
	    tally++;
	}
      if (tally > keeper->pIndexData->pShop->items_until_decrease)
	{
	  origcost = cost;
	  cost -= ((tally - keeper->pIndexData->pShop->items_until_decrease) *
		   (cost / (100 / keeper->pIndexData->pShop->decrease)));
	  if (cost < 0)
	    return 0;
	}
    }
  return cost;
}

void 
do_fix (CHAR_DATA *ch, char *argy)
{
  DEFINE_COMMAND ("fix", do_patch, POSITION_STANDING, 0, LOG_NORMAL,
"This command allows people to repair armor and weapons.")
do_patch(ch, argy);
return;
}

void
do_patch (CHAR_DATA *ch, char * argy)
{
  SINGLE_OBJECT *obj;
  char arg1[SML_LENGTH];
  int power;
  int maxrepair;
  int condnow;
  AFFECT_DATA *afd;
  DEFINE_COMMAND ("patch", do_patch, POSITION_STANDING, 0, LOG_NORMAL,
   "This command allows people to repair armor and weapons.")
    
    if (IS_MOB(ch)) return;
  argy = one_argy(argy, arg1);
  if ((obj = get_obj_inv (ch, arg1)) == NULL)
    {
      send_to_char("You do not have that item in your inventory to repair.\n\r",ch);
      return;
    }
  if(obj->pIndexData->item_type != ITEM_WEAPON && 
     obj->pIndexData->item_type != ITEM_ARMOR)
    {
      send_to_char("You can only patch weapons and armor.\n\r", ch);
      return;
    }
  WAIT_STATE(ch, 5*PULSE_VIOLENCE);
  afd = obj->pIndexData->affected;
  if (obj->pIndexData->item_type == ITEM_WEAPON)
    {
      I_WEAPON *wep = (I_WEAPON *) obj->more;
      if (wep->strength == 0)
	{
	  send_to_char("This item cannot be patched.\n\r", ch);
	  return;
	}
      if(wep->damage_p == 100)
	{ 
          send_to_char("This weapon is fine.\n\r", ch);
          return;
        }
      power = (wep->firstdice * wep->seconddice);
     
      LOOP (afd, obj->pIndexData->affected, afd->next)
	{
	  if (afd->location == APPLY_DAMROLL && afd->modifier > 0)
	    power += 5;
	  if (afd->location == APPLY_HIT)
	    power += (afd->modifier) /3;
	  if(afd->location > 0 && afd->location <= APPLY_CON && afd->modifier > 0)
	    power += 5;
	  if (afd->location == APPLY_MOVE)
	    power += (afd->modifier/4);
	}
      maxrepair = 100 - power;
      if (is_member(ch, GUILD_TINKER))
	maxrepair += 20;
      if (wep->damage_p > maxrepair)
	{
	  send_to_char("You cannot patch this item any better than it already is.\n\r", ch);
	  return;
	}
      if(number_range(1,130) < (ch->pcdata->learned[gsn_patch] +
(is_member(ch, GUILD_TINKER) ? 20 : 0)))
	{
	  act("You patch $p", ch, obj, ch, TO_CHAR);
	  act("$n patches $s $p.", ch, obj, ch, TO_NOTVICT);
	  wep->damage_p += number_range(2, 6) + (is_member(ch, GUILD_TINKER) ? number_range(2,4) : 0);
	  if (wep->damage_p > 100)
	    wep->damage_p = 100;
          skill_gain(ch, gsn_patch, TRUE);
	  return;
	}
      else
	{
	  act("You try to patch your $p, but make it worse!", ch, obj, NULL, TO_CHAR);
	  act("$n tries to patch $s $p, but makes it worse!", ch, obj, ch, TO_NOTVICT);
	  wep->damage_p -= number_range(1,10);
	  if (wep->damage_p < 2)
	    {
	      act("The $p just BROKE!", ch, obj, NULL, TO_ROOM);
	      obj_from(obj);
	      free_it(obj);
	      return;
	    }
	}
    }
 if (obj->pIndexData->item_type == ITEM_ARMOR)
    {
      I_ARMOR *arm = (I_ARMOR *) obj->more;
      if (arm->max_condition == 0)
	{
	  send_to_char("This item cannot be patched.\n\r", ch);
	  return;
	}
      if(arm->condition_now == arm->max_condition)
        {
          send_to_char("This item is fine!\n\r", ch);
          return;
        }
      power = arm->protects_legs + arm->protects_body + arm->protects_arms + arm->protects_head;
      if (power < 11)
	power  = 11;
      LOOP (afd, obj->pIndexData->affected, afd->next)
	{
	  if (afd->location == APPLY_DAMROLL && afd->modifier > 0)
	    power += 5;
	  if (afd->location == APPLY_HIT)
	    power += (afd->modifier)/3;
	
	  if(afd->location > 0 && afd->location <= APPLY_CON && afd->modifier > 0)
	    power += 5;
	}
      maxrepair = 100 - power;
      condnow = 100*(arm->condition_now)/(arm->max_condition);
      if (is_member(ch, GUILD_TINKER))
	maxrepair += 20;
     
      if (condnow > maxrepair)
	{
	  send_to_char("You cannot patch this item any better than it already is.\n\r", ch);
	  return;
	}
      if(number_range(1,130) < (ch->pcdata->learned[gsn_patch] +
(is_member(ch, GUILD_TINKER) ? 20 : 0)))
	{
	  act("You patch $p", ch, obj, ch, TO_CHAR);
	  act("$n patches $s $p.", ch, obj, ch, TO_NOTVICT);
	  arm->condition_now += UMAX(1, (number_range(2,6) + (is_member(ch, GUILD_TINKER) ? 3 : 0))* (arm->max_condition)/100);
	  if (arm->condition_now > arm->max_condition)
	    arm->condition_now = arm->max_condition;
          skill_gain(ch, gsn_patch, TRUE);
	  return;
	}
      else
	{
	  act("You try to patch your $p, but make it worse!", ch, obj, NULL, TO_CHAR);
	  act("$n tries to patch $s $p, but makes it worse!", ch, obj, ch, TO_NOTVICT);
	  arm->condition_now -= UMAX(1, ((number_range(1,10) * (arm->max_condition)/100)));
	  if (arm->condition_now < 2)
	    {
	      act("The $p just BROKE!", ch, obj, NULL, TO_ROOM);
	      obj_from(obj);
	      free_it(obj);
	      return;
	    }
	}
    } 
 return;
}
  void 
do_repair (CHAR_DATA * ch, char *argy)
{
  char buf[SML_LENGTH];
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  CHAR_DATA *keeper;
  CHAR_DATA *keepert;
  SINGLE_OBJECT *obj;
  int repair_cost;
  bool warm;
  bool wrep;
  bool alll;
 
  DEFINE_COMMAND ("repair", do_repair, POSITION_STANDING, 0, LOG_NORMAL, "This command will repair a damaged weapon(s) or armor(s), depending on the shopkeeper present.")

    alll = FALSE;
  obj = NULL;
  repair_cost = 0;
  arg1[0] = '\0';
  arg2[0] = '\0';
  if (IS_MOB (ch))
    return;
  if (argy[0] == '\0' || argy == "")
    {
      send_to_char ("Syntax: Repair <item/all> to get the estimated cost.\n\r", ch);
      send_to_char (" Repair <item/all> YES to go ahead and repair the item.\n\r", ch);
      return;
    }
  argy = one_argy (argy, arg1);
  if (argy[0] != '\0' && argy != "")
    argy = one_argy (argy, arg2);
  else
    arg2[0] = '\0';
  if (!str_cmp (arg1, "all"))
    alll = TRUE;
  if (!alll && (obj = get_obj_inv (ch, arg1)) == NULL)
    {
      send_to_char ("You do not have that item.\n\r", ch);
      return;
    }
  if ((keeper = find_repair (ch, 1)) == NULL)
    wrep = FALSE;
  else
    wrep = TRUE;
  if ((keepert = find_repair (ch, 2)) == NULL)
    warm = FALSE;
  else
    warm = TRUE;
  if (keeper == NULL && keepert != NULL)
    keeper = keepert;
  if (!warm && !wrep)
    {
      send_to_char ("No-one here will repair any of your equipment.\n\r", ch);
      return;
    }
  if (alll)
    {
      /*REPAIR ALL */
      SINGLE_OBJECT *obj_nxt;
      int repair_cost_t;
      repair_cost_t = 0;
      for (obj = ch->carrying; obj != NULL; obj = obj_nxt)
	{
	  obj_nxt = obj->next_content;
	  if (obj->next == NULL)
	    obj_nxt = NULL;
	  if (obj == NULL)
	    {
	      send_to_char ("Found nothing to repair.\n\r", ch);
	      return;
	    }
	  if (obj->pIndexData->item_type != ITEM_WEAPON && obj->pIndexData->item_type != ITEM_ARMOR)
	    continue;
	  if (obj->pIndexData->item_type == ITEM_WEAPON && !wrep)
	    continue;
	  if (obj->pIndexData->item_type == ITEM_ARMOR && !warm)
	    continue;
	  if (obj->pIndexData->item_type == ITEM_WEAPON)
	    {
	      I_WEAPON *wp = (I_WEAPON *) obj->more;
	      
	      if (wp->damage_p > 75)
		repair_cost=((100-wp->damage_p)*(obj->cost))/(pow.repair_weapons[0]);
	      else if (wp->damage_p > 33)
		repair_cost =((100-wp->damage_p)*(obj->cost))/(pow.repair_weapons[1]);
	      else
		repair_cost =((100-wp->damage_p)*(obj->cost))/(pow.repair_weapons[2]);
	      if (repair_cost < 1)
		continue;
	      
	    }
	  if (obj->pIndexData->item_type == ITEM_ARMOR)
	    {
	      I_ARMOR *r = (I_ARMOR *) obj->more;
	      repair_cost =( (r->max_condition - r->condition_now)*
			     (obj->cost))/ (pow.repair_armors);
	      
	      if (repair_cost < 1)
		continue;
	    }
	  sprintf (buf, "$N will repair $p for %d coins.", repair_cost);
	  act (buf, ch, obj, keeper, TO_CHAR);
	  if (is_member (ch, GUILD_TINKER))
	    {
	      act ("...but since you are a member of the Tinker's Guild...", ch, NULL, NULL, TO_CHAR);
	      if (repair_cost > 6)
		{
		  repair_cost = (repair_cost) -
		    ((pow.tinker_discount * repair_cost) / 100);
		}
	      sprintf (buf, "$N will repair $p for %d coins.", repair_cost);
	      act (buf, ch, obj, keeper, TO_CHAR);
	    }
	  repair_cost_t += repair_cost;
	  if (str_cmp (arg2, "yes"))
	    continue;
	  if (tally_coins (ch) < repair_cost_t)
	    {
	      act ("You don't have enough money to repair everything!", ch, obj, NULL, TO_CHAR);
	      return;
	    }
	}
      /*END OBJ LIST */
      send_to_char ("------------------\n\r", ch);
      sprintf (buf, "Total: %d coins\n\r", repair_cost_t);
      send_to_char (buf, ch);
      if (str_cmp (arg2, "yes"))
	return;
      send_to_char ("\n\r", ch);
      for (obj = ch->carrying; obj != NULL; obj = obj_nxt)
	{
	  obj_nxt = obj->next_content;
	  if (obj->next == NULL)
	    obj_nxt = NULL;
	  if (obj->pIndexData->item_type != ITEM_WEAPON && obj->pIndexData->item_type != ITEM_ARMOR)
	    continue;
	  if (obj->pIndexData->item_type == ITEM_WEAPON && !wrep)
	    continue;
	  if (obj->pIndexData->item_type == ITEM_ARMOR && !warm)
	    continue;
	  
	  if (obj->pIndexData->item_type == ITEM_WEAPON)
	    {
	      I_WEAPON *wp = (I_WEAPON *) obj->more;
	      if (wp->damage_p > 99)
		continue;
	    }
	  
	  if (obj->pIndexData->item_type == ITEM_ARMOR)
	    {
	      I_ARMOR *r = (I_ARMOR *) obj->more;
	      if (r->max_condition == 0 || r->max_condition <= r->condition_now)
		continue;
	    }
	  
	  act ("$N gives you back $p, brand new!", ch, obj, keeper, TO_CHAR);
	  if (obj->pIndexData->item_type == ITEM_ARMOR)
	    {
	      I_ARMOR *r = (I_ARMOR *) obj->more;
	      if (obj->wear_loc != WEAR_NONE)
		{
		  int tempslot;
		  tempslot = obj->wear_loc;
		  unequip_char (ch, obj);
                  if (number_range(1,110) == 4 && r->max_condition > 1)
                    r->max_condition--;
		  r->condition_now = r->max_condition;
		  equip_char (ch, obj, tempslot);
		  continue;
		}
	      else
		r->condition_now = r->max_condition;
	      continue;
	    }
	  if (obj->pIndexData->item_type == ITEM_WEAPON)
            {
	      ((I_WEAPON *) obj->more)->damage_p = 100;
	      if (number_range(1,100) == 8 && ((I_WEAPON *) obj->more)->strength > 1)
		((I_WEAPON *)obj->more)->strength--; 
	      continue;
	    }
        }
      if (repair_cost_t > 0)
	{
	   sub_coins (repair_cost_t, ch);
	   create_amount (repair_cost_t, keeper, NULL, NULL);
	}
      return;
    }
  /*END REPAIR ALL */
  if (obj->pIndexData->item_type == ITEM_WEAPON && !wrep)
    {
      act ("$N can't repair weapons, only armor.", ch, NULL, keeper, TO_CHAR);
      return;
    }
  if (obj->pIndexData->item_type == ITEM_ARMOR && !warm)
    {
      act ("$N can't repair armors, only weapons.", ch, NULL, keeper, TO_CHAR);
      return;
    }
  if (obj->pIndexData->item_type != ITEM_WEAPON && obj->pIndexData->item_type != ITEM_ARMOR)
    return;
  if (obj->pIndexData->item_type == ITEM_WEAPON)
    {
      I_WEAPON *wp = (I_WEAPON *) obj->more;
      if (wp->firstdice * wp->seconddice < 17)
	repair_cost = (100 -wp->damage_p)*(obj->cost)/pow.repair_weapons[0];
      else if (wp->firstdice * wp->seconddice < 26)
	repair_cost = (100 -wp->damage_p)*(obj->cost)/pow.repair_weapons[1];
      else
	{
	  if (LEVEL (keeper) > 75)
	    repair_cost = (100 -wp->damage_p)*(obj->cost)/pow.repair_weapons[2];
	  else
	    {
	      act ("$N's eyes widen when $S sees $p.\n\r$N refuses to touch such a powerful weapon.", ch, obj, keeper, TO_CHAR);
	    }
	}
      if (repair_cost < 1)
	{
	  send_to_char ("That object doesn't need any repair.\n\r", ch);
	  return;
	}
    }
  else if (obj->pIndexData->item_type == ITEM_ARMOR)
    {
      I_ARMOR *r = (I_ARMOR *) obj->more;
      repair_cost = (r->max_condition - r->condition_now)*(obj->cost)/pow.repair_armors;
      
      
      if (repair_cost < 1)
	{
	  send_to_char ("That object doesn't need any repair.\n\r", ch);
	  return;
	}
    }
  else
    {
      send_to_char ("I can't repair that!\n\r", ch);
      return;
    }
  sprintf (buf, "$N will repair $p for %d coins.", repair_cost);
  act (buf, ch, obj, keeper, TO_CHAR);
  if (is_member (ch, GUILD_TINKER))
    {
      act ("...but since you are a member of the Tinker's Guild...", ch, NULL, NULL, TO_CHAR);
      if (repair_cost > 6)
	repair_cost = (repair_cost) -
	  ((pow.tinker_discount * repair_cost) / 100);
      sprintf (buf, "$N will repair $p for %d coins.", repair_cost);
      act (buf, ch, obj, keeper, TO_CHAR);
    }
  if (!(!str_cmp (arg2, "yes")))
    return;
  if (tally_coins (ch) < repair_cost)
    {
      act ("You don't have enough money to repair $p!", ch, obj, NULL, TO_CHAR);
      return;
    }
  if (repair_cost > 0)
    {
      sub_coins (repair_cost, ch);
      create_amount (repair_cost, keeper, NULL, NULL);
    }
  if (obj->pIndexData->item_type == ITEM_ARMOR)
    {
      I_ARMOR *r = (I_ARMOR *) obj->more;
      if (obj->wear_loc != WEAR_NONE)
	{
	  int tempslot;
	  tempslot = obj->wear_loc;
	  unequip_char (ch, obj);
	  r->condition_now = r->max_condition;
	  equip_char (ch, obj, tempslot);
	  return;
	}
      else
	r->condition_now = r->max_condition;
      return;
    }
  if (obj->pIndexData->item_type == ITEM_WEAPON)
    ((I_WEAPON *) obj->more)->damage_p = 100;
  return;
}
void 
do_resize (CHAR_DATA * ch, char *argy)
{
  char buf[SML_LENGTH];
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  CHAR_DATA *keeper;
  SINGLE_OBJECT *obj;
  I_ARMOR *r;
  int repair_cost;
  bool warm;
  bool alll;
  DEFINE_COMMAND ("resize", do_resize, POSITION_STANDING, 0, LOG_NORMAL, "This command allows you to resize an object (or objects) so that they fit you.  You must be at a shopkeeper.")
    alll = FALSE;
  obj = NULL;
  repair_cost = 0;

  if (IS_MOB (ch))
    return;
  if (argy[0] == '\0')
    {
      send_to_char ("Syntax: Resize <item/all> to get the estimated cost.\n\r", ch);
      send_to_char (" Resize <item/all> YES to go ahead and repair the item.\n\r", ch);
      return;
    }
  argy = one_argy (argy, arg1);
  if (argy[0] != '\0')
    argy = one_argy (argy, arg2);
  else
    arg2[0] = '\0';
  if (!str_cmp (arg1, "all"))
    alll = TRUE;
  if (!alll && (obj = get_obj_inv (ch, arg1)) == NULL)
    {
      send_to_char ("You do not have that item.\n\r", ch);
      return;
    }
  warm = TRUE;
  if ((keeper = find_repair (ch, 2)) == NULL)
    warm = FALSE;
  if (!warm)
    {
      send_to_char ("No-one here will resize any of your equipment.\n\r", ch);
      return;
    }
  if (alll)
    {
      /*REPAIR ALL */
      SINGLE_OBJECT *obj_nxt;
      int repair_cost_t;
      repair_cost_t = 0;
      for (obj = ch->carrying; obj != NULL; obj = obj_nxt)
	{
	  obj_nxt = obj->next_content;
	  if (obj->next == NULL)
	    obj_nxt = NULL;
	  if (obj == NULL)
	    {
	      send_to_char ("Found nothing to resize.\n\r", ch);
	      return;
	    }
	  if (obj->pIndexData->item_type != ITEM_ARMOR)
	    continue;
	  if (obj->wear_loc != -1 || OBJ_FITS (obj, ch))
	    continue;
	  repair_cost = obj->cost / pow.resize_divisor;
	  if (repair_cost < 1)
	    repair_cost = 1;
	  sprintf (buf, "$N will resize $p for %d coins.", repair_cost);
	  act (buf, ch, obj, keeper, TO_CHAR);
	  repair_cost_t += repair_cost;
	  if (!(!str_cmp (arg2, "yes")))
	    continue;
	  if (tally_coins (ch) < repair_cost_t)
	    {
	      act ("You don't have enough money to resize everything!", ch, obj, NULL, TO_CHAR);
	      return;
	    }
	}
      /*END OBJ LIST */
      send_to_char ("------------------\n\r", ch);
      sprintf (buf, "Total: %d coins\n\r", repair_cost_t);
      send_to_char (buf, ch);
      if (!(!str_cmp (arg2, "yes")))
	return;
      send_to_char ("\n\r", ch);
      for (obj = ch->carrying; obj != NULL; obj = obj_nxt)
	{
	  obj_nxt = obj->next_content;
	  if (obj->next == NULL)
	    obj_nxt = NULL;
	  if (obj->pIndexData->item_type != ITEM_ARMOR || obj->wear_loc != -1 || OBJ_FITS (obj, ch))
	    continue;
	  act ("$N gives you back $p, fit to perfection!", ch, obj, keeper, TO_CHAR);
	  SET_OBJ_SIZE (ch, obj);
	  continue;
	}
      if (repair_cost_t > 0)
	{
	  sub_coins (repair_cost_t, ch);
	}
      return;
    }
  if (obj->pIndexData->item_type != ITEM_ARMOR || obj->wear_loc != -1 || OBJ_FITS (obj, ch))
    {
      act ("Why would you want to resize that?", ch, NULL, keeper, TO_CHAR);
      return;
    }
  if (obj->pIndexData->item_type != ITEM_ARMOR)
    return;
  repair_cost = obj->cost / pow.resize_divisor;
  if (repair_cost < 1)
    repair_cost = 1;
  sprintf (buf, "$N will resize $p for %d coins.", repair_cost);
  act (buf, ch, obj, keeper, TO_CHAR);
  if (!(!str_cmp (arg2, "yes")))
    return;
  if (tally_coins (ch) < repair_cost)
    {
      act ("You don't have enough money to resize $p!", ch, obj, NULL, TO_CHAR);
      return;
    }
  if (repair_cost > 0)
    {
      sub_coins (repair_cost, ch);
    }
  act ("$N gives back $p, fitted to perfection!", ch, obj, keeper, TO_CHAR);
  SET_OBJ_SIZE (ch, obj);
  return;
}

void 
do_buy (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  char tells[SML_LENGTH];
  DEFINE_COMMAND ("buy", do_buy, POSITION_STANDING, 0, LOG_NORMAL, "This command allows you to buy an item or pet from a shopkeeper in the room.")
    if (LEVEL (ch) > 100 && !IS_REAL_GOD (ch) && ch->in_room->area->open == 1)
    {
      send_to_char ("Immorts can not buy or sell items in open areas.\n\r", ch);
      return;
    }
  argy = one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Buy what?\n\r", ch);
      return;
    }
  if (IS_SET (ch->in_room->room_flags, ROOM_PET_SHOP))
    {
      char buf[STD_LENGTH];
      CHAR_DATA *pet;
      ROOM_DATA *oneroomNext;
      ROOM_DATA *in_room;
      if (IS_MOB (ch))
	return;
      oneroomNext = get_room_index (ch->in_room->vnum + 1);
      if (oneroomNext == NULL)
	{
	  bug ("Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum);
	  send_to_char ("Sorry, you can't buy that here.\n\r", ch);
	  return;
	}
      in_room = ch->in_room;
      ch->in_room = oneroomNext;
      pet = get_char_room (ch, arg);
      ch->in_room = in_room;
      if (pet == NULL || !IS_SET (pet->act, ACT_PET))
	{
	  send_to_char ("Sorry, you can't buy that here.\n\r", ch);
	  return;
	}
      if (LEVEL (ch) < 17)
	if (ch->pcdata->number_pets > 1)
	  {
	    send_to_char ("You are unable to control that many pets at once!\n\r", ch);
	    return;
	  }
      if (LEVEL (ch) < 30)
	if (ch->pcdata->number_pets > 2)
	  {
	    send_to_char ("You are unable to control that many pets at once!\n\r", ch);
	    return;
	  }
      if (LEVEL (ch) < 50)
	if (ch->pcdata->number_pets > 3)
	  {
	    send_to_char ("You are unable to control that many pets at once!\n\r", ch);
	    return;
	  }
      if (ch->pcdata->number_pets > 4)
	{
	  send_to_char ("You are unable to control that many pets at once!\n\r", ch);
	  return;
	}
      if (tally_coins (ch) < 20 * LEVEL (pet) * LEVEL (pet))
	{
	  send_to_char ("You can't afford it.\n\r", ch);
	  return;
	}
      sprintf (buf, "You receive %s in change.\n\r",
	       sub_coins (20 * LEVEL (pet) * LEVEL (pet), ch));
      send_to_char (buf, ch);
      ch->pcdata->number_pets++;
      pet = create_mobile (pet->pIndexData);
      SET_BIT (pet->act, ACT_PET);
      SET_BIT (pet->affected_by, AFF_CHARM);
      argy = one_argy (argy, arg);
      char_to_room (pet, ch->in_room);
      add_follower (pet, ch);
      send_to_char ("Enjoy your pet.\n\r", ch);
      act ("$n bought $N as a pet.", ch, NULL, pet, TO_ROOM);
      return;
    }
  else
    {
      CHAR_DATA *keeper;
      SINGLE_OBJECT *obj;
      int cost;
      int looking;
      if ((keeper = find_keeper (ch)) == NULL)
	return;
      if (is_number (arg))
	{
	  looking = atoi (arg);
	  cost = 0;
	  for (obj = keeper->carrying; obj != NULL && cost < looking;
	       obj = obj->next_content)
	    {
	      if (obj->wear_loc == WEAR_NONE
		  || obj->wear_loc == WEAR_HOLD_1
		  || obj->wear_loc == WEAR_HOLD_2)
		cost++;
	    }
	}
      else
	obj = get_obj_inv (keeper, arg);
      if (obj)
	cost = get_cost (keeper, obj, TRUE);
      else
	cost = 0;
      if (!obj || cost <= 0 || !can_see_obj (ch, obj))
	{
	  sprintf (tells, "%s I don't sell that.  Use the 'list' command to see what I sell.", NAME (ch));
	  do_tell (keeper, tells);
	  return;
	}
     /* if (!can_drop_obj (keeper, obj) && obj->pIndexData->item_type != ITEM_ARMOR)
	{
	  sprintf (tells, "%s I don't really want to sell it, sorry.", NAME (ch));
	  do_tell (keeper, tells);
	  return;
	}*/
      if (tally_coins (ch) < cost)
	{
	  sprintf (tells, "%s You don't have enough money to buy %s.", NAME (ch),
		   OOSTR (obj, short_descr));
	  do_tell (keeper, tells);
	  return;
	}

      act ("$n buys $p\x1B[0m from $N.", ch, obj, keeper, TO_ROOM);
      act ("You buy $p\x1B[0m from $N.", ch, obj, keeper, TO_CHAR);
      create_amount (cost, keeper, NULL, NULL);
      sub_coins (cost, ch);
      obj_from (obj);
      if (IS_SET (obj->extra_flags, ITEM_INVENTORY))
	obj->extra_flags ^= ITEM_INVENTORY;
      if (obj->pIndexData->item_type == ITEM_ARMOR)
	SET_OBJ_SIZE (ch, obj);
      obj_to (obj, ch);
      obj->wear_loc = -1;
      return;
    }
  return;
}

void 
do_list (CHAR_DATA * ch, char *argy)
{
  int listed_already[1000];
  int k;
  DEFINE_COMMAND ("list", do_list, POSITION_STANDING, 0, LOG_NORMAL, "This command allows you to list items or pets to buy if you are at a shop.")

    hugebuf_o[0] = '\0';
  if (IS_SET (ch->in_room->room_flags, ROOM_PET_SHOP))
    {
      ROOM_DATA *oneroomNext;
      CHAR_DATA *pet;
      bool found;
      oneroomNext = get_room_index (ch->in_room->vnum + 1);
      if (oneroomNext == NULL)
	{
	  bug ("Do_list: bad pet shop at vnum %d.", ch->in_room->vnum);
	  send_to_char ("You can't do that here.\n\r", ch);
	  return;
	}
      found = FALSE;
      check_room_more (oneroomNext);
      for (pet = oneroomNext->more->people; pet; pet = pet->next_in_room)
	{
	  if (IS_SET (pet->act, ACT_PET))
	    {
	      if (!found)
		{
		  found = TRUE;
		  send_to_char ("Pets for sale:\n\r", ch);
		}
	      sprintf (hugebuf_o, "%8d copper pieces - %s\n\r",
		       20 * LEVEL (pet) * LEVEL (pet),
		       SD (pet));
	      send_to_char (hugebuf_o, ch);
	    }
	}
      if (!found)
	send_to_char ("You notice a sign that reads:\n\rSorry, we're out of pets right now.\n\r", ch);
      return;
    }
  else
    {
      char arg[SML_LENGTH];
      char buff[20];
      CHAR_DATA *keeper;
      SINGLE_OBJECT *obj;
      bool flagx;
      int cost;
      int listcnt;
      int num;
      int flex;
      bool found;
      one_argy (argy, arg);
      if ((keeper = find_keeper (ch)) == NULL)
	return;
      listcnt = 0;
      found = FALSE;
      num = 0;
      for (k = 0; k < 1000; k++)
	{
	  listed_already[k] = 0;
	}
      buff[0] = '\0';
      flex = 14-(strlen(ch->in_room->name))/2;
      if (flex > 0)
	for (k = 0; k < flex; k++)
	  strcat(buff, " ");
     
      sprintf(hugebuf_o, "\x1b[1;36m_\\|%s\x1b[0;32mWelcome to %s, what would you like to buy?%s%s\x1b[1;36m|/_\n\n\r", buff,ch->in_room->name,buff,(IS_SET(strlen(ch->in_room->name), 1) ? " " : ""));
      sprintf(hugebuf_o +strlen(hugebuf_o), "\x1b[1;36m.:\x1b[1;34mNum\x1b[1;36m:.     \x1b[1;36m.:\x1b[1;34mItem\x1b[1;36m:.                       \x1b[1;36m.:\x1b[1;34mCost\x1b[1;36m:.\n\r");
      sprintf(hugebuf_o +strlen(hugebuf_o), "\x1b[0;32m[\x1b[0;35m+\x1b[0;32m]--------------------------------------------------------------------[\x1b[0;35m+\x1b[0;32m]\x1b[0;37m\n\r");
      
      for (obj = keeper->carrying; obj; obj = obj->next_content)
	{
	  if ((obj->wear_loc == WEAR_NONE
	       || obj->wear_loc == WEAR_HOLD_1
	       || obj->wear_loc == WEAR_HOLD_2)
	      && can_see_obj (ch, obj)
	      && (cost = get_cost (keeper, obj, TRUE)) > 0
	      && (arg[0] == '\0' || is_name (arg, obj->pIndexData->name)))
	    {
	      if (!found)
		{
		  found = TRUE;
		}
	      flagx = FALSE;
	      for (k = 0; k < 1000; k++)
		{
		  if (listed_already[k] == 0)
		    break;
		  if (listed_already[k] == obj->pIndexData->vnum)
		    {
		      flagx = TRUE;
		      break;
		    }
		}
	      num++;
	      if (flagx)
		continue;
	      sprintf (hugebuf_o + strlen (hugebuf_o), " \x1b[0;32m[\x1b[0;35m%3d\x1b[0;32m]      \x1b[1;36m%-30s\x1b[0;37m %-22s\n\r", num - 1,
		       strip_ansi_codes (OOSTR (obj, short_descr)),
		       name_amount (cost));
	      listed_already[listcnt] = obj->pIndexData->vnum;
	      listcnt++;
	      if (strlen (hugebuf_o) > 69500)
		{
		  return;
		}
	    }
	}
      if (found)
	{
	  page_to_char (hugebuf_o, ch);
	}
      if (!found)
	{
	  if (arg[0] == '\0')
	    send_to_char ("There is nothing to buy.\n\r", ch);
	  else
	    send_to_char ("You can't buy that here.\n\r", ch);
	}
      return;
    }
  return;
}

void 
do_sell (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  char tells[SML_LENGTH];
  CHAR_DATA *keeper;
  SINGLE_OBJECT *obj;
  int cost;
  DEFINE_COMMAND ("sell", do_sell, POSITION_STANDING, 0, LOG_NORMAL, "This command allows you to sell an item to a shopkeeper in the room.")

    one_argy (argy, arg);
  if (LEVEL (ch) > 100 && ch->in_room->area->open == 1 && !IS_REAL_GOD (ch))
    {
      send_to_char ("Immorts can not buy or sell items in open areas.\n\r", ch);
      return;
    }
  if (arg[0] == '\0')
    {
      send_to_char ("Sell what?\n\r", ch);
      return;
    }
  if ((keeper = find_keeper (ch)) == NULL)
    return;
  if ((obj = get_obj_carry (ch, arg)) == NULL)
    {
      sprintf (tells, "%s You aren't holding that item!", NAME (ch));
      do_tell (keeper, tells);
      return;
    }
  if (!can_drop_obj (ch, obj) &&
      (obj->pIndexData->vnum != 6753)
    )
    {
      send_to_char ("You can't let go of it.\n\r", ch);
      return;
    }
  if ((cost = get_cost (keeper, obj, FALSE)) <= 0)
    {
      act ("$n looks at $p\x1B[0m and shakes $s head.", keeper, obj, ch, TO_VICT);
      return;
    }
  if (tally_coins (keeper) < cost)
    {
      act ("$n does not have enough money to buy $p\x1B[0m from you.", keeper,
	   obj, ch, TO_VICT);
      return;
    }
  if ((IS_SET (obj->extra_flags, ITEM_NODROP) ||
       IS_SET (obj->extra_flags, ITEM_NOSAVE)) &&
      (obj->pIndexData->vnum != 6753)
    )
    {
      act ("$n does not want to buy that item from you!", keeper, obj, ch, TO_VICT);
      return;
    }
  act ("$n sells $p\x1B[0m to $N.", ch, obj, keeper, TO_ROOM);
  sprintf (buf, "You sell $p\x1B[0m for %s to $N.", name_amount (cost));
  act (buf, ch, obj, keeper, TO_CHAR);
  create_amount (cost, ch, NULL, NULL);
  sub_coins (cost, keeper);
  if (obj->pIndexData->item_type == ITEM_WEAPON)
    ((I_WEAPON *) obj->more)->damage_p = 100;
  if (obj->pIndexData->item_type == ITEM_ARMOR)
    ((I_ARMOR *) obj->more)->condition_now = ((I_ARMOR *) obj->more)->max_condition;
  if (obj->pIndexData->item_type == ITEM_TRASH)
    {
      act ("$n eats $p\x1B[0m, munching hungrily.", keeper, obj, NULL, TO_NOTVICT);
      free_it (obj);
    }
  else
    {
      if (!IS_SET (obj->extra_flags, ITEM_INVENTORY))
	obj->extra_flags ^= ITEM_INVENTORY;
      obj_from (obj);
      obj_to (obj, keeper);
    }
  return;
}

void 
do_value (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  char tells[SML_LENGTH];
  CHAR_DATA *keeper;
  SINGLE_OBJECT *obj;
  int cost;
  DEFINE_COMMAND ("value", do_value, POSITION_STANDING, 0, LOG_NORMAL, "This command shows you how much a shopkeeper will pay for a specific item.")

    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Value what?\n\r", ch);
      return;
    }
  if ((keeper = find_keeper (ch)) == NULL)
    return;
  if ((obj = get_obj_carry (ch, arg)) == NULL)
    {
      sprintf (tells, "%s You aren't holding that item!", NAME (ch));
      do_tell (keeper, tells);
      return;
    }
  if (!can_drop_obj (ch, obj) &&
      (obj->pIndexData->vnum != 6753))
    {
      send_to_char ("You can't let go of it.\n\r", ch);
      return;
    }
  if ((cost = get_cost (keeper, obj, FALSE)) <= 0)
    {
      act ("$n looks at $p\x1B[0m and shakes $s head.", keeper, obj, ch, TO_VICT);
      return;
    }
  sprintf (tells, "%s I guess I'd give you \x1B[0m%s for \x1B[0m%s.",
	   NAME (ch),
	   name_amount (cost),
	   OOSTR (obj, short_descr));
  do_tell (keeper, tells);
  return;
}
