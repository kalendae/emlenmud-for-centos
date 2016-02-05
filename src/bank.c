#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
void
do_convert (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *mob;
  int monnn;
  char buf[STD_LENGTH];
  DEFINE_COMMAND ("convert", do_convert, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to convert copper into gold at a bank.")
    check_room_more (ch->in_room);
  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_MOB (mob) && IS_SET (mob->act, ACT_BANK_TELLER))
	break;
    }
  if (mob == NULL)
    {
      send_to_char ("You say 'Excuse me, can I convert my currency?'\n\r", ch);
      send_to_char ("Unfortunately, there is no banker in sight.\n\r", ch);
      return;
    }
  monnn = tally_coins (ch);
  if (monnn <= 0)
    return;
  sub_coins (monnn, ch);
  sprintf (buf, "%d", monnn);
  create_amount (monnn, ch, NULL, NULL);
/*do_coins(ch,buf); */
  send_to_char ("Your money has now been converted.\n\r", ch);
  return;
}
/* The format of this command is as follows:

	donate <city/clan> <warpoints/gold> <amount> where amount will
	be set to be at least 100 gold or 100 wps since this will force 
	a save to the disk.
	*/
void 
do_donate (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  char buf[SML_LENGTH];
  int place = 0;
  char type;
  int amount = 0;
  int bank_num;
  DEFINE_COMMAND ("donate", do_donate, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to donate gold or warpoints to the city or clan.")
    if (IS_MOB(ch))
      return;
  if (argy == "" || argy[0] == '\0')
    {
      show_donate(ch);
      return;
    }
  argy = one_argy(argy, arg1);
  if (!is_number (arg1))
    {
      show_donate(ch);
      return;
    }
  amount = atoi (arg1);
  if (amount <= 99)
    {
      show_donate(ch);
      return;
    }
  argy = one_argy(argy, arg2);
  if (!str_cmp (arg2, "warpoints") || !str_cmp(arg2, "wps"))
    type = 'W';
  else
    { 
      if (!str_cmp (arg2, "gold"))
	type = 'G';
      else
	{
	  show_donate (ch);
	  return;
	}
    }
  if ((type == 'G') && (ch->pcdata->bank < (amount*100)))
    {
      send_to_char("You don't have that much money in the bank!\n\r", ch);
      return;
    }
  else if ((type == 'W') && (ch->pcdata->warpoints < amount))
    {
      send_to_char("You don't have that many warpoints to donate!\n\r", ch);
      return;
    }
  argy = one_argy(argy, arg3);
  if (!str_cmp (arg3, "city") || !str_prefix(arg3, "align"))
    {
      place = 0;
      bank_num = ch->pcdata->alignment;
     
      if (type == 'W')
	    {
	      align_bank[bank_num].warpoints += amount;
	      ch->pcdata->warpoints -= amount;
	      sprintf(buf, "Ok donated %d warpoints to your alignment.\n\r", amount);
	      send_to_char (buf, ch);
	      do_bank (ch, "");
	    }
	  else
	    {
	      align_bank[bank_num].gold += amount;
	      ch->pcdata->bank -= 100*amount;
	      sprintf(buf, "Ok donated %d gold to your alignment.\n\r", amount);
	      send_to_char (buf, ch);
	      do_bank (ch, "");
	    }
     
      fwrite_align_bank (bank_num);
      return;
    }
  else
    { 
      if (!str_cmp (arg3, "clan"))
	{
	  place = 1;
	  bank_num = clan_number(ch);
	  if (bank_num <=0)
	    {
	      send_to_char("You aren't in a clan! *BONK* *BONK*\n\r", ch);
	      return;
	    }
	  if (type == 'W')
	    {
	      clan_bank[bank_num].warpoints += amount;
	      ch->pcdata->warpoints -= amount;
	      sprintf(buf, "Ok donated %d warpoints to the clan.\n\r", amount);
	      send_to_char (buf, ch);
	        do_bank (ch, "");
	    }
	  else
	    {
	      clan_bank[bank_num].gold += amount;
	      ch->pcdata->bank -= 100*amount;
	      sprintf(buf, "Ok donated %d gold to the clan.\n\r", amount);
	      send_to_char (buf, ch);
	      do_bank (ch, "");
	    }
	  fwrite_clan_bank (bank_num);
	  return;
	}
      else
	{
	  show_donate (ch);
	  return;
	}
    }
}
void 
do_bankset (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  char arg4[SML_LENGTH];
  bool align = FALSE;
  bool clan = FALSE;
  bool gold = FALSE;
  char type;
  int amount = 0;
  int bank_num;
  DEFINE_COMMAND ("setbank", do_bankset, POSITION_RESTING, MAX_LEVEL, LOG_NORMAL, "This allows admins to set bank amounts.")
    if (IS_MOB(ch))
      return;
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char("Type setbank <align/clan> <number> <amount> <gold/warpoints>\n\r", ch);
      return;
    }
  
  /* Is it alignment or clan bank? */


  argy = one_argy(argy, arg1);
  if (!str_cmp(arg1, "align"))
    align = TRUE;
  else if (!str_cmp(arg1, "clan"))
    clan = TRUE;
  else
    {
      send_to_char("Type setbank <align/clan> <number> <amount> <gold/warpoints>\n\r", ch);
      return;
    }


  /* Which alignment or clan is it? */

  argy = one_argy(argy, arg2);
  if (!is_number(arg2))
    {
      send_to_char("Type setbank <align/clan> <number> <amount> <gold/warpoints>\n\r", ch);
      return;
    }

  bank_num = atoi(arg2);
  if (align && (bank_num < 0 || bank_num >= NUM_ALIGN))
    {
       send_to_char("Type setbank <align/clan> <number> <amount> <gold/warpoints>\n\r", ch);
       send_to_char("You must pick an alignment number between 0 and the number of alignments.\n\r", ch);
      return;
    }
  if (clan && (bank_num < 0 || bank_num >= 23))
    {
       send_to_char("Type setbank <align/clan> <number> <amount> <gold/warpoints>\n\r", ch);
       send_to_char("You must pick an clan number between 0 and the number of clans.\n\r", ch);
      return;
    }
	

  /* How much are you setting the value to? */

  argy = one_argy(argy, arg3);
  if (!is_number(arg3))
    {
      send_to_char("Type setbank <align/clan> <number> <amount> <gold/warpoints>\n\r", ch); 
      return;
    }
  else
    amount = atoi(arg3);
  if (amount < 0 || amount > 30000)
    {
      send_to_char("choose an amount between 0 and 30000.\n\r", ch);
      return;
    }
  
  /* Are you setting gold or warpoints */
  
  
  argy = one_argy(argy, arg4);
  if (!str_cmp(arg4, "gold"))
    gold = TRUE;
  else if (!str_cmp(arg4, "warpoints") || !str_cmp(arg4, "wps"))
    gold = FALSE;
  else
    {
      send_to_char("Type setbank <align/clan> <number> <amount> <gold/warpoints>\n\r", ch); 
      return;
    }

  /* Now set the amount and save the corresponding bank. */

  if (align)
    {
      if (gold)
	align_bank[bank_num].gold = amount;
      else
	align_bank[bank_num].warpoints = amount;
      fwrite_align_bank(bank_num);
    }
  else
    {
      if (gold)
	clan_bank[bank_num].gold = amount;
      else
	clan_bank[bank_num].warpoints = amount; 
      fwrite_clan_bank(bank_num);
    }
  send_to_char(" Ok, bank set.\n\r", ch);
  return;
}


