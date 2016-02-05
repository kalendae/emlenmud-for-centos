/* **************************************************************************
 *                             CHANNELS.C                           
 * **************************************************************************
 *
 *  Mud channels c file.  Contains functions responsible for controlling
 *  channels of communtication within the mud.  
 *
 *  DO NOT PUT FUNCTIONS IN HERE THAT DO NOT INVOLVE CHANNELS!!
 *  Update the following list if you add/remove/modify functions!! 
 *
 *  DOCUMENT ANY AND ALL CHANGES! YOU WILL BE FOUND AND KILLED IF YOU DO NOT!
 *
 *  Functions in this file:
 * --------------------------------------------------------------------------
 *  find_connection_num() - returns conneciton number for descriptor.
 *  channel_function()    - handles channel communication (chat, hunt, etc).
 *  wordwrap()		  - takes text, wraps it, and returns it.
 *  do_global()		  - ??
 *  do_ignore()		  - ignores/unignores characters.
 *  ignore()		  - returns boolean if char passed is ignored.
 *  do_chan_notify()	  - broadcast notification on channel.
 *  do_clan_notify()	  - broadcast notification on clan channel.
 *  clan_notify()         - does actual broadcast.
 *  group_notify()	  - broadcast on group channel.
 *  do_gen_gos()	  - ??
 *  do_yell()		  - yell function.
 *  do_say()		  - say function.
 *  do_say2()	  	  - wrapper for original say function (??).
 *  do_tell()		  - tell function.
 *  do_tell2()		  - wrapper for original tell function (??).
 *  do_quiet()		  - toggles quiet on and off.
 *  do_emote()		  - handles character emotes.
 *  do_emote2()		  - wrapper for original emote function (??).
 *  do_gtell()		  - handles group tells.
 *  do_gtell2()		  - wrapper for gtell function (??).
 */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
#ifndef WINDOWS
#include <sys/time.h>
#endif

char *chan[NUM_ALIGN][30][MAX_SCROLLBACK];
short chan_count[NUM_ALIGN][30];
char yellbuff[1000];

void talk_channel (CHAR_DATA * ch, char *argy, int channel, const char *verb);

#define ENDMARKER "^^"


int 
find_connection_num (DESCRIPTOR_DATA * d)
{
  int i;
  for (i = 0; i < 5; i++)
    {
      if (connectiond[i] == d)
	return i;
    }
  return -1;
}



