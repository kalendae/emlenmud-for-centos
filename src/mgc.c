#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

void say_spell (CHAR_DATA * ch, int sn);
void
renew_affect (CHAR_DATA * ch, AFFECT_DATA * aff)
{
  AFFECT_DATA *paf;
  AFFECT_DATA pasaf;
  bool foundd;
  foundd = FALSE;
  bzero (&pasaf, sizeof (pasaf));
  pasaf.type = aff->type;
  pasaf.duration = aff->duration;
  pasaf.location = aff->location;
  pasaf.modifier = aff->modifier;
  pasaf.bitvector = aff->bitvector;
  pasaf.bitvector2 = aff->bitvector2;
  for (paf = ch->affected; paf != NULL; paf = paf->next)
    {
      if (paf->type == aff->type && paf->location == aff->location)
	{
	  paf->duration = aff->duration;
	  foundd = TRUE;
	}
    }
  if (!foundd)
    affect_to_char (ch, &pasaf);
  return;
}


void
mana (CHAR_DATA * ch, char *argy)
{
  char bufff[500];
  int mana_water = 0;
  int mana_earth = 0;
  int mana_fire = 0;
  int mana_air = 0;
  int mana_spirit = 0;
  int water_lev = 0;
  int earth_lev = 0;
  int fire_lev = 0;
  int air_lev = 0;
  int spirit_lev = 0;
  SINGLE_OBJECT *obj = NULL;
  int nat_level = 0;
  int bonus = 0;

 DEFINE_COMMAND ("mana", mana, POSITION_DEAD, 0, LOG_NORMAL, "Allows you to see your mana in different element types, and max spell levels you can cast.")

    if (IS_MOB (ch))
    return;

   bonus = (is_member(ch, GUILD_WIZARD) ? 1 : 0)+
   (is_member(ch, GUILD_HEALER) ? 1 : 0)+
   (is_member(ch, GUILD_MYSTIC) ? 1 : 0)+
   (is_member(ch, GUILD_CONJURER) ? 1: 0);
 if (IS_AUGMENTED(ch, AUG_CHANNEL))
   {
     bonus *=2;
     bonus +=2;
   }
 nat_level = (get_curr_int (ch) + get_curr_wis (ch)) / 2 + bonus + (ch->pcdata->remort_times+1)/2;

 /* Set up the base case. */

 fire_lev = nat_level;
 air_lev = nat_level;
 earth_lev = nat_level;
 spirit_lev = nat_level;
 water_lev = nat_level;
 if (ch->pcdata->n_mana < 0) ch->pcdata->n_mana = 0;
 mana_fire = ch->pcdata->n_mana;
 mana_air = ch->pcdata->n_mana;
 mana_water = ch->pcdata->n_mana;
 mana_earth = ch->pcdata->n_mana;
 mana_spirit = ch->pcdata->n_mana;


  if ((obj = get_item_held (ch, ITEM_GEM)) != NULL)
    {
      I_GEM *gem = (I_GEM *) obj->more;
      if (IS_SET (gem->gem_type, MANA_FIRE))
	{
	  fire_lev +=  gem->max_level;
	  mana_fire += gem->mana_now;
	}
      if (IS_SET (gem->gem_type, MANA_AIR))
	{
	  air_lev += gem->max_level;
	  mana_air += gem->mana_now;
	}
      if (IS_SET (gem->gem_type, MANA_EARTH))
	{
	  earth_lev += gem->max_level;
	  mana_earth += gem->mana_now;
	}
      if (IS_SET (gem->gem_type, MANA_WATER))
	{
	  water_lev +=  gem->max_level;
	  mana_water += gem->mana_now;
	}
      if (IS_SET (gem->gem_type, MANA_SPIRIT))
	{
	  spirit_lev += gem->max_level;
	  mana_spirit += gem->mana_now;
	}
    }


  sprintf (bufff, "Fire mana available is   : [\x1B[1m%3d\x1B[0m] (Max spell level \x1B[1m%d\x1B[0m).\n\r", mana_fire, fire_lev);
  send_to_char (bufff, ch);
  sprintf (bufff, "Air mana available is    : [\x1B[1m%3d\x1B[0m] (Max spell level \x1B[1m%d\x1B[0m).\n\r", mana_air, air_lev);
  send_to_char (bufff, ch);
  sprintf (bufff, "Water mana available is  : [\x1B[1m%3d\x1B[0m] (Max spell level \x1B[1m%d\x1B[0m).\n\r", mana_water, water_lev);
  send_to_char (bufff, ch);
  sprintf (bufff, "Earth mana available is  : [\x1B[1m%3d\x1B[0m] (Max spell level \x1B[1m%d\x1B[0m).\n\r", mana_earth, earth_lev);
  send_to_char (bufff, ch);
  sprintf (bufff, "Spirit mana available is : [\x1B[1m%3d\x1B[0m] (Max spell level \x1B[1m%d\x1B[0m).\n\r", mana_spirit, spirit_lev);
  send_to_char (bufff, ch);
  sprintf (bufff, "\n\rGeneral mana available is the max of the above values and spell levels.\n\r");
  send_to_char (bufff, ch);
  return;
}



