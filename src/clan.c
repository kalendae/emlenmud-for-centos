
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

CLAN_DATA *clan_index_free;
CLAN_DATA *clan_index_free_2;
CLAN_DATA *

new_clan_index (void)
{
  CLAN_DATA *ClanData;
  int i;
  if (clan_index_free == NULL)
    {
      ClanData = mem_alloc (sizeof (*ClanData));
      top_clan++;
    }
  else
    {
      ClanData = clan_index_free;
      clan_index_free = clan_index_free->next;
    }
  ClanData->leader[0] = '\0';
  ClanData->name[0] = '\0';
  ClanData->alignment = 0;
  ClanData->minlev = 0;
  ClanData->start_vnum = 0;
  ClanData->shitlist = FALSE;
  for (i = 0; i < MAX_CLAN_STORAGE; i++)
    ClanData->storage[i] = NULL;
  for (i= 0; i <= 100; i++)
    ClanData->members[i] = NULL;
  return ClanData;
}

void 
do_cindex_2 (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char line[STD_LENGTH];
  char arg[SML_LENGTH];
  CLAN_DATA *clan;
  int iAtt;
  int cnt;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Cindex which clan?\n\r", ch);
      return;
    }
  if ((clan = get_clan_index_2 (atoi (arg))) == NULL)
    {
      send_to_char ("Invalid clan VNUM.\n\r", ch);
      return;
    }
  sprintf (buf, "\n\r\x1B[1;30m[\x1B[1;34mClan Number: %d\x1B[1;30m] \x1B[1;37m(%s)\x1B[0m\n\r\n\r", clan->vnum, clan->name);
  send_to_char (buf, ch);
  sprintf (buf, "Clan Leader/Founder: \x1B[1;37m%s\x1B[0m\n\r", clan->leader);
  send_to_char (buf, ch);
  sprintf (buf, "Minimum Level Entry: %d\n\r", clan->minlev);
  send_to_char (buf, ch);
  sprintf (buf, "Clanzone starting Vnum: %d\n\r", clan->start_vnum);
  send_to_char (buf, ch);
  if (clan-> alignment >= 0 && clan->alignment < NUM_ALIGN)
    {
      sprintf(buf, "Clan alignment: %s,\n\r\n\r", align_info[clan->alignment].name);
      send_to_char (buf, ch);
    }
  if (clan->shitlist && LEVEL(ch) > 100)
    send_to_char("This clan is currently on the shitlist.\n\r", ch);
   sprintf (buf, "Clan bank: %d gold and %d warpoints.\n\r", clan_bank[clan->vnum].gold, clan_bank[clan->vnum].warpoints);
   send_to_char (buf, ch);
  sprintf (buf, "-----[\x1B[1;34mClan Members\x1B[37;0m]----------------------->\n\r\n\r");
  send_to_char (buf, ch);
  cnt = 0;
  line[0] = '\0';
  for (iAtt = 0; iAtt < 100; iAtt = iAtt + 1)
    {
      if (clan->members[iAtt] == NULL || clan->members[iAtt][0] == ' ')
	continue;
      sprintf (buf, " %-15s ", clan->members[iAtt]);
      strcat (line, buf);
      ++cnt;
      if (cnt == 4)
	{
	  cnt = 0;
	  strcat (line, "\n\r");
	  send_to_char (line, ch);
	  line[0] = '\0';
	}
    }
  if (cnt != 0)
    {
      strcat (line, "\n\r");
      send_to_char (line, ch);
    }
  return;
}