void
do_bank (CHAR_DATA * ch, char *argy)
{
  int clan_num;
  int align;
  int i;
  char buf[SML_LENGTH];
  DEFINE_COMMAND ("bank", do_bank, POSITION_RESTING, 0, LOG_NORMAL, "This command lets you see your alignment and clan bank and own acount.")
    if (IS_MOB(ch))
      return;
  align = ch->pcdata->alignment;
  clan_num = clan_number (ch);
  sprintf(buf, "Your alignment has %d gold and %d warpoints for upgrades.\n\r", align_bank[align].gold, align_bank[align].warpoints);
  send_to_char(buf,ch);
  if (clan_num >= 1)
    {
      sprintf(buf, "Your clan has %d gold and %d warpoints for upgrades.\n\r", clan_bank[clan_num].gold, clan_bank[clan_num].warpoints);
      send_to_char(buf,ch);
    }
  sprintf(buf, "You have %ld gold and %ld warpoints.\n\r", ((ch->pcdata->bank)/100), ch->pcdata->warpoints);
  send_to_char(buf,ch); 
  if (LEVEL(ch) < MAX_LEVEL) 
    return;

  send_to_char("\n\n*********ADMIN INFO**************\n\n\r", ch);
  

  for (i = 0; i < NUM_ALIGN; i++)
    {
      sprintf(buf, "%-12s:%5dG %5dW ", align_info[i].name,
align_bank[i].gold, align_bank[i].warpoints);
      send_to_char(buf, ch);
      if (i % 3 == 2 || i == (NUM_ALIGN-1))
	send_to_char("\n\r", ch);
    }
  for (i = 1; i < 23; i++)
    {
      sprintf(buf, "Clan %2d: %5dG %5dW ", i, clan_bank[i].gold,
clan_bank[i].warpoints);
      send_to_char(buf, ch);
      if (i % 3 == 2 || i == 22)
	send_to_char("\n\r", ch);
    }
  return;
}
      
