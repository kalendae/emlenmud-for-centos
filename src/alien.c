#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

#define AUGMENT_ROOM 111




void 
do_augment (CHAR_DATA * ch, char *argy)
{
  char arg1[STD_LENGTH];
  char buf[STD_LENGTH];
  int i;
  int num;  
  int num_wanted;
  bool can_augment;
DEFINE_COMMAND ("augment", do_augment, POSITION_STANDING, 1, LOG_NORMAL, "This command is used to get augmentations to improve your character.")


if (IS_MOB (ch))
     return;
if (argy[0] == '\0')
  {
    send_to_char("Type augment list or augment cost for more information on this.\n\r", ch);
    return;
  }
argy = one_argy(argy, arg1);
if (!str_cmp(arg1, "list"))
  {
    show_augment_list(ch, argy);
    return;
  }

if (pow.augmentremortonly == TRUE)
  {
    send_to_char ("Stand alone augmentation has been disabled, in order to augment\n\r",ch);
    send_to_char ("you will have to augment at remort, for help on doing this type help transform\n\r", ch);
    return;
  }
if (!str_cmp(arg1, "costs") || !str_cmp(arg1, "cost")) 
  {  
    show_augment_costs(ch, argy);
    return;
  }

if (ch->in_room->vnum != AUGMENT_ROOM)
  {
    send_to_char ("You cannot get augmented here.\n\r", ch);
    return;
  }

if (!str_cmp(arg1, "replace"))
  {
    char arg2[STD_LENGTH];
    char arg3[STD_LENGTH];
    int oldnum = -1;
    int newnum = -1;
    int i;
    argy = one_argy(argy, arg2);
    argy = one_argy(argy, arg3);
    if (arg2[0] == '\0' || arg3[0] == '\0')
      {
	send_to_char("Syntax: augment replace <old_aug> <new_aug>\n\r", ch);
	return;
      }
    for (i = 0; str_cmp(augment_flagss[i].what_you_type, "end_of_list"); i++)
      {
	if (!str_cmp(augment_flagss[i].what_you_type, arg2) || !str_cmp(augment_flagss[i].how_it_appears, arg2))
	  {
	    oldnum = augment_flagss[i].flagname;
	    break;
	  }
      }
    for (i = 0; str_cmp(augment_flagss[i].what_you_type, "end_of_list"); i++)
      {
	if (!str_cmp(augment_flagss[i].what_you_type, arg3) ||
            !str_cmp(augment_flagss[i].how_it_appears, arg3))
	  {
	    newnum = augment_flagss[i].flagname;
	    break;
	  }
      }
    if (oldnum == -1 || newnum == -1)
      {
	send_to_char("You need to check those names again. I don't know which augmentations you are talking about.\n\r", ch);
	return;
      }
    if (!IS_SET(ch->pcdata->augments, oldnum))
      {
	send_to_char("You don't have that augmentation so how do you expect to replace it?\n\r", ch);
	return;
      }
    if (newnum == oldnum)
      {
	send_to_char("You are not changing anything!! Get a clue.\n\r", ch);
	return;
      }
    if (IS_SET(race_info[ch->pcdata->race].nat_augments, oldnum))
      {
	send_to_char("You cannot remove an augmentation that is natural to your race!\n\r", ch);
	return;
      }
    if (IS_SET(align_info[ch->pcdata->alignment].nat_augments, oldnum))
      {
	send_to_char("You cannot remove an augmentation that is natural to your alignment!\n\r", ch);
	return;
      }
    if (IS_AUGMENTED(ch, newnum))
      {
	send_to_char("You already have that augmentation!\n\r", ch);
	return; 
      }
    if (ch->pcdata->bank < 5000)
      {
	send_to_char("You need 5000 coins in the bank for this to be done.\n\r", ch);
	return;
      }
    ch->pcdata->bank -= 5000;
    REMOVE_BIT(ch->pcdata->augments, oldnum);
    SET_BIT(ch->pcdata->augments, newnum);
    fix_char(ch);
    send_to_char("Ok, the augmentations have been switched.\n\r", ch);
    return;
  }
num_wanted = -1;
for (i = 0; str_cmp(augment_flagss[i].what_you_type, "end_of_list"); i++)
  {
    if (!str_cmp(augment_flagss[i].what_you_type, arg1) ||
         !str_cmp(augment_flagss[i].how_it_appears, arg1))
      {
	num_wanted = i;
	break;
      }
  }
if (num_wanted == -1)
  {
    send_to_char("Which augmentation did you want to buy?\n\r", ch);
    return;
  }
if (IS_AUGMENTED(ch, augment_flagss[num_wanted].flagname))
  {
    send_to_char("You already have that augmentation!\n\r", ch);
    return; 
  }
num = get_num_augments(ch);
if (num > 11)
  {
    send_to_char("You cannot get anymore augmentations.\n\r", ch);
    return;
  }
can_augment = TRUE;
if (ch->pcdata->remort_times < pow.augment_cost[num][0])
  {
    sprintf(buf, "You need %d remorts to get this augment and you only have %d.\n\r", pow.augment_cost[num][0], ch->pcdata->remort_times);
    send_to_char(buf, ch);
    can_augment = FALSE;
  }
if (LEVEL(ch) < pow.augment_cost[num][1])
  {
    sprintf(buf, "You need to be level %d to get this augment and you are only level %d.\n\r", pow.augment_cost[num][1], LEVEL(ch));
    send_to_char(buf, ch);
    can_augment = FALSE;
  }
if (tally_coins(ch) < (pow.augment_cost[num][2] * 100))
  {
    sprintf(buf, "You need %d gold for this augment and you only have %d gold.\n\r", pow.augment_cost[num][2], (tally_coins(ch)/100));
    send_to_char(buf, ch);
    can_augment = FALSE;
  }
if (ch->pcdata->warpoints < pow.augment_cost[num][3])
  {
    sprintf(buf, "You need %d warpoints to get this augment and you only have %ld.\n\r", pow.augment_cost[num][3], ch->pcdata->warpoints);
    send_to_char(buf, ch);
    can_augment = FALSE;
  }
if (ch->pcdata->killpoints < pow.augment_cost[num][4])
  {
    sprintf(buf, "You need %d killpoints to get this augment and you only have %d.\n\r", pow.augment_cost[num][4], ch->pcdata->killpoints);
    send_to_char(buf, ch);
    can_augment = FALSE;
  }
if (!can_augment)
  return;
ch->pcdata->warpoints -= pow.augment_cost[num][3];
sub_coins ((pow.augment_cost[num][2]*100), ch);
ch->pcdata->killpoints -= pow.augment_cost[num][4];
ch->pcdata->augments ^= augment_flagss[num_wanted].flagname;
sprintf(buf, "Congratulations %s, you now have the %s augment!\n\r", NAME(ch), augment_flagss[num_wanted].how_it_appears);
send_to_char(buf, ch);
fix_char(ch);
return;


}



