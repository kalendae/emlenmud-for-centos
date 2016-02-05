#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "emlen.h"


bool 
is_member (CHAR_DATA * ch, int guildflag)
{
  if (IS_MOB (ch)) 
    return TRUE; /* JRAJRA - Changed this from FALSE mobs are now 
	always guild members when we are talking about spellcasting. */
  if (IS_SET (ch->pcdata->guilds, guildflag))
    return TRUE;
  else
    return FALSE;
}

void 
do_guild (CHAR_DATA * ch, char *argy)
{
  
  int i;
  CHAR_DATA *mob;
  char buf[STD_LENGTH];
  char arg1[SML_LENGTH];
  DEFINE_COMMAND ("guild", do_guild, POSITION_STANDING, 0, LOG_NORMAL, "This command allows you to perform guild options at a guild house.")
    if (IS_MOB (ch))
      return;
  argy = one_argy(argy, arg1);
  if (!str_cmp (arg1, "info") || !str_cmp(arg1, "cost") || !str_cmp(arg1,
								    "costs"))
    {
      show_guild_costs(ch, "");
      return;
    }		

  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_MOB (mob) && (
	   IS_SET (mob->pIndexData->act3, ACT3_TINKER)
	   || IS_SET (mob->pIndexData->act3, ACT3_WARRIOR)
	   || IS_SET (mob->pIndexData->act3, ACT3_HEALER)
	   || IS_SET (mob->pIndexData->act3, ACT3_WIZARD)
	   || IS_SET (mob->pIndexData->act3, ACT3_THIEFG)
	   || IS_SET (mob->pIndexData->act3, ACT3_RANGER)
	   || IS_SET (mob->pIndexData->act3, ACT3_ROGUE)
	   || IS_SET (mob->pIndexData->act3, ACT3_MYSTIC)
	   || IS_SET (mob->pIndexData->act3, ACT3_CONJURER)
	   || IS_SET (mob->pIndexData->act3, ACT3_BATTLEMASTER)
	   || IS_SET (mob->pIndexData->act3, ACT3_NECROMANCER)
	   || IS_SET (mob->pIndexData->act3, ACT3_MONK)
	   )
	  )
	break;
    }
  if (mob == NULL)
    {
      send_to_char ("There is no guildmaster present here!\n\r", ch);
      return;
    }
  if (arg1[0] == '\0')
    {
      send_to_char ("\n\rOptions:\n\r---> Guild leave\n\r---> Guild info\n\r---> Guild join\n\r---> Guild status\n\r", ch);
      return;
    }
  
  /*end of GUILD INFO */
  if (!str_cmp (arg1, "status"))
    {
      if (!IS_SET (ch->pcdata->guilds, (mob->pIndexData->act3)))
	{
	  send_to_char ("You are not a member of our guild!\n\r", ch);
	  return;
	}
      send_to_char ("You are an highly respected member of the guild.\n\r", ch);
      return;
    }				/*end of GUILD STATUS */
  
  
  if (!str_cmp (arg1, "leave"))
    {
      
      if (!is_member (ch, (mob->pIndexData->act3)))
	{
	  send_to_char ("You aren't even a member!!\n\r", ch);
	  return;
	}
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (IS_SET(mob->pIndexData->act3, guild_data[i].mob_guildmaster_bit))
	    {
	      ch->pcdata->stat[guild_data[i].stat_modified]--;
	      REMOVE_BIT(ch->pcdata->guilds, guild_data[i].player_guild_bit);
	      send_to_char("You are no longer a member of the guild.", ch);
	      player_preset(ch, "Zlughlkheyn");
              do_remove(ch, "all");
	      break;
	    }
	}
    }
  /*end of GUILD LEAVE */
  if (!str_cmp(arg1, "replace"))
    {
      char arg2[STD_LENGTH];
      char arg3[STD_LENGTH];
      int oldnum = -1;
      int newnum = -1;
      
      argy = one_argy(argy, arg2);
      argy = one_argy(argy, arg3);
      if (arg2[0] == '\0' || arg3[0] == '\0')
	{
	  send_to_char("Syntax: Guild Replace <old_guild> <new_guild>\n\r", ch);
	  return;
	}
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (!str_cmp(guild_data[i].what_you_type, arg2))
	    {
	      oldnum = guild_data[i].mob_guildmaster_bit;
	      break;
	    }
	}
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (!str_cmp(guild_data[i].what_you_type, arg3))
	    {
	      newnum = guild_data[i].mob_guildmaster_bit;
	      break;
	    }
	}
      if (oldnum == -1 || newnum == -1)
	{
	  send_to_char("You need to check those names again. I don't know which guilds you are talking about.\n\r", ch);
	  return;
	}
      if (!IS_SET(ch->pcdata->guilds, oldnum))
	{
	  send_to_char("You don't have that guild so how do you expect to replace it?\n\r", ch);
	  return;
	}
      if (newnum == oldnum)
	{
	  send_to_char("You are not changing anything!! Get a clue.\n\r", ch);
	  return;
	}
      if (!IS_SET(mob->pIndexData->act3, newnum))
	{
	  send_to_char("This guildmaster cannot add you. You must find the proper guildmaster for the guild you wish to join.\n\r", ch);
	  return;
	}
      if (is_member(ch, newnum))
	{
	  send_to_char("You are already a member of that guild!\n\r", ch);
	  return; 
	}
      if (ch->pcdata->bank < 60000)
	{
	  send_to_char("You need 60000 coins in the bank for this to be done.\n\r", ch);
	  return;
	}
      if(ch->pcdata->warpoints < 100)
	{
	  send_to_char("You need 100 warpoints for this to be done.\n\r", ch);
	  return;
	}
      ch->pcdata->bank -= 60000;
      ch->pcdata->warpoints -= 100;
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (IS_SET(oldnum, guild_data[i].mob_guildmaster_bit))
	    {
	      ch->pcdata->stat[guild_data[i].stat_modified]--;
	      REMOVE_BIT(ch->pcdata->guilds, guild_data[i].player_guild_bit);
	      break;
	    }
	}
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (IS_SET(newnum, guild_data[i].mob_guildmaster_bit))
	    {
	      ch->pcdata->stat[guild_data[i].stat_modified]++;
	      SET_BIT(ch->pcdata->guilds, guild_data[i].player_guild_bit);
	      break;
	    }
	}
      sprintf(buf, "Congratulations! You are now a member of the %s!\n\r", guild_data[i].what_you_see);
      send_to_char(buf, ch);
      sprintf (buf, "Welcome to our guild, %s!", NAME (ch));
      do_say (mob, buf); 
      player_preset(ch, "Zlughlkheyn");
      fix_char(ch);
      send_to_char("Ok, the guilds have been switched.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "join"))
    {
      int num = get_num_guilds(ch);
      int remorts = pow.guild_info[num][0];
      int lvl = pow.guild_info[num][1];
      int cst = (pow.guild_info[num][2] * 100);
      int wps = pow.guild_info[num][3];
      int kps = pow.guild_info[num][4];
      bool can_join = TRUE;
      if (IS_SET (ch->pcdata->guilds, mob->pIndexData->act3 ))
	{
	  send_to_char ("You are already a member of our guild!\n\r", ch);
	  return;
	}
      if (ch->pcdata->remort_times < remorts)
	{
	  sprintf (buf, "You need to have at least %d remorts to join this guild!\n\r", remorts);
	  send_to_char (buf, ch);
	  can_join = FALSE;
	}
      // Commented out because i replaced it with the code below.
        /* if (LEVEL (ch) < lvl)
	{
	  sprintf (buf, "You need to be level %d to join the guild!\n\r", lvl);
	  send_to_char (buf, ch);
	  can_join = FALSE;
	}*/
      //Added so if the player has a remort that they can join even if they don't meet the level req.
      //Sabelis 1-18-2006
      if ((LEVEL (ch) >lvl) || (ch->pcdata->remort_times>0))
          {
          can_join = TRUE;
          }
      else if (LEVEL (ch) < lvl)
        {
          sprintf (buf, "You need to be level %d to join the guild!\n\r", lvl);
          send_to_char (buf, ch);
          can_join = FALSE;
        }

      if (tally_coins (ch) < cst)
	{
	  sprintf (buf, "We need a payment of %d coins before we can let you join.\n\r", cst);
	  send_to_char (buf, ch);
	  can_join = FALSE;
	}
      if (ch->pcdata->warpoints < wps)
	{
	  sprintf (buf, "You need to have at least %d warpoints to join this guild!\n\r", wps);
	  send_to_char (buf, ch);
	  can_join = FALSE;
	}
      if (ch->pcdata->killpoints < kps)
	{
	  sprintf (buf, "You need to have at least %d killpoints to join this guild!\n\r", kps);
	  send_to_char (buf, ch);
	  can_join = FALSE;
	}
      if (!can_join)
	return;
      
      ch->pcdata->warpoints -= wps;
      ch->pcdata->killpoints -= kps;
      sub_coins (cst, ch);
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (IS_SET(mob->pIndexData->act3, guild_data[i].mob_guildmaster_bit))
	    {
	      ch->pcdata->stat[guild_data[i].stat_modified]++;
	      SET_BIT(ch->pcdata->guilds, guild_data[i].player_guild_bit);
	      break;
	    }
	}
      sprintf(buf, "Congratulations! You are now a member of the %s!\n\r", guild_data[i].what_you_see);
      send_to_char(buf, ch);
      sprintf (buf, "Welcome to our guild, %s!", NAME (ch));
      do_say (mob, buf);
    }
  return;
}

