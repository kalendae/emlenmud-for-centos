/*Offline Player editor */
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
char *itoa (int);

/* True is character is fine with payment, false if not. */


char *
augmentss (int augment)
{
   int num;
  static char tttb[256];
  bool prev = FALSE;
  tttb[0] = '\0';
 
  for (num = 0 ; str_cmp (augment_flagss[num].what_you_type, "end_of_list"); num++)
   {
     if (IS_SET(augment, augment_flagss[num].flagname))
       {
	 if (prev)
	   strcat (tttb, ",");
	 strcat (tttb, augment_flagss[num].how_it_appears);
	 prev = TRUE;
       }
     
   }
  if (tttb[0] == '\0')
    strcat (tttb, "None");
  return tttb;
}

char *
itoa (int n)  
{
  static char t[50];
  sprintf (t, "%d", n);
  return t;
}




char *
natural_abilities (int affect)
{
   int num;
  static char tttb[256];
  bool prev = FALSE;
  tttb[0] = '\0';
 
  for (num = 0 ; str_cmp (affect_flagss[num].what_you_type, "end_of_list"); num++)
   {
     if (IS_SET(affect, affect_flagss[num].flagname))
       {
	 if (prev)
	   strcat (tttb, ",");
	 strcat (tttb, affect_flagss[num].how_it_appears);
	 prev = TRUE;
       }
     
   }
  if (tttb[0] == '\0')
    strcat (tttb, "None");
  return tttb;
}

char *
natural_abilities2 (int affect)
{
   int num;
  static char tttb[256];
  bool prev = FALSE;
  tttb[0] = '\0';
 
  for (num = 0 ; str_cmp(affect_flags_2[num].what_you_type,"end_of_list"); num++)
   {
     if (IS_SET(affect, affect_flags_2[num].flagname))
       {
	 if (prev)
	   strcat (tttb, ",");
	 strcat (tttb, affect_flags_2[num].how_it_appears);
	 prev = TRUE;
       }
     
   }
  if (tttb[0] == '\0')
    strcat (tttb, "None");
  return tttb;
}



