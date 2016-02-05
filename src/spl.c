#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

bool is_elemental_aligned(CHAR_DATA * ch, SPELL_DATA * spell) {
	
  //char buffer[500];
  //sprintf(buffer,"Mana Type is %d\n\r",spell->mana_type);
  //send_to_char (buffer,ch);

  if(IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_WATER) && spell->mana_type == 8)
    return true;
  else if(IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_FIRE) && spell->mana_type == 4)
    return true;
  else if(IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_EARTH) && spell->mana_type == 1)
    return true;
  else if(IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_AIR) && spell->mana_type == 2)
    return true;
  else if(IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_GOOD) && spell->mana_type == 16)
    return true;
  else if(IS_SET(ch->pcdata->elemental_alignment,ELEMENTAL_ALIGN_EVIL) && spell->mana_type == 16)
    return true;
  return false;
}


void
show_hitdam (int gsn, char *noun, int dam, CHAR_DATA * ch, CHAR_DATA * victim)
{
#ifdef NEW_WORLD
  int ty;
  char rvp[1024];
  char rss[1024];
  char *tt;
  ty = 0;
  if (victim->hit < 0)
    return;
  sprintf (rss, "$B$6Your %s *a $N.$R$7", noun);
  for (tt = rss; *tt != '\0'; tt++)
    {
      if (*tt == '*')
	{
	  tt++;
	  if (*tt == 'a')
	    {
	      strcat (rvp, (star_a (dam, victim->max_hit)));
	      ty = strlen (rvp);
	    }
	}
      else
	{
	  rvp[ty] = *tt;
	  rvp[ty + 1] = '\0';
	  ty++;
	}
    }

  act (rvp, ch, NULL, victim, TO_CHAR);
#else
  dam_message (ch, victim, dam, gsn, 0);
#endif
  return;
}


void
spell_summon (int sn, int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *vict;
  if (!target_name || target_name[0] == '\0')
    return;
  if ((vict = get_char_world (ch, target_name)) == NULL)
    {
      send_to_char ("The spell failed.\n\r", ch);
      return;
    }
  if (FIGHTING (vict) || LEVEL (vict) > LEVEL (ch) + 20 || !vict->in_room->area->open ||
      vict->in_room->vnum < 1000 || vict == ch ||
      ch->in_room->vnum < 1000)
    {
      send_to_char ("The spell failed.\n\r", ch);
      return;
    }
  if (vict->pcdata->no_quit_pk > 0 || ch->pcdata->no_quit_pk > 0)
    {
      send_to_char("The spell failed.\n\r", ch);
      return;
    }
  if (vict->pcdata->no_quit > 0)
    {
      send_to_char("Your victim has recently cast a spell of great power and you cannot get a lock on them.\n\r", ch);
      return;
    }
  if (IS_MOB (vict) && !pow.can_summon_mobs)
    {
      send_to_char ("The spell failed.\n\r", ch);
      return;
    }
  if ((vict->in_room && IS_SET (vict->in_room->room_flags, ROOM_NOSUMMON)) ||
      (ch->in_room && IS_SET (ch->in_room->room_flags, ROOM_NOSUMMON)))
    {
      send_to_char ("The spell failed.\n\r", ch);
      return;
    }
  if (IS_PLAYER (vict) && IS_PLAYER (ch) && (DIFF_ALIGN(ch, vict)))
    {
      send_to_char ("The spell failed.\n\r", ch);
      return;
    }

  act ("$n disappears in a flash of bright light!", vict, NULL, vict, TO_ROOM);
  char_from_room (vict);
  char_to_room (vict, ch->in_room);
  act ("$n appears in a blinding flash of light!", vict, NULL, vict, TO_ROOM);
  send_to_char ("You have been summoned!!\n\r", vict);
  do_look (vict, "auto");
  return;
}

void
spell_teleport (int sn, int level, CHAR_DATA * ch, void *vo)
{
  ROOM_DATA *rd;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int i;
  if (!victim)
    victim = ch;
  if (FIGHTING (ch))
    {	
      send_to_char ("The spell failed!\n\r", ch);
      return;
    }
/*
   if (IN_BATTLE(ch))
    {	
      send_to_char ("Not in the battleground!\n\r", ch);
      return;
    }     
   if (IN_BATTLE(victim))
    {	
      send_to_char ("Not in the battleground!\n\r", ch);
      return;
    }     
  if (IS_IMMORTAL(victim) && IS_PLAYER(victim))
    {	
      send_to_char ("The spell failed!\n\r", ch);
      return;
    }  
*/
if (IS_MOB (victim))
      {
       send_to_char ("The spell failed!\n\r", ch);
     return;
    }
  for (i = 0;i < 10000;i++)
  {
    while ((rd = get_room_index (number_range (1001, 99999))) == NULL || !rd->area->open ||
	 rd->sector_type == SECT_WATER_NOSWIM || rd->sector_type == SECT_WATER_SWIM ||
	 IS_SET (rd->room_flags, ROOM_NOSUMMON));
    if (IS_SET (rd->room_flags, ROOM_CANTELEPORT))
	break;
  }
  if (IS_SET(rd->room_flags, ROOM_CANTELEPORT))
  {
      act ("$n disappears!", victim, NULL, victim, TO_ROOM);
      char_from_room (victim);
      char_to_room (victim, rd);
  }
  else
      send_to_char ("The barriers of space are too thick, you remain where you are!\n\r", ch);

  return;
}
void
spell_word_of_recall (int sn, int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  if (IS_MOB (ch))
    return;
  if (!victim)
    victim = ch;
  if (IS_MOB (victim))
    return;
  if (ch->in_room && IS_SET (ch->in_room->room_flags, ROOM_NO_RECALL))
    {
      send_to_char ("The spell failed!\n\r", ch);
      return;
    }
  if (FIGHTING (ch))
    {
      send_to_char ("The spell failed!\n\r", ch);
      return;
    }

  if (DIFF_ALIGN(ch, victim))
    return;

  if (IS_AFFECTED (victim, AFF_CURSE)) {
	  if (victim==ch) {
		send_to_char("A powerful curse is preventing you from recalling!\n\r", ch);
	  } else {
		send_to_char("A powerful curse prevents your spell from succeeding!\n\r",ch);
	  }
	  return;
  }
  act ("$n disappears!", victim, NULL, victim, TO_ROOM);
  char_from_room (victim);
  char_to_room (victim, get_room_index (ch->pcdata->alignment+100));
  victim->pcdata->no_quit_pk = 0; /* JRAJRA */
  victim->pcdata->no_quit = 0;
  return;
}

void
scatter_mob (CHAR_DATA * mob)
{
  int just_moved_dir = -1;
  int moved_rooms = 0;
  int move_tries = 0;
  int move_dir;

  if (!mob->in_room)
    return;

  while (move_tries < 20 && moved_rooms < 3)
    {
      move_dir = number_range (0, 5);
      while (move_dir == rev_dir[just_moved_dir])
	move_dir = number_range (0, 5);
      if (move_char (mob, move_dir))
	{
	  move_tries++;
	  moved_rooms++;
	  just_moved_dir = move_dir;
	  continue;
	}
      else
	move_tries++;
    }

  return;
}

