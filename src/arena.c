#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
void 
do_setlevels (CHAR_DATA * ch, char *argy)
{
  int i;
  int k;
  int l;
  char bufg[200];
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char arg3[SML_LENGTH];
  char prize_arg[MAX_NUM_BG_PRIZES][SML_LENGTH];
  int jj = 0;
  DEFINE_COMMAND ("setlevels", do_setlevels, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Sets up a battleground.")

  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);

/* If a BG was set up and we type it again first all the prizes
are sent back to the person who is running the bg and the prizes are 
cleared. Then, we strip off all the argument names to make up the new
prize list. It is possible to screw people in a BG by changing the 
prizes but that's mean. */

  for (jj = 0; jj < MAX_NUM_BG_PRIZES; jj++)
    {
      if(bg_prize[jj] != NULL)
        {
          obj_from(bg_prize[jj]);
          obj_to(bg_prize[jj], ch);
        }
      bg_prize[jj] = NULL;
      argy = one_argy(argy, prize_arg[jj]);
    }
  if (!is_number (arg2) || !is_number (arg1))
    {
      sprintf(bufg,"The syntax is Setlevels <min> <max> <battleground or prize name> <list of prize names>. \n\rThe current max number of prizes is %d.\n\r", (MAX_NUM_BG_PRIZES+1));
      send_to_char(bufg, ch);
      return;
    }
  k = atoi (arg2);
  i = atoi (arg1);
  if (i < 0 || i > MAX_LEVEL || k < 0 || k > MAX_LEVEL || i > k)
    {
      
      sprintf(bufg, "Ranges are 0-%d.\n\r", MAX_LEVEL);
      send_to_char (bufg, ch);
      return;
    }
  min_level = i;
  max_level = k;
  
  if (!is_number (prize_arg[0]))
  {
    bg_multiplier = (number_range(0, 2) * 50);
    send_to_char ("Setting Battlefield to Random.\n\r", ch);
    l = 0;
  }
  else
  {
    l = atoi(prize_arg[0]) - 1;
    if (l < 4 && l >= 0)
    {
      bg_multiplier = (l * 50);
      l = 1;
    }
    else
    {
      bg_multiplier = (number_range(0,2) * 50);
      send_to_char ("Setting Battlefield to Random.\n\r", ch);
      l = 0;
    }
  }
/* IT is possible to have bgs without prizes now. */

  for(jj = l; jj < MAX_NUM_BG_PRIZES; jj++)
    {
     if (prize_arg[jj] != "" && prize_arg[jj][0] != '\0' &&
         (bg_prize[jj] = get_obj_inv(ch, prize_arg[jj])) == NULL)
       {
         int kk;
         sprintf(bufg, "Bad prize number %d.\n\r", jj);
         send_to_char(bufg, ch);
         for (kk = 0; kk < MAX_NUM_BG_PRIZES; kk++)
           {
             if(bg_prize[kk] != NULL)
               {
                 obj_from(bg_prize[kk]);
                 obj_to(bg_prize[kk], ch);
                 bg_prize[kk] = NULL;
               }
           }
         return;
       }
     if(bg_prize[jj] != NULL) obj_from(bg_prize[jj]);
    }
  send_to_char ("Minimum and maximum battleground level, and prize(s) set.\n\r", ch);
  ticks_to_battlefield = 6;
  return;
}

void 
do_battleground (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("battleground", do_battleground, POSITION_DEAD, 0, LOG_NORMAL, "Signs you up, or removes you from the list of people joining the battleground (when it starts).")

    if (IS_MOB (ch))
    return;
  if (ch->pcdata->bgf)
    {
      send_to_char ("You no longer will participate in the battleground.\n\r", ch);
      ch->pcdata->bgf = FALSE;
      return;
    }
  if (LEVEL (ch) < min_level || LEVEL (ch) > max_level)
    {
      char bufff[500];
      sprintf (bufff, "The battleground is only open to levels %d to %d at this time.\n\r", min_level, max_level);
      send_to_char (bufff, ch);
      return;
    }
 if (ch->pcdata->no_quit_pk > 0)
    {
      send_to_char("Whoa there partner...no using battleground to get out of pk.\n\r", ch);
      return;
    }

  send_to_char ("You will now participate in the battleground when it begins.\n\r", ch);
  ch->pcdata->bgf = TRUE;
  return;
}

