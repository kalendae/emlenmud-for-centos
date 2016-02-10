/* Check that mobs yell when you cast a spell at them */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

void show_hitdam(int, char*, int, CHAR_DATA*, CHAR_DATA*);
#define CHECK_CHAR(ch)   if ( (ch)->data_type==50) return

bool hitop = FALSE;

int ghit_pos = 0;
char reasonfd[30];

bool
check_skill(CHAR_DATA *ch, int gsn)
{
  SPELL_DATA *spl;
  if (IS_MOB(ch))
    if(number_range(1,4) == 3)
      return FALSE;
    else return TRUE;
  if ((spl = skill_lookup(NULL, gsn)) != NULL)
    {
      if (spl->guilds & ~(ch->pcdata->guilds))
	{
	  return FALSE;
	}
      if (LEVEL(ch) < spl->spell_level)
	return FALSE;
    }
  if (ch->pcdata->learned[gsn] == -100 || number_range(1,103) > ch->pcdata->learned[gsn])
    return FALSE;

  return TRUE;
}



void
check_same_side_pk (CHAR_DATA *ch, CHAR_DATA *victim)
{
  char log_buf[200];
  int i;
  CHAR_DATA *guarder;
  if (IS_MOB(victim) || !ch->in_room)
    return;
  check_room_more(ch->in_room);
  if (IS_PLAYER(ch) && (ch != victim) && (!ch->fgt || ch->fgt->fighting != victim))
    {
      if ((!IS_SET(victim->act, PLR_SAME_SIDE_PK) && ch->in_room->vnum >= 1000 && !DIFF_ALIGN(ch, victim)) || LEVEL(victim) >= IMM_LEVEL)
	{
	  sprintf(log_buf, "ILLEGAL PKILL %s attacked %s at %d.\n\r", NAME(ch), NAME(victim), ch->in_room->vnum);
	  log_string(log_buf);
	  SET_BIT(ch->act, PLR_SAME_SIDE_PK);
          send_to_char("\x1b[1;31mYou attacked someone illegally!\n\r", ch);
          send_to_char("\x1b[1;31mYou attacked someone illegally!\n\r",ch);
          send_to_char("\x1b[1;31mYou attacked someone illegally!\n\r", ch);
	  send_to_char("You have been attacked by someone who was not allowed to.\n\r", victim);
	}
      else if (!DIFF_ALIGN(ch, victim) && ch->in_room->vnum >=1000)
	{
	  sprintf(log_buf, "OK PKILL %s attacked %s at %d.\n\r", NAME(ch), NAME(victim), ch->in_room->vnum);
	  log_string(log_buf);
	  SET_BIT(ch->act, PLR_SAME_SIDE_PK);
        }
    }
  if (IS_PLAYER(ch) && IS_PLAYER(victim) && FIGHTING(ch) == NULL && FIGHTING(victim) == NULL && number_range(1,5) == 3)
    {
       char buf[500];
       sprintf(buf, "Help, I am being attacked by %s at %s.\n\r",rNAME(ch, victim), ch->in_room->name);
       do_yell(victim, buf);
     }
  /*ARC: no timers against neutral align, unless battle ground */
  if (IS_PLAYER (ch) && ch->pcdata->no_quit_pk == 0 && IS_PLAYER(victim) && ALIGN(ch)!= 0 
	&& ALIGN(victim)!=0 && !IN_BATTLE(ch) && ch->fgt->challenge != 10)
    {
      ch->pcdata->no_quit_pk = 4;
    }
  if (IS_PLAYER (victim) && victim->pcdata->no_quit_pk == 0 && IS_PLAYER(ch) && ALIGN(ch)!= 0 
	&& ALIGN(victim)!=0 && !IN_BATTLE(ch) && ch->fgt->challenge != 10)
    {
      victim->pcdata->no_quit_pk = 4;
    }
  if(number_range(1,4) == 2)
    for (i = 0; i < 4; i ++)
      {
	if ((guarder = victim->pcdata->guarded_by[i]) != NULL &&
	    IS_PLAYER(guarder) &&
	    (guarder->pcdata->guarding == victim) &&
	    (guarder->in_room == victim->in_room))
	  {
	    if (number_range(1,70) == 3)
	      skill_gain(guarder, gsn_guard, TRUE);
	    if (check_skill(ch, gsn_guard))
	      {
		act("$n heroically steps in front of $N!", guarder, NULL, victim, TO_NOTVICT);
		act("You heroically step in front of $N", guarder, NULL, victim, TO_CHAR);
		act("$n heroically steps in front of you", guarder, NULL, victim, TO_VICT);
		check_fgt(ch);
		ch->fgt->fighting = guarder;
		return;
	      }
	  }
      }
  return;
}



void do_tackle (CHAR_DATA * ch, char *argy) {
  CHAR_DATA *victim;
  int chan;
  int xn;
  char buf[STD_LENGTH];
  DEFINE_COMMAND ("tackle", do_tackle, POSITION_FIGHTING, 0, LOG_NORMAL, "This command makes your character attempt to tackle his/her opponent.")
    check_fgt (ch);


  if (IS_PLAYER(ch))
    if (!IS_SET (ch->ced->fight_ops, F_GROUND)) {
      send_to_char("You cannot tackle someone while you are trying to avoid groundfighting.\n\r", ch);
      return;
    }


  if (ch->fgt->ears > 9) {
    send_to_char ("You can't tackle this soon after giving or recieving a bash.\n\r", ch);
    return;
  }
  chan = 0;
  if (ch->wait > 2) return;

  if (FIGHTING (ch) == NULL) {
    if (argy[0] == '\0' || argy == "")
      {
	send_to_char ("Tackle who?\n\r", ch);
        WAIT_STATE(ch, 3*PULSE_VIOLENCE); 
	return;
      }
    if ((victim = get_char_room (ch, argy)) == NULL)
      {
	send_to_char ("Tackle who?\n\r", ch);
        WAIT_STATE(ch, 3*PULSE_VIOLENCE); 
	return;
      }
    if (victim == ch)
      {
	send_to_char ("You really do look like an idiot!\n\r", ch);
	return;
      }
    /* ----------------------------------------- */
    /*		 Protecting Neutral align.	 */
    /* ----------------------------------------- */
    if (IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN(victim) == 0 && !IN_BATTLE(ch)
	&& ch->fgt->challenge != 10)
    {
	send_to_char ("A strange force prevents you from harming this person.\n\r", ch);
	return;
    }
    if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
	&& ch->fgt->challenge != 10)
    {
	send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
	return;
    }

    /* ----------------------------------------- */
    /*	      Protecting LinkDead-NoTimers 	 */
    /* ----------------------------------------- */
    if(IS_PLAYER(victim) && victim->desc == NULL) {
      if(!(victim->pcdata->no_quit_pk > 0)) {
	send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
	return;
      }
    }

  } else victim = FIGHTING (ch);

  if (FIGHTING(ch) == NULL && FIGHTING(victim) != NULL && !IS_IN_MELEE(ch, victim))
  {
    send_to_char("You cannot get close enough to tackle them!\n\r", ch);
    return;
  }

  if (RIDING (ch) != NULL)
  {
    send_to_char ("You can't tackle someone when you are riding!\n\r", ch);
    return;
  }

  if (is_safe (ch, victim)) return;

  if (!can_see (ch, victim))  {
    send_to_char ("How can you tackle someone you can't see!?\n\r", ch);
    return;
  }

  if (!can_groundfight (victim) && (IS_MOB(ch) ||
   (str_cmp(race_info[ch->pcdata->race].name, "minotaur") &&
   str_cmp(race_info[ch->pcdata->race].name, "gargoyle")))) {
    act ("You do not want to tackle that kind of creature!", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (victim->position == POSITION_GROUNDFIGHTING)
  {
    act ("$N is already on the ground, tackled!", ch, NULL, victim, TO_CHAR);
    WAIT_STATE(ch, 2*PULSE_VIOLENCE);
    return;
  }

  if (victim->position == POSITION_BASHED)
  {
    act ("$N has been bashed, and you stumble past them!", ch, NULL,victim, TO_CHAR);
    WAIT_STATE(ch, 2*PULSE_VIOLENCE);
    return;
  }

  check_ced (ch);
  check_ced (victim);
  if (FIGHTING(victim) != NULL && !(IS_IN_MELEE (ch, victim)))
    {
      send_to_char ("You are not in the main melee!\n\r", ch);
      return;
    }

  if (!(IS_IN_MELEE (ch, victim)))
    {
      send_to_char ("You are not in the main melee!\n\r", ch);
      return;
    }

  if (!pow.tackle_with_weapon)
  {
    if ((ch->hold1 != NULL && (ch->hold1->pIndexData->item_type == ITEM_WEAPON)))
      {
	send_to_char ("You cannot tackle with a weapon in hand!\n\r", ch);
	return;
      }
  }

  if (!pow.tackle_person_with_weapon)
  {
      if ((victim->hold1 != NULL && (victim->hold1->pIndexData->item_type == ITEM_WEAPON || victim->hold1->pIndexData->item_type == ITEM_GEM)) || (victim->hold2 && (victim->hold2->pIndexData->item_type == ITEM_WEAPON
|| victim->hold2->pIndexData->item_type == ITEM_GEM)))
          {
            send_to_char ("Are you CRAZY!? Tackle someone with a weapon!??\n\r", ch);
            send_to_char ("You'd surely get whacked on your approach...\n\r", ch);
            return;
          }
  }
check_same_side_pk(ch, victim);

/* ---------------------------------------------------------------- */
/*                Calculate chance to tackle here                   */
/* ---------------------------------------------------------------- */
chan = 20;
xn   = 0;

/* ---------------------------------- */
/* --------- Victim's Dex ----------- */
/* ---------------------------------- */
chan = get_curr_dex(ch) + (LEVEL(ch)/2);
chan -= get_curr_dex(victim);


/* ------------------------------------- */
/* --------- Char's Grapple  ----------- */
/* ------------------------------------- */
if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_grapple] > 0)
  chan += (ch->pcdata->learned[gsn_grapple]/4);


/* ------------------------------------ */
/* --------- Char's Tackle  ----------- */
/* ------------------------------------ */
if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_tackle] > 0)
  chan += (ch->pcdata->learned[gsn_tackle]/4);


/* ---------------------------------- */
/* ----------- Augments ------------- */
/* ---------------------------------- */
if (IS_AUGMENTED(ch, AUG_HAND_SPD)) chan += 10;
if (IS_AUGMENTED(ch, AUG_HAND_POWER)) chan += 10;
if (IS_AUGMENTED(ch, AUG_FOOT_SPD)) chan += 10;
if (IS_AUGMENTED(ch, AUG_LEG_SPD)) chan += 10;
if (IS_AUGMENTED(victim, AUG_FOOT_SPD)) chan -= 10;
if (IS_AUGMENTED(victim, AUG_LEG_SPD)) chan -= 10;
if (IS_PLAYER(victim) && check_skill(victim, gsn_steadiness))
  {
    chan -= (victim->pcdata->learned[gsn_steadiness]/3);
    skill_gain (victim, gsn_steadiness, TRUE);
  }

/* ---------------------------- */
/* --------- Guilds ----------- */
/* ---------------------------- */
if(is_member(ch,GUILD_WARRIOR))      chan += 10;
if(is_member(ch,GUILD_BATTLEMASTER)) chan += 10;
if(is_member(victim,GUILD_THIEFG))   chan -= 5;
if(is_member(victim,GUILD_ROGUE)) chan -= 5;


/* ---------------------------------- */
/* --------- Ground Fight ----------- */
/* ---------------------------------- */
if (victim->ced && IS_SET (victim->ced->fight_ops, F_GROUND))
  xn = 125;
else
  xn = 175;


/* ---------------------------------- */
/* ------------ Height -------------- */
/* ---------------------------------- */
chan += ch->height / 5;
chan -= victim->height / 5;


/* ---------------------------------- */
/* ------ Race Consideration -------- */
/* ---------------------------------- */
if (IS_PLAYER(ch) && (!str_cmp(race_info[ch->pcdata->race].name, "minotaur")
		  || !str_cmp(race_info[ch->pcdata->race].name, "gargoyle")))
     chan = UMAX(chan, 75);


/* ----------------------- */
/* ----- Skill gain? ----- */
/* ----------------------- */
if (number_range(1,10) == 1)
  skill_gain(ch, gsn_tackle, TRUE);


/* ------------------------------------------ */
/* --------- Now, see if they hit ----------- */
/* ------------------------------------------ */
if ((number_range (0, xn) > chan && IS_AWAKE (victim)) || (number_range(1,70) > ch->pcdata->learned[gsn_tackle]))
  {
    send_to_char ("You miss the tackle and almost fall on the ground!\n\r", ch);
    act ("$n tries to tackle you, but almost falls over instead!", ch, NULL, victim, TO_VICT);
    act ("$n misses a tackle at $N and almost falls over!", ch, NULL, victim, TO_NOTVICT);
    WAIT_STATE (ch, 2 * PULSE_VIOLENCE);
    if (FIGHTING (victim) == NULL)
      set_fighting (victim, ch);
    set_fighting (ch, victim);
    return;
  }
check_ced (victim);
check_ced (ch);
victim->ced->ground_pos = 0;
ch->ced->ground_pos = 0;
NEW_POSITION(victim, POSITION_GROUNDFIGHTING);
NEW_POSITION(ch, POSITION_GROUNDFIGHTING);
if (RIDING (victim) != NULL)
  {
    sprintf (buf, "As you jump up and tackle $N, $S flies off of %s and lands hard on the ground!", PERS (RIDING (victim), ch));
      act (buf, ch, NULL, victim, TO_CHAR);
      sprintf (buf, "$n's flying tackle makes $N fall off of %s. $N lands on his back on the ground.", NAME (RIDING (victim)));
      act (buf, ch, NULL, victim, TO_NOTVICT);
      sprintf (buf, "You fall off of %s as $n tackles you!", PERS (RIDING (victim), victim));
      act (buf, ch, NULL, victim, TO_VICT);
      check_fgt (victim->fgt->riding);
      victim->fgt->riding->fgt->mounted_by = NULL;
      victim->fgt->riding = NULL;
  }
else if (RIDING (victim) == NULL)
  {
    act ("You tackle $N! The fight is taken to the ground!", ch, NULL, victim, TO_CHAR);
    act ("$n tackles $N and they both end up on the ground!", ch, NULL, victim, TO_NOTVICT);
    act ("$n tackles you! The fight is taken to the ground!", ch, NULL, victim, TO_VICT);
  }
set_fighting (ch, victim);
set_fighting (victim, ch);
victim->fgt->fighting = ch;
ch->fgt->fighting = victim;
victim->ced->ground_pos = 0;
ch->ced->ground_pos = 0;
NEW_POSITION(victim, POSITION_GROUNDFIGHTING);
NEW_POSITION(ch, POSITION_GROUNDFIGHTING);
WAIT_STATE (victim, 2 * PULSE_VIOLENCE);
WAIT_STATE (ch, (3 * PULSE_VIOLENCE));
return;
}

void
violence_update (void)
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  CHAR_DATA *victim;
  CHAR_DATA *rch;
  CHAR_DATA *rch_next;
  int posorg;
for (ch = f_first; ch != NULL; ch = ch_next)
  {
    if (ch->data_type==50)
      {
	fprintf(stderr,"That was it!\n");
	remove_from_fighting_list(ch);
	return;
      }
    if (FIGHTING(ch) && FIGHTING(ch)->data_type==50)
      {
	fprintf(stderr,"That was it2!\n");
	ch->fgt->fighting=NULL;
        update_pos(ch);
	return;
      }
    ch_next = ch->next_fighting;
    check_fgt (ch);
    check_ced(ch);
    ch->fgt->ears--;
    if (ch->fgt->ears <= 10)
      {
	ch->fgt->ears = 2;
	if (ch->position == POSITION_BASHED)
	  do_stand (ch, "");
      }
    if ((victim = FIGHTING(ch)) == NULL)
      {
	if (ch->position == POSITION_FIGHTING)
	    NEW_POSITION(ch, POSITION_STANDING);
	if (IS_MOB(ch))
	  {
	    if (IS_SET (ch->pIndexData->act, ACT_FASTHUNT) &&
		ch->fgt->hunting != NULL &&
		ch->position == POSITION_STANDING &&
		number_range (1, 3) == 2)
	      hunt_victim (ch);

	    if (IS_AFFECTED (ch, AFF_CHARM) &&
		ch->fgt->master != NULL &&
		ch->fgt->master->in_room == ch->in_room &&
		ch->fgt->master->fgt != NULL &&
		ch->fgt->master->fgt->fighting != NULL)
	      multi_hit (ch, ch->fgt->master->fgt->fighting, TYPE_UNDEFINED);
	    else
	      continue;
	  }
	else
	  continue;
      }
    if (victim == ch)
      {
	ch->fgt->fighting = NULL;
	ch->position = POSITION_STANDING;
      }
    else if (FIGHTING (victim) == NULL)
      {
	if (victim == NULL) return;
	check_fgt(victim);
	victim->fgt->fighting = ch;
      }
    if (IS_AWAKE (ch) && ch->in_room == victim->in_room)
      {
	if (IS_MOB (ch) && IS_SET (ch->act, ACT_MOBSWITCH))
	  {
	    CHAR_DATA *cir;
	    for (cir = ch->in_room->more->people; cir != NULL; cir = cir->next_in_room)
	      {
		if (FIGHTING (cir) == ch && number_range(1,7) == 3 && FIGHTING (ch) != cir)
		  {
		    act ("$n turns and attacks $N!", ch, NULL, cir, TO_NOTVICT);
		    act ("$n turns towards you and attacks!", ch, NULL, cir, TO_CHAR);
		    set_fighting (ch, cir);
		    break;
		  }
	      }
	  }
	posorg = victim->position;
	if (ch->fgt->challenge == 10 && number_range (1, 7) == 2)
	  {
	    char bufr[500];
	    sprintf (bufr, "($B$1Arena$N$7) $B%s: $4%s $7%s: $4%s", NAME (ch), STRING_HITS (ch), NAME (victim), STRING_HITS (victim));
	    do_arenaecho (ch, bufr, TRUE);
	  }
	if (ch != victim && ch->position == POSITION_GROUNDFIGHTING && victim->position == POSITION_GROUNDFIGHTING)
	  {
	    ground_hit (ch, victim, TYPE_UNDEFINED);
	    if (IS_PLAYER(ch) &&
		IS_AUGMENTED(ch, AUG_FACE_SPIKE) &&
		victim != NULL &&
		victim->data_type != 50 &&
		victim->hit > 10
		&& check_skill(ch, gsn_grapple)  &&
		number_range(1,4) == 3)
	      ground_hit(ch, victim, TYPE_UNDEFINED);
            if (IS_MOB(ch) && number_range(1,10) == 3)
                 do_stand(ch, "");
	  }
	else if (ch->position != POSITION_CASTING)
	  {
              if (ch->fgt->challenge == 10 && number_range (1, 5) == 2)
                {
                  char buf[200];
		  switch (number_range(1,5))
		    {
		    case 1:
		      sprintf (buf, "($B$1Arena$N$7) $BBlood runs freely as %s and %s exchange blows.", NAME (ch), NAME (victim));
		      break;
		    case 2:
		      sprintf (buf, "($B$1Arena$N$7) $BThe two fighters circle, flailing at each other wildly.");
		      break;
		    case 3:
		      sprintf (buf, "($B$1Arena$N$7) $BPieces of flesh fly every where as blows are exchanged.");
		      break;
		    case 4:
		      sprintf (buf, "($B$1Arena$N$7) $B%s lets out a loud warcry before attacking!", NAME (ch));
		      break;
		    case 5:
		      sprintf (buf, "($B$1Arena$N$7) $BYou can almost hear the crunching of bones...");
		      break;
		    }
		  do_arenaecho (ch, buf, TRUE);
		}

	      if (IS_MOB (ch) && ch->pIndexData && ch->pIndexData->opt)
                {
                  if (number_range (1, 101) < ch->pIndexData->opt->warrior_percent)
                    {
		      if (number_range(1,3) ==2)
			do_bash (ch, "");
		      else if (number_range(1,2) == 2)
			do_kick (ch, "");
		      else if (number_range(1,3) == 3)
			do_tackle (ch, "");
		      else if (number_range(1,2) == 2)
			do_flurry (ch, "");
		      else if (number_range(1,2) ==2)
			do_tackle (ch, "");
		      else if (number_range(1,3) == 3)
			do_flail (ch, "");
		      else do_kick(ch, "");
                    }
                  else
                    {
                      if (number_range (1, 101) < ch->pIndexData->opt->cast_percent)
                        {
                          int iil;
                          for (iil = 0; iil < 10; iil++)
                            {
                              if (number_range (1, 2) == 1 && ch->pIndexData->opt->cast_spells[iil] != NULL)
                                {
                                  SPELL_DATA *spl;
                                  if ((spl = skill_lookup (ch->pIndexData->opt->cast_spells[iil], -1)) == NULL)
                                    continue;
                                  if (LEVEL (ch) < spl->spell_level/3)
                                    continue;
                                  if (spl->spell_type == TAR_CHAR_DEFENSIVE ||
                                      spl->spell_type == TAR_CHAR_SELF)
                                    {
                                      general_spell (spl, LEVEL (ch),ch,ch);
                                    }
                                  else
                                    general_spell (spl, LEVEL (ch), ch,victim);
                                  break;
                                }
                            }
                        }
                    }
                }
	      if(number_range(1,4) == 2 &&  victim && IS_PLAYER(victim) && IS_PLAYER(ch))
		{
		  int i;
		  for (i = 0; i < 4; i ++)
		    {
		      CHAR_DATA *guarder;
		      if ((guarder = victim->pcdata->guarded_by[i]) != NULL &&
			  IS_PLAYER(guarder) &&
			  (guarder->pcdata->guarding == victim) &&
			  (guarder->in_room == victim->in_room))
			{
			  if (number_range(1,70) == 3)
			    skill_gain(guarder, gsn_guard, TRUE);
			  if(number_percent() < ch->pcdata->learned[gsn_guard])
			    {
			      act("$n heroically steps in front of $N!", guarder, NULL, victim, TO_NOTVICT);
			      act("You heroically step in front of $N", guarder, NULL, victim, TO_CHAR);
			      act("$n heroically steps in front of you", guarder, NULL, victim, TO_VICT);
			      check_fgt(ch);
			      ch->fgt->fighting = guarder;
			      break;
			    }
			}
		    }
		}
	      if ((victim = FIGHTING(ch)) == NULL)
		{
		  continue;
		}
	      multi_hit (ch, victim, TYPE_UNDEFINED);
	      if (FIGHTING (ch) && FIGHTING (FIGHTING (ch)) != ch && number_range (1, 2) == 2)
		continue;


	  }
	if (posorg == POSITION_CASTING && victim->position != POSITION_CASTING)
	  {
	    send_to_char ("OUCH! You just lost your concentration!\n\r", victim);
	    if (IS_PLAYER (victim))
	      victim->pcdata->tickcounts = 0;
	  }
      }
    else
      {
      stop_fighting (ch, FALSE);
      }
    if ((victim = FIGHTING (ch)) == NULL)
      continue;
    for (rch = ch->in_room->more->people; rch != NULL; rch = rch_next)
      {
	rch_next = rch->next_in_room;
        if ((victim = FIGHTING(ch)) == NULL) break;
	if (FIGHTING(rch) != NULL || !IS_AWAKE(rch)) continue;
	if ((IS_PLAYER (rch) || IS_AFFECTED (rch, AFF_CHARM))
	    && is_same_group (ch, rch)
	    && IS_SET (rch->pcdata->act2, PLR_ASSIST)
	    && IS_MOB (victim)
            && victim->position > POSITION_DEAD
            && victim->data_type != 50
	    && rch->position == POSITION_STANDING)
	  {
	    act ("You join the fight!", rch, NULL, rch, TO_CHAR);
	    act ("$n joins the fight!", rch, NULL, rch, TO_ROOM);
	    multi_hit (rch, victim, TYPE_UNDEFINED);
            if ((victim = FIGHTING(rch)) != NULL)
  	    set_fighting(rch,victim);
	  }

	if (IS_MOB (rch) && !IS_AFFECTED (rch, AFF_CHARM) && (rch->pIndexData == ch->pIndexData) &&LEVEL (rch) > 9 &&  can_see (rch, victim))
	  {
	    check_fgt (rch);
	    rch->fgt->fighting = victim;
	    NEW_POSITION(rch, POSITION_FIGHTING);
	    act ("$n CHARGES into battle!", rch, NULL, NULL, TO_ROOM);
	    multi_hit (rch, victim, TYPE_UNDEFINED);

	  }
	continue;
      }
  }
return;
}

void
do_circle (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *obj;
  int dt;
  SPELL_DATA *spl;
  DEFINE_COMMAND ("circle", do_circle, POSITION_FIGHTING, 0, LOG_NORMAL,
"This command allows you to circle behind an opponent (who isn't fighting you), and thrust a dagger in his/her/its back during combat.")
    check_fgt (ch);
  if ((spl = skill_lookup ("Circle", -1)) == NULL)
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }
  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't circle this soon after giving or recieving a bash.\n\r", ch);
      return;
    }
  if (IS_MOB (ch))
    return;
  if (ch->pcdata->tickcounts > 3)
    {
      send_to_char ("You are off-balance from your last maneuver; you can't circle at the moment.\n\r", ch);
      return;
    }
  if (ch->wait > 1)
    return;
  if (ch->position != POSITION_FIGHTING)
    return;
  if (FIGHTING (ch) == NULL)
    return;
  if (FIGHTING (FIGHTING (ch)) == NULL)
    return;
  check_same_side_pk(ch, FIGHTING(ch));
  if (FIGHTING (FIGHTING (ch)) == ch)
    {
      send_to_char ("You can't circle around behind the mob who is fighting you!\n\r", ch);
      return;
    }
    if (ch->pcdata->learned[!gsn_circle] < 3)
    {
      send_to_char ("You do not know how to circle an opponent!\n\r", ch);
      return;
    } 
  if ((obj = get_item_held (ch, ITEM_WEAPON)) == NULL)
    {
      send_to_char ("You need to wield a weapon.\n\r", ch);
      return;
    }
  dt = ((I_WEAPON *) obj->more)->attack_type;
  if (attack_table[dt].hit_type != TYPE_PIERCE || !IS_OBJ_STAT(obj, ITEM_PIERCE))
    {
      send_to_char ("You need to wield a piercing weapon.\n\r", ch);
      return;
    }
  if (FIGHTING (ch)->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("That person is entangled in mortal combat on the ground!!\n\r", ch);
      return;
    }
  check_ced (ch);
  check_ced (FIGHTING (ch));
  if (!(IS_IN_MELEE (ch, FIGHTING (ch))))
    {
      send_to_char ("You aren't on the front lines of that battle!\n\r", ch);
      return;
    }
  send_to_char ("You start to circle around your victim...\n\r", ch);
  ch->pcdata->tickcounts = spl->mana;   /*13; */
  NEW_POSITION(ch, POSITION_CIRCLE);
  return;
}