void
do_spray (CHAR_DATA * ch, char *argy)
{
  char buf[500];
  char arg1[SML_LENGTH];
  SINGLE_OBJECT *spray;
  CHAR_DATA *victim;
  DEFINE_COMMAND ("spray", do_spray, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to spray a bottle of repellant at a mob.")

    argy = one_argy (argy, arg1);
  if (!str_prefix ("with", argy))
    argy = one_argy (argy, buf);

  if (argy == "" || argy[0] == '\0')
    {
      if ((spray = get_item_held (ch, ITEM_REPELLANT)) == NULL)
	{
	  send_to_char ("Spray who with what?\n\r", ch);
	  return;
	}
    }
  else if ((spray = get_obj_carry (ch, argy)) == NULL)
    {
      if ((spray = get_obj_wear (ch, argy)) == NULL)
	{
	  send_to_char ("You don't seem to be carrying that.\n\r", ch);
	  return;
	}
    }

  if (spray->pIndexData->item_type != ITEM_REPELLANT)
    {
      send_to_char ("You can't spray that!\n\r", ch);
      return;
    }

  if ((victim = get_char_room (ch, arg1)) == NULL)
    {
      send_to_char ("I don't see a creature or person like that to spray it on.\n\r", ch);
      return;
    }

/* Spraying it.. remove a 'squirt' */

  ((I_SPRAY *) spray->more)->sprays--;

  if (IS_PLAYER (victim))
    {
      act ("You spray $N, but $E doesn't seem to be affected.", ch, NULL, victim, TO_CHAR);
      act ("$n sprays some liquid in your face, but nothing happens.", ch, NULL, victim, TO_VICT);
      act ("$n sprays some liquid in $N's face, but nothing seems to happen.", ch, NULL, victim, TO_NOTVICT);
    }

  if (IS_MOB (victim))
    {
      int i;
      bool yes_no = FALSE;
      for (i = 0; i < 9; i++)
	if (((I_SPRAY *) spray->more)->repels[i] == victim->pIndexData->vnum)
	  {
	    yes_no = TRUE;
	    break;
	  }
      if (!yes_no)
	{
	  act ("You spray $N, but $E doesn't seem to be affected.", ch, NULL, victim, TO_CHAR);
	  act ("$n sprays some liquid at you, but nothing happens.", ch, NULL, victim, TO_VICT);
	  act ("$n sprays some liquid at $N, but nothing seems to happen.", ch, NULL, victim, TO_NOTVICT);
	}
      else
	{
	  act ("You spray $N.", ch, NULL, victim, TO_CHAR);
	  act ("$n sprays some liquid at you.", ch, NULL, victim, TO_VICT);
	  act ("$n sprays some liquid at $N.", ch, NULL, victim, TO_NOTVICT);
	  if (FIGHTING (victim))
	    {
	      if (HUNTING (victim) && !str_cmp (HUNTING (victim), NAME (ch)))
		{
		  free_string (HUNTING (victim));
		  victim->fgt->hunting = NULL;
		}
	      stop_fighting (FIGHTING (victim), TRUE);
	      stop_fighting (victim, TRUE);
	    }
	  scatter_mob (victim);
	}
    }

  if (((I_SPRAY *) spray->more)->sprays < 1)
    {
      send_to_char ("The spray is empty, and magically disappears!\n\r", ch);
      free_it (spray);
    }

  return;
}


/* Fix this!!! */
void
spell_dispel_magic (int sn, int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  if (!IS_PLAYER(ch))
    return;
  while (victim->affected)
  {
    affect_remove (victim, victim->affected);
      break;
  }
      victim->affected_by = victim->pcdata->nat_abilities;
      victim->more_affected_by = victim->pcdata->nat_abilities2;
      FIXIT(victim);
  /* JRAJRA - Old dispel magic code..
  if (victim->affected_by == 0
      || level + 30 < LEVEL (victim)
      || (saves_spell (level, victim) && saves_spell (level, victim)))
    {
      send_to_char ("You failed.\n\r", ch);
      return;
    }
  for (;;)
    {
      affected_by = 1 << number_bits (5);
      if (IS_SET (victim->affected_by, affected_by))
	break;
    }
  REMOVE_BIT (victim->affected_by, affected_by);
  send_to_char ("You successfully removed a spell from the victim!\n\r", ch);
  act ("$n just removed a spell effect from you!", ch, NULL, victim, TO_VICT);
  return;
  */
}

void
spell_create_object (SPELL_DATA * spell, int level, CHAR_DATA * ch, void *vo)
{
  SINGLE_OBJECT *obj;
  if ((obj = create_object (get_obj_index (spell->creates_obj), 0)) == NULL)
    {
      send_to_char ("A god has set up this spell wrong.. please contact a god.\n\r", ch);
      return;
    }
  if (IS_SET (spell->spell_bits, SPELL_OBJ_TO_ROOM))
    obj_to (obj, ch->in_room);
  else
    {
      obj_to (obj, ch);
      obj->wear_loc = -1;
    }
  return;
}

bool
remove_sa (SPELL_DATA * spell, int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  if (!is_affectedt (victim, spell->bitvector) || number_range(1,9) >
	get_curr_wis(ch)  || number_range(1,9) > get_curr_wis(victim))
    {
      send_to_char ("You failed to remove any affects.\n\r", ch);
      return FALSE;
    }
  affect_strip_bits (victim, spell->bitvector);
  return TRUE;
}


void
write_stuff (SPELL_DATA * spell, CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (spell->act_to_ch_1)
    act (spell->act_to_ch_1, ch, NULL, victim, TO_CHAR);
  if (spell->act_to_ch_2)
    act (spell->act_to_ch_2, ch, NULL, victim, TO_CHAR);
  if (victim && spell->act_to_vict_1)
    act (spell->act_to_vict_1, ch, NULL, victim, TO_VICT);
  if (victim && spell->act_to_vict_2)
    act (spell->act_to_vict_2, ch, NULL, victim, TO_VICT);

  if (victim && ch == victim && spell->act_to_vict_1)
    act (spell->act_to_vict_1, ch, NULL, victim, TO_CHAR);
  if (victim && ch == victim && spell->act_to_vict_2)
    act (spell->act_to_vict_2, ch, NULL, victim, TO_CHAR);

  if (spell->act_to_notvict_1)
    act (spell->act_to_notvict_1, ch, NULL, victim, TO_NOTVICT);
  if (spell->act_to_notvict_2)
    act (spell->act_to_notvict_2, ch, NULL, victim, TO_NOTVICT);
  return;
}

void
general_spell (SPELL_DATA * spell, int level, CHAR_DATA * ch, void *vo)
{

  if (IS_PLAYER(ch) && spell->spell_type == TAR_CHAR_OFFENSIVE &&
      IS_AUGMENTED(ch, AUG_MIND_POWER) && IS_AUGMENTED(ch, AUG_HAND_SPD) &&
      IS_AUGMENTED(ch, AUG_LEG_SPD) && IS_AUGMENTED (ch, AUG_FOOT_SPD) &&
      IS_AUGMENTED(ch, AUG_FOCUS) && IS_AUGMENTED (ch, AUG_CHANNEL) &&
      IS_AUGMENTED(ch, AUG_MENTAL_ARMOR) && IS_AUGMENTED (ch, AUG_MANA_BOOST) &&
      IS_AUGMENTED(ch, AUG_MIND_FOCUS) && get_curr_wis(ch) > 27 &&
      get_curr_int(ch) > 27 && number_range(1,100) == 3)
    {
    send_to_char("\x1b[1;31mYou have achieved a double cast!!!\n\r", ch);
    general_spell2 (spell, level, ch, vo, FALSE, ch->in_room);    
    }
  general_spell2 (spell, level, ch, vo, FALSE, ch->in_room);
  return;
}


void
general_spell2 (SPELL_DATA * spell, int level, CHAR_DATA * ch, void *vo,
		bool area, ROOM_DATA *targetroom)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  CHAR_DATA *next = NULL;
  bool once = FALSE;
  int dur = 0;
  bool sskip = FALSE;
  bool cast_at_n = FALSE;
  bool casterBonus = false;
  bool victimBonus = false;
  bool alignshardBonus = false;

  // Set elemental Bonus here, and use booleans below
  if(spell != NULL && ch != NULL && victim != NULL) {
    if(is_elemental_aligned(ch,spell)) casterBonus = true;
    if(is_elemental_aligned(victim,spell)) victimBonus = true;
    if(IS_SET(victim->pcdata->elemental_alignment,ELEMENTAL_ALIGN_GOOD) && ch->pcdata->alignment != 2)
      alignshardBonus = true;
    if(IS_SET(victim->pcdata->elemental_alignment,ELEMENTAL_ALIGN_EVIL) && ch->pcdata->alignment != 1)
      alignshardBonus = true;
  }


  if (IS_SET (spell->spell_bits, SPELL_NOT_SELF) && ch == victim && !IS_SET (spell->spell_bits, SPELL_ALL_IN_ROOM))
    {
      send_to_char ("You find that you cannot cast this spell upon yourself...\n\r", ch);
      return;
    }

  if (spell->spell_type != TAR_CHAR_OFFENSIVE && IS_SET(spell->spell_bits, SPELL_HURT_OPP_ALIGN) && !DIFF_ALIGN(ch, victim))
    {
      send_to_char ("The spell failed.\n\r", ch);
      return;
    }
  if (spell->creates_obj != 0)
    {
      spell_create_object (spell, level, ch, vo);
    }
  if (!IS_SET (spell->spell_bits, SPELL_ONLY_MULTI_MOBS) || IS_MOB (victim))
    {
      if (ch && victim && IS_PLAYER (ch) && IS_PLAYER(victim) &&
	  IS_SET (spell->spell_bits, SPELL_ALL_IN_ROOM)  &&
	  spell->spell_type == TAR_CHAR_OFFENSIVE &&
	  (!DIFF_ALIGN(ch, victim)))
	{
	  sskip = TRUE;
	}
      if (!sskip && IS_SET (spell->spell_bits, SPELL_ALL_IN_ROOM))
	{
	  cast_at_n = TRUE;
	  if (victim != NULL && victim->in_room == ch->in_room)
	    victim = ch->in_room->more->people;
	  next = victim->next_in_room;
	}
    }


  for (once = TRUE; ((once || next != NULL) && (!victim || (victim && victim->data_type!=50))); victim = next)
    {
	  /* ARC:
		   Protect Neutral align from aggressive area spells
		   and prevent them from harming other players with same
		 */

	  if (victim && IS_PLAYER (victim) && ch && IS_PLAYER(ch) && spell->spell_type == TAR_CHAR_OFFENSIVE
		  && (ALIGN (victim) == 0 || ALIGN (ch) == 0) && !IN_BATTLE(ch) && ch->fgt->challenge != 10)
	  {
		  next = victim->next_in_room;
		  continue;
	  }
		  

      if (victim && IS_PLAYER (victim) && cast_at_n && IS_SET (spell->spell_bits, SPELL_ONLY_MULTI_MOBS))
	{
	  next = victim->next_in_room;
	  continue;
	}
      else
	{
	  if (!sskip && IS_SET (spell->spell_bits, SPELL_ALL_IN_ROOM))
	    next = victim->next_in_room;
	  else
	    next = NULL;
	}
      once = FALSE;
      if (!sskip && ch)
	{
				
	  if (IS_SET (spell->spell_bits, SPELL_ONLY_GROUP) &&
	      (DIFF_ALIGN(ch, victim) ||
	       (IS_PLAYER (ch) != IS_PLAYER (victim))))
	    continue;
	  if (IS_SET (spell->spell_bits, SPELL_ONLY_NOT_GROUP) &&
	      !DIFF_ALIGN(ch, victim) &&
	      IS_MOB(ch) == IS_MOB(victim))
	    continue;
	}
/*
  if (IS_IMMORTAL(victim) && IS_PLAYER(victim))
    {	
      send_to_char ("Not on an immortal!\n\r", ch);
      return;
    }  
*/
      if (!victim)
	victim = ch;
      /* This is where we check all the "specials" */
      if (spell->spell_bits >= SPELL_SUMMON)
	{
	  if (IS_SET (spell->spell_bits, SPELL_LIFE_TAP))
	    spell_life_tap (spell->gsn, level, ch, vo);
	  if (IS_SET (spell->spell_bits, SPELL_RAISE_UNDEAD))
	    //spell_raise_undead (spell->gsn, level, ch, vo);
	    spell_raise_undead (spell, level, ch, vo);
	  if (IS_SET (spell->spell_bits, SPELL_SCRY))
	    spell_scry (spell->gsn, level, ch, vo);
	  if (IS_SET (spell->spell_bits, SPELL_LOCATE_PERSON))
	    spell_locate_person (spell->gsn, level, ch, vo);
	  if (IS_SET (spell->spell_bits, SPELL_REMOVES_BIT))
	    if (!remove_sa (spell, level, ch, victim))
	      {};
	  if (IS_SET (spell->spell_bits, SPELL_DISPEL_MAGIC))
	    spell_dispel_magic (spell->gsn, level, ch, vo);
	  if (IS_SET (spell->spell_bits, SPELL_IDENTIFY))
	    spell_identify (spell->gsn, level, ch, vo);
	  if (IS_SET (spell->spell_bits, SPELL_LOCATE_OBJECT))
	    spell_locate_object (spell->gsn, level, ch, vo);
	  if (IS_SET (spell->spell_bits, SPELL_TELEPORT))
	    spell_teleport(spell->gsn, level, ch, vo);
	  if (IS_SET (spell->spell_bits, SPELL_SUMMON))
	    spell_summon(spell->gsn, level, ch, vo);
	  if (IS_SET (spell->spell_bits, SPELL_RECALL))
	    spell_word_of_recall(spell->gsn, level, ch, vo);
	  if (IS_SET (spell->spell_bits, SPELL_MASS_DISTORTION))
	    {
	      victim->hit/=2;
	    }

	  if (IS_SET (spell->spell_bits, SPELL_ADD_MOVE))
	    {
	      int addmove = translate (spell->damage, level, ch);
	      int lvl = spell->spell_level;
	      int bonus = ((100 * get_curr_wis(ch))/((10 +lvl*2/7)));
	      affect_strip(ch, gsn_chameleon);
	      if (bonus > 120) bonus = 120;
	      if (bonus < 90) bonus /= 3;
	      if (bonus > 89)
		{
		  if (is_member(ch, GUILD_HEALER)) bonus += lvl;
		  if (is_member(ch, GUILD_MYSTIC)) bonus += lvl/2;
		  if (IS_AUGMENTED(ch, AUG_MIND_FOCUS)) bonus += lvl/3+3;
		  bonus += (ch->pcdata->remort_times) * 8;
		}

	      // Use elemental align modifiers here
              if(casterBonus) bonus += .15 * bonus;
	      if(victimBonus) bonus += .15 * bonus;  // its plus since this not an off spell

	      addmove = (((bonus)*addmove)/100);
              if (IS_PLAYER(ch))
                 addmove = ((race_info[ch->pcdata->race].heal_percent + align_info[ch->pcdata->alignment].heal_percent)*addmove)/100;
	      if (IS_SET(victim->in_room->room_flags, ROOM_LOWMAGIC))
		addmove/=2;
	      if (IS_SET(victim->in_room->room_flags, ROOM_HIGHMAGIC))
		addmove= (3*addmove)/2;
	      victim->move += addmove;
	      if (victim->move > victim->max_move)
		victim->move = victim->max_move;
	      if (victim->move < 1)
		victim->move = 1;
	    }
	  if (IS_SET (spell->spell_bits, SPELL_HEALS_DAM))
	    {
	      int addhit = translate (spell->damage, level, ch);
	      int lvl = spell->spell_level;
	      int bonus = ((100 * get_curr_wis(ch))/((10 + lvl*2/7)));
	      affect_strip(ch, gsn_chameleon);
	      if (bonus > 120) bonus = 120;
	      if (bonus < 90) bonus /= 3;
	      if (bonus > 89)
		{
		  if (is_member(ch, GUILD_HEALER)) bonus += lvl;
		  if (is_member(ch, GUILD_MYSTIC)) bonus += lvl/2;
		  if (IS_AUGMENTED(ch, AUG_MIND_FOCUS)) bonus += lvl/3+3;
		  bonus += (ch->pcdata->remort_times) * 8;
		}

	      // Use elemental align modifiers here
              if(casterBonus) bonus += .15 * bonus;
	      if(victimBonus) bonus += .15 * bonus;  // its plus since this not an off spell

	      addhit = (((bonus)*addhit)/100);
              if (IS_PLAYER(ch))
                 addhit = ((race_info[ch->pcdata->race].heal_percent + align_info[ch->pcdata->alignment].heal_percent)*addhit)/100;
	      victim->hit += addhit;
	      if (IS_SET(victim->in_room->room_flags, ROOM_LOWMAGIC))
		addhit/=2;
	      if (IS_SET(victim->in_room->room_flags, ROOM_HIGHMAGIC))
		addhit= (3*addhit)/2;
	      if (victim->hit > victim->max_hit)
		victim->hit = victim->max_hit;
	      if (victim->hit < 1)
		victim ->hit = 1;
	    }
	}

      /* ------------------------------------------------------------- */
      /*               Set Mana Shield Strength structure edited */
      /* ------------------------------------------------------------- */
      if(spell->bitvector2 == AFF_MANASHIELD)
      {
	int shieldStr = ch->pcdata->level;
        if (is_member(ch, GUILD_WIZARD)) shieldStr += 40;
        if (is_member(ch, GUILD_CONJURER)) shieldStr += 15;
	if (is_member(ch, GUILD_HEALER)) shieldStr += 15;
	if (is_member(ch, GUILD_MYSTIC)) shieldStr += 15;
        if (is_member(ch, GUILD_NECROMANCER)) shieldStr += 40;
	if (IS_AUGMENTED(ch, AUG_MIND_FOCUS)) shieldStr += 5;
	if (IS_AUGMENTED(ch, AUG_FOCUS)) shieldStr += 5;
	if (IS_AUGMENTED(ch, AUG_MIND_POWER)) shieldStr += 5;
	shieldStr += (ch->pcdata->remort_times) * 2;

        // Use elemental align modifiers here
        if(casterBonus) shieldStr += .15 * shieldStr;

	/* Cap it! */
	if(shieldStr > 100)
          shieldStr = 150;

        ch->pcdata->mana_shield_strength = shieldStr;
      }

      write_stuff (spell, ch, victim);
      if (str_cmp (spell->duration, "N/A") && translate (spell->duration, level, ch) > 0)
	{
	  bool failed = FALSE;
	  dur = translate (spell->duration, level, ch);
	  if (spell->spell_type == TAR_CHAR_OFFENSIVE)
	    {
	      if (victim && ch != victim)
		if (victim->in_room != ch->in_room && (number_range(1,5) != 3 || IS_MOB(victim)))
		  {
		    act ("$N resisted the effects of your spell!", ch, NULL, victim, TO_CHAR);
		    act ("You resisted the effects of $n's spell!", ch, NULL, victim, TO_VICT);
		    failed = TRUE;
		  }
		else if (saves_spell(level, victim) &&
			 (IS_MOB(ch) || str_cmp(race_info[ch->pcdata->race].name, "flayer") || number_range(1,5) == 2))
		  {
		    act ("$N resisted the effects of your spell!", ch, NULL, victim, TO_CHAR);
		    act ("You resisted the effects of $n's spell!", ch, NULL, victim, TO_VICT);
		    if (FIGHTING (victim) == NULL && ch->in_room == victim->in_room)
		      set_fighting (victim, ch);
		    failed = TRUE;
		  }

	      if (IS_AUGMENTED(victim, AUG_MENTAL_ARMOR) || IS_AFFECTED_EXTRA(victim, AFF_MINDSHIELD))
		dur = UMAX(dur/2, 1);

	    }
	  if (is_member(ch, GUILD_CONJURER)) dur += dur/3;
	  if (is_member(ch, GUILD_NECROMANCER)) dur += dur/3;
 	  if (is_member(ch, GUILD_MYSTIC)) dur +=dur/3;
	  if (is_member(ch, GUILD_RANGER)) dur += dur/3; 
	  if ((ch->in_room->vnum > 999 || spell->spell_type == TAR_CHAR_OFFENSIVE) && !failed )
	    {
	      int jj;
              bool bigaffect = FALSE;
	      for (jj = 0; jj < 5; jj++)
                {
                  if (translate (spell->modifier[jj], level, ch) != 0 &&
                      spell->location[jj] > 0)
                    bigaffect = TRUE;
                }
	      for (jj = 0; jj < 5; jj++)

		{
                  if (bigaffect && (translate(spell->modifier[jj], level,ch) == 0 || spell->location[jj] <= 0))
                     continue;
                  if (!bigaffect && jj > 0) break;
                  {
		  AFFECT_DATA paf;
		  int durr = dur;
		  int modif = translate (spell->modifier[jj], level, ch);
		  bzero (&paf, sizeof (paf));

		  if (is_member(ch, GUILD_CONJURER))
		    {
		      modif = (4*modif)/3;
		    }
		  paf.type = spell->gsn;
		  paf.duration = UMAX(0, (durr-1));
		  paf.location = spell->location[jj];
		  paf.modifier = modif;
		  paf.bitvector = spell->bitvector;
		  paf.bitvector2 = spell->bitvector2;
		  renew_affect ((IS_AFFECTED_EXTRA (victim,
                  AFF_SPELL_REFLECT) && spell->spell_type == TAR_CHAR_OFFENSIVE) ? ch : victim, &paf);
		  if (paf.bitvector == AFF_SLEEP)
		    {
		      do_sleep (victim, "");
		      NEW_POSITION(victim, POSITION_SLEEPING);
		    }
		}
	    }
	}
        }
      if (spell->spell_type == TAR_CHAR_OFFENSIVE && spell->damage != NULL)
	{
	  int dam_left = 100;
	  int lvl = spell->spell_level;
	  int dam = translate (spell->damage, level, ch);
	  int mint = 17 + lvl/6;
	  int bonus = ((100 * get_curr_int(ch))/(mint));
	  check_same_side_pk(ch, victim);
/* cOMMENTED OUT BECAUSE IT BASHES PEOPLE, KINDA BUGGY
	  if (IS_AUGMENTED(ch, AUG_MIND_POWER)
	      && is_member(ch,GUILD_WIZARD)
	      && is_member(ch, GUILD_CONJURER)
	      && number_range(1, UMIN(spell->spell_level, 60)) > 50
	      && number_range(1,UMIN(LEVEL(ch),60)) > 50)
	    {
	      check_fgt(victim);
	      if (victim->fgt->ears < 9)
		victim->fgt->ears = 10+number_range(1,2);
	      else
		victim->fgt->ears += number_range(1,2);
	      victim->position = POSITION_BASHED;
	    }
*/

	  if (bonus > 120) bonus = 120;
	  if (bonus < 90) bonus /= 4;
	  if (bonus > 89)
	    {
	      if (IS_AUGMENTED(ch, AUG_MIND_POWER)) bonus *= (4/3);
	    }
	  bonus += (ch->pcdata->remort_times) * 6;
// THIS REMORT BONUS USED TO BE 8, LOWERED IT TO CALM MAGES DOWN
	  if (bonus > 150) bonus = 150;

	  if(!str_cmp(ch->pcdata->name,"Liberation") && !str_cmp(spell->spell_name,"Cantrip"))
            bonus = 5000;
	  dam = ((bonus) * dam)/100;
          if (IS_PLAYER(ch))
	    {
	      dam = ((race_info[ch->pcdata->race].attack_spell_percent + align_info[ch->pcdata->alignment].attack_spell_percent)*dam)/100;
/* THIS SERVES NO PURPOSE
	      if (IS_PLAYER(victim) && DIFF_ALIGN(ch, victim))
		dam = dam*12/10;
*/
	    }
	  if(spell->spell_bits2 > 0 && victim && victim->in_room)
	    {
	      if (IS_SET (spell->spell_bits2,SPELL_FIRE))
		{
		  if(IS_AFFECTED_EXTRA(victim,AFF_PROT_FIRE))
		    dam = dam *4/5;
		  else
		    spell_fire_breath (level, ch, (void *) victim);
		  if (victim->in_room->sector_type == SECT_DESERT || victim->in_room->sector_type == SECT_WASTELAND || victim->in_room->sector_type == SECT_LAVA)
		    dam = dam*5/4;
		}
	      if (IS_SET (spell->spell_bits2,SPELL_ACID))
		{
		  if(IS_AFFECTED_EXTRA(victim,AFF_PROT_ACID))
		    dam = dam*4/5;
		  else
		    spell_acid_breath (level, ch, (void *) victim);
		  if (victim->in_room->sector_type == SECT_WATER_SWIM || victim->in_room->sector_type == SECT_WATER_NOSWIM || victim->in_room->sector_type == SECT_UNDERWATER)
		    dam = dam*5/4;
		}
	      if (IS_SET (spell->spell_bits2,SPELL_ICE))
		{
		  if(IS_AFFECTED_EXTRA(victim,AFF_PROT_ICE))
		    dam = dam * 4/5;
		  else
		    spell_ice_breath (level, ch, (void *) victim);
		  if (victim->in_room->sector_type == SECT_ICE || victim->in_room->sector_type == SECT_ARCTIC || victim->in_room->sector_type == SECT_SNOW)
		    dam = dam*5/4;
		}
	      if (IS_SET (spell->spell_bits2,SPELL_WIND))
		{
		  if(IS_AFFECTED_EXTRA(victim,AFF_PROT_WIND))
		    dam = dam*4/5;
		  else
		    spell_wind_breath (level, ch, (void *) victim);
		  if (victim->in_room->sector_type == SECT_AIR || victim->in_room->sector_type == SECT_CLOUDS)
		    dam = dam*5/4;
		}
	      if (IS_SET (spell->spell_bits2,SPELL_SHARD))
		{
		  if(IS_AFFECTED_EXTRA(victim,AFF_PROT_SHARD))
		    dam = dam*4/5;
		  else
		    spell_shard_breath (level, ch, (void *) victim);
		  if (victim->in_room->sector_type == SECT_MOUNTAIN || victim->in_room->sector_type == SECT_HILLS || victim->in_room->sector_type == SECT_ROCKY || victim->in_room->sector_type == SECT_CANYON)
		    dam = dam*5/4;
		}
	      if (IS_SET (spell->spell_bits2,SPELL_SHOCK))
		{
		  if(IS_AFFECTED_EXTRA(victim,AFF_PROT_SHOCK))
		    dam = dam*4/5;
		  else
		    spell_shock_breath (level, ch, (void *) victim);
		  if (weather_info.sky == SKY_LIGHTNING && victim->in_room->sector_type != SECT_INSIDE && victim->in_room->sector_type < SECT_CAVE && !IS_SET(victim->in_room->room_flags, ROOM_INDOORS) && !IS_SET(victim->in_room->room_flags, ROOM_UNDERGROUND))
		    {
		      dam*=2;
		    }
		}
	    }
	  if (!ch || !victim)
	    return;
	  if (IS_SET(victim->in_room->room_flags, ROOM_LOWMAGIC) && IS_PLAYER(ch))
	    dam/=2;
	  if (IS_SET(victim->in_room->room_flags, ROOM_HIGHMAGIC))
	    dam= (3*dam)/2;
	  if (victim->in_room->sector_type == SECT_ASTRAL)
	    dam*=2;
	  if (IS_SET (spell->spell_bits, SPELL_HURT_UNDEAD) && (IS_PLAYER (victim) || (IS_MOB (victim) &&						       victim->pIndexData->mobtype != MOB_GHOST && victim->pIndexData->mobtype != MOB_UNDEAD)))
	    {
	      send_to_char ("This spell can only do harm to undead creatures.\n\r", ch);
	      return;
	    }
	  /*	  if (IS_SET(spell->spell_bits, SPELL_HURT_OPP_ALIGN), !DIFF_ALIGN(ch, victim))
	    dam = 0;*/

	  if (IS_AFFECTED_EXTRA(victim, AFF_SPELL_REFLECT))
	    {
	      affect_strip(victim, gsn_spell_reflect);
	      victim = ch;
	    }
	  if (IS_SET (spell->spell_bits, SPELL_HALVED_SAVE) && saves_spell (level, victim))
	    dam_left -= number_range(30,60);
	  if (IS_AFFECTED_EXTRA (victim, AFF_MINDSHIELD))
	    dam_left -= number_range(0,30);
	  if (IS_AUGMENTED(victim, AUG_MENTAL_ARMOR))
	    dam_left -= number_range(0,20);
	  if (dam_left < 30) dam_left = 30;
	  dam = ((dam_left * dam)/100);
	  if (victim->in_room != ch->in_room)
	    {
	      dam = dam*3/6;
	      if (IS_MOB(victim) && IS_SET(victim->act, ACT_SENTINEL))
		dam = 0;
	      WAIT_STATE(ch, PULSE_VIOLENCE);
	    }

          // Use elemental align modifiers here
          if(casterBonus) {
		//if(!IS_MOB(ch)) send_to_char ("U got the bonus.\n\r",ch);	
		dam += (.15 * dam);
          }
          if(victimBonus) {
		//if(!IS_MOB(victim)) send_to_char ("U got the bonus.\n\r",victim);
		dam -= (.15 * dam);
	  }
          if(alignshardBonus) {
                dam -= (.15 * dam);
          }

	  damage (ch, victim, dam, spell->gsn);
	  show_hitdam (spell->gsn, spell->noun_damage, dam, ch, victim);
	  if (IS_SET (spell->spell_bits2, SPELL_VAMPIRIC))
	    {
	      ch->hit += dam/2;
	      if (ch->hit > ch->max_hit)
		    ch->hit = ch->max_hit;
	      act("You drain the life from $N, and it infuses your body.", ch, NULL, victim, TO_CHAR);
	      act("$n drain the life from you.",ch, NULL, victim, TO_VICT);
	      act("$n drains the life from $N.", ch, NULL, victim, TO_NOTVICT);
	    }

	}

    }
  if (spell && spell->linked_to != NULL)
    {
      SPELL_DATA *sp;
      if ((sp = skill_lookup (spell->linked_to, -1)) != NULL)
	{
	  general_spell (sp, level, ch, vo);
	}
    }
  if (IS_PLAYER (ch))
       {
    skill_gain (ch, spell->gsn, TRUE);
  if (spell->spell_lag > 0 && IS_PLAYER(ch) && ch->pcdata->no_quit < spell->spell_lag)
    ch->pcdata->no_quit = spell->spell_lag;
  if (ch->pcdata->no_quit_pk < spell->spell_lag)
    ch->pcdata->no_quit_pk = spell->spell_lag;
      }
  return;
}