void
show_augment_list(CHAR_DATA *ch, char *argy)
{
  char buf[STD_LENGTH];
  int num = 0;
  int augment;
  augment = ch->pcdata->augments;
  send_to_char ("Augment <name> <replace> <help> <cost>\n\r", ch);
  for (num = 0; str_cmp (augment_flagss[num].what_you_type, "end_of_list")  ; num++)
    {
      if (IS_SET(augment, augment_flagss[num].flagname))
	sprintf(buf, "-->");
      else
	sprintf(buf, "   ");
      send_to_char(buf,ch);
      sprintf(buf, " %14s: %s", augment_flagss[num].how_it_appears,augment_flagss[num].what_it_does);
      send_to_char(buf, ch);
    }
  
  return; 
}

void
show_affect_list(CHAR_DATA *ch, char *argy)
{
  char buf[STD_LENGTH];
  int num = 0;
  int affect;
  DEFINE_COMMAND ("affectlist", show_affect_list, POSITION_STANDING, 1,
LOG_NORMAL, "This command is used to list all the possible affects..")
    
    affect = ch->affected_by;
  send_to_char("This is a list of all the affects (natural abilities):\n\r", ch);
  send_to_char("There is an arrow next to each natural ability you have.\n\n\r", ch);
  for (num = 0; str_cmp (affect_flagss[num].what_you_type, "end_of_list")  ; num++)
    {
      if (IS_SET(affect, affect_flagss[num].flagname))
	sprintf(buf, "-->");
      else
	sprintf(buf, "   ");
      send_to_char(buf,ch);
      sprintf(buf, " %14s: %s", affect_flagss[num].what_you_type,affect_flagss[num].what_it_does);
      send_to_char(buf, ch);
    }
  
  
  return; 
}