void 
do_cindex (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char line[STD_LENGTH];
  char arg[SML_LENGTH];
  CLAN_DATA *clan;
  int iAtt;
  int cnt;
  one_argy (argy, arg);
  if (arg[0] == '\0')
    {
      send_to_char ("Cindex which clan?\n\r", ch);
      return;
    }
  if ((clan = get_clan_index (atoi (arg))) == NULL)
    {
      send_to_char ("Invalid clan VNUM.\n\r", ch);
      return;
    }
  sprintf (buf, "\n\r\x1B[1;30m[\x1B[1;34mClan Number: %d\x1B[1;30m] \x1B[1;37m(%s)\x1B[0m\n\r\n\r", clan->vnum, clan->name);
  send_to_char (buf, ch);
  sprintf (buf, "Clan Leader/Founder: \x1B[1;37m%s\x1B[0m\n\r", clan->leader);
  send_to_char (buf, ch);
  sprintf (buf, "Minimum Level Entry: %d\n\r", clan->minlev);
  send_to_char (buf, ch);
  sprintf (buf, "Clanzone starting Vnum: %d\n\r", clan->start_vnum);
  send_to_char (buf, ch);
  if (clan->alignment < 0 || clan->alignment > NUM_ALIGN-1)
    {
      clan->alignment = 0;
      save_clans();
    }
  sprintf(buf, "Clan alignment: %s\n\r\n\r\x1b[0;37m", align_info[clan->alignment].name);
  send_to_char(buf, ch);
  if (clan->shitlist && LEVEL(ch) > 100)
    send_to_char("This clan is currently on the shitlist.\n\r", ch);
  sprintf (buf, "Clan bank: %d gold and %d warpoints.\n\r", clan_bank[clan->vnum].gold, clan_bank[clan->vnum].warpoints);
   send_to_char (buf, ch);
  sprintf (buf, "-----[\x1B[1;34mClan Members\x1B[37;0m]----------------------->\n\r\n\r");
  send_to_char (buf, ch);
  cnt = 0;
  line[0] = '\0';
  for (iAtt = 0; iAtt < 100; iAtt = iAtt + 1)
    {
      if (clan->members[iAtt] == NULL || clan->members[iAtt][0] == ' ')
	continue;
      sprintf (buf, " %-15s ", clan->members[iAtt]);
      strcat (line, buf);
      ++cnt;
      if (cnt == 4)
	{
	  cnt = 0;
	  strcat (line, "\n\r");
	  send_to_char (line, ch);
	  line[0] = '\0';
	}
    }
  if (cnt != 0)
    {
      strcat (line, "\n\r");
      send_to_char (line, ch);
    }
  return;
}



CLAN_DATA *
new_clan_index_2 (void)
{
  CLAN_DATA *ClanData;
  int iAttack;
  if (clan_index_free_2 == NULL)
    {
      ClanData = mem_alloc (sizeof (*ClanData));
      top_clan_2++;
    }
  else
    {
      ClanData = clan_index_free_2;
      clan_index_free_2 = clan_index_free_2->next;
    }
  ClanData->leader[0] = '\0';
  ClanData->name[0] = '\0';
  ClanData->alignment = 0;
  ClanData->minlev = 0;
  ClanData->start_vnum = 0;
  ClanData->shitlist = FALSE;
  for (iAttack = 0; iAttack <= 100; iAttack++)
    ClanData->members[iAttack] = NULL;
  return ClanData;
}

