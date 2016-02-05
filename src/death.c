#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

bool big_group_pkill_whore = FALSE;

void 
do_investigate (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *obj;

  DEFINE_COMMAND ("investigate", do_investigate, POSITION_STANDING, 0, LOG_NORMAL, "This command attempts to investigate reasons of death and looters of a corpse.  Based on the investigation skill.")
    check_room_more (ch->in_room);
  if (ch->pcdata->learned[gsn_invest] < 1)
    {
      send_to_char ("You don't know how to investigate!\n\r", ch);
      return;
    }
  if (argy[0] == '\0')
    {
      send_to_char ("Investigate what?\n\r", ch);
      return;
    }
  if ((obj = get_obj_list (ch, argy, ch->in_room->more->contents)) == NULL)
    {
      send_to_char ("You do not see that here.\n\r", ch);
      return;
    }
  if ((obj->pIndexData->item_type != ITEM_CORPSE_PC) &&
      (obj->pIndexData->item_type != ITEM_CORPSE_NPC))
    {
      send_to_char ("You can only investigate corpses.\n\r", ch);
      return;
    }
  ch->pcdata->tickcounts = 21;
  check_temp (ch);
  send_to_char ("You begin to investigate....\n\r", ch);
  strcpy (ch->pcdata->temp->temp_string, argy);
  NEW_POSITION(ch, POSITION_INVEST);
  return;
}

void 
do_actual_investigate (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *obj;
  if ((obj = get_obj_list (ch, argy, ch->in_room->more->contents)) == NULL)
    {
      send_to_char ("You do not see that here.\n\r", ch);
      NEW_POSITION(ch, POSITION_STANDING);
      return;
    }
  if ((obj->pIndexData->item_type != ITEM_CORPSE_PC) &&
      (obj->pIndexData->item_type != ITEM_CORPSE_NPC))
    {
      send_to_char ("You can only investigate corpses.\n\r", ch);
      NEW_POSITION(ch, POSITION_STANDING);
      return;
    }
  if (number_range (1, 90) > ch->pcdata->learned[gsn_invest])
    {
      act ("You try to investigate, but you are unable to draw any conclusions due to your limited skill.", ch, NULL, ch, TO_CHAR);
      NEW_POSITION(ch, POSITION_STANDING);
      return;
    }
  send_to_char ("You successfully investigate, and find...\n\r", ch);
  print_reasons (ch, obj);
  NEW_POSITION(ch, POSITION_STANDING);
  return;
}


void 
print_reasons (CHAR_DATA * ch, SINGLE_OBJECT * container)
{
  char buf[500];
  I_CONTAINER *cnt = (I_CONTAINER *) container->more;
  sprintf (buf, "Reason of death: %s.\n\r", (cnt->reason_for_death[0] == '\0' ? "Unknown" : capitalize (cnt->reason_for_death)));
  send_to_char (buf, ch);
  if (cnt->looted_by[0] != '\0')
    {
      if (cnt->looted_by_two[0] != '\0')
	{
	  sprintf (buf, "Looted by: %s and %s.\n\r", cnt->looted_by,
		   cnt->looted_by_two);
	}
      else
	{
	  sprintf (buf, "Looted by: %s.\n\r", cnt->looted_by);
	}
      send_to_char (buf, ch);
    }
  return;
}


void 
pc_death_penalty (CHAR_DATA * ch, CHAR_DATA * victim)
{
  int i;
  bool skill_loss = FALSE;
  if (IS_MOB (victim))
    return;
  if (IS_PLAYER (ch) && pow.skill_loss_on_pk_death)
    skill_loss = TRUE;
  if (IS_MOB (ch) && pow.skill_loss_on_mob_death)
    skill_loss = TRUE;
  if (LEVEL (victim) > 10 && skill_loss)
    {
      for (i = 0; i < SKILL_COUNT; i++)
	{
	  if (i == gsn_hide)
	    continue;
	  if (i == gsn_sneak)
	    continue;
	  if (i == gsn_backstab)
	    continue;
	  if (i == gsn_track)
	    continue;
	  if (i == gsn_dual_wield)
	    continue;
	  if (number_range (1, 3) != 2)
	    continue;
	  if (victim->pcdata->learned[i] < 5)
	    continue;
	  victim->pcdata->learned[i] -= pow.amount_lost_on_death;
	  if (victim->pcdata->learned[i] < 1)
	    victim->pcdata->learned[i] = 1;
	}
    }
  gain_exp (victim, -UMIN((LEVEL(victim)*LEVEL(victim) * 45),
(victim->exp/(LEVEL(victim) +2))));
  return;
}