void
do_balance (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *mob;
  char buf[STD_LENGTH];
  DEFINE_COMMAND ("balance", do_balance, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to see your bank balance when you are at a bank.")
    check_room_more (ch->in_room);
  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_MOB (mob) && IS_SET (mob->act, ACT_BANK_TELLER))
	break;
    }
  if (mob == NULL)
    {
      send_to_char ("You say 'Excuse me, what's my balance?'\n\r", ch);
      send_to_char ("Unfortunately, there is no banker in sight.\n\r", ch);
      return;
    }
  sprintf (buf, "The banker tells you 'You have %ld coins in your account.'\n\r", ch->pcdata->bank);
  send_to_char (buf, ch);
  return;
}

void
do_store (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *mob;
  char buf[STD_LENGTH];
  SINGLE_OBJECT *obj;
  char arg1[STD_LENGTH];
  bool found;
  int store_cost = 0;
  int k;
  DEFINE_COMMAND ("store", do_store, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to store an object in the bank vaults.")

    if (IS_MOB (ch))
    return;
  found = FALSE;
  if (argy == "" || argy[0] == '\0')
    {
      /*list all stored objects */
      send_to_char ("\x1B[1;30m[\x1B[34mItems on storage in the bank vault\x1B[30m]\x1B[37;0m\n\r\n\r", ch);
      for (k = 0; k < MAXST; k++)
	{
	  if (ch->pcdata->storage[k] != NULL)
	    {
	      found = TRUE;
	      sprintf (buf, "\x1B[1;37m%-2d> \x1B[0m%-30s \x1B[1;30m[\x1B[34m%d coins get back\x1B[30m]\x1B[37;0m\n\r", k + 1, strip_ansi_codes (OOSTR (ch->pcdata->storage[k], short_descr)), ch->pcdata->storage[k]->cost / STOREDIV);
	      send_to_char (buf, ch);
	    }
	}
      if (found)
	{
	  send_to_char ("\n\rTo get back a stored object, go to the bank and type UNSTORE #\n\r", ch);
	  send_to_char ("To store an item, go to the bank and type STORE <item>\n\r", ch);
	}
      else
	{
	  send_to_char ("Nothing.\n\r", ch);
	}
      return;
    }
/*End no arg */
  argy = one_argy (argy, arg1);
  if (arg1[0] == '\0')
    return;
  if (IS_MOB (ch))
    return;
  check_room_more (ch->in_room);
  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_MOB (mob) && IS_SET (mob->act, ACT_BANK_TELLER))
	break;
    }
  if (mob == NULL)
    {
      send_to_char ("You can only store items at a bank.\n\r", ch);
      return;
    }
  if ((obj = get_obj_inv (ch, arg1)) == NULL)
    {
      send_to_char ("You don't have that object.\n\r", ch);
      return;
    }

  if (IS_SET (obj->extra_flags, ITEM_NOSTORE) || obj->affected != NULL)
    {
      send_to_char ("Storage of this item is forbidden.\n\r", ch);
      return;
    }
  if (obj->wear_loc != WEAR_NONE)
    {
      send_to_char ("You must remove that object first.\n\r", ch);
      return;
    }
  if (obj->in_obj != NULL || obj->contains != NULL)
    {
      send_to_char ("Get the object into your normal inventory first.\n\r", ch);
      send_to_char ("IE: remove from backpack or sack, etc...\n\r", ch);
      return;
    }
  if (obj->cost < 1)
    {
      send_to_char ("We will not store that item for you.\n\r", ch);
      return;
    }
  sprintf (buf, "The banker will store that item for %s.\n\r. This fee is required for storage and retrieval of items.\n\r", name_amount (obj->cost / STOREDIV));
  send_to_char (buf, ch);
  if (argy == "" || argy[0] == '\0' || str_cmp (argy, "yes"))
    {
      send_to_char ("Type store <item> YES to actually put this item in storage.\n\r", ch);
      return;
    } 
  store_cost = obj->cost / STOREDIV;
  if (tally_coins (ch) < store_cost && ch->pcdata->bank < store_cost)
    {
      send_to_char ("Sorry, you do not have enough money to store the item.\n\r", ch);
      return;
    }
  if (tally_coins (ch) >= store_cost)
    {
      sub_coins (store_cost, ch);
    }
  else
    ch->pcdata->bank -= store_cost;
  k = free_storage (ch);
  if (k == -1)
    {
      send_to_char ("Sorry, we will not store any more items for you.\n\r", ch);
      return;
    }
  obj_from (obj);
  ch->pcdata->storage[k] = obj;
  send_to_char ("Your object is now safe in the vaults of the bank!\n\r", ch);
  save_char_obj (ch);
  return;
}