void
spell_message (int sn, int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *rec;
  char buf[STD_LENGTH];
  char targname[SML_LENGTH];
  if (ch->pcdata->no_quit > 0)/* JRAJRA spell lag */
    {
      send_to_char("You cannot cast message yet!\n\r", ch);
      return;
    }
  else
    {
  if (!target_name || target_name[0] == '\0')
    {
      send_to_char ("A voice booms from the heavens:\n\r", ch);
      send_to_char ("\x1B[1;37mSyntax\x1B[0m: cast 'message' <\x1B[1;37mtarget\x1B[0m> <\x1B[1;37mmsg\x1B[0m>\n\r", ch);
      return;
    }
  target_name = one_argy (target_name, targname);
  if ((rec = get_char_world (ch, targname)) == NULL)
    {
      send_to_char ("You send a voice, but you get the feeling it could not deliver.\n\r", ch);
      return;
    }
  sprintf (buf, "\x1B[1;32mA voice from afar tells you '\x1B[0m%s\x1B[1;32m'\x1B[0m\n\r",
	   target_name);
  send_to_char (buf, rec);
  sprintf (buf, "You send a mystical voice to talk to %s.", NAME (rec));
  send_to_char (buf, ch);

  return;
    }
}

//ARC: The Life Tap Spell.
void
spell_life_tap (int sn, int level, CHAR_DATA * ch, void *vo)
{
	CHAR_DATA *vampire = ch;
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	int life = 0;

	life = vampire->pcdata->level + 30 + number_range(1, vampire->pcdata->level);

	victim->hit -= life;
	
	vampire->hit += life;	
	if (vampire->max_hit < vampire->hit)
		vampire->hit = vampire->max_hit;
	


	return;

}


