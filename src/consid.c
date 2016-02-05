#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

void 
do_consider (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  SINGLE_OBJECT *weapon;
  I_WEAPON *wep;
  int b4, aftr, dif;
  int ch_dam, victim_dam;
  int ch_attacks, victim_attacks;
  int ch_absorb, victim_absorb;
  int rounds_to_kill_vict;
  int rounds_to_kill_ch;
  int round_difference;
  char *msg = '\0';
  DEFINE_COMMAND ("consider", do_consider, POSITION_RESTING, 0, LOG_NORMAL, "This command runs through the battle in your character's mind, and tells you the suspected outcome.")

    
    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Consider killing whom?\n\r", ch);
      return;
    }
  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("They're not here.\n\r", ch);
      return;
    }
  if (victim == ch)
    {
      send_to_char("Suicide is not an option.\n\r", ch);
      return;
    }
  if ((weapon = get_item_held(ch, ITEM_WEAPON)) != NULL)
    {
      wep = (I_WEAPON *)weapon->more;
      ch_dam = wep->firstdice * (wep->seconddice + 1)/2 + 1;
    }
  else
    {
      ch_dam = LEVEL(ch)/20 +2;
    }
  if (IS_MOB(ch))
    ch_dam += ch->pIndexData->damroll;
  ch_dam += GET_DAMROLL(ch)/4;
  if (ch_dam < 2) ch_dam = 2;
  b4 = ch_dam;
  aftr = new_damage(victim, ch_dam, 1, 1003);
  dif = b4 - aftr;
  ch_dam -= dif/2;
  if (ch_dam < 2) ch_dam = 2;
  if (IS_PLAYER(ch))
    ch_dam *=2;
  ch_absorb = 0;
  if (IS_AUGMENTED(ch, AUG_LEG_PLATING))
    ch_absorb += 4;
  if (IS_AUGMENTED(ch, AUG_FOOT_PLATING))
    ch_absorb += 3;
  if (IS_AUGMENTED(ch, AUG_ARM_PLATING))
    ch_absorb += 3;
  if (IS_AUGMENTED(ch,AUG_HEAD_PLATING))
    ch_absorb += 3;
  if (IS_AUGMENTED(ch, AUG_BODY_PLATING))
    ch_absorb += 5;
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_resilience] > 7)
    ch_absorb +=(ch->pcdata->learned[gsn_resilience]/4);
  if (DIFF_ALIGN(ch, ch) && IS_AFFECTED(ch, AFF_PROT_ALIGN))
    ch_absorb += 14;
  if (IS_AFFECTED (ch, AFF_SANCTUARY))
    ch_absorb += 25;
  if (IS_AFFECTED (ch, AFF_PROTECT))
    ch_absorb += 40;
  if (ch_absorb > 80)
    ch_absorb = 80;
  ch_attacks = 10;
  if (IS_AFFECTED(ch, AFF_SPEED)) ch_attacks += 5;
  if (IS_PLAYER(ch))
    {
      if (ch->pcdata->learned[571] > 0)
	ch_attacks += ch->pcdata->learned[571]*3/50 + 1;
      if (ch->pcdata->learned[gsn_second_attack] > 0)
	ch_attacks += ch->pcdata->learned[gsn_second_attack]*3/50 + 1;
      if (ch->pcdata->learned[gsn_third_attack] > 0)
	ch_attacks += ch->pcdata->learned[gsn_third_attack]/20 + 1;
    }
  else
    {
      ch_attacks += UMIN(40, LEVEL(ch)/4);
    }



  if ((weapon = get_item_held(victim, ITEM_WEAPON)) != NULL)
    {
      wep = (I_WEAPON *)weapon->more;
      victim_dam = wep->firstdice * (wep->seconddice + 1)/2 + 1;
    }
  else
    {
	  victim_dam = LEVEL(victim)/20 +2;
    }
  if (IS_MOB(victim))
    victim_dam += victim->pIndexData->damroll; 
  victim_dam += GET_DAMROLL(victim)/4;
  if (victim_dam < 2) victim_dam = 2;
  b4 = victim_dam;
  aftr = new_damage(victim, victim_dam, 1, 1003);
  dif = b4 - aftr;
  victim_dam -= dif/2;
  if (victim_dam < 2) victim_dam = 2;
  if (IS_PLAYER(victim))
    victim_dam = victim_dam*5/4;
  victim_absorb = 0;
  if (IS_AUGMENTED(victim, AUG_LEG_PLATING))
    victim_absorb += 4;
  if (IS_AUGMENTED(victim, AUG_FOOT_PLATING))
    victim_absorb += 3;
  if (IS_AUGMENTED(victim, AUG_ARM_PLATING))
    victim_absorb += 3;
  if (IS_AUGMENTED(victim,AUG_HEAD_PLATING))
    victim_absorb += 3;
  if (IS_AUGMENTED(victim, AUG_BODY_PLATING))
    victim_absorb += 5;
  if (IS_PLAYER(victim) && victim->pcdata->learned[gsn_resilience] > 7)
      victim_absorb +=(victim->pcdata->learned[gsn_resilience]/4);
  if (DIFF_ALIGN(victim, victim) && IS_AFFECTED(victim, AFF_PROT_ALIGN))
    victim_absorb += 15;
  if (IS_AFFECTED (victim, AFF_SANCTUARY))
    victim_absorb += 25;
  if (IS_AFFECTED (victim, AFF_PROTECT))
    victim_absorb += 40;
  if (victim_absorb > 80)
    victim_absorb = 80;
  victim_attacks = 10;
  if (IS_AFFECTED(victim, AFF_SPEED)) victim_attacks += 5;
  if (IS_PLAYER(victim))
    {
      if (victim->pcdata->learned[571] > 0)
	victim_attacks += victim->pcdata->learned[571]*3/50 + 1;
      if (victim->pcdata->learned[gsn_second_attack] > 0)
	victim_attacks += victim->pcdata->learned[gsn_second_attack]*3/50 + 1;
      if (victim->pcdata->learned[gsn_third_attack] > 0)
	victim_attacks += victim->pcdata->learned[gsn_third_attack]/20 + 1;
    }
  else
    {
      victim_attacks += UMIN(30, LEVEL(victim)/4);
    }

  ch_dam = ((100-victim_absorb)*ch_dam)/100;
  ch_dam *= ch_attacks;
  if (ch_dam <=0) ch_dam = 1;
  rounds_to_kill_vict = victim->hit * 10/ch_dam;

  victim_dam = ((100-ch_absorb)*victim_dam)/100;
  victim_dam *= victim_attacks;
  if (victim_dam <= 0) victim_dam = 1;
  rounds_to_kill_ch = ch->hit * 10/victim_dam;
  
  rounds_to_kill_vict += number_range(0,2);
  rounds_to_kill_ch += number_range(0,2);
  round_difference = rounds_to_kill_vict - rounds_to_kill_ch;
  send_to_char("You carefully go over the battle in your mind...\n\r", ch);
  round_difference/=3;
  if (round_difference <= - 10)
    msg = "You will have fun rearranging $N's bodyparts!";
  else
    {
      switch(round_difference)
	{
	case -9:
	  msg = "Why would you bore yourself killing $N...";
	  break;
	case -8:
	  msg = "$N stands no chance against you.";
	  break;
	case -7:
	  msg = "No sweat... a little work and $N will go down hard...";
	  break;
	case -6:
	  msg = "This should be fairly easy...";
	  break;
	case -5:
	  msg = "You should be able to beat $N unless $E has some tricks up $S sleeve.";  
	  break;
	case -4:
	  msg = "You would probably win unless $N has some tricks up $S sleeve.";
	  break;
	case -3:
	  msg = "It would be a close battle but you would probably win.";
	  break;
	case -2:
	  msg = "It would be a close battle without offensive magic.";
	  break;
	case -1:
	  msg = "You are almost exactly evenly matched (without offensive magic)";
	  break;
	case 0:
	  msg = "You are almost exactly evenly matched.";
	  break;
	case 1:
	  msg = "It would be a close battle but you would probably lose.";
	  break;
	case 2:
	  msg = "You would need a little luck.";
	  break;
	case 3:
	  msg = "You would need quite a bit of luck.";
	  break;
	case 4:
	  msg = "$N will probably beat you in a one on one battle.";
	  break;
	case 5:
	  msg = "You'd probably need a friend or two...";
	  break;
	case 6:
	  msg = "You better have brought some friends!";
	  break;
	case 7:
	  msg = "You better have brought A LOT of friends!";
	  break;
	case 8:
	  msg = "I hope you have an army to back you up!";
	  break;
	case 9:
	  msg = "$N will laugh as $E rips you in two.";
	  break;
	case 10:
	  msg = "$N will squish your face into the ground with ease.";
	  break;
	default:
	case 11:
	  msg = "You truly are a masochist! There is NO WAY!";
	  break;
	}
    }
  act (msg, ch, NULL, victim, TO_CHAR);
  return;
}