void
do_unstore (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *mob;
  char buf[STD_LENGTH];
  SINGLE_OBJECT *obj;
  char arg1[STD_LENGTH];
  int itemnum;
  int k;
  bool found;
  int remove_cost;
  DEFINE_COMMAND ("unstore", do_unstore, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to unstore an object from the bank vaults.")

    if (IS_MOB (ch))
    return;
  found = FALSE;
  if (argy == "" || argy[0] == '\0')
    {
      /*list all stored objects */
      send_to_char ("\x1B[1;30m[\x1B[34mItems on storage in the bank vault\x1B[30m]\x1B[37;0m\n\r\n\r", ch);
      for (k = 0; k < MAXST; k++)
	{
	  if (ch->pcdata->storage[k] != NULL)
	    {
	      found = TRUE;
	      sprintf (buf, "\x1B[1;37m%-2d> \x1B[0m%-30s \x1B[1;30m[\x1B[34m%d coins get back\x1B[30m]\x1B[37;0m\n\r", k + 1, strip_ansi_codes (OOSTR (ch->pcdata->storage[k], short_descr)), ch->pcdata->storage[k]->cost / STOREDIV);
	      send_to_char (buf, ch);
	    }
	}
      if (found)
	{
	  send_to_char ("\n\rTo get back a stored object, go to the bank and type UNSTORE #\n\r", ch);
	  send_to_char ("To store an item, go to the bank and type STORE <item>\n\r", ch);
	}
      else
	{
	  send_to_char ("Nothing.\n\r", ch);
	}
      return;
    }
/*End no arg */
  argy = one_argy (argy, arg1);
  check_room_more (ch->in_room);
  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_MOB (mob) && IS_SET (mob->act, ACT_BANK_TELLER))
	break;
    }
  if (mob == NULL)
    {
      send_to_char ("You can only get store items back at a bank.\n\r", ch);
      return;
    }
  if (arg1[0] != '\0' && is_number (arg1))
    {
      /*Start numeric withdrawl */
      itemnum = atoi (arg1);
      if (itemnum > MAXST || itemnum < 1)
	return;
      itemnum--;
      if (ch->pcdata->storage[itemnum] == NULL)
	{
	  send_to_char ("I could not find that number on storage.\n\r", ch);
	  return;
	}
      obj = ch->pcdata->storage[itemnum];
      k = itemnum;
    }
  else
    /*end numeric withdrawl */
    {
      /*start named withdrawl */
      arg1[0] = UPPER (arg1[0]);
      obj = NULL;
      for (k = 0; k < MAXST; k++)
	{
	  if (ch->pcdata->storage[k] != NULL && is_name (arg1, ch->pcdata->storage[k]->pIndexData->name))
	    {
	      obj = ch->pcdata->storage[k];
	      break;
	    }
	}
      if (obj == NULL)
	{
	  send_to_char ("I couldn't find that in storage.\n\r", ch);
	  return;
	}
    }
/*End named withdrawl */
  if (obj == NULL)
    {
      send_to_char ("I couldn't find that in storage.\n\r", ch);
      return;
    }
  remove_cost = obj->cost / STOREDIV;
  sprintf (buf, "The banker will give you the item back for %s.\n\r", name_amount (remove_cost));
  send_to_char (buf, ch);
  if (argy == "" || argy[0] == '\0' || str_cmp (argy, "yes"))
    {
      send_to_char ("Type unstore <item> YES to actually get the item back from storage.\n\r", ch);
      return;
    }
  if (tally_coins (ch) < remove_cost && ch->pcdata->bank < remove_cost)
    {
      send_to_char ("Sorry, you do not have enough money to get the item back.\n\r", ch);
      return;
    }
  if (tally_coins (ch) >= remove_cost)
    {
      sub_coins (remove_cost, ch);
    }
  else
    ch->pcdata->bank -= remove_cost;
  obj_to (obj, ch);
  ch->pcdata->storage[k] = NULL;
  send_to_char ("Item withdrawn from the bank vaults.\n\r", ch);
  save_char_obj (ch);
  return;
}