void 
channel_function (CHAR_DATA * ch, char *argy, CHANNEL * c, int channel, char *te)
{
  char format[3000];
  char arg1[1000];
  char ind[100];
  bool is_al;
  CHAR_DATA *to;
  int ct;
  int to_align;
  int to_clan = 0;
  int to_clan2 = 0;		/* Add later */
  int p;
  char *t;
  bool is_d = FALSE;
  bool argoneused = FALSE;
  arg1[0] = '\0';
  if (IS_PLAYER (ch) && IS_SET (ch->pcdata->act2, PLR_SILENCE))
    {
      send_to_char ("You are silenced.\n\r", ch);
      return;
    }
  if (ALIGN(ch) < 0 || ALIGN(ch) >= NUM_ALIGN)
    {
      send_to_char("Alignment out of range.\n\r", ch);
      return;
    }
  for (t = argy; *t != '\0'; t++)
    {
      if (*t == '$')
	{
	  if (!pow.morts_use_color_chat && LEVEL (ch) < IMM_LEVEL)
	    {
	      send_to_char ("You can not use $ in a channel.\n\r", ch);
	      return;
	    }
	  is_d = TRUE;
	  continue;
	}
      if (is_d && (*t < '0' || *t > '9') && *t != 'R' && *t != 'B')
	{
	  send_to_char ("Illegal value in a channel.\n\r", ch);
	  return;
	}
      is_d = FALSE;
    }
  is_al = ((c->channel_name[0] >= 'A' && c->channel_name[0] <= 'Z') ||
	   (c->channel_name[0] >= 'a' && c->channel_name[0] <= 'z'));
  if (LEVEL (ch) < c->level)
    {
      send_to_char ("Huh?\n\r", ch);
      return;
    }

/* This doesn't let neuts use x-align! */
  if (LEVEL (ch) < IMM_LEVEL && c->to_align == TO_ALL && ALIGN(ch) == 0)
    {
      send_to_char ("Neutrals may not use this channel.\n\r", ch);
      return;
    }

  /* -------------------------------------------------------------- */
  /*           Check to see if channels is a clan channel.          */
  /* -------------------------------------------------------------- */
  if(c->clan)
    to_clan = clan_number (ch);
  if(c->clan2)
    to_clan2 = clan_number_2 (ch);
  if (to_clan < 1)
    to_clan = 0;
  if (to_clan2 < 1)
    to_clan2 = 0;
  if (c->clan2 && to_clan2 == 0)
    {
      send_to_char ("You aren't in a sect.\n\r", ch);
      return;
    }
  if (c->clan && to_clan == 0)
    {
      send_to_char ("You aren't in a clan.\n\r", ch);
      return;
    }
  
  /* -------------------------------------------------------------- */
  /*                           No Argument                          */
  /* -------------------------------------------------------------- */
  if (argy[0] == '\0' && LEVEL (ch) >= c->to_level)
    {
      int i, j, jj;
      j = ch->position;
      ch->position = POSITION_STANDING;

     /* ----------------------------------------------------------- */
     /* If in a clan, show clan history - handled like tell history */
     /* ----------------------------------------------------------- */
     if (to_clan || to_clan2)
     {
       /* Step through each index, displaying each one   */
       /* ctell_counter : stores number of ctells stored */
       /* last_ctells   : index storing ctells	       */
       for (i = ch->pcdata->ctell_counter + 1; i != ch->pcdata->ctell_counter; i++)
       {
        if (i == 15)
         {
           i = -1;
           continue;
         }
         if (i == -1)
    	   continue;
         if (ch->pcdata->last_ctells[i])
	  act (ch->pcdata->last_ctells[i], ch, NULL, ch, TO_CHAR);
       }
       ch->position = j;
       return;
     }

     /* ----------------------------------------------------------- */
     /*      Handle the channel, since its not a clan channel       */
     /* ----------------------------------------------------------- */
      for (jj = 0; jj < NUM_ALIGN; jj++)
	{
          if (LEVEL(ch) >= IMM_LEVEL || alliance[ALIGN(ch)][jj])
            {   
	      for (i = chan_count[jj][channel] + 1; i != chan_count[jj][channel]; i++)
		{
		  if (i == MAX_SCROLLBACK || i == -1)
		    {
		      i = -1;
		      continue;
		    }
		  if (chan[jj][channel][i])
		    {
		      act (chan[jj][channel][i], ch, NULL, ch, TO_CHAR);
		    }
		}
	    }
	}
      ch->position = j;
      return;
    }
  /* -------------------------------------------------------------- */
  /*                       Turn Channel Off                         */
  /* -------------------------------------------------------------- */
  if (!str_cmp (argy, "off"))
    {
      if (!IS_SET (ch->pcdata->chan_ignore, (1 << channel)))
	ch->pcdata->chan_ignore ^= (1 << channel);
      send_to_char ("You now ignore this channel.  Toggled \x1B[1;37mOFF\x1B[37;0m.\n\r", ch);
      return;
    }
  /* -------------------------------------------------------------- */
  /*                       Turn Channel On                          */
  /* -------------------------------------------------------------- */
  if (!str_cmp (argy, "on"))
    {
      if (IS_SET (ch->pcdata->chan_ignore, (1 << channel)))
	ch->pcdata->chan_ignore ^= (1 << channel);
      send_to_char ("Channel toggled \x1B[1;37mON\x1B[37;0m.\n\r", ch);
      return;
    }
  /* ---------------------------------------------------------------------- */
  /*     	              Ignoring Channel?				    */
  /* ---------------------------------------------------------------------- */
  if (IS_PLAYER(ch) && IS_SET (ch->pcdata->chan_ignore, (1 << channel)))
    {
      ch->pcdata->chan_ignore ^= (1 << channel);
    }

  /* ---------------------------------------------------------------------- */
  /*  				   Got Energy?				    */
  /* ---------------------------------------------------------------------- */
  if (IS_PLAYER (ch) && c->moves > 0 && LEVEL(ch) < IMM_LEVEL)
    {
      if (ch->move < (c->moves+5))
	{
	  send_to_char ("You don't have enough energy to use this channel.\n\r", ch);
	  return;
	}
      SUBMOVE(ch, (c->moves));
    }

  /* ---------------------------------------------------------------------- */
  /*                  Check to see what aligment to send to                 */
  /* ---------------------------------------------------------------------- */
  if (c->to_align == TO_SAME)
    {
      if (LEVEL (ch) > IMM_LEVEL)
	{
	  /* first word is stripped off, dont worry, its appended later */
	  argy = one_argy(argy, arg1);
	  if (is_number(arg1))
	    {
	      to_align = atoi(arg1);
	      if (to_align >= 0 && to_align < NUM_ALIGN)
	        argoneused = TRUE;
	      else
		to_align = -2;
	    }
	  else
	    to_align = -2;
	}
      else
	to_align = ALIGN (ch);
    }
  else
    to_align = -2;
  
 
  /* ---------------------------------------------------------------------- */
  /*                 Do Pray, Clan Talk here for the speaker                */
  /* ---------------------------------------------------------------------- */
  ind[0] = '\0';
  if (to_align == 0 && to_align < NUM_ALIGN)
    sprintf (ind, "\x1B[1;32m[Align %d]\x1B[0;37m ", to_align);
   else
   if (to_align == 1 && to_align < NUM_ALIGN)
    sprintf (ind, "\x1B[1;31m[Align %d]\x1B[0;37m ", to_align);
   else
   if (to_align == 2 && to_align < NUM_ALIGN)
    sprintf (ind, "\x1B[1;34m[Align %d]\x1B[0;37m ", to_align);
   else
   if (to_align == 3 && to_align < NUM_ALIGN)
    sprintf (ind, "\x1B[1;30m[Align %d]\x1B[0;37m ", to_align);
   format[0] = '\0';
   p = ch->position;
   ch->position = POSITION_STANDING;
   sprintf (format, "%s%sYou %s%s%s%s%s%s%s\x1B[0;37m", (LEVEL(ch) > IMM_LEVEL ? ind : ""), 
	    ch->pcdata->chan_color[channel], c->channel_name, 
	    (is_al ? ", '" : " "), (argoneused ? "" : arg1), (argoneused ? "" : " "),
	    argy, ch->pcdata->chan_color[channel], 
	    (is_al ? "'" : ""));
   act(format, ch, NULL, NULL, TO_CHAR);
  
   ch->position = p;

   /* -------------------------------------- */
   /* If not a clan channel, handle this way */
   /* -------------------------------------- */
   if (to_clan==0 && to_clan2==0) 
     {
       /* ---------------------------------------------------------------------- */
       /* Following line formats string stored for prays, gossips, hunts, etc    */
       /* Modify the line here to store whatever you want, such as player's name */
       /* that spoke on that channel - this affects MOST channels, but not tells */
       /* or group tells.							 */
       /* ---------------------------------------------------------------------- */
       if(channel == 5) {  // This is the IMMORTAL chat
	sprintf (format, "%s%10s > %s%s%s\x1B[37;0m", ch->pcdata->chan_color[channel], NAME(ch),
	  (argoneused ? "" : arg1), (argoneused ? "" : " "),argy);
       } else {
	sprintf (format, "%s%10s > %s%s%s\x1B[37;0m", ch->pcdata->chan_color[channel],
		IS_IMMORTAL(ch) ? "[Immortal]" : NAME(ch),
	  (argoneused ? "" : arg1), (argoneused ? "" : " "),argy);
       }
      
       if (chan[ALIGN (ch)][channel][chan_count[ALIGN (ch)][channel]]) {
 	  free (chan[ALIGN (ch)][channel][chan_count[ALIGN (ch)][channel]]);
	}
      
       chan[ALIGN (ch)][channel][chan_count[ALIGN (ch)][channel]] = malloc (strlen (format) + 1);
       strcpy (chan[ALIGN (ch)][channel][chan_count[ALIGN (ch)][channel]], format);
       chan_count[ALIGN (ch)][channel]++;
       if (chan_count[ALIGN (ch)][channel] >= MAX_SCROLLBACK)
	chan_count[ALIGN (ch)][channel] = 0;
     }
   /* -------------------------------------- */
   /*  Store Clan Talk for the speaker here  */
   /* -------------------------------------- */
   else
   {
     sprintf (format, "%s%10s > %s%s%s\x1B[37;0m", ch->pcdata->chan_color[channel],NAME(ch),
	  (argoneused ? "" : arg1), (argoneused ? "" : " "),argy);
     if (ch->pcdata->last_ctells[ch->pcdata->ctell_counter])
     {
       free (ch->pcdata->last_ctells[ch->pcdata->ctell_counter]);
     }
      ch->pcdata->last_ctells[ch->pcdata->ctell_counter] = malloc (strlen (format) + 1);
      strcpy (ch->pcdata->last_ctells[ch->pcdata->ctell_counter], format);
      ch->pcdata->ctell_counter++;
      if (ch->pcdata->ctell_counter > 14)
	ch->pcdata->ctell_counter = 0;
   }

  {
    /* ---------------------------------------------------------------------- */
    /*                 Do Pray, Clan Talk here for everyone else              */
    /* ---------------------------------------------------------------------- */
    DESCRIPTOR_DATA *d;
    for (d= descriptor_list; d != NULL; d = d->next)
      {
	if ((to = d->character) == NULL) continue;
	if (ch == to)
	  continue;
	if (IS_PLAYER (to))
	  {
	    if (LEVEL (to) < c->to_level)
	      continue;
	    if (ignore (ch, to))
	      continue; 
	    if (to->pcdata->quiet > 0)
	      continue;
	    if (IS_SET (to->pcdata->chan_ignore, (1 << channel)))
	      continue;
	    if (LEVEL (to) < IMM_LEVEL)
	      {
		if(c->clan2 && clan_number_2 (to) != to_clan2)
		  continue;
		if (c->clan && clan_number (to) != to_clan)
		  continue;
		if (to_align != -2 && !alliance[ALIGN(to)][ to_align])
		  continue;
	      }
	    sprintf (format, "%s%s%s %s%s%s%s%s\x1B[37;0m",
		     (LEVEL (to) > IMM_LEVEL ? ind : ""), to->pcdata->chan_color[channel], TPERS (ch, to),
		     c->channel_name, (is_al ? "s, '" : " "),
		     (argoneused ? "" : arg1), (argoneused ? "" : " "),
		     argy, to->pcdata->chan_color[channel], (is_al ? "'" : ""));
	    
            p = to->position;
	    to->position = POSITION_STANDING;
	    act (format, to, NULL, to, TO_CHAR);
	    to->position = p;

            /* ---------------------------------------------------- */
            /*      Store Clan Talk for everyone else here          */
            /* ---------------------------------------------------- */
            if (to_clan != 0 || to_clan2 != 0)
            { 
              sprintf (format, "%s%10s > %s%s%s\x1B[37;0m", ch->pcdata->chan_color[channel],NAME(ch),
                (argoneused ? "" : arg1), (argoneused ? "" : " "),argy);
              if (to->pcdata->last_ctells[to->pcdata->ctell_counter])
              {
                free (to->pcdata->last_ctells[to->pcdata->ctell_counter]);
              }
              to->pcdata->last_ctells[to->pcdata->ctell_counter] = malloc (strlen (format) + 1);
              strcpy (to->pcdata->last_ctells[to->pcdata->ctell_counter], format);
              to->pcdata->ctell_counter++;
              if (to->pcdata->ctell_counter > 14)
               to->pcdata->ctell_counter = 0;
            }
            /* ----------- End Store ------------- */

	  }
      }
  }
  
  return;
}