void 
do_fatal (CHAR_DATA * victim, CHAR_DATA * ch, int dt)
{
  int chance_num;
  chance_num = number_range (1, 18);
  if (chance_num == 1 && can_groundfight (victim))
    {
      if (dt == 1001 || dt == 1)
	{
	  act ("Your slash rips into $N's neck, and tears it open!", ch, NULL, victim, TO_CHAR);
	  act ("$n's mighty slash rips your neck open!", ch, NULL, victim, TO_VICT);
	  act ("$n's slash slices $N's neck wide open!", ch, NULL, victim, TO_NOTVICT);
	}
      else if (dt == 1002 || dt == 2)
	{
	  act ("You puncture a hole in $N's neck with your pierce!", ch, NULL, victim, TO_CHAR);
	  act ("$n's stab punctures a large hole in your neck!", ch, NULL, victim, TO_VICT);
	  act ("$n pierces a large hole in $N's neck!", ch, NULL, victim, TO_NOTVICT);
	}
      else if (dt == 1003 || dt == 3)
	{
	  act ("Your powerful attack rips open $N's neck!", ch, NULL, victim, TO_CHAR);
	  act ("$n's attack tears into your neck!", ch, NULL, victim, TO_VICT);
	  act ("$n's attack rips open $N's neck!", ch, NULL, victim, TO_NOTVICT);
	}
      else if (dt == 1005 || dt == 5)
	{
	  act ("Your pound crushes $N's windpipe! Ouch!", ch, NULL, victim, TO_CHAR);
	  act ("$n's powerful pound crushes your windpipe!", ch, NULL, victim, TO_VICT);
	  act ("$n pounds $N's neck, crushing $S windpipe!", ch, NULL, victim, TO_NOTVICT);
	}
      else
	{
	  act ("$n catches you across the neck. Your neck is torn open!", ch, NULL, victim, TO_VICT);
	  act ("$n catches $N across the neck. Ouch!", ch, NULL, victim, TO_NOTVICT);
	  act ("You catch $N across the neck; that's gotta hurt!", ch, NULL, victim, TO_CHAR);
	}
      victim->hit = 1;
    }
  if (chance_num == 2)
    {
      if (dt == 1001 || dt == 1)
	{
	  act ("Your slash tears deep into $N's body!", ch, NULL, victim, TO_CHAR);
	  act ("$n's slash tears deep into your body!!", ch, NULL, victim, TO_VICT);
	  act ("$n's slash tears deep into $N's body!", ch, NULL, victim, TO_NOTVICT);
	}
      else if (dt == 1002 || dt == 2)
	{
	  act ("You stab a hole right through $N's gut!", ch, NULL, victim, TO_CHAR);
	  act ("$n stabs a hole right through your gut!", ch, NULL, victim, TO_VICT);
	  act ("$n stabs a hole right through $N's gut!", ch, NULL, victim, TO_NOTVICT);
	}
      else if (dt == 1003 || dt == 3)
	{
	  act ("Your powerful attack sends $N's eyeball flying!", ch, NULL, victim, TO_CHAR);
	  act ("Your eyeball is torn from your socket by $n's attack!", ch, NULL, victim, TO_VICT);
	  act ("$n tears an eyeball off of $N with $s attack!", ch, NULL, victim, TO_NOTVICT);
	}
      else if (dt == 1005 || dt == 5)
	{
	  act ("You can feel $N's skull cave in under your pound!", ch, NULL, victim, TO_CHAR);
	  act ("You can feel your skull cave in under $n's pound!", ch, NULL, victim, TO_VICT);
	  act ("$n crushes $N's skull with a mighty pound!", ch, NULL, victim, TO_NOTVICT);
	}
      else
	{
	  act ("$n's attack sends $N flying backwards in pain!", ch, NULL, victim, TO_NOTVICT);
	  act ("Your attack sends $N flying backwards in pain!", ch, NULL, victim, TO_CHAR);
	  act ("$n's attack sends you flying backwards in pain!", ch, NULL, victim, TO_VICT);
	}
      victim->hit = 1;
    }
  if (chance_num == 3)
    {
      if (dt == 1001 || dt == 1)
	{
	  act ("Your slash penetrates deep into $N's skull!", ch, NULL, victim, TO_CHAR);
	  act ("$n's slash penetrates deep into your skull!!", ch, NULL, victim, TO_VICT);
	  act ("$n's slash penetrates deep into $N's skull!", ch, NULL, victim, TO_NOTVICT);
	}
      else if (dt == 1002 || dt == 2)
	{
	  act ("Your stab hits $N right in the temple!", ch, NULL, victim, TO_CHAR);
	  act ("$n's stab pierces your temple and enters your brain!", ch, NULL, victim, TO_VICT);
	  act ("$n stabs a deep hole into $N's temple!", ch, NULL, victim, TO_NOTVICT);
	}
      else if (dt == 1003 || dt == 3)
	{
	  return;
	}
      else if (dt == 1005 || dt == 5)
	{
	  act ("You can hear $N's neck break as your pound connects!", ch, NULL, victim, TO_CHAR);
	  act ("Your neak breaks under the force of $n's attack!", ch, NULL, victim, TO_VICT);
	  act ("$n breaks $N's neck with a powerful pound!", ch, NULL, victim, TO_NOTVICT);
	}
      else
	{
	  return;
	}
      victim->hit = 1;
    }
  return;
}