int
find_clan_in (CHAR_DATA * ch)
{
  CLAN_DATA *cdat;
  int retval;
  retval = -1;
  for (cdat = get_clan_index (1); cdat != NULL; cdat = get_clan_index (cdat->vnum + 1))
    {
      if (ch->in_room->vnum >= cdat->start_vnum + 1 && ch->in_room->vnum <= cdat->start_vnum + 20)
	retval = cdat->vnum;
    }
  return retval;
}

void
do_clanstore (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  SINGLE_OBJECT *obj;
  char arg1[STD_LENGTH];
  CLAN_DATA *clan;
  bool found = FALSE;
  int k;
  DEFINE_COMMAND ("clanstore", do_clanstore, POSITION_RESTING, 0, LOG_NORMAL, "Allows a character to put an object into clan storage.")

    if (IS_MOB (ch))
      return;
  if (!IS_SET(ch->in_room->room_flags, ROOM_CLANSTORE))
    {
      send_to_char("You are not in a clan storage location...\n\r", ch);
      return;
    }
  for (k = 0; k < 20 && !found; k++)
    {
      for (clan = clan_index_hash[k]; clan != NULL && !found; clan = clan->next)
	{
	  if ((ch->in_room->vnum >= clan->start_vnum) && (ch->in_room->vnum - clan->start_vnum) < 50)
	    {
	      found = TRUE;
	      break;
	    }
	}
    }
  if (!found || clan == NULL)
    {
      send_to_char("You are not in a clan storage location...\n\r", ch);
      return;
    }
  found = FALSE;
  if(argy == "" || argy[0] == '\0')
    {
      
      /*list all stored objects */
      send_to_char ("\x1B[1;30m[\x1B[34mItems on storage in the clan vault\x1B[30m]\x1B[37;0m\n\r\n\r", ch);
      for (k = 0; k < MAX_CLAN_STORAGE; k++)
	{
	  if (clan->storage[k] != NULL)
	    {
	      found = TRUE;
	      sprintf (buf, "\x1B[1;36m%-2d> \x1B[37m%-30s \x1B[34m[Clan Vault - Free]\x1B[37;0m\n\r", k + 1, strip_ansi_codes (OOSTR (clan->storage[k], short_descr)));
	      send_to_char (buf, ch);
	    }
	}
      if (found)
	{
	  send_to_char ("\n\rTo get an object from clan vaults, use CUNSTORE #\n\r", ch);
	  send_to_char ("To store a clan item, go to the clan donation and type CSTORE <item>\n\r", ch);
	}
      else
	{
	  send_to_char ("Nothing.\n\r", ch);
	}
      return;
    }
  argy = one_argy (argy, arg1);
  if ((obj = get_obj_inv (ch, arg1)) == NULL)
    {
      send_to_char ("You don't have that object.\n\r", ch);
      return;
    }

 
  if (obj->wear_loc != WEAR_NONE)
    {
      send_to_char ("You must remove that object first.\n\r", ch);
      return;
    }
  if (obj->in_obj != NULL || obj->contains != NULL)
    {
      send_to_char ("Get the object into your normal inventory first.\n\r", ch);
      send_to_char ("IE: remove from backpack or sack, etc...\n\r", ch);
      return;
    }
/*if (argy=="" || argy[0]=='\0' || str_cmp(argy,"yes")) 
   {
   send_to_char("Type cstore <item> YES to actually put this item in clan storage.\n\r",ch);
   return;
   }
 */
  k = free_clan_storage (clan);
  if (k == -1)
    {
      send_to_char ("Sorry, game currently limits clan storage to 30 items.\n\r", ch);
      return;
    }
  obj_from (obj);
  clan->storage[k] = obj;
  send_to_char ("Your object is now safe in the vaults of the clanhouse!\n\r", ch);
  fwrite_clan_storage (clan->vnum);
  return;
}