// The Raise Undead Spell.
void spell_raise_undead (SPELL_DATA * spell, int level, CHAR_DATA * ch, void *vo) {

	CHAR_DATA *undead_creature;
	MOB_PROTOTYPE *the_corpse;
	MOB_PROTOTYPE *the_mobile;
	AFFECT_DATA af;

	char buf[500];

	int sn = spell->gsn;

	// Grab the target corpse object
	SINGLE_OBJECT *target_corpse = (SINGLE_OBJECT *) vo;

	// Return if target corpse is a PC
	if ((target_corpse->pIndexData->item_type != ITEM_CORPSE_NPC))  {
	  send_to_char ("You just can't seem to raise that from the dead!\n\r", ch);
	  return;
	}

	// Get the mobile number from the spell data  
	the_mobile = get_mob_index (spell->creates_obj);

	if (the_mobile == NULL || the_mobile->vnum == 1) { // Nothing there!
	  // Get the corpse mobs vnum number and create it
	  the_mobile = get_mob_index (((I_CONTAINER *)(target_corpse->more))->key_vnum);
	}

	if (the_mobile == NULL || the_mobile->vnum == 1) {
	  send_to_char ("You just can't seem to raise that from the dead!\n\r", ch);
	  return;
	}

	undead_creature = create_mobile (the_mobile);

	char_to_room (undead_creature, ch->in_room);

	act ("$n has raised $N from the dead\x1b[0;37m!", ch, NULL, undead_creature, TO_ROOM);
	act ("You raise $N from the dead\x1b[0;37m!", ch, NULL, undead_creature, TO_CHAR);
	free_it (target_corpse);

	if (!(ch->number_of_mob_followers)) ch->number_of_mob_followers = 0;
	else if (ch->number_of_mob_followers > 1) {
	  send_to_char ("But it falls *THUD* to the floor and decomposes...too many followers perhaps?\n\r",ch);
	  act ("But it falls *THUD* to the floor and decomposes...", ch, NULL, NULL, TO_ROOM);
	  free_it (undead_creature);
	  return;
	}
  
	bzero (&af, sizeof (af));
  
	if (level < (LEVEL (undead_creature)-60) || IS_AFFECTED (undead_creature, AFF_NOCHARM)) {
	  act ("$N has been raised but snarls at you in complete hatred and attacks!", ch, NULL, undead_creature, TO_CHAR);
	  act ("$N snarls at $n in complete hatred and attacks!", ch, NULL, undead_creature, TO_ROOM);
  	  check_fgt(undead_creature);
	  undead_creature->is_undead = TRUE;
	  undead_creature->fgt->fighting = ch;
	  NEW_POSITION(undead_creature, POSITION_FIGHTING);
	  multi_hit (undead_creature, ch, TYPE_UNDEFINED);
	  return;
	}
	// Fix undead stats so it can group, assist, etc
	undead_creature->pcdata->alignment = ch->pcdata->alignment;
	undead_creature->pIndexData->alignment = ch->pcdata->alignment;
//	sprintf(buf,"Setting undead creature alignement to %d\n\r",undead_creature->pcdata->alignment);
//	send_to_char (buf,ch);
	undead_creature->pcdata->act2 = PLR_AUTOGOLD | PLR_AUTOSPLIT | PLR_ASSIST;

	add_follower (undead_creature, ch);
	do_group(ch, undead_creature->pcdata->name);
	(ch->number_of_mob_followers)++;
	af.type = sn;
	af.duration = -1;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = AFF_CHARM;
	affect_to_char (undead_creature, &af);
	undead_creature->is_undead = TRUE;
  
	act ("$N gazes at you as if $S life revolved around you.", ch, NULL, undead_creature, TO_CHAR);
	return;
}