void 
make_corpse (CHAR_DATA * ch)
{
  char buf[STD_LENGTH];
  SINGLE_OBJECT *corpse = NULL;
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj_next;
  char *name = NULL;
  check_room_more (ch->in_room);
  if (IS_MOB (ch) && IS_CORPSE_MOB (ch))
    {
      name = NAME (ch);
      corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_NPC), 0);
      corpse->timer = number_range (2, 15);
      ((I_CONTAINER *) (corpse->more))->key_vnum=ch->pIndexData->vnum;
    }
  else if (IS_PLAYER (ch))
    {
      name = NAME (ch);
      corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_PC), 0);
      corpse->timer = number_range (100, 150);
    }

  if (corpse && reasonfd[0] != '\0')
    {
      strcpy (((I_CONTAINER *) corpse->more)->reason_for_death, reasonfd);
    }
  reasonfd[0] = '\0';

  if (!(IS_MOB (ch) && !IS_CORPSE_MOB (ch)))
    {
      sprintf (buf, STRR (corpse, short_descr), name);
      if (corpse->short_descr)
	free_string (corpse->short_descr);
      corpse->short_descr = str_dup (buf);
      sprintf (buf, STRR (corpse, description), name);
      if (corpse->description)
	free_string (corpse->description);
      corpse->description = str_dup (buf);
    }
  if (IS_PLAYER (ch) || (IS_MOB (ch) && ch->pIndexData->pShop == NULL))
    {
      if (IS_MOB (ch) && !IS_CORPSE_MOB (ch))
	{
	  check_room_more (ch->in_room);
	  ch->in_room->more->copper += ch->copper;
	  ch->in_room->more->gold += ch->gold;
	}
      else 
	{
	  ((I_CONTAINER *) corpse->more)->money += tally_coins (ch);
	  ch->gold=0;
	  ch->copper=0;
	}
    }

  /* -------------------------------------------------- */
  /*   		 Loop through items			*/
  /* -------------------------------------------------- */
  for (obj = ch->carrying; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;

      /* ------------------------------------------------------- */
      /* If the item is ITEM_SAVE, then do nothing for that item */
      /* ------------------------------------------------------- */
      if (IS_MOB(ch))
      {
        if(!IS_SET(obj->extra_flags, ITEM_SAVE))
	{
	      obj_from (obj);
	      if (IS_SET (obj->extra_flags, ITEM_INVENTORY) || (IS_PLAYER (ch) && IS_SET (obj->extra_flags, ITEM_NODROP)))
		{
		  free_it (obj);
		}
	      else if (IS_MOB (ch) && !IS_CORPSE_MOB (ch))
		obj_to (obj, ch->in_room);
	      else
		obj_to (obj, corpse);
          }
      }
/* THIS SETS PARTIAL LOOTING SO IF IS 1/2 thats half of equipment */     
else
      {
      if((!IS_SET(obj->extra_flags, ITEM_SAVE) && number_range(1,1) == 1)
        || IS_SET(ch->pcdata->act2, PLR_HARDCORE))
	{
	      obj_from (obj);
	      if (IS_SET (obj->extra_flags, ITEM_INVENTORY) || (IS_PLAYER (ch) && IS_SET (obj->extra_flags, ITEM_NODROP)))
		{
		  free_it (obj);
		}
	      else if (IS_MOB (ch) && !IS_CORPSE_MOB (ch))
		obj_to (obj, ch->in_room);
	      else
		obj_to (obj, corpse);
	}
      }
    }


  if (!(IS_MOB (ch) && !IS_CORPSE_MOB (ch)))
    {
      ch->gold = 0;
      ch->copper = 0;
      obj_to (corpse, ch->in_room);
    }
  return;
}