void
do_clanunstore (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  SINGLE_OBJECT *obj;
  char arg1[STD_LENGTH];
  int itemnum;
  int k;
  bool found = FALSE;
  CLAN_DATA *clan = NULL;
  DEFINE_COMMAND ("clanunstore", do_clanunstore, POSITION_RESTING, 0, LOG_NORMAL, "Allows a character to get an object from clan storage.")

    if (IS_MOB (ch))
      return;
  if (!IS_SET(ch->in_room->room_flags, ROOM_CLANSTORE))
    {
      send_to_char("You are not in a clan storage location...\n\r", ch);
      return;
    }
  for (k = 0; k < 20 && !found; k++)
    {
      for (clan = clan_index_hash[k]; clan != NULL && !found; clan = clan->next)
	{
	  if ((ch->in_room->vnum >= clan->start_vnum) && (ch->in_room->vnum - clan->start_vnum) < 50)
	    {
	      found = TRUE;
	      break;
	    }
	}
    }
  if (!found || clan == NULL)
    {
      send_to_char("You are not in a clan storage location...\n\r", ch);
      return;
    }
  found = FALSE;
  if (argy == "" || argy[0] == '\0')
    {
      /*list all stored objects */
      send_to_char ("\x1B[1;30m[\x1B[34mItems on storage in the clan vault\x1B[30m]\x1B[37;0m\n\r\n\r", ch);
      for (k = 0; k < MAX_CLAN_STORAGE; k++)
	{
	  if (clan->storage[k] != NULL)
	    {
	      found = TRUE;
	      sprintf (buf, "\x1B[1;36m%-2d> \x1B[37m%-30s \x1B[34m[Clan Vault - Free]\x1B[37;0m\n\r", k + 1, strip_ansi_codes (OOSTR (clan->storage[k], short_descr)));
	      send_to_char (buf, ch);
	    }
	}
      if (found)
	{
	  send_to_char ("\n\rTo get an object from clan vaults, use CUNSTORE #\n\r", ch);
	  send_to_char ("To store a clan item, go to the clan donation and type CSTORE <item>\n\r", ch);
	}
      else
	{
	  send_to_char ("Nothing.\n\r", ch);
	}
      return;
    }
  argy = one_argy (argy, arg1);
  if (arg1[0] != '\0' && is_number (arg1))
    {
      /*Start numeric withdrawl */
      itemnum = atoi (arg1);
      if (itemnum > MAX_CLAN_STORAGE || itemnum < 1)
	return;
      itemnum--;
      if (clan->storage[itemnum] == NULL)
	{
	  send_to_char ("I could not find that number on storage.\n\r", ch);
	  return;
	}
      obj = clan->storage[itemnum];
      k = itemnum;
    }
  else
    /*end numeric withdrawl */
    {
      /*start named withdrawl */
      arg1[0] = UPPER (arg1[0]);
      obj = NULL;
      for (k = 0; k < MAX_CLAN_STORAGE; k++)
	{
	  if (clan->storage[k] != NULL && is_name (arg1, clan->storage[k]->pIndexData->name))
	    {
	      obj = clan->storage[k];
	      break;
	    }
	}
      if (obj == NULL)
	{
	  send_to_char ("I couldn't find that in storage.\n\r", ch);
	  return;
	}
    }
/*End named withdrawl */
/*if (argy=="" || argy[0]=='\0' || str_cmp(argy,"yes")) 
   {
   send_to_char("Type cunstore <item> YES to actually get the item back from storage.\n\r",ch);
   return; 
   }
 */
  if (obj == NULL)
    {
      send_to_char ("I couldn't find that in storage.\n\r", ch);
      return;
    }
  obj_to (obj, ch);
  clan->storage[k] = NULL;
  send_to_char ("Item withdrawn from the clan vaults.\n\r", ch);
  save_char_obj (ch);
  fwrite_clan_storage (clan->vnum);
  return;
}