void
spell_scry (int sn, int level, CHAR_DATA * ch, void *vo)
{
  char buf[STD_LENGTH];
  if (target_name == '\0')
    {
      return;
    }
  if (ch->pcdata->online_spot->located == TRUE && ch->pcdata->online_spot->pk_since_locate < 3)
    {
      send_to_char("You must pkill before using this spell again.\n\r", ch);
      return;
    }
  if (!scry (ch, level, target_name))
    {
      sprintf (buf, "You were unable to scry %s.\n\r", target_name);
      send_to_char (buf, ch);
    }
  else
    {
      ch->pcdata->online_spot->located = TRUE;
      ch->pcdata->online_spot->pk_since_locate = 0;
    }
  return;
}
void

spell_locate_person (int sn, int level, CHAR_DATA * ch, void *vo)
{
  char buf[STD_LENGTH];
  if (target_name == '\0')
    {
      return;
    }
  if (!locate_person (ch, level, target_name))
    {
      sprintf (buf, "You were unable to locate %s.\n\r", target_name);
      send_to_char (buf, ch);
    }
  else
    {
      ch->pcdata->online_spot->located = TRUE;
      ch->pcdata->online_spot->pk_since_locate = 0;
    }
  return;
}

void
spell_identify (int sn, int level, CHAR_DATA * ch, void *vo)
{
  SINGLE_OBJECT *obj = (SINGLE_OBJECT *) vo;
  SPELL_DATA *spp;
  char buf3[500];
  char buf2[STD_LENGTH];
  char buf[STD_LENGTH];
  AFFECT_DATA *paf;

  send_to_char (
		 "\x1B[0m-----------------------------------------------------------------------------\n\r",
		 ch);
  sprintf (buf, "'%s\x1B[0m' is some sort of \x1B[1;34m%s\x1B[0m.\n\r",
	   STRR (obj, short_descr),
	   item_type_name (obj->pIndexData->item_type));
  send_to_char (buf, ch);
  sprintf (buf, "It weighs \x1B[1;37m%d\x1B[0;37m Stones and \x1b[1;37m%d\x1b[0;37m Pebbles , and is apparently worth \x1B[1;37m%d\x1B[0;33m copper\x1B[0m.\n\r",
	   (obj->pIndexData->weight/WGT_MULT),(obj->pIndexData->weight % WGT_MULT), obj->cost);
  send_to_char (buf, ch);
  sprintf (buf, "It is affected by [\x1B[1;34m%s\x1B[0;34m]\x1B[0m.\n\r",
	   extra_bit_name (obj->extra_flags));
  send_to_char (buf, ch);
  sprintf (buf,
	   "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n\r");
  switch (obj->pIndexData->item_type)
    {
    case ITEM_SCROLL:
    case ITEM_POTION:
      {
	I_POTION *pot = (I_POTION *) obj->more;
	send_to_char (buf, ch);
	sprintf (buf, "Level \x1B[1;34m%d\x1B[0m spells of:",
pot->spell_level);
	send_to_char (buf, ch);
	if (pot->spells[0] >= 0 && pot->spells[0] < SKILL_COUNT)
	  {
	    send_to_char (" '\x1B[1;37m", ch);
	    if ((spp = skill_lookup (NULL, pot->spells[0])) != NULL)
	      send_to_char (spp->spell_name, ch);
	    send_to_char ("\x1B[0m'", ch);
	  }
	if (pot->spells[1] >= 0 && pot->spells[1] < SKILL_COUNT)
	  {
	    send_to_char (" '\x1B[1;37m", ch);
	    if ((spp = skill_lookup (NULL, pot->spells[1])) != NULL)
	      send_to_char (spp->spell_name, ch);
	    send_to_char ("\x1B[0m'", ch);
	  }
	if (pot->spells[2] >= 0 && pot->spells[2] < SKILL_COUNT)
	  {
	    send_to_char (" '\x1B[1;37m", ch);
	    if ((spp = skill_lookup (NULL, pot->spells[2])) != NULL)
	      send_to_char (spp->spell_name, ch);
	    send_to_char ("\x1B[0m'", ch);
	  }
	send_to_char (".\n\r", ch);
      }
    break;
    case ITEM_WAND:
    case ITEM_STAFF:
      {
	I_WAND *wnd = (I_WAND *) obj->more;
	send_to_char (buf, ch);
	sprintf (buf, "Has \x1B[34m%d\x1B[0m of \x1B[1;34m%d\x1B[0m charges of level \x1B[1;37m%d\x1B[0m",
		 wnd->current_charges, wnd->max_charges, wnd->spell_level);
	send_to_char (buf, ch);
	if (wnd->spells[0] >= 0 && wnd->spells[0] < SKILL_COUNT)
	  {
	    send_to_char (" '\x1B[1;37m", ch);
	    if ((spp = skill_lookup (NULL, wnd->spells[0])) != NULL)
	      send_to_char (spp->spell_name, ch);
	    send_to_char ("\x1B[0m'", ch);
	  }
	send_to_char (".\n\r", ch);
	break;
      }
    case ITEM_GEM:
      {
	I_GEM *gem = (I_GEM *) obj->more;
	sprintf (buf2, "Mana Type(s): ");
	if (IS_SET (gem->gem_type, MANA_FIRE))
	  strcat (buf2, "\x1B[1;31mFIRE ");
	if (IS_SET (gem->gem_type, MANA_WATER))
	  strcat (buf2, "\x1B[1;34mWATER ");
	if (IS_SET (gem->gem_type, MANA_EARTH))
	  strcat (buf2, "\x1B[0;33mEARTH ");
	if (IS_SET (gem->gem_type, MANA_AIR))
	  strcat (buf2, "\x1B[1;36mAIR ");
	if (IS_SET (gem->gem_type, MANA_SPIRIT))
	  strcat (buf2, "\x1B[1;34mSPIRIT ");
	strcat (buf2, "\x1B[37;0m");
	sprintf (buf3, "\n\rMaximum mana: \x1B[1;37m%d\x1B[0m.", gem->max_mana);
	strcat (buf2, buf3);
	send_to_char (buf2, ch);
	send_to_char ("\n\rGems may not give you their max mana if you have low intelligence or wisdom.\n\r", ch);
	sprintf (buf2, "Max spell level for this gem: %d\n\r", gem->max_level);
	send_to_char (buf2, ch);
	break;
      }
    case ITEM_WEAPON:
      {
	I_WEAPON *weap = (I_WEAPON *) obj->more;
	send_to_char (buf, ch);
	sprintf (buf, "This weapon is capable of \x1B[1;34m%d\x1B[0md\x1B[1;34m%d\x1B[0m damage.\n\r",
		 weap->firstdice, weap->seconddice);
	send_to_char (buf, ch);
	if (weap->ammo_type > 0 && IS_OBJ_STAT(obj, ITEM_RANGED))
	  {
	    sprintf(buf, "This ranged weapon takes ammunition of type %d.\n\r", weap->ammo_type);
	    send_to_char(buf, ch);
	  }
	break;
      }
    case ITEM_AMMO:
      {
	I_AMMO *ammo = (I_AMMO *) obj->more;
	send_to_char (buf, ch);
	sprintf (buf, "This ammunition is capable of \x1B[1;34m%d\x1B[0md\x1B[1;34m%d\x1B[0m damage.\n\r",
		 ammo->firstdice, ammo->seconddice);
	send_to_char(buf, ch);
	sprintf(buf, "This ammo is of type %d.\n\r", ammo->ammo_type);
	send_to_char(buf, ch);
	break;
      }


    case ITEM_ARMOR:
      {
	I_ARMOR *arm = (I_ARMOR *) obj->more;
	send_to_char (buf, ch);
	sprintf (buf, "Leg AC: %d  Body AC: %d  Head AC: %d  Arm AC: %d.\n\r", arm->protects_legs,
		 arm->protects_body, arm->protects_head, arm->protects_arms);
	send_to_char (buf, ch);
	break;
      }
    }
  for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location != APPLY_NONE && paf->modifier != 0)
	{
	  if (paf->modifier > 0)
	    sprintf (buf, "It is +\x1B[1;36m%d\x1B[0m to \x1B[36m%s\x1B[0m.\n\r",
		     paf->modifier, affect_loc_name (paf->location));
	  else
	    sprintf (buf, "It is -\x1B[1;36m%d\x1B[0m to \x1B[36m%s\x1B[0m.\n\r",
		     paf->modifier * -1, affect_loc_name (paf->location));
	  send_to_char (buf, ch);
	}
    }
  for (paf = obj->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location != APPLY_NONE && paf->modifier != 0)
	{
	  if (paf->modifier > 0)
	    sprintf (buf, "It is +\x1B[1;36m%d\x1B[0m to \x1B[36m%s\x1B[0m.\n\r",
		     paf->modifier, affect_loc_name (paf->location));
	  else
	    sprintf (buf, "It is -\x1B[1;36m%d\x1B[0m to \x1B[36m%s\x1B[0m.\n\r",
		     paf->modifier * -1, affect_loc_name (paf->location));
	  send_to_char (buf, ch);
	}
    }

  send_to_char (
		 "-----------------------------------------------------------------------------\n\r", ch);
  return;
}