int
find_char_mana (CHAR_DATA * ch, int bit, int sn)
{
  SINGLE_OBJECT *gem;
  int mana = 0;
  SPELL_DATA *spell = NULL;
  int level = 0;
  if (IS_MOB(ch) || LEVEL(ch) >= 100) return 500;
  spell = skill_lookup (NULL, sn);
  level = (is_member(ch, GUILD_WIZARD) ? 1 : 0)+
    (is_member(ch, GUILD_HEALER) ? 1 : 0)+
    (is_member(ch, GUILD_MYSTIC) ? 1 : 0)+
    (is_member(ch, GUILD_CONJURER) ? 1: 0);
  if (IS_AUGMENTED(ch, AUG_CHANNEL))
    {
      level *=2;
      level +=2;
    }
  level += ((get_curr_int (ch) + get_curr_wis (ch)) / 2) +(ch->pcdata->remort_times+1);
   if (ch->pcdata->n_mana < 0) ch->pcdata->n_mana = 0;
  mana = ch->pcdata->n_mana;
  if ((gem = ch->hold1) != NULL && gem->pIndexData->item_type == ITEM_GEM && IS_SET(((I_GEM *) gem->more)->gem_type, bit))
    {
      mana += ((I_GEM *)gem->more)->mana_now;
      level += ((I_GEM *)gem->more)->max_level;
    }
  else if ((gem = ch->hold2) != NULL && gem->pIndexData->item_type == ITEM_GEM && IS_SET(((I_GEM *) gem->more)->gem_type, bit))
    {
      mana += ((I_GEM *)gem->more)->mana_now;
      level += ((I_GEM *)gem->more)->max_level;
    }
  if (spell->spell_level > level)
    {
      send_to_char("You cannot cast this spell without the proper type of gem!\n\r", ch);
      return 0;
    }
  return mana;
}


/*
   * Lookup a skill by slot number.
   * Used for object loading.
 */
int
slot_lookup (int slot)
{
  extern bool fBootDb;
  int sn;
  SPELL_DATA *spell;
  if (slot <= 0)
    return -1;
  return slot;
  for (sn = 0; sn < SKILL_COUNT; sn++)
    {
      if ((spell = skill_lookup (NULL, sn)) == NULL)
	continue;
      if (slot == spell->gsn /*spell->slot */ )
	return sn;
    }
  if (fBootDb)
    {
      bug ("Slot_lookup: bad slot %d.", slot);
      return 1;
      /*abort( ); */
    }
  return -1;
}

/* bah this crap is useless - taken from other code */