void
do_deposit (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *mob;
  char buf[STD_LENGTH];
  int amount;
  char arg1[STD_LENGTH];
  DEFINE_COMMAND ("deposit", do_deposit, POSITION_RESTING, 0, LOG_NORMAL, "This allows you to deposit coins into the bank.")

    argy = one_argy (argy, arg1);
  if (IS_MOB (ch))
    return;
  check_room_more (ch->in_room);
  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_MOB (mob) && IS_SET (mob->act, ACT_BANK_TELLER))
	break;
    }
  if (mob == NULL)
    {
      send_to_char ("You say 'I'd like to deposit some coins.'\n\r", ch);
      send_to_char ("... If only there was a banker here.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "all"))
    {
      amount = tally_coins (ch);
      if (amount < 1)
	{
	  send_to_char ("You have no money to deposit!\n\r", ch);
	  return;
	}
      ch->pcdata->bank += amount;
      sprintf (buf, "You deposit %d coins into your account.\n\r", amount);
      send_to_char (buf, ch);
      sprintf (buf, "Your new balance is: %ld copper.\n\r", ch->pcdata->bank);
      send_to_char (buf, ch);
      sub_coins (amount, ch);
      return;
    }
  if (!is_number (arg1))
    return;
  amount = atoi (arg1);
  if (amount < 1)
    return;
  if (!str_cmp (argy, "silver"))
    amount *= 10;
  if (!str_cmp (argy, "gold"))
    amount *= 100;
  if (!str_cmp (argy, "platinum"))
    amount *= 1000;
  if (!str_cmp (argy, "obsidian"))
    amount *= 10000;
  if (amount < 0 || amount > tally_coins (ch))
    {
      send_to_char ("You do not have that many coins.\n\r", ch);
      send_to_char ("Note: Deposit all and Withdraw all works too.\n\r", ch);
      return;
    }
  sub_coins (amount, ch);
  ch->pcdata->bank += amount;
  sprintf (buf, "You deposit %d coins into your account.\n\r", amount);
  send_to_char (buf, ch);
  sprintf (buf, "Your new balance is: %ld copper.\n\r", ch->pcdata->bank);
  send_to_char (buf, ch);
  return;
}

void
do_withdraw (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *mob;
  char buf[STD_LENGTH];
  int amount, surcharge;
  char arg1[STD_LENGTH];
  AUCTION *auct;
  DEFINE_COMMAND ("withdraw", do_withdraw, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to withdraw coins from your bank account.")

    if (IS_MOB (ch))
    return;
for (auct = auction_list; auct != NULL; auct = auct->next)
    {
      if (auct->tochar == ch)
        {
          send_to_char("You cannot withdraw money while you are bidding in an auction!\n\r", ch);
          return;
        }
    }

  argy = one_argy (argy, arg1);
  check_room_more (ch->in_room);
  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_MOB (mob) && IS_SET (mob->act, ACT_BANK_TELLER))
	break;
    }
  if (mob == NULL)
    {
      send_to_char ("You can only do that at a bank.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "all"))
    {
      /*if (get_bet_slot(NAME(ch))!=-1) 
         {
         send_to_char("Not while you have a bet on the arena battle going!\n\r",ch);
         return;
         }
       */
      amount = ch->pcdata->bank;
      if (amount < 5)
	return;
      surcharge = UMAX (1, (amount * .03));
      create_amount (amount - surcharge, ch, NULL, NULL);
      ch->pcdata->bank -= amount;
      sprintf (buf, "You withdraw \e[1;36m%d\e[0m from your account, and the bank keeps \e[1;36m%d\e[0m as interest.\n\r", amount - surcharge, surcharge);
      send_to_char (buf, ch);
      sprintf (buf, "Your new balance is: \e[1;36m%ld\e[0m.\n\r", ch->pcdata->bank);
      send_to_char (buf, ch);
      return;
    }
  amount = atoi (arg1);
  if (!str_cmp (argy, "silver"))
    amount *= 10;
  if (!str_cmp (argy, "gold"))
    amount *= 100;
  if (!str_cmp (argy, "platinum"))
    amount *= 1000;
  if (!str_cmp (argy, "obsidian"))
    amount *= 10000;
  if (amount <= 0)
    {
      send_to_char ("Please specify how much you wish to withdraw or ALL.\n\r", ch);
      return;
    }
  if (amount < 1 || amount > ch->pcdata->bank)
    {
      send_to_char ("You do not have that much in your account!\n\r", ch);
      send_to_char ("Withdraw ALL now works too.\n\r", ch);
      return;
    }
  surcharge = UMAX (1, (amount * .03));
  if (amount - surcharge <= 0)
    {
      send_to_char ("But you would get no money back with the three percent surcharge!\n\r", ch);
      return;
    }
  create_amount (amount - surcharge, ch, NULL, NULL);
  ch->pcdata->bank -= amount;
  sprintf (buf, "You withdraw \e[1;36m%d\e[0m from your account, and the bank keeps \e[1;36m%d\e[0m as interest.\n\r", amount - surcharge, surcharge);
  send_to_char (buf, ch);
  sprintf (buf, "Your new balance is: \e[1;36m%ld\e[0m.\n\r", ch->pcdata->bank);
  send_to_char (buf, ch);
  return;
}

#ifdef BNEW_WORLD
void
do_blackjack (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *mob;
  char buf[STD_LENGTH];
  int amount, pcard[6], ccard[6], ptotal = 0, ctotal = 0;
  int a, N;
  char arg1[STD_LENGTH];
  DEFINE_COMMAND ("blackjack", do_blackjack, POSITION_RESTING, 0, LOG_NORMAL, "This allows you to deposit coins into the bank.")

    argy = one_argy (argy, arg1);
  if (IS_MOB (ch))
    return;
  check_room_more (ch->in_room);

  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_MOB (mob) && IS_SET (mob->act, ACT_BANK_TELLER))
	break;
    }
  if (mob == NULL)
    {
      send_to_char ("You say 'I'd like to play blackjack.'\n\r", ch);
      send_to_char ("... If only there was a banker here.\n\r", ch);
      return;
    }
  if (!is_number (arg1))
    return;
  amount = atoi (arg1);
  if (amount < 1 || amount > tally_coins (ch))
    {
      send_to_char ("You do not have that many coins.\n\r", ch);
      return;
    }

  for (N = 0; N != 5; N++)
    {
      pcard[N] = number_range (1, 13);
      if (pcard[N] == 11)
	send_to_char ("You receive a Jack.\n\r", ch);
      else if (pcard[N] == 12)
	send_to_char ("You receive a Queen.\n\r", ch);
      else if (pcard[N] == 13)
	send_to_char ("You receive a King.\n\r", ch);
      else if (pcard[N] == 1)
	send_to_char ("You receive an Ace.\n\r", ch);
      else
	{
	  sprintf (buf, "You receive a %d.\n\r", pcard[N]);
	  send_to_char (buf, ch);
	}
      if (pcard[N] > 10)
	pcard[N] = 10;
      if (pcard[N] == 1)
	pcard[N] = 11;
      ptotal += pcard[N];
      if (ptotal > 21)
	for (a = -1; a != N; a++)
	  {
	    if (pcard[a] == 11)
	      {
		ptotal -= 10;
		pcard[a] = 1;
	      }
	  }

      if (ptotal > 16)
	break;
    }
  send_to_char ("\n\r", ch);

  for (N = 0; N != 5; N++)
    {
      ccard[N] = number_range (1, 13);
      if (ccard[N] == 11)
	send_to_char ("Dealer receives a Jack.\n\r", ch);
      else if (ccard[N] == 12)
	send_to_char ("Dealer receives a Queen.\n\r", ch);
      else if (ccard[N] == 13)
	send_to_char ("Dealer receives a King.\n\r", ch);
      else if (ccard[N] == 1)
	send_to_char ("Dealer receives an Ace.\n\r", ch);
      else
	{
	  sprintf (buf, "Dealer receives a %d.\n\r", ccard[N]);
	  send_to_char (buf, ch);
	}
      if (ccard[N] > 10)
	ccard[N] = 10;
      if (ccard[N] == 1)
	ccard[N] = 11;
      ctotal += ccard[N];
      if (ctotal > 21)
	for (a = -1; a != N; a++)
	  {
	    if (ccard[a] == 11)
	      {
		ctotal -= 10;
		ccard[a] = 1;
	      }
	  }
      if (ctotal > 16)
	break;
    }
  send_to_char ("\n\r", ch);
  sprintf (buf, "Your total is %d.\n\r", ptotal);
  send_to_char (buf, ch);
  sprintf (buf, "Computer's total is %d.\n\r", ctotal);
  send_to_char (buf, ch);

  if ((ptotal > ctotal) && (ptotal < 22))
    {
      send_to_char ("Congratulations!  You won...!\n\r", ch);
      ch->pcdata->bank += amount;
    }
  if ((ptotal == ctotal) && (ptotal < 22))
    {
      send_to_char ("Tie... Dealer wins.\n\r", ch);
      ch->pcdata->bank -= amount;
    }
  if ((ptotal < ctotal) && (ctotal < 22))
    {
      send_to_char ("You lost...\n\r", ch);
      ch->pcdata->bank -= amount;
    }
  if ((ptotal > 21) && (ctotal < 22))
    {
      send_to_char ("You lost...\n\r", ch);
      ch->pcdata->bank -= amount;
    }
  if ((ctotal > 21) && (ptotal < 22))
    {
      send_to_char ("Congratulations!  You won...!\n\r", ch);
      ch->pcdata->bank += amount;
    }
  if ((ptotal > 21) && (ctotal > 21))
    send_to_char ("Draw.. Both you and the dealer are over.\n\r", ch);

  return;
}
#endif