char *
wordwrap (char *txt, int wrapamount)
{
  static char hugebuf[20048];
  char *t = txt;
  bool in_ansi = FALSE;
  char lastword[2048];
  int linel = 0;
  int lastword_len = 0;
  int lastword_wordlen = 0;
  hugebuf[0] = '\0';
  for (; *t != '\0'; t++)
    {
      if (*t == '\r')
	continue;
      if (*t == '\n')
	{
	  if (linel + lastword_wordlen > wrapamount)
	    {
	      strcat (hugebuf, "\n\r");
	      linel = lastword_wordlen;
	    }
	  lastword[lastword_len] = '\0';
	  strcat (hugebuf, lastword);
	  strcat (hugebuf, "\n\r");
	  linel = 0;
	  lastword_wordlen = 0;
	  lastword_len = 0;
	  continue;
	}
      if (*t == '\x1B')
	{
	  in_ansi = TRUE;
	}
      lastword[lastword_len] = *t;
      lastword_len++;
      if (!in_ansi)
	lastword_wordlen++;
      if (*t == 'm' || *t == 'H')
	{
	  in_ansi = FALSE;
	}
      if (*t == ' ')
	{
	  lastword[lastword_len] = '\0';
	  if (linel + lastword_wordlen > wrapamount)
	    {
	      strcat (hugebuf, "\n\r");
	      linel = lastword_wordlen;
	    }
	  else
	    linel += lastword_wordlen;
	  strcat (hugebuf, lastword);
	  lastword_wordlen = 0;
	  lastword_len = 0;
	}
    }
  lastword[lastword_len] = '\0';
  if (linel + lastword_wordlen > wrapamount)
    strcat (hugebuf, "\n\r");
  strcat (hugebuf, lastword);
hugebuf[20000]='\0';
  return hugebuf;
}

void 
do_global (char *buf, int level, int toggler)
{
  DESCRIPTOR_DATA *d;
  if (level > MAX_LEVEL)
    return;
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d->connected == CON_PLAYING
	  && d->character
	  && LEVEL (d->character) >= level
	  && IS_SET (d->character->act, toggler)
	  && !d->character->pcdata->quiet)
	{
	  send_to_char (buf, d->character);
	  send_to_char ("\n\r", d->character);
	}
    }
  return;
}