void
say_spell (CHAR_DATA * ch, int sn)
{
  char buf[STD_LENGTH];
  char buf2[STD_LENGTH];
  CHAR_DATA *rch;
  char *pName;
  int iSyl;
  int length;
  SPELL_DATA *spell;
  struct syl_type
    {
      char *old;
      char *new;
    }
   ;
  static const struct syl_type syl_table[] =
  {
    {
      " ", " "
    }
    ,
    {
      "ar", "zad"
    }
    ,
    {
      "au", "lju"
    }
    ,
    {
      "bless", "ufc"
    }
    ,
    {
      "blind", "plina"
    }
    ,
    {
      "bur", "gna"
    }
    ,
    {
      "cu", "laq"
    }
    ,
    {
      "de", "byn"
    }
    ,
    {
      "en", "oog"
    }
    ,
    {
      "light", "solg"
    }
    ,
    {
      "lo", "qg"
    }
    ,
    {
      "mor", "fel"
    }
    ,
    {
      "move", "ewa"
    }
    ,
    {
      "ness", "kuj"
    }
    ,
    {
      "ning", "mnab"
    }
    ,
    {
      "per", "dew"
    }
    ,
    {
      "balefire", "Pika......CHHUUUUUUUUU!!!"
    }
    ,
    {
      "ra", "jih"
    }
    ,
    {
      "re", "cus"
    }
    ,
    {
      "son", "bru"
    }
    ,
    {
      "tect", "ira"
    }
    ,
    {
      "tri", "fel"
    }
    ,
    {
      "ven", "xaq"
    }
    ,
    {
      "a", "e"
    }
    ,
    {
      "b", "d"
    }
    ,
    {
      "c", "w"
    }
    ,
    {
      "d", "j"
    }
    ,
    {
      "e", "k"
    }
    ,
    {
      "f", "e"
    }
    ,
    {
      "g", "m"
    }
    ,
    {
      "h", "y"
    }
    ,
    {
      "i", "s"
    }
    ,
    {
      "j", "i"
    }
    ,
    {
      "k", "p"
    }
    ,
    {
      "l", "x"
    }
    ,
    {
      "m", "a"
    }
    ,
    {
      "n", "r"
    }
    ,
    {
      "o", "u"
    }
    ,
    {
      "p", "g"
    }
    ,
    {
      "q", "z"
    }
    ,
    {
      "r", "n"
    }
    ,
    {
      "s", "l"
    }
    ,
    {
      "t", "d"
    }
    ,
    {
      "u", "h"
    }
    ,
    {
      "v", "b"
    }
    ,
    {
      "w", "o"
    }
    ,
    {
      "x", "a"
    }
    ,
    {
      "y", "i"
    }
    ,
    {
      "z", "b"
    }
    ,
    {
      "", ""
    }
  }
   ;
  if ((spell = skill_lookup (NULL, sn)) == NULL)
    return;
  buf[0] = '\0';
  for (pName = spell->spell_funky_name; *pName != '\0'; pName += length)
    {
      for (iSyl = 0; (length = strlen (syl_table[iSyl].old)) != 0; iSyl++)
	{
	  if (!str_prefix (syl_table[iSyl].old, pName))
	    {
	      strcat (buf, syl_table[iSyl].new);
	      break;
	    }
	}
      if (length == 0)
	length = 1;
    }
  sprintf (buf2, "$n utters the words, '%s'", buf);
  sprintf (buf, "$n utters the words, '%s'", spell->spell_funky_name);
  for (rch = ch->in_room->more->people; rch; rch = rch->next_in_room)
    {
      if (rch != ch)
	{
	  if (IS_MOB (rch))
	    act (buf, ch, NULL, rch, TO_VICT);
	  else if (rch->pcdata->learned[sn] > 30)
	    act (buf, ch, NULL, rch, TO_VICT);
	  else
	    act (buf2, ch, NULL, rch, TO_VICT);
	}
    }
  return;
}

/*
   * Compute a saving throw.
   * Negative apply's make saving throw better.
 */
bool
saves_spell (int level, CHAR_DATA * victim)
{
  int save;
  save = 40 + (((LEVEL (victim) - level) -
(victim->pcdata->saving_throw)));
  if (IS_AUGMENTED(victim, AUG_MENTAL_ARMOR)) save += 10;
  if (IS_AFFECTED_EXTRA(victim, AFF_MINDSHIELD)) save += 15;
  save = URANGE (10, save, 90);
  return number_percent () < save;
}

int
mana_now (CHAR_DATA * ch)
{
  SINGLE_OBJECT *obj;
  int mana = 0;
  if (IS_MOB (ch)) return 500;
  if (ch->pcdata->n_mana < 0) ch->pcdata->n_mana = 0;
  mana = ch->pcdata->n_mana;
  if ((obj = get_item_held(ch, ITEM_GEM)) != NULL)
    mana += ((I_GEM *) obj->more)->mana_now;
  return mana;
}