void
show_affect_list2(CHAR_DATA *ch, char *argy)
{
  char buf[STD_LENGTH];
  int num = 0;
  int affect;
  DEFINE_COMMAND ("affecttwo", show_affect_list2, POSITION_STANDING, 1, LOG_NORMAL, "This command is used to list all the possible affects..") 
    affect = ch->more_affected_by;
  send_to_char("This is a list of all the available extra affects:\n\r", ch);
  send_to_char("There is an arrow next to each natural extra affect you have.\n\n\r", ch);
  for (num = 0; str_cmp (affect_flags_2[num].what_you_type, "end_of_list")  ; num++)
    {
      if (IS_SET(affect, affect_flags_2[num].flagname))
	sprintf(buf, "-->");
      else
	sprintf(buf, "   ");
      send_to_char(buf,ch);
      sprintf(buf, " %14s: %s", affect_flags_2[num].what_you_type,affect_flags_2[num].what_it_does);
      send_to_char(buf, ch);
    }
  return; 
}






int
get_num_augments(CHAR_DATA *ch)
  {
    int num = 0;
    int i=0;
    int augment = ch->pcdata->augments;
    for (i =0; str_cmp(augment_flagss[i].what_you_type, "end_of_list");
i++)
      {
	if (IS_SET(augment,augment_flagss[i].flagname) &&
	    !IS_SET(race_info[ch->pcdata->race].nat_augments,augment_flagss[i].flagname) &&
	    !IS_SET(align_info[ch->pcdata->alignment].nat_augments,augment_flagss[i].flagname))
	  
	  num++;
      }
    return num;
  }

void
show_augment_costs(CHAR_DATA *ch, char *argy)
{
char buf[STD_LENGTH];
int augment = 0;
int augment_num =  get_num_augments(ch);  

for (augment = 0; augment < 12; ++augment)
    {
      if(augment == augment_num)
	sprintf(buf,"-->");
      else
	sprintf(buf, "   ");
      send_to_char(buf, ch);
      sprintf(buf, "Augment:%2d, Remorts:%2d, Level: %2d, Gold:%5d, Wps:%5d, Kps:%5d\n\r", (augment+1), pow.augment_cost[augment][0], pow.augment_cost[augment][1], pow.augment_cost[augment][2], pow.augment_cost[augment][3], pow.augment_cost[augment][4]);
      send_to_char(buf, ch);
      if ((augment == augment_num))
	{
	  sprintf(buf, "-->You are level %d, with %d remorts, %ld warpoints, and %d killpoints.\n\r", LEVEL(ch), ch->pcdata->remort_times, ch->pcdata->warpoints, ch->pcdata->killpoints);
	  send_to_char(buf, ch);
	}
    }
if (augment_num > 11)
  send_to_char("You already have the maximum number of augmentationss!\n\r", ch);
return;
}