void 
do_ignore (CHAR_DATA * ch, char *argy)
{
  int k;
  char general_use[STD_LENGTH];
  DEFINE_COMMAND ("ignore", do_ignore, POSITION_DEAD, 0, LOG_NORMAL, "This command allows you to ignore a specific player.")

    if (IS_MOB (ch))
    return;
  if (argy == "" || argy[0] == '\0')
    {
      bool foundy;
      foundy = FALSE;
      send_to_char ("-Currently Ignored Characters-\n\r", ch);
      for (k = 0; k < MAX_IGNORE; k++) /* JRAJRA */
	{
	  if (ch->pcdata->ignore[k])
	    {
	      sprintf (general_use, "+= %s =+\n\r", ch->pcdata->ignore[k]);
	      foundy = TRUE;
	      send_to_char (general_use, ch);
	    }
	}
      if (!foundy)
	send_to_char ("None.\n\r", ch);
      send_to_char ("\n\rSyntax: Ignore <name>\n\r", ch);
      return;
    }
  if (!str_cmp(argy, "Structure") || !str_cmp(argy, "Bern") || !str_cmp(argy, "Atog"))
    {
      send_to_char("\x1b[1;32mY00 R\x1b[1;36m0\x1b[1;35m0\x1b[1;37m0\x1b[1;34m0\x1b[1;32m0\x1b[1;33m0\x1b[1;31m0\x1b[1;32mL\x1b[1;31m tRyInG tO iGnOrE\x1b[1;33m aN aDmIn!\n\r", ch);
      return;
    }
  




  if (strlen (argy) > 17)
    return;
  for (k = 0; k < 17; k++)
    {
      /*Check for whitespaces */
      if (argy[k] == '\0')
	break;
      if (argy[k] == ' ')
	{
	  send_to_char ("Syntax is: IGNORE <name>\n\r", ch);
	  return;
	}
    }
  for (k = 0; k < MAX_IGNORE; k++)
    {
      if (ch->pcdata->ignore[k] && !str_cmp (ch->pcdata->ignore[k], argy))
	{
	  send_to_char ("Ignore removed.\n\r", ch);
	  free_string (ch->pcdata->ignore[k]);
	  ch->pcdata->ignore[k] = NULL;
	  return;
	}
    }
  for (k = 0; k < MAX_IGNORE; k++)
    {
      if (!ch->pcdata->ignore[k])
	{
	  ch->pcdata->ignore[k] = str_dup (capitalize (argy));
	  send_to_char ("Ignore added.\n\r", ch);
	  return;
	}
    }
  send_to_char ("No free ignore slots. Free one up by typing IGNORE <name>.\n\r", ch);
  return;
}

bool 
ignore (CHAR_DATA * victim, CHAR_DATA * ch)	/*Is the character ignored? */
{
  int k;
  if (IS_MOB (ch))
    return FALSE;
   if (IS_MOB(victim))
    return FALSE;
   if (LEVEL(victim) >=MAX_LEVEL)
    return FALSE;
  for (k = 0; k < MAX_IGNORE; k++)
    {
      if (!ch->pcdata->ignore[k])
	continue;
      if (!str_prefix (ch->pcdata->ignore[k], NAME (victim)))
	return TRUE;
    }
  return FALSE;
}

void 
do_chan_notify (CHAR_DATA * ch, char *txt)
{
#ifdef NEW_WORLD
  DESCRIPTOR_DATA *dd;
  char buf[2048];
  if (LEVEL (ch) < 2)
    return;
  for (dd = descriptor_list; dd != NULL; dd = dd->next)
    {
      if (!dd->character)
	continue;
      if (dd->connected != CON_PLAYING)
	continue;
      if (LEVEL (ch) > IMM_LEVEL && LEVEL (dd->character) < IMM_LEVEL+1)
	continue;
      if (ch == dd->character)
	continue;
      if (IS_SET (dd->character->pcdata->deaf, CHANNEL_NOTIFY))
	continue;
      if (LEVEL (dd->character) < IMM_LEVEL && DIFF_ALIGN(ch, dd->character))
	continue;
      sprintf (buf, "\x1B[1;32m>\x1B[34m>\x1B[31m>\x1B[37m> \x1B[36mNotify:\x1B[0;36m %s\x1B[37;0m", txt);
      send_to_char (buf, dd->character);
    }
#endif
  return;
}

void 
do_clan_notify (CHAR_DATA * ch, char *argy)
{
  int ii;
  char general_use[STD_LENGTH];
  CLAN_DATA *clan;
  DEFINE_COMMAND("announce",do_clan_notify,POSITION_SLEEPING,0,LOG_NORMAL,"Allows the clan leader to announce a message to all clan members online.")
  ii = clan_number (ch);
  if (ii < 1)
    {
      send_to_char ("You aren't even IN a clan!\n\r", ch);
      return;
    }
  clan = get_clan_index (ii);
  if (str_cmp (RNAME (ch), clan->leader))
    {
      send_to_char ("Only the clan leader can globally notify the clan.\n\r", ch);
      return;
    }
  strcpy (general_use, argy);
  strcat (general_use, " - (ClanLeader)");
  clan_notify (general_use, ii);
  return;
}

void 
clan_notify (char *argy, int clan_num)	/*By Owen Emlen */
{
  char buf[STD_LENGTH];
  DESCRIPTOR_DATA *d;
  sprintf (buf, "\x1B[37;0m*\x1B[1mClan Notify: %s\x1B[0m\n\r", argy);
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *och;
      CHAR_DATA *vch;
      och = d->original ? d->original : d->character;
      vch = d->character;
      if (((d->connected == CON_PLAYING
	    || d->connected == CON_AEDITOR
	    || d->connected == CON_OEDITOR
	    || d->connected == CON_CEDITOR
	    || d->connected == CON_MEDITOR
	    || d->connected == CON_REDITOR)))
	{
	  if (!check_clan (get_clan_index (clan_num), RNAME (vch)))
	    continue;
	  send_to_char (buf, vch);
	}
    }
  return;
}