void
do_actual_circle (CHAR_DATA * ch, char *argy, bool used_flash)
{
  SINGLE_OBJECT *obj;
  int dt;
  CHAR_DATA *c;
  DESCRIPTOR_DATA *d;
  SPELL_DATA *spl;
  if (IS_MOB (ch))
    return;
  if (ch->wait > 1)
    return;
  if (FIGHTING (ch) == NULL)
    return;
  if (FIGHTING (FIGHTING (ch)) == NULL)
    return;
  if (ch->position != POSITION_CIRCLE && !used_flash)
    return;
  NEW_POSITION(ch, POSITION_STANDING);
  if (IS_PLAYER (ch) && number_range(1,8) == 3)
   {
     skill_gain (ch, gsn_circle, TRUE);
     skill_gain (ch, gsn_backstab, TRUE);
   }
  if ((spl = skill_lookup ("Circle", -1)) == NULL)
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }
  if (FIGHTING (FIGHTING (ch)) == ch && !used_flash)
    {
      send_to_char ("You can't circle around behind the mob who is fighting you!\n\r", ch);
      return;
    }
   if (ch->pcdata->learned[gsn_circle] < 2)
    {
      send_to_char ("You do not know how to circle an opponent!\n\r", ch);
      return;
    }
  if ((obj = get_item_held (ch, ITEM_WEAPON)) == NULL)
    {
      send_to_char ("You need to wield a weapon.\n\r", ch);
      return;
    }
  dt = ((I_WEAPON *) obj->more)->attack_type;
  if (attack_table[dt].hit_type != TYPE_PIERCE || !IS_OBJ_STAT(obj, ITEM_PIERCE))
    {
      send_to_char ("You need to wield a piercing weapon.\n\r", ch);
      return;
    }
  if (FIGHTING (ch)->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("That person is entangled in mortal combat on the ground!!\n\r", ch);
      return;
    }
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (!d->character || d->connected != CON_PLAYING)
        continue;
      c = d->character;
      if (FIGHTING (c) == FIGHTING (ch) && c->position == POSITION_CIRCLE && !used_flash)
        {
          act ("$B$4You bump into $N, who is also trying to circle at the same time!", ch, NULL, c, TO_CHAR);
          act ("$B$4$n bumps into $N, who are both trying to circle at the same time!  What fools!", ch, NULL, c, TO_NOTVICT);
          act ("$B$4$n bumps into you as you both crowd behind the same mob, trying to circle!", ch, NULL, c, TO_VICT);
          NEW_POSITION(c, POSITION_FIGHTING);
          free_temp (c);
          SUBHIT(c,4);
          SUBHIT(ch,5);
          return;
        }
    }
  if (IS_PLAYER(FIGHTING(ch)) && number_range(1,12) == 3 &&
       number_percent() < FIGHTING(ch)->pcdata->learned[gsn_awareness])
    {
      act("$n just tried to circle you!", ch, NULL, FIGHTING(ch), TO_VICT);
      act("You try to circle $N, but $E notices.", ch, NULL, FIGHTING(ch), TO_CHAR);
      skill_gain(FIGHTING(ch), gsn_awareness, TRUE);
      return;
    }
  if (number_range (0, 104) < ch->pcdata->learned[gsn_circle])
    {
      set_fighting (FIGHTING (ch), ch);
      set_fighting (ch, FIGHTING (ch));
      if (IS_PLAYER (ch))
        ch->pcdata->tickcounts = spl->casting_time;
      one_hit(ch, FIGHTING(ch), gsn_circle, obj);
      if (is_member(ch, GUILD_THIEFG) &&
	  is_member(ch, GUILD_ROGUE) &&
          FIGHTING(ch) != NULL
	  && FIGHTING(ch)->data_type != 50 &&
	  number_percent () < ((get_curr_dex(ch)-34)*2 + (IS_AUGMENTED(ch,AUG_HAND_SPD) ? 2 : 0)))
	one_hit (ch, FIGHTING(ch), gsn_circle, obj);
      return;
    }
  else
    send_to_char ("You failed to circle your opponent.\n\r", ch);
  return;
}

void
check_pkill (CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (CHALLENGE (ch) == 10 || (IN_BATTLE (ch)))
    return;
  return;
}

void
multi_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt)
{
  int chance;
  int gmembers;
  bool second;
  CHAR_DATA *vch;
  SINGLE_OBJECT *wield1;
  SINGLE_OBJECT *wield2;
  second = FALSE;
  CHECK_CHAR (ch);
  CHECK_CHAR (victim);
  check_fgt (ch);
  check_fgt (victim);
/* Not in same room */
  if (ch->in_room != victim->in_room)
    {
      ch->fgt->fighting=NULL;
      victim->fgt->fighting=NULL;
      ch->position=POSITION_STANDING;
      victim->position=POSITION_STANDING;
      return;
    }
  check_ced (ch);
  check_ced (victim);
  add_to_fighting_list (ch);
  add_to_fighting_list (victim);
  if (is_safe (ch, victim))
    {
      return;
    }
  if (IS_MOB (ch) && (ch->pIndexData->mobtype == MOB_DUMMY))
    {
      return;

  }


  check_same_side_pk(ch, victim);
  if (ch->position == POSITION_SLEEPING)
    {
    return;
    }
  if (victim == NULL || victim->data_type == 50)
    {
    return;
    }
  if (ch->position <= POSITION_STUNNED && ch->position >= 0)
    return;
  if (victim->position == POSITION_DEAD)
    {
      stop_fighting (ch, TRUE);
      ch->wait = 0;
      stop_fighting (victim, TRUE);
    }
  if (IS_MOB (victim) && IS_SET (victim->act, ACT_UNTOUCHABLE))
    {
      send_to_char ("Your physical attacks don't seem to hurt this creature!\n\r", ch);
      return;
    }
  wield1 = ((ch->hold1 != NULL && ch->hold1->pIndexData->item_type ==
	     ITEM_WEAPON) ? ch->hold1 : NULL);
  wield2 = ((ch->hold2 != NULL && ch->hold2->pIndexData->item_type ==
	     ITEM_WEAPON) ? ch->hold2 : NULL);
  if (wield2 && !wield1)
    {
      wield1 = wield2;
      wield2 = NULL;
    }

  if (IS_MOB (victim) && (victim->pIndexData->mobtype == MOB_GHOST))
    {
      bool can_hit_it;
      can_hit_it = FALSE;
      if (wield1 && (IS_OBJ_STAT (wield1, ITEM_GLOW) || IS_OBJ_STAT (wield1, ITEM_HUM)))
	can_hit_it = TRUE;
      else if (wield2 && (IS_OBJ_STAT (wield2, ITEM_GLOW) || IS_OBJ_STAT (wield2, ITEM_HUM)))
	can_hit_it = TRUE;
      if (is_member(ch, GUILD_MONK))
        can_hit_it = TRUE;
      if (!can_hit_it)
	{
	  send_to_char ("Your attack goes right through your victim as if it was not there!\n\r", ch);
	  return;
	}
    }


  /* Kill self capabilities here */
  if (pow.can_fight_self)
    {
      if (IS_PLAYER (ch) && ch->desc != NULL && FIGHTING (ch) == NULL)
	{
	  write_to_descriptor2 (ch->desc, "\n\r", 2);
	  ch->fgt->fighting = victim;
	}
      else
	{
	  check_fgt (ch);
	  ch->fgt->fighting = victim;
	}
    }
  if (FIGHTING (victim) == NULL)
    {
      ch->ced->attackers[0] = victim;
      victim->ced->attackers[0] = ch;
      victim->fgt->fighting = ch;
    }
  if (ch == FIGHTING (victim) && !(IS_IN_MELEE (ch, victim)))
    {
      /*victim is being hit, but not in melee?? */
      victim->ced->attackers[0] = ch;       /*Set him in position 1 attacking */
    }
  for (vch = victim->in_room->more->people; vch != NULL; vch = vch->next_in_room)
    {
      if (FIGHTING (vch) == victim)
	{
	  check_ced (vch);
	  if (!IS_IN_MELEE (vch, victim))
	    {
	      for (gmembers = 0; gmembers < 4; gmembers++)
		{
		  if (victim->ced->attackers[gmembers] == NULL ||
		      victim->ced->attackers[gmembers]->in_room != victim->in_room)
		    {
		      victim->ced->attackers[gmembers] = vch;
		      break;
		    }
		}
	    }
	}
    }

  if (!IS_IN_MELEE (ch, victim) && wield1 != NULL && !IS_OBJ_STAT(wield1, ITEM_POLEARM))
    {
      send_to_char ("\x1B[1;35m<-> (\x1B[32mYou are not within melee range\x1B[35m) <->\x1B[0;37m\n\r", ch);
      return;
    }
  if (ch->fgt->combat_delay_count <= -5)
    {
      ch->fgt->combat_delay_count--;
      if (ch->fgt->combat_delay_count < -8)
	ch->fgt->combat_delay_count = -1;
    }
  if (ch->fgt->combat_delay_count > 0)
    {
      ch->fgt->combat_delay_count--;
      return;
    }
  if (ch->position != POSITION_BASHED && ch->fgt->combat_delay_count == 0)
    {
      ch->fgt->combat_delay_count = -1;
      ch->fgt->ears = 2;
      send_to_char ("You scramble to your feet!\n\r", ch);
      act ("$n stands up quickly, trying not to get whacked again!", ch, NULL, NULL, TO_ROOM);
      NEW_POSITION(ch, POSITION_FIGHTING);
    }
  if (IS_MOB (ch) && ch->position == POSITION_BASHED && ch->fgt->combat_delay_count < 1)
    {
      ch->fgt->combat_delay_count = -5;
      ch->fgt->ears = 2;
      send_to_char ("You scramble to your feet!\n\r", ch);
      act ("$n stands up, trying not to get bashed down again!", ch, NULL, NULL, TO_ROOM);
      NEW_POSITION(ch, POSITION_FIGHTING);
    }
  if (ch->position == POSITION_BASHED)
    {
      update_pos (ch);
      return;
    }
  if (ch->fgt->combat_delay_count < 1 && ch->position == POSITION_STANDING)
    {
      NEW_POSITION(ch, POSITION_FIGHTING);
    }
  if (number_range (1, 40) == 3 && RIDING (ch) == NULL &&
      can_groundfight (ch) && can_groundfight (victim)
      && ch->hit < ch->max_hit/2)
    {
      if (number_range (1, 2) == 1 ||
	  FIGHTING (victim) != ch ||
	  !IS_SET (ch->ced->fight_ops, F_GROUND) ||
	  !IS_SET (victim->ced->fight_ops, F_GROUND) ||
	  LEVEL (ch) < 10 || LEVEL (victim) < 10 ||
	  victim->position == POSITION_GROUNDFIGHTING ||
	  ch->position == POSITION_GROUNDFIGHTING)
	{
	  act ("You slip on a pool of blood that has formed on the ground here!\n\rYou hit your head hard as your face meets the ground!", ch, NULL, victim, TO_CHAR);
	  act ("$n slips on a pool of blood and falls on $s face!", ch, NULL, victim, TO_ROOM);
	  SUBHIT(ch,(7 + (UMIN (10, LEVEL (ch) / 7))));
	  WAIT_STATE (ch, PULSE_VIOLENCE);
	  return;
	}
      else
	{
	  act ("You slip on some blood and $N falls on top of you!", ch, NULL, victim, TO_CHAR);
	  act ("$n slips on some blood and drags $N down too!", ch, NULL, victim, TO_NOTVICT);
	  act ("$n slips on a pool of blood on the ground, but as $n\n\rgoes down, $n grabs you, taking you down too!", ch, NULL, victim, TO_VICT);
	  act ("The fight is taken to the ground!", ch, NULL, victim, TO_ROOM);
	  act ("The fight is taken to the ground!", ch, NULL, victim, TO_CHAR);
	  NEW_POSITION(ch, POSITION_GROUNDFIGHTING);
	  NEW_POSITION(victim, POSITION_GROUNDFIGHTING);
	  WAIT_STATE (ch, PULSE_VIOLENCE);
	  WAIT_STATE (victim, PULSE_VIOLENCE);
	  return;
	}
    }
  one_hit (ch, victim, dt, wield1);
  if (IS_AFFECTED_EXTRA (ch, AFF_SPEED) && number_range(1,2) == 2)
      one_hit (ch, victim, dt, wield1);
  if (is_member(ch, GUILD_MONK))
  {
    if (number_range(1,4) == 2 && ch->pcdata->learned[gsn_unarmed] > 85)
      one_hit (ch, victim, dt, wield1);
    if (number_range(1,4) == 2 && ch->pcdata->stance != 2)
      one_hit (ch, victim, dt, wield1);
    if (number_range(1,4) == 2 && 
       (ch->pcdata->stance == 1 || ch->pcdata->stance == 4 || 
        ch->pcdata->stance == 7) )
      one_hit (ch, victim, dt, wield1);
    if (number_range(1,4) == 2 && ch->pcdata->stance == 4)
      one_hit (ch, victim, dt, wield1);
    if (number_range(1,4) == 2 && ch->pcdata->stance == 4)
      one_hit (ch, victim, dt, wield1);
  }    
  if (is_member(ch, GUILD_MONK) && number_range(1,40) == 2 && 
      IS_SET(ch->pcdata->elemental_alignment, ELEMENTAL_ALIGN_FIRE) &&
      victim->hit > 10 )
  {
      act ("$B$0Your fist unleashes a ball of $B$4fire $B$0at $N!", ch, NULL, victim, TO_CHAR);
      act ("$B$0A loose $B$4fireball $B$0leaps from $n's fist!", ch, NULL, victim, TO_VICT);
      act ("$B$0$n's fist explodes in $B$4fire $B$0at $N!", ch, NULL, victim, TO_NOTVICT);
      show_hitdam(gsn_shoot, "shot", 60, ch, victim);
      damage(ch, victim, 40, gsn_shoot);
  }
  if (is_member(ch, GUILD_MONK) && number_range(1,40) == 2 && 
      IS_SET(ch->pcdata->elemental_alignment, ELEMENTAL_ALIGN_EARTH) &&
      victim->hit > 10 )
  {
      act ("$B$0Your foot launches a $R$2huge rock $B$0at $N!", ch, NULL, victim, TO_CHAR);
      act ("$B$0A $R$2huge rock $B$0surges from $n's foot!", ch, NULL, victim, TO_VICT);
      act ("$B$0$n's foot kicks up a $R$2huge rock $B$0at $N!", ch, NULL, victim, TO_NOTVICT);
      show_hitdam(gsn_shoot, "shot", 60, ch, victim);
      damage(ch, victim, 40, gsn_shoot);
  }
  if (is_member(ch, GUILD_MONK) && number_range(1,40) == 2 && 
      IS_SET(ch->pcdata->elemental_alignment, ELEMENTAL_ALIGN_AIR) &&
      victim->hit > 10 )
  {
      act ("$B$0Your fist sends a tornado of $B$3air $B$0at $N!", ch, NULL, victim, TO_CHAR);
      act ("$B$0A tornado of $B$3air $B$0leaps from $n's fist!", ch, NULL, victim, TO_VICT);
      act ("$B$0$n's fist creates a tornado of $B$3air $B$0at $N!", ch, NULL, victim, TO_NOTVICT);
      show_hitdam(gsn_shoot, "shot", 60, ch, victim);
      damage(ch, victim, 40, gsn_shoot);
  }
  if (is_member(ch, GUILD_MONK) && number_range(1,40) == 2 && 
      IS_SET(ch->pcdata->elemental_alignment, ELEMENTAL_ALIGN_WATER) &&
      victim->hit > 10 )
  {
      act ("$B$0Your elbow strike summons a bolt of $B$1lightning$B$0!", ch, NULL, victim, TO_CHAR);
      act ("$B$0A bolt of $B$1lightning $B$0arcs from $n's elbow strike!", ch, NULL, victim, TO_VICT);
      act ("$B$0$n's elbow strike explodes in $B$1lightning$B$0!", ch, NULL, victim, TO_NOTVICT);
      show_hitdam(gsn_shoot, "shot", 60, ch, victim);
      damage(ch, victim, 40, gsn_shoot);
  }
  if (FIGHTING(ch) != victim)
    return;
  if (wield2 && IS_PLAYER(ch) &&
      number_percent() < ch->pcdata->learned[571]*3/5)
    {
      one_hit (ch, victim, dt, wield2);
      if (FIGHTING(ch) != victim)
        return;
      skill_gain(ch, 571, TRUE);
      if (is_member(ch, GUILD_WARRIOR) && is_member(ch,GUILD_BATTLEMASTER) && number_range(1,7) == 2)
	{
	  one_hit (ch, victim, dt, wield2);
	   if (FIGHTING(ch) != victim)
	     return;
	  skill_gain(ch, 571, TRUE);
	}
    }

  chance = IS_MOB (ch) ? LEVEL (ch) / 4 :
    ((ch->pcdata->learned[gsn_second_attack]*3/5)); /* JRAJRA were 4 */
  if (number_percent () < chance)
    {
      one_hit (ch, victim, dt, wield1);
      if (FIGHTING(ch) != victim)
       return;
      if (IS_PLAYER (ch))
	skill_gain (ch, gsn_second_attack, TRUE);
    }
  chance = (IS_MOB (ch) ? LEVEL (ch) / 5 :
    ch->pcdata->learned[gsn_third_attack]/2); /* JRAJRA was 4 */
  if (number_percent () < chance)
    {
      one_hit (ch, victim, dt, wield1);
      if (FIGHTING(ch) != victim)
        return;
      if (IS_PLAYER (ch))
	skill_gain (ch, gsn_third_attack, TRUE);
    }

  if (victim->position == POSITION_RESTING || victim->position == POSITION_SLEEPING || victim->position == POSITION_STANDING)
    {
      set_fighting (victim, ch);
    }
  return;
}


/* This is the new check parry/dodge/shieldblock code which comes in BEFORE
   all the damage and crap are calculated. */