void 
do_group_consider (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *attacker;
  char *msg = '\0';
  SINGLE_OBJECT *weapon;
  I_WEAPON *wep;
  int b4, aftr, dif;
  int attacker_dam, victim_dam;
  int attacker_attacks, victim_attacks;
  int attacker_absorb, victim_absorb;
  int rounds_to_kill_vict;
  int rounds_to_kill_ch;
  int round_difference;
  int group_dam = 0;

  DEFINE_COMMAND ("gconsider", do_group_consider, POSITION_RESTING, 0, LOG_NORMAL, "This command runs through the battle in your character's mind as if you were the tank for your group, and tells you the suspected outcome.")
    one_argy (argy, arg);
  if (chars_in_group (ch) < 2)
    {
      send_to_char ("But you aren't in a group!!\n\r", ch);
      return;
    }
  if (arg[0] == '\0')
    {
      send_to_char ("Group-Consider killing whom?\n\r", ch);
      return;
    }
  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("They're not here.\n\r", ch);
      return;
    }
  if (victim == ch)
    {
      send_to_char("Suicide is not an option.\n\r", ch);
      return;
    }
  if ((weapon = get_item_held(victim, ITEM_WEAPON)) != NULL)
    {
      wep = (I_WEAPON *)weapon->more;
      victim_dam = wep->firstdice * (wep->seconddice + 1)/2 + 1;
    }
  else
    {
      victim_dam = LEVEL(victim)/20 +2;
    }
  if (IS_MOB(victim))
    victim_dam += victim->pIndexData->damroll; 
  victim_dam += GET_DAMROLL(victim)/4;
  if (victim_dam < 2) victim_dam = 2;
  b4 = victim_dam;
  aftr = new_damage(victim, victim_dam, 1, 1003);
  dif = b4 - aftr;
  victim_dam -= dif/2;
  if (victim_dam < 2) victim_dam = 2;
  if (IS_PLAYER(victim))
    victim_dam = victim_dam*5/4;
  victim_absorb = 0;
  if (IS_AUGMENTED(victim, AUG_LEG_PLATING))
    victim_absorb += 4;
  if (IS_AUGMENTED(victim, AUG_FOOT_PLATING))
    victim_absorb += 3;
  if (IS_AUGMENTED(victim, AUG_ARM_PLATING))
    victim_absorb += 3;
  if (IS_AUGMENTED(victim,AUG_HEAD_PLATING))
    victim_absorb += 3;
  if (IS_AUGMENTED(victim, AUG_BODY_PLATING))
    victim_absorb += 5;
  if (IS_PLAYER(victim) && victim->pcdata->learned[gsn_resilience] > 7)
      victim_absorb +=(victim->pcdata->learned[gsn_resilience]/4);
  if (DIFF_ALIGN(victim, victim) && IS_AFFECTED(victim, AFF_PROT_ALIGN))
    victim_absorb += 15;
  if (IS_AFFECTED (victim, AFF_SANCTUARY))
    victim_absorb += 20;
  if (IS_AFFECTED (victim, AFF_PROTECT))
    victim_absorb += 40;
  if (victim_absorb > 80)
    victim_absorb = 80;
  victim_attacks = 10;
  if (IS_AFFECTED(victim, AFF_SPEED)) victim_attacks += 5;
  if (IS_PLAYER(victim))
    {
      if (victim->pcdata->learned[571] > 0)
	victim_attacks += victim->pcdata->learned[571]*3/50 + 1;
      if (victim->pcdata->learned[gsn_second_attack] > 0)
	victim_attacks += victim->pcdata->learned[gsn_second_attack]*3/50 + 1;
      if (victim->pcdata->learned[gsn_third_attack] > 0)
	victim_attacks += victim->pcdata->learned[gsn_third_attack]/20 + 1;
    }
  else
    {
      victim_attacks += UMIN(30, LEVEL(victim)/4);
    }
  for (attacker = ch->in_room->more->people; attacker !=NULL; attacker = attacker->next_in_room)
    {
      if (is_same_group (attacker, ch))
	{
	  if ((weapon = get_item_held(attacker, ITEM_WEAPON)) != NULL)
	    {
	      wep = (I_WEAPON *)weapon->more;
	      attacker_dam = wep->firstdice * (wep->seconddice + 1)/2 + 1;
	    }
	  else
	    {
	      attacker_dam = LEVEL(attacker)/20 +2;
	    }
	  if (IS_MOB(attacker))
	    attacker_dam += attacker->pIndexData->damroll;
	  attacker_dam += GET_DAMROLL(attacker)/4;
	  if (attacker_dam < 2) attacker_dam = 2;
	  b4 = attacker_dam;
	  aftr = new_damage(victim, attacker_dam, 1, 1003);
	  dif = b4 - aftr;
	  attacker_dam -= dif/2;
	  if (attacker_dam < 2) attacker_dam = 2;
	  if (IS_PLAYER(attacker))
	    attacker_dam *=2;
	  if (attacker == ch)
	    {
	      attacker_absorb = 0;
	      if (IS_AUGMENTED(attacker, AUG_LEG_PLATING))
		attacker_absorb += 4;
	      if (IS_AUGMENTED(attacker, AUG_FOOT_PLATING))
		attacker_absorb += 3;
	      if (IS_AUGMENTED(attacker, AUG_ARM_PLATING))
		attacker_absorb += 3;
	      if (IS_AUGMENTED(attacker,AUG_HEAD_PLATING))
		attacker_absorb += 3;
	      if (IS_AUGMENTED(attacker, AUG_BODY_PLATING))
		attacker_absorb += 5;
	      if (IS_PLAYER(attacker) && attacker->pcdata->learned[gsn_resilience] > 7)
		attacker_absorb +=(attacker->pcdata->learned[gsn_resilience]/4);
	      if (DIFF_ALIGN(attacker, attacker) && IS_AFFECTED(attacker, AFF_PROT_ALIGN))
		attacker_absorb += 15;
	      if (IS_AFFECTED (attacker, AFF_SANCTUARY))
		attacker_absorb += 20;
	      if (IS_AFFECTED (attacker, AFF_PROTECT))
		attacker_absorb += 40;
	      if (attacker_absorb > 80)
		attacker_absorb = 80;
	    }
	  attacker_attacks = 10;
	  if (IS_AFFECTED(attacker, AFF_SPEED)) attacker_attacks += 5;
	  if (IS_PLAYER(attacker))
	    {
	      if (attacker->pcdata->learned[571] > 0)
		attacker_attacks += attacker->pcdata->learned[571]*3/50 + 1;
	      if (attacker->pcdata->learned[gsn_second_attack] > 0)
		attacker_attacks += attacker->pcdata->learned[gsn_second_attack]*3/50 + 1;
	      if (attacker->pcdata->learned[gsn_third_attack] > 0)
		attacker_attacks += attacker->pcdata->learned[gsn_third_attack]/20 + 1;
	    }
	  else
	    {
	      attacker_attacks += UMIN(40, LEVEL(attacker)/4);
	    }
	  group_dam += attacker_attacks * attacker_dam;
	  
	}
      
    }
  group_dam = ((100-victim_absorb)*group_dam)/100;
  if (group_dam <= 0) group_dam = 1;
  rounds_to_kill_vict = victim->hit * 10/group_dam + number_range(0,2);

  victim_dam = ((100-attacker_absorb)*victim_dam)/100;
  victim_dam *= victim_attacks;
  if (victim_dam <= 0) victim_dam = 1;
  rounds_to_kill_ch = ch->hit * 10/victim_dam + number_range(0,2);

  round_difference = rounds_to_kill_vict - rounds_to_kill_ch;
  round_difference /= 3;

  send_to_char("You carefully go over the battle in your mind...\n\r", ch);

  if (round_difference <= - 10)
    msg = "Your group will have fun ripping off $N's bodyparts!";
  else
    {
      switch(round_difference)
	{
	case -9:
	 msg = "Why would your group bore itself with $N...";
	  break;
	case -8:
	  msg = "$N stands no chance against your group.";
	  break;
	case -7:
	 msg = "A little teamwork and $N will go down hard...";
	  break;
	case -6:
	  msg = "This should be fairly easy...";
	  break;
	case -5:
	   msg = "Your group should be able to beat $N.";
	  break;
	case -4:
	  msg = "Your group would probably win but it might be close.";
	  break;
	case -3:
	  msg = "It would be a close battle but your group would probably win.";
	  break;
	case -2:
	  msg = "It would be a close battle without offensive magic.";
	  break;
	case -1:
	  msg = "Your group and the mob are almost exactly evenly matched."; 
	  break;
	case 0:
	 msg = "The mob might take you out before your group wins.";
	  break;
	case 1:
	  msg = "You will probably die, but your group should win.";
	  break;
	case 2:
	 msg = "You would need a little luck tanking this foe.";
	  break;
	case 3:
	  msg = "Your group probably will win, but you will might die in the process.";
	  break;
	case 4:
	  msg = "Your group may win, but you will probably die in the battle.";
	  break;
	case 5:
	  msg = "You will probably die, but your group still stands a small chance.";
	  break;
	case 6:
	 msg = "You will probably have to flee, as will others in your group.";
	  break;
	case 7:
	   msg = "Your group probably will have to flee unless you get lucky.";
	  break;
	case 8:
	   msg = "$N will probably triumph over your group!";
	  break;
	case 9:
	  msg = "$N will laugh as $E rips all of you apart.";
	  break;
	case 10:
	 msg = "$N will crush your group with ease.";
	  break;
	default:
	case 11:
	  msg = "$N could destroy your group with ease!";
	  break;
	}
    }
  act (msg, ch, NULL, victim, TO_CHAR); 
  return;
}