void 
group_notify (char *argy, CHAR_DATA * ch)	/*By Owen Emlen */
{
  char buf[STD_LENGTH];
  DESCRIPTOR_DATA *d;
  sprintf (buf, "\x1B[37;0m++\x1B[1m[Group]: %s\x1B[0m\n\r", argy);
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *och;
      CHAR_DATA *vch;
      och = d->original ? d->original : d->character;
      vch = d->character;
      if (vch == NULL || och == NULL)
	continue;
      if (((d->connected == CON_PLAYING
	    || d->connected == CON_AEDITOR
	    || d->connected == CON_OEDITOR
	    || d->connected == CON_CEDITOR
	    || d->connected == CON_MEDITOR
	    || d->connected == CON_REDITOR)))
	{
	  if (!is_same_group (vch, ch))
	    continue;
	  send_to_char (buf, vch);
	}
    }
  return;
}

void 
do_gen_gos (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("yell", do_gen_gos, POSITION_RESTING, 0, LOG_NORMAL, "This command allows you to yell to the area around you.")

    do_yell (ch, argy);
  return;
}

void 
do_yell (CHAR_DATA * ch, char *argy)
{
  char buffr[500];
  char buf[1000];
  yellbuff[0] = '\0';
  if (!can_yell (ch))
    return;
  if (IS_PLAYER(ch) && IS_SET(ch->pcdata->act2, PLR_SILENCE)) return;
  if (argy == "" || argy[0] == '\0' || !argy)
    return;
  if (strlen (argy) > 500)
    return;
  strcpy (buf, argy);
  strcpy (yellbuff, argy);
  sprintf (buffr, "%sYou yell, '", "\x1B[1;31m");
  send_to_char (buffr, ch);
  send_to_char (buf, ch);
  send_to_char ("'\x1B[37;0m\n\r", ch);
  
  find_people_n (ch, 25);
  
  return;
}

