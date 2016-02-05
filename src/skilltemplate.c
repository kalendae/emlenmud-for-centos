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


  if (!DIFF_ALIGN(ch, victim) && ALIGN(victim) <= 3 && IS_PLAYER(victim) &&
!IS_MOB(ch) && !IN_BATTLE(ch) && !IN_BATTLE(victim))
    {
      send_to_char ("No more Same side pk to prevent same align-pkdeath. You can not attack members of the same alignment!\n\r", ch);
      return;
    }

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
	  if (IS_AUGMENTED(ch, AUG_LEG_STR)) damm += number_range(1,4);
	  if (IS_AUGMENTED(ch, AUG_FOOT_STR)) damm += number_range(3,6);
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