void
take_mana_char (CHAR_DATA * ch, int mana, int bit)
{

  SINGLE_OBJECT *gem;
  if (IS_MOB (ch))
    return;
  if (ch->pcdata->n_mana < 0) ch->pcdata->n_mana = 0;
    if (ch->pcdata->n_mana >= mana)
      {
	ch->pcdata->n_mana -= mana;
	return;
      }
    else
      {
	mana -= ch->pcdata->n_mana;
	ch->pcdata->n_mana = 0;
	if ((gem = ch->hold1) != NULL && gem->pIndexData->item_type == ITEM_GEM && IS_SET(((I_GEM *) gem->more)->gem_type, bit))
	  {
	    ((I_GEM *) gem->more)->mana_now -= mana;
	  }
	else if ((gem = ch->hold2) != NULL && gem->pIndexData->item_type == ITEM_GEM && IS_SET(((I_GEM *) gem->more)->gem_type, bit))
	  {
	    ((I_GEM *) gem->more)->mana_now -= mana;
	  }
	return;
      }
  return;
}



/* FIX THIS UP!! */

char *target_name;
void
do_cast (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *obj;
  int mana;
  CHAR_DATA *victim;
  int char_mana;
  int range = 0;
  bool ranged = FALSE;
  SPELL_DATA *spell;
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  DEFINE_COMMAND ("cast", do_cast, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to cast a spell.  Format is: cast 'spellname' <target>.")

    if (IS_MOB (ch))
    return;
  if (IS_AFFECTED_EXTRA(ch, AFF_SPELL_REFLECT))
   {
    affect_strip (ch, gsn_spell_reflect);
   }

  if (ch->pcdata->tickcounts > 11 && (FIGHTING (ch) != NULL))
    {
      send_to_char ("You can't cast another spell this soon after your last one!\n\r", ch);
      return;
    }
  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("You can't concentrate enough! You are groundfighting!\n\r", ch);
      return;
    }
  if (ch->position == POSITION_BASHED)
    {
      send_to_char ("You can't cast while you are sitting on your butt!\n\r", ch);
      return;
    }
  if (ch->position == POSITION_CASTING)
    {
      send_to_char ("You are already casting a spell!\n\r", ch);
      return;
    }
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_ranged] > pow.max_prac_spells)
    {
      range = 1;
      if (LEVEL(ch) > 90) range++;
      if (ch->pcdata->remort_times > 3) range++;
      if (ch->pcdata->remort_times > 6) range++;
      skill_gain(ch, gsn_ranged, TRUE);
    }
  if (strlen (argy) > 40)
    return;
  target_name = one_argy (argy, arg1);
  one_argy (target_name, arg2);
  if (IS_MOB (ch) && ch->desc == NULL)
    return;
  if (IS_SET (ch->in_room->room_flags, ROOM_NOMAGIC))
    {
      send_to_char ("The ground absorbs your magical incantations, preventing you from casting!\n\r", ch);
      return;
    }
  if (((spell = skill_lookup (arg1, -1)) == NULL)
      || (IS_PLAYER (ch) && LEVEL (ch) < spell->spell_level) || (IS_PLAYER(ch) && (ch->pcdata->learned[spell->gsn] <= 0))) 
    {
      send_to_char ("You can't do that.\n\r", ch);
      return;
    }
  if (spell->slot != 1)
    {
      send_to_char ("That is not a spell!\n\r", ch);
      return;
    }
  if (ch->position < spell->position && ch->position !=POSITION_MEDITATING)
    {
      send_to_char ("You can't concentrate enough.\n\r", ch);
      return;
    }
  switch (spell->spell_type)
    {
    default:
      bug ("Do_cast: bad target for sn %d.", spell->gsn);
      return;
    case TAR_IGNORE:
      break;
    case TAR_CHAR_OFFENSIVE:
      if (arg2[0] == '\0')
	{
	  if ((victim = FIGHTING (ch)) == NULL)
	    {
	      send_to_char ("Cast the spell on whom?\n\r", ch);
	      return;
	    }
	}
      else if (arg2[0] == '1' && arg2[1] == '.')

        {
          send_to_char ("Syntax: cast <spell name> <victim name> can not use 1. to target with a spell!\n\r", ch);
          return;
        }
      else
	{
	  if ((victim = get_char_room_near (ch, ch->in_room, range, arg2)) == NULL)
	    {
	      send_to_char ("They aren't here.\n\r", ch);
	      return;
	    }
	  else if (victim != ch)
	    {
	      if (ch->in_room == victim->in_room)
		ranged = FALSE;
	      else
		ranged = TRUE;
	    }
	}


      if (ch == victim)
	{
	  send_to_char ("You can't do that to yourself.\n\r", ch);
	  return;
	}

	/* ----------------------------------------------------------------------------
		Protect Neutral align from direct offensive spells and prevent them from
		casting these as well
	  ----------------------------------------------------------------------------  */
	if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (victim) == 0 && !IN_BATTLE (ch)
		&& ch->fgt->challenge != 10 )
	{
		send_to_char ("A strange force prevents you from harming this person.\n\r", ch);
		return;
	}

	if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE (ch)
		&& ch->fgt->challenge != 10 )
	{
		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
		return;
	}

      /* --------------------------------------------- */
      /* 	 Protect LinkDead-NoPKTimers	       */
      /* --------------------------------------------- */
      if(IS_PLAYER(victim) && victim->desc == NULL) {
	if(!(victim->pcdata->no_quit_pk > 0)) {
	  send_to_char ("Your magic cannot harm someone who's existance is questionable.\n\r", ch);
	  return;
	}
      }

    break;
    case TAR_CHAR_DEFENSIVE:
      if (arg2[0] == '\0')
	{
	  victim = ch;
	}
      else
	{
	  if ((victim = get_char_room_near (ch, ch->in_room, range, arg2)) == NULL)
	    {
	      send_to_char ("They aren't here.\n\r", ch);
	      return;
	    }
	  else if(victim != ch)
	    {
	      if (ch->in_room == victim->in_room)
		ranged = FALSE;
	      else
		ranged = TRUE;
	    }
	}
      break;
    case TAR_CHAR_SELF:
      if (arg2[0] != '\0' && !is_name (arg2, RNAME (ch)))
	{
	  send_to_char ("You can't cast this spell on another.\n\r", ch);
	  return;
	}
      break;
    case TAR_OBJ_INV:
      if (arg2[0] == '\0')
	{
	  send_to_char ("What should the spell be cast upon?\n\r", ch);
	  return;
	}
      if ((obj = get_obj_inv (ch, arg2)) == NULL)
	{
	  send_to_char ("You are not carrying that.\n\r", ch);
	  return;
	}
	  break;
	case TAR_OBJ_ROOM:
		if (arg2[0] == '\0')
		{
			send_to_char ("What should the spell be cast upon?\n\r", ch);
			return;
		}
	  
         obj = get_obj_list (ch, arg2, ch->in_room->more->contents);
	    if (obj == NULL || IS_SET (obj->extra_flags, ITEM_UNSEEN))
	    {
	      send_to_char("Nothing like that on the ground.\n\r", ch);
	      return;
	    }
      break;
    }
  char_mana = find_char_mana (ch, spell->mana_type, spell->gsn);
  if (2 + LEVEL (ch) - spell->spell_level == 0)
    mana = spell->mana;
  else
    {
      mana = IS_MOB (ch) ? 0 : UMAX (
				      spell->mana,
			       100 / (2 + LEVEL (ch) - spell->spell_level));
    }
  if (IS_PLAYER (ch) && mana > char_mana && LEVEL(ch) < MAX_LEVEL)
    {
      send_to_char ("You don't have enough magical energy of the right element to cast that spell.\n\r", ch);
      return;
    }
  if (strlen (argy) > 140)
    return;
  if (ch->desc) ch->desc->repeat = 0;
  NEW_POSITION(ch, POSITION_CASTING);
  act ("You begin to utter some strange incantations...", ch, NULL, ch, TO_CHAR);
  act("$N begins to utter some strange incantations...", ch, NULL, ch, TO_ROOM);

  check_temp (ch);
  strcpy (ch->pcdata->temp->temp_string, argy);
  if (IS_PLAYER (ch))
    if ((LEVEL (ch) < 100) || (LEVEL (ch) > 99 && IS_SET (ch->pcdata->act2, PLR_HOLYLAG)))
      {
	int bonus =
	  (is_member(ch, GUILD_MYSTIC) ? 4 : 0) +
	  (is_member(ch, GUILD_WIZARD) ? 4 : 0) +
	  (is_member(ch, GUILD_HEALER) ? 4 : 0) +
	  (is_member(ch, GUILD_CONJURER) ? 4 : 0) +
	  (IS_AUGMENTED(ch, AUG_MIND_POWER) ? 3 : 0) +
	  (IS_AUGMENTED(ch, AUG_MIND_FOCUS) ? 3 : 0) +
	  (IS_AUGMENTED(ch, AUG_FOCUS) ? 7 : 0) +
	  ch->pcdata->remort_times*0;
	check_fgt (ch);
	ch->pcdata->tickcounts = ((spell->casting_time) * (100-bonus))/100;
	if (ranged && (ch->pcdata->learned[gsn_ranged] > pow.max_prac_spells))
		ch->pcdata->tickcounts +=5;


      }
  return;
}

