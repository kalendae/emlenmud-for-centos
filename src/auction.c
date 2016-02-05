#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

/* I stole this idea from someplace like aardwolf or something...
   it's my code but I am sure this is close to how they did it. */

void 
do_bid (CHAR_DATA * ch, char *argy)	
{
  char general_use[STD_LENGTH*3];
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  AUCTION *auct;
  int auct_number;
  long bid;
  long prevbids;
  DEFINE_COMMAND ("bid", do_bid, POSITION_SLEEPING, 0, LOG_NORMAL, "This command allows you to bid on an auction.")
    
    if (IS_MOB (ch))
      return;
  
  if (LEVEL (ch) > IMM_LEVEL && LEVEL (ch) < MAX_LEVEL)
    {
      send_to_char ("You can't bid! You're an immortal.\n\r", ch);
      return;
    }
  if (ch->in_room->vnum < 1000)
    {
      send_to_char("No auction for you Portalwhore!!!!\n\r", ch);
      return;
    }
  if (IS_SET(ch->act, PLR_NOAUCTION))
    {
      send_to_char("Due to your abuse of the auction command, you cannot use it anymore. There is no imm command to undo this status.\n\r", ch);
      return;
    }
  if (argy[0] == '\0')
    {
      bool found = FALSE;
      for (auct = auction_list; auct != NULL; auct =auct->next)
	{
	  if (auct->bychar && !DIFF_ALIGN(auct->bychar, ch))
	    {
	      found = TRUE;
	      sprintf(general_use, "\x1b[1;34m#\x1b[1;32m[\x1b[1;33m%d\x1b[1;32m]\x1b[0;37m: %s\x1b[0;37m by %s, Bid: %ld, Minbid: %ld\n\r", 
		      auct->number, auct->item_name, NAME(auct->bychar), auct->bid, auct->minbid);
	      send_to_char (general_use, ch);
	    }
	}
      if (!found)
	{
	  send_to_char("There is currently nothing up for auction.\n\r", ch);
	  return;
	}
      else
	{
	  send_to_char("Type bid <amount> <auction number> to bid on an item.\n\r", ch);
	  return;
	}
    }
  argy = one_argy(argy, arg1);
  argy = one_argy(argy, arg2);
  if (!is_number(arg1) || !is_number(arg2))
    {
      send_to_char("Syntax: bid <amount> <auction number>\n\r", ch);
      return;
    }
  bid = atoi(arg1);
  auct_number = atoi(arg2);
  REMOVE_BIT(ch->pcdata->deaf, CHANNEL_AUCTION);
  prevbids = 0;
  for (auct = auction_list; auct != NULL; auct = auct->next)
    {
      if (auct->tochar == ch)
	prevbids += auct->bid;
    }
  for (auct = auction_list; auct != NULL; auct = auct->next)
    {
      if (auct->number == auct_number)
	{
	  if (LEVEL(ch) < MAX_LEVEL && DIFF_ALIGN(ch, auct->bychar))
	    {
	      send_to_char("There is no auction of that number going on.\n\r", ch);
	      return;
	    }
	  if (ch == auct->bychar)
	    {
	      send_to_char("You cannot bid on your own item!\n\r", ch);
	      return;
	    }
	  if (ch == auct->tochar)
	    {
	      send_to_char("You already have the highest bid on that item.\n\r", ch);
	      return;
	    }
	  if (bid < auct->minbid || bid < auct->bid + 100 || ch->pcdata->bank < bid + prevbids)
	    {
	      sprintf(general_use,"You must beat the current bid of %ld by at least 100 coins, the current minbid of %ld, and you must have the money in the bank to cover this bid and all other current bids!\n\r", auct->bid, auct->minbid);
	      send_to_char(general_use, ch);
	      return;
	    }
	  auct->going = 0;
	  auct->tochar = ch;
	  auct->bid = bid;
	  sprintf (general_use, "I'll pay %ld coins for \x1B[1;37m%s\x1B[1;34m!\x1b[0;37m", bid, auct->item_name);
	  do_auction(ch, general_use, auct->number);
	  return;
	}
    }
  send_to_char("There is no auction of that number going on.\n\r", ch);
  return;
}

void clear_auction(AUCTION *auct)
{
  AUCTION *prev;
  if(auct->item && auct->bychar)
    obj_to(auct->item, auct->bychar);
  auct->item = NULL;
  auct->tochar = NULL;
  auct->number = 0;
  auct->minbid = 0;
  auct->bid = 0;
  auct->item_name[0] = '\0';
  auct->bychar = NULL;
  if (auct == auction_list)
    {
      auction_list = auct->next;
      auct->next = NULL;
      free_m(auct);
      return;
    }
  for (prev = auction_list; prev != NULL; prev = prev->next)
    if (prev->next == auct)
      break;
  if (prev == NULL)
    {
      fprintf(stderr, "BUG: Tried to remove null member from auction list!\n");
      return;
    }
  else
    {
      prev->next = auct->next;
      auct->next = NULL;
      free_m(auct);
    }
  return;
}