void
spell_locate_object (int sn, int level, CHAR_DATA * ch, void *vo)
{
  char buf[SML_LENGTH];
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *in_obj;
  bool found;
  int number = 0;
  found = FALSE;
  if (level > 100 || LEVEL (ch) > 100)
    return;
  for (obj = object_list; obj != NULL && number <= LEVEL (ch);
       obj = obj->next)
    {
      if (!can_see_obj (ch, obj) || !is_name (target_name, obj->pIndexData->name))
	continue;
      found = TRUE;
      number++;
      for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj)
	;
      if (in_obj->carried_by != NULL)
	{
	  sprintf (buf, "%s carried by %s.\n\r",
		   STRR (obj, short_descr), PERS (in_obj->carried_by, ch));
	}
      else
	{
	  sprintf (buf, "%s in %s.\n\r",
		   STRR (obj, short_descr), in_obj->in_room == NULL
		   ? "somewhere" : in_obj->in_room->name);
	}
      buf[0] = UPPER (buf[0]);
      send_to_char (buf, ch);
    }
  if (!found)
    send_to_char ("Nothing like that in hell, heaven or earth.\n\r", ch);
  return;
}

void
spell_acid_breath (int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  SINGLE_OBJECT *obj_lose;
  SINGLE_OBJECT *obj_next;
  I_ARMOR *arm;
  I_WEAPON *wep;
  if (ch->in_room->vnum < 1000  || number_percent () > level)
    return;
  for (obj_lose = victim->carrying; obj_lose != NULL; obj_lose = obj_next)
    {
      obj_next = obj_lose->next_content;
      if (number_range (1, 4) != 3)
	continue;
      switch (obj_lose->pIndexData->item_type)
	{
	case ITEM_ARMOR:
	  {
	    arm = (I_ARMOR *)obj_lose->more;
	    if (arm->condition_now > 0)
	      {
		act ("$p is covered with acid!", victim, obj_lose, NULL, TO_CHAR);
		arm->condition_now-= 4;
		if (arm->condition_now <= 0)
		  {
		    obj_from(obj_lose);
		    free_it(obj_lose);
		    break;
		  }
		if(number_range(1,50) == 2)
		  {
		    act ("$p is splashed with acid and loses some of its resilience!", victim, obj_lose, NULL, TO_CHAR);
		    arm->max_condition-= 3;
		    if (arm->max_condition < 1)
		      arm->max_condition = 1;
		    if (arm->protects_legs > 1)
		      arm->protects_legs -= number_bits(1);
		    if (arm->protects_body > 1)
		      arm->protects_body -= number_bits(1);
		    if (arm->protects_head > 1)
		      arm->protects_head -= number_bits(1);
		    if (arm->protects_arms > 1)
		      arm->protects_arms -= number_bits(1);
		  }
	      }
	  }
	break;
	case ITEM_WEAPON:
	  wep = (I_WEAPON *)obj_lose->more;
	  if (wep->damage_p > 0)
	    {
	      act("$p is splashed with acid!", ch, obj_lose, NULL, TO_CHAR);
	      wep->damage_p-=5;
	      if (wep->damage_p <= 0)
		{
		  obj_from(obj_lose);
		  free_it(obj_lose);
		  break;
		}
	      if(number_range(1,50) == 3)
		{
		  if (wep->firstdice > 1)
		    wep->firstdice -= number_bits(1);
		  if (wep->seconddice > 1)
		    wep->seconddice -= number_bits(1);
		  act("The acid damages $p permanently!", victim, obj_lose, NULL, TO_CHAR);
		  if (wep->strength > 5)
		      wep->strength -= 3;
		}
	    }
	  break;
	case ITEM_CONTAINER:
	  act ("$p fumes and dissolves!",victim, obj_lose, NULL, TO_CHAR);
	  obj_from (obj_lose);
	  free_it (obj_lose);
	  break;
	}
    }
return;
}

void
spell_fire_breath (int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  SINGLE_OBJECT *obj_lose;
  SINGLE_OBJECT *obj_next;
  if (ch->in_room->vnum < 1000 || IS_AFFECTED_EXTRA (victim, AFF_PROT_FIRE) || number_percent () < level)
    return;
  for (obj_lose = victim->carrying; obj_lose != NULL; obj_lose = obj_next)
    {
      char *msg;
      obj_next = obj_lose->next_content;
      if (number_range (1,4) != 3)
	continue;
      switch (obj_lose->pIndexData->item_type)
	{
	default:
	  continue;
	case ITEM_CONTAINER:
	  {
	    I_CONTAINER * cnt = (I_CONTAINER *) obj_lose->more;
	    if (!IS_SET(cnt->flags, CONT_INDESTRUCTABLE))
	      msg = "$p ignites and burns!";
	    else
	      continue;
	  }
	break;
	case ITEM_POTION:
	  msg = "$p bubbles and boils!";
	  break;
	case ITEM_SCROLL:
	  msg = "$p crackles and burns!";
	  break;
	case ITEM_STAFF:
	  msg = "$p smokes and chars!";
	  break;
	case ITEM_WAND:
	  msg = "$p sparks and sputters!";
	  break;
        case ITEM_AMMO:
          msg =  "$p blackens and burns!";
          break;
 	case ITEM_FOOD:
	  msg = "$p blackens and crisps!";
	  break;
	case ITEM_PILL:
	  msg = "$p melts and drips!";
	  break;
	}
      act (msg, victim, obj_lose, NULL, TO_CHAR);
      obj_from (obj_lose);
      free_it (obj_lose);
    }
  return;
}