void 
do_say2 (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("'", do_say, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to say something to the room you are in.")
    do_say (ch, argy);
  return;
}

void 
do_say (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  CHAR_DATA *mob;
  char *scn;
  char *verb;
  char conv[STD_LENGTH];
  char new[STD_LENGTH * 2];
  DEFINE_COMMAND ("say", do_say, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to say something to the room you are in.")

    for (scn=argy; *scn!='\0'; scn++) {
		if (*scn=='$') return;
	  }

  for (scn = argy; *scn != '\0'; scn++)
    {
      if (*scn == '%')
	*scn = '.';
      if (*scn == '$')
	*scn = ' ';
    }

    new[0] = '\0';
  strcpy (new, argy);
  if (IS_MOB (ch) && ch->position == POSITION_SLEEPING)
    return;

  
  
  if (IS_SET (ch->in_room->room_flags, ROOM_SILENCE))
    {
      send_to_char ("Your voice is swallowed by the thick air all around you.\n\r", ch);
      return;
    }
  if (argy[0] == '\0')
    {
      send_to_char ("Syntax: \x1B[1;37msay\x1B[0;37m <\x1B[1;36mtext\x1B[0;37m>\n\r", ch);
      send_to_char ("\n\r <\x1B[1;36mtext\x1B[0;37m> is what you wish to say.\n\r", ch);
      return;
    }
  switch (argy[strlen (argy) - 1])
    {
    default:
      verb = "say";
      break;
    case '!':
      verb = "exclaim";
      break;
    case '?':
      verb = "ask";
      break;
    case ')':
      if (strlen (argy) >= 2 && argy[strlen (argy) - 2] == ':')
	verb = "grin";
      else
	verb = "say";
      break;
    case '(':
      if (strlen (argy) >= 2 && argy[strlen (argy) - 2] == ':')
	verb = "frown";
      else
	verb = "say";
      break;
    }
  for (scn = new; *scn != '\0'; scn++)
    {
      if (*scn == '%')
	*scn = '.';
      if (*scn == '$')
	*scn = ' ';
    }
  
  sprintf (conv, new);
  sprintf (buf, "%sYou %s, '%s'$R", /* $t */
	   (IS_MOB (ch) ? "\x1B[1;37m" : color_table[ch->pcdata->colors[COLOR_SAY]].code), verb,
	    conv);
  if (ch->desc != NULL)
    {
      act (buf, ch, conv, NULL, TO_CHAR);
    }

  if (IS_PLAYER (ch) && strlen (conv) > 8)
    {
      sprintf (buf, "%s says (%d): %s\n", NAME (ch), ch->in_room->vnum, conv);
     
    }
again_4:
/* Said something, check for triggers on the room! */
  {
    SINGLE_TRIGGER *tr;
    SCRIPT_INFO *s;
    for (tr = trigger_list[TSAYS]; tr != NULL; tr = tr->next)
      {
	if (ch->in_room->vnum == tr->attached_to_room)
	  {
	    if (tr->running_info && !tr->interrupted)
	      continue;		/* Already running, interrupted, but script says not to allow interruptions. */
	    if (tr->keywords[0] != '\0' && !one_is_of_two (argy, tr->keywords))
	      continue;
	    if (tr->players_only && IS_MOB (ch))
	      continue;
	    if (tr->running_info && tr->interrupted != 2)
	      {
		end_script (tr->running_info);
		goto again_4;
	      }
	    /* ----------------- */
	    /* Start the script! */
	    /* ----------------- */
	    tr->running_info = mem_alloc (sizeof (*tr->running_info));
	    s = tr->running_info;
	    bzero (s, sizeof (*s));
	    s->current = ch;
	    s->room = ch->in_room;
	    strcpy (s->code_seg, tr->code_label);
	    s->current_line = 0;
	    s->called_by = tr;
	    s->next = info_list;
	    info_list = s;
	    execute_code (s);
	    /* ----------------- */
	  }
      }
  }
/* End trigger check! */



  check_room_more (ch->in_room);
  for (mob = ch->in_room->more->people; mob != NULL; mob = mob->next_in_room)
    {
      sprintf (buf, "%s$N %ss, '%s'$R", /* $t */
	       (IS_MOB (mob) ? "\x1B[1;37m" : color_table[mob->pcdata->colors[COLOR_SAY]].code), 
		verb,argy );
      if( IS_PLAYER(mob) && IS_PLAYER(ch) && ignore(ch, mob)) continue;
      if (IS_PLAYER (ch) && IS_MOB (mob) && MASTER (mob) == ch)
	{
	  if (!str_infix ("rest", new))
	    {
	      do_rest (mob, "");
	    }
	  if (!str_infix ("stand", new))
	    {
	      do_stand (mob, "");
	    }
	}
      if (IS_MOB (ch)
	  && IS_MOB (mob)
	  && mob == ch)
	continue;
      if (IS_MOB (ch))
	{
	  switch (ch->pIndexData->mobtype)
	    {
	    case MOB_CANINE:
	      sprintf (buf, "Bark! Bark! Bow! Wow! Bark!");
	      break;
	    case MOB_RODENT:
	      sprintf (buf, "Eek! Squeek! Squeek! Tsst!");
	      break;
	    case MOB_INSECT:
	      sprintf (buf, "Click! Click! Tak! Clack!");
	      break;
	    case MOB_FISH:
	    case MOB_ARACHNID:
	    case MOB_GENERAL_ANIMAL:
	      buf[0] = '\0';
	      break;
	    case MOB_FELINE:
	      sprintf (buf, "Meow! Hiss! Meow! Meow!");
	      break;
	    case MOB_HORSE:
	      sprintf (buf, "Neigh Neigh Neigh!");
	      break;
	    }
	}
      if (mob != ch && buf[0] != '\0')
	{
	  act (buf, mob, argy, ch, TO_CHAR);
	}
    again_5:
/* Said something, check for triggers on mobbies! */
      {
	SINGLE_TRIGGER *tr;
	SCRIPT_INFO *s;
	for (tr = trigger_list[TSAYS]; tr != NULL; tr = tr->next)
	  {
	    if (IS_MOB (mob) && mob->pIndexData->vnum == tr->attached_to_mob)
	      {
		if (tr->running_info && !tr->interrupted)
		  continue;	/* Already running, interrupted, but script says not to allow interruptions. */
		if (tr->keywords[0] != '\0' && !one_is_of_two (argy, tr->keywords))
		  continue;
		if (tr->running_info && tr->interrupted != 2)
		  {
		    end_script (tr->running_info);
		    goto again_5;
		  }
		/* ----------------- */
		/* Start the script! */
		/* ----------------- */
		tr->running_info = mem_alloc (sizeof (*tr->running_info));
		s = tr->running_info;
		bzero (s, sizeof (*s));
		s->current = ch;
		s->mob = mob;
		strcpy (s->code_seg, tr->code_label);
		s->current_line = 0;
		s->called_by = tr;
		s->next = info_list;
		info_list = s;
		execute_code (s);
		/* ----------------- */
	      }
	  }
      }
/* End trigger check! */

    }
  return;
}

void 
do_tell (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  char buf[STD_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *swp;
  char *t;
  bool is_d = FALSE;
  int position;
  char name[100];
  DEFINE_COMMAND ("tell", do_tell, POSITION_DEAD, 0, LOG_NORMAL, "Allows you to send a message to the specified person.")

/*#ifdef NEW_WORLD*/
    /* ------------------------------------------------------------------------ */
    /* if not arguments, then list back the last ch->pcdata->tell_counter tells */
    /* ------------------------------------------------------------------------ */
    if (argy[0] == '\0' && IS_PLAYER (ch))
    {
      int i;
      int j;
      j = ch->position;
      ch->position = POSITION_STANDING;
      for (i = ch->pcdata->tell_counter + 1; i != ch->pcdata->tell_counter; i++)
	{
	  if (i == 15)
	    {
	      i = -1;
	      continue;
	    }
	  if (i == -1)
	    continue;
	  if (ch->pcdata->last_tells[i])
	    act (ch->pcdata->last_tells[i], ch, NULL, ch, TO_CHAR);
	}
      ch->position = j;
      return;
    }
/*#endif*/

  if (IS_SET (ch->in_room->room_flags, ROOM_SILENCE))
    {
      send_to_char ("Your voice is silenced by the thick air around you.\n\r", ch);
      return;
    }
  if (IS_PLAYER (ch) && ch->pcdata->quiet)
    {
      send_to_char ("You cannot tell in quiet mode.\n\r", ch);
      return;
    }
  argy = one_argy (argy, arg);
  for (t = argy; *t != '\0'; t++)
    {
      if (*t == '$')
	{
	  if (!pow.morts_use_color_chat && LEVEL (ch) < IMM_LEVEL)
	    {
	      send_to_char ("You can not use $ in a channel.\n\r", ch);
	      return;
	    }
	  is_d = TRUE;
	  continue;
	}
      if (is_d && (*t < '0' || *t > '9') && *t != 'R' && *t != 'B')
	{
	  send_to_char ("Illegal value in a channel.\n\r", ch);
	  return;
	}
      is_d = FALSE;
    }
  if (arg[0] == '\0' || argy[0] == '\0')
    {
      send_to_char ("Tell whom what?\n\r", ch);
      return;
    }

  if ((victim = get_char_world (ch, arg)) == NULL
      || (IS_MOB (victim) && victim->in_room != ch->in_room) ||
      (DIFF_ALIGN(ch, victim) && LEVEL (victim) < IMM_LEVEL && LEVEL (ch) < IMM_LEVEL))
    {
      send_to_char ("You sure you got that name right?\n\r", ch);
      return;
    }
  if (victim->timer > 49)
    {
      send_to_char ("Your recipient is currently away from the keyboard.\n\r", ch);
      return;
    }
  if (ignore (ch, victim))
    return;
  if (IS_PLAYER (victim) && victim->pcdata->quiet == 2)
    {
      send_to_char ("Sorry, I'm busy... Please try later if it's important.\n\r", ch);
      return;
    }
  if (IS_PLAYER (victim) && victim->desc == NULL)
    {
      send_to_char ("Your recipient is linkless. Please try later...\n\r", ch);
      return;
    }
  if (LEVEL (ch) > IMM_LEVEL && LEVEL (ch) < MAX_LEVEL && LEVEL (victim) < IMM_LEVEL)
    {
      send_to_char ("Builders cannot send tells to mortals.\n\r", ch);
      return;
    }
  sprintf (buf, "%sYou tell $N '%s'$R",  
	   (IS_MOB (ch) ? "$7" : color_table[ch->pcdata->colors[COLOR_TELL]].act_code),
	   argy);
  position = ch->position;
  ch->position = POSITION_STANDING;
  act (buf, ch, argy, victim, TO_CHAR);
  ch->position = position;
  if (IS_PLAYER (ch) && IS_PLAYER (victim) && strlen (argy) > 7)
    {
      sprintf (buf, "%s tells %s: %s\n", NAME (ch), NAME (victim), argy);
     
    }
  strcpy (name, TPERS (ch, victim));
  name[0] = UPPER (name[0]);
  sprintf (buf, "\x1B[1;36m-->%s %s tells you '%s'$R",
	   (IS_MOB (victim) ? "$4$B" : color_table[victim->pcdata->colors[COLOR_TELL]].act_code), name, argy);
  position = victim->position;
  swp = ch;
  ch = victim;
  victim = swp;

/*#ifdef NEW_WORLD*/
  if (IS_PLAYER (ch))
    {
      if (ch->pcdata->last_tells[ch->pcdata->tell_counter])
	{
	  free (ch->pcdata->last_tells[ch->pcdata->tell_counter]);
	}
      ch->pcdata->last_tells[ch->pcdata->tell_counter] = malloc (strlen (buf) + 1);
      strcpy (ch->pcdata->last_tells[ch->pcdata->tell_counter], buf);
      ch->pcdata->tell_counter++;
      if (ch->pcdata->tell_counter > 14)
	ch->pcdata->tell_counter = 0;
    }
/*#endif*/
  swp = ch;
  ch = victim;
  victim = swp;

  victim->position = POSITION_STANDING;
  act (buf, victim, argy, ch, TO_CHAR);
  victim->position = position;
again_6:
/* Said something, check for triggers on the room! */
  if (IS_MOB (victim))
    {
      SINGLE_TRIGGER *tr;
      SCRIPT_INFO *s;
      for (tr = trigger_list[TTELLS]; tr != NULL; tr = tr->next)
	{
	  if (victim->pIndexData->vnum == tr->attached_to_mob)
	    {
	      if (tr->running_info && !tr->interrupted)
		continue;	/* Already running, interrupted, but script says not to allow interruptions. */
	      if (tr->keywords[0] != '\0' && !one_is_of_two (argy, tr->keywords))
		continue;
	      if (tr->running_info && tr->interrupted != 2)
		{
		  end_script (tr->running_info);
		  goto again_6;
		}
	      /* ----------------- */
	      /* Start the script! */
	      /* ----------------- */
	      tr->running_info = mem_alloc (sizeof (*tr->running_info));
	      s = tr->running_info;
	      bzero (s, sizeof (*s));
	      s->current = ch;
	      s->mob = victim;
	      strcpy (s->code_seg, tr->code_label);
	      s->current_line = 0;
	      s->called_by = tr;
	      s->next = info_list;
	      info_list = s;
	      execute_code (s);
	      /* ----------------- */
	    }
	}
    }
/* End trigger check! */




  check_ced (victim);
  victim->ced->reply = ch;
  if (IS_PLAYER (ch) && IS_MOB (victim) && MASTER (victim) == ch)
    {
      if (!str_infix ("stand", argy))
	{
	  do_stand (victim, "");
	}
      if (!str_infix ("rest", argy))
	{
	  do_rest (victim, "");
	}
    }
  return;
}

void 
do_reply (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *victim;
  CHAR_DATA *swp;
  char *t;
  char buf[STD_LENGTH];
  int position;
  char name[100];
  DEFINE_COMMAND ("reply", do_reply, POSITION_DEAD, 0, LOG_NORMAL, "This command allows you to respond to the person who sent you your last tell.")

 for (t = argy; *t != '\0'; t++)
    {
      if (*t == '$')
	{
	  send_to_char ("You can not use $ in reply.\n\r", ch);
	  return;
	}
 }

  check_ced (ch);
  if ((victim = ch->ced->reply) == NULL || (DIFF_ALIGN(ch, victim) &&
LEVEL(ch) < IMM_LEVEL && LEVEL(victim) < IMM_LEVEL))
    {
      send_to_char ("They aren't here.\n\r", ch);
      return;
    }
  if (victim->timer > 49)
    {
      send_to_char ("Your recipient is currently away from the keyboard.\n\r", ch);
      return;
    }
  if (ignore (ch, victim))
    return;
  if (IS_PLAYER (victim) && victim->pcdata->quiet == 2)
    {
      send_to_char ("Sorry, I'm busy... Please try later if it's important.\n\r", ch);
      return;
    }
  sprintf (buf, "%sYou tell $N '%s'$R", /* $t */
	   (IS_MOB (ch) ? "$7" : color_table[ch->pcdata->colors[COLOR_TELL]].act_code),
		argy);
  position = ch->position;
  ch->position = POSITION_STANDING;
  act (buf, ch, argy, victim, TO_CHAR);
  ch->position = position;
  position = victim->position;
  if (strlen (argy) > 7)
    {
      sprintf (buf, "%s repl to %s: %s\n", NAME (ch), NAME (victim), argy);
    
    }
  victim->position = POSITION_STANDING;
  strcpy (name, TPERS (ch, victim));
  name[0] = UPPER (name[0]);
  sprintf (buf, "\x1B[1;36m-->%s %s tells you '%s'$R",
	   "$B$2", name, argy);
  swp = ch;
  ch = victim;
  victim = swp;
  if (IS_PLAYER (ch))
    {
      if (ch->pcdata->last_tells[ch->pcdata->tell_counter])
	{
	  free (ch->pcdata->last_tells[ch->pcdata->tell_counter]);
	}
      ch->pcdata->last_tells[ch->pcdata->tell_counter] = malloc (strlen (buf) + 1);
      strcpy (ch->pcdata->last_tells[ch->pcdata->tell_counter], buf);
      ch->pcdata->tell_counter++;
      if (ch->pcdata->tell_counter > 14)
	ch->pcdata->tell_counter = 0;
    }
  swp = ch;
  ch = victim;
  victim = swp;
  check_ced (victim);
  act (buf, victim, argy, ch, TO_CHAR);
  victim->position = position;
  victim->ced->reply = ch;
  return;
}

void 
do_quiet (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("quiet", do_quiet, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command allows you to enter quiet mode.")

    if (IS_MOB (ch))
    return;
  if (!ch->pcdata->quiet)
    {
      send_to_char ("Entering [Quiet] Mode. . .\n\r", ch);
      ch->pcdata->quiet = 1;
    }
  else
    {
      send_to_char ("You turn off quiet and prepare for the scroll about to hit you.\n\r", ch);
      ch->pcdata->quiet = 0;
    }
  return;
}

void 
do_emote2 (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND (":", do_emote, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to emote something to the room you are in.")
    do_emote (ch, argy);
  return;
}


void 
do_emote (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char buff[STD_LENGTH];
  char *plast;
  CHAR_DATA *rch;
  DEFINE_COMMAND ("emote", do_emote, POSITION_RESTING, 0, LOG_NORMAL, "Allows you to emote something to the room you are in.")


    if (!ch->in_room) return;
    check_room_more(ch->in_room);
    if (IS_SET (ch->in_room->room_flags, ROOM_SILENCE))
    {
      send_to_char ("The thick air surrounds you, and you can't think straight.\n\r", ch);
      return;
    }
  if (argy[0] == '\0')
    {
      send_to_char ("Emote what?\n\r", ch);
      return;
    }
  for (plast = argy; *plast != '\0'; plast++);
  strcpy (buf, argy);
  if (isalpha (plast[-1]))
    strcat (buf, ".");
  for (rch = ch->in_room->more->people; rch != NULL; rch = rch->next_in_room)
    {
      if (!ignore(ch, rch))
	{
	  sprintf(buff, "%s %s", rNAME(ch, rch), buf);
	  send_to_char(buff, rch);
	}
    }
  return;
}

void 
do_gtell2 (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND (";", do_gtell, POSITION_DEAD, 0, LOG_NORMAL, "See gtell.")
    return;
}

void 
do_gtell (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH * 5];
  CHAR_DATA *gch;
  char *scn;
  char new[SML_LENGTH * 2];
  int oldpos;
  DEFINE_COMMAND ("gtell", do_gtell, POSITION_DEAD, 0, LOG_NORMAL, "This command sends a message to everyone in your current group.")

  /* --------------------------------------------- */
  /* If no argument then list the last group tells */
  /* --------------------------------------------- */
  if (argy[0] == '\0' && IS_PLAYER (ch))
  {
    int i;
    int j;
    int x;
    j = ch->position;
    ch->position = POSITION_STANDING;

   /* Step through each index, displaying each one   */
   /* gtell_counter : stores number of gtells stored */
   /* last_gtells   : index storing gtells	     */
    for (i = ch->pcdata->gtell_counter + 1; i != ch->pcdata->gtell_counter; i++)
    {
       if (i == 15)
       {
         i = -1;
         continue;
       }
       if (i == -1)
	 continue;
       if (ch->pcdata->last_gtells[i])
	act (ch->pcdata->last_gtells[i], ch, NULL, ch, TO_CHAR);
    }
      ch->position = j;
      return;
  }

  strcpy (new, argy);
  for (scn = new; *scn != '\0'; scn++)
    {
      if (*scn == '%')
	*scn = '.';
      if (*scn == '$')
	*scn = ' ';
    }
  sprintf (buf, "%sYou tell the group '%s'\x1B[37;0m",
	   (IS_MOB (ch) ? "\x1B[37;0m" : color_table[ch->pcdata->colors[COLOR_TELL]].code),
	   new);

  oldpos = ch->position;
  ch->position = POSITION_STANDING;
  act (buf, ch, NULL, NULL, TO_CHAR);
  ch->position = oldpos;
  if (strlen (new) > 5)
    sprintf (buf, "%s grouptells: %s\n", NAME (ch), new);

  for (gch = char_list; gch != NULL; gch = gch->next)
    {
      int position;
      sprintf (buf, "%s%s tells the group '%s'\x1B[37;0m",
	       (IS_MOB (gch) ? "\x1B[37;0m" : color_table[gch->pcdata->colors[COLOR_TELL]].code),
	       NAME (ch), new);
      position = gch->position;
      gch->position = POSITION_STANDING;
      if (is_same_group (gch, ch) && gch != ch)
	{
	  act (buf, gch, NULL, NULL, TO_CHAR);

	  /* ----------------------------------------------------- */
	  /* Store gtell in list for later recall for all in group */
	  /*  But this doesnt store it for the person that gt-ed   */
	  /* ----------------------------------------------------- */

	  /* Format the buf correctly ---------------------------- */
	  sprintf ( buf, "%s%10s > %s",
	       (IS_MOB (gch) ? "\x1B[37;0m" : color_table[gch->pcdata->colors[COLOR_TELL]].code),
		NAME (ch),
		new);

	  /* Now, store buffer ----------------------------------- */
	  if (IS_PLAYER (gch))
	  {
	    if (gch->pcdata->last_gtells[gch->pcdata->gtell_counter])
              free (gch->pcdata->last_gtells[gch->pcdata->gtell_counter]);
      	    gch->pcdata->last_gtells[gch->pcdata->gtell_counter] = malloc (strlen (buf) + 1);
	    strcpy (gch->pcdata->last_gtells[gch->pcdata->gtell_counter], buf);
      	    gch->pcdata->gtell_counter++;
	    if (gch->pcdata->gtell_counter > 14)
	      gch->pcdata->gtell_counter = 0;
	  }
	}
      gch->position = position;
    }

  /* ------------------------------------ */
  /* Store gtell for person that said it  */
  /* ------------------------------------ */
	
  /* Format the buf correctly ---------------------------- */
  sprintf ( buf, "%s%10s > %s",
	(IS_MOB (ch) ? "\x1B[37;0m" : color_table[ch->pcdata->colors[COLOR_TELL]].code),
	NAME(ch),
	new);

  /* Now, store buffer ----------------------------------- */
  if (IS_PLAYER (ch))
    {
      if (ch->pcdata->last_gtells[ch->pcdata->gtell_counter])
	{
	  free (ch->pcdata->last_gtells[ch->pcdata->gtell_counter]);
	}
      ch->pcdata->last_gtells[ch->pcdata->gtell_counter] = malloc (strlen (buf) + 1);
      strcpy (ch->pcdata->last_gtells[ch->pcdata->gtell_counter], buf);
      ch->pcdata->gtell_counter++;
      if (ch->pcdata->gtell_counter > 14)
	ch->pcdata->gtell_counter = 0;
    }

  return;
}