AUCTION *
new_auction (void)
{
  AUCTION *auc;
  auc = mem_alloc (sizeof (*auc));
  auc->bychar = NULL;
  auc->tochar = NULL;
  auc->minbid = 100;
  auc->bid = 0;
  auc->item = NULL;
  auc->item_name[0] = '\0';
  auc->number = 0;
  auc->going = 0;
  auc->next = auction_list;
  auction_list = auc;
  return auc;
}
  




void 
do_tauction (CHAR_DATA * ch, char *argy)	/*By Owen Emlen */
{
  SINGLE_OBJECT *tobj;
  char buff[1024];
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  int ocost;
  AUCTION *auct_new = NULL;
  DEFINE_COMMAND ("auction", do_tauction, POSITION_SLEEPING, 0, LOG_NORMAL, "This command allows you to auction a piece of equipment.")
    if (IS_MOB (ch))
      return;
  
  if (IS_SET (ch->pcdata->deaf, CHANNEL_AUCTION))
    {
      send_to_char ("But you aren't even listening to the auction channel!\n\r", ch);
      return;
    }
  if (pow.auction_lag && ch->pcdata->no_quit_pk > 0) 
    {
      send_to_char("You must wait for your PKill no-quit timer to run out before you auction.\n\r",ch);
      return;
    }
  if (FIGHTING (ch))
    {
      send_to_char ("You cannot auction while fighting.\n\r", ch);
      return;
    }
  if (LEVEL (ch) > 99 && LEVEL(ch) < MAX_LEVEL)
    {
      send_to_char ("You cannot auction items as an immortal!\n\r", ch);
      return;
    }
  if (IS_SET(ch->act, PLR_NOAUCTION))
    {
      send_to_char("Due to your abuse of the auction command, you cannot use it anymore. There is no imm command to undo this status.\n\r", ch);
      return;
    }

  if (ch->in_room->vnum < 1000)
    {
      send_to_char("No auction for you Portalwhore!!!!\n\r", ch);
      return;
    }
  
  if (argy[0] == '\0')
    {
      send_to_char ("Syntax: Auction <item> <min-bid> <y/n>\n\r", ch);
      send_to_char ("Or : Auction halt\n\r", ch);
      send_to_char ("Note : Min-bid is in # of COPPER coins\n\r", ch);
      send_to_char (" : <y/n> is autotransfer (exclude y/n to see cost)\n\r", ch);
      return;
    }
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  if (!str_cmp (arg1, "halt"))
    {
      int halt_number = atoi(arg2);
      AUCTION *auct = NULL;
      AUCTION *auctn = NULL;
      AUCTION *prev = NULL;
      if (!is_number(arg2) || halt_number < 0)
	{
	  send_to_char("Auction halt <auction_number>.\n\r", ch);
	  return;
	}
      for (auct = auction_list; auct != NULL; auct = auctn)
	{
	  auctn = auct->next;
	  if (auct->bychar == ch && auct->number == halt_number)
	    {
	      if (auct->going > 1)
		{
		  send_to_char("You cannot withdraw this late into the auction!\n\r", ch);
		  return;
		}
	      sprintf(buff, "Auction of %s halted.\n\r", auct->item_name);
	      do_auction(ch, buff, auct->number);
	      clear_auction(auct);
              ch->pcdata->auction_count += 50;
              if (ch->pcdata->auction_count > 400)
                {
                  send_to_char("Due to abuse of auction, you cannot use it anymore. There is no imm command to undo this status.\n\r", ch);
                  SET_BIT(ch->act, PLR_NOAUCTION);
                }
	      return;
	    }
	}
      send_to_char("You are not selling anything!\n\r", ch);
      return;
    }
  for (auct_new = auction_list; auct_new != NULL; auct_new = auct_new->next)
    {
      if (auct_new->bychar == ch)
	{
	  send_to_char("You are already auctioning something.\n\r", ch);
	  return;
	}
    }


  if ((tobj = get_obj_inv (ch, arg1)) == NULL)
    {
      send_to_char ("You do not have that item!\n\r", ch);
      return;
    }
  if (tobj->wear_loc != -1 || tobj->in_obj != NULL)
    {
      send_to_char ("The object must be in your inventory!\n\r", ch);
      return;
    }
  if (!is_number (arg2))
    {
      send_to_char ("Syntax: Auction <item> <min bid # of coins> <y/n>\n\r", ch);
      send_to_char ("Or : Auction halt\n\r", ch);
      send_to_char ("Note : Min-bid is in # of COPPER coins\n\r", ch);
      send_to_char (" : <y/n> is autotransfer (exclude y/n to see cost)\n\r", ch);
      return;
    }
  ocost = atoi (arg2);
  if (ocost > 200000000)
    {
      send_to_char ("The maximum auction price is 200000000 coins.\n\r", ch);
      return;
    }
  if (ocost < 100 || tobj->cost < 10)
    {
      send_to_char ("Why bother selling something that cheap anyways!\n\r", ch);
      return;
    }
  if (ch->pcdata->bank < tobj->cost/7)
    {
      send_to_char ("You don't have enough money in the bank to autotransfer.\n\r", ch);
      return;
    }
  ch->pcdata->bank -= tobj->cost/7;
  obj_from (tobj);
  auct_new = new_auction();
  auct_new->bychar = ch;
  auct_new->item = tobj;
  auct_new->minbid = ocost + (tobj->cost/7);
  strcpy(auct_new->item_name, OOSTR(tobj, short_descr));
  auct_new->bid = 0;
  auct_new->number = ++auction_number;
  
  sprintf (buff, "<\x1B[1;37m%s\x1B[1;34m> MinBid: %d.",auct_new->item_name, auct_new->minbid);
  do_auction (ch, buff, auct_new->number);
  if (tobj->pIndexData->item_type == ITEM_WEAPON || tobj->pIndexData->item_type == ITEM_ARMOR)
    {
      get_condition (tobj);
      sprintf (buff, "Condition [%s]. Type BID to list all current auctions.",
	       condition_var);
      do_auction (ch, buff, auct_new->number);
    }
  ch->pcdata->listen_auction = TRUE;
  ch->pcdata->auction_count += 50;
  if (ch->pcdata->auction_count > 400)
    {
      send_to_char("Due to abuse of auction, you cannot use it anymore. There is no imm command to undo this status.\n\r", ch);
      SET_BIT(ch->act, PLR_NOAUCTION);
    }
  return;
}