void 
do_mobscatter (CHAR_DATA * ch, char *argy)
{
  char vnum[100];
  char how_many[100];
  int vn;
  int objs;
  ROOM_DATA *rd = NULL;
  CHAR_DATA *mob;
  MOB_PROTOTYPE *mid;
  DEFINE_COMMAND ("mobscatter", do_mobscatter, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command scatters (amount) mobs of prototype <vnum> around the world.  Syntax: mobscatter <vnum> (amount).")

    argy = one_argy (argy, vnum);
  argy = one_argy (argy, how_many);
  if (!is_number (vnum))
    {
      send_to_char ("Uhh... syntax is: \"mobscatter <vnum> (amount)\".\n\r", ch);
      return;
    }
  if ((mid = get_mob_index ((vn = atoi (vnum)))) == NULL)
    {
      send_to_char ("That mob vnum doesn't exist...\n\r", ch);
      return;
    }
  if (is_number (how_many))
    {
      objs = atoi (how_many);
    }
  else
    objs = 1;
  while (objs > 0)
    {
      while (rd == NULL)
	{
	  rd = get_room_index (number_range (1000, top_vnum_room));
	}
      if (!rd->area->open)
	{
	  rd = NULL;
	  continue;
	}
      if (rd->sector_type == SECT_WATER_SWIM || rd->sector_type == SECT_WATER_NOSWIM)
	{
	  rd = NULL;
	  continue;
	}
      objs--;
      mob = create_mobile (mid);
      char_to_room (mob, rd);
      rd = NULL;
    }
  return;
}

void 
do_scatter (CHAR_DATA * ch, char *argy)
{
  char vnum[100];
  char how_many[100];
  int vn;
  int objs;
  ROOM_DATA *rd = NULL;
  SINGLE_OBJECT *obj;
  OBJ_PROTOTYPE *oid;
  DEFINE_COMMAND ("objscatter", do_scatter, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command scatters (amount) objects of prototype <vnum> around the world.  Syntax: objscatter <vnum> (amount).")

    argy = one_argy (argy, vnum);
  argy = one_argy (argy, how_many);
  if (!is_number (vnum))
    {
      send_to_char ("Uhh... syntax is: \"scatter <vnum> (amount)\".\n\r", ch);
      return;
    }
  if ((oid = get_obj_index ((vn = atoi (vnum)))) == NULL)
    {
      send_to_char ("That object vnum doesn't exist...\n\r", ch);
      return;
    }
  if (is_number (how_many))
    {
      objs = atoi (how_many);
    }
  else
    objs = 1;
  while (objs > 0)
    {
      while (rd == NULL)
	{
	  rd = get_room_index (number_range (1000, top_vnum_room));
	}
      if (!rd->area->open)
	{
	  rd = NULL;
	  continue;
	}
      if (rd->sector_type == SECT_WATER_SWIM || rd->sector_type == SECT_WATER_NOSWIM)
	{
	  rd = NULL;
	  continue;
	}
      objs--;
      obj = create_object (oid, 1);
      obj_to (obj, rd);
      rd = NULL;
    }
  return;
}

/*Strip a character of all non-worn items, INCLUDING what's inside backpacks */
void 
strip_nonworn (CHAR_DATA * ch)
{
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *obj_next;
  for (obj = ch->carrying; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      if (IS_SET (obj->pIndexData->extra_flags, ITEM_SAVE))
	continue;
      if (number_range (1, 4) == 2)
	{
	  if (obj->contains != NULL)
	    {
	      free_it (obj);
	      send_to_char ("Poof!!  You feel a bit lighter.\n\r", ch);
	      continue;
	    }
	  if (obj->in_obj != NULL)
	    {
	      free_it (obj);
	      continue;
	    }
	  if (obj->wear_loc == -1)
	    {
	      free_it (obj);
	      continue;
	    }
	}
    }
  return;
}




void 
do_alien (CHAR_DATA * ch, char *argy)
{
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *next_obj;
  int i, k, l, m, augmented;
  char buf1[SML_LENGTH];
  char buf2[SML_LENGTH];
  char buf3[SML_LENGTH];
  char buf4[SML_LENGTH];
  char workstr[STD_LENGTH];
  int tmp_stat[NUM_STATS];
  DEFINE_COMMAND ("transform", do_alien, POSITION_STANDING, pow.remort_level, LOG_ALWAYS, "This command is used to transform your character.  See help transform.")

    if (IS_MOB (ch))
    return;
  if (LEVEL (ch) < pow.remort_level)
    {
    send_to_char("You do not meet the remort level requirement.\n\r", ch);
    return;
    }
  if (pow.max_remorts == 0)
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }

  if (pow.max_remorts == 1 && ch->pcdata->remort_times > 0)
    {
      send_to_char ("You have already transformed; you cannot do it again!\n\r", ch);
      return;
    }

  if (ch->pcdata->remort_times >= pow.max_remorts)
    {
      send_to_char ("You have reached the max transformations possible.\n\r", ch);
      return;
    }

  if ((pow.transform_in_room > 1 && 
      (!ch->in_room || ch->in_room->vnum != pow.transform_in_room) &&
      ch->pcdata->race < 11) ||
      (pow.asctransform_in_room > 1 &&
      (!ch->in_room || ch->in_room->vnum != pow.asctransform_in_room) &&
      ch->pcdata->race > 10))
    {
      send_to_char ("You aren't in the proper place in the world to do that!!\n\r", ch);
      return;
    }
  if (pow.add_to_stats && (argy == "" || argy[0] == '\0'))
    {
      show_remort (ch);
      return;
    }
  if (pow.add_to_stats)
    {
      argy = one_argy (argy, buf1);
      argy = one_argy (argy, buf2);
      if (!str_cmp (buf1, "race"))
	  {
		if (ch->pcdata->remort_times > 1)
		{
		   send_to_char ("You can only switch races before you remort for the 3rd time.\n\r", ch);
		   show_remort(ch);
		   return;
	        }
	        else if (!is_number (buf2))
		{
		   send_to_char ("Transform into which race?\n\r", ch);
		   show_remort(ch);
		   return;
		}
		k = atoi(buf2);
		if (k > 11)
		{
		   send_to_char ("You can not transform into an ascended race!\n\r", ch);
		   show_remort(ch);
		   return;
		}
		else if (k < 1)
		{
		   send_to_char ("Which Race?\n\r", ch);
		   show_remort(ch);
		   return;
		}
		else
		{
		   raceswitch (ch, (k - 1));
		   i = 0;
		   for (k = 0;k < NUM_STATS;k ++)
		   {
			if(ch->pcdata->stat[k] > race_info[ch->pcdata->race].limits[k])
		        {  
			   i += ch->pcdata->stat[k] - race_info[ch->pcdata->race].limits[k];  
	                   ch->pcdata->stat[k] = race_info[ch->pcdata->race].limits[k];
		        }
			else if (i > 0)
                        {
			   l = race_info[ch->pcdata->race].limits[k] - ch->pcdata->stat[k];
			   if (i - l > 0)
			   {
			     ch->pcdata->stat[k] += l;
			     i = i - l;
			   }
			   else
			   {
			     ch->pcdata->stat[k] += i;
			     i = 0;
			   }
			}
			}
		   }
	  }
	//If Augments are Only Allowed at Remort check for them here
        argy = one_argy (argy, buf3);
	augmented = -1;
	if (pow.augmentremortonly)
	{
        	for (m = 0; str_cmp(augment_flagss[m].what_you_type,"end_of_list"); m++)
		{
		if (!str_cmp(augment_flagss[m].what_you_type, buf3) ||
		!str_cmp(augment_flagss[m].how_it_appears, buf3))
	  	  {	
	  		augmented = m;
			break;
	  	  }
		}
	}
      if (str_cmp (buf1, "str") && str_cmp (buf1, "int") &&
	  str_cmp (buf1, "wis") && str_cmp (buf1, "con") && 
	  str_cmp (buf1, "dex") && str_cmp (buf1, "race") &&
	  str_cmp (buf1, "none"))
	{
	  show_remort (ch);
	  return;
	}
      if (str_cmp (buf2, "str") && str_cmp (buf2, "int") && 
	  str_cmp (buf2, "wis") && str_cmp (buf2, "con") && 
	  str_cmp (buf2, "dex") && str_cmp(buf1, "race") &&
	  str_cmp (buf2, "none"))
	{
	  show_remort (ch);
	  return;
	}
      if (str_cmp (buf3, "str") && str_cmp (buf3, "int") && 
	  str_cmp(buf3, "wis") && str_cmp (buf3, "con") && 
	  str_cmp (buf3,"dex") && str_cmp(buf1, "race") &&
	  str_cmp (buf3,"none") && augmented < 0)
	{
	  show_remort (ch);
	  return;
	}
    
      argy = one_argy (argy, buf4);
      for (k = 0; k < NUM_STATS; k++)
	tmp_stat[k] = 0;
      for (k = 1; k <= 3; k++)
	{
	  char tb[STD_LENGTH];
	  tb[0] = '\0';
	  if (k == 1)
	    strcpy (tb, buf1);
	  if (k == 2)
	    strcpy (tb, buf2);
	  if (k == 3)
	    strcpy (tb, buf3);
	
	  if (!str_cmp (tb, "str"))
	    tmp_stat[STR]++;
	  if (!str_cmp (tb, "int"))
	    tmp_stat[INT]++;
	  if (!str_cmp (tb, "con"))
	    tmp_stat[CON]++;
	  if (!str_cmp (tb, "wis"))
	    tmp_stat[WIS]++;
	  if (!str_cmp (tb, "dex"))
	    tmp_stat[DEX]++;
	}
      for (i = 0; i < NUM_STATS; i++)
	{
	  if (ch->pcdata->stat[i] + tmp_stat[i] > 36)
	    {
	      send_to_char("You may not push any stats over 36!\n\r", ch);
	      return;
	    }
	}
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (is_member(ch, guild_data[i].mob_guildmaster_bit))
	    ch->pcdata->stat[guild_data[i].stat_modified]--;
	}
      for (i = 0; i < NUM_STATS; i++)
	{
	  if ((ch->pcdata->stat[i] + tmp_stat[i]) > 
	      (race_info[ch->pcdata->race].limits[i]+
         align_info[ch->pcdata->alignment].limits[i]))
	    {
 	      send_to_char ("You may not push any stats over your race and alignment limits!\n\r", ch);
              for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
          	{
	           if (is_member(ch, guild_data[i].mob_guildmaster_bit))
	            ch->pcdata->stat[guild_data[i].stat_modified]++;
	        }

	      return;
	    }
	}
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (is_member(ch, guild_data[i].mob_guildmaster_bit))
	    ch->pcdata->stat[guild_data[i].stat_modified]++;
	}

      for (i = 0; i < NUM_STATS; i++)
	ch->pcdata->stat[i] += tmp_stat[i];
    }