void
show_guild_costs(CHAR_DATA *ch, char *argy)
{
char buf[500];
int guild = 0;
int guild_num = get_num_guilds(ch);
send_to_char("These are the requirements for each guild, and where you are at the moment.\n\r", ch);
send_to_char("An arrow is next to the next guild you will get.\n\n\r", ch);
for (guild = 0; guild < 8; ++guild)
    {
      if(guild == guild_num)
	sprintf(buf,"-->");
      else
	sprintf(buf, "   ");
      send_to_char(buf, ch);
      sprintf(buf, "Guild: %1d, Remorts:%2d, Level: %2d, Gold:%5d, Wps:%5d, Kps %5d\n\r", (guild+1), pow.guild_info[guild][0], pow.guild_info[guild][1], pow.guild_info[guild][2], pow.guild_info[guild][3], pow.guild_info[guild][4]);
      send_to_char(buf, ch);
      if ((guild == guild_num))
	{
	  sprintf(buf, "-->You are level %d, with %d remorts, %ld warpoints, and %d killpoints.\n\r", LEVEL(ch), ch->pcdata->remort_times, ch->pcdata->warpoints, ch->pcdata->killpoints);
	  send_to_char(buf, ch);
	}
    }

}

int
get_num_guilds(CHAR_DATA *ch)
{
int num = 0;
if (is_member(ch, GUILD_HEALER))
  num++;
if (is_member(ch, GUILD_THIEFG))
  num++;
if (is_member(ch, GUILD_WARRIOR))
  num++;
if (is_member(ch, GUILD_ROGUE))
  num++;
if (is_member(ch, GUILD_WIZARD))
  num++;
if (is_member(ch, GUILD_TINKER))
  num++;
if (is_member(ch, GUILD_RANGER))
  num++;
if (is_member(ch, GUILD_CONJURER))
  num++;
if (is_member(ch, GUILD_MYSTIC))
  num++;
if (is_member(ch, GUILD_BATTLEMASTER))
  num++;
if (is_member(ch, GUILD_NECROMANCER))
  num++;
if (is_member(ch, GUILD_MONK))
  num++;

return num;
}