void
do_actual_cast (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  CHAR_DATA *victim;
  SINGLE_OBJECT *obj;
  void *vo;
  int mana;
  int range = 0;
  int char_mana;
  int sn;
  SPELL_DATA *spell;
/*
   * Switched NPC's can cast spells, but others can't.
 */
  MARK_CHAR_DEBUG ("Actual_cast", ch)
    if (IS_MOB (ch) && ch->desc == NULL)
    return;
  if (ch->position == POSITION_GROUNDFIGHTING)
    return;
  if (FIGHTING (ch) == NULL && ch->position == POSITION_CASTING)
    NEW_POSITION(ch, POSITION_STANDING);
  if (FIGHTING (ch) != NULL && ch->position == POSITION_CASTING)
    NEW_POSITION(ch, POSITION_FIGHTING);
  target_name = one_argy (argy, arg1);
  one_argy (target_name, arg2);
  if (IS_SET (ch->in_room->room_flags, ROOM_NOMAGIC) && LEVEL(ch) <MAX_LEVEL)
    {
      send_to_char ("The ground absorbs your magical incantations, preventing you from casting!\n\r", ch);
      return;
    }
  if (IS_SET (ch->in_room->room_flags, ROOM_LOWMAGIC) && LEVEL(ch)<MAX_LEVEL && (number_range(1,3) == 3))
    {
      send_to_char ("The ground absorbs your magical incantations, preventing you from casting!\n\r", ch);
      return;
    }
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_ranged] >= pow.max_prac_spells)
    {
      range = 1;
      if (LEVEL(ch) > 60) range++;
      if (ch->pcdata->remort_times > 3) range++;
      if (ch->pcdata->remort_times > 5) range++;
    }

  if (arg1[0] == '\0')
    {
      send_to_char ("Cast which what where?\n\r", ch);
      return;
    }
  if ((spell = skill_lookup (arg1, -1)) == NULL
      || (IS_PLAYER (ch) && LEVEL (ch) < spell->spell_level))
    {
      send_to_char ("You can't do that.\n\r", ch);
      return;
    }
  sn = spell->gsn;
  if (ch->position == POSITION_CASTING)
    NEW_POSITION(ch, POSITION_STANDING);
  if (ch->position < spell->position)
    {
      send_to_char ("You can't concentrate enough.\n\r", ch);
      return;
    }
  if (2 + LEVEL (ch) - spell->spell_level == 0)
    mana = spell->mana;
  else
    {
      mana = IS_MOB (ch) ? 0 : UMAX (spell->mana,
				      100 / (2 + LEVEL (ch) - spell->spell_level));
    }