void 
show_player (CHAR_DATA * god, CHAR_DATA * victim)
{
  char buf[1024];
  sprintf (buf, "[\x1B[1;35m%d\x1B[37;0m] Name: \x1B[1;37m%s\x1B[37;0m  (\x1B[1;37m%s #%d\x1B[37;0m)  Gender: %s  Email: %s\n\r",
	   LEVEL (victim), NAME (victim), race_info[victim->pcdata->race].name, victim->pcdata->race,
	   (victim->pcdata->sex == SEX_MALE ? "M" : victim->pcdata->sex == SEX_FEMALE ? "F" : "N"),
	   victim->pcdata->email);
  send_to_char (buf, god);
  sprintf (buf, "Align: %d  Remorts: %d  NatAC: %d  Deaths: %d  Warpts: %ld Height: %d\n\r",
	   victim->pcdata->alignment, victim->pcdata->remort_times,
	   victim->pcdata->nat_armor, victim->pcdata->deaths,
	   victim->pcdata->warpoints, victim->height);
  send_to_char (buf, god);
  sprintf (buf, "Clan: %s  Clan2: %s  Room #%d  BankMoney: %ld  Pracs: %d  Learns: %d\n\r",
	(clan_number (victim) < 0) ? "None" : (itoa (clan_number (victim))),
    (clan_number_2 (victim) < 0) ? "None" : (itoa (clan_number_2 (victim))),
	   (victim->in_room ? victim->in_room->vnum : 0),
     victim->pcdata->bank, victim->pcdata->practice, victim->pcdata->learn);
  send_to_char (buf, god);
  sprintf (buf, "Total kills: %d  Kill points: %d  HP: \x1B[1;37m%ld/%ld\x1B[37;0m  MV: \x1B[1;37m%d/%d\x1B[37;0m\n\r",
	   victim->pcdata->totalkills, victim->pcdata->killpoints,
	   victim->hit, victim->max_hit, victim->move, victim->max_move);
  send_to_char (buf, god);
  sprintf (buf, "Hitroll: %d  Damroll: %d  Thirst: %d/50  Full: %d/50  Drunk: %d/0\n\r",
	   GET_HITROLL (victim), GET_DAMROLL (victim),
	   victim->pcdata->condition[COND_THIRST], victim->pcdata->condition[COND_FULL],
	   victim->pcdata->condition[COND_DRUNK]);
  send_to_char (buf, god);
  sprintf (buf, "Saving_throw: %d  Items Car: %d  Weight Car: %d Stones, %d Pebbles Age: %d  Dntn: %d\n\r",
	   victim->pcdata->saving_throw, victim->pcdata->carry_number,
   (victim->pcdata->carry_weight/WGT_MULT),(victim->pcdata->carry_weight % WGT_MULT), get_age (victim), victim->pcdata->donated);
  send_to_char (buf, god);
  sprintf (buf, "\x1B[1;34m----- Stats and Extras ------------ (%s) ----------\x1B[37;0m\n\r",
	 victim->desc ? "Player currently online" : "Player not logged in");
  send_to_char (buf, god);
  sprintf (buf, "Str: %2d/%2d  Con: %2d/%2d  Dex: %2d/%2d  Int: %2d/%2d  Wis: %2d/%2d\n\r",
	   get_curr_str (victim), victim->pcdata->stat[0],
	   get_curr_con (victim), victim->pcdata->stat[4],
	   get_curr_dex (victim), victim->pcdata->stat[3],
	   get_curr_int (victim), victim->pcdata->stat[1],
	   get_curr_wis (victim), victim->pcdata->stat[2]);
  send_to_char (buf, god);
  sprintf (buf, "Use Smd, Cmd, Dmd, Imd, and Wmd to set stat modifiers.   Exp: %ld\n\r",
	   victim->exp);
  send_to_char (buf, god);
  if (victim->pcdata->cool_skills != 0)
    {
      bool prev;
      prev = FALSE;
      send_to_char ("Cool Skills: ", god);
      send_to_char (".\n\r", god);
    }
  if (victim->pcdata->nat_abilities != 0)
    {
      bool prev;
      prev = FALSE;
      send_to_char ("Natural Abilities: ", god);
      send_to_char (natural_abilities (victim->pcdata->nat_abilities), god);
      send_to_char (".\n\r", god);
      send_to_char ("Natural Abilities2: ", god);
      send_to_char (natural_abilities2 (victim->pcdata->nat_abilities2), god);
      send_to_char (".\n\r", god);
      send_to_char ("Augments: ", god);
      send_to_char (augmentss (victim->pcdata->augments), god);
      send_to_char (".\n\r", god);
    }
  show_guilds (god, victim->pcdata->guilds);
  send_to_char ("Type EXTRA to see more data.  Other bits: ", god);
  if (IS_SET (victim->pcdata->act2, PLR_SILENCE))
    {
      send_to_char ("Silenced ", god);
    }
  if (IS_SET (victim->act, PLR_FREEZE))
    {
      send_to_char ("Frozen ", god);
    }
  if (IS_SET(victim->act, ACT_CRASHER))
    {
       send_to_char ("Crasher ", god);
    } 
  if (IS_SET (victim->act, ACT_UNVAL))
    {
      send_to_char ("UNVALIDATED ", god);
    }
  send_to_char ("\n\r", god);
  return;
}

void 
show_extras (CHAR_DATA * god, CHAR_DATA * victim)
{
  int i = 0;
  int tt;
  SINGLE_OBJECT *prev;
  SINGLE_OBJECT *obj;
  SPELL_DATA *s;
  hugebuf_o[0] = '\0';
  strcpy (hugebuf_o, "\x1B[1;37mSkills...\x1B[37;0m");
  for (tt = 0; tt < SKILL_COUNT; tt++)
    {
      if (victim->pcdata->learned[tt] > 0)
	{
	  if ((s = skill_lookup (NULL, tt)) != NULL)
	    {
	      if (i % 2 == 0)
		strcat (hugebuf_o, "\n\r");
	      i++;
	      sprintf (hugebuf_o + strlen (hugebuf_o), " [%17s #%3d %3d%%]   ", s->spell_name, tt, victim->pcdata->learned[tt]);
	    }
	}
    }
  strcat (hugebuf_o, "\n\r");
  strcat (hugebuf_o, "\x1B[1;37m\n\rInventory...\x1B[37;0m");
  prev = FALSE;
  i = 0;
  for (obj = victim->carrying; obj != NULL; obj = obj->next_content)
    {
      if (i % 11 == 0)
	strcat (hugebuf_o, "\n\r");
      i++;
      if (obj->wear_loc == -1)
	sprintf (hugebuf_o + strlen (hugebuf_o), "%-6d ", obj->pIndexData->vnum);
      else
	sprintf (hugebuf_o + strlen (hugebuf_o), "*%-5d ", obj->pIndexData->vnum);
    }
  strcat (hugebuf_o, "\n\r");
  i = 0;
  strcat (hugebuf_o, "\x1B[1;37m\n\rStorage...\x1B[37;0m");
  prev = FALSE;
  for (tt = 0; tt < MAXST; tt++)
    {
      if (victim->pcdata->storage[tt] == NULL)
	continue;
      if (i % 11 == 0)
	strcat (hugebuf_o, "\n\r");
      i++;
      sprintf (hugebuf_o + strlen (hugebuf_o), "%6d ", victim->pcdata->storage[tt]->pIndexData->vnum);
    }
  strcat (hugebuf_o, "\n\r");
  page_to_char (hugebuf_o, god);
  return;
}