long 
bidtotal (int nummy)
{
  int k;
  long x;
  x = 0;
  for (k = 0; k < 50; k++)
    {
      if (biddata.bidders[k][0] == '\0')
	continue;
      if (biddata.bids[k] == 0)
	continue;
      if (nummy == 1 && biddata.bidfor[k] == TRUE)
	continue;
      if (nummy == 2 && biddata.bidfor[k] == FALSE)
	continue;
      x += biddata.bids[k];
    }
  return x;
}

void 
give_back_money (void)
{
  int k;
  CHAR_DATA *victim;
  for (k = 0; k < 50; k++)
    {
      if (biddata.bidders[k][0] == '\0')
	continue;
      if ((victim = get_player_world (char_list, biddata.bidders[k], FALSE)) == NULL)
	continue;
      if (IS_MOB (victim))
	continue;
      victim->pcdata->bank += biddata.bids[k];
    }
  return;
}

void 
free_arena (void)
{
  int k;
  biddata.fighters[0][0] = '\0';
  biddata.fighters[1][0] = '\0';
  for (k = 0; k < 50; k++)
    {
      biddata.bidders[k][0] = '\0';
      biddata.bids[k] = 0;
      biddata.bidfor[k] = FALSE;
    }
  biddata.taking_bids = FALSE;
  biddata.waitticks = -1;
  arena_timelimit = -1;
  return;
}

int 
get_bet_slot (char *charname)
{
  int i, k;
  i = -1;
  for (k = 0; k < 50; k++)
    {
      if (!str_cmp (biddata.bidders[k], charname))
	{
	  i = k;
	  break;
	}
    }
  return i;
}

int 
free_bet_slot (void)
{
  int k;
  int i;
  i = -1;
  for (k = 0; k < 50; k++)
    {
      if (biddata.bidders[k][0] == '\0')
	{
	  i = k;
	  break;
	}
    }
  return i;
}

void 
do_cancel (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *winner;
  CHAR_DATA *loser;
  check_ced (ch);
  if (ch == NULL)
    ch = char_list->next;
  if (ch == NULL)
    ch = char_list->next;
  if (ch == NULL)
    ch = char_list->next;
  winner = get_char_world_2 (ch, biddata.fighters[0]);
  loser = get_char_world_2 (ch, biddata.fighters[1]);
  if (winner != NULL)
    {
      check_ced (winner);
      char_from_room (winner);
      char_to_room (winner, get_room_index (winner->ced->wasroom));
      winner->ced->wasroom = 0;
    }
  if (loser != NULL)
    {
      check_ced (loser);
      char_from_room (loser);
      char_to_room (loser, get_room_index (loser->ced->wasroom));
      loser->ced->wasroom = 0;
    }
  free_arena ();
  do_arenaecho (ch, "$1$BArena battle has been cancelled.", FALSE);
  return;
}