void
spell_ice_breath (int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  SINGLE_OBJECT *obj_lose;
  SINGLE_OBJECT *obj_next;
  I_ARMOR *arm;
  I_WEAPON *wep;
  if (ch->in_room->vnum < 1000 || IS_AFFECTED_EXTRA (victim, AFF_PROT_ICE) || number_percent () < level)
    return;
  for (obj_lose = victim->carrying; obj_lose != NULL;
       obj_lose = obj_next)
    {
      obj_next = obj_lose->next_content;
      if (number_range (1, 5) != 3)
	continue;
      switch (obj_lose->pIndexData->item_type)
	{
	default:
	  continue;
	case ITEM_POTION:
	  {
	    act ("$p turns to ice and explodes!", victim, obj_lose, NULL, TO_CHAR);
	    obj_from (obj_lose);
	    free_it (obj_lose);
	  }
	break;
	case ITEM_DRINK_CON:
	  {
	    act ("$p turns to ice and explodes!", victim, obj_lose, NULL, TO_CHAR);
	    obj_from (obj_lose);
	    free_it (obj_lose);
	  }
	break;
	case ITEM_ARMOR:
	  arm = (I_ARMOR *)obj_lose->more;
	  if (arm->condition_now > 0)
	    {
	      act ("$p freezes and cracks!", victim, obj_lose, NULL, TO_CHAR);
	      arm->condition_now-= 4;
	      if (arm->condition_now <= 0)
		{
		  obj_from(obj_lose);
		  free_it(obj_lose);
		  break;
		}
	      if(number_range(1,50) == 2)
		{
		  act ("$p is frozen and permanently damaged!", victim, obj_lose, NULL, TO_CHAR);
		  arm->max_condition-= 3;
		  if (arm->max_condition < 2)
		    arm->max_condition = 2;
		  if (arm->protects_legs > 2)
		    arm->protects_legs -= number_bits(1);
		  if (arm->protects_body > 2)
		    arm->protects_body -= number_bits(1);
		  if (arm->protects_head > 2)
		    arm->protects_head -= number_bits(1);
		  if (arm->protects_arms > 2)
		    arm->protects_arms -= number_bits(1);
		}
	    }
	  break;
	case ITEM_WEAPON:
	  wep = (I_WEAPON *)obj_lose->more;
	  if (wep->damage_p > 0)
	    {
	      act("$p is frozen!", ch, obj_lose, NULL, TO_CHAR);
	      wep->damage_p-=5;
	      if (wep->damage_p <= 0)
		{
		  obj_from(obj_lose);
		  free_it(obj_lose);
		  break;
		}
	      if(number_range(1,50) == 3)
		{
		  if (wep->firstdice > 1)
		    wep->firstdice -= number_bits(1);
		  if (wep->seconddice > 1)
		    wep->seconddice -= number_bits(1);
		  act("The ice damages $p permanently!", victim, obj_lose, NULL, TO_CHAR);
		  if (wep->strength > 5)
		    {
		      wep->strength -= 3;
		    }
		}
	    }
	  break;
	}
    }
  return;
}
void
spell_shard_breath (int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  SINGLE_OBJECT *obj_lose;
  SINGLE_OBJECT *obj_next;
  I_ARMOR *arm;
  I_WEAPON *wep;
   if (ch->in_room->vnum < 1000 || IS_AFFECTED_EXTRA (victim, AFF_PROT_SHARD) || number_percent () < level)
    return;
   for (obj_lose = victim->carrying; obj_lose != NULL;
	obj_lose = obj_next)
     {
       obj_next = obj_lose->next_content;
       if(number_range(1,4) != 3) continue;
       switch (obj_lose->pIndexData->item_type)
	 {
	 default:
	   continue;
	 case ITEM_POTION:
	   {
	     act ("$p is hit with a shard and explodes!", victim,obj_lose, NULL, TO_CHAR);
	     obj_from (obj_lose);
	     free_it (obj_lose);
	   }
	 break;
	 case ITEM_DRINK_CON:
	   {
	     act ("$p gets zinged and rips open!", victim, obj_lose,NULL, TO_CHAR);
	     obj_from (obj_lose);
	     free_it (obj_lose);
	   }
	 break;
	 case ITEM_ARMOR:
	   arm = (I_ARMOR *)obj_lose->more;
	   if (arm->condition_now > 0)
	     {
	       act ("$p gets pelted with shards!", victim, obj_lose, NULL, TO_CHAR);
	       arm->condition_now-= 4;
	       if (arm->condition_now <= 0)
		 {
		   obj_from(obj_lose);
		   free_it(obj_lose);
		   break;
		 }
	       if(number_range(1,50) == 2)
		 {
		   act ("$p absorbs a solid blow from a flying sliver!", victim, obj_lose, NULL, TO_CHAR);
		   arm->max_condition-= 3;
		   if (arm->max_condition < 2)
		     arm->max_condition = 2;
		   if (arm->protects_legs > 1)
		     arm->protects_legs -= number_bits(1);
		   if (arm->protects_body > 1)
		     arm->protects_body -= number_bits(1);
		   if (arm->protects_head > 1)
		     arm->protects_head -= number_bits(1);
		   if (arm->protects_arms > 1)
		     arm->protects_arms -= number_bits(1);
		 }
	     }
	   break;
	 case ITEM_WEAPON:
	   wep = (I_WEAPON *)obj_lose->more;
	   if (wep->damage_p > 0)
	     {
	       act("$p gets zinged!", ch, obj_lose, NULL, TO_CHAR);
	       wep->damage_p-=5;
	       if (wep->damage_p <= 0)
		 {
		   obj_from(obj_lose);
		   free_it(obj_lose);
		   break;
		 }
	       if(number_range(1,50) == 3)
		 {
		   if (wep->firstdice > 1)
		     wep->firstdice -= number_bits(1);
		   if (wep->seconddice > 1)
		     wep->seconddice -= number_bits(1);
		   act("The shards damage $p permanently!", victim, obj_lose, NULL, TO_CHAR);
		   if (wep->strength > 5)
		     {
		       wep->strength -= 3;
		     }
		 }
	     }
	   break;
	 }
     }
   return;
}

void
spell_shock_breath (int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj_next;
  if (ch->in_room->vnum < 1000 || IS_AFFECTED_EXTRA (victim, AFF_PROT_SHOCK) || number_percent () < level)
    return;
  for (obj = victim->carrying; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      if (number_range (1, 4) != 3) continue;
      switch (obj ->pIndexData->item_type)
	{
	default:
	  continue;
	case ITEM_SCROLL:
	case ITEM_PILL:
	case ITEM_POTION:
	  {
	    act ("$p glows with power, then fades!", victim, obj, NULL, TO_CHAR);
	    if ((((I_POTION *) obj->more)->spell_level > 10)  && (number_range(1,50) != 3))
	      ((I_POTION *) obj->more)->spell_level -= number_range(5,10);
	    else
	      ((I_POTION *) obj->more)->spell_level += number_range(5,10);
	  }
	break;
	case ITEM_STAFF:
	case ITEM_WAND:
	  {
	    act ("$p sparks with energy, then falls silent!",  victim, obj, NULL, TO_CHAR);
	    if ((((I_WAND *) obj->more)->current_charges > 0)  && (number_range(1,50) != 3))
	      ((I_WAND *) obj->more)->current_charges--;
	    else
	      ((I_WAND *) obj->more)->current_charges++;
	    if ((((I_WAND *) obj->more)->spell_level > 10)  && (number_range(1,50) != 3))
	      ((I_WAND *) obj->more)->spell_level -= number_range(5,10);
	    else
	      ((I_WAND *) obj->more)->spell_level += number_range(5,10);
	  }
	break;
	case ITEM_GEM:
	  {
	    if(number_range(1,3) == 3)
	      {
		if (number_range(1,20) == 3)
		  {
		    ((I_GEM *) obj->more)->gem_type =number_range(1,31);
		    act ("$p seems to change color before your eyes...",  victim, obj, NULL, TO_CHAR);
		  }
		if ((((I_GEM *) obj->more)->max_mana > 15)  &&(number_range(1,10) == 3))
		  if(number_range(1,20) != 4)
		    {
			  act ("$p flares brightly and then fades...",  victim, obj, NULL, TO_CHAR);
			  ((I_GEM *) obj->more)->max_mana -= number_range(1,15);
		    }
		  else
		    {
		      act ("$p flares brightly and glows with power",  victim, obj, NULL, TO_CHAR);
		      ((I_GEM *) obj->more)->max_mana += number_range(1,15);
		    }
		if ((((I_GEM *) obj->more)->max_level > 10)  &&(number_range(1,10) == 3))
		  if(number_range(1,20) != 4)
		    {
		      act ("$p glows brightly and then fades...",  victim, obj, NULL, TO_CHAR);
		      ((I_GEM *) obj->more)->max_level -= number_range(1,10);
		    }
		  else
		    {
		      act ("$p glows brightly and hums with power",  victim, obj, NULL, TO_CHAR);
			  ((I_GEM *) obj->more)->max_level += number_range(1,10);
		    }
	      }

	  }
	break;
	}
    }
  return;
}