void 
pedit (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  char buf[1024];
  char arg1[512];
  char arg2[512];
  char arg3[512];
  char arg4[512];
  static char store_arg[SML_LENGTH];
  int value;
  strcpy (store_arg, argy);
  victim = (CHAR_DATA *) ch->desc->pEdit;
  if (!victim)
    {
      ch->desc->connected = CON_PLAYING;
      ch->desc->pEdit = NULL;
      return;
    }
  arg1[0] = '\0';
  arg2[0] = '\0';
  arg3[0] = '\0';
  arg4[0] = '\0';
  value = -1;
  argy = one_argy (argy, arg1);
  if (arg1[0] == '\0' || !str_cmp (arg1, "show"))
    {
      show_player (ch, victim);
      return;
    }

  if (!str_prefix ("extra", arg1))
    {
      show_extras (ch, victim);
      return;
    }
  if (argy && argy != "")
    argy = one_argy (argy, arg2);
  if (argy && argy != "")
    argy = one_argy (argy, arg3);
  if (argy && argy != "")
    argy = one_argy (argy, arg4);
  if (arg2[0] == '-')
    value = atoi (arg2);
  else
    value = is_number (arg2) ? atoi (arg2) : -1;
  
  if (!str_prefix ("valid", arg1))
    {
      if (IS_SET (victim->act, ACT_UNVAL))
	REMOVE_BIT (victim->act, ACT_UNVAL);
      else
	SET_BIT (victim->act, ACT_UNVAL);
      send_to_char ("Validated flag toggled.\n\r", ch);
      if(victim->pcdata->level == 0)
	victim->pcdata->level = 1;
      return;
    }
  if (!str_prefix ("lamer", arg1))
    {
      if (IS_SET (victim->act, PLR_LAMER))
	REMOVE_BIT (victim->act, PLR_LAMER);
      else
	SET_BIT (victim->act, PLR_LAMER);
      send_to_char ("Lamer flag toggled.\n\r", ch);
      return;
    }
  if (!str_prefix ("frozen", arg1) || !str_prefix("freeze", arg1))
    {
      if (IS_SET (victim->act, PLR_FREEZE))
	REMOVE_BIT (victim->act, PLR_FREEZE);
      else
	SET_BIT (victim->act, PLR_FREEZE);
      send_to_char ("Freeze flag toggled.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "height") && value >= 0)
    {
      if (value < 1 || value > 32000)
	{
	  send_to_char ("Valid range for height is 1 - 32000.\n\r", ch);
	  return;
	}
      victim->height = value;
      send_to_char ("Victim height set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "name") && arg2[0] != '\0')
    {
      sprintf (buf, "%s%s", PLAYER_DIR, NAME (victim));
      unlink (buf);
      free_string (victim->pcdata->name);
      victim->pcdata->name = str_dup (capitalize (arg2));
      save_char_obj (victim);
      send_to_char ("Name changed.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "race") && value >= 0 && value < RACE_COUNT)
    {
      raceswitch(victim, value);
      send_to_char ("Race changed.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "sex") && value >= 0 && value < 3)
    {
      victim->pcdata->sex = value;
      send_to_char ("Victim's sex set.\n\r", ch);
      return;
    }
  if (!str_prefix ("remort", arg1) && value >= 0)
    {
      victim->pcdata->remort_times = value;
      send_to_char ("Victim remort times set.\n\r", ch);
      return;
    }
  if (!str_cmp(arg1, "password") && arg2[0] != '\0' && strlen(arg2) > 4)
    {
      char buff[100];
      char *pwdnew;
      char *p;
      free_string(pwdnew);
      pwdnew = arg2;
      for (p = pwdnew; *p != '\0'; p++)
	{
	  if (*p == '~')
	    {
	      send_to_char("The new password is not acceptable, try again.\n\r", ch);
	      return;
	    }
	}
      free_string(victim->pcdata->pwd);
      victim->pcdata->pwd = str_dup(pwdnew);
      if (victim->pcdata->rpwd)
        free_string(victim->pcdata->rpwd);
      victim->pcdata->rpwd = str_dup(arg2);
      sprintf(buff, "Ok password reset to crypt of %s \n\r", arg2);
      send_to_char(buff, ch);
      return;
    }
  if (!str_cmp ("email", arg1) && arg2[0] != '\0')
    {
      strcpy (victim->pcdata->email, arg2);
      send_to_char ("Victim email set.\n\r", ch);
      return;
    }
  if (!str_prefix ("nat", arg1) && value != -1)
    {
      victim->pcdata->nat_armor = value;
      send_to_char ("Victim natural armor set.\n\r", ch);
      return;
    }
  if (!str_prefix ("death", arg1) && value >= 0)
    {
      victim->pcdata->deaths = value;
      send_to_char ("Victim deaths set.\n\r", ch);
      return;
    }
  if (!str_prefix ("war", arg1) && value >= 0)
    {
      victim->pcdata->warpoints = value;
      send_to_char ("Victim warpoints set.\n\r", ch);
      return;
    }
  if (!str_cmp ("room", arg1) && value > 1)
    {
      CHAR_DATA *ii;
      ROOM_DATA *rd;
      for (ii = char_list; ii != NULL; ii = ii->next)
	{
	  if (ii == victim)
	    {
	      send_to_char ("You can't change a player's room while they are online!\n\r", ch);
	      return;
	    }
	}
      if ((rd = get_room_index (value)) == NULL)
	{
	  send_to_char ("That room doesn't exist.\n\r", ch);
	  return;
	}
      victim->in_room = rd;
      send_to_char ("Victim in_room changed.\n\r", ch);
      return;
    }
  if (!str_cmp ("bank", arg1) && value >= 0)
    {
      victim->pcdata->bank = value;
      send_to_char ("Victim bank amount set.\n\r", ch);
      return;
    }
  if (!str_prefix ("prac", arg1) && value >= 0)
    {
      victim->pcdata->practice = value;
      send_to_char ("Victim practices set.\n\r", ch);
      return;
    }
  if (!str_prefix ("learn", arg1) && value >= 0)
    {
      victim->pcdata->learn = value;
      send_to_char ("Victim learns set.\n\r", ch);
      return;
    }
  if ((!str_cmp ("tkills", arg1) || !str_cmp ("kills", arg1)) && value >= 0)
    {
      victim->pcdata->totalkills = value;
      send_to_char ("Total kills set.\n\r", ch);
      return;
    }
  if ((!str_cmp ("kpts", arg1) || !str_prefix ("killpo", arg1)) && value >= 0)
    {
      victim->pcdata->killpoints = value;
      send_to_char ("Killpoints set.\n\r", ch);
      return;
    }
  if (!str_cmp ("ac", arg1))
    {
      if (str_cmp ("yes", arg2))
	{
	  send_to_char ("Make sure the chracter is NAKED before you do this, and that\n\r", ch);
	  send_to_char ("NO AFFECTS are present on him/her (use noaffect). Then type ac yes\n\r", ch);
	  return;
	}
      victim->armor = 100;
      send_to_char ("Victim's armor set to 100.\n\r", ch);
      return;
    }
  if (!str_cmp ("hpnow", arg1) && value >= 0)
    {
      victim->hit = value;
      send_to_char ("Hit points now set.\n\r", ch);
      return;
    }
  if (!str_cmp ("hp", arg1) && value >= 0)
    {
      victim->max_hit = value;
      send_to_char ("Hit points max set.\n\r", ch);
      return;
    }
  if (!str_cmp ("mvnow", arg1) && value >= 0)
    {
      victim->move = value;
      send_to_char ("Move points now set.\n\r", ch);
      return;
    }
  if (!str_cmp ("mv", arg1) && value >= 0)
    {
      victim->max_move = value;
      send_to_char ("Move points max set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "wis") && value >= 0)
    {
      if (value < 3 || value > 36)
	{
	  send_to_char ("Wisdom range is 3 to 32.\n\r", ch);
	  return;
	}
      send_to_char ("Wisdom set.\n\r", ch);
      victim->pcdata->stat[2] = value;
      return;
    }
  if (!str_cmp (arg1, "dex") && value >= 0)
    {
      if (value < 3 || value > 36)
	{
	  send_to_char ("Dexterity range is 3 to 32.\n\r", ch);
	  return;
	}
      send_to_char ("Dexterity set.\n\r", ch);
      victim->pcdata->stat[3] = value;
      return;
    }
  if (!str_cmp (arg1, "con") && value >= 0)
    {
      if (value < 3 || value > 36)
	{
	  send_to_char ("Constitution range is 3 to 32.\n\r", ch);
	  return;
	}
      send_to_char ("Constitution set.\n\r", ch);
      victim->pcdata->stat[4] = value;
      return;
    }
  if (!str_cmp (arg1, "int") && value >= 0)
    {
      if (value < 3 || value > 36)
	{
	  send_to_char ("Intelligence range is 3 to 32.\n\r", ch);
	  return;
	}
      send_to_char ("Intelligence set.\n\r", ch);
      victim->pcdata->stat[1] = value;
      return;
    }
  if (!str_cmp (arg1, "str") && value >= 0)
    {
      if (value < 3 || value > 36)
	{
	  send_to_char ("Strength range is 3 to 32.\n\r", ch);
	  return;
	}
      send_to_char ("Strength set.\n\r", ch);
      victim->pcdata->stat[0] = value;
      return;
    }
  if (!str_cmp (arg1, "wmd"))
    {
      send_to_char ("Wisdom mod set.\n\r", ch);
      victim->pcdata->stat_mod[2] = value;
      return;
    }
  if (!str_cmp (arg1, "dmd"))
    {
      send_to_char ("Dexterity mod set.\n\r", ch);
      victim->pcdata->stat_mod[3] = value;
      return;
    }
  if (!str_cmp (arg1, "imd"))
    {
      send_to_char ("Intelligence mod set.\n\r", ch);
      victim->pcdata->stat_mod[1] = value;
      return;
    }
  if (!str_cmp (arg1, "cmd"))
    {
      send_to_char ("Constitution mod set.\n\r", ch);
      victim->pcdata->stat_mod[4] = value;
      return;
    }
  if (!str_cmp (arg1, "smd"))
    {
      send_to_char ("Strength mod set.\n\r", ch);
      victim->pcdata->stat_mod[0] = value;
      return;
    }
  if (!str_cmp (arg1, "Sec") && value > 0 && value < 10)
    {
      send_to_char ("Security set.\n\r", ch);
      victim->pcdata->security = value;
      return;
    }
  if (!str_cmp (arg1, "hitrl"))
    {
      send_to_char ("Hitroll set.\n\r", ch);
      victim->hitroll = value;
      return;
    }
  if (!str_cmp (arg1, "damrl"))
    {
      send_to_char ("Damroll set.\n\r", ch);
      victim->damroll = value;
      return;
    }
  if (!str_cmp (arg1, "thirst"))
    {
      send_to_char ("Thirst set.\n\r", ch);
      victim->pcdata->condition[COND_THIRST] = value;
      return;
    }
  if (!str_cmp (arg1, "full"))
    {
      send_to_char ("Full set.\n\r", ch);
      victim->pcdata->condition[COND_FULL] = value;
      return;
    }
  if (!str_cmp (arg1, "drunk"))
    {
      send_to_char ("Drunk set.\n\r", ch);
      victim->pcdata->condition[COND_DRUNK] = value;
      return;
    }
  if (!str_prefix ("sav", arg1))
    {
      send_to_char ("Saving throw set.\n\r", ch);
      victim->pcdata->saving_throw = value;
      return;
    }
  if (!str_cmp (arg1, "cool") && arg2[0] != '\0')
    {
      return;
    }
  if (!str_cmp (arg1, "guild") && arg2[0] != '\0')
    {
      int i;
      for (i = 0; str_cmp(guild_data[i].what_you_type, "end_of_list"); i++)
	{
	  if (!str_cmp(guild_data[i].what_you_type, arg2))
	    {
	      char buf[100];
	      sprintf(buf, guild_data[i].what_you_see);
	      if (IS_SET(victim->pcdata->guilds, guild_data[i].mob_guildmaster_bit))
		strcat(buf, " Removed.\n\r");
	      else
		strcat(buf, " Added.\n\r");
	      send_to_char(buf, ch);
	      victim->pcdata->guilds ^= guild_data[i].mob_guildmaster_bit;
	      return;
	    }
	}


      send_to_char ("Unknown guild.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "level"))
    {
      if (value < 1 || value > 99)
	return;
      victim->exp = (FIND_EXP ((value - 1), 1));
      send_to_char ("Level set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "align") || !str_cmp (arg1, "alignment"))
    {
      if (value < 0 || value >= NUM_ALIGN)
	return;
      alignswitch(victim, value);
      send_to_char ("Alignment set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "affect"))
    {
      char bufff[200];
      bool isoff = FALSE;
      int num = 0;
      bool found = FALSE;
      int affect = victim->pcdata->nat_abilities;
      for (num = 0; str_cmp(affect_flagss[num].what_you_type, "end_of_list") && (!found) ; num++)
	{
	  if (!str_cmp(arg2, affect_flagss[num].what_you_type ))
	    {
	      found = TRUE;
	      isoff =((IS_SET(affect,affect_flagss[num].flagname))? FALSE : TRUE);
	      sprintf(bufff, " %s bit toggled %s\n\r",affect_flagss[num].what_you_type ,(isoff ? "On" : "Off") );
	      TOGGLE_BIT(victim->pcdata->nat_abilities, affect_flagss[num].flagname);
	    }
	}
      if (!found)
	{
	  sprintf(bufff, "Which affect????\n\r.");
	  send_to_char(bufff, ch);
	}
      else
	{
	  send_to_char(bufff, ch);
	  return;
	}
    }
  if (!str_cmp (arg1, "affect2"))
    {
      char bufff[200];
      bool isoff = FALSE;
      int num = 0;
      bool found = FALSE;
      int affect = victim->pcdata->nat_abilities2;
      for (num = 0; str_cmp(affect_flags_2[num].what_you_type, "end_of_list") && (!found); num++)
	{
	  if (!str_cmp(arg2, affect_flags_2[num].what_you_type ))
	    {
	      found = TRUE;
	      isoff =((IS_SET(affect,affect_flags_2[num].flagname))? FALSE : TRUE);
	      sprintf(bufff, " %s bit toggled %s\n\r",affect_flags_2[num].what_you_type ,(isoff ? "On" : "Off") );
	      TOGGLE_BIT(victim->pcdata->nat_abilities2, affect_flags_2[num].flagname);
	    }
	}
      if (!found)
	{
	  sprintf(bufff, "Which affect????\n\r.");
	  send_to_char(bufff, ch);
	}
      else
	{
	  send_to_char(bufff, ch);
	  return;
	}
    }
  

  if (!str_prefix ("skill", arg1) && value > 0 && value < SKILL_COUNT && is_number (arg3))
    {
      int set_to;
      set_to = atoi (arg3);
      if (set_to < 0 || set_to > 100)
	{
	  send_to_char ("Valid ranges are 0 (unlearned) to 100 (perfect).\n\r", ch);
	  return;
	}
      if (set_to == 0)
	set_to = -100;
      victim->pcdata->learned[value] = set_to;
      send_to_char ("Skill/Spell set.\n\r", ch);
      return;
    }
  if (!str_prefix ("obj", arg1) && value != -1)
    {
      SINGLE_OBJECT *otr;
      SINGLE_OBJECT *ofnd;
      OBJ_PROTOTYPE *crobj;
      ofnd = NULL;
      if (value < 0)		/*Take away an object */
	{
	  value = 0 - value;
	  for (otr = victim->carrying; otr != NULL; otr = otr->next_content)
	    if (otr->pIndexData->vnum == value)
	      {
		ofnd = otr;
		break;
	      }
	  if (ofnd == NULL)
	    {
	      send_to_char ("Obj not found.\n\r", ch);
	      return;
	    }
	  if (ofnd->wear_loc != -1)
	    {
	      unequip_char (victim, ofnd);
	    }
	  obj_from (ofnd);
	  free_it (ofnd);
	  send_to_char ("Object removed.\n\r", ch);
	  return;
	}
      if ((crobj = get_obj_index (value)) == NULL)
	{
	  send_to_char ("Invalid object.\n\r", ch);
	  return;
	}
      ofnd = create_object (crobj, 1);
      obj_to (ofnd, victim);
      ofnd->wear_loc = -1;
      send_to_char ("Object added.\n\r", ch);
      return;
    }
  if (!str_prefix ("stor", arg1) && value != -1)
    {
      int otr;
      SINGLE_OBJECT *ofnd;
      ofnd = NULL;
      if (value < 0)		/*Take away an object */
	{
	  value = 0 - value;
	  for (otr = 0; otr < MAXST; otr++)
	    {
	      if (victim->pcdata->storage[otr] == NULL)
		continue;
	      if (victim->pcdata->storage[otr]->pIndexData->vnum == value)
		{
		  ofnd = victim->pcdata->storage[otr];
		  break;
		}
	    }
	  if (ofnd == NULL)
	    {
	      send_to_char ("Obj not found.\n\r", ch);
	      return;
	    }
	  free_it (victim->pcdata->storage[otr]);
	  victim->pcdata->storage[otr] = NULL;
	  send_to_char ("Object removed.\n\r", ch);
	  return;
	}
      send_to_char ("You can only remove personal storage items, not give.\n\r", ch);
      return;
    }
  if (!str_prefix ("silence", arg1))
    {
      victim->pcdata->act2 ^= PLR_SILENCE;
      send_to_char ("Toggled.\n\r", ch);
      return;
    }
  if (!str_prefix ("frozen", arg1))
    {
      victim->act ^= PLR_FREEZE;
      send_to_char ("Toggled.\n\r", ch);
      return;
    }
  if (!str_prefix ("crasher", arg1))
    {
      victim->act ^= ACT_CRASHER;
      send_to_char ("Toggled.\n\r", ch);
      return;
    }
  if (!str_cmp ("done", arg1))
    {
      CHAR_DATA *iii;
      ch->desc->connected = CON_PLAYING;
      ch->desc->pEdit = NULL;
      fix_char(victim);
      save_char_obj (victim);
      for (iii = char_list; iii != NULL; iii = iii->next)
	{
	  if (iii == victim)
	    return;
	}
      extract_char (victim, TRUE);
      return;
    }
  if (!str_suffix ("edit", arg1))
    {
      send_to_char ("Please type done before you go to another editor.\n\r", ch);
      return;
    }
  interpret (ch, store_arg);
  return;
}

void 
do_pedit (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  CHAR_DATA *cy;
  char edbuf[500];
  DEFINE_COMMAND ("pedit", do_pedit, POSITION_DEAD, 110, LOG_ALWAYS, "Allows you to edit a player online or offline.")

    if (!IS_REAL_GOD (ch))
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }
  if (!argy || argy == "" || argy[0] == '\0')
    {
      send_to_char ("Pedit who?", ch);
      return;
    }
  pedit_found = NULL;
  victim = NULL;
  for (cy = char_list; cy != NULL; cy = cy->next)
    {
      if (IS_MOB (cy))
	continue;
      if (!str_cmp (NAME (cy), argy))
	{
	  victim = cy;
	  break;
	}
    }
  sprintf (edbuf, "%s", argy);
  if (victim == NULL)
    {
      if (!load_char_obj (NULL, edbuf, TRUE))
	{
	  send_to_char ("That character was not found.\n\r", ch);
	  return;
	}
    }
  if (victim == NULL)
    victim = pedit_found;
  ch->desc->pEdit = (void *) victim;
  ch->desc->connected = CON_PEDITOR;
  return;
}


void raceswitch(CHAR_DATA * ch, int newrace)
{

  int i;
  char buf[SML_LENGTH];
  int oldrace = ch->pcdata->race;
  if (IS_MOB(ch)) return;


  /* Check to see if the choice of the new race is valid. */

  if ( newrace < 0 || newrace >= RACE_COUNT || !str_cmp(race_info[newrace].name, "blank"))
    {
      return;
    }
  
  if (ch->pcdata->race >=0 && ch->pcdata->race < RACE_COUNT && str_cmp(race_info[ch->pcdata->race].name, "blank"))
    {
      
      /* Get rid of the old races natural stuff and modify stats down or up */
	
      REMOVE_BIT(ch->pcdata->nat_abilities,race_info[ch->pcdata->race].affect_bits);
      REMOVE_BIT(ch->pcdata->nat_abilities2, race_info[ch->pcdata->race].affect_bits2);
      REMOVE_BIT( ch->affected_by,race_info[ch->pcdata->race].affect_bits);
      REMOVE_BIT(ch->more_affected_by, race_info[ch->pcdata->race].affect_bits2);
      REMOVE_BIT(ch->pcdata->augments, race_info[ch->pcdata->race].nat_augments);

      for (i = 0; i < NUM_STATS; i++)
	{
	  ch->pcdata->stat[i] -= race_info[ch->pcdata->race].limits[i];
	}
      ch->max_hit -= (LEVEL(ch) * race_info[ch->pcdata->race].hps_bonus);
      if (ch->max_hit < 10) ch->max_hit = 10;
      MAXHIT(ch);
      ch->damroll -= race_info[ch->pcdata->race].combat_damage_bonus;
    }

  /* Wipe skills */
  
  for (i = 0; i < SKILL_COUNT; i++)
    ch->pcdata->learned[i] = -100;
 
  /* Give pracs back. */
  ch->pcdata->learn = LEVEL(ch) +2;
  ch->pcdata->practice =
1+(LEVEL(ch)-1)*(wis_app[get_curr_wis(ch)-1].practice);



  /* Now add in the new stuff from alignment and race */
 

  ch->pcdata->race = newrace;
  {
    int i;
    for (i = 0;  i < NUM_STATS; i++)
      {
	ch->pcdata->stat[i] += race_info[ch->pcdata->race].limits[i];
      }
  }
  ch->pcdata->nat_abilities |= race_info[ch->pcdata->race].affect_bits;
  ch->pcdata->nat_abilities |= align_info[ch->pcdata->alignment].affect_bits;
  ch->affected_by |= ch->pcdata->nat_abilities;
  
  ch->pcdata->nat_abilities2 |= race_info[ch->pcdata->race].affect_bits2;
  ch->pcdata->nat_abilities2 |= align_info[ch->pcdata->alignment].affect_bits2;
  ch->more_affected_by |= ch->pcdata->nat_abilities2;
  
  ch->pcdata->augments |= race_info[ch->pcdata->race].nat_augments;
  ch->pcdata->augments |= align_info[ch->pcdata->alignment].nat_augments;

  ch->max_hit += (LEVEL(ch) * race_info[ch->pcdata->race].hps_bonus);
  if (ch->max_hit < 10) ch->max_hit = 10;
  MAXHIT(ch);
  ch->damroll += race_info[ch->pcdata->race].combat_damage_bonus;


  ch->height = number_range (race_info[ch->pcdata->race].height_min, race_info[ch->pcdata->race].height_max);
  natural_mana(ch);
  fix_char(ch);
  sprintf(buf, "You went from being a %s to being a %s.\n\r", ((oldrace >= 0 && oldrace < RACE_COUNT && str_cmp(race_info[oldrace].name, "blank")) ? race_info[oldrace].name : "Something"), ((ch->pcdata->race >= 0 && ch->pcdata->race < RACE_COUNT && str_cmp(race_info[ch->pcdata->race].name, "blank")) ? race_info[ch->pcdata->race].name : "Something"));
  send_to_char(buf, ch);
  return;
}


void alignswitch(CHAR_DATA * ch, int newalign)
{

  int i;
  char buf[SML_LENGTH];
  int oldalign = ch->pcdata->alignment;
  if (IS_MOB(ch)) return;


  /* Check to see if the choice of the new align is valid. */

  if ( newalign < 0 || newalign >= NUM_ALIGN || !str_cmp(align_info[newalign].name, "blank"))
    {
      return;
    }
  
  if (ch->pcdata->alignment >=0 && ch->pcdata->alignment < NUM_ALIGN && str_cmp(align_info[ch->pcdata->alignment].name, "blank"))
    {
      /* Get rid of the old aligns natural stuff and modify stats down or up */
	
     
      REMOVE_BIT(ch->pcdata->nat_abilities,race_info[ch->pcdata->race].affect_bits);
      REMOVE_BIT(ch->pcdata->nat_abilities2, race_info[ch->pcdata->race].affect_bits2);
      REMOVE_BIT(ch->affected_by,race_info[ch->pcdata->race].affect_bits);
      REMOVE_BIT(ch->more_affected_by, race_info[ch->pcdata->race].affect_bits2);
      REMOVE_BIT(ch->pcdata->augments, race_info[ch->pcdata->race].nat_augments);
      
      for (i = 0; i < NUM_STATS; i ++)
	{
	  ch->pcdata->stat[i] -= align_info[ch->pcdata->alignment].limits[i];
	}
      ch->max_hit -= (LEVEL(ch) * align_info[ch->pcdata->alignment].hps_bonus);
      if (ch->max_hit < 10) ch->max_hit = 10;
      MAXHIT(ch);
      ch->damroll -= align_info[ch->pcdata->alignment].combat_damage_bonus;
      
    }

  /* Wipe skills */
  if( LEVEL(ch) != pow.level_to_choose || ch->pcdata->remort_times > 0)
    for (i = 0; i < SKILL_COUNT; i++)
      ch->pcdata->learned[i] = -100;
  
  /* Give pracs back */

  ch->pcdata->learn = LEVEL(ch) +2;
  ch->pcdata->practice =
1+(LEVEL(ch)-1)*(wis_app[get_curr_wis(ch)-1].practice);

 

  /* Now add in the new stuff from alignment and align */
 
  ch->height = number_range (race_info[ch->pcdata->race].height_min, race_info[ch->pcdata->race].height_max);

  ch->pcdata->alignment = newalign;
  for (i = 0; i < NUM_STATS; i ++)
    {
      ch->pcdata->stat[i] += align_info[ch->pcdata->alignment].limits[i];
    }
  
  ch->pcdata->nat_abilities |= race_info[ch->pcdata->race].affect_bits;
  ch->pcdata->nat_abilities |= align_info[ch->pcdata->alignment].affect_bits;
  ch->affected_by |= ch->pcdata->nat_abilities;
  
  ch->pcdata->nat_abilities2 |= race_info[ch->pcdata->race].affect_bits2;
  ch->pcdata->nat_abilities2 |= align_info[ch->pcdata->alignment].affect_bits2;
  ch->more_affected_by |= ch->pcdata->nat_abilities2;
  
  ch->pcdata->augments |= align_info[ch->pcdata->alignment].nat_augments;
  ch->pcdata->augments |= race_info[ch->pcdata->race].nat_augments;

  ch->max_hit += (LEVEL(ch) * align_info[ch->pcdata->alignment].hps_bonus);
  if (ch->max_hit < 10) ch->max_hit = 10;
  MAXHIT(ch);
  ch->damroll += align_info[ch->pcdata->alignment].combat_damage_bonus;


  natural_mana(ch);
  fix_char(ch);
  sprintf(buf, "You went from being a %s to being a %s.\n\r", ((oldalign >= 0 && oldalign < NUM_ALIGN && str_cmp(align_info[oldalign].name, "blank")) ? align_info[oldalign].name : "Something"), ((newalign >= 0 && newalign < NUM_ALIGN && str_cmp(align_info[newalign].name, "blank")) ? align_info[newalign].name : "Something"));
  send_to_char(buf, ch);
  return;
}