void 
end_arena (CHAR_DATA * winner, CHAR_DATA * loser)
{
  bool whowon;
  char buffy[500];
  CHAR_DATA *temp;
  int totalbets;
  int winnermoney;
  int totalwinners;
  int share;
  int k;
  if (winner == loser)
    {
      CHAR_DATA *fch;
      for (fch = char_list; fch != NULL; fch = fch->next)
	{
	  if(IS_MOB(fch)) continue;
	  if(fch == loser) continue;
	  if(fch->fgt->challenge == 10)
	    {
	      winner = fch;
	      break;
	    }
	}
    }
  stop_fighting (winner, TRUE);
  stop_fighting (loser, TRUE);
  winner->fgt->challenge = 0;
  loser->fgt->challenge = 0;
  check_ced (winner);
  check_ced (loser);
  totalbets = 0;
  winnermoney = 0;
  totalwinners = 0;
  if (!str_cmp (NAME (winner), biddata.fighters[0]))
    whowon = FALSE;
  else
    whowon = TRUE;
  if (IS_PLAYER (winner)) /* JRAJRA */
    winner->pcdata->no_quit_pk = 0;
  if (IS_PLAYER (loser))
    loser->pcdata->no_quit_pk = 0;
  char_from_room (winner);
  char_from_room (loser);
  char_to_room (winner, get_room_index (winner->ced->wasroom));
  winner->ced->wasroom = 0;
  char_to_room (loser, get_room_index (loser->ced->wasroom));
  loser->ced->wasroom = 0;
  do_save (winner, "");
  do_save (loser, "");
  if (IS_PLAYER (winner))
    winner->pcdata->challenge_time = 12;
  if (IS_PLAYER (loser))
    loser->pcdata->challenge_time = 12;
  sprintf (buffy, "$B$7Arena Victor: $5%s$7. Arena Loser: $5%s$7.", NAME (winner), NAME (loser));
  do_arenaecho (winner, buffy, FALSE);
/*tally all the bets */
/*handle all the bets */
  for (k = 0; k < 50; k++)	/*remove bank money and tally totals */
    {
      if (biddata.bidders[k][0] == '\0')
	continue;
      if (biddata.bids[k] == 0)
	continue;
      if ((temp = get_player_world (winner, biddata.bidders[k], FALSE)) == NULL)
	continue;
      if (IS_MOB (temp))
	continue;
      /*temp->pcdata->bank-=biddata.bids[k]; */
      totalbets += biddata.bids[k];
      if (biddata.bidfor[k] == whowon)
	{
	  winnermoney += biddata.bids[k];
	  totalwinners++;
	}
      else
	{
	  send_to_char ("Bummer... can kiss your bet goodbye.\n\r", temp);
	}
      temp = NULL;
    }
  for (k = 0; k < 50; k++)	/*Award money to winners */
    {
      if (biddata.bidders[k][0] == '\0')
	continue;
      if ((temp = get_player_world (winner, biddata.bidders[k], FALSE)) == NULL)
	continue;
      if (IS_MOB (temp))
	continue;
      if (biddata.bidfor[k] == whowon)
	{
	  share = (((biddata.bids[k] * 100) / winnermoney) * totalbets) / 100;
	  if (biddata.bids[k] > share + 1)
	    share = biddata.bids[k] + 1;
	  temp->pcdata->bank += share;
	  sprintf (buffy, "You just won %d coins from your bet on the arena battle!\n\rThe money has been placed in your bank account.\n\r", share);
	  send_to_char (buffy, temp);
	  temp = NULL;
	}
    }
  free_arena ();
  return;
}

void 
do_arena (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  CHAR_DATA *victimtwo;
  char buffy[500];
  DEFINE_COMMAND ("arena", do_arena, POSITION_DEAD, 0, LOG_NORMAL, "Toggles arena message status.")

    if (IS_MOB (ch))
    return;
  if (argy != "" && argy[0] != '\0' && !str_cmp (argy, "on"))
    {
      ch->pcdata->arena_msg = TRUE;
      send_to_char ("You now will see all arena messages and can recieve challenges.\n\r", ch);
      return;
    }
  if (argy != "" && argy[0] != '\0' && !str_cmp (argy, "off"))
    {
      ch->pcdata->arena_msg = FALSE;
      send_to_char ("You will no longer see anything related to the arena.\n\r", ch);
      return;
    }
  if (LEVEL (ch) < MAX_LEVEL)
    return;
  if (biddata.fighters[0][0] == '\0')
    {
      send_to_char ("But no battle has been set up!\n\r", ch);
      return;
    }
  if ((victim = get_char_world_2 (ch, biddata.fighters[0])) == NULL)
    {
      sprintf (buffy, "<%s> was a lamer and left the game. Battle cancelled.", biddata.fighters[0]);
      give_back_money ();
      do_arenaecho (ch, buffy, FALSE);
      do_cancel (ch, "");
      return;
    }
  if ((victimtwo = get_char_world_2 (ch, biddata.fighters[1])) == NULL)
    {
      sprintf (buffy, "<%s> was a lamer and left the game. Battle cancelled.", biddata.fighters[1]);
      give_back_money ();
      do_arenaecho (ch, buffy, FALSE);
      do_cancel (ch, "");
      return;
    }
  arena ();
  return;
}