bool
check_defense (CHAR_DATA * ch, CHAR_DATA * victim)
{
  int chance;
  CHAR_DATA *fch;
  int number = 0;
  check_ced(victim);
  if (victim->move <= 6)
    return FALSE;
  if (!IS_AWAKE (victim) || victim->position <= POSITION_STUNNED)
    return FALSE;
  if ((IS_AFFECTED (ch, AFF_INVISIBLE) &&
       !IS_AFFECTED (victim, AFF_DETECT_INVIS)) || IS_AFFECTED(victim, AFF_BLIND))
    {
      if(number_range(1,10) == 3)
	act ("You try to defend yourself, but you can't see your opponent!", victim, NULL, ch, TO_CHAR);
      return FALSE;
    }
  for (fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
    {
      if (FIGHTING(fch) == victim)
	number++;
    }
  if (number < 1)
    number = 1;
  if ((victim->hold1 != NULL && victim->hold1->pIndexData->item_type == ITEM_WEAPON) ||
      (victim->hold2 != NULL && victim->hold2->pIndexData->item_type == ITEM_WEAPON))
    {
      if (IS_MOB (victim))
	chance = (LEVEL (victim)*3/2)/number;
      else
	{
	  chance = (victim->pcdata->learned[gsn_parry]+
		    (is_member (victim, GUILD_WARRIOR) ? 30 : 0)+
		    (is_member (victim, GUILD_BATTLEMASTER) ? 30 : 0)+
		    (IS_AUGMENTED(victim, AUG_BODY_POWER) ? 12 : 0)+
		    (IS_AUGMENTED(victim, AUG_ARM_POWER) ? 12 : 0)+
		    (IS_AUGMENTED(victim, AUG_HAND_SPD) ? 100 : 0)+
		    (IS_AUGMENTED(victim, AUG_HAND_POWER) ? 12 : 0))/number;
	}
      if (number_range (1, 900) < UMIN(170, chance))
	{
	  if (number_range (1, 2) == 2)
	    {
	    }
	  else
	    {
	      act ("You forcefully deflect $n's blade, parrying the attack.", ch, NULL, victim, TO_VICT);
	      act ("$N forcefully deflects your blade, parrying the attack.", ch, NULL, victim, TO_CHAR);
	      act ("$N deflects $n's attack with his weapon.", ch, NULL, victim, TO_NOTVICT);
	    }

	  if (number_range (1, 3) == 2)
	    skill_gain (victim, 566, TRUE);
	  SUBMOVE(victim,1);
	  return TRUE;
	}
    }
  /* Failed Parry, now check dodge. */

  if (IS_MOB (victim))
    chance = (LEVEL (victim) * 3/2) / (number);
  else
    {
      chance = ((victim->pcdata->learned[gsn_dodge]) +
		(is_member (victim, GUILD_THIEFG) ? 44 : 0) +
		(IS_AUGMENTED(victim, AUG_LEG_SPD) ? 30 : 0) +
		(IS_AUGMENTED(victim, AUG_FOOT_SPD) ? 30 : 0))/number;
      chance -= ((victim->pcdata->carry_weight/get_curr_str(victim)));
      if (is_member(ch, GUILD_MONK))
        {
          if (ch->pcdata->stance == 1)
            chance -= 40;
          else if (ch->pcdata->stance == 4)
            chance -= 20;
          else if (ch->pcdata->stance == 5)
            chance += 20;
          else if (ch->pcdata->stance == 8)
            chance += 40;
        }
/* This takes into account the *10 from the new weight code and the
  desire to subtract 10 points for each multi of str carried...*/
    }

  if (number_range (0, 400) < UMIN(180, chance))
    {
      if (victim->position != POSITION_BASHED)
        {
          if (number_range (1, 2) == 2)
            {
              act ("You step aside, dodging $n's attack.", ch, NULL, victim, TO_VICT);
              act ("$N quickly steps aside, dodging your attack.", ch, NULL, victim, TO_CHAR);
              act ("$N steps aside, dodging $n's attack.", ch, NULL, victim, TO_NOTVICT);
            }
          else
            {
              if (number_range (1, 2) == 2)
                {
                  act ("You duck right, dodging $n's attack.", ch, NULL, victim, TO_VICT);
                  act ("$N ducks right, dodging your attack.", ch, NULL, victim, TO_CHAR);
                  act ("$N ducks right, dodging $n's attack.", ch, NULL, victim, TO_NOTVICT);
                }
              else
                {
                  act ("You duck left, dodging $n's attack.", ch, NULL, victim, TO_VICT);
                  act ("$N ducks left, dodging your attack.", ch, NULL, victim, TO_CHAR);
                  act ("$N ducks left, dodging $n's attack.", ch, NULL, victim, TO_NOTVICT);
                }
            }
        }
      else
        {
          act ("You roll out of the way of $n's attack.", ch, NULL, victim, TO_VICT);
          act ("$N rolls out of the way of your attack.", ch, NULL, victim, TO_CHAR);
          act ("$N rolls out of the way of $n's attack.", ch, NULL, victim, TO_NOTVICT);
        }
      if (number_range (1, 4) == 2)
        skill_gain (victim, 555, TRUE);
      SUBMOVE(victim,1);
      return TRUE;
    }

  /* Failed dodge, now check evade. */

if (IS_PLAYER(victim) && is_member(victim, GUILD_MONK))
{
  if (IS_MOB (victim))
    chance = (LEVEL (victim) * 3/2) / (number);
  else
    {
      chance = ((victim->pcdata->learned[gsn_evade] - 68) +
		(IS_AUGMENTED(victim, AUG_LEG_SPD) ? 5 : 0) +
		(IS_AUGMENTED(victim, AUG_FOOT_SPD) ? 5 : 0) +
                3*((get_curr_str(victim) + get_curr_dex(victim) + get_curr_wis(victim))/3 - 20));
      if (ch->pcdata->stance == 1)
        chance -= 15;
      else if (ch->pcdata->stance == 4)
        chance -= 10;
      else if (ch->pcdata->stance == 5)
        chance += 10;
      else if (ch->pcdata->stance == 8)
        chance += 15;
    }

  if (number_range (0, 100) < chance)
    {
      if (victim->position != POSITION_BASHED)
        {
          if (number_range (1, 2) == 2)
            {
              act ("You bend backward, evading $n's attack.", ch, NULL, victim, TO_VICT);
              act ("$N quickly bends backward, evading your attack.", ch, NULL, victim, TO_CHAR);
              act ("$N bends backward, evading $n's attack.", ch, NULL, victim, TO_NOTVICT);
            }
          else
            {
              if (number_range (1, 2) == 2)
                {
                  act ("You leap high, evading $n's attack.", ch, NULL, victim, TO_VICT);
                  act ("$N leaps high, evading your attack.", ch, NULL, victim, TO_CHAR);
                  act ("$N leaps high, evading $n's attack.", ch, NULL, victim, TO_NOTVICT);
                }
              else
                {
                  act ("You duck low, evading $n's attack.", ch, NULL, victim, TO_VICT);
                  act ("$N duck low, evading your attack.", ch, NULL, victim, TO_CHAR);
                  act ("$N duck low, evading $n's attack.", ch, NULL, victim, TO_NOTVICT);
                }
            }
        }
      else
        {
          act ("Even bashed, you manage to evade $n's attack.", ch, NULL, victim, TO_VICT);
          act ("$N evades your attack even while bashed.", ch, NULL, victim, TO_CHAR);
          act ("$N evades $n's attack, even while bashed.", ch, NULL, victim, TO_NOTVICT);
        }
      if (number_range (1, 4) == 2)
        skill_gain (victim, 560, TRUE);
      SUBMOVE(victim,1);
      return TRUE;
    }
} /* END EVADE CHECK */

  /* Failed dodge, now check shield block */

  if (IS_PLAYER(victim)  && IS_SET(victim->act, PLR_HAS_SHIELD))
    {
      chance = (victim->pcdata->learned[gsn_shield_block] +
		(IS_AUGMENTED(victim, AUG_ARM_POWER) ? 20 : 0)+
		(is_member(victim, GUILD_RANGER) ? 50 : 0)+
		(IS_AUGMENTED(victim, AUG_BODY_POWER) ? 20 : 0))/number;

      if (number_range (1, 950) < chance)
	{
	  act ("You block $n's attack with your shield.", ch, NULL, victim, TO_VICT);
	  act ("$N blocks your attack with $S shield.", ch, NULL, victim, TO_CHAR);
	  act ("$N blocks $n's attack with $S shield.", ch, NULL, victim, TO_NOTVICT);
	  skill_gain (victim, gsn_shield_block, TRUE);
	  if (number_range (1, 15) == 2)
	    {
	      int dam;
	      act ("You slam into $n with your shield.", ch, NULL, victim, TO_VICT);
	      act ("$N slams into you with $s shield.", ch, NULL, victim, TO_CHAR);
	      act ("$N slams $S shield into $n's body.", ch, NULL, victim, TO_NOTVICT);
	      dam = ((get_curr_str (ch)) / (number_range (2, 5)));
	      SUBHIT(victim,dam);
	      if (victim->hit < 1)
		victim->hit = 1;
	    }
	  return TRUE;
	}
    }
  if ((IS_MOB(victim) && LEVEL(victim) > IMM_LEVEL) || (IS_PLAYER(victim)))
    {
      int chance = 0;
      if (IS_PLAYER(victim))
	{
	  if(!IS_SET(victim->pcdata->act2, PLR_AUTOBLINK) || victim->pcdata->n_mana < 5)
	    return FALSE;

	  chance = UMAX(0, (get_curr_int(victim)+get_curr_wis(victim) - 42));
	  if (is_member(victim, GUILD_WIZARD)) chance += 3;
	  if (is_member(victim, GUILD_HEALER)) chance += 3;
	  if (is_member(victim, GUILD_CONJURER)) chance += 3;
	  if (is_member(victim, GUILD_MYSTIC)) chance += 3;
	  chance *=  UMAX(0, victim->pcdata->learned[gsn_blink]/8);
	}
      else
	{
	  if (victim->pIndexData->opt != NULL)
	    chance = LEVEL(victim)/2;
	}
      if (number_range(1,900) < UMIN(320, chance))
	{
	  act ("$B$3You blink out of the way of $n's attack.$R$7", ch, NULL, victim, TO_VICT);
	  act ("$B$3$N blinks out of the way of your attack.$R$7", ch, NULL, victim, TO_CHAR);
	  act ("$B$3$N blinks out of the way of $n's attack.$R$7", ch, NULL, victim, TO_NOTVICT);
	  if (IS_PLAYER(victim))
	    {
	      victim->pcdata->n_mana--;
	      if (number_range(1,37) == 3)
		skill_gain (victim, gsn_blink, TRUE);
	    }
	  return TRUE;
	}
    }
  return FALSE;
}




int
addoffense (CHAR_DATA * ch, SINGLE_OBJECT * obj)
{
  if (IS_MOB (ch))
    return 0;
  if (obj == NULL || obj->pIndexData->item_type != ITEM_WEAPON) return 0;
  if (IS_SET (obj->extra_flags, ITEM_POLEARM))
    {
      skill_gain(ch, gsn_polearm, TRUE);
      return (ch->pcdata->learned[gsn_polearm] == -100 ? -30 : (-20+ch->pcdata->learned[gsn_polearm] / 7));
    }
  else if (IS_SET (obj->extra_flags, ITEM_PIERCE))
    {
      skill_gain (ch, gsn_pierce, TRUE);
      return (ch->pcdata->learned[gsn_pierce] == -100 ? 0 : ch->pcdata->learned[gsn_pierce] / 4);
    }
  else if (IS_SET (obj->extra_flags, ITEM_CONCUSSION))
    {
      skill_gain (ch, gsn_concussion, TRUE);
      return (ch->pcdata->learned[gsn_concussion] == -100 ? 0 : ch->pcdata->learned[gsn_concussion] / 5);
    }
  else if (IS_SET (obj->extra_flags, ITEM_WHIP))
    {
      skill_gain (ch, gsn_whip, TRUE);
      return (ch->pcdata->learned[gsn_whip] == -100 ? 0 : ch->pcdata->learned[gsn_whip] / 4);
    }
  else if (ch->pcdata->learned[gsn_slashing] == -100)
    return 0;
  else if (IS_SET (obj->extra_flags, ITEM_HEAVY_SLASH))
    {
      skill_gain (ch, gsn_slashing, TRUE);
      if (get_curr_str (ch) < 11)
        return -3;
      if (get_curr_str (ch) < 16)
        return 0;
      if (get_curr_str (ch) < 18)
        return (ch->pcdata->learned[gsn_slashing] / 7);
      if (get_curr_str (ch) < 21)
        return (ch->pcdata->learned[gsn_slashing] / 6);
      if (get_curr_str (ch) < 24)
        return (ch->pcdata->learned[gsn_slashing] / 5);
      else
        return (ch->pcdata->learned[gsn_slashing] / 4);
    }
  else if (IS_SET (obj->extra_flags, ITEM_LIGHT_SLASH))
    {
      skill_gain (ch, gsn_slashing, TRUE);
      return ((ch->pcdata->learned[gsn_slashing] / 4) + ((get_curr_str (ch) - 5) / 2));
    }
  else if (IS_SET (obj->extra_flags, ITEM_SLASHING))
    {
      skill_gain (ch, gsn_slashing, TRUE);
      return (ch->pcdata->learned[gsn_slashing] / 5);
    }
  return 0;
}
int
addoffense2 (CHAR_DATA * ch, SINGLE_OBJECT * obj)
{
  if (IS_MOB (ch))
    return 0;
  if (obj == NULL || obj->pIndexData->item_type != ITEM_WEAPON) return 0;
  if (IS_SET (obj->extra_flags, ITEM_POLEARM))
    return (ch->pcdata->learned[gsn_polearm] == -100 ? -30 : (-20+ch->pcdata->learned[gsn_polearm] / 7));
  else if (IS_SET (obj->extra_flags, ITEM_PIERCE))
    return (ch->pcdata->learned[gsn_pierce] == -100 ? 0 : ch->pcdata->learned[gsn_pierce] / 4);
  else if (IS_SET (obj->extra_flags, ITEM_CONCUSSION))
    return (ch->pcdata->learned[gsn_concussion] == -100 ? 0 : ch->pcdata->learned[gsn_concussion] / 5);
  else if (IS_SET (obj->extra_flags, ITEM_WHIP))
    return (ch->pcdata->learned[gsn_whip] == -100 ? 0 : ch->pcdata->learned[gsn_whip] / 4);
  else if (ch->pcdata->learned[gsn_slashing] == -100)
    return 0;
  else if (IS_SET (obj->extra_flags, ITEM_HEAVY_SLASH))
    {
      if (get_curr_str (ch) < 11)
        return -3;
      if (get_curr_str (ch) < 16)
        return 0;
      if (get_curr_str (ch) < 18)
        return (ch->pcdata->learned[gsn_slashing] / 7);
      if (get_curr_str (ch) < 21)
        return (ch->pcdata->learned[gsn_slashing] / 6);
      if (get_curr_str (ch) < 24)
        return (ch->pcdata->learned[gsn_slashing] / 5);
      else
        return (ch->pcdata->learned[gsn_slashing] / 4);
    }
  else if (IS_SET (obj->extra_flags, ITEM_LIGHT_SLASH))

    return ((ch->pcdata->learned[gsn_slashing] / 4) + ((get_curr_str (ch) - 5) / 2));
  else if (IS_SET (obj->extra_flags, ITEM_SLASHING))

    return (ch->pcdata->learned[gsn_slashing] / 5);
  return 0;
}

void
one_hit (CHAR_DATA * ch, CHAR_DATA * victim, int dt, SINGLE_OBJECT * wield)
{
  SINGLE_OBJECT *armor;
  int to_hit;
  int bonushit;
  int dam = 0;
  int damm = 0;
  int iWear;
  int hit_pos;
  int minShieldDamage = 1;
  int maxShieldDamage = 1;
  bonushit = 0;
  CHECK_CHAR (ch);
  CHECK_CHAR (victim);

  check_ced (ch);
  check_ced (victim);
  check_fgt (ch);
  check_fgt (victim);

  /* dont hit if you are a dummy mob */
  if (IS_MOB (ch) && (ch->pIndexData->mobtype == MOB_DUMMY)) {
    return;
  }
/*
  if(IS_MOB (ch)) send_to_char("Its a mob\n\r",victim);
  if(ch->pIndexData->mobtype == MOB_DUMMY) send_to_char("Its a dummy\n\r",victim);
*/
  if (ch->in_room != victim->in_room || victim->position == POSITION_DEAD)
    {
      ch->fgt->fighting=NULL;
      victim->fgt->fighting=NULL;
      ch->position=POSITION_STANDING;
      victim->position=POSITION_STANDING;
      return;
    }
  if(IS_AFFECTED_EXTRA(ch,AFF_PARALYZE))
    {
      send_to_char("YOU...CAN'T...MOVE...\n\r", ch);
      return;
    }
  hit_pos = number_range (1, 4);
  if (ch && victim && IS_MOB (ch) && IS_PLAYER (victim) &&
      (!HUNTING (ch) || (HUNTING (ch) && !is_number (HUNTING (ch)))))

    {
      if (strlen (NAME (victim)) < 16)
	{
	  if (HUNTING (ch) != NULL)
	    {
	      free_string (ch->fgt->hunting);
	      ch->fgt->hunting = NULL;
	    }
	  ch->fgt->hunting = str_dup (NAME (victim));
	}
    }
  if (MOUNTED_BY (victim) == ch)
    {
      ch->fgt->riding = NULL;
      victim->fgt->mounted_by = NULL;
    }
  /*
   * Figure out the type of damage message.
   */
  if (victim->position == POSITION_GROUNDFIGHTING && number_range(1,3) == 1)
    {
      act ("$N is groundfighting and you can't find a line of attack.", ch, NULL, victim, TO_CHAR_SPAM);
      return;
    }

  if (dt != gsn_backstab && dt != gsn_circle && dt != gsn_impale && dt != gsn_charge)
    {
      /* ------------------------------------------- */
      /*       Do magical shield damage here         */
      /* ------------------------------------------- */
      if (IS_AFFECTED_EXTRA(victim, AFF_FIRESHIELD) && number_range(1,10) == 1)
	{
          minShieldDamage = 1;
	  maxShieldDamage = (LEVEL(ch) * 2)/2;
          if(minShieldDamage >= maxShieldDamage)
	    minShieldDamage = maxShieldDamage;
          damm = number_range(minShieldDamage,maxShieldDamage);

	  /* cap damage here */
	  if(damm >= 50)
          {
	    if(IS_MOB(ch))
	      damm = 25;
            else
	      damm = 50;
	  }

          if (IS_AFFECTED_EXTRA(ch, AFF_PROT_FIRE))
            damm = damm/2;
	  if (IS_AFFECTED_EXTRA(ch, AFF_MINDSHIELD))
	    damm = damm/4;
	  if (IS_AUGMENTED(ch, AUG_MENTAL_ARMOR))
	    damm = damm/5;
          if (damm < 1) damm = 1;
          if (ch-> hit > dam)
            {
              SUBHIT(ch, damm);
              update_pos (ch);
	      dam_message (victim,ch, damm, gsn_fireshield, 0);
            }
	}
      if(number_range(1,2) == 2)
	if (check_defense(ch, victim)) return;
    }
  if (dt == TYPE_UNDEFINED)
    {
      dt = TYPE_HIT;
      if (wield != NULL && wield->pIndexData->item_type == ITEM_WEAPON)
        {
	  dt += ((I_WEAPON *)wield->more)->attack_type;
	}
    }

  if (wield && wield->pIndexData->item_type != ITEM_WEAPON)
    wield = NULL;
  else
    {
      if (IS_PLAYER (ch))
	bonushit = addoffense (ch, wield);
    }
  to_hit = GET_HITROLL(ch) + (bonushit*4/3) +
    (IS_MOB(ch) ? ch->pIndexData->hitroll : (ch->pcdata->learned[gsn_accuracy] > 0 ? ch->pcdata->learned[gsn_accuracy]/8 : 0));
  to_hit -= LEVEL(victim)/4 - get_curr_dex(victim);
  if (IS_PLAYER(victim))
    {
      to_hit -= ((!IS_SET (victim->ced->fight_ops, F_AGGRESSIVE) ? 8 : 0) +
		 (!IS_SET (victim->ced->fight_ops, F_POWER) ? 8 : 0) +
		 (IS_AUGMENTED(victim, AUG_LEG_SPD) ? 6 : 0) +
		 (IS_AUGMENTED(victim, AUG_FOOT_SPD) ? 6 : 0));
    }
  if (to_hit > 80) to_hit -= (to_hit-80)/2;
  if (number_percent() >= to_hit && number_range (0, 9) >= 3 &&
      dt != gsn_backstab && dt != gsn_circle && IS_AWAKE (victim)
      && dt != gsn_impale && dt != gsn_charge) {
      /* Shit! Missed! */
      ghit_pos = hit_pos;
      damage (ch, victim, 0, dt);
      ghit_pos = 0;
      return;
    }

  if (IS_MOB (ch))
    {
      if (wield != NULL && wield->pIndexData->item_type == ITEM_WEAPON)
	dam = (dice (FIRSTDICE (wield), SECONDDICE (wield)));
      dam += number_range (LEVEL (ch) / 10, (LEVEL (ch)/5));
      dam += ch->pIndexData->damroll/2;
      if (dam <= 0)
	dam = 1;
      if (dam >= LEVEL(ch))
	dam -= number_range (0, (dam - LEVEL(ch)));
      /*Prevent LOW level mobs from doing caps damage to newbies here*/
      if (LEVEL(ch) < 50 && dam >= LEVEL(ch))
	dam = number_range (LEVEL(ch)/4, LEVEL(ch)/2);
    }
  else
    {
      if (wield == NULL && is_member(ch, GUILD_MONK))
        dam = dice (4,4);
      if (wield != NULL && wield->pIndexData->item_type == ITEM_WEAPON)
	{
          if (is_member(ch, GUILD_MONK))
            return;
	  dam = dice (FIRSTDICE (wield), SECONDDICE (wield));
	  if (is_member(ch, GUILD_TINKER) && IS_OBJ_STAT(wield, ITEM_CONCUSSION))
	    dam += number_range(0,1);
	  if (is_member(ch, GUILD_RANGER) && (IS_OBJ_STAT(wield, ITEM_SLASHING) || IS_OBJ_STAT(wield, ITEM_HEAVY_SLASH) || IS_OBJ_STAT(wield, ITEM_LIGHT_SLASH)))

	    dam += number_range(0,1);
	  if (is_member(ch, GUILD_THIEFG) && IS_OBJ_STAT(wield, ITEM_PIERCE))
	    dam += number_range(0,2);
	  if (is_member(ch, GUILD_ROGUE) && IS_OBJ_STAT(wield, ITEM_PIERCE))
	    dam += number_range(0,2);
	}
      else
	{
	  dam = number_range(0, (1+LEVEL(ch)/15));

	}
    }
   if (GET_DAMROLL(ch) > 0)
       {
          dam += number_range(0, GET_DAMROLL(ch)/5);
	}
  if (dt != gsn_circle && dt != gsn_backstab)
    {
      int b4 = dam;
      int aftr;
      int dif;
      int absorbed = 0;
      bool tinker = is_member(ch, GUILD_TINKER);
      bool conc = FALSE;
      bool power = FALSE;
      if (is_member(ch, GUILD_MONK) && ch->pcdata->stance == 7
          && number_range(1,3) == 3)
        power = TRUE;
      if (IS_AUGMENTED(ch, AUG_BODY_POWER)) dam += number_range(0,1);
      if (IS_AUGMENTED(ch, AUG_ARM_POWER)) dam += number_range(0,1);
      if (IS_AUGMENTED(ch, AUG_HAND_POWER)) dam += number_range(0,1);
      if (is_member(ch, GUILD_WARRIOR)) dam += number_range(0,1);
      if (is_member(ch, GUILD_BATTLEMASTER)) dam += number_range(0,1);
      if (is_member(ch, GUILD_MONK))
      {
        int monkbonus = ch->pcdata->learned[gsn_unarmed] - 68;
        if (monkbonus < 0)
          monkbonus = 0;
        monkbonus /= 8;
        dam += number_range(0,monkbonus);
        if (number_range(1,5) == 1)
          skill_gain(ch, gsn_unarmed, TRUE);
        if (ch->pcdata->stance == 3)
          dam += number_range(1, 3);
        else if (ch->pcdata->stance == 6)
          dam += number_range(3, 5);
        else if (ch->pcdata->stance == 7)
          dam += number_range(4, 8);
        else if (ch->pcdata->stance == 2)
          dam -= number_range(3, 6);
        else if (ch->pcdata->stance == 5)
          dam -= number_range(1, 3);
      }

  if (is_member(ch, GUILD_MONK) && IS_SET(ch->act, PLR_HAS_SHIELD))
    dam = 0;

  if (IS_PLAYER(ch) && is_member(ch, GUILD_MONK))
  {
  int opp_align = 0;
  int same_align = 0;
  CHAR_DATA *fch;
  for (fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
    {
      if (IS_PLAYER(fch))
      {
        if (ch->pcdata->alignment == fch->pcdata->alignment)
          same_align += 1;
        else if (fch->pcdata->alignment > 0)
          opp_align += 1;
      }
    }
    dif = opp_align - same_align;
    if (opp_align < 2)
      dif = 0;
    if (dif == 2) 
      dam+= 8;
    else if (dif == 3)
      dam+= 15;
    else if (dif == 4)
      dam+= 20;
    else if (dif > 5)
      dam+= 29;
  } /* END OF MONK OUTNUMBERED STUFF */

      if (ch->pcdata->learned[gsn_enhanced_damage] > 0)
	{
	  int roll = 0;
	  roll = number_range(0, ch->pcdata->learned[gsn_enhanced_damage]);
	  if (roll <= 2) roll = 2;
	  dam += (dam *  roll)/ 390;
	  if (number_range (1, 15) == 10)
	    {
	      skill_gain (ch, gsn_enhanced_damage, TRUE);
	      skill_gain (ch, gsn_accuracy, TRUE);
	    }
	}

      if (wield && wield->pIndexData->item_type == ITEM_WEAPON)
	{
	  if (IS_OBJ_STAT(wield, ITEM_POWER))
	    power = TRUE;
	  if (IS_OBJ_STAT(wield, ITEM_CONCUSSION))
	    conc = TRUE;
        }
      aftr = new_damage (victim, dam, hit_pos, dt);
      dif = b4 - aftr;
      dam -= dif;
      if ((tinker && conc) || power)
	dam += dif/4;
      if (IS_PLAYER (ch) && dif > 4 && number_range (1, 100) < ch->pcdata->learned[gsn_penetration])
	{
	  if (number_range (1, 5) == 2)
	    skill_gain (ch, gsn_penetration, TRUE);
	  dam += dif/4;
	}
	  if (IS_PLAYER(victim) && !strcmp(race_info[victim->pcdata->race].name, "gargoyle"))
	    absorbed += number_range(0, 48);
	  if (IS_AUGMENTED(victim, AUG_LEG_PLATING))
	    absorbed+=number_range(0,8);
	  if (IS_AUGMENTED(victim, AUG_FOOT_PLATING))
	    absorbed+=number_range(0,8);
	  if (IS_AUGMENTED(victim, AUG_ARM_PLATING))
	    absorbed+=number_range(0,8);
	  if (IS_AUGMENTED(victim,AUG_HEAD_PLATING))
	    absorbed+=number_range(0,8);
	  if (IS_AUGMENTED(victim, AUG_BODY_PLATING))
	    absorbed+=number_range(0,8);
          if (is_member(victim, GUILD_MONK))
            {
              if (victim->pcdata->stance == 1)
                absorbed -=number_range(6,12);
              else if (victim->pcdata->stance == 2)
                absorbed +=number_range(6,12);
              else if (victim->pcdata->stance == 3)
                absorbed -=number_range(6,12);
              else if (victim->pcdata->stance == 4)
                absorbed -=number_range(12,18);
              else if (victim->pcdata->stance == 5)
                absorbed +=number_range(6,12);
              else if (victim->pcdata->stance == 6)
                absorbed -=number_range(12,18);
              else if (victim->pcdata->stance == 8)
                absorbed +=number_range(18,24);
            }
	  if (IS_PLAYER(victim) && victim->pcdata->learned[gsn_resilience] > 7 && !is_member(victim, GUILD_WIZARD)
             && (victim->pcdata->stance == 1 || victim->pcdata->stance == 
                 5 || victim->pcdata->stance == 8))
	    {
	      absorbed +=(victim->pcdata->learned[gsn_resilience]*3/4);
	      if (IS_MOB(ch))
	        absorbed -= number_range((victim->pcdata->learned[gsn_resilience]*1/4), (victim->pcdata->learned[gsn_resilience]*1/2));

	      if (number_range(1,10) == 3)
		skill_gain(victim, gsn_resilience, TRUE);
	    }
	  if (DIFF_ALIGN(ch, victim) && IS_AFFECTED(victim, AFF_PROT_ALIGN))
	      absorbed+= number_range(12,25);
	  if (IS_AFFECTED (victim, AFF_SANCTUARY))
	  {
	    absorbed += number_range (20, 25);
	    if (is_member(ch, GUILD_WARRIOR) || is_member (ch,GUILD_BATTLEMASTER))
	    	absorbed -= number_range (1, 3);
	  }
	  if (IS_AFFECTED (victim, AFF_PROTECT))
	  {
	    absorbed += number_range(10,20);
	    if (is_member(ch, GUILD_WARRIOR) && is_member (ch,GUILD_BATTLEMASTER))
		absorbed -= number_range (3, 7);
	  }
	  if (conc) absorbed = (absorbed * 4)/5;
	  if (power) absorbed = (absorbed * 2)/5;
	  if (absorbed > 80)
	    absorbed = 80;
 	  dam = ((100-absorbed)*dam)/100;

    }
  if (dam < 0)
    dam = 0;
  if (ch->in_room->sector_type == SECT_ASTRAL)
    dam/=2;

  /* ------------------------------------------------------------------ */
  /*                        Critical Hit ?                              */
  /* ------------------------------------------------------------------ */
  if (number_range(1,75) == 7 && dt != gsn_backstab && dt != gsn_circle)
    if (wield && wield->pIndexData->item_type == ITEM_WEAPON)
      {
	if (IS_PLAYER(ch) && IS_OBJ_STAT(wield, ITEM_PIERCE)
	    && IS_AUGMENTED(ch, AUG_HAND_SPD) && get_stat(ch, DEX) > 27
	    && check_skill(ch, gsn_critical))
	  {
	    dam += number_range(2,5);
	    dam*=8;
           dam = dam * (race_info[ch->pcdata->race].backstab_percent +
              align_info[ch->pcdata->alignment].backstab_percent)/100;
	    act ("\x1b[1;33mCritical \x1b[1;31mHit!!!!", ch, NULL,
		 NULL, TO_ROOM);
	    act ("\x1b[1;33mCritical \x1b[1;31mHit!!!!", ch, NULL,
		 NULL, TO_CHAR);
	    if (number_range(1,20) == 2)
	      skill_gain(ch, gsn_critical, TRUE);
	  }
      }





  if (number_range(1,38) == 2 && ch->position != POSITION_BASHED && victim->position != POSITION_BASHED)
    {
      if (IS_AFFECTED(ch, AFF_FLYING) && !IS_AFFECTED(victim, AFF_FLYING))
	{
	  act ("$n swoops down and attacks $N!", ch, NULL, victim, TO_NOTVICT);
	  act ("$n swoops down from the air and attacks you!", ch, NULL, victim, TO_VICT);
	  act ("You swoop down from the air and attack $N!", ch, NULL, victim, TO_CHAR);
	  dam += 2;
	  if (number_range (1, 12) == 5)
	    {
	      if (number_range (1, 2) == 1)
		{
		  act ("$N catches $n with an elbow while $e is approaching!", ch, NULL, victim, TO_NOTVICT);
		  act ("You catch $n with an elbow as $e approaches!", ch, NULL, victim, TO_VICT);
		  act ("$N smashes $S elbow into your face as you swoop down!", ch, NULL, victim, TO_CHAR);
		}
	      else
		{
		  act ("$N jumps out of the way and $n smashes into the ground!", ch, NULL, victim, TO_NOTVICT);
		  act ("You jump out of the way and $n smashes into the ground!", ch, NULL, victim, TO_VICT);
		  act ("$N steps out of the way and you smash into the ground!", ch, NULL, victim, TO_CHAR);
		}
	      if (ch->hit > 7)
		SUBHIT(ch,7);

	      return;
	    }
	}
      else if (!IS_AFFECTED(ch, AFF_FLYING) && IS_AFFECTED(victim, AFF_FLYING))
	{
	  /*hitter not flying, victim flying */
	  act ("$N quickly flies out of the way, dodging your attack!", ch, NULL, victim, TO_CHAR);
	  act ("$N flies out of $n's attack path.", ch, NULL, victim, TO_NOTVICT);
	  act ("You fly out of the path of $n's attack.", ch, NULL, victim, TO_VICT);
	  return;
	}
      else if (IS_AFFECTED(ch,AFF_FLYING) && IS_AFFECTED(victim, AFF_FLYING))
	{
	  /*both hitter and victim flying */
	  act ("$N and $n get tangled and smash into the ground!", ch, NULL, victim, TO_NOTVICT);
	  act ("You smash into the ground as you struggle with $n!", ch, NULL, victim, TO_VICT);
	  act ("You smash into the ground as you struggle with $N!", ch, NULL, victim, TO_CHAR);
	  if (ch->hit > 10)
	    {
	      SUBHIT(ch,9);
	      SUBHIT(victim,9);
	    }
	  return;
	}
    }
  if (!IS_AWAKE (victim))  dam *= 3;
  if (victim->position == POSITION_RESTING) dam = (dam * 3) / 2;

  if (wield != NULL  && wield->pIndexData->extra_flags >= ITEM_ELEMENTAL)
    {
      if(IS_OBJ_STAT(wield, ITEM_ELEMENTAL) && number_range(1,25) == 2)
	{
	  switch (number_range(1,5))
	    {
	    case 1:
	      spell_fire_breath (LEVEL(ch), ch, victim);
	      break;
	    case 2:
	      spell_acid_breath (LEVEL(ch), ch, victim);
	      break;
	    case 3:
	      spell_ice_breath (LEVEL(ch), ch, victim);
	      break;
	    case 4:
	      spell_shock_breath (LEVEL(ch), ch, victim);
	      break;
	    case 5:
	      spell_wind_breath (LEVEL(ch), ch, victim);
	      break;
	    }
	}

      if ( IS_OBJ_STAT(wield, ITEM_WOUND) && number_range(1,100) == 93)
	{
	  AFFECT_DATA paf;
	  bzero (&paf, sizeof (paf));
	  paf.type = gsn_wound;
	  paf.duration = 3;
	  paf.location = 0;
	  paf.modifier = 0;
	  paf.bitvector = 0;
	  paf.bitvector2 = AFF_WOUND;
	  renew_affect (victim, &paf);
	   act ("You open up a huge gash in $N's side.", ch, wield, victim, TO_CHAR);
	   act ("$n lifts the $p high and rips open a huge gash in $N's side.", ch, wield, victim, TO_NOTVICT);
	   act ("$n lifts up $s $p and brings it down, opening a huge gash in your side.", ch, wield, victim, TO_VICT);

	}
      if (IS_OBJ_STAT(wield, ITEM_BLIND) && number_range(1,200) == 46 && FALSE)
	{
	  if (!IS_AFFECTED_EXTRA(victim, AFF_PROT_BLIND))
	    {
	      AFFECT_DATA paf;
	      bzero (&paf, sizeof (paf));
	      paf.type = gsn_blindness;
	      paf.duration = 3;
	      paf.location = 0;
	      paf.modifier = 0;
	      paf.bitvector = 0;
	      paf.bitvector2 = AFF_BLIND;
	      renew_affect (victim, &paf);
	      act ("$p slices across $N's eyes.", ch, wield, victim, TO_CHAR);
	      act ("$n slices $p across $N's eyes.", ch, wield, victim, TO_NOTVICT);
	      act ("$n slices $s $p across your eyes. You can't see!", ch, wield, victim, TO_VICT);
	      
	    }
	}
      
      if (IS_OBJ_STAT(wield, ITEM_POISON) &&
	  number_range(1,10)== 2 &&
	  !IS_AFFECTED_EXTRA(victim, AFF_PROT_POISON))
	{
	  AFFECT_DATA paf;
	  bzero (&paf, sizeof (paf));
	  paf.type = gsn_poison;
	  paf.duration = 3;
	  paf.location = 0;
	  paf.modifier = 0;
	  paf.bitvector = 0;
	  paf.bitvector2 = AFF_POISON;
	  renew_affect (victim, &paf);
	}
      /* SPECIAL ITEM EFFECTS - FLATLINE */ 
      if (IS_OBJ_STAT(wield, ITEM_KNOCKDOWN) && number_range(1,80) == 2)
        {
	      act ("$p sends $N sprawling to the ground!", ch, wield, victim, TO_CHAR);
	      act ("$n's $p sends $N sprawling to the ground!", ch, wield, victim, TO_NOTVICT);
	      act ("$n's $p sends you sprawling to the ground!", ch, wield, victim, TO_VICT);
	      if (RIDING (victim) != NULL) {
		      check_fgt (victim->fgt->riding);
		      victim->fgt->riding->fgt->mounted_by = NULL;
		      victim->fgt->riding = NULL;
	      }
	      if (victim->position != POSITION_FIGHTING)
		      victim->fgt->fighting = ch;
	      victim->position = POSITION_BASHED;
	      set_fighting(ch, victim);
	      victim->fgt->ears = 10;
	}

      
      if (IS_OBJ_STAT(wield, ITEM_MANARESTORE) && number_range(1,50) == 4)
        {
	      act ("$p sends a crackle of energy up your arm as it strikes $N!", ch, wield, victim, TO_CHAR);
	}
      if (IS_OBJ_STAT(wield, ITEM_CRUSADER) && number_range(1,90) == 6)
        {
	      act ("$p engulfs you in a brilliant glow as it strikes $N!", ch, wield, victim, TO_CHAR);
	      act ("$n is engulfed in a brilliant glow as $n's $p strikes $N!", ch, wield, victim, TO_NOTVICT);
	      act ("$n is engulfed in a brilliant glow as $s $p strikes you!", ch, wield, victim, TO_VICT);
	      dam *= number_range(5, 10);
	}
      if (IS_OBJ_STAT(wield, ITEM_MANABURN) && number_range(1,100) == 6)
        {
	      act ("$p engulfs $N in burning arcane energies!", ch, wield, victim, TO_CHAR);
	      act ("$n's $p engulfs $N in burning arcane energies!", ch, wield, victim, TO_NOTVICT);
	      act ("$n's $p engulfs you in burning arcane energies!", ch, wield, victim, TO_VICT);
	}
      if (IS_OBJ_STAT(wield, ITEM_OVERPOWER) && number_range(1,150) == 6)
      
        {
	      act ("$p strikes $N causing $S to scream in pain!", ch, wield, victim, TO_CHAR);
	      act ("$N screams in pain as $n's $p strikes $S!", ch, wield, victim, TO_NOTVICT);
	      act ("You scream in pain as $n's $p strikes you!", ch, wield, victim, TO_VICT);
	      dam *= number_range(5, 10);
	}
      if (IS_OBJ_STAT(wield, ITEM_WEAKEN) && number_range(1,100) == 6)
        {
	      act ("$p saps $N's strength!", ch, wield, victim, TO_CHAR);
	      act ("$n's $p strikes you!  You feel weaker.", ch, wield, victim, TO_VICT);
	      SUBMOVE(victim, (1+dam)*number_range(3,5));
	}
      if (IS_OBJ_STAT(wield, ITEM_VAMPIRIC) && number_range(1,100) == 6)
        {
	      act ("$p seems to greedily lap up the $N's blood!  You feel healthy.", ch, wield, victim, TO_CHAR);
	      act ("$n's $p seems to greedily lap up $N's blood!", ch, wield, victim, TO_NOTVICT);
	      act ("$n's $p seems to greedily lap up your blood!", ch, wield, victim, TO_VICT);
	      ch->hit += (1+dam)*(1+number_range(1,3));
	      if (ch->hit > ch->max_hit)
		      ch->hit = ch->max_hit;
	}
      
    }
  /* bscalc Backstab damage here bsdam*/
  if (dt == gsn_circle || dt == gsn_backstab)
    {
  int muu = (LEVEL (ch) * 3 / 16);
  if (muu > 15) muu = 15;
      if (is_member(ch, GUILD_ROGUE))
        {
          muu += 4;
        }
      if (is_member(ch, GUILD_THIEFG))
        {
          muu += 3;
        }
	if ( is_member(ch, GUILD_WIZARD) &&
	   !is_member(ch, GUILD_THIEFG) &&
	     !is_member(ch, GUILD_ROGUE))
	 {
   	  muu -= 3;
	 }

	if (IS_AUGMENTED(ch, AUG_HAND_SPD)) muu += 2;
	if (IS_AUGMENTED(ch, AUG_HAND_POWER)) muu += 2;

  dam = muu*dam;

  /*  Damage reductions based on certain classifications and guilds */

	/* Guild Reductions  If in wizards and NOT assassins AND thiefs
	 * Reduction by 1/4 to get rid of mage stabbers that havent
	 * thief/assassin guilded
	*/

	if ( is_member(ch, GUILD_WIZARD) &&
	    !is_member(ch, GUILD_THIEFG) &&
	    !is_member(ch, GUILD_ROGUE))
 		dam=dam/5;

        if ( is_member(ch, GUILD_HEALER) &&
	    !is_member(ch, GUILD_THIEFG) &&
	    !is_member(ch, GUILD_ROGUE))
		dam-dam/5;

	/* Reductions based on crappy dex
         * Primary damage reduction here, increase the divider
	 * to lower backstab damage overall. Basic formula
         * is just (dam) * (char_dex) / (divisor)
	*/
	dam = (dam*(get_curr_dex(ch))/23);

	/* Here we modify the damage even more for lower end of the
	 * DEX scale  THESE ARE ADDITIVE (or subtractive as the case may
	 * be) to the above reductions.
	*/
	if (get_curr_dex(ch) < 32)
	   dam = dam*7/6;
	if (get_curr_dex(ch) < 29)
	   dam = dam*3/4;
        if (get_curr_dex(ch) < 27)
           dam = dam*4/7;
        if (get_curr_dex(ch) < 23)
           dam = dam*3/7;
        if (get_curr_dex(ch) < 21)
           dam = dam*2/7;
        if (get_curr_dex(ch) < 18)
           dam = dam/4;

     if(IS_PLAYER(ch))
        dam = (dam*(race_info[ch->pcdata->race].backstab_percent +
	  align_info[ch->pcdata->alignment].backstab_percent))/100;

      if(dt == gsn_backstab)
        {
          if (dam > 450) dam = 450 +number_range(1,90);
         }

      if(dt == gsn_circle)
        {
          if (dam > 280) dam = 250 +number_range(1,100);
         }
  }




  if (dt == gsn_impale) {
      int muu;
      if (dam < 5) dam = 5;
      muu = 5 + (get_curr_str (ch)/ 3);
      SUBMOVE(ch, 20);
      if (is_member (ch, GUILD_WARRIOR)) {
          muu += 3;
          dam += 2;
      }

      if (is_member (ch, GUILD_BATTLEMASTER)) {
          muu += 3;
          dam += 2;
      }

      dam *= muu;
      dam /= 3;
    }

  if (dt == gsn_charge) {
      int muu;
      if (dam < 10) dam = 10;
      muu = 5 + (get_curr_str (ch)/ 2);
      SUBMOVE(ch, 40);
      if (is_member (ch, GUILD_WARRIOR)) {
          muu += 3;
          dam += 4;
      }

      if (is_member (ch, GUILD_BATTLEMASTER)) {
          muu += 3;
          dam += 4;
      }

      dam *= muu;
      dam /= 2;
    }


  if (dam <= 0)
    dam = 1;
  if (number_range (1, 5) == 1 && ch->move > 0) {
      SUBMOVE(ch,0);
  }
  ghit_pos = hit_pos;

  if (wield) {
      dam = dam * (UMIN(100,( 30+(((I_WEAPON*)wield->more)->damage_p))))/100;
      damage(ch, victim, dam, dt);
  }
  else {
      hitop=TRUE;
      damage (ch, victim, dam, dt);
  }

  if (victim->data_type==50 || FIGHTING (ch) == NULL)  {
    return;
  }

  {
    bool broken = FALSE;
    if (wield !=NULL && (((I_WEAPON *) wield->more)->strength != 0) &&
	(dam >= ((I_WEAPON *) wield->more)->strength))
      if (number_range (1, 36) == 5)
	{
	  --((I_WEAPON *) wield->more)->damage_p;
	  if (((I_WEAPON *) wield->more)->damage_p == 1)
	    {
	      send_to_char ("--->ACK! Your weapon just broke!\n\r", ch);
	      unequip_char (ch, wield);
	      obj_from(wield);
	      obj_to(wield,get_room_index(8));
	      broken = TRUE;
	    }
	  else if (number_range (1, 4) == 3)  /*only display occasionally */
	    send_to_char ("Your weapon is damaged slightly by your attack!\n\r", ch);
	}
  }
  ghit_pos = 0;

  if (dam >= 4 && number_range (1, 20) == 2
      && (armor = get_eq_char (victim,(iWear = number_range (WEAR_NONE + 1, (MAX_WEAR-1))))) != NULL
      && armor->pIndexData->item_type == ITEM_ARMOR
      && armor->wear_loc != -1
      && ((I_ARMOR *) armor->more)->max_condition)

    {
      int localle;
      char blah[500];
      char *tmp;
      if ((hit_pos == STRIKE_BODY && (armor->wear_loc > WEAR_FACE && armor->wear_loc < WEAR_ARMS)) ||
	  (hit_pos == STRIKE_HEAD && armor->wear_loc < WEAR_BODY) ||
	  (hit_pos == STRIKE_ARMS && (armor->wear_loc > WEAR_BACK && armor->wear_loc < WEAR_HOLD_1)) ||
	  (hit_pos == STRIKE_LEGS && armor->wear_loc > WEAR_BELT_5))
	{
	  if (armor->pIndexData->short_descr[0] != '\0')
	    {
	      for (tmp = armor->pIndexData->short_descr; *tmp != '\0';
		   tmp++)
		{
		};
	      if (*(tmp - 1) == 's')
		sprintf (blah, "$p are damaged.");
	      else
		sprintf (blah, "$p is damaged.");
	      act (blah, victim, armor, NULL, TO_CHAR);
	      localle = armor->wear_loc;
	      unequip_char (victim, armor);
	      --((I_ARMOR *) armor->more)->condition_now;
	      equip_char (victim, armor, localle);
	      if (((I_ARMOR *) armor->more)->condition_now <= 0)
		{
		  if (*tmp == 's')
		    sprintf (blah, "The damage was so powerful, $p fall apart!");
		  else
		    sprintf (blah, "The damage was so powerful, $p falls apart!");
		  act (blah, victim, armor, NULL, TO_CHAR);
		  unequip_char (victim, armor);
		  free_it (armor);
		  return;
		}
	    }
	}
    }
  return;
}







bool
can_trip (CHAR_DATA * ch)
{
  if (number_range (1, 4) != 1)
    return FALSE;
  if (IS_PLAYER (ch))
    return TRUE;
  if (ch->pIndexData->mobtype == MOB_HUMAN
      || ch->pIndexData->mobtype == MOB_DWARF
      || ch->pIndexData->mobtype == MOB_ELF
      || ch->pIndexData->mobtype == MOB_GIANT
      || ch->pIndexData->mobtype == MOB_DRAGON)
    return TRUE;
  return FALSE;
}
bool
bashable (CHAR_DATA * ch)
{
  if (IS_PLAYER (ch))
    return TRUE;
  if (ch->pIndexData->mobtype == MOB_GHOST)
    return FALSE;
  if (ch->pIndexData->mobtype == MOB_DRAGON)
    return FALSE;
  return TRUE;
}
int
kickable (CHAR_DATA * ch)
{
  if (IS_PLAYER (ch))
    return 1;
  if (ch->pIndexData->mobtype == MOB_GHOST)
    return 0;
  if (IS_AFFECTED (ch, AFF_FLYING))
    return 2;
  return 1;
}

bool
pkill (CHAR_DATA * ch, CHAR_DATA * victim)
{
  return TRUE;
}


/*
   * Inflict damage from a hit.
 */
void
damage (CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt)
{
  char buf[500];
  AFFECT_DATA *paf;
  SPELL_DATA *spell;

  if (!ch || !victim)
    {
    return;
    }

  CHECK_CHAR (ch);
  CHECK_CHAR (victim);
  check_fgt (ch);
  check_fgt (victim);

  if (ch->in_room != victim->in_room) /* Not a spell or ranged */
    {
      if (dt > 300 && dt != gsn_shoot && dt != gsn_throw)
	{
	  ch->fgt->fighting=NULL;
	  victim->fgt->fighting=NULL;
	  ch->position=POSITION_STANDING;
	  victim->position=POSITION_STANDING;
	  return;
	}
      else
	{
	  if (IS_MOB(victim) && IS_SET(victim->act, ACT_SENTINEL))
            {
	      dam = 0;
              return;
            }
	}
    }
  if (IS_PLAYER(ch) && LEVEL(ch) >= IMM_LEVEL && LEVEL(ch) < MAX_LEVEL && (!ch->in_room->area || ch->in_room->area->open == 1))
    {
      ch->fgt->fighting = NULL;
      victim->fgt->fighting = NULL;
      return;
    }

  if (victim->position == POSITION_DEAD || victim->data_type == 50)
    {
      ch->wait = 0;
      stop_fighting(ch,TRUE);
      return;
    }

  if (ch->in_room == victim->in_room)
    {
      add_to_fighting_list (ch);
      add_to_fighting_list (victim);
      if (!FIGHTING(ch)) set_fighting(ch,victim);
      if (!FIGHTING(victim)) set_fighting(victim,ch);

      if (FIGHTING (victim) == ch && FIGHTING (ch) != victim && !(victim->position == POSITION_GROUNDFIGHTING &&   ch->position == POSITION_GROUNDFIGHTING) && dt != gsn_kick &&  victim->position != POSITION_CASTING && victim->position != POSITION_BASHED
	  && ch->position != POSITION_CASTING  && ch->position != POSITION_BASHED)
	{
	  NEW_POSITION(ch, POSITION_FIGHTING);
	  NEW_POSITION(victim, POSITION_FIGHTING);

	  if (ch!=victim || pow.can_fight_self)
	    {
	      ch->fgt->fighting = victim;
	    }
	}

      if (ch->position == POSITION_GROUNDFIGHTING && victim->position != POSITION_GROUNDFIGHTING)
	{
	  NEW_POSITION(ch, POSITION_FIGHTING);
	}
      update_pos (victim);
    }
  if (victim != ch)
    {
      if (is_safe (ch, victim))
	{

	  if (IS_MOB (ch))
	    return;
	  act ("\x1B[1;37mYou hear a loud clash of thunder in the sky, sent forth by", ch, NULL, NULL, TO_CHAR);
	  act ("the gods, and you decide not to attack.\x1B[0m", ch, NULL, NULL, TO_CHAR);
	  act ("You hear massive clashes of thunder in the heavens above.",
	       ch, NULL, NULL, TO_NOTVICT);
	  return;
	}
      if(IS_PLAYER(ch))
	{
	  if (IS_MOB(victim) || !DIFF_ALIGN(ch, victim))
	    {
	      if(ch->pcdata->no_quit_pk < MOB_QUIT_TIMER)
		ch->pcdata->no_quit_pk = MOB_QUIT_TIMER;
	    }
	  /*ARC: no timers associated with neuts */
	  else if(IS_PLAYER(victim) && ALIGN(ch)!= 0 && ALIGN(victim)!=0)
	    {
	      ch->pcdata->no_quit_pk = PK_QUIT_TIMER;
	      victim->pcdata->no_quit_pk = PK_QUIT_TIMER;
	    }
	}
      else
	{
	  if (IS_PLAYER(victim) && victim->pcdata->no_quit_pk < MOB_QUIT_TIMER)
	    victim->pcdata->no_quit_pk = MOB_QUIT_TIMER;
	}
    }

  if (can_yell (victim) && victim->hit < 8 && victim->max_hit > 100 && ch->hit > 60 && dt > 300)
    {
      do_fatal (victim, ch, dt);
    }
  if (IS_PLAYER (ch) && dam > 2 && ch && victim && ch != victim)
    {
      int gn;
      gn = (dam * 3) + (LEVEL (victim) * 3) + ((LEVEL (ch) * LEVEL (ch)) / 8) + number_range (1, (LEVEL (victim) + LEVEL (ch)));
      gn/=7;
        if (IS_PLAYER(victim) && !DIFF_ALIGN(ch, victim))
          gn = 0;
        gain_exp (ch, gn);
	ch->pcdata->voting_on += gn;
      }
    /*
     * More charm stuff.
     */
  if (MASTER (victim) == ch)
    stop_follower (victim);

  if (IS_AFFECTED (ch, AFF_HIDE))
    {
      affect_strip (ch, gsn_hide);
      REMOVE_BIT (ch->affected_by, AFF_HIDE);
      act ("$n is revealed.", ch, NULL, NULL, TO_ROOM);
    }
  if (IS_AFFECTED_EXTRA (ch, AFF_CHAMELEON))
    {
      affect_strip (ch, gsn_chameleon);
      REMOVE_BIT (ch->more_affected_by, AFF_CHAMELEON);
      act ("$n comes back from the \x1b[1;32me\x1b[1;33mn\x1b[1;32mv\x1b[1;33mi\x1b[1;32mr\x1b[1;33mo\x1b[1;32mn\x1b[1;33mm\x1b[1;32me\x1b[1;33mn\x1b[1;32mt.", ch, NULL, NULL, TO_ROOM);
    }
  if (dt >= TYPE_HIT)
    {
      if ((IS_MOB (ch) && number_percent () < 20) && ch->move > 10)
	{
	  if (can_trip (ch) &&
	      victim->position != POSITION_GROUNDFIGHTING && victim->position != POSITION_CASTING)
	    {
	      disarm (ch, victim);
	    }
	}
      if ((IS_MOB (ch) && number_percent () < 8) &&
	  victim->position != POSITION_GROUNDFIGHTING && can_trip (ch) &&
	  victim->position != POSITION_CASTING && can_trip (victim)
	  && victim->fgt->combat_delay_count < 0
	  && ch->move > 10)
	{
	  if (number_range (0, 7) == 1)
	    trip (ch, victim);
	  else
	    {
	      act ("$n tries to trip you, but you quickly jump out of the way!", ch, NULL, victim, TO_VICT_SPAM);
	      act ("You try to trip $N, but $N swiftly jumps out of the way!", ch, NULL, victim, TO_CHAR);
	      act ("$n tries to trip $N, but $N swiftly jumps out of the way!", ch, NULL, victim, TO_NOTVICT_SPAM);
	    }
	}
      if (dt > 999 && dt != gsn_circle && dt != gsn_backstab)
	{
	  if (IS_AFFECTED (victim, AFF_INVISIBLE) &&
	      !IS_AFFECTED (ch, AFF_DETECT_INVIS) && number_range (1, 14) == 3)
	    {
	      act ("You flail wildly around, trying to hit your invisible opponent...", ch, NULL, victim, TO_CHAR_SPAM);
	      return;
	    }
	}
    }

  if (dt == gsn_backstab || dt == gsn_circle || (dt != gsn_shoot && dt != gsn_throw && dt != gsn_kick && (!(dt > 0 && dt < 300))))
  {

    /* ------------------------- */
    /*       Mana Shield         */
    /* ------------------------- */
    if(IS_AFFECTED_EXTRA(victim, AFF_MANASHIELD) && dt != gsn_backstab && dt != gsn_circle)
    {
      if(victim->pcdata->mana_shield_strength >= dam)
      {
        victim->pcdata->mana_shield_strength -= dam;
        dam = 0;
        if(!IS_MOB(ch))
        {
          sprintf(buf,"Your victim's \x1b[1;37mMana Shield\x1b[0;37m \x1b[1;36mCRACKLES\x1b[0;37m as your hit is absorbed!\n\r",NAME(victim));
          send_to_char(buf,ch);
        }
        sprintf(buf,"Your \x1b[1;37mMana Shield\x1b[0;37m \x1b[1;36mCRACKLES\x1b[0;37m as protects you from %s's attack!\n\r",NAME(ch));
        send_to_char(buf,victim);
      }
      else
      {
        dam = dam - victim->pcdata->mana_shield_strength;
        victim->pcdata->mana_shield_strength = 0;
	/* find any and all manashield's and remove them */
	for(paf = victim->affected ; paf != NULL ; paf = paf->next)
        {
	   spell = skill_lookup(NULL, (int) paf->type);
	   if(spell == NULL)
	     continue;
	   if(spell->bitvector2 == AFF_MANASHIELD)
	     affect_remove (victim, paf);
        }
        sprintf(buf,"%s's \x1b[1;32mmana shield\x1b[0;37m absorbs your hit then winks out of existance.\n\r",NAME(victim));
        send_to_char(buf,ch);
        sprintf(buf,"Your \x1b[1;37mMana Shield\x1b[0;37m \x1b[1;31mcollapses!\x1b[0;37m\n\r",NAME(ch));
        send_to_char(buf,victim);

        /* Shield is gone, hit the person with the rest of damage */
        dam_message (ch, victim, dam, dt, ghit_pos);
      }
    }
    else  /* they got no shield, so whack em! */
      dam_message (ch, victim, dam, dt, ghit_pos);
  } /* end if dt == gsn_back */

  /* ------------------------- */
  /*       Actual damage       */
  /* ------------------------- */

       SUBHIT(victim,dam);

  /* ------------------------- */
  /*        Post damage        */
  /* ------------------------- */
  if (IS_PLAYER(victim) && victim->position == POSITION_CASTING)
    {
      if (!IS_AUGMENTED((victim), AUG_FOCUS))
	{
	  if (dam > 4)
	    {
	      NEW_POSITION(victim, POSITION_FIGHTING);
	      send_to_char ("OW! You can't cast getting hit like this!\n\r", victim);
	      victim->pcdata->tickcounts = 0;
	    }
	}
      else
	{
	  if (dt < 300)
	    {
	      if (dam > (4+LEVEL(victim)/4 + victim->pcdata->remort_times))
		{
		  NEW_POSITION(victim, POSITION_FIGHTING);
		  send_to_char ("OUCH! You just lost your concentration!\n\r", victim);
		  victim->pcdata->tickcounts = 0;
		}
	    }
	  else
	    {
	      if (dam > 5+LEVEL(victim)/12 + victim->pcdata->remort_times)
		{
		  NEW_POSITION(victim, POSITION_FIGHTING);
		  send_to_char ("OUCH! You just lost your concentration!\n\r", victim);
		  victim->pcdata->tickcounts = 0;
		}
	    }
	}
    }
  if (!IS_AWAKE (victim))
    {
      stop_fighting (victim, FALSE);

    }
  if(ch->in_room == victim->in_room)
    {
      if (victim->position != POSITION_CASTING && victim->position != POSITION_BASHED && victim->position != POSITION_GROUNDFIGHTING && victim->position != POSITION_FIGHTING &&  victim->position != POSITION_CIRCLE && victim->position > POSITION_FIGHTING)
	{
	  victim->position = POSITION_FIGHTING;
	  if (FIGHTING(victim) == NULL)
	    {
	      check_fgt(victim);
	      victim->fgt->fighting = ch;
	    }
	}
    }

  if (IS_PLAYER (victim)
      && LEVEL (victim) >= LEVEL_IMMORTAL
      && victim->hit < 1)
    victim->hit = 1;
  update_pos (victim);
isdedman:
  switch (victim->position)
    {
    case POSITION_MORTAL:
      act ("$n is mortally wounded, and will die soon if not aided.",
	   victim, NULL, NULL, TO_ROOM);
      send_to_char (
		    "You are mortally wounded, and will die soon if not aided.\n\r",
		    victim);
      victim->wait = 10;
      break;
    case POSITION_INCAP:
      act ("$n is incapacitated and will suffer a slow, painful death if not aided.",
	   victim, NULL, NULL, TO_ROOM);
      send_to_char (
		    "You are incapacitated and will suffer a slow, painful death if not aided.\n\r",
		    victim);
      victim->wait = 10;
      break;
    case POSITION_STUNNED:
      act ("$n is stunned, but will probably recover.",
	   victim, NULL, NULL, TO_ROOM);
      send_to_char ("You are stunned, but will probably recover.\n\r",
		    victim);
      victim->wait = 10;
      break;
    case POSITION_DEAD:
      if (IS_PLAYER (ch) && LEVEL (ch) < 8)
	{
	  ch->copper++;
	  send_to_char ("You just got a copper coin from the gods for the kill!\n\r", ch);
	}
      act ("$n has been killed!", victim, 0, 0, TO_ROOM);
      send_to_char ("You have been killed!\n\r\n\r", victim);
      ch->wait = 0;
      victim->wait = 10;
      break;
    default:
      if (dam > victim->max_hit / 5)
	send_to_char ("Ouch, that was *not* pleasant!\n\r", victim);
      if (victim->hit < victim->max_hit / 8)
	send_to_char ("You sure are BLEEDING!\n\r", victim);
      break;
    }

  if (!ch || !victim)
    return;
  if (victim->position == POSITION_DEAD)
    {
      if (IS_PLAYER (victim) && IN_BATTLE (victim) == TRUE)
	{
	  stop_fighting (victim, TRUE);
	  NEW_POSITION(victim, POSITION_STANDING);
	  NEW_POSITION(ch, POSITION_STANDING);
	  MAXHIT(victim);
	  victim->pcdata->no_quit_pk = 0;
	  victim->pcdata->no_quit = 0;
	  char_from_room (victim);
          char_to_room (victim, get_room_index (victim->fgt->wasroomtwo));
	  do_look (victim, "");
	  return;
	}
      if (IS_PLAYER (victim) && CHALLENGE (victim) != 0)
	{
	  stop_fighting (victim, TRUE);
	  NEW_POSITION(victim, POSITION_STANDING);
	  NEW_POSITION(ch, POSITION_STANDING);
	  ch->fgt->fighting = NULL;
	  victim->fgt->fighting = NULL;
	  MAXHIT(ch);
	  MAXHIT(victim);
	  victim->pcdata->no_quit_pk = 0;
	  victim->pcdata->no_quit = 0;
	  end_arena (ch, victim);
	  return;
	}
      group_gain (ch, victim);
      ch->wait = 0;
      if (IS_PLAYER (victim))
	{
	int ii;
	char buf[STD_LENGTH];
        CHAR_DATA *gch;
        bool pk_fight = FALSE;
        if(victim->pcdata->no_quit_pk >= 5 && IS_MOB(ch) && !victim->in_room->area->homeland && !IS_SET(ch->act, ACT_AGGRESSIVE)
           && !IS_SET(ch->pIndexData->act4, ACT4_KILL_OPP) )
           {
                    victim->pcdata->total_wps -= 200;
                    victim->pcdata->warpoints -= 200;
                    if (victim->pcdata->warpoints < 0)
                      victim->pcdata->warpoints = 0;
                    if (victim->pcdata->total_wps < 0)
                      victim->pcdata->total_wps = 0;
            }
/*
             check_room_more(victim->in_room);
             for (gch = victim->in_room->more->people; gch != NULL; gch=gch->next_in_room)
                {
                  if(IS_PLAYER(gch) && DIFF_ALIGN(gch, victim))
                    {
                       pk_fight = TRUE;
                       break;
                    }
                 }
              if (!pk_fight)
                {
*/
/*                } */
	victim->pcdata->deaths++;
	victim->pcdata->no_quit_pk = 0;
	reasonfd[0] = '\0';
	if (strlen (NAME (ch)) < 30)
	  strcpy (reasonfd, NAME (ch));
	sprintf (log_buf, "%s killed by %s at %d",
		 NAME (victim),
		 NAME (ch),
		 victim->in_room->vnum);
	log_string (log_buf);
	sprintf (log_buf, "%s just got killed by %s!", NAME (victim), NAME (ch));
/*WORLD ECHO ADDED BY KILITH 05
  THIS ONLY SHOWS ECHO WHEN PLAYER KILLED BY PLAYER
  THIS WILL ALSO SHOW SAME SIDE PKILL SO WILL SHOW OTHER
  SIDE WHO IS THE ASSHOLE ON THE OTHER SIDE!! LOL
  NOT DRUNK WHILE CODING THIS SP WILL WORK JUST FINE!!
*/
if (IS_PLAYER (victim) && IS_PLAYER (ch))
{
sprintf (buf, "\x1b[1;31m%s \x1B[0;37mwas just \x1b[1;31mk\x1b[0;31mille\x1b[1;31md  \x1B[0;37mby \x1b[1;31m%s\x1B[0;37m!!",  NAME (victim), NAME (ch));
    do_echo (ch, buf);
}
	ii = clan_number (victim);
	if (ii > 0)
	  clan_notify (log_buf, ii);
	if (LEADER (victim) != NULL)
	  group_notify (log_buf, victim);
	if (IS_PLAYER (ch))
	  {
	    sprintf (log_buf, "%s just killed %s!", NAME (ch), NAME (victim));
	    ii = clan_number (ch);
	    if (ii > 0)
	      clan_notify (log_buf, ii);
	  }
	sprintf (buf, "Ntfy> %s", log_buf);
	NOTIFY (buf, LEVEL_IMMORTAL, WIZ_NOTIFY_DEATH);
	if (IS_PLAYER (ch) && IS_PLAYER(victim) && DIFF_ALIGN(ch, victim))
	  {
	    if (victim->pcdata->warpoints > (1+LEVEL(victim)/10))
	      victim->pcdata->warpoints -= (1 + (LEVEL (victim) / 10));
	    if (victim->pcdata->warpoints < 0)
	      victim->pcdata->warpoints = 0;
	  }
	if (IS_MOB (ch) && IS_PLAYER (victim))
	  {
	    pc_death_penalty (ch, victim);

	  }
	else
	  gain_exp (victim, -(LEVEL (victim) * 1423));
      }

    raw_kill (victim, (dt == 1003));

    if (IS_PLAYER (ch) && IS_MOB (victim))
/*    if (IS_MOB (victim)) */
      {
	int old_gold = 0;
	char buf[STD_LENGTH];
	old_gold = tally_coins (ch);
	if (IS_SET (ch->pcdata->act2, PLR_AUTOLOOT))
	  do_get (ch, "all from corpse");
	else if (IS_SET (ch->pcdata->act2, PLR_AUTOGOLD))
	  do_get (ch, "all.coins from corpse");
	if (IS_SET (ch->pcdata->act2, PLR_AUTOSAC))
	  do_sacrifice (ch, "corpse");
	if (IS_SET (ch->pcdata->act2, PLR_AUTOSPLIT)
	    && tally_coins (ch) - old_gold > 1)
	  {
	    sprintf (buf, "%d", tally_coins (ch) - old_gold);
	    do_split (ch, buf);
	  }
      }
    ch->wait = 0;
    return;
  }

if (victim == ch)
  return;
/*
 * Take care of link dead people.
 */
if (IS_PLAYER (victim) && victim->desc == NULL)
  {
    if (number_range (0, victim->wait) < 2)
      {
	do_flee (victim, "");
	return;
      }
  }

/*
 * Wimp out?
 */
if (IS_MOB (victim) &&
    dam > 0 &&
    IS_SET (victim->act, ACT_WIMPY) &&
    number_bits (1) == 0
    && victim->hit < victim->max_hit / 6
    && victim->hit > 2)
  {
    do_flee (victim, "");
    return;
  }
if (IS_PLAYER (victim) &&
    dam > 0 &&
    victim->ced &&
    victim->hit < victim->ced->wimpy &&
    victim->position != POSITION_BASHED &&
    victim->position >= POSITION_SLEEPING &&

    victim->hit > 0)
  {
    if (IS_AFFECTED_EXTRA(victim, AFF_PARALYZE))
      {
	send_to_char("You are DYING, but you CAN'T MOVE!!!\n\r", ch);
	return;
      }
    else
      do_flee (victim, "");
    return;
  }
return;
}


bool
is_safe (CHAR_DATA * ch, CHAR_DATA * victim)
{
  CHAR_DATA *tch;
if (!victim->in_room) return TRUE;
if (ch->in_room && ((ch->in_room->vnum < 111 && ch->in_room->vnum > 95)||
   ch->in_room->vnum == 112))
  return TRUE;

for (tch = victim->in_room->more->people; tch != NULL; tch = tch->next_in_room)
  if (IS_PLAYER(tch) && LEVEL(tch) > IMM_LEVEL  && IS_SET (tch->act, PLR_HOLYPEACE))
    {
      if (FIGHTING (victim) != NULL)
	{
	  victim->fgt->fighting = NULL;
	  NEW_POSITION(victim, POSITION_STANDING);
	}
      if (FIGHTING (ch) != NULL)
	{
	  ch->fgt->fighting = NULL;
	  NEW_POSITION(ch, POSITION_STANDING);
	}
      send_to_char ("No fighting in the presence of Gods!\n\r", ch);
      return TRUE;
    }
return FALSE;
}



void
update_pos (CHAR_DATA * victim)
{
  if (victim == NULL)
    return;
  if (victim->hit > 0)
    {
      if (victim->position <= POSITION_STUNNED && victim->position >= 0)
        {
          if (FIGHTING (victim) == NULL)
            NEW_POSITION(victim, POSITION_STANDING);
        }
      return;
    }

  if (IS_MOB (victim) || victim->hit <= -5)
    {
      NEW_POSITION(victim, POSITION_DEAD);
      return;
    }

  if (victim->hit <= -3)
    NEW_POSITION(victim, POSITION_MORTAL);
  else if (victim->hit <= -1)
    NEW_POSITION(victim, POSITION_INCAP);
  else
    NEW_POSITION(victim, POSITION_STUNNED);
  return;
}
/*
   * Start fights.
 */
void
set_fighting (CHAR_DATA * ch, CHAR_DATA * victim)
{

  check_ced (ch);
  check_ced (victim);
  check_fgt (ch);
  check_fgt (victim);

  if (ch == NULL || victim == NULL)
    {

    return;
    }
  if (ch==victim && !pow.can_fight_self) return;
  ch->fgt->fighting = victim;

  if (ch->position != POSITION_GROUNDFIGHTING && ch->position != POSITION_BASHED)
    NEW_POSITION(ch, POSITION_FIGHTING);

  add_to_fighting_list (ch);

  add_to_fighting_list (victim);

  if (IS_MOB (victim) && FIGHTING (victim) == NULL &&
    IS_PLAYER(ch) && IS_SET(victim->act, ACT_SOLOYELL) &&
      victim->pIndexData->yeller_number > 0 &&
      number_range(1,victim->pIndexData->yeller_number) == 1 &&
      can_yell (victim))
    {
      char buffr[500];

      if (number_range (1, 2) == 1)
	sprintf (buffr, "Help! %s is attacking me!", NAME(ch));
      else
	sprintf (buffr, "%s is trying to kill me! Send help!", NAME(ch));
      do_yell (victim, buffr);
    }
  if (FIGHTING(victim) == NULL)
    {
    victim->fgt->fighting = ch;

    }
  return;
}

/*
 * Stop fights.
 */
void
stop_fighting (CHAR_DATA * ch, bool fBoth)
{
  CHAR_DATA *fch;
  CHAR_DATA *fch_next;

  if (FIGHTING(ch)==FIGHTING(FIGHTING(ch)))
    fBoth=TRUE;
  if (fBoth && FIGHTING (ch))
    {
      NEW_POSITION(ch->fgt->fighting,POSITION_STANDING);
      if (FIGHTING (FIGHTING (ch)))
	{
	  ch->fgt->fighting->fgt->fighting = NULL;
	}
      update_pos (FIGHTING (ch));
    }
  for (fch = f_first; fch != NULL; fch = fch_next)
    {
      fch_next = fch->next_fighting;
      if (fch == ch || (fBoth && FIGHTING (fch) != NULL && FIGHTING (fch) == ch))
        {
          fch->fgt->fighting = NULL;
          if (fch->position == POSITION_BASHED) continue;
          NEW_POSITION(fch, POSITION_STANDING);
          fch->fgt->combat_delay_count = -1;
          update_pos (fch);
        }
    }

  if (ch->fgt)
    {
      ch->fgt->fighting = NULL;
    }
  NEW_POSITION(ch, POSITION_STANDING);
  update_pos (ch);
  return;
}

bool
IS_CORPSE_MOB (CHAR_DATA * ch)
{
  if (IS_PLAYER (ch))
    return TRUE;

  if (ch->is_undead)
	  return FALSE;
  
  switch (ch->pIndexData->mobtype)
    {
    case MOB_GHOST:
      return FALSE;
      break;
    case MOB_STATUE:
      return FALSE;
      break;
    case MOB_PLANT:
      return FALSE;
      break;
    case MOB_FISH:
      return FALSE;
      break;
    }
  return TRUE;
}

void
disarm (CHAR_DATA * ch, CHAR_DATA * victim)
{
  int num = get_curr_str (ch) - get_curr_str (victim) + 8;
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *objc;
  if ((objc = get_item_held (ch, ITEM_WEAPON)) == NULL)
    return;
    if (((obj = get_item_held (victim, ITEM_WEAPON)) == NULL))
       if (((obj = get_item_held (victim, ITEM_GEM)) == NULL))
        return;
  if (ch->in_room && ch->in_room->vnum < 1000) return;
  if (IS_AUGMENTED(victim, AUG_HAND_POWER))
    num -= 5;
  if (IS_AUGMENTED(ch, AUG_HAND_POWER))
    num += 4;
  if (number_range(1,40) > num || number_range(1,6) != 3)
    {
      act ("$n tries to disarm you, but fails!", ch, NULL, victim,TO_VICT_SPAM + 1000);
      act ("You try to disarm $N, but fail!", ch, NULL, victim, TO_CHAR +1000);
      act ("$n tries to disarm $N, but fails!", ch, NULL, victim,TO_NOTVICT_SPAM + 1000);
      return;
    }

  if (IS_SET (obj->extra_flags, ITEM_NOREMOVE))
    {
      act ("$B$n tries to disarm you, but fails!$R",ch, obj, victim, TO_VICT);
      act ("You try to disarm $N, but fail!",ch, obj, victim, TO_CHAR);
      return;
    }

  act ("$B$4$n disarms you and sends your $p flying!$R$7", ch,obj,victim,TO_VICT);
  act ("$B$4You have managed to disarm $N!$R$7", ch, NULL, victim, TO_CHAR);
  act ("$B$4$n disarms $N!$R$7", ch, NULL, victim, TO_NOTVICT);
  unequip_char (victim, obj);
  obj_from (obj);
  if (!can_drop_obj(victim, obj))
  obj_to (obj, victim);
  else
  obj_to (obj, victim);
  return;
}

void
do_bash (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  int chan = 0;
  bool notft = FALSE;
  int tmpi;
  char buf[STD_LENGTH];
  DEFINE_COMMAND ("bash", do_bash, POSITION_FIGHTING, 0, LOG_NEVER, "This command allows you to bash an opponent to the ground.")
    check_fgt (ch);
  if (ch->wait > 2)
    return;
  ch->wait+= 5;
if (ch->pcdata->learned[gsn_bash] < 10)
  {
    send_to_char("You don't know how to bash!\n\r", ch);
    WAIT_STATE(ch, 1*PULSE_VIOLENCE);
    return;
  }
  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't bash this soon again after giving or recieving a bash.\n\r", ch);
      WAIT_STATE(ch, 1*PULSE_VIOLENCE);
      return;
    }
  if (ch->in_room->sector_type == SECT_UNDERWATER)
    {
      send_to_char ("Bash someone underwater? I don't think so...\n\r", ch);
      return;
    }
  if ((FIGHTING (ch) == NULL || ch->position != POSITION_FIGHTING) && (argy == "" || argy[0] == '\0'))
    {
      send_to_char ("Bash whom?\n\r", ch);
      return;
    }
  if (FIGHTING (ch) == NULL)
    {
      notft = TRUE;
      if ((victim = get_char_room (ch, argy)) == NULL)
        {
          if (argy[0] == '\0')
            {
              send_to_char ("Bash who?\n\r", ch);
              return;
            }
          send_to_char ("They aren't here.\n\r", ch);
          return;
        }
      if (victim == ch)
        {
          send_to_char ("You really do look like an idiot!\n\r", ch);
          return;
        }
	/* ----------------------------------------- */
	/*	    Protecting Neutral align.	     */
	/* ----------------------------------------- */
	if (IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN(victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
	{
		send_to_char ("A strange force prevents you from harming this person.\n\r", ch);
		return;
	}
	if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
	{
		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
		return;
	}

      /* ----------------------------------------- */
      /*      Protecting LinkDead-NoTimers 	   */
      /* ----------------------------------------- */
      if(IS_PLAYER(victim) && victim->desc == NULL) {
        if(!(victim->pcdata->no_quit_pk > 0)) {
	  send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
  	  return;
        }
      }

    }
  else
    victim = ch->fgt->fighting;

  if (RIDING (ch) != NULL)
    {
      send_to_char ("You can't bash someone when you are riding!\n\r", ch);
      return;
    }

  if (is_safe (ch, victim))
    {
      return;
    }

  check_ced (ch);
  check_ced (victim);
  if (!(IS_IN_MELEE (ch, victim)))
    {
      send_to_char ("You aren't on the front lines of that battle!\n\r", ch);
      return;
    }
  if (FIGHTING(ch) == NULL && FIGHTING(victim) != NULL && !IS_IN_MELEE(ch, victim))
    {
      send_to_char("You cannot get close enough to bash them!\n\r", ch);
      return;
    }
  if (!can_see (ch, victim))
    {
      send_to_char ("How can you bash someone you can't see!?\n\r", ch);
      return;
    }

  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      act ("You are wrestling on the ground! You can't bash $N!", ch, NULL, victim, TO_CHAR);
      return;
    }

  if (victim->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("Your victim is groundfighting!\n\r", ch);
      WAIT_STATE(ch, 1*PULSE_VIOLENCE);
      return;
    }

  if (ch->position == POSITION_BASHED)
    {
      act ("You yourself are bashed.. how do you expect to bash $N?", ch, NULL, victim, TO_CHAR);
      return;
    }

  if (!bashable (victim))
    {
      act ("You can't bash that kind of creature!", ch, NULL, victim, TO_CHAR);
      return;
    }
  check_fgt (victim);

  if (victim->fgt->combat_delay_count < -2)
    {
      act ("$N is alert from $S last bash to the ground. You nearly fall over!", ch, NULL, victim, TO_CHAR);
      WAIT_STATE (ch, 1 * PULSE_VIOLENCE);
      return;
    }

  if (victim->position == POSITION_BASHED || victim->fgt->ears > 2)
    {
      act ("$N has already been bashed to the ground!", ch, NULL, victim, TO_CHAR);
      WAIT_STATE(ch, 2*PULSE_VIOLENCE);
      return;
    }

  if (number_range (1, 5) == 3 && IS_AWAKE (victim) && IS_AFFECTED (victim, AFF_FLYING) && !IS_AFFECTED (ch, AFF_FLYING))
    {
/*basher is not flying, victim is */
      act ("$N quickly flies out of the way of $n's bash!", ch, NULL, victim, TO_NOTVICT + 1000);
      act ("$N quickly flies out of the way of your bash!", ch, NULL, victim, TO_CHAR + 1000);
      act ("You quickly fly out of the way of $n's bash!", ch, NULL, victim, TO_VICT + 1000);
      WAIT_STATE (ch, 2 * PULSE_VIOLENCE);
      return;
    }
check_same_side_pk(ch, victim);
  if (IS_MOB (victim))
    tmpi = 10;
  else
    tmpi = (15 + get_curr_dex (victim) +(victim->pcdata->learned[gsn_dodge] / 5));
  if (IS_MOB (ch))
    chan = (LEVEL (ch) * 2);
  else
    chan = ((ch->pcdata->learned[gsn_bash]) + (get_curr_str (ch) * 3)) -tmpi;
  if (IS_PLAYER (ch) && ch->pcdata->learned[gsn_bash] < 5)
    chan = 5;
  if (IS_AUGMENTED(victim, AUG_FOOT_SPD)) chan -= 10;
  if (IS_AUGMENTED(victim, AUG_LEG_SPD)) chan -= 10;
  if (IS_AUGMENTED(ch, AUG_BODY_POWER)) chan += 5;
  if (IS_AUGMENTED(ch, AUG_LEG_POWER)) chan += 5;
  if (IS_AUGMENTED(ch, AUG_ARM_POWER)) chan += 5;
  if (IS_PLAYER(victim) && victim->pcdata->learned[gsn_steadiness]
>=10)
    {
      chan -= (victim->pcdata->learned[gsn_steadiness]/3);
      skill_gain(victim, gsn_steadiness, TRUE);
    }
  if (chan < 5)
    chan = 5;
  if (chan > 155)
    chan = number_range (150, 160);
  if (number_range (0, 215) > chan && IS_AWAKE (victim))
    {
      act ("You miss the bash and almost fall on the ground!", ch, NULL, victim, TO_CHAR + 1000);
      act ("$n tries to bash you, but almost falls over instead!", ch, NULL, victim, TO_VICT + 1000);
      act ("$n misses a bash at $N and almost falls over!", ch, NULL, victim, TO_NOTVICT + 1000);
      if (notft)
        {
          WAIT_STATE (ch, (double) 4 * PULSE_VIOLENCE);
        }
      else
        WAIT_STATE (ch, (double) 3 * PULSE_VIOLENCE);

      if (FIGHTING (victim) == NULL)
        set_fighting (victim, ch);
      set_fighting (ch, victim);
      return;
    }

  if (RIDING (victim) != NULL)
    {

      sprintf (buf, "$B$2Your powerful bash sends $N flying off of %s!$R$7", PERS (RIDING (victim), ch));
      act (buf, ch, NULL, victim, TO_CHAR);
      sprintf (buf, "$B$2$n's bash sends $N flying off of %s!$R$7", NAME (RIDING (victim)));
      act (buf, ch, NULL, victim, TO_NOTVICT);
      sprintf (buf, "$B$2You fly off of %s as $n bashes you!$R$7", PERS (RIDING (victim), victim));
      act (buf, ch, NULL, victim, TO_VICT);
      check_fgt (victim->fgt->riding);
      victim->fgt->riding->fgt->mounted_by = NULL;
      victim->fgt->riding = NULL;
    }

  else if (RIDING (victim) == NULL)
    {
      if (number_range (1, 18) != 4 || !can_groundfight (victim) || LEVEL (victim) < 12
          || !can_groundfight (ch) || LEVEL (ch) < 12 || !pow.bash_slip_to_tackle)
        {
          char buf[400];
          act ("$B$2Your powerful bash sends $N sprawling onto the ground!$R$7", ch, NULL, victim, TO_CHAR + 1000);
          act ("$B$2$n sends $N sprawling backwards on to the ground with a powerful bash!$R$7", ch, NULL, victim, TO_NOTVICT + 1000);
          act ("$B$2$n sends you sprawling backwards on to the ground with a powerful bash!$R$7", ch, NULL, victim, TO_VICT + 1000);
          if (IS_PLAYER (ch))
            skill_gain (ch, gsn_bash, TRUE);
          if (CHALLENGE (ch) == 10 && number_range(1,3) == 2)
            {
              sprintf (buf, "($B$1Arena$N$7) $B%s just sent %s flying with a bash!", NAME (ch), NAME (victim));
              do_arenaecho (ch, buf, TRUE);
            }
        }
      else
        {
          char buf[500];
          int i;
          act ("$B$5You bash $N, but trip and stumble! You are now groundfighting!$R$7", ch, NULL, victim, TO_CHAR);
          act ("$B$5$n bashes $N, but trips and falls! They begin fighting on the ground!$R$7", ch, NULL, victim, TO_NOTVICT);
          act ("$B$5$n bashes you, but trips and looses his balance and falls on top of you. You begin to ground-fight!!$R$7", ch, NULL, victim, TO_VICT);
          if (CHALLENGE (ch) == 10 && number_range(1,3)  == 2)
            {
              i = number_range (1, 3);
              if (i == 1)
                sprintf (buf, "($B$1Arena$N$7) $B%s tries to bash %s but they both fall to the\n\r($B$1Arena$N$7) $Bground in a bloody tangle of elbows and teeth...", NAME (ch), NAME (victim));
              if (i == 2)
                sprintf (buf, "($B$1Arena$N$7) $B%s falls on his face, trying to bash %s.\n\r($B$1Arena$N$7) $BThey both fall over and begin clawing at each other!", NAME (ch), NAME (victim));
              if (i == 3)
                sprintf (buf, "($B$1Arena$N$7) $B%s tries to bash %s, but %s catches %s\n\r($B$1Arena$N$7) $Bwith an elbow and they both fall to the ground in a bloody mess!", NAME (ch), NAME (victim),
                     NAME (victim), NAME (ch));
              do_arenaecho (ch, buf, TRUE);
            }
          set_fighting (ch, victim);
          victim->fgt->fighting = ch;
          NEW_POSITION(victim, POSITION_GROUNDFIGHTING);
          NEW_POSITION(ch, POSITION_GROUNDFIGHTING);
          return;
        }
    }
  {
    SPELL_DATA *s;
    if ((s = skill_lookup (NULL, gsn_bash)) == NULL)
      {
        fprintf (stderr, "Bash not found!\n");
        return;
      }

    if (victim->data_type == 50)
       return;
    victim->fgt->combat_delay_count = s->casting_time+2;
    if (victim->position != POSITION_FIGHTING)
      victim->fgt->fighting = ch;
    victim->position = POSITION_BASHED;
    set_fighting (ch, victim);
    if ((number_percent() < ch->pcdata->learned[gsn_smash]) &&
    ( ch->pcdata->learned[gsn_smash] > 20))
     {
       victim->fgt->ears = 11 + (s->casting_time);
       ch->fgt->ears = 9 + (s->casting_time * 2);
     }
    else
     {
       victim->fgt->ears = 10 + (s->casting_time);
       ch->fgt->ears = 10 + (s->casting_time *2);
     }
    if((IS_MOB(ch) && number_range(1,10) == 4) ||(IS_PLAYER(ch) && is_member(ch, GUILD_BATTLEMASTER) && is_member(ch, GUILD_WARRIOR) && ch->pcdata->learned[gsn_smash] > 9 && number_percent() < ch->pcdata->learned[gsn_smash]))
      {
	int damm = (number_range(1, LEVEL(ch)))*2/3;
	damage (ch, victim, damm, gsn_smash);
	if (victim->data_type == 50)
	  return;
      }
  }
  return;
}

void
trip (CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (ch->in_room->sector_type == SECT_UNDERWATER)
    {
      return;
    }

  if (RIDING (victim) != NULL)
    return;
  if (
       IS_AFFECTED (victim, AFF_FLYING)
       || IS_IMMORTAL (ch))
    {
      act ("$n tries to trip you, but you float over it!", ch, NULL, victim, TO_VICT_SPAM);
      act ("You try to trip $N, but $N floats over it!", ch, NULL, victim, TO_CHAR);
      act ("$n tries to trip $N, but $N floats over it!", ch, NULL, victim, TO_NOTVICT_SPAM);
      return;
    }

  act ("$B$n trips you and you go down!$R", ch, NULL, victim, TO_VICT);
  act ("You trip $N and $E goes down!", ch, NULL, victim, TO_CHAR);
  act ("$n trips $N!", ch, NULL, victim, TO_NOTVICT);
  victim->fgt->combat_delay_count = UMAX (1, dice (1, 5 - get_curr_dex (victim) / 6));
  WAIT_STATE (ch, PULSE_VIOLENCE);
  WAIT_STATE (victim, PULSE_VIOLENCE);
  return;
}
void
do_kill (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *fch, *fch_next;
  DEFINE_COMMAND ("kill", do_kill, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to start combat or switch targets during combat.")
    check_fgt (ch);
  one_argy (argy, arg);

  if (ch->fgt->ears > 9)
    {
      send_to_char ("You are suffering from the ever-popular bash lag (tm), and therefore\n\rcannot attack another mob/player or use the kill command.\n\r", ch);
      return;
    }
  if (ch->wait > 2) return;


  if (arg[0] == '\0')
    {
      send_to_char ("Syntax: Kill <thing/person to kill>\n\r", ch);
      return;
    }

  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("Who or what is that?\n\r", ch);
   /*   WAIT_STATE(ch, 1*PULSE_VIOLENCE); */
      return;
    }

  if (victim == ch)
    {
      send_to_char ("You hit yourself.. moron!\n\r", ch);
      WAIT_STATE(ch, 2*PULSE_VIOLENCE);
      return;
    }

  if (!can_see (ch, victim))
    {
      send_to_char ("Who or what is that?\n\r", ch);
      return;
    }

  if (is_safe (ch, victim))
    {
      return;
    }

  /* ----------------------------------------- */
  /*	    Protecting Neutral align.	     */
  /* ----------------------------------------- */

  if (IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN(victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
  {
	  send_to_char ("A strange force prevents you from harming this person.\n\r", ch);
	   return;
  }
  if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
  {
		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
		return;
  }

  /* ----------------------------------------- */
  /*	      Protecting LinkDead-NoTimers 	 */
  /* ----------------------------------------- */
  if(IS_PLAYER(victim) && victim->desc == NULL) {
    if(!(victim->pcdata->no_quit_pk > 0)) {
	send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
	return;
    }
  }


 check_same_side_pk(ch, victim);
  if (victim->position == POSITION_GROUNDFIGHTING)
    {
      set_fighting (ch, victim);
      return;
    }

  if (IS_AFFECTED (ch, AFF_CHARM) && MASTER (ch) == victim)
    {
      act ("$N is your master!", ch, NULL, victim, TO_CHAR);
      return;
    }

  for (fch = ch->in_room->more->people; fch != NULL; fch = fch_next)
    {
      fch_next = fch->next_in_room;
      if (IS_MOB (fch) && IS_SET (fch->pIndexData->act3, ACT3_GUARD_MOB)
          && victim->pIndexData != NULL && victim->pIndexData->vnum == fch->pIndexData->guard)
        {
          do_say (fch, "You'll have to get by me first!");
          multi_hit (fch, ch, TYPE_UNDEFINED);
          return;
        }
    }

  if (FIGHTING (ch) && victim == FIGHTING (ch))
    {
      act ("You're already fighting $N!", ch, NULL, victim, TO_CHAR);
      return;
    }

  check_fgt (victim);

  if (ch->position == POSITION_FIGHTING && FIGHTING (ch) != victim)
    {
     if (!IS_AWAKE(victim))
       {
         multi_hit (ch, victim, TYPE_UNDEFINED);
         return;
       }
      act ("Your new target is $N!", ch, NULL, victim, TO_CHAR);
      act ("$n turns and targets $N!", ch, NULL, victim, TO_ROOM);
      WAIT_STATE (ch, PULSE_VIOLENCE);
      set_fighting(ch,victim);
    }
  /*if ((IS_PLAYER (victim)) && IS_AFFECTED (ch, AFF_CHARM))
    {
      return;
    }*/

  if (ch->fgt->combat_delay_count >= 0)
    {

      ch->fgt->combat_delay_count = -1;
    }

  if (victim->fgt->combat_delay_count >= 0)
    {
      victim->fgt->combat_delay_count = -1;
    }


WAIT_STATE (ch, 1 * PULSE_VIOLENCE);
if (IS_MOB (ch) && IS_SET (ch->pIndexData->act4, ACT4_KILLGLADIATOR))
    {
      do_say (ch, "You are not permitted to leave!!");
    }

  set_fighting (ch, victim);
  if (victim->position != POSITION_FIGHTING &&
      victim->position != POSITION_GROUNDFIGHTING &&
      victim->position != POSITION_BASHED)
    victim->fgt->fighting = ch;
  multi_hit (ch, victim, TYPE_UNDEFINED);
  return;
}
void
do_flurry (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("flurry", do_flurry, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to flail around madly in a wild flurry.  Anyone can do this (it uses moves).")
    if (!FIGHTING (ch))
    {
      send_to_char ("You must be fighting to flurry.\n\r", ch);
      return;
    }
  one_argy (argy, arg);

  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't flurry this soon after giving or recieving a bash.\n\r", ch);
      return;
    }

  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("You can't Flurry while groundfighting!\n\r", ch);
      return;
    }

  if (ch->position == POSITION_BASHED)
    {
      send_to_char ("You can't flurry when you have been bashed!\n\r", ch);
      return;
    }

  if (FIGHTING (ch) == NULL)
    {
      send_to_char ("You can't begin a fight with a flurry; you must be already fighting!\n\r", ch);
      return;
    }

  if (ch->move < pow.flurry_mps)
    {
      send_to_char ("You are too exhausted to flurry.\n\r", ch);
      return;
    }
  victim = ch->fgt->fighting;
  check_same_side_pk(ch, victim);
  SUBMOVE(ch,pow.flurry_mps);
  WAIT_STATE (ch, 2 * PULSE_VIOLENCE);
  act ("You open up a wild flurry of multiple attacks!", ch, NULL, victim, TO_CHAR + 1000);
  act ("$n opens up in a wild flurry of multiple attacks!", ch, NULL, victim, TO_ROOM + 1000);
  hitop=FALSE;
  multi_hit (ch, victim, TYPE_UNDEFINED);
   if (FIGHTING(ch) == victim && (IS_MOB(ch)
    ||(ch->pcdata->learned[gsn_flurry] > 0 && number_range(1,170) <
     ch->pcdata->learned[gsn_flurry])))
  multi_hit (ch, victim, TYPE_UNDEFINED);

  hitop = FALSE;
  if(IS_PLAYER(ch) && number_range(1,20) == 3)
     skill_gain(ch, gsn_flurry, TRUE);
  return;
}

void
do_flash (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  I_TOOL *pow;
  char *argg;
  SINGLE_OBJECT *powder;
  DEFINE_COMMAND ("flash", do_flash, POSITION_FIGHTING, 0, LOG_NORMAL,"This command allows a thief to flash behind someone and circle them.")
    if (!FIGHTING (ch))
    {
      send_to_char ("You must be fighting to flash.\n\r", ch);
      return;
    }
  argg = str_dup (argy);
  one_argy (argg, arg);
  if ((victim = FIGHTING(ch)) == NULL)
    {
      send_to_char("You must be fighting someone to flash behind them!\n\r", ch);
      return;
    }


  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't flash this soon after giving or recieving a bash.\n\r", ch);
      return;
    }

  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("You can't flash while groundfighting!\n\r", ch);
      return;
    }
  if (ch->position == POSITION_BASHED)
    {
      send_to_char ("You can't flash when you have been bashed!\n\r", ch);
      return;
    }

  if (FIGHTING (ch) == NULL)
    {
      send_to_char ("You can't begin a fight with a flash; you must be already fighting!\n\r", ch);
      return;
    }
  powder = get_item_held(ch, ITEM_TOOL);
  if (powder == NULL)
    {
      send_to_char("You must be holding flash powder to do this!\n\r",ch);
      return;
    }
  check_same_side_pk(ch, victim);
  SUBMOVE(ch, 20);
  pow = (I_TOOL *) powder->more;
  if (use_tool (powder, TOOL_FLASH_POWDER))
    {
      skill_gain(ch, gsn_flash, TRUE);
      if (number_percent() > ch->pcdata->learned[gsn_flash])
         {
           act ("You disappear in a bright $B$6FLASH$R$7, but reappear in the same spot.", ch, NULL, victim, TO_CHAR);
           act ("$n disappears in a bright $B$6FLASH$R$7, but reappears in the same spot.", ch, NULL, victim, TO_VICT);
           act ("$n disappears in a bright $B$6FLASH$R$7, but reappears in the same spot.", ch, NULL, victim, TO_NOTVICT);
           WAIT_STATE(ch, 3*PULSE_VIOLENCE);
           return;
         }
      else
         {
           act ("You disappear in a bright $B$6FLASH$R$7, and reappear right behind $N!", ch, NULL, victim, TO_CHAR);
           act ("$n disappears in a bright $B$6FLASH$R$7, and reappears behind YOU!", ch, NULL, victim, TO_VICT);
           act ("$n disappears in a bright $B$6FLASH$R$7, and reappears behind $N!", ch, NULL, victim, TO_NOTVICT);
           do_actual_circle(ch, argy, TRUE);
           WAIT_STATE(ch, 5*PULSE_VIOLENCE);

           return;
         }
     }
   else
     {
       send_to_char("You need flash powder to do this.\n\r", ch);
       return;
      }
  return;
}


void
do_bs (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("bs", do_bs, POSITION_STANDING, 0, LOG_NORMAL, "See backstab.")
    do_backstab (ch, argy);
  return;
}

void
do_backstab (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  SINGLE_OBJECT *obj;
  int dt;
  SPELL_DATA *spl;
  DEFINE_COMMAND ("backstab", do_backstab, POSITION_STANDING, 0, LOG_NORMAL, "This command allows you to sneak behind an opponent and place a piercing weapon in his/her/its back.")

    if ((spl = skill_lookup ("Backstab", -1)) == NULL)
      {
	send_to_char ("Huh?\n\r", ch);
	return;
      }

  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Backstab whom?\n\r", ch);
      return;
    }

  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }

  if (victim == ch)
    {
      send_to_char ("How can you sneak up on yourself?\n\r", ch);
      return;
    }

  if (is_safe (ch, victim))
    {
      return;
  }

    /* ----------------------------------------- */
    /*	    Protecting Neutral align.		 */
    /* ----------------------------------------- */

    if (IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN(victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
    {
  	  send_to_char ("A strange force prevents you from harming this person.\n\r", ch);
  	  return;
    }
    if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0  && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
  	{
  		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
  		return;

	}

  /* ----------------------------------------- */
  /*	      Protecting LinkDead-NoTimers 	 */
  /* ----------------------------------------- */
  if(IS_PLAYER(victim) && victim->desc == NULL) {
    if(!(victim->pcdata->no_quit_pk > 0)) {
	send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
	return;
    }
  }


  if ((obj = get_item_held (ch, ITEM_WEAPON)) == NULL)
    {
      send_to_char ("You need to wield a weapon.\n\r", ch);
      return;
    }

  dt = ((I_WEAPON *) obj->more)->attack_type;
  if (attack_table[dt].hit_type != TYPE_PIERCE || !IS_OBJ_STAT(obj, ITEM_PIERCE))
    {
      send_to_char ("You need to wield a piercing weapon.\n\r", ch);
      return;
    }

  if (FIGHTING (victim) != NULL)
    {
      send_to_char ("You can't backstab a person who is already *fighting*.\n\r", ch);
      return;
    }

  if (IS_MOB (victim) && victim->pIndexData->act3 % 32 != 0)
    {
      send_to_char ("But you can't get behind the mob to backstab; it's guarding an exit.\n\r", ch);
      return;
    }

  if (!IS_AWAKE (victim))
    {
      do_actual_backstab (ch, argy);
      return;
    }

  send_to_char ("You begin to stealthily move behind your target...\n\r", ch);
  if (IS_PLAYER (ch))
    ch->pcdata->tickcounts = spl->mana;
  if (IS_MOB (ch))
    {
      do_actual_backstab (ch, argy);
      return;
    }

  if (strlen (argy) > 140)
    return;
  check_temp (ch);
  strcpy (ch->pcdata->temp->temp_string, argy);
  NEW_POSITION(ch, POSITION_BACKSTAB);
  return;
}

void
do_actual_backstab (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  SINGLE_OBJECT *obj;
  int dt;
  SPELL_DATA *spl;
  if ((spl = skill_lookup ("Backstab", -1)) == NULL)
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }

  one_argy (argy, arg);
  if (ch->position == POSITION_FIGHTING)
    return;
  NEW_POSITION(ch, POSITION_STANDING);
  if (arg[0] == '\0')
    {
      send_to_char ("Backstab whom?\n\r", ch);
      return;
    }

  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }

  if (victim == ch)
    {
      send_to_char ("How can you sneak up on yourself?\n\r", ch);
      return;
    }

  if ((obj = get_item_held (ch, ITEM_WEAPON)) == NULL)
    {
      send_to_char ("You need to wield a weapon.\n\r", ch);
      return;
    }

  dt = ((I_WEAPON *) obj->more)->attack_type;
  if (attack_table[dt].hit_type != TYPE_PIERCE || !IS_OBJ_STAT(obj, ITEM_PIERCE))
    {
      send_to_char ("You need to wield a piercing weapon.\n\r", ch);
      return;
    }
  if (number_range(1,3) == 3)
    skill_gain (ch, gsn_backstab, TRUE);

  if (FIGHTING (victim) != NULL)
    {
      send_to_char ("You can't backstab a person who is already *fighting*.\n\r", ch);
      return;
    }
  if (IS_PLAYER(victim) && number_range(1,12) == 3 &&
       number_percent() < victim->pcdata->learned[gsn_awareness])
    {
      act("$n just tried to stab you!", ch, NULL, victim, TO_VICT);
      act("You try to stab $N, but $E notices.", ch, NULL, victim, TO_CHAR);
      skill_gain(victim, gsn_awareness, TRUE);
      return;
    }



  if (IS_AFFECTED (victim, AFF_FLYING) && !IS_AFFECTED (ch, AFF_FLYING))
    {
      /*victim is flying, char is not */
      if (number_range (1, 4) > 1)
        {
          act ("You manage to jump up high enough to get your dagger to $S back...", ch, NULL, victim, TO_CHAR);
        }
      else
        {
          act ("You can't reach $N because $E is flying too high...", ch, NULL, victim, TO_CHAR);
          return;
        }
    }
  check_same_side_pk(ch, victim);

  if (IS_PLAYER (ch))
    ch->pcdata->tickcounts = spl->casting_time;

  if (!IS_AWAKE (victim) || IS_MOB (ch)
      || (IS_PLAYER (ch) && number_percent () < ch->pcdata->learned[gsn_backstab]))
    {
      one_hit(ch, victim, gsn_backstab, obj);
      if (is_member(ch, GUILD_THIEFG) &&
	  is_member(ch, GUILD_ROGUE) &&
    ((victim = FIGHTING(ch)) != NULL)
	  && victim->data_type != 50 &&
	  number_percent () < 
((get_curr_dex(ch)-15)*2+(IS_AUGMENTED(ch,AUG_HAND_SPD) ? 2 : 0)))one_hit (ch, victim, gsn_backstab, obj);
      return;
    }
  else
    damage (ch, victim, 0, gsn_backstab);
  set_fighting(ch, victim);
  return;
}