void 
cedit (CHAR_DATA * ch, char *argy)
{
  CLAN_DATA *ClanData;
  char arg[STD_LENGTH];
  char arg1[STD_LENGTH];
  char arg2[STD_LENGTH];
  char buf[STD_LENGTH];
  int value;
  strcpy (arg, argy);
  ClanData = (CLAN_DATA *) ch->desc->pEdit;
  smash_tilde (argy);
  argy = one_argy (argy, arg1);
  strcpy (arg2, argy);
  value = atoi (arg2);
  if (!str_cmp (arg1, "show") || arg1[0] == '\0')
    {
      sprintf (buf, "%d", ClanData->vnum);
      if (ch->fgt->attacker_turn)
	do_cindex_2 (ch, buf);
      else
	do_cindex (ch, buf);
      return;
    }
  if (!str_cmp (arg1, "done"))
    {
      ch->desc->pEdit = NULL;
      ch->desc->connected = CON_PLAYING;
      send_to_char ("Please remember to ASAVE CLANS. :)\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "leader") && arg2[0] != '\0')
    {
      strcpy (ClanData->leader, arg2);
      send_to_char ("Clan Leader Set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "name") && arg2[0] != '\0')
    {
      strcpy (ClanData->name, arg2);
      send_to_char ("Clan Name Set.\n\r", ch);
      return;
    }
  if (!str_cmp(arg1, "shitlist"))
    {
      TOGGLE_BIT(ClanData->shitlist,1);
      send_to_char("Ok shitlist toggled.\n\r", ch);
      return;
    }
  if ((!str_cmp( arg1, "alignment") || !str_cmp(arg1, "align")) && is_number(arg2) && value >= 0 && value < NUM_ALIGN)
    {
      ClanData->alignment = value;
      send_to_char("Ok, clan alignment changed.\n\r", ch);
      return;
    }
  
  if ((!str_cmp (arg1, "start") || !str_cmp (arg1, "vnum")) &&
      is_number (arg2) && value >= 0)
    {
      ROOM_DATA *rcheck;
      rcheck = get_room_index (value);
      if (rcheck == NULL)
	{
	  send_to_char ("Invalid Starting VNUM.\n\r", ch);
	  return;
	}
      ClanData->start_vnum = value;
      send_to_char ("Clan starting VNUM of clanhouse set.\n\r", ch);
      return;
    }
  if (
       (!str_cmp (arg1, "minlev") ||
	!str_cmp (arg1, "level") ||
	!str_cmp (arg1, "minimum") ||
	!str_cmp (arg1, "minlevel"))
       && is_number (arg2) && value >= 0)
    {
      ClanData->minlev = value;
      send_to_char ("Clan minimum entry level set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "add") && arg2[0] != '\0' && strlen (arg2) > 2)
    {
      int cnum;
      char buffy[255];
      arg2[0] = UPPER (arg2[0]);
      if (check_clan (ClanData, arg2))
	{
	  send_to_char ("That person is already a member!\n\r", ch);
	  return;
	}
      cnum = clan_free (ClanData);
      if (cnum < 0)
	{
	  send_to_char ("Clan is FULL!\n\r", ch);
	  return;
	}
      ClanData->members[cnum] = str_dup (arg2);
      send_to_char ("Clan member added!\n\r", ch);
      sprintf (buffy, "%s is now a member of the clan!", arg2);
      clan_notify (buffy, ClanData->vnum);
      return;
    }
  if (!str_cmp (arg1, "remove") && arg2[0] != '\0' && strlen (arg2) > 2)
    {
      int i;
      bool foundy;
      foundy = FALSE;
      for (i = 0; i < 100; i++)
	{
	  if (!str_cmp (ClanData->members[i], arg2))
	    {
	      free_string (ClanData->members[i]);
	      ClanData->members[i] = NULL;
	      foundy = TRUE;
	      break;
	    }
	}
      if (foundy)
	send_to_char ("Clan member removed.\n\r", ch);
      else
	send_to_char ("Clan member not found.\n\r", ch);
      return;
    }
  interpret (ch, arg);
  return;
}


void 
do_cadd (CHAR_DATA * ch, char *argy)
{
  CLAN_DATA *ClanData;
  char t[1000];
  int i;
  DEFINE_COMMAND ("cadd", do_cadd, POSITION_DEAD, 0, LOG_NORMAL, "Adds a member to the clan.  For the clan leader only.")

    if (IS_MOB (ch))
    return;
  strcpy (t, argy);
  for (i = 0; t[i] != '\0'; i++)
    {
      if (!isalpha (t[i]))
	{
	  send_to_char ("No SPACES or non-letters can't be used!\n\r", ch);
	  return;
	}
    }
  if (clan_number (ch) < 1)
    {
      send_to_char ("You are not a clan leader!!\n\r", ch);
      return;
    }
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("Syntax: CADD <membername>\n\r", ch);
      return;
    }
  ClanData = get_clan_index (clan_number (ch));
  if (str_cmp (ch->pcdata->name, ClanData->leader))
    {
      send_to_char ("Only the clan leader may add members.\n\r", ch);
      return;
    }
  if (scan_clan (argy))
    {
      send_to_char ("That person is already in another clan!\n\r", ch);
      return;
    }
  {
    int cnum;
    CHAR_DATA * victim;
    char buffy[255];
    argy[0] = UPPER (argy[0]);
    if (check_clan (ClanData, argy))
      {
	send_to_char ("That person is already a member!\n\r", ch);
	return;
      }
    if ((victim = get_char_world(ch, argy)) == NULL)   
      {   
        send_to_char("They aren't here to add to the clan!\n\r", ch);
        return;
      }
    if (DIFF_ALIGN(ch, victim))
      {
        send_to_char("They aren't here to add to the clan!\n\r", ch); 
        return;
      }
    if (victim !=NULL && IS_SET(victim->act, PLR_NOCLAN))
      {
        send_to_char ("That person is not permitted to join a clan.\n\r",ch);
        return;
      }
    cnum = clan_free (ClanData);
    if (cnum < 0)
      {
	send_to_char ("Clan is FULL!\n\r", ch);
	return;
      }
    strcpy (buffy, argy);
    ClanData->members[cnum] = str_dup (buffy);
    {
      CHAR_DATA *victim;
      if ((victim = get_char_world(ch, argy)) != NULL && IS_PLAYER(victim))
	victim->pcdata->clan = cnum;
    }
    send_to_char ("Clan member added!\n\r", ch);
    sprintf (buffy, "%s is now a member of the clan!", argy);
    clan_notify (buffy, ClanData->vnum);
    save_clans ();
    return;
  }
  return;
}

void 
do_cremove (CHAR_DATA * ch, char *argy)
{
  CLAN_DATA *ClanData;
  DEFINE_COMMAND ("cremove", do_cremove, POSITION_DEAD, 0, LOG_NORMAL, "Removes a member of the clan.  For the clan leader only.")

    if (IS_MOB (ch))
    return;
  if (clan_number (ch) < 1)
    {
      send_to_char ("You are not a clan leader!!\n\r", ch);
      return;
    }
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("Syntax: CREMOVE <membername>\n\r", ch);
      return;
    }
  ClanData = get_clan_index (clan_number (ch));
  if (str_cmp (ch->pcdata->name, ClanData->leader))
    {
      send_to_char ("Only the clan leader may remove members.\n\r", ch);
      return;
    }
  {
    int i;
    bool foundy;
    foundy = FALSE;
    for (i = 0; i < 100; i++)
      {
	if (!str_cmp (ClanData->members[i], argy))
	  {
	    free_string (ClanData->members[i]);
	    ClanData->members[i] = NULL;
	    foundy = TRUE;
	    break;
	  }
      }
    if (foundy)
      {
	CHAR_DATA *victim;
	if ((victim = get_char_world(ch, argy)) != NULL && IS_PLAYER(victim))
	  victim->pcdata->clan = -1;
	send_to_char ("Clan member removed.\n\r", ch);
      }
    else
      send_to_char ("Clan member not found.\n\r", ch);
    save_clans ();
    return;
  }
  return;
}

void 
do_clan_quit (CHAR_DATA * ch, char *argy)
{
  int k;
  CLAN_DATA *ClanData;
  DEFINE_COMMAND ("cquit", do_clan_quit, POSITION_DEAD, 0, LOG_NORMAL, "Allows you to leave your current clan.")

    k = clan_number (ch);
  if (k < 1)
    {
      send_to_char ("You must first be in a clan before you can leave the clan!\n\r", ch);
      return;
    }
  ClanData = get_clan_index (k);
  if (ClanData != NULL)
    {
      int i;
      bool foundy;
      char buffy[500];
      foundy = FALSE;
      for (i = 0; i < 100; i++)
	{
	  if (!str_cmp (ClanData->members[i], NAME (ch)))
	    {
	      free_string (ClanData->members[i]);
	      ClanData->members[i] = NULL;
	      foundy = TRUE;
	      break;
	    }
	}
      if (foundy)
	{
	  send_to_char ("You have left the clan.\n\r", ch);
	  ch->pcdata->clan = -1;
	}
      else
	send_to_char ("Clan member not found.\n\r", ch);
      sprintf (buffy, "%s has left the clan!!", NAME (ch));
      clan_notify (buffy, k);
      save_clans ();
      return;
    }
  return;
}

void 
clan_check_align (CHAR_DATA * ch)
{
  int ii;
  int i;
  CLAN_DATA *ClanData;
  ii = clan_number (ch);
  if (ii < 1)
    return;
  ClanData = get_clan_index (ii);
  if (ClanData->alignment != ch->pcdata->alignment)
    {
      send_to_char ("You are no longer a member of your clan because of your alignment change!\n\r", ch);
      for (i = 0; i < 100; i++)
	{
	  if (!str_cmp (ClanData->members[i], RNAME (ch)))
	    {
	      free_string (ClanData->members[i]);
	      ClanData->members[i] = NULL;
	      break;
	    }
	}
      if (!str_cmp (ClanData->leader, RNAME (ch)))
	strcpy (ClanData->leader, "Leaderless!");
      return;
    }
  
  
  return;
}

int 
clan_number (CHAR_DATA * ch)
{
  CLAN_DATA *clan;
  int j;
  if (IS_MOB (ch))
    return -1;
  for (j = 0; j < 20; j++)
    {
      for (clan = clan_index_hash[j]; clan != NULL; clan = clan->next)
	{
	  int i;
	  if (!str_cmp (RNAME (ch), clan->leader))
	    return clan->vnum;
	  for (i = 0; i < 100; i++)
	    {
	      if (clan->members[i] != NULL && !str_cmp (RNAME (ch), clan->members[i]))
		{
		  return clan->vnum;
		}
	    }
	}
    }
  return -1;
}

int 
clan_number_2 (CHAR_DATA * ch)
{
  CLAN_DATA *clan;
  if (IS_MOB (ch))
    return -1;
  for
    (clan = get_clan_index_2 (1); clan != NULL; clan = get_clan_index_2 (clan->vnum + 1) /*clan->next */ )
    {
      int i;
      if (!str_cmp (RNAME (ch), clan->leader))
	return clan->vnum;
      for (i = 0; i < 100; i++)
	{
	  if (clan->members[i] != NULL && !str_cmp (RNAME (ch), clan->members[i]))
	    {
	      return clan->vnum;
	    }
	}
    }
  return -1;
}

bool 
scan_clan_2 (char *arg)
{
  CLAN_DATA *clan;
  for
    (clan = get_clan_index_2 (1); clan != NULL; clan = get_clan_index_2 (clan->vnum + 1) /*clan->next */ )
    {
      int i;
      if (!str_cmp (arg, clan->leader))
	return TRUE;
      for (i = 0; i < 100; i++)
	{
	  if (clan->members[i] != NULL && !str_cmp (arg, clan->members[i]))
	    {
	      return TRUE;
	    }
	}
    }
  return FALSE;
}

bool 
check_clan (CLAN_DATA * clan, char *argy)
{
  int i;
  if (!str_cmp (argy, clan->leader))
    return TRUE;
  for (i = 0; i < 100; i++)
    {
      if (clan->members[i] == NULL)
	continue;
      if (!str_cmp (clan->members[i], argy))
	return TRUE;
    }
  return FALSE;
}



bool 
scan_clan (char *arg)
{
  CLAN_DATA *clan;
  for
    (clan = get_clan_index (1); clan != NULL; clan = get_clan_index (clan->vnum + 1) /*clan->next */ )
    {
      int i;
      if (!str_cmp (arg, clan->leader))
	return TRUE;
      for (i = 0; i < 100; i++)
	{
	  if (clan->members[i] != NULL && !str_cmp (arg, clan->members[i]))
	    {
	      return TRUE;
	    }
	}
    }
  return FALSE;
}

int 
clan_free (CLAN_DATA * clan)
{
  int i;
  for (i = 0; i < 100; i++)
    {
      if (clan->members[i] == NULL)
	return i;
    }
  return -1;
}

void 
do_clan (CHAR_DATA * ch, char *argy)	/*CLANS by Owen Emlen */
{
  int ii;
  char buf[450];
  CLAN_DATA *t;
  DEFINE_COMMAND ("clan", do_clan, POSITION_DEAD, 0, LOG_NORMAL, "Shows 'clan list' or clan info on your current clan.")

    if (!str_cmp (argy, "list"))
    {
      int i;
      for (i = 0; i < 20; i++)
	{
	  for (t = clan_index_hash[i]; t != NULL; t = t->next)
	    {
	      sprintf (buf, "%d) %s %s   - Leader: %s\n\r", t->vnum, t->name,
               align_info[t->alignment].name, t->leader);
	      send_to_char (buf, ch);
	    }
	}

      return;
    }
  ii = clan_number (ch);
  if (ii < 1)
    {
      send_to_char ("You aren't in a clan!\n\r", ch);
      return;
    }
  sprintf (buf, "%d", ii);
  do_cindex (ch, buf);
  return;
}

void 
do_clan_2 (CHAR_DATA * ch, char *argy)	/*CLANS by Owen Emlen */
{
  int ii;
  char buf[450];
  CLAN_DATA *t;
  DEFINE_COMMAND ("clan2", do_clan_2, POSITION_DEAD, 0, LOG_NORMAL, "Shows 'clan2 list' or clan info on your current clan.")
    if (!str_cmp (argy, "list"))
    {
      int i;
      for (i = 0; i < 20; i++)
	{
	  for (t = clan_index_hash_2[i]; t != NULL; t = t->next)
	    {
	      sprintf (buf, "%d) %s\n\r", t->vnum, t->name);
	      send_to_char (buf, ch);
	    }
	}

      return;
    }
  ii = clan_number_2 (ch);
  if (ii < 1)
    {
      send_to_char ("You aren't in a secondary clan!\n\r", ch);
      return;
    }
  sprintf (buf, "%d", ii);
  do_cindex_2 (ch, buf);
  return;
}