/*
   * Locate targets.
 */
  victim = NULL;
  obj = NULL;
  vo = NULL;
  switch (spell->spell_type)
    {
    default:
      bug ("Do_cast: bad target for sn %d.", sn);
      return;
    case TAR_IGNORE:
      break;
    case TAR_CHAR_OFFENSIVE:
      if (arg2[0] == '\0')
	{
	  if ((victim = FIGHTING (ch)) == NULL)
	    {
	      send_to_char ("Cast the spell on whom?\n\r", ch);
	      return;
	    }
	}
      else
	{
	  if ((victim = get_char_room_near (ch, ch->in_room, range, arg2)) == NULL)
	    {
	      send_to_char ("They aren't here.\n\r", ch);
	      return;
	    }
	}
      if (ch == victim && LEVEL (ch) < 10)
	{
	  send_to_char ("You can't do that to yourself.\n\r", ch);
	  return;
	}
      if (ch->position != POSITION_FIGHTING || FIGHTING (ch) != victim)
	if (!pkill (ch, victim))
	  return;
      if (IS_PLAYER (ch) && LEVEL(ch) < MAX_LEVEL)
	ch->pcdata->tickcounts = spell->casting_time/2+3;
      if (is_member(ch, GUILD_MYSTIC))
	ch->pcdata->tickcounts /= 2;
      /* if ( IS_PLAYER(ch) )
         {
         * Removed no-char fuzzyness *
         if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
         {
         send_to_char( "You can't do that on your own follower.\n\r",
         ch );
         return;
         }
         }
       */
      vo = (void *) victim;
      break;
    case TAR_CHAR_DEFENSIVE:
      if (arg2[0] == '\0')
	{
	  victim = ch;
	}
      else
	{
	  if ((victim = get_char_room_near (ch, ch->in_room, range, arg2)) == NULL)
	    {
	      send_to_char ("They aren't here.\n\r", ch);
	      return;
	    }
	}
      vo = (void *) victim;
      break;
    case TAR_CHAR_SELF:
      if (arg2[0] != '\0' && !is_name (arg2, RNAME (ch)))
	{
	  send_to_char ("You can't cast this spell on another.\n\r", ch);
	  return;
	}
      vo = (void *) ch;
      break;
    case TAR_OBJ_INV:
      if (arg2[0] == '\0')
	{
	  send_to_char ("What should the spell be cast upon?\n\r", ch);
	  return;
	}
      if ((obj = get_obj_inv (ch, arg2)) == NULL)
	{
	  send_to_char ("You are not carrying that in your hand.\n\r", ch);
	  return;
	}
      vo = (void *) obj;
      break;
    
	case TAR_OBJ_ROOM:
		if (arg2[0] == '\0')
		{
			send_to_char ("What should the spell be cast upon?\n\r", ch);
			return;
		}
	  
       obj = get_obj_list (ch, arg2, ch->in_room->more->contents);
	   if (obj == NULL || IS_SET (obj->extra_flags, ITEM_UNSEEN))
	    {
	      send_to_char("Nothing like that on the ground...\n\r", ch);
	      return;
	    }
	  vo = (void *) obj;
      break;
	}

  MARK_CHAR_DEBUG ("Actual_cast_find_char_mana", ch)

    char_mana = find_char_mana (ch, spell->mana_type, sn);

  MARK_CHAR_DEBUG ("Actual_cast_after_find_char_mana", ch)

    if (victim != NULL && spell->spell_type == TAR_CHAR_OFFENSIVE &&
	is_safe (ch, victim) && ((IS_PLAYER (ch) && !IS_MOB (victim) && !DIFF_ALIGN (ch, victim)) || IS_MOB (victim)))
    {
      send_to_char ("You are in a safe zone. No casting nasty spells like that!\n\r", ch);
      return;
    }
  if (IS_PLAYER (ch) && mana > char_mana)
    {
      switch (spell->mana_type)
	{
	default:
	  send_to_char ("You don't have enough mana to cast that spell.\n\r", ch);
	  break;
	case MANA_FIRE:
	  send_to_char ("You don't have enough mana to contain the Fire.\n\r", ch);
	  break;
	case MANA_AIR:
	  send_to_char ("You require more mana for Air.\n\r", ch);
	  break;
	case MANA_WATER:
	  send_to_char ("You don't have enough mana for Water.\n\r", ch);
	  break;
	case MANA_EARTH:
	  send_to_char ("Your Earth mana is insufficient.\n\r", ch);
	  break;
	case MANA_SPIRIT:
	  send_to_char ("Your Spirit mana is insufficient.\n\r", ch);
	  break;
	}
      return;
    }

  if( IS_PLAYER(ch) && (LEVEL(ch) < 100) &&
      (number_range(1,125) >
      (ch->pcdata->learned[spell->gsn]
       + (is_member (ch, GUILD_WIZARD) ? 6 : 0)
       + (is_member (ch, GUILD_HEALER) ? 6 : 0)
       + (is_member (ch, GUILD_MYSTIC) ? 6 : 0)
       + (is_member (ch, GUILD_CONJURER) ? 6 : 0)
       + (IS_AUGMENTED(ch, AUG_FOCUS) ? 7 :0)
       + (IS_AUGMENTED(ch, AUG_MIND_POWER) ? 3 : 0)
       + (IS_AUGMENTED(ch, AUG_MIND_FOCUS) ? 3 : 0))))
    {
      send_to_char ("You lost your concentration.\n\r", ch);
      take_mana_char (ch, mana / 2, spell->mana_type);
    }
  else
    {
      say_spell (ch, sn);
      if (IS_PLAYER(ch) && LEVEL(ch) < 100)
	{
	  if (spell->spell_lag > 0 && ch->pcdata->no_quit > 0)
	    {
	      char buf[200];
	      sprintf(buf, "You must wait %d hours before casting %s!\n\r", ch->pcdata->no_quit, spell->spell_name);
	      send_to_char(buf, ch);
	      return;
	    }
          take_mana_char (ch, mana, spell->mana_type);
	  if (number_percent() < spell->fail_chance)
	    {
	      send_to_char("The spell failed...\n\r", ch);
	      return;
	    }
	  if (ch->pcdata->no_quit_pk > 0 && number_percent() < spell->pk_fail_chance)
	    {
	      send_to_char("The spell failed...\n\r", ch);
	      return;
	    }

	  if (spell->guilds != 0)
	    {
	      int i;
	      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
		{
		  if(IS_SET(spell->guilds, guild_data[i].mob_guildmaster_bit) && !IS_SET(ch->pcdata->guilds, guild_data[i].mob_guildmaster_bit))
		    {
		      send_to_char("You do not have the proper guilds to cast this!\n\r", ch);
		      return;
		    }
		}
	    }
	}
      general_spell (spell, LEVEL (ch), ch, vo);
      if (victim && victim->data_type==50)
	{
	  return;
	}
    }


    if (spell->spell_type == TAR_CHAR_OFFENSIVE
	&& victim != ch
	&& MASTER (victim) != ch)
    {
      CHAR_DATA *vch;
      CHAR_DATA *vch_next;
      for (vch = ch->in_room->more->people; vch; vch = vch_next)
	{
	  vch_next = vch->next_in_room;
	  if (victim == vch && FIGHTING (victim) == NULL)
	    {
	      multi_hit (victim, ch, TYPE_UNDEFINED);
	      break;
	    }
	}
    }