/*Begin transforming the character */
  for (obj = ch->carrying; obj != NULL; obj = next_obj)
    {
      next_obj = obj->next_content;
      if (IS_SET (obj->pIndexData->extra_flags, ITEM_SAVE))
	continue;
      if (pow.vaporize_inv && (obj->wear_loc == -1 || obj->contains != NULL))
	{
	  sprintf (workstr, "%s just vaporized!!\n\r", capitalize (obj->pIndexData->short_descr));
	  send_to_char (workstr, ch);
	  free_it (obj);
	  continue;
	}
      else if (obj->wear_loc != -1)
	unequip_char (ch, obj);
      if (pow.vaporize_all)
	{
	  sprintf (workstr, "%s just vaporized!!\n\r", capitalize (obj->pIndexData->short_descr));
	  send_to_char (workstr, ch);
	  free_it (obj);
	  continue;
	}
    }
  send_to_char ("Your worn items were removed to inventory.\n\r", ch);	  
  if (augmented > -1)
  {
    SET_BIT (ch->pcdata->augments, augment_flagss[augmented].flagname);
    sprintf(workstr, "Congratulations %s, you now have the%s augment!\n\r",
	NAME(ch), augment_flagss[augmented].how_it_appears);
    send_to_char(workstr, ch);
  }
  ch->pcdata->level = 1;
  ch->exp = 1;
  check_fgt(ch);
  free_string(ch->fgt->phunting);
  ch->fgt->phunting = NULL;
  free_string(ch->fgt->hunting);
  ch->fgt->hunting = NULL;
  ch->pcdata->voting_on = 0;
  ch->pcdata->deaths =0;
  REMOVE_BIT(ch->act, PLR_PKED_THIS_MORT);
  ch->pcdata->practice = 10;
  ch->pcdata->learn = 2;
  ch->pcdata->remort_times++; 
  set_initial_hp (ch);
  for (k = 0; k < SKILL_COUNT; k++)
    {
      ch->pcdata->learned[k] = -100;
    }
  natural_mana (ch);
  if (pow.beams_to_room > 0)
    {
      int to_room;
      if (pow.beams_to_room == 1)
	{
	  to_room = (ALIGN(ch) + 100);
	}
      else
	to_room = pow.beams_to_room;
      char_from_room (ch);
      char_to_room (ch, get_room_index (to_room));
      do_look (ch, "auto");
    }
  send_to_char ("\x1B[1;37mTransformation complete!!!\x1B[37;0m\n\r", ch);
  if (pow.max_remorts > 1)
    {
      sprintf (workstr, "This is remort number \x1B[1;37m%d\x1B[37;0m for your character!\n\r", ch->pcdata->remort_times);
      send_to_char (workstr, ch);
      if (ch->pcdata->remort_times == 2)
	send_to_char ("You now never will get hungry!!\n\r", ch);
      if (ch->pcdata->remort_times == 5)
	send_to_char ("You now never will get thirsty!!\n\r", ch);
    }

  max_remort_times[ch->pcdata->alignment] = 
   UMAX(max_remort_times[ch->pcdata->alignment], ch->pcdata->remort_times);
  
  fix_char(ch);
  do_save (ch, "xx2xx11");
  return;
}