void 
arena (void)
{
  CHAR_DATA *victim;
  CHAR_DATA *victimtwo;
  CHAR_DATA *dumchar;
  char buffy[500];
  dumchar = char_list;
  if ((victim = get_char_world_2 (dumchar, biddata.fighters[0])) == NULL)
    {
      sprintf (buffy, "<%s> was a lamer and left the game. Battle cancelled.", biddata.fighters[0]);
      give_back_money ();
      do_arenaecho (dumchar, buffy, FALSE);
      do_cancel (dumchar, "");
      return;
    }
  if ((victimtwo = get_char_world_2 (victim, biddata.fighters[1])) == NULL)
    {
      sprintf (buffy, "<%s> was a lamer and left the game. Battle cancelled.", biddata.fighters[1]);
      give_back_money ();
      do_arenaecho (victim, buffy, FALSE);
      do_cancel (victim, "");
      return;
    }
  do_arenaecho (victim, "$4$BThe arena battle has started. All bets closed.", FALSE);
  biddata.taking_bids = FALSE;
  check_fgt (victim);
  check_fgt (victimtwo);
  victim->fgt->challenge = 10;
  victimtwo->fgt->challenge = 10;
  sprintf (buffy, "%s 504", NAME (victim));
  do_transfer (victim, buffy);
  save_char_obj (victim);
  sprintf (buffy, "%s 505", NAME (victimtwo));
  do_transfer (victimtwo, buffy);
  arena_timelimit = (LEVEL (victim) + LEVEL (victimtwo)) / 2;
  save_char_obj (victimtwo);
  return;
}

void 
do_bet (CHAR_DATA * ch, char *argy)
{
  char buffy[500];
  long bet;
  int fighternum;
  char buf[500];
  int kkkk;
  DEFINE_COMMAND ("bet", do_bet, POSITION_DEAD, 0, LOG_NORMAL, "This command allows you to bet on arena fights.")

    if (!pow.arena_on)
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }
  if (IS_MOB (ch))
    return;
  if (LEVEL (ch) > 109)
    {
      send_to_char ("You aren't allowed to bet... it's like giving mortals free money :(\n\r", ch);
      return;
    }
  if (LEVEL (ch) > 100 && LEVEL (ch) < MAX_LEVEL)
    {
      send_to_char ("Due to cheat possibilities, immortals may not bet.\n\r", ch);
      return;
    }
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("Bet INFO for arena news/status.\n\rBet fighter# <amount> to bet on a fighter.\n\rBet SHOW to view your current bet.\n\rBet WITHDRAW to remove your current bet.\n\r", ch);
      return;
    }
  if (!biddata.taking_bids && biddata.fighters[0][0] == '\0')
    {
      send_to_char ("No arena battle has been set up.\n\r", ch);
      return;
    }
  kkkk = get_bet_slot (NAME (ch));
  if (!str_cmp (argy, "info"))
    {
      int position;
      position = ch->position;
      ch->position = POSITION_STANDING;
      act ("$B$1-----Upcoming Arena Battle-----", ch, NULL, NULL, TO_CHAR);
      sprintf (buf, "$B$2Fighter #1: $3%s", biddata.fighters[0]);
      act (buf, ch, NULL, NULL, TO_CHAR);
      sprintf (buf, "$B$2Fighter #2: $4%s", biddata.fighters[1]);
      act (buf, ch, NULL, NULL, TO_CHAR);
      act ("$B$1-------------------------------", ch, NULL, NULL, TO_CHAR);
      sprintf (buf, "\n\r\x1B[1;37mTotal Money on fighter #1 so far: \x1B[32m%ld\x1B[1;37m.\n\rTotal Money on fighter #2 so far: \x1B[32m%ld\x1B[1;37m.\x1B[37;0m\n\r", bidtotal (1), bidtotal (2));
      send_to_char (buf, ch);
      ch->position = position;
      return;
    }
  if (!str_cmp (argy, "show"))
    {
      sprintf (buf, "\n\rTotal Money on fighter #1 so far: %ld.\n\rTotal Money on fighter #2 so far: %ld.\n\r", bidtotal (1), bidtotal (2));
      send_to_char (buf, ch);
      if (kkkk == -1)
	{
	  send_to_char ("You currently have no bet placed.\n\r", ch);
	  return;
	}
      if (biddata.bidfor[kkkk] == FALSE)
	sprintf (buffy, "%s", biddata.fighters[0]);
      else
	sprintf (buffy, "%s", biddata.fighters[1]);
      sprintf (buf, "Your current bet is \x1B[1;34m%ld\x1B[37;0m coins on \x1B[1;37m%s\x1B[0m.\n\r", biddata.bids[kkkk], buffy);
      send_to_char (buf, ch);
      return;
    }
  if (!biddata.taking_bids && biddata.fighters[0][0] != '\0')
    {
      send_to_char ("The battle has begun... bets are closed.\n\r", ch);
      return;
    }