return;
}

/*
   * Cast spells at targets using a magical object.
 */
void
obj_cast_spell (int sn, int level, CHAR_DATA * ch, CHAR_DATA * victim, SINGLE_OBJECT * obj)
{
  void *vo;
  SPELL_DATA *spell;
  if (IS_SET (ch->in_room->room_flags, ROOM_NOMAGIC))
    {
      send_to_char ("The ground seems to absorb the spell...!\n\r", ch);
      return;
    }
  if (sn <= 0 || (spell = skill_lookup (NULL, sn)) == NULL)
    return;
  switch (spell->spell_type)
    {
    default:
      bug ("Obj_cast_spell: bad target for sn %d.", sn);
      return;
    case TAR_IGNORE:
      vo = NULL;
      break;
    case TAR_CHAR_OFFENSIVE:
      if (victim == NULL)
	victim = FIGHTING (ch);
      if (victim == NULL)
	{
	  send_to_char ("You can't do that.\n\r", ch);
	  return;
	}
      if (IS_PLAYER (ch))
	{
	  if (ch->pcdata->tickcounts > 2)
	    {
	      send_to_char ("The wand refuses to respond!\n\r", ch);
	      return;
	    }
	}
      if (ch->position != POSITION_FIGHTING || FIGHTING (ch) != victim)
	if (!pkill (ch, victim))
	  return;
      vo = (void *) victim;
      break;
    case TAR_CHAR_DEFENSIVE:
      if (victim == NULL)
	victim = ch;
      vo = (void *) victim;
      break;
    case TAR_CHAR_SELF:
      vo = (void *) ch;
      break;
    case TAR_OBJ_INV:
      if (obj == NULL)
	{
	  send_to_char ("You can't do that.\n\r", ch);
	  return;
	}
      vo = (void *) obj;
      break;
    }
  target_name = "";
  general_spell (spell, level, ch, vo);
/* (*skill_table[sn].spell_fun) ( sn, level, ch, vo ); */
  if (IS_PLAYER (ch))
    ch->pcdata->tickcounts = spell->casting_time + 10;
  if (spell->spell_type == TAR_CHAR_OFFENSIVE
      && victim != ch
      && MASTER (victim) != ch)
    {
      CHAR_DATA *vch;
      CHAR_DATA *vch_next;
      for (vch = ch->in_room->more->people; vch; vch = vch_next)
	{
	  vch_next = vch->next_in_room;
	  if (victim == vch && FIGHTING (victim) == NULL)
	    {
	      check_fgt (victim);
	      multi_hit (victim, ch, TYPE_UNDEFINED);
	      victim->fgt->fighting = ch;
	      NEW_POSITION(victim, POSITION_FIGHTING);
	      break;
	    }
	}
    }
  return;
}