void
do_flee (CHAR_DATA * ch, char *argy)
{
  ROOM_DATA *room_one;
  ROOM_DATA *room_two;
  EXIT_DATA *pexit;
  int  i;
  CHAR_DATA *victim;
  CHAR_DATA *fch;
  char buf[STD_LENGTH];
  int attempt;
  bool double_flee = TRUE;
  bool door_open[6];
  int num_open = 0;
  int num_chose = 0;
  int door = 0;
  DEFINE_COMMAND ("flee", do_flee, POSITION_GROUNDFIGHTING, 0, LOG_NORMAL, "This command allows you to flee from combat.")

    check_fgt (ch);
  if (!ch || !ch->in_room) return;
  if ((ch->in_room->vnum < 108 && ch->in_room->vnum > 97)|| ch->position < POSITION_GROUNDFIGHTING)
    return;
  if (ch->fgt->ears > 9)
    {
      send_to_char ("You are too off balance from your last combat action to flee!\n\r", ch);
      return;
    }

  if (IS_PLAYER (ch) && ch->pcdata->tickcounts > 1)
    {
      send_to_char ("You fail to find a pause in combat in which to flee!\n\r", ch);
      return;
    }

  if (ch->wait > 0)
    {
      send_to_char ("You are too off-balance and cannot flee.\n\r", ch);
      return;
    }

  if (ch->position == POSITION_BASHED)
    {
      send_to_char ("You try to crawl away, but you are bashed to the ground!\n\rBetter stand up first...\n\r", ch);
      return;
    }

  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("You attempt to stand up...\n\r", ch);
      do_stand (ch, "");
      return;
    }

  if((victim = FIGHTING(ch)) != NULL)
    {
      if (victim->in_room != ch->in_room)
	{
          check_room_more(victim->in_room);
          for (fch = victim->in_room->more->people; fch != NULL; fch = fch->next_in_room)
            {
               check_fgt(fch);
               if (fch->fgt->fighting == ch)
                 fch->fgt->fighting = NULL;
            }
	  ch->fgt->fighting = NULL;
	  ch->position = POSITION_STANDING;
	  ch->wait = 0;
          return;
	}
    }
  if ((victim = FIGHTING(ch))== NULL)
    {
      bool attacked = FALSE;
      if (ch->position == POSITION_BASHED)
        {
          NEW_POSITION(ch, POSITION_STANDING);
          update_pos (ch);
          return;
        }
      check_room_more(ch->in_room);
      for (fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
        {
          if (FIGHTING(fch) == ch)
            {
              attacked = TRUE;
              check_fgt(ch);
              ch->fgt->fighting = fch;
              victim = fch;
              break;
            }
        }
      if (!attacked)
	{
	  NEW_POSITION(ch, POSITION_STANDING);
	  update_pos (ch);
          WAIT_STATE(ch, 3);
	  send_to_char ("You don't have to flee if you're not fighting.\n\r", ch);
	  return;
	}
    }
  if ((number_range (1, 100) < (pow.flee_chance - (IS_AUGMENTED(ch,AUG_FOOT_SPD) ? 5 : 0) - (IS_AUGMENTED(ch, AUG_LEG_SPD) ? 5 : 0) )))
    {
      WAIT_STATE(ch, 5);
      send_to_char("You couldn't manage to flee from combat!\n\r",ch);
      return;
    }

  if (IS_MOB (ch))
    {
      if (number_range (1, 2) == 2)
        {
          act ("$n tries to escape, but can't get away!", ch, NULL, ch, TO_ROOM);
          return;
        }
      if (victim->position == POSITION_DEAD)
        {
          stop_fighting (victim, TRUE);
          ch->wait = 0;
          return;
        }
      if (ch->position == POSITION_FIGHTING)
      NEW_POSITION(ch, POSITION_STANDING);
    }
	entlab = 500;

  if (IS_PLAYER (ch) && rchars_in_group (ch) > 1)
    double_flee = FALSE;
  room_one = ch->in_room;
entlab = 501;
  num_open = 0;
  for (attempt = 0; attempt < 6; attempt++)
    {
      door_open[attempt] = FALSE;
      if ((pexit = room_one->exit[attempt]) != NULL
          && (ROOM_DATA *) pexit->to_room != NULL
	  && (ROOM_DATA *) pexit->to_room != room_one
          && ((ROOM_DATA *) pexit->to_room)->sector_type != SECT_WATER_NOSWIM
          && (!pexit->d_info || !IS_SET (pexit->d_info->exit_info, EX_CLOSED)))
	{
	  door_open[attempt] = TRUE;
	  num_open++;
	}
    }
entlab = 502;
  if (num_open == 0)
    {
      send_to_char("Ack! It won't let you get away!\n\r", ch);
      return;
    }
 if (num_open  >= 2)
  num_chose = number_range(1,num_open);
 else num_chose = 1;
  door = 0;
entlab = 503;
	  for(i = 0; i < 6; i ++)
    {
      if (door_open[i] == TRUE)
	{
	  door++;
	  if (num_chose == door)
	    {
	      door = i;
	      break;
	    }
	}
    }
entlab = 504;
  if((pexit = ch->in_room->exit[door]) != NULL
      && pexit->to_room != NULL)
    room_two = (ROOM_DATA *) pexit->to_room;
  if (RIDING (ch) != NULL)
    {
      send_to_char ("You fall from your mount as you try to escape!\n\r", ch);
      check_fgt (ch->fgt->riding);
      ch->fgt->riding->fgt->mounted_by = NULL;
      ch->fgt->riding = NULL;
    }
  if (MOUNTED_BY (ch) != NULL)
    {
      send_to_char ("Your rider falls off as you try to escape!\n\r", ch);
      send_to_char ("Your mount bolts from under you!\n\r", MOUNTED_BY (ch));
      check_fgt (ch->fgt->mounted_by);
      ch->fgt->mounted_by->fgt->riding = NULL;
      ch->fgt->mounted_by = NULL;
    }
entlab = 505;
  if(move_char (ch, door + 20))
    {
      int jj = 0;
      entlab = 1419;
      if (ch->data_type == 50) return;
      entlab = 1420;
      check_fgt(ch);
      entlab = 1421;
      ch->fgt->fighting = NULL;
      ch->position = POSITION_STANDING;
      ch->fgt->combat_delay_count = -1;
entlab = 506;
      sprintf (buf, "\x1B[1;32m%s flees %s in panic!\x1B[0m\n\r", capitalize (NAME (ch)), dir_name[door]);
      check_room_more(room_one);
      for (fch = room_one->more->people; fch != NULL; fch = fch->next_in_room)
	{
          check_fgt(fch);
	  if (fch->fgt->fighting == ch)
	    {
	      fch->fgt->fighting = NULL;
	      if (fch->position != POSITION_BASHED)
                fch->position = POSITION_STANDING;
	    }
	  send_to_char(buf, fch);
	}
entlab = 507;

      check_ced(victim);
      for (jj = 0; jj < 4; jj++)
	if (victim->ced->attackers[jj] == ch)
	  victim->ced->attackers[jj] = NULL;
      if(ch->in_room != room_two)
	return;
entlab = 508;
      if (double_flee && IS_PLAYER(ch))
	{
	  room_two = ch->in_room;
	  num_open = 0;
	  door = 0;
	  num_chose = 0;
	  for (attempt = 0; attempt < 6; attempt++)
	    {
	      door_open[attempt] = FALSE;

	      if ((pexit = room_two->exit[attempt]) != NULL
		  && (ROOM_DATA *) pexit->to_room != NULL
		  && (ROOM_DATA *) pexit->to_room != room_one
		  && (ROOM_DATA *) pexit->to_room != room_two
		  && ((ROOM_DATA *) pexit->to_room)->sector_type != SECT_WATER_NOSWIM
		  && (!pexit->d_info || !IS_SET (pexit->d_info->exit_info, EX_CLOSED)))
		{
		  door_open[attempt] = TRUE;
		  num_open++;
		}
	    }
entlab = 509;
	  if (num_open == 0)
	    {
	      return;
	    }
	  num_chose = number_range(1,num_open);
	  for(i = 0; i < 6; i ++)
	    {
	      if (door_open[i] == TRUE)
		{
		  door++;
		  if (num_chose == door)
		    {
		      door = i;
		      break;
		    }
		}
	    }
	  move_char (ch, door + 20);
          entlab = 1422;
          if (ch->data_type == 50) return;
          entlab = 1423;
	}
entlab = 510;
      if (IS_PLAYER (ch))
	{
	  send_to_char ("Run away!  You flee head over heels!\n\r", ch);
	  if (ch->fgt)
	    ch->fgt->fighting = NULL;
	  ch->position = POSITION_STANDING;
          ch->fgt->combat_delay_count = -1;
          WAIT_STATE(ch, 3);
	  gain_exp (ch, -10);
	}
entlab = 511;
	stop_fighting (ch, FALSE);
      return;
    }
  else
    {
      WAIT_STATE(ch, PULSE_VIOLENCE);
      send_to_char("You can't get away!\n\r", ch);
    }
  return;
}