/*OKAY, real bid below */
  if (!str_cmp (argy, "withdraw"))
    {
      if (kkkk == -1)
	{
	  send_to_char ("But you didn't bet anything...\n\r", ch);
	  return;
	}
      ch->pcdata->bank += biddata.bids[kkkk];
      biddata.bidders[kkkk][0] = '\0';
      biddata.bids[kkkk] = 0;
      biddata.bidfor[kkkk] = FALSE;
      send_to_char ("Bet removed.\n\r", ch);
      return;
    }
  argy = one_argy (argy, buffy);
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("Bet INFO for arena news/status.\n\rBet # <amount> to bet on a fighter.\n\rBet SHOW to view your current bet.\n\rBet WITHDRAW to remove your current bet.\n\r", ch);
      return;
    }
  if (!is_number (argy) || !is_number (buffy))
    {
      send_to_char ("Bet INFO for arena news/status.\n\rBet # <amount> to bet on a fighter.\n\rBet SHOW to view your current bet.\n\rBet WITHDRAW to remove your current bet.\n\r", ch);
      return;
    }
  fighternum = atoi (buffy);
  bet = atoi (argy);
  if (bet < 1)
    return;
  if (fighternum != 1 && fighternum != 2)
    {
      send_to_char ("Bet INFO for arena news/status.\n\rBet # <amount> to bet on a fighter.\n\rBet SHOW to view your current bet.\n\rBet WITHDRAW to remove your current bet.\n\r", ch);
      return;
    }
  if (!str_cmp (NAME (ch), biddata.fighters[0]) && fighternum == 2)
    {
      send_to_char ("You cannot bid on your opponent!\n\r", ch);
      return;
    }
  if (!str_cmp (NAME (ch), biddata.fighters[1]) && fighternum == 1)
    {
      send_to_char ("You cannot bid on your opponent!\n\r", ch);
      return;
    }
  if (bet < 1 || bet > ch->pcdata->bank)
    {
      send_to_char ("You don't have that much money in your bank.\n\r", ch);
      return;
    }
  if (kkkk == -1)
    {
      int q;
      if ((q = free_bet_slot ()) == -1)
	{
	  send_to_char ("Sorry, too many bets in already.\n\r", ch);
	  return;
	}
      strcpy (biddata.bidders[q], NAME (ch));
      biddata.bids[q] = bet;
      if (fighternum == 1)
	biddata.bidfor[q] = FALSE;
      else
	biddata.bidfor[q] = TRUE;
      kkkk = q;
    }
  else
    {
      ch->pcdata->bank += biddata.bids[kkkk];
      biddata.bids[kkkk] = bet;
      if (fighternum == 1)
	biddata.bidfor[kkkk] = FALSE;
      else
	biddata.bidfor[kkkk] = TRUE;
    }
  if (biddata.bidfor[kkkk] == FALSE)
    sprintf (buffy, "%s", biddata.fighters[0]);
  else
    sprintf (buffy, "%s", biddata.fighters[1]);
  sprintf (buf, "Bet of \x1B[1;37m%ld\x1B[0m coins placed on \x1B[1;32m%s\x1B[37;0m.\n\r", bet, buffy);
  send_to_char (buf, ch);
  send_to_char ("Bet placed. You may change it until the second the battle begins.\n\r", ch);
  ch->pcdata->bank -= bet;
  return;
}