void 
do_auction (CHAR_DATA * ch, char *argy, int number)
{
  char temp[STD_LENGTH];
  DESCRIPTOR_DATA *dd;
  sprintf (temp, "\x1B[1;32m<A> \x1b[1;34m#\x1b[1;32m[\x1b[1;33m%d\x1b[1;32m]\x1b[0;37m\x1B[34m[\x1B[1;37m%s\x1B[1;34m] %s\x1B[0;37m\n\r", number, NAME (ch), argy);
  for (dd = descriptor_list; dd != NULL; dd = dd->next)
    {
      if (!dd->character || dd->connected != CON_PLAYING)
	continue;
      if (DIFF_ALIGN(dd->character, ch))
	continue;
      if (IS_SET (dd->character->pcdata->deaf, CHANNEL_AUCTION))
	continue;
      send_to_char (temp, dd->character);
    }
  return;
}



void 
auction_update ()
{
  char general_use[STD_LENGTH];
  AUCTION *auct;
  AUCTION *auctn;
  AUCTION *prev;
  for (auct = auction_list; auct != NULL; auct = auctn)
    {
      auctn = auct->next;
      if (auct->bid == 0)
	{
	  if (++auct->going > 2)
	    {
	      sprintf (general_use, "Due to lack of interest, \x1B[1;37m%s\x1B[1;34m is no longer for sale.", auct->item_name);
	      do_auction (auct->bychar, general_use, auct->number);
	      clear_auction(auct);
	      continue;
	    }
	  entlab = 610;
	  sprintf (general_use, "Anyone? \x1B[1;37m%s\x1B[1;34m for %d coins?", auct->item_name, auct->minbid);
	  do_auction (auct->bychar, general_use, auct->number);
	  continue;
	}
      if (++auct->going < 3)
	{
	  entlab = 620;
	  if (auct->going == 1)
	    {
	      sprintf (general_use, "\x1B[1;37m%s\x1B[1;34m is going once to %s for %d coins.",   capitalize (auct->item_name),  NAME (auct->tochar), auct->bid);
	      do_auction (auct->bychar, general_use, auct->number);
	      continue;
	    }
	  if (auct->going == 2)
	    {
	      sprintf (general_use, "\x1B[1;37m%s\x1B[1;34m is going TWICE to %s for %d coins.",   capitalize (auct->item_name),   NAME (auct->tochar), auct->bid);
	      do_auction (auct->bychar, general_use, auct->number);
	      continue;
	    }
	}
      if (auct->going >= 3)
	{
	  sprintf (general_use, "\x1B[1;37m%s\x1B[1;34m has been SOLD to %s for %d coins.",   auct->item_name,   NAME (auct->tochar), auct->bid);
	  do_auction (auct->bychar, general_use, auct->number);
	  sprintf (general_use, "You won the auction!\n\rITEM: <%s> with your bid of %d coins.\n\r", auct->item_name, auct->bid);
	  send_to_char (general_use, auct->tochar);
	  send_to_char ("Item and money have been auto transferred.\n\r", auct->bychar);
	  auct->bychar->pcdata->bank += auct->bid;
	  auct->tochar->pcdata->bank -= auct->bid;
	  if(auct->item && auct->tochar)
	    obj_to (auct->item, auct->tochar);
	  auct->item = NULL;
	  clear_auction(auct);
	  continue;
	}
    }
  return;
}

