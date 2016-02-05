#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

// Some stuff from other code - not much.. mostly weather crap

long average_time[MT];
long times_through[MT];
long last_time[MT];
long this_time[MT];
long total_time[MT];
long temp_time[MT];
char *itoa (int);
int hit_gain (CHAR_DATA * ch);
int move_gain (CHAR_DATA * ch);
void mobile_update (void);
void weather_update (void);
void char_update (void);
void char_only_update (void);
void transport_update (void);
void obj_update (void);
void aggr_update (void);
void do_disaster (void);
static int pulse_upd = PULSE_AREA;
static int hours_up = 0;


void
fix_char (CHAR_DATA * ch)
{
  I_ARMOR *r; 
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;
  SINGLE_OBJECT *obj;
  int mod, i;
  if (!ch || IS_MOB (ch))
    return;

  ch->hitroll = 0;
  ch->damroll = 0;
  ch->armor = ch->pcdata->nat_armor;
  ch->pcdata->plus_hide = 0;
  ch->pcdata->plus_sneak = 0;
  ch->pcdata->plus_kick = 0;
  ch->pcdata->armor[0] = 0;
  ch->pcdata->armor[1] = 0;
  ch->pcdata->armor[2] = 0;
  ch->pcdata->armor[3] = 0;
  ch->pcdata->carry_weight = 0;
  ch->pcdata->carry_number = 0;
  ch->pcdata->command_objs = 0;
  for (i = 0; i < NUM_STATS; i++)
    {
      ch->pcdata->stat_mod[i] = 0;
    }
  for (i = 0; i < 3; i++)
    {
      ch->pcdata->learned[race_info[ch->pcdata->race].nat_skills[i]] = 100;
      ch->pcdata->learned[align_info[ch->pcdata->alignment].nat_skills[i]] = 100;
    }
  
  if (ch->pcdata->warpoints < 0) ch->pcdata->warpoints = 0;
  if (ch->pcdata->total_wps < 0) ch->pcdata->total_wps = 0;
  LOOP (paf, ch->affected, paf->next)
    { 
      mod = paf->modifier;
      
      switch (paf->location)
	{
	default:
	  break;
	case APPLY_NONE:
	  break;
	case APPLY_AC:
	  ch->armor += mod;
	  break;
	case APPLY_HITROLL:
	  ch->hitroll += mod;
	  break;
	case APPLY_DAMROLL:
	  ch->damroll += mod;
	  break;
	case APPLY_KICK_DAMAGE:
	  ch->pcdata->plus_kick += mod;
	  break;
	case APPLY_SNEAK:
	  ch->pcdata->plus_sneak += mod;
	  break;
	case APPLY_WARMTH:
	  ch->pcdata->warmth += mod;
	  break;
	case APPLY_HIDE:
	  ch->pcdata->plus_hide += mod;
	  break;
	case APPLY_STR:
          ch->pcdata->stat_mod[STR] += mod;
          break;
	case APPLY_INT:
          ch->pcdata->stat_mod[INT] += mod;
          break;
	case APPLY_WIS:
          ch->pcdata->stat_mod[WIS] += mod;
          break;
	case APPLY_DEX:
          ch->pcdata->stat_mod[DEX] += mod;
          break;
	case APPLY_CON:
          ch->pcdata->stat_mod[CON] += mod;
          break;

	}
      
    }
  LOOP (obj, ch->carrying, obj->next_content)
    {
      ch->pcdata->carry_weight += get_obj_weight (obj);
      ch->pcdata->carry_number += get_obj_number (obj);
      if (IS_OBJ_STAT(obj, ITEM_COMMANDSCRIPT))
	ch->pcdata->command_objs++;
      if (obj->wear_loc == -1 || obj->wear_loc == WEAR_BELT_1 || obj->wear_loc == WEAR_BELT_2 || obj->wear_loc == WEAR_BELT_3)
	continue;
      if(obj->pIndexData->item_type == ITEM_ARMOR)
	{
	  r = (I_ARMOR *) obj->more;
	  if (r->max_condition == 0)
	    {
	      ch->pcdata->armor[0] += (r->protects_body);
	      ch->pcdata->armor[1] += (r->protects_head);
	      ch->pcdata->armor[2] += (r->protects_legs);
	      ch->pcdata->armor[3] += (r->protects_arms);
	    }
	  else
	    {
	      ch->pcdata->armor[2] += (r->protects_legs * r->condition_now) / (r->max_condition);
	      ch->pcdata->armor[0] += (r->protects_body * r->condition_now) / (r->max_condition);
	      ch->pcdata->armor[1] += (r->protects_head * r->condition_now) / (r->max_condition);
	      ch->pcdata->armor[3] += (r->protects_arms * r->condition_now) / (r->max_condition);
	    }
	}
      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf_next)
	{
	  mod = paf->modifier;
	  paf_next = paf->next;
	  switch (paf->location)
	    {
	    default: 
	      break;
	    case APPLY_NONE:
	      break;
	    case APPLY_AC:
	      ch->armor += mod;
	      break;
	    case APPLY_HITROLL:
	      ch->hitroll += mod;
	      break;
	    case APPLY_DAMROLL:
	      ch->damroll += mod;
	      break;
	    case APPLY_KICK_DAMAGE:
	      ch->pcdata->plus_kick += mod;
	      break;
	    case APPLY_SNEAK:
	      ch->pcdata->plus_sneak += mod;
	      break;
	    case APPLY_WARMTH:
	      ch->pcdata->warmth +=mod;
	      break;
	    case APPLY_HIDE:
	      ch->pcdata->plus_hide += mod;
	      break;
	    case APPLY_STR:
	      ch->pcdata->stat_mod[STR] += mod;
	      break;
	    case APPLY_INT:
	      ch->pcdata->stat_mod[INT] += mod;
	      break;
	    case APPLY_WIS:
	      ch->pcdata->stat_mod[WIS] += mod;
	      break;
	    case APPLY_DEX:
	      ch->pcdata->stat_mod[DEX] += mod;
	      break;
	    case APPLY_CON:
	      ch->pcdata->stat_mod[CON] += mod;
	      break;
	      
	    }
       }
      for (paf = obj->affected; paf != NULL; paf = paf_next)
	{
	  mod = paf->modifier;
	  paf_next = paf->next;
	  switch (paf->location)
	    {
	    default: 
	      break;
	    case APPLY_NONE:
	      break;
	    case APPLY_AC:
	      ch->armor += mod;
	      break;
	    case APPLY_HITROLL:
	      ch->hitroll += mod;
	      break;
	    case APPLY_DAMROLL:
	      ch->damroll += mod;
	      break;
	    case APPLY_KICK_DAMAGE:
	      ch->pcdata->plus_kick += mod;
	      break;
	    case APPLY_SNEAK:
	      ch->pcdata->plus_sneak += mod;
	      break;
	    case APPLY_WARMTH:
	      ch->pcdata->warmth += mod;
	      break;
	    case APPLY_HIDE:
	      ch->pcdata->plus_hide += mod;
	      break;
	    case APPLY_STR:
	      ch->pcdata->stat_mod[STR] += mod;
	      break;
	    case APPLY_INT:
	      ch->pcdata->stat_mod[INT] += mod;
	      break;
	    case APPLY_WIS:
	      ch->pcdata->stat_mod[WIS] += mod;
	      break;
	    case APPLY_DEX:
	      ch->pcdata->stat_mod[DEX] += mod;
	      break;
	    case APPLY_CON:
	      ch->pcdata->stat_mod[CON] += mod;
	      break;
	      
	    }
	}
    }
  check_ced(ch);
  
  ch->damroll += 
    ((is_member(ch, GUILD_WARRIOR) ? 2 : 0) + 
     (is_member(ch, GUILD_BATTLEMASTER) ? 2 : 0) + 
     ((ch->pcdata->remort_times))+
     (IS_SET (ch->ced->fight_ops, F_AGGRESSIVE) ? 2 : 0)+
     (IS_SET (ch->ced->fight_ops, F_POWER) ? 1 : 0)+
      race_info[ch->pcdata->race].combat_damage_bonus+
     align_info[ch->pcdata->alignment].combat_damage_bonus);

  
  
  ch->hitroll += 
    (LEVEL(ch)/4 +
     (IS_SET(ch->ced->fight_ops, F_AGGRESSIVE) ? -4 : 0) +
     (IS_AUGMENTED(ch, AUG_HAND_SPD) ? 4 : 0) + 
     (IS_AUGMENTED(ch, AUG_LEG_SPD) ? 4 : 0) +
     (IS_AUGMENTED(ch, AUG_FOOT_SPD) ? 4 : 0) +
     (is_member(ch, GUILD_THIEFG) ? 7 : 0) +
     (is_member(ch, GUILD_ROGUE) ? 7 : 0));
  
  ch->affected_by |= ch->pcdata->nat_abilities;
  ch->affected_by |= race_info[ch->pcdata->race].affect_bits;
  ch->more_affected_by |= ch->pcdata->nat_abilities2;
  ch->more_affected_by |= race_info[ch->pcdata->race].affect_bits2;
  ch->affected_by |= align_info[ch->pcdata->alignment].affect_bits;
  ch->more_affected_by |= align_info[ch->pcdata->alignment].affect_bits2;
  
  
  

  return;
}



void
advance_level (CHAR_DATA * ch)
{
  int add_prac;
  int add_learn;
  int ct = 0;
  char buffer[500];

  if (IS_MOB (ch))
    return;

  add_level_bonuses (ch, 1);
  add_prac = (pow.practice_percent * wis_app[get_curr_wis (ch) - 1].practice) / 100;
    add_learn = LEVEL (ch) < 4 ? 2 : 1;

  ch->pcdata->practice += add_prac;
  ch->pcdata->learn += add_learn;

  sprintf(buffer,"\x1B[1;30m-->\x1B[1;37mYou gain \x1B[1;31m%d practice(s)\x1B[1;37m and \x1B[1;31m%d learn(s).\x1B[0;37m\n\r",add_prac,add_learn);
  send_to_char(buffer,ch);

  natural_mana (ch);
  
  return;
}


void
gain_exp (CHAR_DATA * ch, int gain)
{
  char general_use[500];
  int ii=0;
  if (IS_MOB (ch) || LEVEL (ch) >= 90 || gain == 0)
    return;
  if (IN_BATTLE (ch) || CHALLENGE (ch) == 10)
    return;
  if (gain < 1)
    if (-gain >= ch->exp)
      return;
  
  ch->exp += gain;
  while (LEVEL (ch) < pow.max_level && ch->exp >= FIND_EXP (LEVEL (ch), ch->pcdata->race))
    {
      send_to_char ("You have become more powerful!\n\r", ch);
      sprintf (general_use,
               "\x1B[1;30m-->\x1B[37;0mYour overall power and learning capacities have increased!\n\r");
      send_to_char (general_use, ch);
      ch->pcdata->level++;
      advance_level (ch);
      if (ALIGN(ch) == 0)
      if (ch->pcdata->remort_times == 0 && ch->pcdata->level == pow.level_to_choose -1)
	{
	  send_to_char("You must choose your alignment next level!!!!\n\r", ch);
	  send_to_char("\x1b[1;31mPlease type help choose and aligninfo to help you decide.\n\r", ch);
	  return;
	}
      ii = clan_number (ch);
      sprintf(general_use, "%s has just gained a level! Up to level %d!!", NAME(ch), LEVEL(ch));
      if (ii > 0)
	clan_notify (general_use, ii);
      if (ALIGN(ch) == 0)
      if (LEVEL(ch) == pow.level_to_choose && ch->pcdata->remort_times == 0 && pow.choose_in_room != 0 && get_room_index(pow.choose_in_room) != NULL)
	{
	  char_from_room(ch);
	  char_to_room(ch, get_room_index(pow.choose_in_room));
	  do_look(ch, "auto");
	  send_to_char("You must now choose which path you wish to take: Evil or Good.\n\r", ch);
	}
    }
  return;
}

int
max_mana (CHAR_DATA * ch)
{
   int base;
   SINGLE_OBJECT *gem;
  if (IS_MOB (ch))
    return 500;
  base = get_curr_int(ch) + 2 + get_curr_wis(ch) +
    (((LEVEL (ch) / 3) * (get_curr_int (ch) +
			  get_curr_wis (ch))) / 9);
  if (IS_AUGMENTED(ch, AUG_MANA_BOOST))
    base = (12*base)/10;
  base = (base * (race_info[ch->pcdata->race].mana_percent + align_info[ch->pcdata->alignment].mana_percent))/100;
  if ((gem = get_item_held(ch, ITEM_GEM)) != NULL)
    base += (((I_GEM *)gem->more)->max_mana);
  return base;


}

void
mana_gain (CHAR_DATA * ch, short hehe)
{
  SINGLE_OBJECT *gem1;
  SINGLE_OBJECT *gem2;
  char general_use[500];
  /*MANA REGEN HERE */
  if (IS_MOB (ch))
    return;

  if (ch->pcdata->n_mana < ch->pcdata->n_max_mana && hehe != 2)
    {
      int tt = pow.base_mana_regen_bonus +
	((ch->position == POSITION_MEDITATING) ? pow.med_mana_regen_bonus : 0 )+
	(IS_AUGMENTED(ch, AUG_MIND_FOCUS) ? 1 : 0) +
	(IS_AUGMENTED(ch, AUG_MIND_POWER)? 1 : 0) +
	(IS_AUGMENTED(ch, AUG_MANA_BOOST) ? 1 : 0);
      ch->pcdata->n_mana = UMIN (ch->pcdata->n_max_mana, ch->pcdata->n_mana + tt); 
    }
  if ((hehe == 2) || IS_AFFECTED(ch, AFF_MINDSAP))
    {
      ch->pcdata->n_mana -= 3;
      if (ch->pcdata->n_mana <= 0)
	ch->pcdata->n_mana = 0;
    }
  gem1 = ch->hold1;
  if ( gem1 != NULL && gem1->pIndexData->item_type == ITEM_GEM)
    {
      I_GEM *gm1 = ((I_GEM *) gem1->more);
      if (hehe == 2 || IS_AFFECTED(ch, AFF_MINDSAP))
	{
	  gm1->mana_now = UMAX(gm1->mana_now - 3, 0);
	  return;
	}
      if (gm1->mana_now == gm1->max_mana) return;
      if (gm1->mana_now == (gm1->max_mana - 1))
	{
	   sprintf (general_use, "%s has been charged to its maximum capacity!\n\r",   capitalize (STRR (gem1, short_descr)));
	   send_to_char (general_use, ch);
	}
      
      if (IS_AUGMENTED(ch, AUG_MANA_BOOST)) gm1->mana_now++;
      if (hehe) gm1->mana_now++;
      else if (ch->position == POSITION_MEDITATING)
	gm1->mana_now +=(7 + (get_curr_int (ch) / 5));
      else if (ch->position == POSITION_SLEEPING)
	gm1->mana_now += (4 + (get_curr_int (ch) / 8));
      else if (ch->position == POSITION_RESTING)
	gm1->mana_now += 3;
      else
	gm1->mana_now += 2;
      if (get_curr_wis (ch) > 19)
	gm1->mana_now++;
      if (get_curr_wis (ch) > 23)
	gm1->mana_now++;
      if (gm1->mana_now > gm1->max_mana)
	gm1->mana_now = gm1->max_mana;
      else
	{
	  if (gm1->mana_now > 19 && gm1->mana_now < 30)
	    send_to_char ("Your gem begins to glow softly.\n\r", ch);
	  if (gm1->mana_now > 35 && gm1->mana_now < 45)
	    send_to_char ("Your gem begins to warm your hand with its power.\n\r", ch);
	  if (gm1->mana_now > 61 && gm1->mana_now <100 )
	    send_to_char ("Your gem hums with powerful magical energy!\n\r", ch);
	}
    }
  gem2 = ch->hold2;
  if ( gem2 != NULL && gem2->pIndexData->item_type == ITEM_GEM)
    {
      I_GEM *gm2 = ((I_GEM *) gem2->more);
      if (hehe == 2 || IS_AFFECTED(ch, AFF_MINDSAP))
	{
	  gm2->mana_now = UMAX(gm2->mana_now - 3, 0);
	  return;
	}
      if (gm2->mana_now == gm2->max_mana) return;
      if (gm2->mana_now == (gm2->max_mana - 1))
	{
	   sprintf (general_use, "%s has been charged to its maximum capacity!\n\r",   capitalize (STRR (gem2, short_descr)));
	   send_to_char (general_use, ch);
	}
      
      if (IS_AUGMENTED(ch, AUG_MANA_BOOST)) gm2->mana_now++;
      if (hehe) gm2->mana_now++;
      else if (ch->position == POSITION_MEDITATING)
	gm2->mana_now +=(7 + (get_curr_int (ch) / 5));
      else if (ch->position == POSITION_SLEEPING)
	gm2->mana_now += (4 + (get_curr_int (ch) / 8));
      else if (ch->position == POSITION_RESTING)
	gm2->mana_now += 3;
      else
	gm2->mana_now += 2;
      if (get_curr_wis (ch) > 19)
	gm2->mana_now++;
      if (get_curr_wis (ch) > 23)
	gm2->mana_now++;
      if(gm2->mana_now > gm2->max_mana)
	gm2->mana_now = gm2->max_mana;
      else
	{
	  
	  if (gm2->mana_now > 19 && gm2->mana_now < 22)
	    send_to_char ("Your gem begins to glow softly.\n\r", ch);
	  if (gm2->mana_now > 32 && gm2->mana_now < 35)
	    send_to_char ("Your gem begins to warm your hand with its power.\n\r", ch);
	  if (gm2->mana_now > 61 && gm2->mana_now < 64)
	    send_to_char ("Your gem hums with powerful magical energy!\n\r", ch);
	}
    }
  return;
}