void 
death_cry (CHAR_DATA * ch, bool flagg)
{
  ROOM_DATA *was_in_room;
  char *msg;
  int door;
  int vnum;
  vnum = 0;
  if (IS_MOB (ch) && ch->pIndexData->mobtype == MOB_REPTILE)
    {
      msg = "$n turns into a bloody red mess of blood and gore.";
    }
  else if (IS_MOB (ch) && ch->pIndexData->mobtype == MOB_UNDEAD)
    {
      msg = "$n turns to dust before your eyes.";
    }
  else if (IS_MOB (ch) && ch->pIndexData->mobtype == MOB_GHOST)
    {
      msg = "$n disappears into thin air...";
    }
  else if (IS_MOB (ch) && ch->pIndexData->mobtype == MOB_FISH)
    {
      msg = "$n turns the water red and sinks beneath the surface...";
    }
  else if (IS_MOB (ch) && ch->pIndexData->mobtype == MOB_PLANT)
    {
      msg = "$n turns into a lifeless pile of weeds.";
    }
  else if (IS_MOB (ch) && ch->pIndexData->mobtype == MOB_STATUE)
    {
      msg = "$n crumbles into a pile of dust.";
    }
  else if (IS_MOB (ch) && ch->pIndexData->mobtype == MOB_BIRD)
    {
      msg = "$n falls to the ground, dead.";
    }
  else if (flagg == FALSE || !can_yell (ch))
    {
      msg = "You hear $n's death cry.";
    }
  else
    switch (number_bits (4))
      {
      default:
	msg = "You hear $n's death cry.";
	break;
      case 0:
	msg = "$n hits the ground ... DEAD.";
	break;
      case 1:
	msg = "$n splatters blood on your armor.";
	break;
      case 2:
	msg = "You smell $n's sphincter releasing in death.";
	vnum = OBJ_VNUM_INTESTINES;
	break;
      case 3:
	msg = "A fountain of blood forms as $n's head falls to the ground";
	vnum = OBJ_VNUM_SEVERED_HEAD;
	break;
      case 4:
	msg = "$n's heart is torn from $s chest.";
	vnum = OBJ_VNUM_TORN_HEART;
	break;
      case 5:
	msg = "A stream of blood forms as $n's arm is sliced from $s dead body.";
	vnum = OBJ_VNUM_SLICED_ARM;
	break;
      case 6:
	msg = "Blood runs everywhere as $n's leg is sliced from $s dead body.";
	vnum = OBJ_VNUM_SLICED_LEG;
	break;
      }
  act (msg, ch, NULL, NULL, TO_ROOM);
  if (vnum != 0)
    {
      char buf[STD_LENGTH];
      SINGLE_OBJECT *obj;
      char *name;
      name = NAME (ch);
      obj = create_object (get_obj_index (vnum), 0);
      obj->timer = number_range (4, 7);
      sprintf (buf, STRR (obj, short_descr), name);
      /*if (obj->short_descr && obj->short_descr!=NULL) free_string( obj->short_descr );
       */ obj->short_descr = str_dup (buf);
      sprintf (buf, STRR (obj, description), name);
      /*if (obj->description && obj->description!=NULL) free_string( obj->description );
       */ obj->description = str_dup (buf);
      obj_to (obj, ch->in_room);
    }
  if (IS_MOB (ch))
    msg = "You hear something's death cry.";
  else
    msg = "You hear someone's death cry.";
  was_in_room = ch->in_room;
  for (door = 0; door <= 5; door++)
    {
      EXIT_DATA *pexit;
      if ((pexit = was_in_room->exit[door]) != NULL
	  && pexit->to_room != NULL
	  && pexit->to_room != was_in_room)
	{
	  ch->in_room = pexit->to_room;
	  act (msg, ch, NULL, NULL, TO_ROOM);
	}
    }
  ch->in_room = was_in_room;
  return;
}