void 
remove_all_objs (CHAR_DATA * ch)
{
  SINGLE_OBJECT *obj;
  SINGLE_OBJECT *nxt;
  for (obj = ch->carrying; obj != NULL; obj = nxt)
    {
      nxt = obj->next_content;
      if (obj->wear_loc != -1)
	unequip_char (ch, obj);
    }
  send_to_char ("Your worn items were removed to inventory.\n\r", ch);
  return;
}


void 
do_warcry (CHAR_DATA * ch, char *argy)
{
  AFFECT_DATA aff;
  SPELL_DATA *s;
  int dam;
  DEFINE_COMMAND ("warcry", do_warcry, POSITION_RESTING, 0, LOG_NORMAL, "This skill increases your damroll for a bit.")
    bzero (&aff, sizeof (aff));
  if ((s = skill_lookup (NULL, gsn_warcry)) == NULL)
    return;
  if (ch->position != POSITION_STANDING)
    {
      send_to_char("You must be standing to let out a warcry!\n\r", ch);
      return;
    }
  if (IS_PLAYER(ch) && ch->pcdata->learned[gsn_warcry] < 50)
    {
     send_to_char("Huh?\n\r", ch);
     return;
     }	
  if (IS_PLAYER(ch) && (number_percent() > ch->pcdata->learned[gsn_warcry]))
    {
      send_to_char("You try to let out a warcry, but fail.\n\r", ch);
      return;
    }
  if (ch->move < s->mana)
    {
      send_to_char ("You are too exhausted.\n\r", ch);
      return;
    }
  aff.type = gsn_warcry;
  aff.duration = number_range ((LEVEL(ch)/4),(LEVEL(ch)/2) );
  aff.location = APPLY_DAMROLL;
  dam = number_range(3,4);
  if (is_member(ch, GUILD_WARRIOR)) dam += number_range(1,2);
  if (is_member(ch, GUILD_BATTLEMASTER)) dam += number_range(1,4);
  
  aff.modifier = dam;
  aff.bitvector = 0;
  SUBMOVE(ch, s->mana);
  renew_affect (ch, &aff);
  send_to_char("You let out a gigantic warcry!\n\r", ch);
  act("$n lets out a gigantic warcry!", ch, NULL, NULL, TO_ROOM);
  return;
}