/*
   * Regeneration stuff.
 */
int
hit_gain (CHAR_DATA * ch)
{
  int gain;
  if (FIGHTING (ch))
    return 0;
  gain = pow.base_hp_regen;
  switch (ch->position)
    {
    case POSITION_SLEEPING:
      gain += pow.hp_bonus_sleeping + (number_range (1, get_curr_con (ch) / 4));
      break;
    case POSITION_RESTING:
      gain += pow.hp_bonus_resting + (number_range (1, get_curr_con (ch) / 8));
      break;
    }
#ifdef NEW_WORLD
  if (IS_MOB (ch) && ch->hit < (ch->max_hit / 2) && !FIGHTING (ch))
    {
      gain += LEVEL (ch) / 5;
    }
#else
  if (IS_MOB(ch)) {
        gain += LEVEL(ch)/5;
        }
#endif
  if (IS_PLAYER (ch))
    {
      if (ch->pcdata->condition[COND_FULL] <= 5)
        gain /= 2;
      if (ch->pcdata->condition[COND_FULL] <= 0)
        gain = 1;
      if (ch->pcdata->condition[COND_THIRST] <= 5)
        gain /= 2;
      if (ch->pcdata->condition[COND_THIRST] <= 0)
        gain = 1;
    }
  if (IS_AFFECTED (ch, AFF_POISON)
      && (IS_MOB (ch)))
    {
      gain /= 3;
    }
  if (IS_AFFECTED (ch, AFF_POISON)
      && (IS_PLAYER (ch)))
    {
      gain /= 4;
    }
  if (IS_AFFECTED (ch, AFF_PLAGUE))
    gain /= 8;
  if (IS_PLAYER (ch))
    {
      gain = (gain * (race_info[ch->pcdata->race].regen_hit_percent + align_info[ch->pcdata->alignment].regen_hit_percent)) / 100;
    }
  if (IS_PLAYER(ch) && is_member(ch, GUILD_MONK) && ch->pcdata->learned[gsn_spiritregen] > 60)
    gain *= 2;
  return UMIN (gain, ch->max_hit - ch->hit);
}

int
move_gain (CHAR_DATA * ch)
{
  int gain = 0;
  if (FIGHTING (ch))
    return 1;
  gain = pow.base_mp_regen;
  switch (ch->position)
    {
    case POSITION_SLEEPING:
      gain += pow.mp_bonus_sleeping + get_curr_dex (ch) / 2;
      break;
    case POSITION_RESTING:
      gain += pow.mp_bonus_resting + get_curr_dex (ch) / 3;
      break;
    }
  if (IS_PLAYER (ch))
    {
      if (ch->pcdata->condition[COND_FULL] <= 0)
        {
          gain = 1;
        }
      if (ch->pcdata->condition[COND_THIRST] <= 0)
        gain = 1;
    }
  if (IS_AFFECTED (ch, AFF_POISON))
    gain /= 3;
  if (IS_AFFECTED (ch, AFF_PLAGUE))
    gain /= 4;
  if (IS_PLAYER (ch))
    {
      gain = (gain * (race_info[ch->pcdata->race].regen_move_percent + align_info[ch->pcdata->alignment].regen_move_percent)) / 100;
    }
  if (IS_PLAYER(ch) && is_member(ch, GUILD_MONK) && ch->pcdata->learned[gsn_spiritregen] > 60)
    gain *= 2;
  return UMIN (gain, ch->max_move - ch->move);
}

void
gain_condition (CHAR_DATA * ch, int iCond, int value)
{
  int condition;
  if (value == 0 || IS_MOB (ch))
    return;
  if (!ch->in_room || ch->in_room->vnum < 1000) return;
  ch->pcdata->condition[iCond] += value;
  if (ch->pcdata->condition[COND_DRUNK] < 0)
    ch->pcdata->condition[COND_DRUNK] = 0;
  if (ch->pcdata->condition[iCond] > 48)
    ch->pcdata->condition[iCond] = 48;
  condition = ch->pcdata->condition[iCond];
  if (ch->pcdata->condition[iCond] < -2)
    {
      switch (iCond)
        {
        case COND_FULL:
          send_to_char ("You hurt from hunger!\n\r", ch);
          break;
        case COND_THIRST:
          send_to_char ("You hurt from thirst!\n\r", ch);
          break;
        default:
          return;
        }
      if (ch->desc != NULL)
        {
          if (ch->hit + (condition * 10) < 0)
            {
              SUBHIT(ch,1);
              
              update_pos (ch);
            }
          else
            {
              ADDHIT(ch,(condition * 5));
            }
          
          if (ch->hit == -1)
            {
              send_to_char ("Soooo verrrryyyy hunnnnggrrryyyy....\n\r", ch);
              ch->pcdata->condition[COND_THIRST] = 48;
              ch->pcdata->condition[COND_FULL] = 48;
              strcpy (reasonfd, "Starvation");
              raw_kill (ch, FALSE);
              send_to_char ("Your vision blurs as the world slips away around you...\n\r", ch);
              /* MAXHIT(ch); */
              
              update_pos (ch);
              return;
            }
          return;
        }
    }

  if (ch->pcdata->condition[iCond] == 9)
    {
      switch (iCond)
        {
        case COND_FULL:
          send_to_char ("You are beginning to get hungry... better get some food.\n\r", ch);
          break;
        case COND_THIRST:
          send_to_char ("You are beginning to get thirsty.\n\r", ch);
          break;
        }
    }
  if (ch->pcdata->condition[iCond] <= 1)
    {
      switch (iCond)
        {
        default:
          break;
        case COND_FULL:
          send_to_char ("You begin to feel yourself weaken from intense hunger!\n\r", ch);
          break;
        case COND_THIRST:
          send_to_char ("You feel so dehydrated that your vision begins to blur!\n\r", ch);
          break;
        }
    }
  if (ch->pcdata->condition[iCond] == 3)
    {
      switch (iCond)
        {
        default:
          break;
        case COND_FULL:
          send_to_char ("Your stomach rumbles with hunger.\n\r", ch);
          break;
        case COND_THIRST:
          send_to_char ("You begin to feel dehydrated.\n\r", ch);
          break;
        case COND_DRUNK:
          if (condition != 0)
            send_to_char ("You are sober.\n\r", ch);
          break;
        case COND_STARVING:
          send_to_char ("You are STARVING! Get some food!\n\r", ch);
          break;
        }
    }
  return;
}


void
mobile_update (void)
{
  char buffy[STD_LENGTH];
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  EXIT_DATA *pexit;
  int door;
  buffy[0] = '\0';
/* Examine all mobs. */
  LOOP (ch, char_list, ch_next)
  {
    ch_next = ch->next;
    if (ch->in_room == NULL)
      {
        CHAR_DATA *fx;
        for (fx = char_list; fx != NULL; fx = fx->next)
          {
            if (FIGHTING (fx) && FIGHTING (fx) == ch)
	      {
              fx->fgt->fighting = NULL;
	      }
	  }
        fprintf (stderr, "Extracted mob/char that was not in any room...\n");
        extract_char (ch, TRUE);
        continue;
      }
if (FIGHTING(ch) && FIGHTING(ch)->data_type==50) {
        fprintf(stderr,"That was it2!\n");
        ch->fgt->fighting=NULL;
        }
    if (IS_PLAYER (ch))
      {
        if (ch->position != POSITION_FIGHTING && ch->position != POSITION_CASTING &&
            ch->position != POSITION_SEARCHING && ch->position != POSITION_BASHED &&
            ch->position != POSITION_BACKSTAB && ch->position != POSITION_CIRCLE &&
            ch->position != POSITION_GROUNDFIGHTING)
          ch->wait = 0;
        if (FIGHTING (ch) == NULL && ch->position == POSITION_BASHED)
          NEW_POSITION(ch, POSITION_STANDING);
        if (ch->wait > 0)
          ch->wait -= 13;
        if (ch->wait < 0)
          ch->wait = 0;
        continue;
      }
    else
      {
	if (ch->in_room->area->nplayer < 1 || IS_AFFECTED (ch, AFF_CHARM))
	  continue;
	if (ch->pIndexData->spec_fun && (ch->position != POSITION_GROUNDFIGHTING) && (ch->position != POSITION_BASHED))
	  {
	    
	    if ((*ch->pIndexData->spec_fun) (ch))
	      continue;
	  }
	
	if(ch->pIndexData->act4 >= ACT4_KILL_OPP)
	  {
	    if (FIGHTING(ch) != NULL && 
		IS_SET(ch->pIndexData->act4, ACT4_SUMMONER) &&  
		ch->pIndexData->opt != NULL &&
		ch->pIndexData->opt->summons > 1 && 
		(get_mob_index(ch->pIndexData->opt->summons) !=NULL))
	      {
		int chan = LEVEL(ch)/2;
		CHAR_DATA * summoned;
		if (chan > 10) chan = 10;
		if (number_percent() < chan)
		  {
		    summoned = create_mobile(get_mob_index(ch->pIndexData->opt->summons));
		    char_to_room(summoned, ch->in_room);
		    act("$n barks a single syllable and suddenly $N appears!", ch, NULL, summoned, TO_ROOM);
		    check_fgt(summoned);
		    summoned->fgt->fighting = ch->fgt->fighting;
		    
		  }
		
	      }
	    if (IS_SET (ch->pIndexData->act4, ACT4_REST) && FIGHTING (ch) == NULL)
	      {
		if (ch->next_in_room && (ch->position == POSITION_RESTING || ch->position == POSITION_SLEEPING))
		  NEW_POSITION(ch, POSITION_STANDING);
		else if (ch->position == POSITION_STANDING)
		  {
		    /* If a mob's exhausted, let it rest. */
		    if (ch->move < ch->max_move / 4)
		      do_sleep (ch, "");
		    else if (ch->move < ch->max_move / 2)
		      do_rest (ch, "");
		    if (ch->hit < ch->max_hit / 7)
		      do_sleep (ch, "");
		    else if (ch->hit < ch->max_hit / 4)
		      do_rest (ch, "");
		  }
	      }
	    if (ch->position != POSITION_STANDING)
	      continue;
	    if (ch->in_room->more && IS_SET (ch->pIndexData->act4, ACT4_SLEEPAGGRO) && (FIGHTING (ch) == NULL))
	      {
		CHAR_DATA *tch;
		CHAR_DATA *tch_next;
		for (tch = ch->in_room->more->people; tch != NULL; tch = tch_next)
		  {
		    if (!tch->next_in_room)
		      tch_next = NULL;
		    else
		      tch_next = tch->next_in_room;
		    if (IS_PLAYER (tch) && tch->position == POSITION_SLEEPING)
		      {
			multi_hit(ch, tch, TYPE_UNDEFINED);
			break;
		      }
		  }
	      }
	    
	    if (FIGHTING (ch) == NULL)
	      {
		if (ch->in_room->more && ch->in_room->more->pcs > 0)
		  {
		    
		    if (IS_SET (ch->pIndexData->act4, ACT4_KILL_OPP) && FIGHTING (ch) == NULL)
		      {
			CHAR_DATA *to_attack;
			for (to_attack = ch->in_room->more->people; to_attack != NULL; to_attack = to_attack->next_in_room)
			  {
			    if (to_attack == ch || (IS_PLAYER (to_attack) && to_attack->pcdata->level > IMM_LEVEL))
			      continue;
			    if (!DIFF_ALIGN(ch, to_attack))
			      continue;
			    if (to_attack != NULL && can_see (ch, to_attack))
			      {
				if (ch->position == POSITION_RESTING)
				  {
				    do_stand (ch, "");
				  }
				if (ch->position == POSITION_STANDING)
				  {
				    set_fighting (ch, to_attack);
				    break;
				  }
				break;
			      }
			  }
		      }
		    if (FIGHTING(ch) == NULL && IS_SET(ch->pIndexData->act4, ACT4_KILL_CLAN))
		      {
			CHAR_DATA *tcr;
			CLAN_DATA *clandata;
			for (tcr = ch->in_room->more->people; tcr != NULL; tcr = tcr->next_in_room)
			  {
			    if (IS_MOB(tcr)) continue;
			    if (tcr->pcdata->clan == -1) continue;
			    if ((clandata = get_clan_index(tcr->pcdata->clan)) == NULL) continue;
			    if(clandata->shitlist == TRUE && can_see(ch, tcr))
			      {
				char clanbuf[200];
			        sprintf(clanbuf, "All of you %s scum need to die now, %s!", clandata->name, NAME(tcr));
				do_say(ch, clanbuf);
				multi_hit(ch, tcr, TYPE_UNDEFINED);
				break;
			      }
			  }
		      }
		    if (IS_SET (ch->pIndexData->act4, ACT4_ASSISTALL))
		      {
			CHAR_DATA *tcr;
			for (tcr = ch->in_room->more->people; tcr != NULL; tcr = tcr->next_in_room)
			  {
			    if (IS_MOB (tcr))
			      continue;
			    if (FALSE)
			      {
				set_fighting (ch, tcr->fgt->fighting);
				break;
			      }
			  }
		      }
		    if (IS_SET (ch->pIndexData->act4, ACT4_RESCUEONE))
		      do_rescue (ch, "prtct");
		    if (IS_SET (ch->pIndexData->act4, ACT4_RESCUETWO))
		      do_rescue (ch, "codeone");
		    if (IS_SET (ch->pIndexData->act4, ACT4_ASSISTONE))
		      do_assist (ch, "aone");
		    if (IS_SET (ch->pIndexData->act4, ACT4_ASSISTTWO))
		      do_assist (ch, "atwo");
		  }
		if (IS_SET (ch->pIndexData->act4, ACT4_KILLALLONE) && FIGHTING (ch) == NULL)
		  {
		    CHAR_DATA *to_attack;
		    for (to_attack = ch->in_room->more->people; to_attack != NULL; to_attack = to_attack->next_in_room)
		      {
			if (to_attack == ch || (IS_PLAYER (to_attack) && to_attack->pcdata->level > IMM_LEVEL))
			  continue;
			if (IS_MOB (to_attack) && IS_SET (to_attack->pIndexData->act4, ACT4_KILLALLONE))
			  continue;
			break;
		      }
		    if (to_attack != NULL && can_see (ch, to_attack))
		      {
			if (ch->position == POSITION_RESTING)
			  {
			    do_stand (ch, "");
			  }
			if (ch->position != POSITION_STANDING)
			  goto pasthere;
			set_fighting (ch, to_attack);
			break;
		      }
		  }
		if (IS_SET (ch->pIndexData->act4, ACT4_KILLALLTWO) && FIGHTING (ch) == NULL)
		  {
		    CHAR_DATA *to_attack;
		    for (to_attack = ch->in_room->more->people; to_attack != NULL; to_attack = to_attack->next_in_room)
		      {
			if (to_attack == ch || (IS_PLAYER (to_attack) && to_attack->pcdata->level > IMM_LEVEL))
			  continue;
			if (IS_MOB (to_attack) && IS_SET (to_attack->pIndexData->act4, ACT4_KILLALLTWO))
			  continue;
			break;
		      }
		    if (to_attack != NULL && can_see (ch, to_attack))
		      {
			if (ch->position == POSITION_RESTING)
			  {
			    do_stand (ch, "");
			  }
			if (ch->position != POSITION_STANDING)
			  goto pasthere;
			set_fighting (ch, to_attack);
			break;
		      }
		  }
	      pasthere:
		if (IS_SET (ch->pIndexData->act4, ACT4_KILLGLADIATOR))
	      do_kill (ch, "gladiator");
		if (IS_SET (ch->pIndexData->act4, ACT4_RANGER))
		  do_kill (ch, "beast");
		if (IS_SET (ch->pIndexData->affected_by, AFF_HIDE)
		    && !IS_SET (ch->affected_by, AFF_HIDE)
		    && ch->next_in_room == NULL)
		  SET_BIT (ch->affected_by, AFF_HIDE);
	      }
	  }
	/* Wander */
	if (!IS_SET (ch->act, ACT_SENTINEL)
	    && (MOUNTED_BY (ch) == NULL)
	    && !IS_SET (ch->pIndexData->act3, ACT3_FOLLOW)
	    && (door = number_bits (5)) <= 5
	    && (pexit = ch->in_room->exit[door]) != NULL
	    && (ROOM_DATA *) pexit->to_room != NULL
	    && (!pexit->d_info || !IS_SET (pexit->d_info->exit_info, EX_CLOSED))
	    && !IS_SET (((ROOM_DATA *) pexit->to_room)->room_flags, ROOM_NO_MOB)
	    && number_range (1, 3) == 2
	    && (!IS_SET (ch->act, ACT_STAY_SECTOR)
		|| ((ROOM_DATA *) pexit->to_room)->sector_type == ch->in_room->sector_type)
	    && (!IS_SET (ch->act, ACT_STAY_AREA)
		|| ((ROOM_DATA *) pexit->to_room)->area == ch->in_room->area))
	  {
	    move_char (ch, door);
	    if (ch->position < POSITION_STANDING)
	      continue;
	  }
    /* Flee */
	if (ch->hit < ch->max_hit / 10
	    && (!IS_SET (ch->act, ACT_SENTINEL))
	    && (door = number_bits (3)) <= 5
	    && (pexit = ch->in_room->exit[door]) != NULL
	    && (ROOM_DATA *) pexit->to_room != NULL
	    && (!pexit->d_info || !IS_SET (pexit->d_info->exit_info, EX_CLOSED))
	    && !IS_SET (((ROOM_DATA *) pexit->to_room)->room_flags, ROOM_NO_MOB))
	  {
	    CHAR_DATA *rch;
	    bool found;
	    found = FALSE;
	    if (((ROOM_DATA *) pexit->to_room)->more)
	      {
		for (rch = ((ROOM_DATA *) pexit->to_room)->more->people;
		     rch != NULL;
		     rch = rch->next_in_room)
		  {
		    if (IS_PLAYER (rch))
		      {
			found = TRUE;
			break;
		      }
		  }
		if (!found)
		  move_char (ch, door);
	      }
	  }
      }
  }
  return;
}