void 
raw_kill (CHAR_DATA * victim, bool used_sword)
{
  CHAR_DATA *tmpch;
  CHAR_DATA *tmpchnx;
  CHAR_DATA *attacker = NULL;
  ROOM_DATA *in_room;
  int i;
  if (IS_PLAYER(victim) && victim->desc)
    {
      victim->desc->inbuf[0] = '\0';
      victim->desc->incomm[0] = '\0';
    }
  in_room = victim->in_room;  
  check_ced (victim);
  check_fgt (victim);
  for (i = 0; i < 4; i++)
    victim->ced->attackers[i] = NULL;
  if (in_room)
    {
      check_room_more(in_room);
      for (tmpch  = in_room->more->people; tmpch != NULL; tmpch = tmpchnx)
        {
          tmpchnx = tmpch->next_in_room;
          if (tmpch->fgt && tmpch->fgt->fighting == victim)
            {
              tmpch->fgt->fighting = NULL;
              tmpch->position = POSITION_STANDING;
            }
          if(tmpch->fgt && tmpch->fgt->phunting && 
	      !str_cmp(tmpch->fgt->phunting, NAME(victim)))
            {
               free_string(tmpch->fgt->phunting); 
               tmpch->fgt->phunting = NULL;
            }
          if (tmpch->fgt && tmpch->fgt->hunting &&
            !str_cmp(tmpch->fgt->hunting, NAME(victim)))
            { 
               free_string(tmpch->fgt->hunting);
               tmpch->fgt->hunting = NULL;
             }
         }
     
     }
  if (IS_PLAYER(victim) && victim->desc)
    {
      victim->desc->incomm[0] = '\0';
      victim->desc->inbuf[0] = '\0';
    }
  if (victim->fgt->fighting != NULL)
    {
      attacker = victim->fgt->fighting;
      if (attacker != NULL && IS_MOB(victim) &&
	  IS_SET(victim->pIndexData->act, ACT_AVENGED) &&
	  (victim->pIndexData->opt != NULL) &&  (victim->pIndexData->opt->avenger > 1) && ((get_mob_index(victim->pIndexData->opt->avenger)) != NULL))
	{
	  ROOM_DATA *to_room;
	  int num = number_range(1, ((LEVEL(victim)/15)+1));
	  int i;
	  int lower_area_vnum = victim->pIndexData->area->lvnum;
	  int upper_area_vnum = victim->pIndexData->area->uvnum;
	  for (i = 0; i < num; i++)
	    {
	      int new_room_num = number_range(lower_area_vnum, upper_area_vnum);
	      if ((to_room = get_room_index(new_room_num)) != NULL)
		{
		  CHAR_DATA *avenger;
		  avenger = create_mobile(get_mob_index(victim->pIndexData->opt->avenger));
		  char_to_room(avenger, to_room);
		  check_fgt(avenger);
		  if (attacker !=NULL)
		    {
		      avenger->fgt->hunting = str_dup(RNAME(attacker));
		      avenger->fgt->phunting = str_dup(RNAME(attacker));
		    }
		}
	      
	    }
	}
      victim->fgt->fighting = NULL;
      NEW_POSITION(victim, POSITION_RESTING);
    }
  if (IS_PLAYER(victim))
    {
	
          victim->pcdata->died_recently++;
	            if (victim->pcdata->died_recently > 2)
         send_to_char ("\x1B[1;35m
**WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING**\n\r
**WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING**\n\r
**WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING**\n\r
             ONE MORE DEATH AND YOU WILL BE FROZEN!\n\r
        DO **NOT** EXPECT TO GET UNFROZEN IF YOU DIE AGAIN.\n\r
**WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING**\n\r
**WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING**\n\r
**WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING*WARNING**\n\r
   	     LOG OUT FOR A BIT AND COME BACK LATER
\x1b[0;37m\n\r \n\r",victim);
          if (victim->pcdata->died_recently > 3)
            SET_BIT(victim->act, PLR_FREEZE);
	
    }
  if (IS_PLAYER (victim) && LEVEL (victim) > 1 && 
      victim->pcdata->carry_number > 0)
  save_corpse (victim);
  for (tmpch = char_list; tmpch != NULL; tmpch = tmpchnx)
    {
      tmpchnx = tmpch->next;
      if (tmpch->fgt && (tmpch->fgt->fighting == victim))
        {
	  CHAR_DATA *cd, *cdnext;
	  tmpch->fgt->fighting = NULL;
	  tmpch->position = POSITION_STANDING;
	  if (tmpch->in_room)
	    {
	      check_room_more(tmpch->in_room);
	      for (cd = tmpch->in_room->more->people; cd != NULL; cd = cdnext) 
		{
		  cdnext = cd->next_in_room;
		  if  (cd->fgt && cd->fgt->fighting == tmpch)
		    {   
		      tmpch->fgt->fighting = cd;
		      tmpch->position = POSITION_FIGHTING;   
		    }
		}
	    }
	  if (IS_PLAYER (tmpch))
	    continue;
        }
     if (HUNTING (tmpch) && !str_cmp (tmpch->fgt->hunting, NAME (victim)))
        {
	   free_string (tmpch->fgt->hunting);
	   tmpch->fgt->hunting = NULL;
	}
    }
  stop_fighting(victim, TRUE);
  death_cry (victim, used_sword);


  if (MOUNTED_BY (victim) != NULL)
    {
      check_fgt (victim->fgt->mounted_by);
      act ("$n dies, and you fall to the ground.",
	   victim, NULL, victim->fgt->mounted_by, TO_VICT);
      act ("$n dies, causing $N to fall to the ground!", victim, NULL, victim->fgt->mounted_by, TO_NOTVICT);
      check_fgt(victim->fgt->mounted_by);
	victim->fgt->mounted_by->fgt->riding = NULL;
      NEW_POSITION(victim->fgt->mounted_by, POSITION_RESTING);
      victim->fgt->mounted_by = NULL;
    }
  if (RIDING (victim) != NULL)
    {
      act ("$n falls off you, dead.",
	   victim, NULL, victim->fgt->riding, TO_VICT);
      check_fgt (victim->fgt->riding);
      victim->fgt->riding->fgt->mounted_by = NULL;
      victim->fgt->riding = NULL;
    }
  make_corpse(victim);
  if (IS_MOB (victim))
    {
      victim->pIndexData->killed++;
      extract_char (victim, TRUE);
      return;
    }
  extract_char (victim, FALSE);
  
  
  while (victim->affected)
    affect_remove (victim, victim->affected);
  
  
  if (IS_PLAYER (victim))
    {
      victim->affected_by = victim->pcdata->nat_abilities;
      victim->more_affected_by = victim->pcdata->nat_abilities2;
    }
  victim->armor = 100;
  NEW_POSITION(victim, POSITION_RESTING);
  victim->hit = UMAX (5, victim->hit);
  victim->move = UMAX (5, victim->move);
  save_char_obj (victim);
  victim->pcdata->plus_kick = 0;
  victim->pcdata->plus_hide = 0;
  victim->pcdata->plus_sneak = 0;

  if (IS_SET(victim->pcdata->act2, PLR_HARDCORE))
    {
      CHAR_DATA *sch;
      char buf2[STD_LENGTH];
      sch = create_mobile(get_mob_index(1));
      char_to_room(sch, get_room_index(2));
      sprintf(buf2, "$B$7HARDCORE NOTIFY : $B$4%s $B$7has been killed!!\n\r", victim->pcdata->name);
      do_echo(sch, buf2);
      send_to_char("You are now frozen for eternity.  Please delete.\n\r", victim);
//      victim->pcdata->title = "used to be a hardcore player.  R.I.P.";
//      SET_BIT(victim->act, PLR_FREEZE);
//      do_pdelete(sch, victim->pcdata->name);
// THIS CRASHES THE MUD FOR NOW
      victim->pcdata->temp_room = get_room_index(8);
      char_to_room(victim, victim->pcdata->temp_room);
      do_look(victim, "auto");
      extract_char(sch, TRUE);
    }

  return;
}




void 
group_gain (CHAR_DATA * ch, CHAR_DATA * victim)
{
  CHAR_DATA *gch;
  char g_exp[500];
  char buffer[500];
  bool pkill = FALSE;
  int xp, total_xp;
  EXIT_DATA *pexit;
  ROOM_DATA *in_room = victim->in_room;
  ROOM_DATA *curr_room = victim->in_room;
  ROOM_DATA *to_room = NULL;
  int members = 0;
  int door, range;
  int highest;
  int same_align = 0;
  int opp_align = 0;
  if (victim == ch)
    return;
  if (!victim->in_room) return;
  if (IS_PLAYER(ch) && IS_PLAYER(victim) && DIFF_ALIGN(ch, victim))
    {
      pkill = TRUE;
      fprintf(stderr, "Pkill: ");
    }
	
  // We want this next line commented out because necromancers can raise mobs
  // to fight for them - the get the xp for that too.
  //if (IS_MOB(ch)) return;
  check_room_more (victim->in_room);

  if(!IS_MOB(ch)) highest = LEVEL(ch);
  else highest = 1;


  /* This next piece of code counts up the number of people of the
     same align as the person who killed the victim, the number of the
     opposite align people from who killed the victim, the number
     of group members in the killer's group nearby (within 3 linear
     spaces of the victim room...it also checks for shitty building
     and mazes by stopping at looping rooms to avoid double awards of
     exp. */


  victim->in_room->y = FALSE;
  for (door = 0; door < MAX_DIR; door++) 
    {
      curr_room = victim->in_room;
      for (range = 0; range < 3 && curr_room != NULL; range++) 
	{
	  if(curr_room->y && curr_room != victim->in_room) break;
	  curr_room->y = TRUE;
	  check_room_more(curr_room);
	  for (gch = curr_room->more->people; gch != NULL; gch =gch->next_in_room) 
	    {
	      if(door == 0 || range > 0)
		{
		  if (pkill)
		    {
		      if (IS_PLAYER(gch))
			{
			  if (!DIFF_ALIGN(ch, gch))
			    {
			      // Check to see if the gch is really a mob
			      // since necromancers can have grouped mobiles.
			      if(!IS_MOB(gch)) {
			        same_align ++;
			        members++;
                                fprintf(stderr, "%s ", NAME(gch));
			        if (LEVEL(gch) > highest)
				  highest = LEVEL(gch);
                              }
			    }
			  else 
			    opp_align ++;
			}
		    }
		  else 
		    {
		      if (is_same_group(ch, gch))
			{
			  // Check to see if the gch is really a mob
			  // since necromancers can have grouped mobiles.
			  if(!IS_MOB(gch)) {
			    members++;
  			    if (LEVEL (gch) > highest)
			      highest = LEVEL (gch);
                          }
			}
		    }
		}
	    }
	  if (((pexit = (EXIT_DATA *) curr_room->exit[door]) != NULL) &&
	      ((to_room = (ROOM_DATA *) pexit->to_room) != NULL) 
	      && (!pexit->d_info || !IS_SET(pexit->d_info->exit_info, EX_CLOSED)))
	    curr_room = to_room;
	  else
	    curr_room = NULL;          
	}
    }
   if (pkill) fprintf(stderr, "\n");
  /* Clean up the room markers. */

 //sprintf(buffer,"Highest level in group is %d\n\r",highest);
 //do_echo (ch,buffer);

  for (door = 0; door < MAX_DIR; door++)
    {
      curr_room = victim->in_room;
      for (range = 0; range < 3 && curr_room != NULL; range++)
	{
	  curr_room->y = FALSE;
	  if (((pexit = (EXIT_DATA *) curr_room->exit[door]) != NULL) &&
	      ((to_room = (ROOM_DATA *) pexit->to_room) != NULL) 
	      && (!pexit->d_info || !IS_SET(pexit->d_info->exit_info, EX_CLOSED)))
	    curr_room = to_room;
	  else
	    curr_room = NULL;          
	}
    }
  
 


  if (opp_align == 0)
    opp_align = 1;
  if (same_align == 0)
    same_align = 1;
  if (members == 0)
    members = 1;
  xp = translate(pow.mob_exp, LEVEL(victim), victim); 
  if (IS_PLAYER(victim))
/* THIS CHANGED PKEXP FOR WHEN YOU ATTACK THINGS STRUCTURE */
    xp *= ((LEVEL(victim)/8) * (((victim->pcdata->remort_times + 1) * 2)*6));
  else
    {
    xp *= 3;  /* THIS TEMPORARILY X2 MOB EXP -- STRUCTURE */
    if (ch->pcdata->level < 40)
      xp = xp*3/2;
    if (ch->pcdata->remort_times > 1)
      xp = xp * (1 - .05 * ch->pcdata->remort_times);
    }
  xp +=(number_range(0, xp/10));
  big_group_pkill_whore = FALSE;
    if (pkill) 
    {
      members = same_align;
      if (same_align > opp_align + 2 && victim->in_room->area&&!victim->in_room->area->homeland && same_align > 5)
	// If there are too many people close to the victim, and the victim's
	// room was not inside a homeland area. 
	{
	  //big_group_pkill_whore = TRUE;
	  xp = 0;
	  fprintf(stderr, "BGPKW!!!!\n");
          do_yell(ch, "WE ARE HORDING LAMERS AND SHOULD BE DELETED!!!\n\r");
	}
    }
  if (members > 6)
    xp /= members;
  if (members > 7)
    xp /= 2*members;
  total_xp = xp;
  /* Whore pkill is set if a person pkills a diff aligned
      person and (currently) 6 or more other people of
     his alignment are within range of getting the exp
     This is also used for the make_corpse(victim);
     function because if the group engages in big group
     pkill whore tactics. they get NO eq either. */
  if (!big_group_pkill_whore)        
    {
      if (pkill)
	{  
	  victim->pcdata->pkilled++;
	  victim->pcdata->pkers+=same_align;
	  if (LEVEL(ch) <= LEVEL(victim) && same_align == 1 &&
	      ch->pcdata->remort_times <= victim->pcdata->remort_times &&
	      LEVEL(ch) > 45)
	    ch->pcdata->hard_pkills++;
	}
      victim->in_room->y = FALSE;
      for (door = 0; door < MAX_DIR; door++)
	{
	  curr_room = victim->in_room;
	  for (range = 0; range < 3 && curr_room != NULL; range++)
	    {
	      if (curr_room->y && curr_room != victim->in_room) break;
	      if (door == 0 || range > 0)
		{
		  curr_room->y = TRUE;
		  if (range > 0 && curr_room == victim->in_room) continue;
		  check_room_more(curr_room);
		  for (gch = curr_room->more->people; gch != NULL; gch = gch->next_in_room)
		    {
		      char buf[500];
		      xp = total_xp;
		      if (IS_PLAYER(gch) && pkill && !DIFF_ALIGN(ch, gch)) 
			{
			  check_add_trophy (gch, victim, same_align, opp_align);
			  gch->pcdata->pkills++;
			  gch->pcdata->pklevels += LEVEL(victim);
			  gch->pcdata->pkhelpers += same_align;
			}
		      if (is_same_group(ch, gch) || (!DIFF_ALIGN(ch, gch) && pkill))
			{
			  if ((highest - LEVEL(gch)) > 13)
			    {
			      if (highest < 39) xp /=2;
			      if ((highest-LEVEL(gch)) > 13 && highest < 49) xp /= 2;
			      if ((highest-LEVEL(gch)) > 18 && highest < 67) xp /= 3;
			      if ((highest-LEVEL(gch)) > 23 && highest < 75) xp /= 4;
			      if ((highest-LEVEL(gch)) > 34 && highest < 85) xp /= 5;
			      if ((highest-LEVEL(gch)) > 45) xp/= 6;
			    }
			  gch->pcdata->totalkills++;
			  if (LEVEL (gch) > 15 && LEVEL (gch) < (LEVEL (victim) + 29))
			    gch->pcdata->killpoints++;
			  if (LEVEL (gch) > 25 && LEVEL (gch) < (LEVEL (victim) + 37))
			    gch->pcdata->killpoints++;
			  if (LEVEL (gch) > 45 && LEVEL (gch) < (LEVEL (victim) + 44))
			    gch->pcdata->killpoints++;
			  if (LEVEL (gch) > 55 && LEVEL (gch) < (LEVEL (victim) + 53))
			    gch->pcdata->killpoints++;
			  if (IS_PLAYER (gch) && IS_PLAYER (victim) && (!DIFF_ALIGN(gch, victim)))
			    xp = 1;      
                          if (IS_SET(gch->pcdata->act2, PLR_HARDCORE))
                            xp *=3;
			  sprintf (g_exp, "You've gained %d experience for the kill, and %d for fighting.\n\r", xp, gch->pcdata->voting_on);
			  send_to_char (g_exp, gch);
			  gch->pcdata->voting_on = 0;
			  gain_exp (gch, xp);
			}
		    }
		}
	      if ((pexit = curr_room->exit[door]) != NULL &&
		  ((to_room = pexit->to_room) != NULL) &&
		  (!pexit->d_info ||!IS_SET(pexit->d_info->exit_info,EX_CLOSED)))
		curr_room = to_room;
	      else
		curr_room = NULL;     
	    }
	}
      for (door = 0; door < MAX_DIR; door++)
	{
	  curr_room = victim->in_room;
	  for (range = 0; range < 4 && curr_room != NULL; range++)
	    {
	      curr_room->y = FALSE;
	      if (((pexit = (EXIT_DATA *) curr_room->exit[door]) != NULL) &&
		  ((to_room = (ROOM_DATA *) pexit->to_room) != NULL) 
		  && (!pexit->d_info || !IS_SET(pexit->d_info->exit_info, EX_CLOSED)))
		curr_room = to_room;
	      else
		curr_room = NULL;          
	    }
	}
    }
  return;
}