void 
do_acceptc (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  ROOM_DATA *r;
  char tempstr[50];
  char buf[500];
  DEFINE_COMMAND ("accept", do_acceptc, POSITION_DEAD, 0, LOG_NORMAL, "This allows you to accept an arena challenge.")

    if (!pow.arena_on)
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }
  if (IS_MOB (ch))
    return;
  if (!ch->pcdata->challenged_by)
    {
      send_to_char ("But you have not been challenged!\n\r", ch);
      return;
    }
  strcpy (tempstr, ch->pcdata->challenged_by);
  if (biddata.fighters[0][0] != '\0')
    {
      send_to_char ("Someone beat you to it; an arena battle has already been arranged\n\rby another party. Try again after the battle.\n\r", ch);
      return;
    }
  if (argy == "" || argy[0] == '\0')
    {
      sprintf (buf, "\x1B[1;37mYou have been challenged by \x1B[32m%s\x1B[37m.\x1B[37;0m Type ACCEPT YES to accept the challenge.\n\r", ch->pcdata->challenged_by);
      send_to_char (buf, ch);
      return;
    }
  if (ch->hit < ch->max_hit)
    {
      send_to_char ("You must be at your max hit points to accept the challenge.\n\r", ch);
      return;
    }
  if (str_cmp (argy, "yes"))
    return;
  if ((victim = get_char_world_2 (ch, tempstr)) == NULL)
    {
      send_to_char ("You challenger left the world.\n\r", ch);
      if (ch->pcdata->challenged_by)
	free_string (ch->pcdata->challenged_by);
      ch->pcdata->challenged_by = NULL;
      return;
    }
  if (victim->desc == NULL)
    {
      send_to_char ("Your challenger has lost link.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    return;
  if (victim->hit < victim->max_hit)
    {
      send_to_char ("Your challenger was dumb and fought something or somehow fell below max\n\r", ch);
      send_to_char ("hitpoints. Tell your opponent to rest up, then you can accept.\n\r", ch);
      send_to_char ("Your challengee accepted but you are no longer at full hit points and thus\n\r", victim);
      send_to_char ("cannot fight the battle. Rest up and your opponent may accept later.\n\r", victim);
      return;
    }
  if (ch->in_room && ch->in_room->vnum < 1000)
    {
      send_to_char ("You cannot accept a challenge from here...\n\r", ch);
      return;
    }

  if ((r = get_room_index (VNUM_WAITONE)) == NULL)
    return;
  if (r->more && r->more->people)
    {
      send_to_char ("Someone was fucking around and screwed up the arena.  Try back later.\n\r", ch);
      return;
    }
  if ((r = get_room_index (VNUM_WAITTWO)) == NULL)
    return;
  if (r->more && r->more->people)
    {
      send_to_char ("Someone was fucking around and screwed up the arena.  Try back later.\n\r", ch);
      return;
    }
  sprintf (buf, "%s %s", NAME (victim), NAME (ch));
  do_set_fighters (ch, buf);
  if (ch->pcdata->challenged_by)
    {
      free_string (ch->pcdata->challenged_by);
      ch->pcdata->challenged_by = NULL;
    }
  victim->pcdata->challenge_time = 0;
  return;
}

void 
do_decline (CHAR_DATA * ch, char *argy)
{
  char buf[500];
  char tempstr[50];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("decline", do_decline, POSITION_DEAD, 0, LOG_NORMAL, "This command allows you to decline an arena challenge")

    if (IS_MOB (ch))
    return;
  if (!ch->pcdata->challenged_by)
    {
      send_to_char ("But you have not been challenged!\n\r", ch);
      return;
    }
  strcpy (tempstr, ch->pcdata->challenged_by);
  if (argy == "" || argy[0] == '\0')
    {
      sprintf (buf, "\x1B[1;37mYou have been challenged by \x1B[32m%s\x1B[37m.\x1B[37;0m Type DECLINE YES to decline the challenge.\n\r", ch->pcdata->challenged_by);
      send_to_char (buf, ch);
      return;
    }
  if (str_cmp (argy, "yes"))
    return;
  if ((victim = get_char_world_2 (ch, tempstr)) == NULL)
    {
      send_to_char ("You challenger left the world anyway... what an idiot.\n\r", ch);
      if (ch->pcdata->challenged_by)
	free_string (ch->pcdata->challenged_by);
      ch->pcdata->challenged_by = NULL;
      return;
    }
  if (ch->pcdata->challenged_by)
    free_string (ch->pcdata->challenged_by);
  ch->pcdata->challenged_by = NULL;
  if (IS_MOB (victim))
    return;
  victim->pcdata->challenge_time = 0;
  if (victim->desc == NULL)
    {
      send_to_char ("Your challenger has lost link.\n\r", ch);
      send_to_char ("-->You have successfully declined the challenge.<--\n\r", ch);
      send_to_char ("If you are being disturbed by challenges, type CHALLENGE OFF.\n\r", ch);
      return;
    }
  send_to_char ("\x1B[1;37m*****\x1B[0mYour challenge has just been declined!\n\r", victim);
  send_to_char ("-->You have successfully declined the challenge.<--\n\r", ch);
  send_to_char ("If you are being disturbed by challenges, type CHALLENGE OFF.\n\r", ch);
  return;
}

void 
do_challenge (CHAR_DATA * ch, char *argy)
{
  char buf[500];
  CHAR_DATA *victim;
  char arg1[500];
  DEFINE_COMMAND ("challenge", do_challenge, POSITION_DEAD, 0, LOG_NORMAL, "This command allows you to challenge another player to an arena battle.")

    if (!pow.arena_on)
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }
  if (IS_MOB (ch))
    return;
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("Format is Challenge <playername> if you wish to challenge someone.\n\r", ch);
      return;
      /* if (ch->pcdata->challenge_on) 
         {
         send_to_char("You no longer will accept any challenges from anyone.\n\r",ch);
         ch->pcdata->challenge_on=FALSE; 
         }
         else
         {
         send_to_char("You now accept challenges if given.\n\r",ch);
         ch->pcdata->challenge_on=TRUE;
         }
         return; */
    }
  if (!str_cmp (argy, "on"))
    {
      send_to_char ("You now accept challenges if given.\n\r", ch);
      ch->pcdata->challenge_on = TRUE;
      return;
    }
  if (!str_cmp (argy, "off"))
    {
      send_to_char ("You no longer will accept any challenges from anyone.\n\r", ch);
      ch->pcdata->challenge_on = FALSE;
      return;
    }
  if (biddata.fighters[0][0] != '\0')
    {
      send_to_char ("There is already a fight arranged/going on.\n\r", ch);
      return;
    }
  if (ch->hit != ch->max_hit)
    {
      send_to_char ("You must be at full hit points to challenge someone.\n\r", ch);
      return;
    }
  if (LEVEL (ch) < 6)
    {
      send_to_char ("You must be level 6 before you may fight in the arena.\n\r", ch);
      return;
    }
  if (ch->pcdata->challenge_on == FALSE)
    {
      send_to_char ("You must have challenge ON if you wish to challenge someone.\n\r", ch);
      return;
    }
  argy = one_argy (argy, arg1);
  if ((victim = get_char_world_2 (ch, arg1)) == NULL)
    {
      send_to_char ("That person is not currently playing or visible to you.\n\r", ch);
      return;
    }
  if (IS_MOB (victim))
    {
      send_to_char ("You cannot challenge NPC's.\n\r", ch);
      return;
    }
  if(ALIGN(ch) != 0 && ALIGN(victim) != 0) {
    if (!pow.arena_good_evil && (DIFF_ALIGN(ch, victim))) {
      send_to_char ("That person is not currently playing or visible to you.\n\r", ch);
      return;
    }
  }
  if (ch->in_room && ch->in_room->vnum < 1000)
    {
      send_to_char ("You cannot challenge someone from here...\n\r", ch);
      return;
    }
 
  if (LEVEL (victim) > 100)
    {
      send_to_char ("No challenging immorts!\n\r", ch);
      return;
    }
  
  if (victim->pcdata->challenge_on == FALSE)
    {
      send_to_char ("That player is not accepting challenges.\n\r", ch);
      return;
    }
  if (victim->pcdata->challenged_by && argy != "" && argy[0] != '\0' && !str_cmp (argy, "cancel") && !str_cmp (NAME (ch), victim->pcdata->challenged_by))
    {
      send_to_char ("Your challenger has cancelled the challenge. What a wimp!\n\r", victim);
      send_to_char ("Challenge withdrawn.\n\r", ch);
      if (victim->pcdata->challenged_by)
	{
	  free_string (victim->pcdata->challenged_by);
	  victim->pcdata->challenged_by = NULL;
	}
      ch->pcdata->challenge_time = 0;
      return;
    }
  if (ch->pcdata->challenge_time > 0)
    {
      send_to_char ("You must wait a bit before offering another challenge.\n\r", ch);
      return;
    }
  if (ch == victim)
    {
      send_to_char ("Challenge yourself? You mentally challenged or something, STUPID?!\n\r", ch);
      return;
    }
  if (ch->pcdata->no_quit_pk > 0)
    {
      send_to_char("Whoa there partner...no using challenges to get out of pk.\n\r", ch);
      return;
    }
  if (victim->pcdata->no_quit_pk > 0)
    {
      send_to_char("That person has been in combat recently and cannot arena until they calm down.\n\r", ch);
      return;
    }
  sprintf (buf, "\x1B[1;34mINCOMING CHALLENGE from \x1B[37m%s\x1B[1;34m.\n\r", NAME (ch));
  send_to_char (buf, victim);
  send_to_char ("\x1B[0;37mUse ACCEPT to accept the challenge, DECLINE to decline.\n\r", victim);
  ch->pcdata->challenge_time = 5;
  if (victim->pcdata->challenged_by)
    {
      free_string (victim->pcdata->challenged_by);
      victim->pcdata->challenged_by = NULL;
    }
  victim->pcdata->challenged_by = str_dup (NAME (ch));
  send_to_char ("You challenge has been successfully sent. Because the other character must\n\r", ch);
  send_to_char ("also be at full health, the response may be slow. If your victim does not\n\r", ch);
  send_to_char ("answer the challenge within 5 ticks, you may propose a challenge to\n\r", ch);
  send_to_char ("someone else. Type CHALLENGE <person> CANCEL to cancel this challenge.\n\r", ch);
  return;
}