void
weather_update (void)
{
  char buf[STD_LENGTH];
  char general_use[500];
  int diff;
  CHAR_DATA *wch;
  CHAR_DATA *ch_next;
  CHAR_DATA *pMob;
  SCRIPT_INFO *scr;
  SCRIPT_INFO *scr_next;
  SINGLE_TRIGGER *tr;
  SCRIPT_INFO *s;
  MOB_PROTOTYPE *pMobIndex;
  int lightning = 0;
  int extracted = 0;
  int newday = 0;
  buf[0] = '\0';


/* Check running scripts for delay resumes */
  for (scr = info_list; scr != NULL; scr = scr_next)
    {
      scr_next = scr->next;
      if (scr->delay_ticks <= 0)
        continue;
      if (scr->tick_type != 3)
        continue;
      scr->delay_ticks--;
      if (scr->delay_ticks == 0)
        {
          execute_code (scr);
        }
    }

  if (ticks_to_reboot != 0)
    {
      if (ticks_to_reboot == 1)
        {
          CHAR_DATA *bah;
          FILE *fpqr;
          extern bool aturion_down;
          if (IS_MOB(char_list))
          do_asave (char_list, "changed");
          fprintf (stderr, "Auto-saved changed areas.\n");
          for (bah = char_list; bah != NULL; bah = bah->next)
            {
              if (IS_PLAYER (bah))
                {
                  do_save (bah, "xx2xx11");
                }
            }
          fprintf (stderr, "Auto-saved all players.\n");
          fprintf (stderr, "Auto-reboot invoked...\n");
          fpqr = fopen ("reason.fi", "w+");
          fprintf (fpqr, "Mud-Auto-Reboot \n\rEnd~\n\r");
          fclose (fpqr);
          aturion_down = TRUE;
          return;
        }


      --ticks_to_reboot;
      sprintf (general_use, "\x1B[1;34m----[\x1B[32mMud Autosave and Reboot in \x1B[37m%d\x1B[32m game ticks/hour%s!\x1B[34m]----\x1B[37;0m",ticks_to_reboot,
               (ticks_to_reboot == 1 ? "" : "s"));
      {
	CHAR_DATA *sch;
	sch=create_mobile(get_mob_index(1));
	char_to_room(sch,get_room_index(2));
	do_echo (sch, general_use);
	extract_char(sch,TRUE);
      }
    }
  if (ticks_to_battlefield != 0)
    {
      CHAR_DATA *sch;
      sch=create_mobile(get_mob_index(1));
      char_to_room(sch,get_room_index(2));

      if (ticks_to_battlefield == 1)
        {
          allow_anything = TRUE;
	  {
	    do_transfer (sch, "all Battleground");
	  }
          allow_anything = FALSE;
        }
      --ticks_to_battlefield;
        if(ticks_to_battlefield > 0)
          {
            int jj;
            bool hasprize = FALSE;
            sprintf (general_use, "\x1B[1;34m-->\x1B[37m%d\x1B[34m game hour%s to battleground! [Levels %d-%d]\x1B[37;0m",
		   ticks_to_battlefield, (ticks_to_battlefield == 1 ? "" : "s"),
                   min_level, max_level);
	    do_echo (sch, general_use);
            for (jj = 0; jj < MAX_NUM_BG_PRIZES; jj++)
             {
              if (bg_prize[jj] != NULL)
                {

sprintf(general_use,"\x1b[1;37m-->\x1b[1;31mBattleground\x1b[1;37m Prize:\x1b[0;37m%s",format_obj_to(bg_prize[jj], sch, TRUE));
                    do_echo (sch, general_use);
                   hasprize = TRUE;
         	}
             }
            if (!hasprize)
             {
                do_echo(sch, "\x1b[1;37mNo Prizes.\x1b[0;37m\n\r");
             }
            if (ticks_to_battlefield > 2)
    	    {
   	      do_echo (sch, "\x1B[37;0mTo sign up for the battleground, type battle.");
	    }
       }
      
      extract_char(sch, TRUE);
    }
  /*Check for any winners in the battleground */
  if (ticks_to_battlefield == 0)
    {
      DESCRIPTOR_DATA *dd;
      CHAR_DATA *found_winner;
      ROOM_DATA *rid;
      int cnt;
      int jj;
      char general_use[500];
      int people_in_bground;
      people_in_bground = 0;
      found_winner = NULL;
      for (dd = descriptor_list; dd != NULL; dd = dd->next)
        {
          if (!dd->character || dd->connected != CON_PLAYING)
            continue;
          if (dd->character->in_room && dd->character->in_room->vnum >= (BATTLEGROUND_START_VNUM + bg_multiplier) &&
              dd->character->in_room->vnum <= (BATTLEGROUND_END_VNUM + bg_multiplier))
            {
              people_in_bground++;
              found_winner = dd->character;
            }
        }
      if (people_in_bground == 1 && found_winner)
        {
          /*Below checks for mobs that may still be in bground */
          for (cnt = BATTLEGROUND_START_VNUM + bg_multiplier; cnt < BATTLEGROUND_END_VNUM + bg_multiplier; cnt++)
            {
              if ((rid = get_room_index (cnt)) == NULL)
                continue;
              if ((rid->more && rid->more->people != NULL && rid->more->people != found_winner)
                 || (rid->more && rid->more->people && rid->more->people->next_in_room))
                {
                  found_winner = NULL;
                  break;
                }
            }
          if (found_winner != NULL && IS_PLAYER (found_winner))
            {
              char_from_room (found_winner);
              char_to_room(found_winner,get_room_index(found_winner->pcdata->alignment+100));
              sprintf (general_use, "\x1B[1;34m%s is the battleground winner! \x1B[1;37mCongratulations!\x1B[0m", NAME (found_winner));
	      {
		CHAR_DATA *sch;
		sch=create_mobile(get_mob_index(1));
		char_to_room(sch,get_room_index(2));
		do_echo (sch, general_use);
		do_restore (sch, NAME (found_winner));
		extract_char(sch,TRUE);
	      }
              for(jj = 0; jj <MAX_NUM_BG_PRIZES; jj++)
                {
                 if(bg_prize[jj] != NULL)
                   {
                     obj_from(bg_prize[jj]);
                     obj_to(bg_prize[jj], found_winner);
                     bg_prize[jj] = NULL;
                   }
                }  
              send_to_char ("Congratulations! You've won the battleground!\n\r", found_winner);
            }
        }
   }  
  room_update ();
  switch (++time_info.hour)
    {
    case 2:
      strcat(buf, "Unseen hungry creatures howl off in the distance.\n\r");
      break;
    case 6:
      weather_info.sunlight = SUN_RISE;
      strcat (buf, "The first rays of sunlight appear to the east.\n\r");
      if (time_info.month <= 4 || time_info.month >= 17)
	weather_info.temperature = 35 + number_range(1,15);
      else
	weather_info.temperature = 65 + number_range(1,15);
      break;
    case 7:
      weather_info.sunlight = SUN_MORNING;
      strcat (buf, "A new day begins as the sun peeks over the eastern horizon.\n\r");
      weather_info.temperature += number_range(1,8);
      break;
    case 11:
      weather_info.sunlight = SUN_MIDDAY;
      strcat (buf, "The sun crawls overhead.\n\r");
      weather_info.temperature += number_range(1,12);
      break; 
    case 14:
      weather_info.sunlight = SUN_AFTERNOON;
      strcat (buf, "The sun passes overhead and begins its descent to the west.\n\r");
      weather_info.temperature += number_range(1,9);
      break;
    case 17:
      weather_info.sunlight = SUN_EVENING;
      strcat(buf, "The rays of the sun begin to cast long shadows over the land.\n\r");
      weather_info.temperature -= number_range(1,9);
      break;
    case 20:
      weather_info.sunlight = SUN_SET;
      strcat (buf, "The sky begins to darken in anticipation of the night to come.\n\r");
      weather_info.temperature -= number_range(1,12);
      break;
    case 21:
      weather_info.sunlight = SUN_DARK;
      strcat (buf, "The night has begun.\n\r");
      weather_info.temperature -= number_range(1,8);
      break;
    case 24:
      weather_info.temperature -= number_range(1,10);
      time_info.hour = 0;
      time_info.day++;
      day_counter++;
      save_day_counter ();
      newday = 1;
      break;
    }
  if (time_info.day >= 35)
    {
      time_info.day = 0;
      time_info.month++;
    }
  if (time_info.month >= 17)
    {
      strcat (buf, "It is the dawn of a New Year!!!\n\r");
      time_info.month = 0;
      time_info.year++;
    }
  /*
   * Weather change.
   */
  weather_info.winddir += number_range (0, 2) - 1;
  if (time_info.month >= 3 && time_info.month <= 19)
    diff = weather_info.mmhg > 985 ? -2 : 2;
  else
    diff = weather_info.mmhg > 1015 ? -2 : 2;
  weather_info.change += diff * dice (1, 4) + dice (2, 6) - dice (2, 6);
  weather_info.change = UMAX (weather_info.change, -12);
  weather_info.change = UMIN (weather_info.change, 12);
  weather_info.mmhg += weather_info.change;
  weather_info.mmhg = UMAX (weather_info.mmhg, 960);
  weather_info.mmhg = UMIN (weather_info.mmhg, 1040);
  switch (weather_info.sky)
    {
    default:
      bug ("Weather_update: bad sky %d.", weather_info.sky);
      weather_info.sky = SKY_CLOUDLESS;
      break;
    case SKY_CLOUDLESS:
      if ((((time_info.hour > 14 && time_info.hour < 16) || (time_info.hour > 21 && time_info.hour < 24)) && number_range(1,5) == 3) || (number_range(1,50) == 5))
	{
          if (time_info.month <= 4 || time_info.month >= 17)
            {
              strcat (buf, "A few flakes of snow are falling... there is definately a chill in the air!\n\r");
              weather_info.temperature -= 10;
            }
          else
            {
              if (number_range (1, 2) == 2)
                strcat (buf, "Large, thick clouds begin to form, blocking the sky from view.\n\r");
              else
                strcat (buf, "Low clouds begin to form, obstructing the sky from your view.\n\r");
            }
          weather_info.sky = SKY_CLOUDY;
          weather_info.windspeed += 10;
        }
      break;
    case SKY_CLOUDY:
      if(number_range(1,3) == 2)
	{
	  if (time_info.hour > 15 || number_range(1,4) == 3)
	    {
	      if (time_info.month <= 4 || time_info.month >= 17)
		{
		  strcat (buf, "It begins to snow, and the air grows bitter cold.\n\r");
		  weather_info.temperature -= 6;
		}
	      else if (weather_info.temperature < 75
		       && weather_info.temperature > 45
		       && number_range(1,40) == 3)
		{
		  strcat (buf, "A thick layer of fog rolls in.\n\r");
		  weather_info.sky = SKY_FOGGY;
		  weather_info.temperature -= 2;
		  break;
		}
	      else
		{
		  int hash;
		  ROOM_DATA *ridd;
		  TRACK_DATA *tr;
		  TRACK_DATA *tr_n;
		  PTRACK_DATA *ptr;
		  PTRACK_DATA *ptr_n;
		  bool stp = FALSE;
		  strcat (buf, "Dark clouds in the sky begin to pour rain down upon the lands.\n\r");
		  /* CLEAR ALL TRACKS!! WASHED AWAY */
		  for (hash = 0; hash < HASH_MAX; hash++)
		    {
		      for (ridd = room_hash[hash]; ridd != NULL; ridd = ridd->next)
			{
			  for (tr = ridd->tracks; !stp && tr; tr = tr_n)
			    {
			      tr_n = tr->next_track_in_room;
			      if (tr_n == NULL)
				stp = TRUE;
			      free_m (tr);
			    }
			  stp = FALSE;
			  ridd->tracks = NULL;
			}
		    }
		  /* JRAJRA - GET rid of PTRACKS! After rain... */
		  for (hash = 0; hash < HASH_MAX; hash++)
		    {
		      for (ridd = room_hash[hash]; ridd != NULL; ridd = ridd->next)
			{
			  for (ptr = ridd->ptracks; !stp && ptr; ptr = ptr_n)
			    {
			      ptr_n = ptr->next_track_in_room;
			      if (ptr_n == NULL)
				stp = TRUE;
			      free_m (ptr);
			    }
			  stp = FALSE;
			  ridd->ptracks = NULL;
			}
		    }
		}
	      weather_info.sky = SKY_RAINING;
	      weather_info.windspeed += 10;
	    }
	}
      else
	{
	  if (number_range(1,2) == 2)
	    {
	      if (time_info.month <= 3 || time_info.month >= 17)
		{
		  strcat (buf, "The snow lets up, and the temperature rises slightly.\n\r");
		  weather_info.temperature += 10;
		}
	      else
		strcat (buf, "The clouds disappear, and the beautiful, clear sky is visible above.\n\r");
	      weather_info.sky = SKY_CLOUDLESS;
	      weather_info.windspeed -= 10;
	    }
	}
      break;
    case SKY_RAINING:
      if(number_range(1,6) == 2)
	{
          if (time_info.month <= 4 || time_info.month >= 17)
            {
              strcat (buf, "You are caught in a blizzard... It's *damn* cold!\n\r");
              weather_info.temperature -= 10;
            }
          else
            strcat (buf, "Lightning flashes in the sky; it looks as if a storm is brewing.\n\r");
          weather_info.sky = SKY_LIGHTNING;
          weather_info.windspeed += 10;
        }
      else if(number_range(1,4) == 2)
        {
          if (time_info.month <= 4 || time_info.month >= 17)
            {
              strcat (buf, "The snow lets up, and the temperature rises.\n\r");
              weather_info.temperature += 30;
            }
          else
            strcat (buf, "The rain has stopped.\n\r");
          weather_info.sky = SKY_CLOUDY;
          weather_info.windspeed -= 10;
        }
      break;
    case SKY_FOGGY:
      if (number_range(1,8) == 1)
        {
          strcat (buf, "The fog slowly dissolves...\n\r");
          weather_info.sky = SKY_CLOUDY;
          weather_info.temperature += 5;
        }
      break;
    case SKY_LIGHTNING:
      if(number_range(1,4) != 3)
	{
	  if (time_info.month <= 4 || time_info.month >= 17)
	    {
	      strcat (buf, "The blizzard subsides.\n\r");
              weather_info.temperature += 10;
	    }
	  else
	    strcat (buf, "The lightning has stopped.\n\r");
	  weather_info.sky = SKY_RAINING;
	  weather_info.windspeed -= 10;
	}
      break;
    }
/* Examine all mobs. */
  for (wch = char_list; wch != NULL; wch = ch_next)
    {
      if (!wch->next)
        ch_next = NULL;
      else
        ch_next = wch->next;
      if (!wch->in_room)
        continue;
      
      if (IS_PLAYER(wch))
	{
	  if(wch->pcdata->no_quit > 0)
	    wch->pcdata->no_quit--; 
	  if ((wch->pcdata->no_quit_pk > 0) && (FIGHTING (wch) == NULL || wch->position != POSITION_FIGHTING))
	    wch->pcdata->no_quit_pk--;
	  if(wch->in_room && wch->in_room->sector_type != SECT_INSIDE && 
	     !IS_SET(wch->in_room->room_flags, ROOM_INDOORS) &&
	     !IS_SET(wch->in_room->room_flags, ROOM_UNDERGROUND) &&
	     wch->in_room->sector_type != SECT_UNDERWATER &&
	     wch->in_room->sector_type < SECT_ASTRAL)
	    {
	      if (weather_info.sky == SKY_LIGHTNING && dice (1, 500) == 6
		  && !lightning)
		{
		  send_to_char ("You feel your hair stand on your neck...", wch);
		  send_to_char ("ZAP!!\n\r", wch);
		  act ("Watch out! $n just got struck by lightning!", wch, NULL,
		       NULL, TO_ROOM);
		  if (wch->hit > 31)
                    {
                      SUBHIT(wch, 20);
                    }
		  if (wch->data_type==50) continue;
		  lightning = 1;
		}
	      if (buf[0] != '\0')
		{
		  if (wch->desc != NULL && 
		      wch->desc->connected == CON_PLAYING
		      && IS_AWAKE (wch))
		    {
		      if (((wch->in_room->sector_type == SECT_AIR || wch->in_room->sector_type == SECT_CLOUDS)) && find_str (buf, "fog"))
			continue;
		      if (wch->in_room->sector_type > SECT_CLOUDS &&  wch->in_room->sector_type < SECT_TROPICAL && (!find_str (buf, "snow") || !find_str (buf, "blizzard")   || !find_str (buf, "cloud")))
			continue;
		      
		      if ((wch->in_room->sector_type == SECT_DESERT || wch->in_room->sector_type == SECT_WASTELAND)  && (find_str (buf, "snow") || find_str (buf, "blizzard")))
			continue;
		      if (wch->in_room->sector_type == SECT_TROPICAL  && (find_str (buf, "snow") || find_str (buf, "blizzard")))
			continue;
		      
		      send_to_char (buf, wch);
		    }
		}
	    }
	  continue;
	}
    check_a1:
/* Check for triggers on mobbies! */
      for (tr = trigger_list[TEVERY_HOUR]; tr != NULL; tr = tr->next)
        {
          if (wch->pIndexData->vnum == tr->attached_to_mob)
            {
              if (tr->players_only)
                continue;
              if (tr->running_info && !tr->interrupted)
                continue;       /* Already running, interrupted, but script says not to allow interruptions. */
              if (tr->running_info && tr->interrupted != 2)
                {
                  end_script (tr->running_info);
                  goto check_a1;
                }
              /* ----------------- */
              /* Start the script! */
              /* ----------------- */
              tr->running_info = mem_alloc (sizeof (*tr->running_info));
              s = tr->running_info;
              bzero (s, sizeof (*s));
              s->current = wch;
              s->mob = wch;
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



      /*
         Handles shops creating things - by Owen :)
       */
      if (wch->pIndexData->pShop != NULL && wch->pIndexData->pShop->creates_vnum != 0)
        {
          SHOP_DATA *pShop;
          SINGLE_OBJECT *obj;
          SINGLE_OBJECT *ooo;
          OBJ_PROTOTYPE *obj_mask;
          int vnum_same;
          vnum_same = 0;
          pShop = wch->pIndexData->pShop;
          if (pShop->creates_hours == 1 || (number_range (1, pShop->creates_hours) == 2))
            {
              for (ooo = wch->carrying; ooo != NULL; ooo = ooo->next_content)
                if (ooo->pIndexData->vnum == pShop->creates_vnum)
                  vnum_same++;
              if ((obj_mask = get_obj_index (pShop->creates_vnum)) == NULL)
                {
                  goto skip_over;
                }
              if (obj_mask->item_type == ITEM_FOOD && vnum_same > 24)
                goto skip_over;
              if (obj_mask->item_type != ITEM_FOOD && vnum_same > 4)
                goto skip_over;
              /*Damnit, Jim... I create object now, not a doctor!! */
              {
                obj = create_object (obj_mask, 5);
                obj_to (obj, wch);
                SET_BIT (obj->extra_flags, ITEM_INVENTORY);
                if (pShop->creates_message != NULL)
                  {
                    sprintf (general_use, "%s", pShop->creates_message);
                    act (general_use, wch, NULL, wch, TO_ROOM);
                  }
              }
            }
        }
    skip_over:
      if (wch->in_room == NULL || wch->in_room->area == NULL || wch->in_room->area->nplayer < 1)
        continue;

      if (IS_MOB (wch) && wch->in_room->more->pcs > 0)
        {

        }
      
      if (wch->position == POSITION_SLEEPING
          && IS_SET (wch->pIndexData->act3, ACT3_SLEEP)
          && time_info.hour > 7 && time_info.hour < 21
          && dice (1, 3) == 2)
        {
          act ("$n wakes up from $s sleep.", wch, NULL, NULL, TO_ROOM);
          wch->position = POSITION_STANDING;
          continue;
        }
      if (IS_SET (wch->pIndexData->act3, ACT3_SLEEP)
          && (wch->position == POSITION_STANDING || wch->position == POSITION_RESTING)
          && FIGHTING (wch) == NULL
          && (time_info.hour >= 21 || time_info.hour <= 7)
          && dice (1, 3) == 2)
        {
          act ("$n lays down for the night.", wch, NULL, NULL, TO_ROOM);
          wch->position = POSITION_SLEEPING;
          continue;
        }
      if (IS_MOB (wch) && wch->timer > 0)
        {
          if (--wch->timer == 0)
            {
              act ("$n disappears.", wch, NULL, NULL, TO_ROOM);
              act ("Your time has run out.", wch, NULL, NULL, TO_CHAR);
              raw_kill (wch, FALSE);
              continue;
            }
        }
      
    }
  return;
}

/*
   * Update all chars, including mobs.
   * This function is performance sensitive.
 */
void
char_update (void)
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  time_t save_time;
  AFFECT_DATA af;
  SPELL_DATA *spell;
  bzero (&af, sizeof (af));
  if (chars_sent > 1048576)
    {
      meg_sent++;
      chars_sent -= 1048576;
    }
  save_time = current_time;

  LOOP (ch, char_list, ch_next)
  {
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;
    ch_next = ch->next;
    if (ch->desc && ch->desc->connected != CON_PLAYING)
      continue;
    if (ch->in_room == NULL)
      continue;
    if (CHALLENGE (ch) == 10 && (ch->in_room->vnum > 507 || ch->in_room->vnum < 504))
      ch->fgt->challenge = 0;
    if (IS_PLAYER (ch))
      {
        if (ch->pcdata->challenge_time > 0)
          ch->pcdata->challenge_time--;
        if (ch->pcdata->died_recently> 0 && number_range(1,4) == 2)
          ch->pcdata->died_recently--;
        if (LEVEL (ch) < 100)
          {
            ch->timer++;
            if (ch->timer > 120) {
                if (ch->desc) {
                        close_socket(ch->desc);
                        continue;
                }
            }
            if (ch->timer > 5 && ch->timer < 16)
              ch->timer = 90;
            if (number_range (1, 3) == 2)
              {
                gain_condition (ch, COND_DRUNK, -1);
                if (ch->pcdata->remort_times < 2)
                  gain_condition (ch, COND_FULL, -1);
                if (ch->pcdata->remort_times < 5 )
                  gain_condition (ch, COND_THIRST, -1);
              }
            if (IS_PLAYER (ch) && (ch->in_room->sector_type == SECT_DESERT
                || ch->in_room->sector_type == SECT_WASTELAND)
                && time_info.hour >= 11 && time_info.hour <= 17 )
              {
                send_to_char ("Whew it's hot!", ch);
                gain_condition (ch, COND_THIRST, -2);
              }
          }
      }
    LOOP (paf, ch->affected, paf_next)
    {
      paf_next = paf->next;
      if (paf->duration > 0)
        paf->duration--;
      else if (paf->duration < 0);
      else
        {
          if (paf_next == NULL
              || paf_next->type != paf->type
              || paf_next->duration > 0)
            {
              spell = skill_lookup (NULL, paf->type);
              if (paf->type > 0 && spell && spell->wear_off_msg != NULL &&
                  str_cmp (spell->wear_off_msg, "None"))
                {
                  send_to_char (spell->wear_off_msg, ch);
                  send_to_char ("\n\r", ch);
                }
            }
          affect_remove (ch, paf);
        }
    }
    /*
       * Careful with the damages here,
       * MUST NOT refer to ch after damage taken,
       * as it may be lethal damage (on NPC).
     */
    if (IS_AFFECTED (ch, AFF_POISON))
      {
        act ("$n shivers and suffers.", ch, NULL, NULL, TO_ROOM);
        send_to_char ("\x1B[0;31mYou feel painfully ill.\x1B[0m\n\r", ch);
        if (ch->hit - 2 < 1)
          {
            ch->hit = 1;
            
          }
        else
          SUBHIT(ch,2);
      }
    else if (IS_AFFECTED (ch, AFF_PLAGUE) && ch->in_room->more->pcs > 0)
      {
        act ("$n winces in pain as disease gradually rots his body.",
             ch, NULL, NULL, TO_ROOM);
        send_to_char ("You scream in pain as disease slowly rots your body.\n\r", ch);
     /* Take out Plague spreading JRAJRA  
	 LOOP (plague, ch->in_room->more->people, plague->next)
          if (plague != ch
              && !IS_AFFECTED (plague, AFF_PLAGUE)
              && LEVEL (plague) >= 10 &&
              number_range (1, 100) < pow.plague_chance)
          {
            bzero (&af, sizeof (af));
            af.type = gsn_plague;
            af.duration = 6;
            af.location = APPLY_STR;
            af.modifier = -5;
            af.bitvector = AFF_PLAGUE;
            renew_affect (plague, &af);
            send_to_char ("You feel as if your body is being eaten away from the inside.\n\r", plague);
            act ("$n's face turns a deathly pale as the plague spreads to $s body.",
                 plague, NULL, NULL, TO_ROOM);
          }*/
        if (ch->hit - 8 < 1)
          {
            ch->hit = 1;
            
          }
        else
          SUBHIT(ch,8);
      }
  }
  return;
}
void
dummycall ()
{
  return;
}

void
char_only_update (void)
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  SINGLE_TRIGGER *tr;
  SCRIPT_INFO *s;
  SCRIPT_INFO *scr;
  SCRIPT_INFO *scr_next;

/* Check running scripts for delay resumes */
  for (scr = info_list; scr != NULL; scr = scr_next)
    {
      scr_next = scr->next;
      if (scr->delay_ticks <= 0)
        continue;
      if (scr->tick_type != 2)
        continue;
      scr->delay_ticks--;
      if (scr->delay_ticks == 0)
        {
          execute_code (scr);
        }
    }



  for (ch = char_list; ch != NULL; ch = ch_next)
    {
      ch_next = ch->next;
      if (ch->desc && ch->desc->connected != CON_PLAYING)
        continue;
      if (ch->position <= POSITION_STUNNED && ch->position >= 0 && ch->hit > 0)
        update_pos (ch);
      if (!ch->in_room) continue;
      
      if (IS_PLAYER (ch))
        {
          mana_gain (ch, FALSE);
	  if (!IS_SET(ch->act, PLR_HOLYWALK))
	    {
	      switch (ch->in_room->sector_type)
		{
		default:
		  break;
		case SECT_AIR:
		case SECT_CLOUDS:
		  {
		    if(FIGHTING (ch) == NULL && !IS_AFFECTED (ch, AFF_FLYING))
		      {
			send_to_char ("Oh oh! You are falling to the ground!\n\r", ch);
			act ("$n starts to fall towards the earth!", ch, NULL, NULL, TO_ROOM);
			NEW_POSITION(ch, POSITION_FALLING);
			continue;
		      }
		  }
		break;
		case SECT_UNDERWATER:
		  {
		    if(!IS_AFFECTED (ch, AFF_BREATH_WATER) &&
		       !IS_SET(ch->act, PLR_HAS_SCUBA))
		      {
			send_to_char ("You are drowning!!\n\r", ch);
			SUBHIT(ch, number_range (12, 27));
			update_pos (ch);
			if (ch->hit < 0)
			  {
			    strcpy (reasonfd, "Drowning");
			    raw_kill (ch, FALSE);
			    return;
			  }
			
			continue;
		      }
		  }
		break;
		case SECT_WATER_NOSWIM:
		  {
		    if (!IS_SET(ch->act, PLR_HAS_BOAT))
		      {
			if (ch->move > 20)
			  {
			    send_to_char ("You can barely stay afloat in this deep, rough water!\n\r", ch);
			    SUBMOVE(ch, 17);
			    continue;
			  }
			else
			  {
			    send_to_char ("You can't manage to stay afloat! You are drowning!\n\r", ch);
			    SUBHIT(ch, number_range (5, 15));
			    update_pos (ch);
			    if (ch->hit < 0)
			      {
				strcpy (reasonfd, "Drowning");
				raw_kill (ch, FALSE);
				return;
			      }
			    
			    continue;
			  }
		      }
		  }
		break;
		case SECT_ASTRAL:
		  {
		    if(!IS_AFFECTED_EXTRA (ch, AFF_PROT_ASTRAL))
		      {
			send_to_char ("The astral forces pummel you from all sides!!\n\r", ch);
			SUBHIT(ch, number_range (100, 600));
			update_pos (ch);
			if (ch->hit < 0)
			  {
			    strcpy (reasonfd, "Astral Plane");
			    raw_kill (ch, FALSE);
			    return;
			  }
			
			continue;
		      }
		  }
		break;
		case SECT_LAVA:
		  {
		    if(!IS_AFFECTED_EXTRA (ch, AFF_PROT_LAVA))
		      {
			send_to_char ("The lava is burning you to a crisp!!\n\r", ch);
			SUBHIT(ch, number_range (50, 150));
			update_pos (ch);
			if (ch->hit < 0)
			  {
			    strcpy (reasonfd, "Lava");
			    raw_kill (ch, FALSE);
			    return;
			  }
			
			continue;
		      }
		  }
		}
	    }
           	      FIXIT(ch);
	  if(number_range (1, 50) == 7)
	    {
	      fix_char(ch);
	      ch->affected_by |= ch->pcdata->nat_abilities;
	      ch->more_affected_by |= ch->pcdata->nat_abilities2;
	      if (number_range(1,10) == 3)
		{
		  check_rating (ch);
		  check_topten(ch);
		  check_pkdata(ch);
		  do_save (ch, "xx2xx11");
		}
	    } 
	  
	}
      else
        {
        check_a2:
	  /* Check for triggers on mobbies! */
          for (tr = trigger_list[TEVERY_REGENTICK]; tr != NULL; tr = tr->next)
            {
              if (ch->pIndexData->vnum == tr->attached_to_mob)
                {
                  if (tr->players_only)
                    continue;
                  if (tr->running_info && !tr->interrupted)
                    continue;   /* Already running, interrupted, but script says not to allow interruptions. */
                  if (tr->running_info && tr->interrupted != 2)
                    {
                      end_script (tr->running_info);
                      goto check_a2;
                    }
                  /* ----------------- */
                  /* Start the script! */
                  /* ----------------- */
                  tr->running_info = mem_alloc (sizeof (*tr->running_info));
                  s = tr->running_info;
                  bzero (s, sizeof (*s));
                  s->current = ch;
                  s->mob = ch;
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


          /* Scavenge */
          if (IS_SET (ch->act, ACT_SCAVENGER)
              && ch->in_room->more->contents != NULL
              && number_range (0, 4) == 1)
            {
              SINGLE_OBJECT *obj;
              SINGLE_OBJECT *obj_best;
              int max;
              max = 1;
              obj_best = NULL;
              for (obj = ch->in_room->more->contents; obj != NULL; obj = obj->next_content)
                {
                  if (obj->in_room && IS_SET (obj->pIndexData->wear_flags, ITEM_TAKE) && CAN_WEAR (obj, ITEM_TAKE) && obj->cost > max)
                    {
                      obj_best = obj;
                      max = obj->cost;
                    }
                }
              if (obj_best != NULL && obj_best->in_room == ch->in_room && IS_SET (obj_best->pIndexData->wear_flags, ITEM_TAKE))
                {
                  act ("$n picks up $p.", ch, obj_best, ch, TO_ROOM);
                  obj_from (obj_best);
                  obj_to (obj_best, ch);
                  if (obj_best->in_room == NULL)
                    {
                      wear_obj (ch, obj_best, FALSE, 0);
                    }
                }
            }
          if (IS_SET (ch->pIndexData->act3, ACT3_REGENERATE) && number_range (1, 7) == 2)
            {
              if (ch->hit < ch->max_hit)
                {
                  int healnum;
                  act ("$n regenerates.", ch, NULL, NULL, TO_ROOM);
                  healnum = .20 * (float) (ch->max_hit - ch->hit);
                  ADDHIT(ch, number_range (healnum, (int) (healnum * 1.5)));
                  update_pos (ch);
                  if (ch->hit > ch->max_hit)
                    MAXHIT(ch);
                }
            }
        }
      if (ch->hit < ch->max_hit)
        {
          ADDHIT(ch, hit_gain (ch));
          
        }
      
      if (ch->move < ch->max_move)
        {
          ADDMOVE(ch,move_gain (ch));
        }
      if (ch->hit == -1)
        {
          NEW_POSITION(ch, POSITION_STANDING);
          ch->hit = 1;
        }
      if (ch->position == POSITION_STUNNED)
        update_pos (ch);
      if (check_body_temp (ch))
        continue;
      if (IS_PLAYER (ch) && ch->timer > 93 && ch->pcdata->no_quit_pk < 1)
        {
          do_quit (ch, "specqa");
          continue;
        }
    }
  return;
}
void
transport_update (void)
{
  CHAR_DATA *ch;
  EXIT_DATA *pexit;
  CHAR_DATA *ch_next;
  ROOM_DATA *to_room;
  ROOM_DATA *roomav;
  ROOM_DATA *roomstink;
  bool avalancheoff;
  CHAR_DATA *victim;
  SCRIPT_INFO *scr;
  SCRIPT_INFO *scr_next;
  SINGLE_TRIGGER *tr;
  SCRIPT_INFO *s;
  int door;
  char general_use[500];
  bool FLAGGYSET;
  FLAGGYSET = FALSE;
  avalancheoff = FALSE;
  roomstink = NULL;
  roomav = NULL;
  ch_next = NULL;
  check_background_processes ();


  for (scr = info_list; scr != NULL; scr = scr_next)
    {
      scr_next = scr->next;
      if (scr->delay_ticks <= 0)
        continue;
      if (scr->tick_type != 1)
        continue;
      scr->delay_ticks--;
      if (scr->delay_ticks == 0)
        {
          execute_code (scr);
        }
    }

  for (ch = char_list; ch != NULL; ch = ch_next)
    { 
      ch_next = ch->next;
      if (IS_PLAYER(ch))
	{
	  if (IS_AFFECTED (ch, AFF_WOUND))
	    {
	      if (number_range(1,20)==1)
		send_to_char("Your wounds tear at you, leaving you in agony!\n\r", ch);
	      SUBHIT(ch, number_range(5,13));
	      if (ch->hit < -10) raw_kill (ch, TRUE);
	    }
	  if (IS_AFFECTED (ch, AFF_REGENERATE))
	    {     
	      if (ch->hit < ch->max_hit-5)
		{
		  ADDHIT(ch, number_range(1,7));
		  if (number_range(1,7)==1)
		    send_to_char ("Your wounds close as regeneration mends your flesh\n\r", ch);
		}
	      if (ch->move < ch->max_move)
		{
		  ADDMOVE(ch, 1);
		  if (IS_PLAYER(ch) && !strcmp(race_info[ch->pcdata->race].name, "behemoth"))
		     ADDMOVE (ch, number_range(10, 30));
		}
	    }
	  if (ALIGN(ch)==1 && IN_SUN(ch) && pow.penalize_evil_in_sun == TRUE)
	    {
	      if (number_range(1,8) == 3)
	      {	      
	      SUBHIT(ch, number_range(0,3));
	      SUBMOVE(ch, number_range(0,1));
	      if (number_range(1,4) == 2)
		ch->pcdata->n_mana--;
	      if (ch->pcdata->n_mana < 0) ch->pcdata->n_mana = 0;
	      if (ch->hit < -10) raw_kill(ch, true);
	      if (number_range(1,3) == 1)
	      send_to_char("The light of the sun is draining you of all strength.\n\r", ch);
	      } 
	   }
	}
      else 
	{
	  if (IS_SET(ch->pIndexData->act4, ACT4_QUICKTICK))
	    {
	    check_a3:
	      /* Check for triggers on mobbies! */
	      for (tr = trigger_list[TEVERY_QUICKTICK]; tr != NULL; tr = tr->next)
		{
		  if (ch->pIndexData->vnum == tr->attached_to_mob)
		    {
		      if (tr->players_only)
			continue;
		      if (tr->running_info && !tr->interrupted)
			continue;   /* Already running, interrupted, but script says not to allow interruptions. */
		      if (tr->running_info && tr->interrupted != 2)
			{
			  end_script (tr->running_info);
			  goto check_a3;
			}
		      /* ----------------- */
		      /* Start the script! */
		      /* ----------------- */
		      tr->running_info = mem_alloc (sizeof (*tr->running_info));
		      s = tr->running_info;
		      bzero (s, sizeof (*s));
		      s->current = ch;
		      s->mob = ch;
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
	  
          if (IS_SET (ch->act, ACT_SCRAMBLE)
	      && !IS_SET (ch->act, ACT_SENTINEL)
              && (door = number_range (0, 7)) <= 5
              && (pexit = ch->in_room->exit[door]) != NULL
              && (ROOM_DATA *) pexit->to_room != NULL
              && (!pexit->d_info || !IS_SET (pexit->d_info->exit_info, EX_CLOSED))
              && !IS_SET (((ROOM_DATA *) pexit->to_room)->room_flags, ROOM_NO_MOB)
              && (!IS_SET (ch->act, ACT_STAY_AREA)
		  || ((ROOM_DATA *) pexit->to_room)->area == ch->in_room->area))
            {
              move_char (ch, door);
            }
	  if(IS_SET(ch->pIndexData->act4, ACT4_PROTECTOR) && number_range(1,7) == 3)
	    {
	      CHAR_DATA *enemy;
	      if ((enemy = get_char_room_near(ch, ch->in_room, 6, "enemy")) != NULL )
		{
		  SPELL_DATA *spl;
		  
		  if((spl = skill_lookup(NULL, number_range(58,116))) != NULL)
		    {
		      if (enemy->in_room != ch->in_room)
		      general_spell(spl, LEVEL(ch), ch, enemy);
		    }
		}
	    }
	      


          if (HUNTING (ch) != NULL && ch->position >= POSITION_STANDING)
            {
              char buf[200];
              if ((victim = get_char_room (ch, HUNTING (ch))) == NULL)
                continue;
              if (FIGHTING (ch) != NULL ||
                  ch->position == POSITION_FIGHTING ||
                  ch->position == POSITION_GROUNDFIGHTING)
                continue;
	      if (!IS_SET (ch->pIndexData->act3, ACT3_MUTE) && 
		  (FIGHTING (victim) != NULL &&	
		   victim->fgt->fighting->position != POSITION_GROUNDFIGHTING))
                {
                  if (ch->pIndexData->will_help && ch->pIndexData->yeller_number != 0)
                    {
                      sprintf (buf, "You shall pay for your cruelty!!");
                      if (can_yell (ch))
                        do_say (ch, buf);
                    }
                  else
                    {
                      sprintf (buf, "There you are, %s, but not for long!", NAME (victim));
                      if (can_yell (ch))
                        do_yell (ch, buf);
                    }
                }
              set_fighting (ch, victim);
            }
          ch->wait -= 13;
          if (ch->wait < 0)
            ch->wait = 0;
	  if (FIGHTING (ch) == NULL && number_range (1, 3) == 2 && HUNTING (ch) != NULL && (is_number (ch->fgt->hunting) || !IS_SET (ch->act, ACT_SENTINEL)))
	    hunt_victim (ch);
	  if (ch->in_room->area->nplayer < 1)
	    continue;
	}
      if (IS_PLAYER (ch))
        {
          if (FIGHTING (ch) == NULL && 
	      (ch->position == POSITION_FIGHTING || 
	       ch->position == POSITION_GROUNDFIGHTING))
            NEW_POSITION(ch, POSITION_STANDING);
          ch->wait -= 8;
          if (ch->wait < 0)
            ch->wait = 0;
          if (ch->position == POSITION_FALLING)
	    {
	      if (IS_AFFECTED (ch, AFF_FLYING) || IS_SET (ch->act, PLR_HOLYWALK))
		{
		  send_to_char ("You stop falling.\n\r", ch);
		  act ("$n stops falling.", ch, NULL, NULL, TO_ROOM);
		  NEW_POSITION(ch, POSITION_STANDING);
		  continue;
		}
	      else 
		{
		  if (ch->in_room->exit[DIR_DOWN] &&
		      (ROOM_DATA *) ch->in_room->exit[DIR_DOWN]->to_room != NULL
		      && ch->in_room->sector_type == SECT_AIR)
		    {
		      send_to_char ("Still falling...\n\r", ch);
		      act ("$n falls right by you.", ch, NULL, NULL, TO_ROOM);
		      to_room = ch->in_room->exit[DIR_DOWN]->to_room;
		      char_from_room (ch);
		      char_to_room (ch, to_room);
		      do_look (ch, "auto");
		      act ("$n falls from above...", ch, NULL, NULL, TO_ROOM);
		      continue;
		    }
		  else
		    {
		      send_to_char ("OUCH! You land with a loud THUD!\n\r", ch);
		      act ("$n slams into the ground!", ch, NULL, NULL, TO_ROOM);
		      NEW_POSITION(ch, POSITION_STANDING);
		      if (ch->hit > 25)
			{
			  SUBHIT(ch, number_range(14,20));
			  
			}
		      if (ch->position >= POSITION_STUNNED)
			NEW_POSITION(ch, POSITION_RESTING);
		      continue;
		    }
		}
	    }
          if (ch->in_room->room_flags2 != 0)
            {
              if (IS_AFFECTED (ch, AFF_NOXIOUS) && !FLAGGYSET && !IS_SET (ch->in_room->room_flags2, ROOM2_GASTRAPONE))
                {
                  ch->in_room->room_flags2 ^= ROOM2_GASTRAPONE;
                  roomstink = ch->in_room;
                  FLAGGYSET = TRUE;
                }
              if (IS_SET (ch->in_room->room_flags2, ROOM2_MANADRAIN))
                {
                  mana_gain (ch, 2);
                }
              else if (IS_SET (ch->in_room->room_flags2, ROOM2_EXTRAMANA))
                {
                  mana_gain (ch, TRUE);
                }
              if (IS_SET (ch->in_room->room_flags2, ROOM2_EXTRAHEAL))
                {
                  if (ch->hit < ch->max_hit && ch->position)
                    {
                      ADDHIT(ch, 2);
                      
                    }
                }
	    }
	  if (IS_SET (ch->act, PLR_HOLYWALK))
	    continue;
	}
      
      if (ch->in_room->room_flags2 > 0 && number_range(1,4) == 2)
	{
	  if (IS_SET (ch->in_room->room_flags2, ROOM2_FIRETRAP))
	    {
	      int chan;
	      if (IS_AFFECTED_EXTRA (ch, AFF_PROT_FIRE))
		continue;
	      if (IS_MOB (ch))
		chan = (LEVEL (ch) / (double) 1.5);
	      else
		chan = get_curr_dex (ch) + get_curr_int (ch);
	      if (number_range (1, 55) > chan)
		{
		  act ("Searing flames rise up around you, burning your flesh!", ch, NULL, NULL, TO_CHAR);
		  act ("$N is engulfed in flames!", ch, NULL, ch, TO_ROOM);
		  SUBHIT(ch, number_range (1, 30));
		  update_pos (ch);
		  if (ch->position == POSITION_DEAD)
		    {
		      strcpy (reasonfd, "Fire");
		      raw_kill (ch, FALSE);
		      continue;
		    }
		  
		}
	      else
		act ("Searing flames rise up around you, but you quickly move out of the way!!", ch, NULL, NULL, TO_CHAR);
	    }
	  if (IS_SET (ch->in_room->room_flags2, ROOM2_ROCKTRAP))
	    {
	      if (number_range (0, 2) == 1)
		{
		  int chan;
		  int dam;
		  if (IS_MOB (ch))
		    chan = (LEVEL (ch) / (double) 1.5);
		  else
		    chan = get_curr_dex (ch) + get_curr_int (ch);
		  if (number_range (1, 55) > chan)
		    {
		      act ("Large rocks fall from above, hitting you on the head!", ch, NULL, NULL, TO_CHAR);
		      act ("$N is smashed on the head by large rocks!", ch, NULL, ch, TO_ROOM);
		      dam = number_range (1, 50);
		      if (dam < 1)
			dam = 1;
		      SUBHIT(ch,dam);
		      update_pos (ch);
		      if (ch->position == POSITION_DEAD)
			{
			  strcpy (reasonfd, "Falling rocks");
			  raw_kill (ch, FALSE);
			  continue;
			}
		      
		    }
		  else
		    act ("Your quick thinking and fast action allows you to dodge a bunch of\n\rlarge rocks that fall from the ceiling!", ch, NULL, NULL, TO_CHAR);
		}
	    }
	  if (IS_SET (ch->in_room->room_flags2, ROOM2_ARROWTRAP) ||
	      IS_SET (ch->in_room->room_flags2, ROOM2_POISONTRAP))
	    {
	      int chan;
	      int dam;
	      if (IS_MOB (ch))
		chan = (LEVEL (ch));
	      else
		chan = get_curr_dex (ch) + get_curr_wis (ch) + get_curr_int (ch);
	      if (number_range (1, 80) > chan)
		{
		  act ("Small arrows seem to shoot out at you from nowhere. OUCH!", ch, NULL, NULL, TO_CHAR);
		  act ("$N is pierced by a small barage of arrows!", ch, NULL, ch, TO_ROOM);
		  dam = number_range (1, 25);
		  if (ch->armor < -60)
		    {
		      act ("Your superb armor shrugs off most of the puny arrows!", ch, NULL, NULL, TO_CHAR);
		      dam -= 15;
		    }
		  else if (ch->armor < 10)
		    {
		      act ("Your armor lessens the penetration of the arrows!", ch, NULL, NULL, TO_CHAR);
		      dam -= 6;
		    }
		  if (dam < 1)
		    dam = 1;
		  SUBHIT(ch,dam);
		  update_pos (ch);
		  
		  if (ch->position == POSITION_DEAD)
		    {
		      raw_kill (ch, FALSE);
		      continue;
		    }
		}
	      else
		act ("Arrows suddenly fire at you from out of nowhere, but your quick thinking\n\rand fast reflexes allow you to dodge them!", ch, NULL, NULL, TO_CHAR);
	    }
	  if (IS_SET (ch->in_room->room_flags2, ROOM2_SPIKETRAP) && number_range (1, 3) == 2)
	    {
	      int chan;
	      int dam;
	      if (IS_MOB (ch))
		chan = (LEVEL (ch));
	      else
		chan = get_curr_dex (ch) + get_curr_wis (ch) + get_curr_int (ch);
	      if (number_range (1, 80) > chan)
		{
		  act ("You stumble and fall on some nasty spikes!", ch, NULL, NULL, TO_CHAR);
		  act ("$N stumbles and lands hard on some spikes!", ch, NULL, ch, TO_ROOM);
		  dam = number_range (1, 35);
		  if (ch->armor < -80)
		    {
		      act ("Your superb armor prevents penetration!", ch, NULL, NULL, TO_CHAR);
		      dam -= 25;
		    }
		  else if (ch->armor < 5)
		    {
		      act ("Your armor lessens the penetration of the spikes!", ch, NULL, NULL, TO_CHAR);
		      dam -= 8;
		    }
		  if (dam < 1)
		    dam = 1;
		  SUBHIT(ch,dam);
		  update_pos (ch);
		  
		  if (ch->position == POSITION_DEAD)
		    {
		      raw_kill (ch, FALSE);
		      continue;
		    }
		}
	    }
	  if (IS_SET (ch->in_room->room_flags2, ROOM2_GASTRAPONE) ||
	      IS_SET (ch->in_room->room_flags2, ROOM2_GASTRAPTWO))
	    {
	      int dam;
	      if (IS_AFFECTED_EXTRA (ch, AFF_PROT_GAS))
		continue;
	      if (number_range (1, 2) == 1)
		{
		  act ("Horrible noxious gasses which hang in the air here make your lungs burn!", ch, NULL, NULL, TO_CHAR);
		  act ("$N coughs uncontrollably and $s eyes water!", ch, NULL, ch, TO_ROOM);
		  if (IS_SET (ch->in_room->room_flags2, ROOM2_GASTRAPONE))
		    {
		      /*                                                     ch->in_room->room2_flags^=ROOM_GASTRAPONE; */
		      dam = number_range (1, 10);
		    }
		  else
		    dam = number_range (1, 50);
		  SUBHIT(ch,dam);
		  update_pos (ch);
		  
		  if (ch->position == POSITION_DEAD)
		    {
		      raw_kill (ch, FALSE);
		      continue;
		    }
		  if (IS_SET (ch->in_room->room_flags2, ROOM2_GASTRAPTWO) && !IS_AFFECTED_EXTRA(ch, AFF_PROT_POISON))
		    {
		      AFFECT_DATA af;
		      af.type = gsn_poison;
		      af.duration = number_range (2, 10);
		      af.modifier = -2;
		      if (IS_AFFECTED (ch, AFF_POISON))
			{
			  af.duration = 1;
			  af.modifier = 0;
			}
		      af.location = APPLY_STR;
		      af.bitvector = AFF_POISON;
		      renew_affect (ch, &af);
		      act ("You feel very sick! The gas must be poisonous!", ch, NULL, NULL, TO_CHAR);
		    }
		}
	    }
	  if (IS_SET (ch->in_room->room_flags2, ROOM2_AVALANCHE))
	    {
	      int dam;
	      avalancheoff = TRUE;
	      roomav = ch->in_room;
	      act ("You are smashed by huge chunks of ice and snow!\n\rAVALANCHE!!!!", ch, NULL, NULL, TO_CHAR);
	      dam = number_range (10, 55);
	      if (number_range (1, 3) == 2)
		{
		  act ("Snow completely covers you! You can't breath!!!", ch, NULL, NULL, TO_CHAR);
		  dam += number_range (20, 40);
		}
	      SUBHIT(ch,dam);
	      update_pos (ch);
	      
	      if (ch->position == POSITION_DEAD)
		{
		  raw_kill (ch, FALSE);
		  continue;
		}
	    }
	  if (IS_SET (ch->in_room->room_flags2, ROOM2_GEYSERTRAP) && number_range (1, 10) == 2)
	    {
	      int dam;
	      act ("You are sprayed by scalding hot water!!!", ch, NULL, NULL, TO_CHAR);
	      dam = number_range (10, 40);
	      SUBHIT(ch,dam);
	      update_pos (ch);
	      
	      if (ch->position == POSITION_DEAD)
		{
		  raw_kill (ch, FALSE);
		  continue;
		}
	    }
	  
	  if (FLAGGYSET && IS_SET (roomstink->room_flags2, ROOM2_GASTRAPONE))
	    roomstink->room_flags2 ^= ROOM2_GASTRAPONE;
	  if (avalancheoff)
	    roomav->room_flags2 ^= ROOM2_AVALANCHE;
	}
  /*End traps */
      if (IS_MOB (ch)
	  && (FIGHTING (ch) == NULL && IS_SET (ch->act, ACT_SENTINEL)))
	continue;
      if (IS_SET (ch->in_room->room_flags2, ROOM2_MOVING) &&  
	  !IS_AFFECTED(ch, AFF_FLYING) && 
	  ch->in_room->exit[ch->in_room->more->move_dir] != NULL && 
	  ch->in_room->more &&
	  (ROOM_DATA *) ch->in_room->exit[ch->in_room->more->move_dir]->to_room
	  != NULL
	  && FIGHTING (ch) == NULL
	  && (LEVEL (ch) < IMM_LEVEL || !IS_SET (ch->act, PLR_HOLYWALK))
	  && ch->position != POSITION_FALLING && ch->in_room->more)
	{
	  if (ch->in_room->more->move_message && ch->in_room->more->move_message[0] != '\0')
	    {
	      sprintf (general_use, "You are %s", ch->in_room->more->move_message);
	      send_to_char (general_use, ch);
	      sprintf (general_use, "$n is %s", ch->in_room->more->move_message);
	      act (general_use, ch, NULL, NULL, TO_ROOM);
	    }
	  else
	    {
	      send_to_char ("You feel yourself being swept away...\n\r", ch);
	      act ("$n is swept right by you.", ch, NULL, NULL, TO_ROOM);
	    }
	  to_room = (ROOM_DATA *) ch->in_room->exit[ch->in_room->more->move_dir]->to_room;
	  char_from_room (ch);
	  char_to_room (ch, to_room);
	  do_look (ch, "auto");
	  act ("$n is thrown into the room.", ch, NULL, NULL, TO_ROOM);
	}
    }
  return;
}

void
obj_update (void)
{
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj_next;
  SINGLE_OBJECT *objtmp;
  SINGLE_OBJECT *objnxt;
  if (arena_timelimit > -1)
    arena_timelimit--;
  if (arena_timelimit == 0)
    {
      arena_timelimit = -1;
      give_back_money ();
{
CHAR_DATA *sch;
sch=create_mobile(get_mob_index(1));
char_to_room(sch,get_room_index(2));
do_cancel (sch, "");
extract_char(sch,TRUE);
}
    }

  if (number_range (1, 3) > 1)
    return;
  for (obj = object_list; obj != NULL; obj = obj_next)
    {
      CHAR_DATA *owner;
      CHAR_DATA *rch;
      char *message;
      if (!obj->next)
        obj_next = NULL;
      else
        obj_next = obj->next;
      if (obj->in_room == NULL && obj->carried_by == NULL && obj->in_obj == NULL)
        continue;
      
      
      /* Move this to object update */
      if (obj->carried_by && obj->carried_by->in_room &&
          obj->pIndexData->item_type == ITEM_LIGHT
          && IS_LIT (obj) && ((I_LIGHT *) obj->more)->max_light > 0)
        {
          if (--((I_LIGHT *) obj->more)->light_now == 0 && obj->carried_by->in_room != NULL)
            {
              --obj->carried_by->in_room->light;
              if (IS_PLAYER(obj->carried_by) &&
		  obj->carried_by->pcdata->light > 0)
		obj->carried_by->pcdata->light--;
              act ("$p goes out.", obj->carried_by, obj, NULL, TO_ROOM);
              act ("$p goes out.", obj->carried_by, obj, NULL, TO_CHAR);
              REMOVE_BIT (((I_LIGHT *) obj->more)->light_lit, LIGHT_LIT);
              if (!IS_SET (((I_LIGHT *) obj->more)->light_lit, LIGHT_FILLABLE))
                {
		  
                  free_it (obj);
                  continue;
                }
            }
          if (((I_LIGHT *) obj->more)->light_now > 0)
            {
              switch (PERCENTAGE (((I_LIGHT *) obj->more)->light_now, ((I_LIGHT *) obj->more)->max_light) / 10)
                {
                case 0:
                  act ("$p belonging to $n, sputters.", obj->carried_by, obj, NULL, TO_ROOM);
                  act ("$p flickers and sputters.", obj->carried_by, obj, NULL, TO_CHAR);
                  break;
                case 1:
                  act ("$p belonging to $n, flickers.", obj->carried_by, obj, NULL, TO_ROOM);
                  act ("$p flickers.", obj->carried_by, obj, NULL, TO_CHAR);
                  break;
                case 2:
                  act ("$p belonging to $n, flickers slightly.", obj->carried_by, obj, NULL, TO_ROOM);
                  act ("$p flickers slightly.", obj->carried_by, obj, NULL, TO_CHAR);
                  break;
                }
            }
        }
      
      if (obj->timer <= 0 || --obj->timer > 0)
        continue;
      
      if ((owner = obj->carried_by) != NULL && owner->in_room && obj->wear_loc != -1)
        unequip_char (owner, obj);
      if (owner && !owner->in_room) continue;
      
      switch (obj->pIndexData->item_type)
        {
        default:
          message = "$p vanishes.";
          break;
        case ITEM_FOUNTAIN:
          message = "$p dries up.";
          break;
        case ITEM_CORPSE_NPC:
          message = "$p decays into dust.\x1B[37;0m";
          break;
        case ITEM_CORPSE_PC:
          {
            char name[SML_LENGTH];
            char *pd;
            char buffy[200];
            pd = OOSTR (obj, short_descr);
            pd = one_argy (pd, name);
            pd = one_argy (pd, name);
            pd = one_argy (pd, name);
            pd = one_argy (pd, name);
            /*name now equals the name of the person */
            sprintf (buffy, "rm %s%s.cor &", PLAYER_DIR_2, capitalize (name));
            system (buffy);

            if (obj->in_room == NULL)
              continue;
            for (objtmp = obj->contains; objtmp != NULL; objtmp = objnxt)
              {
                objnxt = objtmp->next_content;
                obj_from (objtmp);
                obj_to (objtmp, obj->in_room);
              }
            message = "$p decays into dust.";
            break;
          }
        case ITEM_FOOD:
          message = "$p rots.";
          break;
        }
      if (obj->pIndexData->vnum != 40)
        {
          if (obj->carried_by != NULL)
            {
              act (message, obj->carried_by, obj, NULL, TO_CHAR);
            }
          else if (obj->in_room != NULL && obj->in_room->more
                   && (rch = obj->in_room->more->people) != NULL)
            {
              act (message, rch, obj, NULL, TO_ROOM);
              act (message, rch, obj, NULL, TO_CHAR);
            }
        }
      /*get rid of power coin!? */
      if (obj->carried_by != NULL && obj->wear_loc != -1)
        unequip_char (obj->carried_by, obj);
      free_it (obj);
    }
  return;
}

bool
can_yell (CHAR_DATA * ch)
{
  if (IS_PLAYER (ch))
    return TRUE;
  if (ch->pIndexData->mobtype == MOB_PLANT ||
      ch->pIndexData->mobtype == MOB_GENERAL_ANIMAL ||
      ch->pIndexData->mobtype == MOB_DUMMY ||
      ch->pIndexData->mobtype == MOB_INSECT ||
      ch->pIndexData->mobtype == MOB_UNDEAD ||
      ch->pIndexData->mobtype == MOB_BIRD ||
      ch->pIndexData->mobtype == MOB_FISH ||
      ch->pIndexData->mobtype == MOB_CANINE ||
      ch->pIndexData->mobtype == MOB_FELINE ||
      ch->pIndexData->mobtype == MOB_REPTILE ||
      ch->pIndexData->mobtype == MOB_GHOST ||
      ch->pIndexData->mobtype == MOB_ARACHNID ||
      ch->pIndexData->mobtype == MOB_HORSE ||
      ch->pIndexData->mobtype == MOB_MUTANT ||
      ch->pIndexData->mobtype == MOB_RODENT)
    return FALSE;
  return TRUE;
}

void
racehate_message (CHAR_DATA * ch)
{
  if (IS_PLAYER (ch))
    return;
  if (ch->pIndexData->mobtype == MOB_PLANT ||
      ch->pIndexData->mobtype == MOB_GENERAL_ANIMAL ||
      ch->pIndexData->mobtype == MOB_DUMMY ||
      ch->pIndexData->mobtype == MOB_INSECT ||
      ch->pIndexData->mobtype == MOB_UNDEAD ||
      ch->pIndexData->mobtype == MOB_BIRD ||
      ch->pIndexData->mobtype == MOB_FISH ||
      ch->pIndexData->mobtype == MOB_CANINE ||
      ch->pIndexData->mobtype == MOB_FELINE ||
      ch->pIndexData->mobtype == MOB_REPTILE ||
      ch->pIndexData->mobtype == MOB_GHOST ||
      ch->pIndexData->mobtype == MOB_ARACHNID ||
      ch->pIndexData->mobtype == MOB_HORSE ||
      ch->pIndexData->mobtype == MOB_MUTANT ||
      ch->pIndexData->mobtype == MOB_RODENT)
    return;
  switch (number_range(1,4))
    {
    case 1:
      do_say (ch, "I don't like your kind! I think I'll just kill you...");
      break;
    case 2:
      do_say (ch, "All of your kind must die!");
      break;
    case 3:
      do_say(ch, "Your kind will be obliterated!");
      break;
    }
  return;
}

void
aggr_update (void)
{
  CHAR_DATA *wch;
  CHAR_DATA *wch_next;
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  CHAR_DATA *victim;
  SCRIPT_INFO *scr;
  SCRIPT_INFO *scr_next;

/* Check running scripts for delay resumes */
  for (scr = info_list; scr != NULL; scr = scr_next)
    {
      scr_next = scr->next;
      if (scr->tick_type != 4)
        continue;
      if (scr->delay_ticks <= 0)
        continue;
      scr->delay_ticks--;
      if (scr->delay_ticks == 0)
        {
          execute_code (scr);
        }
    }


  for (wch = aggro_check; wch != NULL; wch = wch_next)
    {
      wch_next = wch->gen_next;
      if (wch->data_type == 50 || wch->data_type == 0)
        {
          remove_from_aggro_list (wch);
          continue;
        }
      if (wch->in_room->area->nplayer < 1)
        continue;
      --wch->wait;

      if (!wch->in_room->more || !wch->in_room->more->people || wch->in_room->more->pcs < 1)
        continue;

      for (ch = wch->in_room->more->people; ch && ch != NULL; ch = ch_next)
        {
          int count;
          ch_next = ch->next_in_room;
          if (IS_MOB (ch) && ch->pIndexData->race_hate != 0 && FIGHTING (ch) == NULL)
            {
              for (vch = wch->in_room->more->people; vch != NULL; vch = vch_next)
                {
                  if (!vch->next_in_room)
                    vch_next = NULL;
                  else
                    vch_next = vch->next_in_room;
                  if (IS_PLAYER (vch) && can_see (ch, vch) && (IS_SET (ch->pIndexData->race_hate, (1 << ch->pcdata->race))) &&
                      LEVEL (vch) < 101 && FIGHTING (ch) == NULL)
                    {
                      racehate_message (ch);
                      set_fighting (ch, vch);
                      continue;
                    }
                }
            }
          /*dun: */
          if (ch->in_room == NULL)
            continue;
          if (IS_PLAYER (ch)
              || (!IS_SET (ch->act, ACT_AGGRESSIVE)
                  && !IS_SET (ch->act, ACT_ANGRY))
              || FIGHTING (ch) != NULL
              || IS_AFFECTED (ch, AFF_CHARM)
              || !IS_AWAKE (ch)
              || (IS_SET (ch->act, ACT_WIMPY) && IS_AWAKE (wch))
              || !can_see (ch, wch))
            continue;


          if (IS_SET (ch->act, ACT_ANGRY) && (number_range (0, 6) != 1))
            continue;
          count = 0;
          victim = NULL;
          for (vch = wch->in_room->more->people; vch != NULL; vch = vch_next)
            {
              vch_next = vch->next_in_room;
              if (IS_PLAYER (vch)
                  && LEVEL (vch) < LEVEL_IMMORTAL
                  && (!IS_SET (ch->act, ACT_WIMPY) || !IS_AWAKE (vch))
                  && can_see (ch, vch)
              /*&& (!IS_AFFECTED( vch, AFF_SNEAK ) */ )
                {
                  if (number_range (0, count) == 0)
                    victim = vch;
                  count++;
                }
            }
          if (victim == NULL)
            {
              continue;
            }

          /* Safety spell reduces chance of getting attacked by aggro */
          if (IS_AFFECTED (victim, AFF_SAFETY) && LEVEL (ch) < LEVEL (victim)
              && number_range (1, 4) < 4)
            continue;
	  if (IS_SET(ch->pIndexData->act4, ACT4_KILL_OPP) && DIFF_ALIGN(ch, victim))
	    {
	      act("$B$&All of your kind must die!$R$7", ch, NULL, victim, TO_ROOM);
	    }
          else
            {
              switch (number_bits (2))
                {
                default:
                  act ("$4$n attacks $N!$R", ch, NULL, victim, TO_NOTVICT);
                  act ("$4$n attacks you!$R", ch, NULL, victim, TO_VICT);
                  break;
                case 1:
                  if (can_yell (ch))
                    {
                      act ("$4$n screams and attacks $N!$R", ch, NULL, victim, TO_NOTVICT);
                      act ("$4$n screams and attacks you!$R", ch, NULL, victim, TO_VICT);
                    }
                  break;
                case 2:
                  if (can_yell (ch))
                    {
                      act ("$4$n leaps towards $N!$R", ch, NULL, victim, TO_NOTVICT);
                      act ("$4$n leaps towards you!$R", ch, NULL, victim, TO_VICT);
                    }
                  else
                    {
                      act ("$4$n attacks $N!$R", ch, NULL, victim, TO_NOTVICT);
                      act ("$4$n attacks you!$R", ch, NULL, victim, TO_VICT);
                    }
                  break;
                case 3:
                  act ("$4$N's eyes widen in horror as $n attacks!$R", ch, NULL, victim, TO_NOTVICT);
                  act ("$4$n attacks you!$R", ch, NULL, victim, TO_VICT);
                  break;
                }
            }
          multi_hit (ch, victim,TYPE_UNDEFINED);
        }
    }
  return;
}

void
dum (void)
{
  return;
}
/*
   * Reset one room.
 */
void
reset_room (ROOM_DATA * pRoom)
{
  RESET_DATA *pReset;
  CHAR_DATA *pMob;
  SINGLE_OBJECT *pObj;
  CHAR_DATA *LastMob = NULL;
  SINGLE_OBJECT *LastObj = NULL;
  SINGLE_OBJECT *LastObjPrev = NULL;
  EXIT_DATA *pexit;
  bool last;
  int iExit;
  int olevel = 2;
  if (pRoom == NULL)
    return;
  if (get_room_index (pRoom->vnum) == NULL)
    {
      bug ("Reset_room: invalid room %d", pRoom->vnum);
      return;
    }
  pMob = NULL;
  last = TRUE;
  for (iExit = 0; iExit < MAX_DIR; iExit++)
    {
      if (((pexit = (EXIT_DATA *) pRoom->exit[iExit]) != NULL) && pexit->d_info)
        {
          if (pexit->d_info->str < 0 && !IS_SET (pexit->d_info->exit_info, EX_RESET))
            continue;
	  pexit->d_info->exit_info = pexit->d_info->rs_flags;
          if (IS_SET(pexit->d_info->exit_info, EX_CLOSED) &&
              IS_SET(pexit->d_info->exit_info, EX_ISDOOR) &&
              number_range(1,100) == 2)
            SET_BIT(pexit->d_info->exit_info, EX_HASTRAP);
          if (IS_SET (pexit->d_info->rs_flags, EX_ISDOOR)
              && pexit->d_info->maxstr > 0)
            {
              pexit->d_info->str = pexit->d_info->maxstr;
            }
	  if (((ROOM_DATA *) pRoom->exit[iExit]->to_room) != NULL &&
	      (pexit = (EXIT_DATA *) ((ROOM_DATA *) pRoom->exit[iExit]->to_room)->exit[rev_dir[iExit]]) != NULL && pexit->d_info != NULL)
	    pexit->d_info->exit_info = pexit->d_info->rs_flags;
	}
    }
  if (!pRoom->more) return;
  LastMob = NULL;
  LastObj = NULL;
  for (pReset = pRoom->more->reset_first; pReset != NULL; pReset = pReset->next)
    {
      MOB_PROTOTYPE *pMobIndex;
      OBJ_PROTOTYPE *pObjIndex;
      pObj = NULL;
      if ((pReset->percent > 100) && (pReset->percent < 110))
	{
	  int chance = pReset->percent -100;
	  if (number_range(1,100) > chance)
	    {
	      if (pReset->command == 'M')
		LastMob = NULL;
	      continue;
	    }
	}
      else if (number_percent () > UMAX(pReset->percent, 1))
        {
          if (pReset->command == 'M')
            LastMob = NULL;
          continue;
        }
      switch (pReset->command)
        {
        default:
          bug ("Reset_room: bad command %c.", pReset->command);
          break;
        case 'M':
          if ((pMobIndex = get_mob_index (pReset->rs_vnum)) == NULL)
            {
              bug ("Reset_room: 'M': bad vnum %d.", pReset->rs_vnum);
              bug ("Reset in room : %d.", pRoom->vnum);
	              continue;
            }
          if (pMobIndex->count >= pMobIndex->max_in_world /*pReset->loc */ )
            {
              LastMob = NULL;
              break;
            }
	  if(IS_SET(pMobIndex->act, ACT_REBOOT_ONLY)&& pMobIndex->killed > 0)
	    {
	      LastMob = NULL;
              break;
            }
          pMob = create_mobile (pMobIndex);
          if (room_is_dark (pRoom))
            SET_BIT (pMob->affected_by, AFF_INFRARED);
          char_to_room (pMob, pRoom);

/* Mob was born, check for triggers on mobbie! */
        checkagain:
          {
            SINGLE_TRIGGER *tr;
            SCRIPT_INFO *s;
            for (tr = trigger_list[TCREATED]; tr != NULL; tr = tr->next)
              {
                if (pMob->pIndexData->vnum == tr->attached_to_mob)
                  {
                    if (tr->running_info && !tr->interrupted)
                      continue; /* Already running, interrupted, but script says not to allow interruptions. */
                    if (tr->running_info && tr->interrupted != 2)
                      {
                        end_script (tr->running_info);
                        goto checkagain;
                      }
                    /* ----------------- */
                    /* Start the script! */
                    /* ----------------- */
                    tr->running_info = mem_alloc (sizeof (*tr->running_info));
                    s = tr->running_info;
                    bzero (s, sizeof (*s));
                    s->current = pMob;
                    s->mob = pMob;
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
/* End trigger check! */


          if (IS_AFFECTED (pMob, AFF_SLEEP))
            {
              do_sleep (pMob, "");
            }
          if (IS_SET (pMob->pIndexData->act3, ACT3_RIDE))
            {
              if (LastMob != NULL && RIDING (LastMob) == NULL)
                {
                  do_mount (pMob, RNAME (LastMob));
                }
            }
          if (IS_SET (pMob->pIndexData->act3, ACT3_FOLLOW))
            {
              if (LastMob != NULL)
                {
                  add_follower (pMob, LastMob);
                }
            }
          LastObj = NULL;
          LastObjPrev = NULL;
          LastMob = pMob;
          olevel = LEVEL (pMob);
          
          break;
        case 'O':
	  {
	    int vnum = pReset->rs_vnum;
	    pObj = NULL;
	    if(vnum > 90000 && vnum < 90999 && vnum != 90017 && vnum != 90018) continue;
	    if(vnum > 550 && vnum < 603) continue;
	    if(vnum> 99 && vnum < 180)
	      { /* Special pops will go in here. */
		int i = vnum;
		switch (i)
		  {
		  case 100: /* Clothpop */
		    vnum = number_range(200, 204);
		    break;
		  case 101: /* Leatherpop */
		    vnum = number_range(205, 218);
		    break;
		  case 102: /* Chainpop */
		    vnum = number_range(219, 223);
		    break;
		  case 103: /* Metalpop */
		    vnum = number_range(224, 231);
		    break;
		  case 104: /* Steelpop */
		    vnum = number_range(232, 237);
		    break;
		  case 105: /* Leather or less pop */
		    vnum = number_range(200, 218);
		    break;
		  case 106: /* Chain or less pop */
		    vnum = number_range(200, 223);
		    break;
		  case 107: /* Metal or less pop */
		    vnum = number_range(200, 231);
		    break;
		  case 108: /* Steel or less pop */
		    vnum = number_range(200, 237);
		    break;
		  case 109: /* Chain or more pop */
		    vnum = number_range(219, 237);
		    break;
		  case 110: /* Metal or more pop */
		    vnum = number_range(224, 237);
		    break;
		  case 111:
		    {
		      int jj;
		      for (jj = 0; jj < 7; jj++)
			if (number_range(1,3) == 2) break;
		      vnum = 90019 + (jj * 40);
		    }
		  break;
		  case 112:
		    {
		      int jj;
		      for (jj = 0; jj < 8; jj++)
			if (number_range(1,3) == 2) break;
		      if (jj == 7 && number_range(1,3) != 2)
			jj = 0;
		      vnum = 90139 + (jj * 40);
		    }
		  break;
		  case 113:
		    {
		      int jj;
		      for (jj = 0; jj < 9; jj++)
			if (number_range(1,3) == 2) break;
		      if (jj == 8 && number_range(1,5) != 3) 
			jj = 0;
		      vnum = 90219 + (jj * 40);
		    }
		  break;
		  case 114: /* mineral pop */ 
		    {
		      int jj;
		      for (jj = 0; jj < 16; jj++)
			if (number_range(1,3) == 2) 
			  break;
		      if (jj == 15 && number_range(1,4) != 3) jj = 0;
		      vnum = 90019 + (jj * 40);
		    } 
		  break;
		  case 115: /* Wood */
		    {
		      vnum = number_range(551,560);
		    }
		  break;
		  case 116: /* Herbs */
		    {
		    vnum = number_range(561,602);
		    }
                  break;
		  case 117: /* Dagger */
		    {
                      if (number_range(1,2) != 2)
                        vnum = 250; 
                      else if (number_range(1,2) != 2)
                        vnum = 251;
                      else if (number_range(1,2) != 2)
                        vnum = 252;
                      else vnum = 253;                         
                      break;
		    }
		  case 118: /* Swords */
			    {
                      if (number_range(1,2) != 2)
                        vnum = 260; 
                      else if (number_range(1,2) != 2)
                        vnum = 261;
                      else if (number_range(1,2) != 2)
                        vnum = 262;
                      else vnum = 263;                         
                      break;
		    }

		  case 119: /* Clubs */
		    {
                      if (number_range(1,2) != 2)
                        vnum = 270; 
                      else if (number_range(1,2) != 2)
                        vnum = 271;
                      else if (number_range(1,2) != 2)
                        vnum = 272;
                      else vnum = 273;                         
                      break;
		    }

		  case 120: /* Crap wpn */
		    {
                      if (number_range(1,3) != 2)
                        vnum = 250; 
                      else if (number_range(1,3) != 2)
                        vnum = 251;
                      else vnum = 252;
                      vnum += 10*number_range(0,2);
                      break;
		    }

		  case 121: /* Good wpn */
		    {
                      if (number_range(1,2) == 2)
                        vnum = 252; 
                      else if (number_range(1,3) != 2)
                        vnum = 253;
                      else 
                        vnum = 251;
                      vnum += 10*number_range(0,2);
                      break;
		    }
             
		  break;
		  }
	      }
	    if ((pObjIndex = get_obj_index (vnum)) == NULL)
	      {
		bug ("Reset_room: 'O': bad vnum %d.", pReset->rs_vnum);
		bug ("Reset in room : %d.", pRoom->vnum);
		continue;
	      }
	    
	    if (pReset->loc == RESET_LOC_INROOM)
	      {
		SINGLE_OBJECT *obj2;
		int count_in_room;
		count_in_room = 0;
		if (pRoom->more)
		  for (obj2 = pRoom->more->contents; obj2 != NULL; obj2 = obj2->next_content)
		    {
		      if (obj2->pIndexData->vnum == /*pObj->pIndexData->vnum */ pObjIndex->vnum)
			count_in_room++;
		    }
		if (pObjIndex->max_in_room <= count_in_room)
		  continue;
		pObj = create_object (pObjIndex, number_fuzzy (olevel));
		obj_to (pObj, pRoom);
		if ( /*LastObj == NULL || */ pObj->pIndexData->item_type == ITEM_CONTAINER)
		  {
		    LastObjPrev = LastObj;
		    LastObj = pObj;
		  }
		continue;
	      }
	    if (pReset->loc == RESET_LOC_INOBJ && LastObj != NULL)
	      {
		pObj = create_object (pObjIndex, number_fuzzy (olevel));
		obj_to (pObj, LastObj);
		continue;
	      }
	    if (pReset->loc == RESET_LOC_ONOBJ && LastObj != NULL)
	      {
		pObj = create_object (pObjIndex, number_fuzzy (olevel));
		obj_to (pObj, LastObj);
		continue;
	      }
	    if (LastMob != NULL)
	      {
		pObj = create_object (pObjIndex, number_fuzzy (olevel));
		obj_to (pObj, LastMob);
		if ( /*LastObj == NULL || */ pObj->pIndexData->item_type == ITEM_CONTAINER)
		  {
		    LastObjPrev = LastObj;
		    LastObj = pObj;
		  }
		/* For Shopkeepers */
		if (LastMob->pIndexData->pShop != NULL
		    && pObj->wear_loc == WEAR_NONE)
		  {
		    /* pObj->wear_loc = URANGE( WEAR_NONE, pReset->loc, MAX_WEAR ); */
		    pObj->wear_loc = -1;
		    SET_BIT (pObj->extra_flags, ITEM_INVENTORY);
		    if (pReset->loc > 1)
		      {
			int counter;
			for (counter = 1; counter < pReset->loc; counter++)
			  {
			    pObj = NULL;
			    pObj = create_object (pObjIndex, 1);
			    obj_to (pObj, LastMob);
			    pObj->wear_loc = WEAR_NONE;
			    SET_BIT (pObj->extra_flags, ITEM_INVENTORY);
			  }
		      }
		    continue;
		  }
		if (pReset->loc != 99)
		  wear_obj (LastMob, pObj, FALSE, 0);
	      }
	    break;
	  }
	}
      
      /*END OF CASE */
    }
/*END OF LOOP */

/* THIS ACTIVATES ALIGN BONUSES FROM CAPTURED ROOMS */
  if (pRoom->sector_type == SECT_CAPTURE)
  {
    if (!(strchr(pRoom->cbonusstr, 's') == NULL))
      align_info[pRoom->calign].bonus[0] = 1;
    if (!(strchr(pRoom->cbonusstr, 'i') == NULL))
      align_info[pRoom->calign].bonus[1] = 1;
    if (!(strchr(pRoom->cbonusstr, 'w') == NULL))
      align_info[pRoom->calign].bonus[2] = 1;
    if (!(strchr(pRoom->cbonusstr, 'd') == NULL))
      align_info[pRoom->calign].bonus[3] = 1;
    /* if (!(strchr(pRoom->cbonusstr, 'c') == NULL))
      align_info[pRoom->calign].bonus[4] = 1; */
  }

  return;
}

void
do_purgearea (CHAR_DATA * ch, char *argy)
{
  ROOM_DATA *pRoom;
  int ihash;
  DEFINE_COMMAND ("areapurge", do_purgearea, POSITION_DEAD, 101,LOG_NORMAL, "This command performs an invisible purge command in all rooms in the current area.")
    if (IS_MOB (ch))
    return;
  for (ihash = 0; ihash < HASH_MAX; ihash++)
    {
      for (pRoom = room_hash[ihash]; pRoom != NULL; pRoom = pRoom->next)
        {
          if (ch->in_room->area != pRoom->area)
            continue;
          raw_purge (pRoom);
        }
    }
  send_to_char ("Area purged (all objects and mobs).\n\r", ch);
  return;
}

bool temp=FALSE;
void
do_setmiw (CHAR_DATA * ch, char *argy)
{
  int i;
  int j;
  MOB_PROTOTYPE *m;
  int hash;
  char tm[SML_LENGTH];
  char arg[SML_LENGTH];
  DEFINE_COMMAND ("z_setmiw", do_setmiw, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, " use...")
    argy = one_argy (argy, arg);
  if (!is_number (arg) || !is_number (argy))
    return;
  i = atoi (arg);
  j = atoi (argy);
sprintf(arg,"%d to %d.\n\r",i,j);
send_to_char(arg,ch);
temp=TRUE;
  for (hash = 0; hash < HASH_MAX; hash++)
    {
      for (m = mob_index_hash[hash]; m != NULL; m = m->next)
        {
          if (m->vnum < i || m->vnum > j)
            continue;
        strcpy(tm,itoa(m->vnum));
          do_mreset (ch, tm);
          m->max_in_world = ti;
          SET_BIT (m->area->area_flags, AREA_CHANGED);
        }
    }
temp=FALSE;
  send_to_char ("Done.\n\r", ch);
  return;
}


void
do_mreset (CHAR_DATA * ch, char *argy)
{
  RESET_DATA *pReset;
  ROOM_DATA *pRoom;
  int ihash;
  int reset_num;
  char general_use[500];
  DEFINE_COMMAND ("mreset", do_mreset, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This command can be used to find resets of the specified mob prototype.")
    if (IS_MOB (ch))
    return;
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("mreset <vnum> (vnum is mob number to find reset of)\n\r", ch);
      return;
    }
  if (is_number (argy))
    {
      ti = 0;
      reset_num = atoi (argy);
      if (reset_num < 1)
        return;
      for (ihash = 0; ihash < HASH_MAX; ihash++)
        {
          for (pRoom = room_hash[ihash]; pRoom != NULL; pRoom = pRoom->next)
            {
              if (!pRoom->more || pRoom->more->reset_first == NULL)
                continue;
              for (pReset = pRoom->more->reset_first; pReset != NULL; pReset = pReset->next)
                {
                  if (pReset->rs_vnum == reset_num && pReset->command == 'M')
                    {
                      ti++;
                        if (!temp) {
                      sprintf (general_use, "Mob %d reset in room %d at %d chance.\n\r",
                               reset_num, pRoom->vnum, pReset->percent);
                      send_to_char (general_use, ch);
                        }
                    }
                }
            }
        }
    }
  else
    {
      MOB_PROTOTYPE *mmid;
      int iHsh;
      bool found_item;
      bool area_only = TRUE;
      if (str_prefix ("maintenance", argy))
        return;
      if (!str_infix ("all", argy))
        area_only = FALSE;
      for (iHsh = 0; iHsh < HASH_MAX; iHsh++)
        {
          for (mmid = mob_index_hash[iHsh]; mmid != NULL; mmid = mmid->next)
            {
              if (mmid->area->open == 0)
                continue;
              if (area_only && mmid->area != ch->in_room->area)
                continue;
              found_item = FALSE;
              for (ihash = 0; ihash < HASH_MAX; ihash++)
                {
                  for (pRoom = room_hash[ihash]; pRoom != NULL; pRoom = pRoom->next)
                    {
                      if (!pRoom->more || pRoom->more->reset_first == NULL)
                        continue;
                      for (pReset = pRoom->more->reset_first; pReset != NULL; pReset = pReset->next)
                        {
                          if (pReset->rs_vnum == mmid->vnum && pReset->command == 'M')
                            {
                              found_item = TRUE;
                            }
                        }
                      if (found_item)
                        goto firstloopbreak;
                    }
                }
              sprintf (general_use, "Mob %d is unused!\n\r", mmid->vnum);
              write_to_descriptor2 (ch->desc, general_use, 0);
              mmid->guard = -32000;
            firstloopbreak:
                  if (mmid) {};
          }
        }
    }
  return;
}

void
do_oreset (CHAR_DATA * ch, char *argy)
{
  RESET_DATA *pReset;
  ROOM_DATA *pRoom;
  char general_use[500];
  int reset_num;
  int ihash;
  DEFINE_COMMAND ("oreset", do_oreset, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This command can be used to find resets of the specified object prototype.")
    if (IS_MOB (ch))
    return;
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("oreset <vnum> (vnum is object number to find reset of)\n\r", ch);
      return;
    }
  if (is_number (argy))
    {
      reset_num = atoi (argy);
      if (reset_num < 1)
        return;
      for (ihash = 0; ihash < HASH_MAX; ihash++)
        {
          for (pRoom = room_hash[ihash]; pRoom != NULL; pRoom = pRoom->next)
            {
              if (!pRoom->more || pRoom->more->reset_first == NULL)
                continue;
              for (pReset = pRoom->more->reset_first; pReset != NULL; pReset = pReset->next)
                {
                  if (pReset->rs_vnum == reset_num && pReset->command == 'O')
                    {
                      sprintf (general_use, "Object %d reset in room vnum %d at %d chance.\n\r",
                               reset_num, pRoom->vnum, pReset->percent);
                      send_to_char (general_use, ch);
                    }
                }
            }
        }
    }
  else
    {
      int iHsh;
      OBJ_PROTOTYPE *objj;
      bool found_item;
      bool area_only = TRUE;
      if (str_prefix ("maintenance", argy))
        return;
      if (!str_infix ("all", argy))
        area_only = FALSE;
      for (iHsh = 0; iHsh < HASH_MAX; iHsh++)
        {
          for (objj = obj_index_hash[iHsh]; objj != NULL; objj = objj->next)
            {
              if (objj->area->open == 0)
                continue;
              if (area_only && objj->area != ch->in_room->area)
                continue;
              found_item = FALSE;
              for (ihash = 0; ihash < HASH_MAX; ihash++)
                {
                  for (pRoom = room_hash[ihash]; pRoom != NULL; pRoom = pRoom->next)
                    {
                      if (!pRoom->more || pRoom->more->reset_first == NULL)
                        continue;
                      for (pReset = pRoom->more->reset_first; pReset != NULL; pReset = pReset->next)
                        {
                          if (pReset->rs_vnum == objj->vnum && pReset->command == 'O')
                            {
                              found_item = TRUE;
                            }
                        }
                      if (found_item)
                        goto firstloopbreak;
                    }
                }
              sprintf (general_use, "Object %d is unused!\n\r", objj->vnum);
              write_to_descriptor2 (ch->desc, general_use, 0);
              objj->item_type = 99999;
            firstloopbreak:
                  if (objj) {};
            }
        }
    }
  return;
}



void
room_update (void)
{
  ROOM_DATA *pRoom;
  TRACK_DATA *tr_n;
  TRACK_DATA *tr;
  PTRACK_DATA *ptr; /* JRAJRA */
  PTRACK_DATA *ptr_n; /* JRAJRA */
  CHAR_DATA *pch;
  AREA_DATA *ar;
  bool stp = FALSE;
  bool dried = FALSE;
  int i = 0;
  int blud = 0;
  int iHash;
  for (ar = area_first; ar != NULL; ar = ar->next)
    ar->repop_counter--;
  for (iHash = 0; iHash < HASH_MAX; iHash++)
    {
      for (pRoom = room_hash[iHash]; pRoom != NULL; pRoom = pRoom->next)
	{
	  if (!pRoom->area || pRoom->area->repop_counter % 6 != 0) continue;
	  if (pRoom->blood != 0) /* Check for blood and then remove it */
	    {
	      dried = FALSE;
	      blud = pRoom->blood;
	      for (i = 0 ; i < 6 ; i++)
		{
		  if(IS_SET(blud,blood_number[i]) && number_range(1,4) !=1)
		    {
		      REMOVE_BIT(blud, blood_number[i]);
		      dried = TRUE;
		    }
		}
	      pRoom->blood = blud; 
	      if (dried && pRoom->more && pRoom->more->people)
		{
		  for (pch = pRoom->more->people; pch != NULL; pch=pch->next_in_room)
		    send_to_char("\x1b[0;31mA blood trail dries up.\x1b[0;37m\n\r", pch);                  
		}
	    }
	  for (tr = pRoom->tracks; !stp && tr; tr = tr_n)
	    {
	      tr_n = tr->next_track_in_room;
	      if (tr_n == NULL)
		stp = TRUE;
	      free_m (tr);
	    }
	  stp = FALSE;
	  pRoom->tracks = NULL;
	  
	  /* JRAJRA - Clean player tracks in room */
          /* MATUSE - ONE IN 10 CHANCE OF TRACKS WIPING */
	  if (number_range(1,10) == 2)
	    {
	      for (ptr = pRoom->ptracks; !stp && ptr; ptr = ptr_n)
		{
		  ptr_n = ptr->next_track_in_room;
		  if (ptr_n == NULL)
		    stp = TRUE;
		  free_m (ptr);
		}
	      stp = FALSE;
	      pRoom->ptracks = NULL;
	    }
	  
	  if (pRoom->area->repop_counter == 0)
	    {
	      if (pRoom->area->repop[0] != '\0' && pRoom->more)
		for (pch = pRoom->more->people; pch != NULL; pch = pch->next_in_room)
		  send_to_char (pRoom->area->repop, pch);
	      reset_room (pRoom);
	    }
	}
    }
  for (ar = area_first; ar != NULL; ar = ar->next)
    {
      if(ar->repop_counter <= 0)
	ar->repop_counter = UMIN(20,ar->repop_rate);
    }
  return;
}


void
update_handler (void)
{
  pulse_upd--;
  if (pulse_upd % 25 == 12)
    transport_update (); /* Every 2.5 seconds transport update */
  if (pulse_upd % 90 == 78) /* Every 9 seconds plr update */
    {
      int i;
      if (saving != NULL)
          for (i = 0; i < 7; i++)
            {
              save_area (saving);
              REMOVE_BIT (saving->area_flags, AREA_CHANGED);
              saving = saving->next;
              if (saving == NULL)
                {
                  do_global ("Database save finished.\n\r", 0, 0);
                  break;
                }
            }
      auction_update();
      char_only_update ();
    }
  if (pulse_upd % 70 == 32)
      mobile_update (); /* Update mobs every 7 seconds */
  if (pulse_upd % 35 == 6)
      violence_update (); /* One fight round every 4.0 seconds */
  if (pulse_upd % 400 == 200) /* Hourly "tick" every 40 seconds */
    {
      char_update ();
      obj_update ();
      if (biddata.waitticks == 0)
        {
          biddata.waitticks = -1;
          arena ();
        }
      if (biddata.waitticks > 0)
        biddata.waitticks--;
      weather_update ();
      if (ticks_to_reboot == 0)
        {
          hours_up++;
          /* MATUSE - THIS DETERMINES REBOOT TIME */
          /* 450 was 5-8 hour reboots */
          if (hours_up >= 850 && number_range(1,200)<=(1+(hours_up-450)/90))
            {
              ticks_to_reboot = 15;
            }
        }
    }
  if (pulse_upd % 5 == 0)
      aggr_update (); /* Aggros checked 2x per second */
  if (pulse_upd <= 0)
    {
       pulse_upd = PULSE_AREA;
       if (dice (1, 120) == 1356)
        do_disaster ();
       
     }
  return;
}

void
do_disaster ()
{
  CHAR_DATA *mob;
  AREA_DATA *area;
  area = NULL;
  for (mob = char_list; mob != NULL; mob = mob->next)
    {
      if (number_percent () > 90)
        {
          if (mob->in_room && mob->in_room->area)
            area = mob->in_room->area;
          break;
        }
    }
  if (area == NULL)
    return;
  for (mob = char_list; mob != NULL; mob = mob->next)
    {
      if (mob->in_room == NULL || mob->in_room->area == NULL)
        continue;
      if (mob->in_room->area == area)
        {
          if (IS_AFFECTED (mob, AFF_FLYING))
            {
              send_to_char ("You see the earth shake and rumble below!\n\r",
                            mob);
              continue;
            }
          send_to_char ("The earth shakes and rumbles violently!\n\r", mob);
          if (dice (3, 10) > get_curr_dex (mob))
            {
              send_to_char ("The quake knocks you to the ground.\n\r", mob);
              act ("$n is knocked down by the quake.", mob, NULL, NULL, TO_ROOM);
              if (IS_SET (mob->act, PLR_HOLYWALK))
                continue;
              NEW_POSITION(mob, POSITION_RESTING);
              if (mob->hit > 11)
                {
                  SUBHIT(mob, 10);
                }
            }
        }
    }
  return;
}