void
do_guard (CHAR_DATA * ch, char *argy)
{
  char arg1[STD_LENGTH];
  CHAR_DATA *victim;
  int i;
DEFINE_COMMAND ("guard", do_guard, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to guard someone.")
  if (IS_MOB(ch))
  return;

argy = one_argy(argy, arg1);
if (FIGHTING(ch) != NULL || ch->position != POSITION_STANDING)
  {
    send_to_char("You must be standing to guard someone!\n\r", ch);
    return;
  }
if(arg1[0] == '\0' || arg1 == "")
  {
    send_to_char("Guard <name>\n\r", ch);
    return;
  }
if ((victim = get_char_room(ch, arg1)) == NULL)
  {
    send_to_char("Guard <name>\n\r", ch);
    return;
  }
if (IS_MOB(victim))
  {
    send_to_char("You may only guard other players.\n\r", ch);
    return;
  }
if (DIFF_ALIGN(ch,victim))
  {
    send_to_char("You may not guard people of the opposite alignment.\n\r", ch);
    return;
  }
if (ch->pcdata->learned[gsn_guard] < 50)
  {
    send_to_char("You are not experienced enough at guarding to attempt this!\n\r", ch);
    return;
  }
if (victim->pcdata->guarding != NULL)
  {
    send_to_char("You cannot guard someone who is already guarding someone else.\n\r", ch);
    return;
  }
for (i = 0; i < 4; i++)
  {
    if (ch->pcdata->guarded_by[i] != NULL)
      {
	send_to_char("You cannot guard someone if you are already being guarded.\n\r", ch);
	return;
      }
  }
if(ch->pcdata->guarding != NULL)
  {
    send_to_char("You are already guarding someone else. Type Unguard first.\n\r", ch);
    return;
  }
for (i = 0; i < 4; i ++)
  {
    if (victim->pcdata->guarded_by[i] == NULL)
      {
	ch->pcdata->guarding = victim;
	victim->pcdata->guarded_by[i] = ch;
	act("You are now guarding $N.", ch, NULL, victim, TO_CHAR);
	act("$n are now guarding you.", ch, NULL, victim, TO_VICT);
	act("$n is now guarding $N.", ch, NULL, victim, TO_NOTVICT);

	return;
      }
  }

send_to_char("That person already has too many people guarding them.\n\r", ch);
return;

}

void
do_unguard (CHAR_DATA *ch, char *argy)
{
  CHAR_DATA *victim;
  int i;
  DEFINE_COMMAND ("unguard", do_unguard, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to stop guarding someone.")
    if(IS_MOB(ch)) return;
  if(ch->pcdata->guarding == NULL)
    {
      send_to_char("You are not guarding anyone.\n\r", ch);
      return;
    }
  if (FIGHTING(ch) != NULL || ch->position != POSITION_STANDING)
    {
      send_to_char("You must be standing to unguard someone.\n\r", ch);
      return;
    }
  victim = (CHAR_DATA *) ch->pcdata->guarding;
  ch->pcdata->guarding = NULL;
  if (IS_MOB(victim)) return;
  for (i = 0; i < 4; i++)
    {
      if (victim->pcdata->guarded_by[i] == ch)
	{
	  victim->pcdata->guarded_by[i] = NULL;
	  act("You stop guarding $N.", ch, NULL, victim, TO_CHAR);
	  act("$n stops guarding you.", ch, NULL, victim, TO_VICT);
	  act("$n stops guarding $N.", ch, NULL, victim, TO_NOTVICT);
	  return;
	}
    }
return;
}



void
do_rescue (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *fch;
  int temp;
  int temp2;
  DEFINE_COMMAND ("rescue", do_rescue, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to rescue someone getting whacked.  Success is based on your rescue skill.")

    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Who is that?\n\r", ch);
      return;
    }
  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("Who is that?\n\r", ch);
      return;
    }
  if (DIFF_ALIGN(ch, victim))
    {
      send_to_char ("Why would you want to do that?\n\r", ch);
      return;
    }
  if (ch->position != POSITION_STANDING && ch->position != POSITION_FIGHTING)
    {
      send_to_char ("You must be fighting or standing to rescue someone.\n\r", ch);
      return;
    }
  if (victim == ch)
    {
      send_to_char ("You rescue yourself.  That did a lot of good.\n\r", ch);
      return;
    }
  if (IS_PLAYER (ch) && IS_MOB (victim))
    {
      send_to_char ("That person doesn't want your help!\n\r", ch);
      return;
    }
  if (FIGHTING (ch) == victim)
    {
      send_to_char ("You rescue yourself.  That was useful.\n\r", ch);
      return;
    }
  if ((fch = FIGHTING (victim)) == NULL)
    {
      send_to_char ("That person is not fighting right now.\n\r", ch);
      return;
    }
  if (victim->position != POSITION_FIGHTING)
    {
      send_to_char ("That person isn't in a normal fighting position.\n\r", ch);
      return;
    }
  if (fch->position != POSITION_FIGHTING)
    {
      send_to_char ("That person's opponent is not in a normal fighting position!\n\r", ch);
      return;
    }
  if (victim->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("That person is groundfighting! You can't rescue!\n\r", ch);
      return;
    }

  if (victim->position == POSITION_GROUNDFIGHTING)
    {
      if (ch->position == POSITION_GROUNDFIGHTING)
        {
          send_to_char ("You are groundfighting! You can't rescue!\n\r", ch);
          return;
        }
      set_fighting (ch, FIGHTING (victim));
      return;
    }
  WAIT_STATE (ch, 15);
  if (IS_PLAYER (ch) && number_percent () > ch->pcdata->learned[gsn_rescue])
    {
      send_to_char ("You attempt the rescue but fail.\n\r", ch);
      return;
    }
  act ("You rescue $N!", ch, NULL, victim, TO_CHAR);
  act ("$n rescues you!", ch, NULL, victim, TO_VICT);
  act ("$n rescues $N!", ch, NULL, victim, TO_NOTVICT);
  check_fgt (fch);
  check_fgt (ch);
  check_fgt (victim);
  temp = victim->fgt->combat_delay_count;
  temp2 = fch->fgt->combat_delay_count;

  if (IS_PLAYER (ch))
    {
      stop_fighting (fch, FALSE);
      stop_fighting (victim, FALSE);
    }
  {
    int jj = 0;
    check_ced(fch);
    for (jj = 0; jj < 4; jj++)
      if (fch->ced->attackers[jj] == victim)
	fch->ced->attackers[jj] = NULL;
  }
  check_fgt (fch);
  check_fgt (victim);
  check_same_side_pk(ch, fch);
  set_fighting (ch, fch);
  set_fighting (fch, ch);
  check_fgt (fch);
  check_fgt (victim);
  victim->fgt->combat_delay_count = temp;
  fch->fgt->combat_delay_count = temp2;
  WAIT_STATE (ch, 22);
  if (IS_PLAYER (ch)) {
    skill_gain (ch, gsn_rescue, TRUE);
  }
  victim->fgt->combat_delay_count = temp;
  fch->fgt->combat_delay_count = temp2;
  return;
}

void
do_tornado (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  CHAR_DATA *fch;
  bool notft = FALSE;
  int dl = 0;
  int chance;
  DEFINE_COMMAND ("tornado", do_tornado, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to tornado kick an opponent for multiple hits.")
    check_fgt (ch);
  if (!is_member(ch, GUILD_MONK)) {
   send_to_char("Huh?\n\r", ch);
   return;
   }
  if (ch->move < 1) {
   send_to_char("You do not have enough energy to use this skill.\n\r", ch);
   return;
   }
  if (!FIGHTING(ch)) {
   send_to_char("You cannot start combat with a tornado kick.  Use KILL instead.\n\r",ch);
   return;
   }

  if (ch->wait > 2)
    return;
  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't tornado kick this soon after a bash.\n\r", ch);
      return;
    }
  if (FIGHTING (ch) == NULL)
    {
      notft = TRUE;
      if (argy[0] == '\0' || argy == "")
        {
          send_to_char ("Who?\n\r", ch);
/*	 WAIT_STATE(ch, 1*PULSE_VIOLENCE); */
          return;
        }
      if ((victim = get_char_room (ch, argy)) == NULL)
        {
          send_to_char ("Who?\n\r", ch);
/*        WAIT_STATE(ch, 1*PULSE_VIOLENCE); */
	 return;
        }
      if (victim == ch)
        {
          send_to_char ("You kick yourself in the butt, but it doesn't hurt that much.\n\r", ch);
	WAIT_STATE(ch, 5*PULSE_VIOLENCE);
          return;
        }

    /* ----------------------------------------- */
    /*	    Protecting Neutral align.		 */
    /* ----------------------------------------- */

    if (IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN(victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
    {
  	  send_to_char ("A strange force prevents you from harming this person.\n\r", ch);
  	  return;
    }
    if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
  	{
  		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
  		return;

	}

    /* ----------------------------------------- */
    /*	      Protecting LinkDead-NoTimers 	 */
    /* ----------------------------------------- */
    if(IS_PLAYER(victim) && victim->desc == NULL) {
      if(!(victim->pcdata->no_quit_pk > 0)) {
	send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
	return;
      }
    }


  }
  else
    victim = ch->fgt->fighting;

  if (victim->data_type==50) return;

  if (is_safe (ch, victim))
    {
      return;
  }

/*
  if (!DIFF_ALIGN(ch, victim) && ALIGN(ch) == 2 && IS_PLAYER(victim) &&
!IS_MOB(ch) && !IN_BATTLE(ch) && !IN_BATTLE(victim))
    {
      send_to_char ("You can not attack members of the same alignment!\n\r", ch);
      return;
    }
*/
   check_same_side_pk(ch, victim);
  check_ced (ch);
  check_ced (victim);
  if (!(IS_IN_MELEE (ch, victim)))
    {
      send_to_char ("You aren't on the front lines of that battle!\n\r", ch);
      return;
    }
  if (ch->position == POSITION_BASHED)
    {
      send_to_char ("You try to kick, but from the ground you can't hit your opponent.\n\r", ch);
      return;
    }
  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("You can't kick when you are locked into mortal combat on the ground!\n\r", ch);
      return;
    }
    dl = 105;

  if (notft)
    dl = (PULSE_VIOLENCE);
  WAIT_STATE (ch, dl);
  set_fighting (ch, victim);

    act("$s explodes into an agile tornado kick!", ch, NULL,victim, TO_NOTVICT);
    act ("You explode into an agile tornado kick!", ch,NULL,victim, TO_CHAR);
    act ("$n explodes into an agile tornado kick! ", ch, NULL,victim, TO_VICT);
    SUBMOVE(ch,pow.flurry_mps);
    if (IS_PLAYER (ch) && number_range (1, 5) == 2)
      skill_gain (ch, gsn_tornado, TRUE);

  for (fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
  {  
    if (fch->pcdata->alignment > 0 && fch->pcdata->alignment != ch->pcdata->alignment)
    {
      victim = fch;
      chance = 0;
      chance += (ch->pcdata->learned[gsn_tornado] + ch->pcdata->learned[gsn_kick])/2;
      if (IS_AUGMENTED(ch, AUG_LEG_POWER))
	chance += 2;
      if (IS_AUGMENTED(ch, AUG_FOOT_POWER))
	chance += 2;
      if (IS_AUGMENTED(ch, AUG_LEG_SPD))
	chance += 3;
      if (IS_AUGMENTED(ch, AUG_FOOT_SPD))
	chance += 3;
      if (IS_PLAYER(victim) && (victim->pcdata->learned[gsn_steadiness] > 10))
	{
	  chance -= (victim->pcdata->learned[gsn_steadiness]/6);
	  skill_gain(victim, gsn_steadiness, TRUE);
	}
      if (IS_PLAYER(victim) && (victim->pcdata->learned[gsn_block] > 10))
	{
	  if (number_percent() < (victim->pcdata->learned[gsn_block]/3))
	    {
	      act("$N blocks $s's kick and twists away.", ch, NULL,victim, TO_NOTVICT);
	      act ("$N blocks your kick and twists away.", ch,NULL,victim, TO_CHAR);
	      act ("You block $n's kick and twist away.", ch, NULL,victim, TO_VICT);
              if (number_range(1,10) == 3)
                skill_gain(victim, gsn_block, TRUE);
	      return;
	    }
	}
      if (IS_MOB (ch) || (!IS_AWAKE (victim) || (IS_PLAYER (ch) && number_percent () < chance)))
        {
          char buf[500];
          int i;
          int damm;
          damm = translate (damt.kick_dam, LEVEL (ch), ch);
	  if (IS_AUGMENTED(ch, AUG_LEG_POWER)) damm += number_range(1,4);
	  if (IS_AUGMENTED(ch, AUG_FOOT_POWER)) damm += number_range(3,6);
          if (IS_PLAYER (ch) && number_range (1, 3) == 2)
            skill_gain (ch, gsn_kick, TRUE);
	  if (ch->in_room != victim->in_room || victim->data_type == 50)
	    return;
          show_hitdam (gsn_kick, "kick", damm, ch, victim);
          damage (ch, victim, damm/2, gsn_kick);
        }
      else
        {
          set_fighting (ch, victim);
          if (FIGHTING (victim) == NULL)
            victim->fgt->fighting = ch;
          act ("You miss a kick at $N.", ch, NULL, victim, TO_CHAR + 1000);
          act ("$n misses a kick at $N.", ch, NULL, victim, TO_NOTVICT + 1000);
          act ("$n tries to kick you but misses.", ch, NULL, victim, TO_VICT + 1000);
        }
    }
  }
  return;
} /* END TORNADO KICK */

void
do_palm (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  bool notft = FALSE;
  int dl = 0;
  int chance;
  DEFINE_COMMAND ("palm", do_palm, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to strike at your opponent's heart with your palm.")
    check_fgt (ch);
  if (!is_member(ch, GUILD_MONK)) {
   send_to_char("Huh?\n\r",ch);
   return;
   }
  if (ch->move < 1) {
   send_to_char("You don't have enough energy.\n\r",ch);
   return;
   }
  if (!FIGHTING(ch)) {
   send_to_char("You cannot start combat with a palm.  Use KILL instead.\n\r",ch);
   return;
   }

  if (ch->wait > 2)
    return;
  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't palm this soon after a bash.\n\r", ch);
      return;
    }
  if (FIGHTING (ch) == NULL)
    {
      notft = TRUE;
      if (argy[0] == '\0' || argy == "")
        {
          send_to_char ("Who?\n\r", ch);
/*	 WAIT_STATE(ch, 1*PULSE_VIOLENCE); */
          return;
        }
      if ((victim = get_char_room (ch, argy)) == NULL)
        {
          send_to_char ("Who?\n\r", ch);
/*        WAIT_STATE(ch, 1*PULSE_VIOLENCE); */
	 return;
        }
      if (victim == ch)
        {
          send_to_char ("Your elbows aren't that flexible.\n\r", ch);
	WAIT_STATE(ch, 5*PULSE_VIOLENCE);
          return;
        }
    /* ----------------------------------------- */
    /*	    Protecting Neutral align.		 */
    /* ----------------------------------------- */

    if (IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN(victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
    {
  	  send_to_char ("A strange force prevents you from harming this person.\n\r", ch);
  	  return;
    }
    if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
  	{
  		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
  		return;
	}
    /* ----------------------------------------- */
    /*	      Protecting LinkDead-NoTimers 	 */
    /* ----------------------------------------- */
    if(IS_PLAYER(victim) && victim->desc == NULL) {
      if(!(victim->pcdata->no_quit_pk > 0)) {
	send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
	return;
      }
    }
  }
  else
    victim = ch->fgt->fighting;

  if (victim->data_type==50) return;

  if (is_safe (ch, victim))
    {
      return;
  }

/*
  if (!DIFF_ALIGN(ch, victim) && ALIGN(ch) == 2 && IS_PLAYER(victim) &&
!IS_MOB(ch) && !IN_BATTLE(ch) && !IN_BATTLE(victim))
    {
      send_to_char ("You can not attack members of the same alignment!\n\r", ch);
      return;
    }
*/
   check_same_side_pk(ch, victim);
  check_ced (ch);
  check_ced (victim);
  if (!(IS_IN_MELEE (ch, victim)))
    {
      send_to_char ("You aren't on the front lines of that battle!\n\r", ch);
      return;
    }
  if (ch->position == POSITION_BASHED)
    {
      send_to_char ("You try to kick, but from the ground you can't hit your opponent.\n\r", ch);
      return;
    }
  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("You can't kick when you are locked into mortal combat on the ground!\n\r", ch);
      return;
    }
    dl = 110;   //changed from 200

  if (notft)
    dl = (PULSE_VIOLENCE);
  WAIT_STATE (ch, dl);
  set_fighting (ch, victim);
  chance = 0;
  chance += ch->pcdata->learned[gsn_buddha] - 10;
  if (IS_AUGMENTED(ch, AUG_HAND_POWER))
    chance += 5;
  if (IS_AUGMENTED(ch, AUG_HAND_SPD))
    chance += 5;
  if (IS_AUGMENTED(victim, AUG_LEG_SPD))
    chance -= 5;
  if (IS_AUGMENTED(victim, AUG_FOOT_SPD))
    chance -= 5;

  if (IS_MOB (ch) || (!IS_AWAKE (victim) || (IS_PLAYER (ch) && number_percent () < chance)))
    {
    char buf[500];
    int damm;
    damm = (5/2)*(get_curr_str(ch) + get_curr_wis(ch) + get_curr_dex(ch));
    if (IS_AUGMENTED(ch, AUG_HAND_POWER)) damm += 3;
    if (IS_AUGMENTED(ch, AUG_ARM_POWER)) damm += 3;
    if (IS_AUGMENTED(ch, AUG_BODY_POWER)) damm += 3;
    if (IS_AUGMENTED(ch, AUG_HAND_SPD)) damm += 3;

    if (IS_PLAYER (ch) && number_range (1, 10) == 2)
      skill_gain (ch, gsn_buddha, TRUE);
    if (ch->in_room != victim->in_room || victim->data_type == 50)
      return;
    //Changed from SUBMOVE(ch, 100);  Sabelis 1-19-2006
    SUBMOVE(ch, 50);
    act ("$B$0You channel all of your energy into your palm and $B$7strike$B$0!", ch, NULL, victim, TO_CHAR);
    act ("$B$0$n $B$7strikes $B$0$N with a devastating $B$7buddhas palm$B$0!", ch, NULL, victim, TO_NOTVICT);
    act ("$B$0$n strikes at your heart with a $B$7buddhas palm$B$0!", ch, NULL, victim, TO_VICT);
    damage (ch, victim, number_range(damm/3, damm), gsn_buddha);
    }
    else
    {
      set_fighting (ch, victim);
      if (FIGHTING (victim) == NULL)
        victim->fgt->fighting = ch;
      act ("You fail to execute the buddhas palm properly.", ch, NULL, victim, TO_CHAR + 1000);
      act ("$n fails to execute the buddhas palm properly.", ch, NULL, victim, TO_NOTVICT + 1000);
      act ("$n tries to strike you with a buddhas palm, but fails.", ch, NULL, victim, TO_VICT + 1000);
    }
  return;
} 
/* END BUDDHAS PALM */

void
do_nerve (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  bool notft = FALSE;
  int dl = 0;
  int chance;
  DEFINE_COMMAND ("nerve", do_nerve, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to strike a pressure point on an opponent.")
  if (!is_member(ch, GUILD_MONK)) {
   send_to_char("Huh?\n\r",ch);
   return;
   }
    check_fgt (ch);

  if (ch->wait > 2)
    return;
  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't nerve strike this soon after a bash.\n\r", ch);
      return;
    }
  if (FIGHTING (ch) == NULL)
    {
      notft = TRUE;
      if (argy[0] == '\0' || argy == "")
        {
          send_to_char ("Who?\n\r", ch);
 	 WAIT_STATE(ch, 1*PULSE_VIOLENCE);
          return;
        }
      if ((victim = get_char_room (ch, argy)) == NULL)
        {
          send_to_char ("Who?\n\r", ch);
        WAIT_STATE(ch, 1*PULSE_VIOLENCE);
	 return;
        }
      if (victim == ch)
        {
          send_to_char ("You get on your own nerves.\n\r", ch);
	WAIT_STATE(ch, 5*PULSE_VIOLENCE);
          return;
        }

    /* ----------------------------------------- */
    /*	    Protecting Neutral align.		 */
    /* ----------------------------------------- */

    if (IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN(victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
    {
  	  send_to_char ("A strange force prevents you from harming this person.\n\r", ch);
  	  return;
    }
    if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
  	{
  		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
  		return;

	}

    /* ----------------------------------------- */
    /*	      Protecting LinkDead-NoTimers 	 */
    /* ----------------------------------------- */
    if(IS_PLAYER(victim) && victim->desc == NULL) {
      if(!(victim->pcdata->no_quit_pk > 0)) {
	send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
	return;
      }
    }


  }
  else
    victim = ch->fgt->fighting;

  if (victim->data_type==50) return;

  if (is_safe (ch, victim))
  {
    return;
  }
  if (!can_see (ch, victim))
    {
      send_to_char ("You cannot see their pressure points!\n\r", ch);
      return;
    }

/*
  if (!DIFF_ALIGN(ch, victim) && ALIGN(ch) == 2 && IS_PLAYER(victim) &&
!IS_MOB(ch) && !IN_BATTLE(ch) && !IN_BATTLE(victim))
    {
      send_to_char ("You can not attack members of the same alignment!\n\r", ch);
      return;
    }
*/
   check_same_side_pk(ch, victim);
  check_ced (ch);
  check_ced (victim);
  if (!(IS_IN_MELEE (ch, victim)))
    {
      send_to_char ("You aren't on the front lines of that battle!\n\r", ch);
      return;
    }
  if (ch->position == POSITION_BASHED)
    {
      send_to_char ("You try to kick, but from the ground you can't hit your opponent.\n\r", ch);
      return;
    }
  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("You can't nerve strike when you are locked into mortal combat on the ground!\n\r", ch);
      return;
    }
    dl = 62;

  if (notft)
    dl = (PULSE_VIOLENCE);
  WAIT_STATE (ch, dl);
  set_fighting (ch, victim);

      chance = 0;
      chance += ch->pcdata->learned[gsn_nerve]/2;
      if (IS_AUGMENTED(ch, AUG_HAND_SPD))
	chance += 10;
      if (IS_AUGMENTED(ch, AUG_HAND_POWER))
	chance += 10;
      if (IS_AUGMENTED(ch, AUG_ARM_POWER))
	chance += 10;
       if ((get_curr_str(ch) + get_curr_dex(ch) + get_curr_wis(ch))/3 > 23)
	chance += 10;
      if (!IS_AWAKE(victim))
        chance = 100;
      if (IS_MOB (ch) || (!IS_AWAKE (victim) || (IS_PLAYER (ch) && number_percent () < chance)))
        {
          char buf[500];
          int i;
          int damm;
          int statbonus = get_curr_wis(ch) + get_curr_str(ch) + get_curr_dex(ch) - 20;
          damm = number_range(10, statbonus);
	  if (IS_AUGMENTED(ch, AUG_BODY_POWER)) damm += number_range(1,5);
	  if (IS_AUGMENTED(ch, AUG_HAND_POWER)) damm += number_range(1,5);
	  if (IS_AUGMENTED(ch, AUG_ARM_POWER)) damm += number_range(1,5);
          if (!IS_AWAKE(victim))
            damm *= 4;
          if (IS_PLAYER (ch) && number_range (1, 4) == 2)
            skill_gain (ch, gsn_nerve, TRUE);
	  if (ch->in_room != victim->in_room || victim->data_type == 50)
	    return;

          damage (ch, victim, damm, gsn_nerve);
          if (IS_PLAYER(victim) && victim->position == POSITION_CASTING && damm > 50)
	    {
	      NEW_POSITION(victim, POSITION_FIGHTING);
	      send_to_char ("OUCH! You just lost your concentration!\n\r", victim);
	      victim->pcdata->tickcounts = 0;
	    }
        }
      else
        {
          set_fighting (ch, victim);
          if (FIGHTING (victim) == NULL)
            victim->fgt->fighting = ch;
          act ("You miss $N's pressure point.", ch, NULL, victim, TO_CHAR + 1000);
          act ("$n misses $N's pressure point.", ch, NULL, victim, TO_NOTVICT + 1000);
          act ("$n misses your pressure point.", ch, NULL, victim, TO_VICT + 1000);
        }
  return;
} /* END NERVE STRIKE */

void
do_kick (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  int ii, i;
  bool notft = FALSE;
  int dl = 0;
  DEFINE_COMMAND ("kick", do_kick, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to kick an opponent.  Success is based on kicking skill, power is based on level.")
    check_fgt (ch);
/*if (!FIGHTING(ch)) {
   send_to_char("You cannot start combat with a kick.  Use KILL instead.\n\r",ch);
   return;
   } */

  if (ch->wait > 2)
    return;
  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't kick this soon after a bash.\n\r", ch);
      return;
    }
  if (FIGHTING (ch) == NULL)
    {
      notft = TRUE;
      if (argy[0] == '\0' || argy == "")
        {
          send_to_char ("Who?\n\r", ch);
/*	 WAIT_STATE(ch, 1*PULSE_VIOLENCE); */
          return;
        }
      if ((victim = get_char_room (ch, argy)) == NULL)
        {
          send_to_char ("Who?\n\r", ch);
/*        WAIT_STATE(ch, 1*PULSE_VIOLENCE); */
	 return;
        }
      if (victim == ch)
        {
          send_to_char ("You kick yourself in the butt, but it doesn't hurt that much.\n\r", ch);
	WAIT_STATE(ch, 5*PULSE_VIOLENCE);
          return;
        }

    /* ----------------------------------------- */
    /*	    Protecting Neutral align.		 */
    /* ----------------------------------------- */

    if (IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN(victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
    {
  	  send_to_char ("A strange force prevents you from harming this person.\n\r", ch);
  	  return;
    }
    if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
  	{
  		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
  		return;

	}

    /* ----------------------------------------- */
    /*	      Protecting LinkDead-NoTimers 	 */
    /* ----------------------------------------- */
    if(IS_PLAYER(victim) && victim->desc == NULL) {
      if(!(victim->pcdata->no_quit_pk > 0)) {
	send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
	return;
      }
    }


  }
  else
    victim = ch->fgt->fighting;

  if (victim->data_type==50) return;

  if (is_safe (ch, victim))
    {
      return;
  }

/*
  if (!DIFF_ALIGN(ch, victim) && ALIGN(ch) == 2 && IS_PLAYER(victim) &&
!IS_MOB(ch) && !IN_BATTLE(ch) && !IN_BATTLE(victim))
    {
      send_to_char ("You can not attack members of the same alignment!\n\r", ch);
      return;
    }
*/
   check_same_side_pk(ch, victim);
  check_ced (ch);
  check_ced (victim);
  if (!(IS_IN_MELEE (ch, victim)))
    {
      send_to_char ("You aren't on the front lines of that battle!\n\r", ch);
      return;
    }
  if (ch->position == POSITION_BASHED)
    {
      send_to_char ("You try to kick, but from the ground you can't hit your opponent.\n\r", ch);
      return;
    }
  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("You can't kick when you are locked into mortal combat on the ground!\n\r", ch);
      return;
    }
  if (kickable (victim) == 0)
    {
      act ("You can't kick that type of creature!", ch, NULL, victim, TO_CHAR);
      return;
    }
    dl = 43;

  if (notft)
    dl = (PULSE_VIOLENCE);
  WAIT_STATE (ch, dl);
  if (kickable (victim) == 2)
    {
      act ("You try to kick $N, but $E flies out of the way!", ch, NULL, victim, TO_CHAR);
      return;
    }
  set_fighting (ch, victim);
  if (((IS_PLAYER(ch) && is_member(ch, GUILD_BATTLEMASTER)) &&
number_percent()<(ch->pcdata->learned[gsn_double_kick]/5)) ||
(IS_MOB(ch)&&number_percent() < LEVEL(ch)/9))
   {
    i = 2;
    if (IS_PLAYER(ch) && number_range(1,20) == 4)
      skill_gain(ch, gsn_double_kick, TRUE);
   }
  else
    i = 1;
  for (; i > 0; i--)
    {
      int chance = 0;
      chance += ch->pcdata->learned[gsn_kick];
      if (is_member(ch, GUILD_WARRIOR))
	chance += 5;
      if (is_member(ch, GUILD_BATTLEMASTER))
	chance += 5;
      if (IS_PLAYER(victim) &&
(victim->pcdata->learned[gsn_steadiness] > 10))
	{
	  chance -= (victim->pcdata->learned[gsn_steadiness]/4);
	  skill_gain(victim, gsn_steadiness, TRUE);
	}
      if (IS_PLAYER(victim) && (victim->pcdata->learned[gsn_block] > 10))
	{
	  if (number_percent() < (victim->pcdata->learned[gsn_block]/3))
	    {
	      act("$N blocks $s's kick and twists away.", ch, NULL,victim, TO_NOTVICT);
	      act ("$N blocks your kick and twists away.", ch,NULL,victim, TO_CHAR);
	      act ("You block $n's kick and twist away.", ch, NULL,victim, TO_VICT);
              if (number_range(1,10) == 3)
                skill_gain(victim, gsn_block, TRUE);
	      return;
	    }
	}
      if (IS_MOB (ch) || (!IS_AWAKE (victim) || (IS_PLAYER (ch) && number_percent () < chance)))
        {
          char buf[500];
          int i;
          int damm;
          damm = translate (damt.kick_dam, LEVEL (ch), ch);
	  if (IS_AUGMENTED(ch, AUG_LEG_POWER)) damm += number_range(1,4);
	  if (IS_AUGMENTED(ch, AUG_FOOT_POWER)) damm += number_range(3,6);
          if (IS_PLAYER (ch) && number_range (1, 3) == 2)
            skill_gain (ch, gsn_kick, TRUE);
	  if (ch->in_room != victim->in_room || victim->data_type == 50)
	    return;
          if (CHALLENGE (ch) == 10 && number_range(1,3) == 2)
            {
              i = number_range (1, 3);
              if (i == 1)
                sprintf (buf, "($B$1Arena$N$7) $B%s kicks %s in the ribs... snap crackle pop!", NAME (ch), NAME (victim));
              if (i == 2)
                sprintf (buf, "($B$1Arena$N$7) $B%s kicks %s's knee; that's gotta hurt!", NAME (ch), NAME (victim));
              if (i == 3)
                sprintf (buf, "($B$1Arena$N$7) $B%s's kick catches %s in the side of the head!", NAME (ch), NAME (victim));
              do_arenaecho (ch, buf, TRUE);
            }
          if (can_yell (victim))
            {
              if (damm < 5)
                {
                  act ("\x1B[1;35mYour weak kick barely bruises $N's shin.\x1B[37;0m", ch, NULL, victim, TO_CHAR);
                  act ("\x1B[1;37m$n's weak kick barely bruises $N's shin.\x1B[0m", ch, NULL, victim, TO_NOTVICT);
                  act ("\x1B[1;36m$n's weak kick bruises your shin.\x1B[37;0m", ch, NULL, victim, TO_VICT);
                }
              else
                {
                  ii = number_range (1, 5);
                  if (ii == 1)
                    {
                      act ("\x1B[1;35mYour kick slams into $N's shin!\x1B[37;0m", ch, NULL, victim, TO_CHAR);
                      act ("\x1B[1;37m$n's kick slams into $N's shin.\x1B[0m", ch, NULL, victim, TO_NOTVICT);
                      act ("\x1B[1;36m$n's kick slams into your shin!\x1B[37;0m", ch, NULL, victim, TO_VICT);
                    }
                  else if (ii == 2)
                    {
                      act ("\x1B[1;35mYour kick smashes into $N's solar plexus!\x1B[37;0m", ch, NULL, victim, TO_CHAR);
                      act ("\x1B[1;37m$n kicks $N directly in the solar plexus!\x1B[0m", ch, NULL, victim, TO_NOTVICT);
                      act ("\x1B[1;36m$n kicks you in the solar plexus! Ouch!\x1B[37;0m", ch, NULL, victim, TO_VICT);
                    }
                  else if (ii == 3)
                    {
                      act ("\x1B[1;35mYour kick smashes into $N's knee!\x1B[37;0m", ch, NULL, victim, TO_CHAR);
                      act ("\x1B[1;37m$n kicks $N's knee very hard!\x1B[0m", ch, NULL, victim, TO_NOTVICT);
                      act ("\x1B[1;36m$n kicks your knee, and you nearly fall over...\x1B[37;0m", ch, NULL, victim, TO_VICT);
                    }
                  else if (ii == 4)
                    {
                      act ("\x1B[1;35mYou lift your leg high, and kick $N in the head!!\x1B[37;0m", ch, NULL, victim, TO_CHAR);
                      act ("\x1B[1;37m$n kicks $N's right in the face! Ouch...\x1B[0m", ch, NULL, victim, TO_NOTVICT);
                      act ("\x1B[1;36m$n kicks you in the face!\x1B[37;0m", ch, NULL, victim, TO_VICT);
                      act ("\x1B[1;31mBlood spurts from $N's nose!\x1B[37;0m", ch, NULL, victim, TO_CHAR);
                      act ("\x1B[1;31mBlood spurts from $N's nose!\x1B[37;0m", ch, NULL, victim, TO_NOTVICT);
                      act ("\x1B[1;31mBlood pours forth from your injured nose!\x1B[37;0m", ch, NULL, victim, TO_VICT);
                    }
                  else if (ii == 5)
                    {
                      act ("\x1B[1;35mYour kick connects with $N's ribs!\x1B[37;0m", ch, NULL, victim, TO_CHAR);
                      act ("\x1B[1;37m$n kicks $N in the ribs!\x1B[0m", ch, NULL, victim, TO_NOTVICT);
                      act ("\x1B[1;36m$n kicks you in the ribs!! Ouch!\x1B[37;0m", ch, NULL, victim, TO_VICT);
                    }
                }
            }
          else
            {
              act ("\x1B[1;35mWham! Your kick slams into $N!\x1B[37;0m", ch, NULL, victim, TO_CHAR);
              act ("\x1B[1;37m$n's kick slams into $N!\x1B[37;0m", ch, NULL, victim, TO_NOTVICT);
              act ("\x1B[1;37m$n's kick slams into you hard!\x1B[37;0m", ch, NULL, victim, TO_VICT);
            }

          show_hitdam (gsn_kick, "kick", damm, ch, victim);
          damage (ch, victim, damm/2, gsn_kick);
        }
      else
        {
          set_fighting (ch, victim);
          if (FIGHTING (victim) == NULL)
            victim->fgt->fighting = ch;
          act ("You miss a kick at $N.", ch, NULL, victim, TO_CHAR + 1000);
          act ("$n misses a kick at $N.", ch, NULL, victim, TO_NOTVICT + 1000);
          act ("$n tries to kick you but misses.", ch, NULL, victim, TO_VICT + 1000);
        }
    }
  return;
}


void
do_flail (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  int i;
  int x = 4;

  DEFINE_COMMAND ("flail", do_flail, POSITION_FIGHTING, 0,
LOG_NORMAL,"You flail around hitting people as much as possible. This can only be used when you are afraid of dying, it is a last-ditch attempt to kill your opponent.")
    if (FIGHTING(ch) == NULL)
      {
	send_to_char("You cannot flail unless you are fighting.\n\r", ch);
	return;
      }

  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't flail this soon after a bash.\n\r", ch);
      return;
    }


  if (ch->position != POSITION_FIGHTING)
    {
      send_to_char("You are not in the proper position to do this!\n\r",ch);
      WAIT_STATE(ch, 1*PULSE_VIOLENCE);
      SUBMOVE(ch, 20);
      return;
    }
  if (IS_PLAYER(ch) && ch->pcdata->learned[580] < 0)
    {
      send_to_char("You don't know how to flail!\n\r", ch);
      WAIT_STATE(ch, 1*PULSE_VIOLENCE);
      return;
    }



  if (ch->move < 0)
    {
      send_to_char("You are too weak to flail!\n\r", ch);
      return;
    }
  if (ch->hit > ch->max_hit/2)
    {
      send_to_char("You are not frantic enough to be able to use this skill!\n\r", ch);

      WAIT_STATE(ch, 2*PULSE_VIOLENCE);
      return;
    }
  if (get_curr_str(ch) < 26)
    {
      send_to_char("You are not strong enough to flail!\n\r", ch);
      return;
    }
  if (IS_PLAYER(ch))
    {
      int random = number_range(1,150);
      int learned = ch->pcdata->learned[580];

      if(random > learned)
	{
	  send_to_char("You try to flail around, but can't!\n\r", ch);
	  WAIT_STATE(ch, 3*PULSE_VIOLENCE);

	  return;
	}
    }
  send_to_char("You flail around in a mad frenzy!\n\r", ch);
  SUBMOVE(ch, 75);
  victim = FIGHTING(ch);

  for (i = 0; (i < x && victim->data_type != 50); i++)
  {
    multi_hit(ch, victim, TYPE_UNDEFINED);
    if (number_range(1,100) <= ((LEVEL(ch)/2)/(i+1)))
       x++;
  }
  skill_gain(ch, 580, TRUE);
  WAIT_STATE(ch, 5*PULSE_VIOLENCE);
  return;
}

void
do_impale (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
   SINGLE_OBJECT *wield;
  DEFINE_COMMAND ("impale", do_impale, POSITION_FIGHTING, 0, LOG_NORMAL,"This command lets you impale an opponent with a polearm.")
    check_fgt (ch);
  if (ch->wait > 4)
    return;

  WAIT_STATE (ch, PULSE_VIOLENCE);
  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't impale someone this soon after a bash.\n\r", ch);
      return;
    }
  if (FIGHTING (ch) == NULL)
    {
      if (argy[0] == '\0' || argy == "")
        {
          send_to_char ("Who?\n\r", ch);
          WAIT_STATE(ch, 1*PULSE_VIOLENCE);
          return;
        }
      if ((victim = get_char_room (ch, argy)) == NULL)
        {
          send_to_char ("Who?\n\r", ch);
          WAIT_STATE(ch, PULSE_VIOLENCE);
          return;
        }
    }
  else
    victim = ch->fgt->fighting;


  if (victim == ch)
    {
      send_to_char("No.\n\r", ch);
      WAIT_STATE(ch, 30*PULSE_VIOLENCE);
      return;
    }

    /* ----------------------------------------- */
    /*	    Protecting Neutral align.		 */
    /* ----------------------------------------- */

	if (IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN(victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
	{
		send_to_char ("A strange force prevents you from harming this person.\n\r", ch);
		return;
	}
	if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
	{
		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
		return;
	}

  /* ----------------------------------------- */
  /*	      Protecting LinkDead-NoTimers 	 */
  /* ----------------------------------------- */
  if(IS_PLAYER(victim) && victim->desc == NULL) {
    if(!(victim->pcdata->no_quit_pk > 0)) {
	send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
	return;
    }
  }


  if (victim->data_type==50) return;

  check_ced (ch);
  check_ced (victim);
  if (ch->position == POSITION_BASHED)
    {
      send_to_char ("You try to impale your opponent, but just can't seem to get up.\n\r", ch);
      WAIT_STATE(ch, 4*PULSE_VIOLENCE);
      return;
    }
  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      send_to_char ("You can't impale someone while you are groundfighting!\n\r", ch);
      WAIT_STATE(ch, PULSE_VIOLENCE);
      return;
    }
  if((wield = get_item_held(ch, ITEM_WEAPON)) == NULL)
    {
      send_to_char ("You must be wielding a polearm!\n\r", ch);
      WAIT_STATE(ch, 1*PULSE_VIOLENCE);
      return;
    }
  if ((wield == NULL) || !IS_OBJ_STAT(wield, ITEM_POLEARM))
    {
      send_to_char ("You must be wielding a polearm!\n\r", ch);
      WAIT_STATE(ch, 1* PULSE_VIOLENCE);
      return;
    }
  if (number_percent() >= ch->pcdata->learned[gsn_impale])
    {
      send_to_char("You attempt to impale your victim but fail!\n\r", ch);
      WAIT_STATE(ch, 8*PULSE_VIOLENCE);
      return;
     }
  check_same_side_pk(ch, victim);
  if (FIGHTING(ch) != victim);
  set_fighting (ch, victim);
  act("$B$4You lunge toward $N with $p.$R$7", ch, wield, victim,TO_CHAR);
  act("$B$4$n lunges toward you with $s $p.$R$7", ch, wield, victim,TO_VICT);
  act("$B$4$n lunges toward $N with $s $p.$R$7", ch, wield,victim,TO_NOTVICT);
  one_hit (ch, victim, gsn_impale, wield);

  if(LEVEL(ch) >= 80 && number_range(1,10) == 1 && ch->position == POSITION_FIGHTING) {
    act("$B$4You lunge toward $N with $p.$R$7", ch, wield, victim,TO_CHAR);
    act("$B$4$n lunges toward you with $s $p.$R$7", ch, wield, victim,TO_VICT);
    act("$B$4$n lunges toward $N with $s $p.$R$7", ch, wield,victim,TO_NOTVICT);
    one_hit (ch, victim, gsn_impale, wield);
  }

  if (IS_PLAYER (ch)) skill_gain (ch, gsn_impale, TRUE);
  WAIT_STATE(ch, 8* PULSE_VIOLENCE);
  if (number_range(1,10) == 1 && !IS_OBJ_STAT(wield, ITEM_POWER))

    {
      if (((I_WEAPON *)wield->more)->damage_p > 10)
	((I_WEAPON *)wield->more)->damage_p -= 5;
      else
	{
	  send_to_char ("Your polearm just snapped under the stress.n\r",ch);
	  unequip_char (ch, wield);
	  obj_from(wield);
	  free_it(wield);
	}
    }
  return;
}

void
do_disarm (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  SINGLE_OBJECT *obj;
  int percent;
  DEFINE_COMMAND ("disarm", do_disarm, POSITION_FIGHTING, 0, LOG_NORMAL, "This command allows you to disarm an opponent.  Success is based on the disarm skill.")

    if (CHALLENGE (ch) == 10)
    {
      send_to_char ("You cannot disarm in the arena.\n\r", ch);
      return;
    }
  if (ch->wait > 2)
    {
      send_to_char ("You are too off balance to disarm right now.\n\r", ch);
      return;
    }
  if (IS_PLAYER (ch) && ch->pcdata->tickcounts > 3)
    {
      send_to_char ("You are too off balance to disarm right now.\n\r", ch);
      return;
    }

  if (IN_BATTLE (ch) == TRUE)
    {
      send_to_char ("You cannot disarm while in the battleground.\n\r", ch);
      return;
    }
  if (get_item_held (ch, ITEM_WEAPON) == NULL)
    {
      send_to_char ("You must wield a weapon to disarm.\n\r", ch);
      return;
    }
  if ((victim = FIGHTING (ch)) == NULL)
    {
      send_to_char ("You aren't fighting anybody or anything.\n\r", ch);
      return;
    }
  if ((obj = get_item_held (victim, ITEM_WEAPON)) == NULL &&
       (obj = get_item_held (victim, ITEM_GEM)) == NULL)
    {
      send_to_char ("Your opponent is not wielding an item.\n\r", ch);
      return;
    }
  WAIT_STATE(ch, 3*PULSE_VIOLENCE);
  percent = number_percent () + LEVEL (victim) - (LEVEL (ch) * 2);
  if (IS_MOB (ch) || (IS_PLAYER (ch) && percent < ch->pcdata->learned[gsn_disarm] * 2 / 3))
    {
      disarm (ch, victim);
      skill_gain (ch, gsn_disarm, FALSE);
    }
  else
    send_to_char ("You tried to disarm your opponent, but failed.\n\r",ch);
  return;
}

void
do_sla (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("sla", do_sla, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This is so you don't try to slap someone and slay em instead!")

    send_to_char ("Slay?  Slam?  Sleep?  What do you mean?\n\r", ch);
  return;
}

void
do_slay (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  char arg[SML_LENGTH];
  DEFINE_COMMAND ("slay", do_slay, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command slays a mob or player.")

    if (IS_MOB (ch))
    return;
  if (!IS_REAL_GOD (ch))
    {
      send_to_char ("Command has been removed.\n\r", ch);
      return;
    }
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Who is that?\n\r", ch);
      return;
    }
  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("Who is that?\n\r", ch);
      return;
    }
  if (ch == victim)
    {
      send_to_char ("You aren't allowed to slay yourself.  Go see a psychiatrist.\n\r", ch);
      return;
    }
  if (IS_PLAYER (victim) && LEVEL (ch) < MAX_LEVEL)
    {
      send_to_char ("You shouldn't be slaying other players.\n\r", ch);
      return;
    }
  act ("You grab $S head and you squeeze hard... until $N's skull explodes into a bloody mess!",
       ch, NULL, victim, TO_CHAR);
  act ("$n grabs your head and squeezes hard... until your eyes pop out!  Everything goes dark...",
       ch, NULL, victim, TO_VICT);
  act ("$n grabs ahold of $N's head and squeezes hard.  Pretty soon $N's entire head and skull are just bloody pieces of flesh-and-brain-chunks on the ground.",
       ch, NULL, victim, TO_NOTVICT);
  raw_kill (victim, FALSE);
  return;
}

bool
hit_suck_disarm (CHAR_DATA * ch, CHAR_DATA * victim, int hit, int dam)
{
  SINGLE_OBJECT *obj;
  if (number_range (1, 100) >= LEVEL (ch)
      && (obj = get_item_held (victim, ITEM_WEAPON)) != NULL
      && hand_empty (ch) != WEAR_NONE)
    {
      unequip_char (victim, obj);
      obj->wear_loc = hand_empty (ch);
      obj_from (obj);
      obj_to (obj, ch);
      act ("Your weapon sucks $N's weapon right out of $S hand!", ch, NULL, victim, TO_CHAR);
      act ("$n's weapon sucks your weapon right out of your hand!", ch, NULL, victim, TO_VICT);
      act ("$n's magical weapon sucks $N's weapon right out of $S hand!", ch, NULL, victim, TO_NOTVICT);
      return TRUE;
    }
  return FALSE;
}


void
skill_gain (CHAR_DATA * ch, short gsn, bool report)
 {
  char buf[STD_LENGTH];
  bool learnyes;
  int vall[10];
  SPELL_DATA *spell;
  if (IS_MOB (ch))
    return;
  spell = skill_lookup (NULL, gsn);
  if (spell == NULL)
    return;
  if (bg_multiplier && bg_multiplier == 0)
  	if (ch->in_room && ch->in_room->vnum >= BATTLEGROUND_START_VNUM &&
		ch->in_room->vnum <= BATTLEGROUND_END_VNUM)
   		return;
  else
	if (ch->in_room && ch->in_room->vnum >= (BATTLEGROUND_START_VNUM + bg_multiplier) && ch->in_room->vnum <= (BATTLEGROUND_END_VNUM + bg_multiplier))
		return;

/* MATUSE - THIS SAYS NO IMPROVES WITH SPELL OF POWER */
  if (ch->pcdata->no_quit > 0)
    return;

  if (gsn == gsn_slashing || gsn == gsn_whip || gsn == gsn_concussion ||
gsn == gsn_pierce || gsn == gsn_polearm || gsn == gsn_second_attack || gsn
== gsn_third_attack || gsn == gsn_enhanced_damage || gsn ==
gsn_penetration || gsn == 571 || gsn == gsn_accuracy)
    {
      vall[0] = 202;
      vall[1] = 187;
      vall[2] = 165;
      vall[3] = 154;
      vall[4] = 165;
      vall[5] = 168;
      vall[6] = 173;
      vall[7] = 184;
      vall[8] = 192;
      vall[9] = 245;
    }
  else
    {

      if (spell->slot != 0)
        {
          vall[0] = 22;
          vall[1] = 18;
          vall[2] = 12;
          vall[4] = 10;
          vall[5] = 10;
          vall[6] = 10;
          vall[7] = 12;
          vall[8] = 13;
          vall[9] = 25;
        }
      else
        {
          vall[0] = 17;
          vall[1] = 12;
          vall[2] = 10;
          vall[3] = 7;
          vall[4] = 5;
          vall[5] = 5;
          vall[6] = 5;
          vall[7] = 6;
          vall[8] = 7;
          vall[9] = 9;
        }
    }
  /* This way whores cannot sit in safe areas all day improving skills. */
  if (ch->pcdata->no_quit_pk < 5 && number_range(1,15) != 10)
    return;
  learnyes = FALSE;
  if (ch->pcdata->learned[gsn] < 40)
    return;
  if (ch->pcdata->learned[gsn] < 50 && number_range (1, vall[0]) == 5)
    learnyes = TRUE;
  else if (ch->pcdata->learned[gsn] < 60 && number_range (1, vall[1]) ==5)
    learnyes = TRUE;
  else if (ch->pcdata->learned[gsn] < 70 && number_range (1, vall[2]) ==5)
    learnyes = TRUE;
  else if (ch->pcdata->learned[gsn] < 80 && number_range (1, vall[3]) ==5)
    learnyes = TRUE;
  else if (ch->pcdata->learned[gsn] < 90 && number_range (1, vall[4]) ==5)
    learnyes = TRUE;
  else if (ch->pcdata->learned[gsn] < 92 && number_range (1, vall[5]) ==5)
    learnyes = TRUE;
  else if (ch->pcdata->learned[gsn] < 94 && number_range (1, vall[6]) ==5)
    learnyes = TRUE;
  else if (ch->pcdata->learned[gsn] < 96 && number_range (1, vall[7]) ==5)
    learnyes = TRUE;
  else if (ch->pcdata->learned[gsn] < 98 && number_range (1, vall[8]) ==5)
    learnyes = TRUE;
  else if (number_range (1, vall[9]) == 5)
    learnyes = TRUE;
  if (learnyes && ch->pcdata->learned[gsn] < 100)
    {
      {
        SPELL_DATA *spl;
        spl = skill_lookup (NULL, gsn);
        if (spl == NULL)
          sprintf (buf, "ERROR!! PLEASE REPORT TO ORIN!\n\r");
        else
          sprintf (buf, "\x1B[1;37m**** [You have become better at %s!] ****\x1B[0m\n\r",
                   spl->spell_funky_name);
        send_to_char (buf, ch);
      }
      ch->pcdata->learned[gsn]++;
    }
  return;
 }

void
disengage (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("disengage", disengage, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to disengage from battle if someone is not directly hitting you.")

    if (FIGHTING (ch) == NULL)
    {
      send_to_char ("You aren't fighting anything or anyone!\n\r", ch);
      return;
    }
  if (ch->fgt && ch->fgt->fighting && ch->fgt->fighting->fgt &&
      ch->fgt->fighting->fgt->fighting == ch)
    {
      send_to_char ("Your opponent is attacking YOU! You can't disengage!\n\r", ch);
      return;
    }
  if (ch->fgt)
    {
      ch->fgt->fighting = NULL;
    }
  ch->position = POSITION_STANDING;
  WAIT_STATE (ch, 4 * PULSE_VIOLENCE);
  return;
}



void
do_load (CHAR_DATA *ch, char *argy)
{
  SINGLE_OBJECT *bow;
  SINGLE_OBJECT *arrow;
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND ("load", do_load, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to load a missile weapon.")

    if (IS_MOB(ch)) return;

  if (FIGHTING(ch) != NULL)
    {
      send_to_char("You cannot load a weapon while engaged in mortal combat!\n\r", ch);
      return;
    }
  if (ch->position != POSITION_STANDING)
    {
      send_to_char("You cannot load a weapon unless you are standing.\n\r", ch);
      return;
    }
  if ((bow = get_item_held(ch, ITEM_WEAPON)) == NULL || !IS_OBJ_STAT(bow, ITEM_RANGED) || ((I_WEAPON *)bow->more)->ammo_type <= 0)
    {
      send_to_char("You must be holding a ranged weapon to load it.\n\r", ch);
      return;
    }
  if (bow->contains != NULL)
    {
      send_to_char("Your bow is already loaded.\n\r", ch);
      return;
    }
  arrow = NULL;
  for(obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
      if (obj->pIndexData->item_type == ITEM_AMMO && (((I_AMMO *)obj->more)->ammo_type == ((I_WEAPON *)bow->more)->ammo_type))
	{
	  {
	    arrow = obj;
	    break;
	  }
	}
    }
  if (arrow == NULL)
    {
      send_to_char("You don't have any missiles of the proper type to load into your weapon.\n\r", ch);
      return;
    }
  obj_from(arrow);
  obj_to(arrow, bow);
  send_to_char("You load your weapon.\n\r", ch);
  WAIT_STATE(ch, 8*((I_WEAPON *)bow->more)->ammo_type);
  return;
}

void
do_unload (CHAR_DATA *ch, char *argy)
{
  SINGLE_OBJECT *bow;
  SINGLE_OBJECT *arrow;
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND ("unload", do_unload, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to unload a missile weapon.")

    if (IS_MOB(ch)) return;

  if (FIGHTING(ch) != NULL)
    {
      send_to_char("You cannot unload a weapon while engaged in mortal combat!\n\r", ch);
      return;
    }
  if (ch->position != POSITION_STANDING)
    {
      send_to_char("You cannot unload a weapon unless you are standing.\n\r", ch);
      return;
    }
  if ((bow = get_item_held(ch, ITEM_WEAPON)) == NULL || !IS_OBJ_STAT(bow, ITEM_RANGED) || ((I_WEAPON *)bow->more)->ammo_type <= 0)
    {
      send_to_char("You must be holding a ranged weapon to unload it.\n\r", ch);
      return;
    }
  if ((arrow = bow->contains) == NULL)
    {
      send_to_char("Your bow is not loaded.\n\r", ch);
      return;
    }
  else
    {
      obj_from(arrow);
      obj_to(arrow, ch);
      send_to_char("You unload your weapon.\n\r", ch);
      return;
    }
  return;
}



void
do_shoot (CHAR_DATA *ch, char *argy)
{
  SINGLE_OBJECT *bow;
  SINGLE_OBJECT *arrow;
  CHAR_DATA *victim;
  int range = 0;

  DEFINE_COMMAND ("shoot", do_shoot, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to shoot a loadded missile weapon.")

    if (IS_MOB(ch)) return;

/*  victim = get_char_room_near (ch, ch->in_room, range, argy); */

  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't shoot this soon after a bash.\n\r", ch);
      return;
    }


  if (FIGHTING(ch) != NULL)
    {
      send_to_char("You cannot shoot a weapon while engaged in mortal combat!\n\r", ch);
      return;
    }
  if (ch->position != POSITION_STANDING)
    {
      send_to_char("You cannot shoot a weapon unless you are standing.\n\r", ch);
      return;
    }
  if ((bow = get_item_held(ch, ITEM_WEAPON)) == NULL || !IS_OBJ_STAT(bow, ITEM_RANGED))
    {
      send_to_char("You must be holding a ranged weapon to shoot it.\n\r", ch);
      return;
    }
  if ((arrow = bow->contains) == NULL)
    {
      send_to_char("Your missile weapon is not loaded.\n\r", ch);
      return;
    }
  if (arrow->pIndexData->item_type != ITEM_AMMO)
    {
      send_to_char("Your bow was loaded with something other than ammunition.\n\r", ch);
      obj_from(arrow);
      free_it(arrow);
      return;
    }
  range = ((I_WEAPON *)bow->more)->ammo_type;
  if (range == 3) range++;
  if (range <= 0)
    {
      send_to_char("This is not a ranged weapon.\n\r", ch);
      return;
    }
  if ((victim = get_char_room_near (ch, ch->in_room, range, argy)) == NULL)
    {
      send_to_char("You don't see that person here to shoot.\n\r", ch);
      return;

  }
    
   /* ----------------------------------------- */
   /*	    Protecting Neutral align.		 */
   /* ----------------------------------------- */

   if (IS_PLAYER(victim) && ALIGN (victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
   {
   	   send_to_char("A strange force prevents you from harming this person.\n\r", ch);
   	   return;
   }
   if (IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
   {
   		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
   		return;
   }


  /* ----------------------------------------- */
  /*	      Protecting LinkDead-NoTimers 	 */
  /* ----------------------------------------- */
  if(IS_PLAYER(victim) && victim->desc == NULL) {
    if(!(victim->pcdata->no_quit_pk > 0)) {
	send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
	return;
    }
  }


/*
  if (!DIFF_ALIGN(ch, victim) && IS_PLAYER(victim) && ALIGN(ch) == 2 &&
!IS_MOB(ch) && !IN_BATTLE(ch) && !IN_BATTLE(victim))
    {
      send_to_char("You can not attack members of the same alignment!\n\r", ch);
      return;
    }
*/

  check_temp(ch);
  strcpy(ch->pcdata->temp->temp_string, argy);
  ch->pcdata->tickcounts = 10+(((I_WEAPON *)bow->more)->ammo_type)*4;
  NEW_POSITION(ch, POSITION_SHOOTING);
  return;
}

void
do_actual_shoot (CHAR_DATA *ch, char *argy)
{
  SINGLE_OBJECT *bow;
  SINGLE_OBJECT *arrow;
  CHAR_DATA *victim;
  int dam = 0;
  int range = 0;
  int to_hit;
  int mult;



  if (IS_MOB(ch)) return;

  if (FIGHTING(ch) != NULL)
    {
      send_to_char("You cannot shoot a weapon while engaged in mortal combat!\n\r", ch);
      return;
    }
  if (ch->position != POSITION_SHOOTING)
    {
      send_to_char("You cannot shoot a weapon unless you are in a shooting position.\n\r", ch);
      return;
    }
  if ((bow = get_item_held(ch, ITEM_WEAPON)) == NULL || !IS_OBJ_STAT(bow, ITEM_RANGED))
    {

      send_to_char("You must be holding a ranged weapon to shoot it.\n\r", ch);
      return;
    }
  if ((arrow = bow->contains) == NULL)
    {
      send_to_char("Your missile weapon is not loaded.\n\r", ch);
      return;
    }
  if (arrow->pIndexData->item_type != ITEM_AMMO)
    {
      send_to_char("Your bow was loaded with something other than ammunition.\n\r", ch);
      obj_from(arrow);
      free_it(arrow);
      return;
    }
  range = ((I_WEAPON *)bow->more)->ammo_type;
  if (range == 3) range++;
  if (range <= 0)
    {
      send_to_char("This is not a ranged weapon.\n\r", ch);
      return;
    }
  if ((victim = get_char_room_near (ch, ch->in_room, range, argy)) == NULL)
    {
      send_to_char("You don't see that person here to shoot.\n\r", ch);
      WAIT_STATE(ch, PULSE_VIOLENCE);
      return;
    }
  
   /* ----------------------------------------- */
   /*	    Protecting Neutral align.		 */
   /* ----------------------------------------- */

   if (IS_PLAYER(victim) && ALIGN (victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
   {
   	   send_to_char("A strange force prevents you from harming this person.\n\r", ch);
   	   return;
   }
   if (IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)
   {
   		send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
   		return;
   }

  /* ----------------------------------------- */
  /*	      Protecting LinkDead-NoTimers 	 */
  /* ----------------------------------------- */
  if(IS_PLAYER(victim) && victim->desc == NULL) {
    if(!(victim->pcdata->no_quit_pk > 0)) {
	send_to_char ("You cannot attack someone who exists slightly outside reality.\n\r", ch);
	return;
    }
  }

  to_hit = GET_HITROLL(ch);
  if (number_range(1,10) == 3)
    skill_gain(ch, gsn_shoot, TRUE);
  check_same_side_pk(ch, victim);
  if (number_range(1,110) >  ch->pcdata->learned[gsn_shoot])
    {
      send_to_char("You let loose with your arrow, but miss your intended victim!\n\r", ch);
      act ("$n lets loose an arrow, but it misses.", ch, NULL, NULL, TO_ROOM);
      act("An arrow whizzes by your head.", victim, NULL, NULL, TO_CHAR);
      act("An arrow whizzes by $n's head.", victim, NULL, NULL, TO_ROOM);

      obj_from(arrow);
      free_it(arrow);
      WAIT_STATE(ch, PULSE_VIOLENCE);
      return;
    }
  mult = ((I_WEAPON *)bow->more)->ammo_type;
  if (mult == 2)
    mult++;
  else if (mult == 3)
    mult-=2;
  mult += number_range(0,2);
  dam = (dice (((I_AMMO *)arrow->more)->firstdice, ((I_AMMO *)arrow->more)->seconddice));
  if (dam < 3)  dam = 3;
  dam += UMIN(bow->pIndexData->made_of, arrow->pIndexData->made_of);
  dam = dam*mult;
  if (IS_MOB(victim) && IS_SET(victim->act, ACT_SENTINEL))
    dam = 0;
  act("You let loose with your arrow and strike $N", ch, NULL, victim, TO_CHAR);
  act("$n lets loose with $s arrow and strikes $N", ch, NULL, victim, TO_ROOM);

  show_hitdam(gsn_shoot, "shot", dam, ch, victim);
  damage (ch, victim, dam, gsn_shoot);
  obj_from(arrow);
  free_it(arrow);
  NEW_POSITION(ch, POSITION_STANDING);
  return;
}

void
do_throw (CHAR_DATA *ch, char *argy)
{
  SINGLE_OBJECT *thrown;
  int range;
  int dam;
  int mult;
  CHAR_DATA *victim;
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  int to_hit;

  DEFINE_COMMAND ("throw", do_throw, POSITION_FIGHTING, 0, LOG_NORMAL, "Allows you to throw a held  weapon (axe or piercer).")

  argy = one_argy(argy, arg1);
  argy = one_argy(argy, arg2);
  if (ch->fgt->ears > 9)
    {
      send_to_char ("You can't throw this soon after a bash.\n\r", ch);
      return;
    }


  if ((thrown = get_item_held(ch, ITEM_WEAPON)) == NULL || (!IS_OBJ_STAT(thrown, ITEM_PIERCE) && !is_name("axe", thrown->pIndexData->name)))
    {
      send_to_char("You must be holding a piercing weapon or an axe to throw it.\n\r", ch);
      return;
    }
  if (!is_name(arg1, thrown->pIndexData->name))
    {
      send_to_char("You must be holding that item to throw it.\n\r", ch);
      return;
    }


  range = 0;
  if (IS_OBJ_STAT(thrown, ITEM_PIERCE))
    range = 2;
  else
    if (is_name("axe", thrown->pIndexData->name))
      range = 1;
    else
      range = 0;
  if (range <= 0)
    {
      send_to_char("This weapon cannot be thrown.\n\r", ch);
      return;
    }
  if ((victim = get_char_room_near (ch, ch->in_room, range, arg2)) == NULL)
    {
      send_to_char("You don't see that person to throw at.\n\r", ch);
      WAIT_STATE(ch, PULSE_VIOLENCE);
      return;
    }

  /* ARC:
     Protect Neutral align
   */
  if (IS_PLAYER(victim) && IS_PLAYER(ch) && ALIGN (victim) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10)   {
	  send_to_char("A strange force prevents you from throwing.\n\r", ch);
 	  return;
   }
  if ( IS_PLAYER(ch) && IS_PLAYER(victim) && ALIGN (ch) == 0 && !IN_BATTLE(ch)
		&& ch->fgt->challenge != 10) {
	send_to_char ("A strange force prevents you from attacking other players.\n\r", ch);
	return;
  }


  to_hit = GET_HITROLL(ch);
  if (number_range(1,10) == 3)
    skill_gain(ch, gsn_throw, TRUE);
  check_same_side_pk(ch, victim);
  if (number_range(1,110) > ch->pcdata->learned[gsn_throw])
    {
      act("You heave your $p toward $N, but miss.", ch, thrown, victim, TO_CHAR);
       act ("$n throws the $p, but it misses.", ch, thrown, NULL, TO_ROOM);
      act("A $p flies by your head.", victim, thrown, NULL, TO_CHAR);
      act("A $p flies by $n's head.", victim, thrown, NULL, TO_ROOM);
      obj_from(thrown);
      if (number_range(1,8) == 4)
	free_it(thrown);
      else
	obj_to(thrown, victim->in_room);
      WAIT_STATE(ch, PULSE_VIOLENCE);
      return;
    }
  mult = (3-range)+number_range(0,2);
  dam = (dice (((I_WEAPON *) thrown->more)->firstdice, ((I_WEAPON*) thrown->more)->seconddice));
  if (dam < 2)  dam = 2;
  dam += 2*thrown->pIndexData->made_of;
  dam *= mult;
  if (IS_MOB(victim) && IS_SET(victim->act, ACT_SENTINEL))
    dam = 0;
  act("You throw your $p and strike $N", ch, thrown, victim, TO_CHAR);
  act("$n throws $s $p and strikes $N", ch, thrown, victim, TO_ROOM);

  show_hitdam(gsn_throw, "hit", dam, ch, victim);
  damage (ch, victim, dam, gsn_throw);
  obj_from(thrown);
  if (number_range(1,8) == 4)
    free_it(thrown);
  else
    obj_to (thrown, victim);
  WAIT_STATE(ch, 30);
  return;
}