void 
do_set_fighters (CHAR_DATA * ch, char *argy)
{
  char buffy[500];
  char buf[500];
  CHAR_DATA *victimtwo;
  CHAR_DATA *victim;
  ROOM_DATA *rm;
  ROOM_DATA *rm2;
  DEFINE_COMMAND ("setfighters", do_set_fighters, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Sets arena fighters.")

    if (IS_MOB (ch))
    return;
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("Format is setfighters <fighter1> <fighter2>.\n\r", ch);
      return;
    }
  argy = one_argy (argy, buffy);
  if (argy == "" || argy[0] == '\0')
    {
      send_to_char ("Format is setfighters <fighter1> <fighter2>.\n\r", ch);
      return;
    }
  if ((victimtwo = get_char_world_2 (ch, argy)) == NULL)
    {
      send_to_char ("The second person isn't in the world.\n\r", ch);
      return;
    }
  if ((victim = get_char_world_2 (ch, buffy)) == NULL)
    {
      send_to_char ("The first person isn't in the world.\n\r", ch);
      return;
    }
  if ((rm = get_room_index (VNUM_WAITONE)) == NULL)
    return;
  if ((rm2 = get_room_index (VNUM_WAITTWO)) == NULL)
    return;
  check_room_more (rm);
  check_room_more (rm2);
  if (rm->more->people != NULL ||
      rm2->more->people != NULL)
    {
      send_to_char ("Someone is apparently stuck in the waiting room, sorry.\n\r", victim);
      send_to_char ("Someone is apparently stuck in the waiting room, sorry.\n\r", victimtwo);
      do_cancel (ch, "");
      return;
    }
  check_ced (victim);
  check_ced (victimtwo);
  victim->ced->wasroom = victim->in_room->vnum;
  sprintf (buf, "%s %d", NAME (victim), VNUM_WAITONE);
  do_transfer (ch, buf);
  victimtwo->ced->wasroom = victimtwo->in_room->vnum;
  sprintf (buf, "%s %d", NAME (victimtwo), VNUM_WAITTWO);
  do_transfer (ch, buf);
  send_to_char ("You have been transferred to the waiting room while bets are placed.\n\r", victim);
  send_to_char ("You have been transferred to the waiting room while bets are placed.\n\r", victimtwo);
  biddata.waitticks = ARENA_WAIT_TICK;
  do_arenaecho (ch, "$B$1-----Upcoming Arena Battle-----", FALSE);
  sprintf (buf, "$B$2Fighter #1: $3%s", NAME (victim));
  do_arenaecho (ch, buf, FALSE);
  sprintf (buf, "$B$2Fighter #2: $4%s", NAME (victimtwo));
  do_arenaecho (ch, buf, FALSE);
  do_arenaecho (ch, "$B$1-------------------------------", FALSE);
  do_arenaecho (ch, "For betting info, type help bet", FALSE);
  biddata.taking_bids = TRUE;
  strcpy (biddata.fighters[0], NAME (victim));
  strcpy (biddata.fighters[1], NAME (victimtwo));
  return;
}