void
spell_wind_breath (int level, CHAR_DATA * ch, void *vo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
   if (((ch->in_room->vnum >= (BATTLEGROUND_START_VNUM + bg_multiplier)) &&
      (ch->in_room->vnum <= (BATTLEGROUND_END_VNUM + bg_multiplier))) ||
      IS_AFFECTED_EXTRA (victim, AFF_PROT_WIND))
  return;
  if (number_percent () < level && !saves_spell (level, victim))
    {
      if ((number_range (1,50) == 5) && IS_AFFECTED(victim,
AFF_INVISIBLE))
	{
	  affect_strip_bits (victim, AFF_INVISIBLE);
	  send_to_char("Everyone can SEE you now!\n\r", victim);
	}
      if ((number_range (1,50) == 5) && IS_AFFECTED(victim, AFF_FLYING))
	{
	  affect_strip_bits (victim, AFF_FLYING);
	  send_to_char("You fall back down to the ground!\n\r", victim);
	}
      if (number_range (1,50) == 5 && IS_AFFECTED(victim, AFF_HIDE))
	{
	  affect_strip_bits (victim, AFF_HIDE);
	  send_to_char("The burst of air uncovers your hiding place!\n\r", victim);
	}
      if ((number_range (1,50) == 5) && IS_AFFECTED(victim, AFF_SNEAK))
	{
	  affect_strip_bits (victim, AFF_SNEAK);
	  send_to_char("The wind makes you stumble and crash!\n\r", victim);
	}
      if ((number_range (1,50) == 5) && IS_AFFECTED(victim, AFF_INFRARED))
	{
	  affect_strip_bits (victim, AFF_INFRARED);
	  send_to_char("The dust swirls in your eyes, ruining your night vision!\n\r", victim);
	}
      if ((number_range (1,50) == 5) && IS_AFFECTED(victim,
AFF_DETECT_INVIS))
	{
	  affect_strip_bits (victim, AFF_DETECT_INVIS);
	  send_to_char("The massive storm obscures your sensitive vision!\n\r", victim);
	}
      if ((number_range (1,50) == 5) && IS_AFFECTED(victim,
AFF_DETECT_HIDDEN))
	{
	  affect_strip_bits (victim, AFF_DETECT_HIDDEN);
	  send_to_char("The wind makes it impossible to detect movements in the woods!\n\r", victim);
	}
    }
  /*  JRAJRAJRA */
  return;
}


TRAP_DATA *
new_trap (CHAR_DATA * ch, ROOM_DATA * room)
{
  TRAP_DATA *trp;
  trp = (TRAP_DATA *) mem_alloc ((sizeof (*trp)));
  trp->material = 1;
  trp->level = LEVEL(ch);
  trp->affects = 0;
  room->more->trap = trp;
  return trp;
}
/*
void
do_trapset (CHAR_DATA *ch, char *argy)
{

TRAP_DATA *trp;
SINGLE_OBJECT *obj;
int material, affects, level, door;
char arg1[SML_LENGTH];
  DEFINE_COMMAND ("trapset", do_trapset, POSITION_STANDING, 0, LOG_NORMAL, "Creates a trap in the room!")
    WAIT_STATE (ch, 35);
  if (!ch || !ch->in_room) return;
  check_room_more(ch->in_room);
  if ((obj = get_item_held(ch, ITEM_TRAP)) == NULL)
    {
      send_to_char("You must be holding a trap in order to set a trap!\n\r", ch);
      return;
    }
  if (ch->position != POSITION_STANDING)
    {
      send_to_char("You must be standing to set a trap!\n\r", ch);
      return;
    }
  argy = one_argy(argy, arg1);
  if ((door = get_direction_number(arg1)) < DIR_MAX)
   {
     EXIT_DATA *pexit;
    if ((pexit = ch->in_room->exit[door]) == NULL || !pexit->to_room ||
         !pexit->d_info)
      {
        send_to_char("What are you doing? There is no exit there.\n\r",ch);
        return;
     }
   if(IS_SET(pexit->d_info->exit_info, EX_HASTRAP))
     {
       send_to_char("It's already trapped!\n\r", ch);
       return;
     }
   if(number_range(1,120) > ch->pcdata->learned[gsn_trapset])
    {
      send_to_char("You failed to set the trap.\n\r", ch);
      obj_from(obj);
      free_it(obj);
      return;
    }
    SET_BIT(pexit->d_info->exit_info, EX_HASTRAP);
    obj_from(obj);
    free_it(obj);
    send_to_char("Ok, the door is now trapped.\n\r", ch);
    return;
   }
  if (ch->in_room->more->trap)
    {
      ch->in_room->more->trap->material++;
      ch->in_room->more->trap->material += 10;
      set_off_trap(ch, ch->in_room, TRUE);
      return;
    }
  material =  ((I_TRAP *) obj->more)->material;
  if (material <= 0) material = 1;
  obj_from(obj);
  free_it (obj);
  level = LEVEL(ch);
  trp = new_trap (ch, ch->in_room);
  trp->level = LEVEL(ch);
  trp->material = material;
  trp->affects = affects;

  if (((!is_member(ch, GUILD_RANGER) || !is_member(ch, GUILD_THIEFG)
       || !is_member(ch, GUILD_TINKER)) && number_range (1,120) >= ch->pcdata->learned[gsn_trapset]))
    {
      trp->material +=1;
      trp->level +=10;
      set_off_trap (ch,ch->in_room, TRUE);
    }
  else
    send_to_char("Ok trap set.\n\r", ch);
  return;
}

void
do_detrap (CHAR_DATA *ch, char* argy)
{
  char arg1[SML_LENGTH];
  int door;
  EXIT_DATA *pexit;
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND("detrap", do_detrap, POSITION_STANDING, 0, LOG_NORMAL, "Removes a trap from the room.")

      if (IS_MOB(ch))
      return;
    WAIT_STATE(ch, 5*PULSE_VIOLENCE);
    argy = one_argy(argy, arg1);

  if(arg1[0] == '\0' || arg1 == "")
    {
      send_to_char("Detrap what?\n\r", ch);
      return;
    }
  if ((door = get_direction_number (arg1)) != DIR_MAX)
    {
      if ((pexit = ch->in_room->exit[door]) == NULL)
	{
	  send_to_char("There is no exit in that direction to detrap!\n\r", ch);
	  return;
	}
      if (!pexit->d_info || !IS_SET(pexit->d_info->exit_info, EX_HASTRAP))
	{
	  send_to_char("That exit is not trapped!\n\r", ch);
	  return;
	}
      else
	{
	  if (number_percent () < ch->pcdata->learned[gsn_detrap])
	    {
	      send_to_char("Ok, trap removed successfully.\n\r", ch);
	      REMOVE_BIT(pexit->d_info->exit_info, EX_HASTRAP);
	      skill_gain(ch, gsn_detrap, TRUE);
	      return;
	    }
	  else
	    {
	      send_to_char("That exit is not trapped!\n\r", ch);
	      return;
	    }
	}
    }
  if(!str_cmp(arg1, "room"))
    {
      check_room_more(ch->in_room);
      if(ch->in_room->more->trap == NULL)
	{
	  send_to_char("There is no trap here to remove!\n\r", ch);
	  return;
	}
      if(number_percent() < ch->pcdata->learned[gsn_detrap])
	{
	  TRAP_DATA *trp;
	  trp = ch->in_room->more->trap;
	   send_to_char("Ok, trap removed successfully.\n\r", ch);
	   free_m(trp);
	   ch->in_room->more->trap = NULL;
	   skill_gain(ch, gsn_detrap, TRUE);
	   return;
	}
      else
	{
	  send_to_char("There is no trap here to remove!\n\r", ch);
	  return;

	}
    }
  if ((obj = get_obj_here (ch, arg1, SEARCH_INV_FIRST)) != NULL)
    {
      if (obj->pIndexData->item_type != ITEM_CONTAINER)
	{
	  send_to_char ("That's not a container.\n\r", ch);
	  return;
	}
      else
	{
	  I_CONTAINER *con = (I_CONTAINER *) obj->more;
	  if(!IS_SET(con->flags, CONT_HASTRAP))
	    {
	      send_to_char("This container is not trapped.\n\r", ch);
	      return;
	    }
	  else
	    {
	      if (number_percent() < ch->pcdata->learned[gsn_detrap])
		{
		  REMOVE_BIT(con->flags, CONT_HASTRAP);
		  send_to_char("Ok, trap removed successfully.\n\r", ch);
		  return;
		}
	      else
		{
		  send_to_char("This container is not trapped.\n\r", ch);
		  return;
		}
	    }
	}
    }
  else
    {
      send_to_char("Exactly what do you think is trapped in this room?\n\r", ch);
      return;
    }
return;
}
void
set_off_trap(CHAR_DATA *victim, ROOM_DATA *room, bool mistake)
{
  TRAP_DATA *trp;
  check_room_more(room);
  if (!victim->in_room || victim->in_room != room) return;
  trp = room->more->trap;
  if (!room->more->trap)
    {
      fprintf(stderr, "Tried to set off trap and no trap in room!\n");
      return;
    }
    level = victim->in_room->more->trap->level;
  material = victim->in_room->more->trap->material;
  dam = level * material/10;

  {
    CHAR_DATA *trapmob;
    trapmob = create_mobile(get_mob_index(2));
    char_to_room(trapmob, victim->in_room);
    for (fch = room->more->people; fch != NULL; fch = fch_next)
      {
	fch_next = fch->next_in_room;
	if (fch == trapmob)
	  continue;
	act("The trap that $n set off hits $N\n\r", victim, NULL, fch, TO_NOTVICT);
	act("The fallout from the trap that $n triggered hits YOU!\n\r", victim, NULL, fch, TO_VICT);
	damage(trapmob, fch, dam, gsn_trapset);
      }
    extract_char(trapmob, TRUE);
  }
  {
    CHAR_DATA *trapmob;
    CHAR_DATA *fch;
    SPELL_DATA *spl;
    int dam;
    trapmob = create_mobile(get_mob_index(2));
    char_to_room(trapmob, victim->in_room);

    check_room_more(victim->in_room);
    for(fch = victim->in_room->more->people; fch != NULL; fch = fch->next_in_room)
      {
	dam = number_range(5,10) * number_range(5,10);
	if (fch == trapmob) continue;
	act("The force of the trap hits $N!", trapmob, NULL, fch, TO_NOTVICT);
	act("The trap hits You!", trapmob, NULL, fch, TO_VICT);
	if ((spl = skill_lookup(NULL, (number_range(61,114)))) != NULL && number_range(1,3) == 2)
	  {
	    general_spell(spl, 150, trapmob, fch);
	  }
	else
	  damage(trapmob, fch, dam, gsn_trapset);
	stop_fighting(trapmob, TRUE);
      }
    extract_char(trapmob, TRUE);
  }

  room->more->trap = NULL;
  free_m(trp); 
  return; 
} */


