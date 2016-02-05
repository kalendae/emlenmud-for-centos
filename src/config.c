
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

extern char dontecho[];
extern char doecho[];
extern char echo_off_str[];
extern char echo_on_str[];
extern char linemode_off[];
extern char linemode_on[];
char *disabled_commands[MAX_DISABLED_COMMANDS];

void
do_beep (CHAR_DATA * ch, char *argy)
{
  CHAR_DATA *vict;
  char beep_string[500];
  DEFINE_COMMAND ("beep", do_beep, POSITION_DEAD, IMM_LEVEL, LOG_NORMAL, "This command sends a beep from one immort to a player/other immort.")

    if ((vict = get_char_world (ch, argy)) == NULL)
    {
      send_to_char ("Player not found.\n\r", ch);
      return;
    }
  if (IS_MOB (vict))
    return;
  sprintf (beep_string, "%c%c%c%c%cBeep sent from %s to %s!\n\r", 7, 7, 7, 7, 7, NAME (ch), NAME (vict));
  send_to_char (beep_string, vict);
  send_to_char (beep_string, ch);
  return;
}


void
do_blank (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("blank", do_blank, POSITION_DEAD, 0, LOG_NORMAL, "This command toggles whether or not you have a blank line before your prompt.")
    if (!ch)
    return;
  if (IS_SET (ch->pcdata->act2, PLR_BLANK))
    do_config (ch, "-blank");
  else
    do_config (ch, "+blank");
  return;
}

void
do_channels (CHAR_DATA * ch, char *argy)
{
  char arg[1024];
  DEFINE_COMMAND ("channels", do_channels, POSITION_DEAD, 0, LOG_NORMAL, "Allows you to toggle yell and auction channels.")

    one_argy (argy, arg);

  if (arg[0] == '\0')
    {
      if (IS_PLAYER (ch) && IS_SET (ch->act, PLR_SILENCE))
        {
          send_to_char ("You are silenced.\n\r", ch);
          return;
        }

      send_to_char ("\x1B[1;33m\n\rChannels:\x1B[37;0m\n\r\n\r", ch);


      send_to_char (!IS_SET (ch->pcdata->deaf, CHANNEL_AUCTION)
                    ? "\x1B[1;37m*\x1B[0mAUCTION  - You are listening to the auction channel.\n\r"
                  : " auction  - You are ignoring the auction channel.\n\r",
                    ch);
      send_to_char (!IS_SET (ch->pcdata->deaf, CHANNEL_YELL)
            ? "\x1B[1;37m*\x1B[0mYELL     - You are listening to yells.\n\r"
                    : " yell     - You are ignoring yells.\n\r",
                    ch);
      send_to_char ("\x1B[37;0mTo turn a specific channel on/off, type the channel name alone.\n\r", ch);

    }
  else
    {
      bool fClear;
      int bit;

      if (arg[0] == '+')
        fClear = TRUE;
      else if (arg[0] == '-')
        fClear = FALSE;
      else
        {
          send_to_char ("Channels -channel or +channel?\n\r", ch);
          return;
        }

      if (!str_cmp (arg + 1, "auction"))
        bit = CHANNEL_AUCTION;
      else if (!str_cmp (arg + 1, "yell"))
        bit = CHANNEL_YELL;
      else
        {
          send_to_char ("Set or clear which channel?\n\r", ch);
          return;
        }

      if (fClear)
        {
          char buf[50];
          REMOVE_BIT (ch->pcdata->deaf, bit);
          sprintf (buf, "You now are aware of %ss.\n\r", arg + 1);
          send_to_char (buf, ch);
        }
      else
        {
          char buf[50];
          SET_BIT (ch->pcdata->deaf, bit);
          sprintf (buf, "You now ignore %ss.\n\r", arg + 1);
          send_to_char (buf, ch);
        }
    }

  return;
}



void
display_toggles (CHAR_DATA * ch)
{
  send_to_char ("Groundfighting Options:\n\r", ch);
  check_ced (ch);
  if (IS_SET (ch->ced->fight_ops, F_JUJI))
    send_to_char ("\x1B[1;30m[\x1B[34mJOINTLOCK \x1B[30m]\x1B[1;37m You WILL attempt wrist-bends and arm hyperextensions.\n\r\x1B[0m", ch);
  else
    send_to_char ("\x1B[1;30m[jointlock ] \x1B[37;0m You WON'T attempt wrist-bends and arm hyperextensions.\n\r\x1B[37;0m", ch);
  if (IS_SET (ch->ced->fight_ops, F_CHOKE))
    send_to_char ("\x1B[1;30m[\x1B[34m  CHOKE   \x1B[30m]\x1B[1;37m You WILL attempt to choke your victim.\n\r\x1B[0m", ch);
  else
    send_to_char ("\x1B[1;30m[  choke   ] \x1B[37;0mYou WON'T attempt to choke your victim.\n\r\x1B[37;0m", ch);
  if (IS_SET (ch->ced->fight_ops, F_BITE))
    send_to_char ("\x1B[1;30m[\x1B[34m   BITE   \x1B[30m]\x1B[1;37m You WILL attempt to bite your victim.\n\r\x1B[0m", ch);
  else
    send_to_char ("\x1B[1;30m[   bite   ] \x1B[37;0mYou WON'T attempt to bite your victim.\n\r\x1B[37;0m", ch);
  if (IS_SET (ch->ced->fight_ops, F_ELBOW))
    send_to_char ("\x1B[1;30m[\x1B[34m  ELBOW   \x1B[30m]\x1B[1;37m You WILL use your elbows.\n\r\x1B[0m", ch);
  else
    send_to_char ("\x1B[1;30m[  elbow   ] \x1B[37;0mYou WON'T use your elbows.\n\r\x1B[37;0m", ch);
  if (IS_SET (ch->ced->fight_ops, F_KNEE))
    send_to_char ("\x1B[1;30m[\x1B[34m   KNEE   \x1B[30m]\x1B[1;37m You WILL use your knees.\n\r\x1B[0m", ch);
  else
    send_to_char ("\x1B[1;30m[   knee   ] \x1B[37;0mYou WON'T use your knees.\n\r\x1B[37;0m", ch);
  if (IS_SET (ch->ced->fight_ops, F_HEADBUTT))
    send_to_char ("\x1B[1;30m[\x1B[34m HEADBUTT \x1B[30m]\x1B[1;37m You WILL headbutt.\n\r\x1B[0m", ch);
  else
    send_to_char ("\x1B[1;30m[ headbutt ] \x1B[37;0mYou WON'T headbutt.\n\r\x1B[37;0m", ch);
  send_to_char ("\n\rNormal Combat Options:\n\r", ch);
  if (IS_SET (ch->ced->fight_ops, F_POWER))
    send_to_char ("\x1B[1;30m[\x1B[34m  POWER   \x1B[30m]\x1B[1;37m You concentrate your efforts on power.\n\r\x1B[0m", ch);
  else
    send_to_char ("\x1B[1;30m[  power   ] \x1B[37;0mYou concentrate your efforts on speed.\n\r\x1B[37;0m", ch);
  if (IS_SET (ch->ced->fight_ops, F_AGGRESSIVE))
    send_to_char ("\x1B[1;30m[\x1B[34mAGGRESSIVE\x1B[30m]\x1B[1;37m You will fight aggressively.\n\r\x1B[0m", ch);
  else
    send_to_char ("\x1B[1;30m[aggressive] \x1B[37;0mYou will fight defensively.\n\r\x1B[37;0m", ch);
  if (IS_SET (ch->ced->fight_ops, F_GROUND))
    send_to_char ("\x1B[1;30m[\x1B[34m  GROUND  \x1B[30m]\x1B[1;37m You are indifferent to fighting on the ground.\n\r\x1B[0m", ch);
  else
    send_to_char ("\x1B[1;30m[  ground  ] \x1B[37;0mYou will try to avoid groundfighting at all costs.\x1B[37;0m\n\r", ch);
  return;
}

void
do_fight (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("fight", do_fight, POSITION_SLEEPING, 0, LOG_NORMAL, "This command allows you to view/change your fighting options.")

    check_ced (ch);
  if (argy[0] == '\0')
    {
      send_to_char ("NOTE: To toggle fight options, type FIGHT <option>\n\r\n\r", ch);
      display_toggles (ch);
      return;
    }
  if (!str_cmp (argy, "jointlock"))
    ch->ced->fight_ops ^= F_JUJI;
  else if (!str_cmp (argy, "headbutt"))
    ch->ced->fight_ops ^= F_HEADBUTT;
  else if (!str_cmp (argy, "elbow"))
    ch->ced->fight_ops ^= F_ELBOW;
  else if (!str_cmp (argy, "knee"))
    ch->ced->fight_ops ^= F_KNEE;
  else if (!str_cmp (argy, "bite"))
    ch->ced->fight_ops ^= F_BITE;
  else if (!str_cmp (argy, "choke"))
    ch->ced->fight_ops ^= F_CHOKE;
  else 
    if (!str_cmp (argy, "aggressive"))
      {
	if (IS_SET(ch->ced->fight_ops, F_AGGRESSIVE))
	  ch->damroll -=2;
	else
	  ch->damroll +=2;
	ch->ced->fight_ops ^= F_AGGRESSIVE;
      }
  else if (!str_cmp (argy, "power"))
    {
      if (IS_SET(ch->ced->fight_ops, F_POWER))
	ch->damroll -=1;
      else
	ch->damroll +=1;
      ch->ced->fight_ops ^= F_POWER;
    }
  else if (!str_cmp (argy, "ground"))
    ch->ced->fight_ops ^= F_GROUND;
  else
    {
      send_to_char ("Invalid fight option.\n\r", ch);
      return;
    }
  send_to_char ("Fight option toggled.\n\r", ch);
  return;
}

void
do_stance (CHAR_DATA * ch, char *argy)
{
  char buf3[STD_LENGTH];
  DEFINE_COMMAND ("stance", do_stance, POSITION_SLEEPING, 0, LOG_NORMAL, "This command allows you to view/change your monk fighting stance.")
  check_ced (ch);
  if (!is_member(ch, GUILD_MONK))
  {
    send_to_char("Yeah, you wish you were a monk.\n\r", ch);
    return;
  }
/*
  if (ch->pcdata->learned[gsn_stance1] < 68)
  {
    send_to_char("You need to train in stances at a master.\n\r", ch);
    return;
  }
*/
  if (argy[0] == '\0')
  {
    if (ch->pcdata->stance == 0)
      sprintf(buf3, "\x1B[1;36mYour current stance is \x1B[1;37mBasic.\n\r");
    if (ch->pcdata->stance == 1)
      sprintf(buf3, "\x1B[1;36mYour current stance is \x1B[1;37mViper.\n\r");
    if (ch->pcdata->stance == 2)
      sprintf(buf3, "\x1B[1;36mYour current stance is \x1B[1;37mMantis.\n\r");
    if (ch->pcdata->stance == 3)
      sprintf(buf3, "\x1B[1;36mYour current stance is \x1B[1;37mWolf.\n\r");
    if (ch->pcdata->stance == 4)
      sprintf(buf3, "\x1B[1;36mYour current stance is \x1B[1;37mTiger.\n\r");
    if (ch->pcdata->stance == 5)
      sprintf(buf3, "\x1B[1;36mYour current stance is \x1B[1;37mMongoose.\n\r");
    if (ch->pcdata->stance == 6)
      sprintf(buf3, "\x1B[1;36mYour current stance is \x1B[1;37mBear.\n\r");
    if (ch->pcdata->stance == 7)
      sprintf(buf3, "\x1B[1;36mYour current stance is \x1B[1;37mDragon.\n\r");
    if (ch->pcdata->stance == 8)
      sprintf(buf3, "\x1B[1;36mYour current stance is \x1B[1;37mCrane.\n\r");
    send_to_char(buf3, ch);
    return;
  }
  else
  {
    if (!strcmp(argy, "basic"))
      ch->pcdata->stance = 0; 
    else if (!strcmp(argy, "viper"))
      ch->pcdata->stance = 1; 
    else if (!strcmp(argy, "mantis"))
      ch->pcdata->stance = 2; 
    else if (!strcmp(argy, "wolf"))
      ch->pcdata->stance = 3; 
    else if (!strcmp(argy, "tiger")) /*&& ch->pcdata->race > 8)*/ //Evolve was taken out, gave these to non-ascended
      ch->pcdata->stance = 4;
    else if (!strcmp(argy, "mongoose")) /*&& ch->pcdata->race > 8)*/
      ch->pcdata->stance = 5;
    else if (!strcmp(argy, "bear")) /*&& ch->pcdata->race > 8)*/
      ch->pcdata->stance = 6;
    else if (!strcmp(argy, "dragon") && ch->pcdata->level > 79
             && ch->pcdata->race > 8)
      ch->pcdata->stance = 7;
    else if (!strcmp(argy, "crane") && ch->pcdata->level > 79
             && ch->pcdata->race > 8)
      ch->pcdata->stance = 8;
    else
    {
      send_to_char("Not a valid stance, try again.\n\r", ch);
      return;
    }
    send_to_char ("Stance changed.\n\r", ch);
    return;
  }
}

void
do_wimpy (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  int wimpy;
  DEFINE_COMMAND ("wimpy", do_wimpy, POSITION_DEAD, 0, LOG_NORMAL, "This command allows you to change your auto-flee below <amt> hps.")

    one_argy (argy, arg);
  check_ced (ch);
  if (arg[0] == '\0')
    wimpy = 20;
  else
    wimpy = atoi (arg);
  if (wimpy < 0)
    {
      send_to_char ("Invalid.\n\r", ch);
      return;
    }
  if (wimpy > (ch->max_hit / 3))
    {
      send_to_char ("Your wimpy must be less than 1/3 of your max hps!\n\r", ch);
      return;
    }
  ch->ced->wimpy = wimpy;
  sprintf (buf, "You decide to flee at \x1B[1m%d\x1B[0m hit points.\n\r", wimpy);
  send_to_char (buf, ch);
  return;
}

void
do_brief (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("brief", do_brief, POSITION_DEAD, 0, LOG_NORMAL, "This command toggles whether or not you see long room descriptions.")
    if (!ch)
    return;
  if (IS_SET (ch->pcdata->act2, PLR_BRIEF))
    do_config (ch, "-brief");
  else
    do_config (ch, "+brief");
  return;
}


void
do_pagelen (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  int lines;
  DEFINE_COMMAND ("pagelength", do_pagelen, POSITION_DEAD, 0, LOG_NORMAL, "This changes your default page length/screen size.")

    one_argy (argy, arg);
  if (arg[0] == '\0')
    lines = 24;
  else
    lines = atoi (arg);
  if (lines < 1)
    {
      send_to_char (
               "Negative or Zero values for a page pause is not legal.\n\r",
                     ch);
      return;
    }
  if (lines > 100)
    {
      send_to_char (
                     "Can't set page length greater than 100.\n\r",
                     ch);
      return;
    }
  ch->pcdata->pagelen = lines;
  do_clear (ch, "");
  sprintf (buf, "Page pause set to \x1B[1m%d\x1B[0m lines.\n\r", lines);
  send_to_char (buf, ch);
  return;
}


bool isDisabled(char *command)
{
  int i;

  /* look for the command in the list  */
  for(i = 0; i <= MAX_DISABLED_COMMANDS; ++i)
  {
    if(disabled_commands[i] != NULL)
    {
      if(!str_cmp(disabled_commands[i], command))
        return TRUE;
    }
  }
  
  return FALSE;
}


void do_disable (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char arg[SML_LENGTH];
  int i;

  DEFINE_COMMAND ("disable", do_disable, POSITION_DEAD, 0, LOG_NORMAL, "This command disables commands on the fly.")

  if(IS_PLAYER(ch))
  {
    if(!IS_IMMORTAL(ch))
    {
      send_to_char("huh?",ch);
      return;
    }
    
    if(argy[0] == '\0')
    {
      send_to_char(" -------------------- \x1B[3mDISABLED COMMANDS \x1B[0m--------------------\n\r",ch);
      for(i = 0; i <= MAX_DISABLED_COMMANDS; ++i)
      {
         if(disabled_commands[i] != NULL)
	 {
	   sprintf(buf,"         %s\n\r",disabled_commands[i]);
	   send_to_char(buf,ch);
	 }
      }	
      send_to_char("\r -----------------------------------------------------------\n\r",ch);
      return;
    }
  }
  else
   return;

  one_argy(argy,arg);

  /* look for the command in the list and enable it */
  for(i = 0; i <= MAX_DISABLED_COMMANDS; ++i)
  {
    if(disabled_commands[i] != NULL)
    {
      if(!str_cmp(disabled_commands[i], arg))
      {
	free(disabled_commands[i]);
        disabled_commands[i] = NULL;
        sprintf(buf, "\'%s\' has been enabled.\n\r", arg);
        send_to_char(buf,ch);
        return;
      }
    }
  }

  /* put command in list and disable it */
  for(i = 0; i <= MAX_DISABLED_COMMANDS; ++i)
  {
    if(disabled_commands[i] == NULL || !str_cmp(disabled_commands[i],""))
    {
      disabled_commands[i] = malloc(strlen(arg) + 1);
      strcpy(disabled_commands[i],arg);
      sprintf(buf,"\'%s\' has been disabled.\n\r",arg);
      send_to_char(buf,ch);
      return;
    }
  }

  return;
}


extern char *prompts[];

void
do_prompt (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("prompt", do_prompt, POSITION_DEAD, 0, LOG_NORMAL, "This command toggles your prompt on or off.")
    if (pow.player_prompt && !str_cmp (argy, "all"))
    {
      send_to_char ("Pick a default prompt by using 'prompt <letter>':\n\r", ch);
      send_to_char ("[A] ", ch);
      try_prompt (prompts[0], ch);
      send_to_char ("[B] ", ch);
      try_prompt (prompts[1], ch);
      send_to_char ("[C] ", ch);
      try_prompt (prompts[2], ch);
      send_to_char ("[D] ", ch);
      try_prompt (prompts[3], ch);
      send_to_char ("[E] ", ch);
      try_prompt (prompts[4], ch);
      send_to_char ("[F] ", ch);
      try_prompt (prompts[5], ch);
      send_to_char ("[G] ", ch);
      try_prompt (prompts[6], ch);
      send_to_char ("[H] ", ch);
      try_prompt (prompts[7], ch);
      send_to_char ("[I] ", ch);
      try_prompt (prompts[8], ch);
      send_to_char ("[J] ", ch);
      try_prompt (prompts[9], ch);
      send_to_char ("[K] ", ch);
      try_prompt (prompts[10], ch);
      send_to_char ("[L] ", ch);
      try_prompt (prompts[11], ch);
      send_to_char ("[M] ", ch);
      try_prompt (prompts[12], ch);
      send_to_char ("[N] ", ch);
      try_prompt (prompts[13], ch);
      send_to_char ("[O] ", ch);
      try_prompt (prompts[14], ch);
      send_to_char ("[P] ", ch);
      try_prompt (prompts[15], ch);
      send_to_char ("Or make your own prompt.  (see help prompt).\n\r", ch);
      return;
    }
  if (argy[0] != '\0' && argy[1] == '\0' && UPPER (argy[0]) >= 'A')
    {
      set_default_prompt (ch, argy[0]);
      return;
    }
  if (pow.player_prompt && argy[0] != '\0')
    {
      if (!check_pr (argy))
        {
          send_to_char ("Invalid prompt.  See help prompt for info.\n\r", ch);
          return;
        }
      if (strlen(argy) > 120)
        {     
          send_to_char("Your prompt is too long.\n\r", ch);
          return;
        }
      if (ch->pcdata->rprompt)
        free_string (ch->pcdata->rprompt);
      ch->pcdata->rprompt = str_dup (argy);
      send_to_char ("Prompt set.  See help prompt for info.\n\r", ch);
      return;
    }
  if (!IS_SET (ch->pcdata->act2, PLR_PROMPT))
    {
      send_to_char ("Prompt \x1B[1mON\x1B[0m.\n\r", ch);
      SET_BIT (ch->pcdata->act2, PLR_PROMPT);
    }
  else
    {
      REMOVE_BIT (ch->pcdata->act2, PLR_PROMPT);
      send_to_char ("Prompt \x1B[1;30mOFF\x1B[0m.\n\r", ch);
    }
  return;
}

void
do_auto (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("auto", do_auto, POSITION_DEAD, 0, LOG_NORMAL, "See Config.")

    do_config (ch, "");
  return;
}

void
do_autoloot (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("autoloot", do_autoloot, POSITION_DEAD, 0, LOG_NORMAL, "Toggles autoloot on/off.")

    send_to_char ("AUTOLOOT ", ch);
  do_config (ch, "autoloot");
  return;
}
void 
do_autoblink (CHAR_DATA *ch, char *argy)
{
   DEFINE_COMMAND ("autoblink", do_autoblink, POSITION_DEAD, 0, LOG_NORMAL, "Toggles autoblink on/off.")

    send_to_char ("AUTOBLINK ", ch);
  do_config (ch, "autoblink");
  return;
}
void
do_autoassist (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("autoassist", do_autoassist, POSITION_DEAD, 0, LOG_NORMAL, "Toggles autoassist on/off.")

    send_to_char ("AUTOASSIST ", ch);
  do_config (ch, "autoassist");
  return;
}

void
do_autoscan (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("autoscan", do_autoscan, POSITION_DEAD, 0, LOG_NORMAL, "Toggles autoscan on/off.")

    send_to_char ("AUTOSCAN ", ch);
  do_config (ch, "autoscan");
  return;
}

void
do_autosac (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("autosac", do_autosac, POSITION_DEAD, 0, LOG_NORMAL, "Toggles autosacrificing on/off.")

    send_to_char ("AUTOSAC ", ch);
  do_config (ch, "autosac");
  return;
}

void
do_autoexit (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("autoexit", do_autoexit, POSITION_DEAD, 0, LOG_NORMAL, "Toggles autoexit on/off.")

    send_to_char ("AUTOEXIT ", ch);
  do_config (ch, "autoexit");
  return;
}

void
do_autogold (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("autogold", do_autogold, POSITION_DEAD, 0, LOG_NORMAL, "Toggles automatic gold looting on/off.")

    send_to_char ("AUTOGOLD ", ch);
  do_config (ch, "autogold");
  return;
}

void
do_autosplit (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("autosplit", do_autosplit, POSITION_DEAD, 0, LOG_NORMAL, "Toggles automatic gold splitting on/off.")

    send_to_char ("AUTOSPLIT ", ch);
  do_config (ch, "autosplit");
  return;
}

void
do_assist (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  DEFINE_COMMAND ("assist", do_assist, POSITION_STANDING, 0, LOG_NORMAL, "Toggles autoassist on/off or assists a specified player in combat.")

    if (IS_PLAYER (ch) && argy[0] == '\0')
    {
      /* send_to_char( "ASSIST ", ch );
         do_config( ch, "assist" ); */
      send_to_char ("Assist who?\n\r", ch);
      return;
    }
  one_argy (argy, arg);
  if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("Assist WHO?\n\r", ch);
      return;
    }
  if (ch->position == POSITION_GROUNDFIGHTING)
    {
      act ("I don't think so! You are on the ground, fighting!", ch, NULL, victim, TO_CHAR);
      return;
    }
  if (ch->position != POSITION_STANDING)
    {
      act ("You probably should get into a standing position first...", ch, NULL, victim, TO_CHAR);
      return;
    }
  if (IS_PLAYER (ch) && IS_MOB (victim))
    {
      act ("$N doesn't want your help!", ch, NULL, victim, TO_CHAR);
      return;
    }
  if (victim == ch)
    {
      send_to_char ("Assist yourself? Assist yourself!\n\r", ch);
      return;
    }
  if (FIGHTING (victim) == NULL)
    {
      send_to_char ("To assist someone, they must be fighting...\n\r", ch);
      return;
    }
  if (FIGHTING (victim) == ch)
    {
      send_to_char ("It would be foolish to assist your opponent!\n\r", ch);
      return;
    }
 /* if (MASTER (victim) == ch && IS_PLAYER (ch))
    {
      if (IS_PLAYER (ch) && !IS_MOB (victim) && LEVEL (victim->fgt->fighting) + 10 < LEVEL (ch))
        {
          send_to_char ("A strange force prevents you from assisting!\n\r", ch);
          return;
        } 
    } */
  //ARC: No assisting different aligns!
  if (IS_PLAYER(victim) && IS_PLAYER (ch) && DIFF_ALIGN(ch, victim))
  {
	send_to_char ("You cannot assist that type of creature!", ch);
	return;
  }


  act ("You join $N's fight!", ch, NULL, victim, TO_CHAR);
  act ("$n assists you!", ch, NULL, victim, TO_VICT);
  act ("$n joins in the fight, assisting $N!", ch, NULL, victim, TO_NOTVICT);
  check_fgt (ch);
  check_fgt (victim);
  ch->fgt->fighting = victim->fgt->fighting;
  set_fighting (ch, victim->fgt->fighting);
  check_same_side_pk(ch, victim->fgt->fighting);
  return;
}

void
do_color (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("color", do_ansi, POSITION_DEAD, 0, LOG_NORMAL, "Configures channel colors.")

    return;
}

void
do_ansi (CHAR_DATA * ch, char *argy)
{
  CHANNEL *cn;
  char arg[SML_LENGTH];
  char buf[STD_LENGTH];
  char *t;
  int i = 0;
  int k;
  int p;
  DEFINE_COMMAND ("ansi", do_ansi, POSITION_DEAD, 0, LOG_NORMAL, "Configures channel colors.")


    argy = one_argy (argy, arg);

  if (arg[0] == '\0')
    {
      if (IS_SET (ch->pcdata->act2, PLR_ANSI))
        {
          p = ch->position;
          ch->position = POSITION_STANDING;
          send_to_char ("Channel Color Configuration/Settings...", ch);
          for (cn = chan_first; cn != NULL; cn = cn->next)
            {
              if (cn->level > LEVEL (ch))
                {
                  i++;
                  continue;
                }
              sprintf (buf, "$R%s", ch->pcdata->chan_color[i]);
              act (buf, ch, NULL, ch, TO_CHAR);
              sprintf (buf, "%12ss \x1B[37;0mwill use color code \x1B[1;37m%s\x1B[37;0m.",
                       capitalize (cn->commands[0]),
                       ch->pcdata->chan_color[i]);
              send_to_char (buf, ch);
              i++;
            }
          send_to_char ("\n\r", ch);
          ch->position = p;
          return;
        }
      send_to_char ("To toggle ANSI, type 'config ansi'. Type HELP ANSI for more help.\n\r", ch);
      return;
    }
  i = 0;
  for (cn = chan_first; cn != NULL; cn = cn->next)
    {
      if (!str_cmp (arg, cn->commands[0]))
        break;
      i++;
    }
  if (!cn)
    {
      send_to_char ("I don't know of that channel.\n\r", ch);
      return;
    }
  if (argy[0] == '\0')
    {
      send_to_char ("Change the color code to what?\n\r", ch);
      return;
    }
  k = 0;
  for (t = argy; *t != '\0'; t++)
    {
      if (*t != 'R' && *t != 'B' && (*t < '0' || *t > '9') && *t != '$')
        {
          send_to_char ("Invalid code encountered.  Use $R for regular text, not $N.\n\r", ch);
          return;
        }
      k++;
    }
  if (k > 9)
    {
      send_to_char ("Color code too long!\n\r", ch);
      return;
    }
  strcpy (ch->pcdata->chan_color[i], argy);
  send_to_char ("Color set.\n\r", ch);
  ansi_color (NTEXT, ch);
  return;
}

void
do_config (CHAR_DATA * ch, char *argy)
{
  char arg[SML_LENGTH];
  char buf[STD_LENGTH];
  DEFINE_COMMAND ("config", do_config, POSITION_DEAD, 0, LOG_NEVER, "This allows you to configure several aspects of gameplay.")

if ( IS_MOB(ch) )
   return;

    one_argy (argy, arg);
  if (arg[0] == '\0')
    {
   
      send_to_char ("\n\r", ch);
      
      send_to_char (IS_SET (ch->pcdata->act2, PLR_PROMPT_UP)
                    ? "\x1B[1;37m[ \x1B[34mUPPROMPT \x1B[1;37m ] If you have mapping on, the prompt will be up.\n\r"
                    : "\x1B[37;0m[ \x1B[34mupprompt  \x1B[37m] If you have mapping on, the prompt will be down.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_AUTOEXIT)
                    ? "\x1B[1;37m[ \x1B[34mAUTOEXIT \x1B[1;37m ] You automatically see exits.\n\r"
                    : "\x1B[37;0m[ \x1B[34mautoexit  \x1B[37m] You don't automatically see exits.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_AUTOLOOT)
                    ? "\x1B[1;37m[ \x1B[34mAUTOLOOT  \x1B[1;37m] You automatically loot corpses.\n\r"
                    : "\x1B[37;0m[\x1B[34m autoloot  \x1B[37;0m] You don't automatically loot corpses.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_AUTOSAC)
                    ? "\x1B[1;37m[\x1B[34m AUTOSAC   \x1B[1;37m] You automatically sacrifice corpses.\n\r"
                    : "\x1B[37;0m[\x1B[34m autosac   \x1B[37;0m] You don't automatically sacrifice corpses.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_AUTOGOLD)
                    ? "\x1B[1;37m[\x1B[34m AUTOGOLD  \x1B[1;37m] You automatically loot coins from corpses.\n\r"
                    : "\x1B[37;0m[\x1B[34m autogold  \x1B[37;0m] You don't automatically loot coins from corpses.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_AUTOSPLIT)
                    ? "\x1B[1;37m[\x1B[34m AUTOSPLIT \x1B[1;37m] You automatically split coins looted from corpses.\n\r"
                    : "\x1B[37;0m[ \x1B[34mautosplit \x1B[37;0m] You don't automatically split coins looted from corpses.\n\r"
                    ,ch);
        send_to_char (IS_SET (ch->pcdata->act2, PLR_AUTOSCAN)
                    ? "\x1B[1;37m[\x1B[34m AUTOSCAN \x1B[1;37m] You notice players of opposite alignment nearby.\n\r"
                    : "\x1B[37;0m[ \x1B[34mautoscan \x1B[37;0m] You don't notice players of different alignment nearby.\n\r"
                    ,ch);
 send_to_char (IS_SET (ch->pcdata->act2, PLR_AUTOBLINK)
                    ? "\x1B[1;37m[\x1B[34m BLINK    \x1B[1;37m ] You automatically blink away from hits in combat.\n\r"
                    : "\x1B[37;0m[\x1B[34m blink    \x1B[37;0m ] You don't automatically blink away from hits in combat.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_ASSIST)
                    ? "\x1B[1;37m[\x1B[34m ASSIST   \x1B[1;37m ] You automatically assist your group in combat.\n\r"
                    : "\x1B[37;0m[\x1B[34m assist   \x1B[37;0m ] You don't automatically assist your group in combat.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_BLANK)
                    ? "\x1B[1;37m[\x1B[34m BLANK    \x1B[1;37m ] You have a blank line before your prompt.\n\r"
                    : "\x1B[37;0m[\x1B[34m blank    \x1B[37;0m ] You have no blank line before your prompt.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_BRIEF)
                    ? "\x1B[1;37m[\x1B[34m BRIEF    \x1B[1;37m ] You see brief descriptions.\n\r"
                    : "\x1B[37;0m[\x1B[34m brief    \x1B[37;0m ] You see long descriptions.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_COMBINE)
                    ? "\x1B[1;37m[\x1B[34m COMBINE  \x1B[1;37m ] You see object lists in combined format.\n\r"
                    : "\x1B[37;0m[\x1B[34m combine  \x1B[37;0m ] You see object lists in single format.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_PROMPT)
        ? "\x1B[1;37m[\x1B[34m PROMPT    \x1B[1;37m] You have a prompt.\n\r"
                    : "\x1B[37;0m[\x1B[34m prompt    \x1B[37;0m] You don't have a prompt.\n\r"
                    ,ch);
      send_to_char (IS_SET (ch->pcdata->act2, PLR_ANSI)
                    ? "\x1B[1;37m[\x1B[34m ANSI     \x1B[1;37m ] You receive ansi color codes.\n\r"
                    : "\x1B[37;0m[ \x1B[34mansi     \x1B[37;0m ] You don't receive ansi color codes.\n\r"
                    ,ch);
      /* send_to_char( IS_SET(ch->pcdata->act2, PLR_TELNET_GA)
         ? "\x1B[1;37m[\x1B[34m TELNETGA\x1B[1;37m ] You can not use arrow keys, echo local.\n\r"
         : "\x1B[37;0m[\x1B[34m telnetga\x1B[37;0m ] You can use arrow keys, let mud echo.\n\r"
         , ch ); */
      if (IS_PLAYER (ch))
        {
          send_to_char (ch->pcdata->challenge_on
                        ? "\x1B[1;37m[\x1B[34m CHALLNGE  \x1B[1;37m] You allow arena challenges from other players.\n\r"
                        : "\x1B[37;0m[\x1B[34m challnge  \x1B[37;0m] You cannot be challenged in the arena.\n\r"
                        ,ch);
          send_to_char (ch->pcdata->arena_msg == 1
                        ? "\x1B[1;37m[\x1B[34m ARENA     \x1B[1;37m] You will see updated status of arena battles.\n\r"
                        : (ch->pcdata->arena_msg == 2
                           ? "\x1B[1;37m[\x1B[34m aReNa     \x1B[1;37m] You will see battles start/stop but nothing else.\n\r"
                           : "\x1B[37;0m[\x1B[34m arena     \x1B[37;0m] You will not see any arena messages.\n\r"
                        )
                        ,ch);
        }
      send_to_char (IS_SET (ch->pcdata->act2, PLR_SILENCE)
           ? "\x1B[1;37m[ \x1B[34mSILENCE\x1B[1;37m ] You are silenced.\n\r"
                    : ""
                    ,ch);
      if (IS_IMMORTAL (ch))
        {
          send_to_char (IS_SET (ch->act, PLR_HOLYLIGHT) && IS_IMMORTAL (ch)
                        ? ""
               : "[ holylite ] Holylight is off; please type HOLYLIGHT.\n\r"
                        ,ch);
          send_to_char (IS_SET (ch->act, PLR_HOLYPEACE) && IS_IMMORTAL (ch)
                        ? "\x1B[1;37m[ \x1B[34mPEACE   \x1B[1;37m ] No mortals can fight in your presence.\x1B[37;0m\n\r"
                        : "\x1B[37;0m[ \x1B[34mpeace   \x1B[37;0m ] Your presence doesn't affect mortal battles.\n\r"
                        ,ch);
          send_to_char (IS_SET (ch->pcdata->act2, PLR_HOLYLAG) && IS_IMMORTAL (ch)
                        ? "\x1B[1;37m[ \x1B[34mHOLYLAG \x1B[1;37m ] You are lagged (delayed) up the butt, like mortals.\x1B[37;0m\n\r"
                        : "\x1B[37;0m[ \x1B[34mholylag \x1B[37;0m ] You aren't 'delayed' on any commands.\n\r", ch);
          send_to_char (IS_SET (ch->act, PLR_HOLYWALK) && IS_IMMORTAL (ch)
                        ? ""
                 : "[ holywalk ] Holywalk is off; please type HOLYWALK.\n\r"
                        ,ch);
        }
      send_to_char ("\n\r\x1B[1;32mPlease see the CHANNEL command also.\x1B[37;0m\n\r", ch);
    }
  else
    {
      char arg2[STD_LENGTH];
      bool fSet;
      int bit;
          arg2[0]='\0';
      fSet = arg[0] == '+';
      if (arg[0] == '-' || arg[0] == '+')
        {
          sprintf (arg2, "%c", arg[0]);
          sprintf (arg, "%s", arg + 1);
        }
      if (!str_cmp (arg, "peace"))
        {
          send_to_char ("Please type 'holypeace' at the prompt to toggle it on/off.\n\r", ch);
          return;
        }
      if (!str_cmp (arg, "autoexit") /*&& IS_IMMORTAL(ch) */ )
        bit = PLR_AUTOEXIT;
      else if (!str_cmp (arg, "autoscan") /*&& IS_IMMORTAL(ch) */ )
        bit = PLR_AUTOSCAN;
      else if (!str_cmp (arg, "autoloot") /* && IS_IMMORTAL(ch) */ )
        bit = PLR_AUTOLOOT;
      else if (!str_cmp (arg, "autosac") /* && IS_IMMORTAL(ch) */ )
        bit = PLR_AUTOSAC;
      else if (!str_cmp (arg, "autogold") /* && IS_IMMORTAL(ch) */ )
        bit = PLR_AUTOGOLD;
      else if (!str_cmp (arg, "upprompt"))
        bit = PLR_PROMPT_UP;
      else if (!str_cmp (arg, "assist"))
        bit = PLR_ASSIST;
      else if (!str_cmp (arg, "autosplit"))
        bit = PLR_AUTOSPLIT;
      else if (!str_cmp (arg, "autoblink"))
        bit = PLR_AUTOBLINK;
      else if (!str_cmp (arg, "blank"))
        bit = PLR_BLANK;
       else if (!str_cmp (arg, "brief"))
        bit = PLR_BRIEF;
       else if (!str_cmp (arg, "combine"))
	 bit = PLR_COMBINE;
       else if (IS_IMMORTAL (ch) && !str_cmp (arg, "holylag"))
	 bit = PLR_HOLYLAG;
       else if (!str_cmp (arg, "prompt"))
	 bit = PLR_PROMPT;
       else if (!str_cmp (arg, "upprompt"))
	 bit = PLR_PROMPT_UP;
       else if (!str_cmp (arg, "ansi"))
	 bit = PLR_ANSI;
       else
	 {
	   if (!str_cmp (arg, "challenge") || !str_cmp (arg, "challnge"))
	     {
              if (IS_MOB (ch))
                return;
              if (ch->pcdata->challenge_on)
                {
                  send_to_char ("Challenges toggled \x1B[1;30mOFF\x1B[37;0m.\n\r", ch);
                  ch->pcdata->challenge_on = FALSE;
                }
              else
                {
                  send_to_char ("Challenges toggled\x1B[1m ON\x1B[0m.\n\r", ch);
                  ch->pcdata->challenge_on = TRUE;
                }
              return;
	     }
	   if (!str_cmp (arg, "arena") || !str_cmp (arg, "arenanotfy") || !str_cmp (arg, "arenanotify"))
	     {
	       if (IS_MOB (ch))
		 return;
	       if (ch->pcdata->arena_msg == 2)
		 {
		   send_to_char ("Arena messages toggled \x1B[1;30mOFF\x1B[37;0m.\n\r", ch);
		   ch->pcdata->arena_msg = 0;
		 }
	       else if (ch->pcdata->arena_msg == 1)
		 {
		   send_to_char ("Arena notify will be shown, but not battle spam.\n\r", ch);
		   ch->pcdata->arena_msg = 2;
		 }
	       else
		 {
		   send_to_char ("Arena messages toggled\x1B[1m ON\x1B[0m.\n\r", ch);
		   ch->pcdata->arena_msg = 1;
		 }
	       return;
	     }
	   send_to_char ("Config which option?\n\r", ch);
	   return;
	 }
       if (fSet)
	 {
	   sprintf (buf, "Enabled.\n\r");
	   SET_BIT (ch->pcdata->act2, bit);

	 }
       else if (!fSet && arg2[0] == '-')
	 {
	   sprintf (buf, "Disabled.\n\r");
	   REMOVE_BIT (ch->pcdata->act2, bit);
	   
	 }
       else
	 {
	   if (IS_SET (ch->pcdata->act2, bit))
            {
              sprintf (buf, "Toggled \x1B[1;30mOFF\x1B[37;0m.\n\r");
              REMOVE_BIT (ch->pcdata->act2, bit);
             
            }
	   else
	     {
	       SET_BIT (ch->pcdata->act2, bit);
	       sprintf (buf, "Toggled\x1B[1m ON\x1B[0m.\n\r");

	     }
	 }
        send_to_char (buf, ch);
    }
  return;
}

void
set_title (CHAR_DATA * ch, char *title)
{
  char buf[STD_LENGTH];
  if (IS_MOB (ch))
    {
      bug ("Set_title: NPC.", 0);
      return;
    }
  strcpy (buf, title);
/*
   if ( isalpha(title[0]) || isdigit(title[0]) )
   {
   buf[0] = ' ';
   strcpy( buf+1, title );
   }
   else
   {
   strcpy( buf, title );
   }
 */
  free_string (ch->pcdata->title);
  ch->pcdata->title = str_dup (buf);
  if (strlen(ch->pcdata->title) > 80)
  {
  free_string (ch->pcdata->title);
  ch->pcdata->title = str_dup ("my title was too long");
  }
  return;
}

void
do_title (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  char *t;
  DEFINE_COMMAND ("title", do_title, POSITION_DEAD, 0, LOG_NORMAL, "This command allows you to change your title.")

    if (IS_MOB (ch))
    return;
  if (argy[0] == '\0')
    {
      send_to_char ("Change your title to what?\n\r", ch);
      return;
    }
 if (IS_SET(ch->act, PLR_LAMER))
   {
     send_to_char("Access denied.\n\r", ch);
     return;
   }
  if (strlen (argy) > 60)
    {
      send_to_char ("Your title may not be more than 60 characters.\n\r", ch);
      return;
    }
  for (t = argy; *t != '\0'; t++)
    {
      if (*t == '$' && *(t + 1) != '\0' && (*(t + 1) == 'F' || *(t + 1) == 'f'))
        {
          send_to_char ("Sorry, no flashing titles.\n\r", ch);
          return;
        }
    }


  for (t = argy; *t != '\0'; t++ )
    {
      if (*t == '{' || *t == '}')
      {
          send_to_char ("You tried to use an invalid character in your title.\n\r", ch);
          return;
      }
    }
  sprintf (buf, "%s\x1B[37;0m", anseval (argy, ch));
/* buf[26] = '\0'; */
/* smash_tilde( buf ); */
  set_title (ch, buf);
  send_to_char ("Title set.\n\r", ch);
  return;
}

void
do_description (CHAR_DATA * ch, char *argy)
{
  int k;
  int countcr;
  DEFINE_COMMAND ("description", do_description, POSITION_DEAD, 0, LOG_NORMAL, "This command allows you to type in your character's description.")

    k = 0;
  countcr = 0;
  if (IS_MOB (ch))
    return;
 if (IS_SET(ch->act, PLR_LAMER))
   {
     send_to_char("Access denied\n\r",ch);
     return;
   }

  string_append (ch, &ch->pcdata->description);
/*if (ch->description!=NULL) {
   for (t=ch->description; *t!='\0'; t++) {
   if (*t=='\n') countcr++;
   k++;
   if (countcr>15 || k>790) {
   char tp[800];
   *t='\0';
   send_to_char("Description too long! Sorry!\n\r",ch);
   strcpy(tp,ch->description);
   free_string(ch->description);
   ch->description=str_dup(tp);
   break;
   }
   }
   }
 */
/*
   if ( argy[0] != '\0' )
   {
   buf[0] = '\0';
   smash_tilde( argy );
   if ( argy[0] == '+' )
   {
   if ( STR(ch, description) != NULL )
   strcat( buf, STR(ch, description) );
   argy++;
   while ( isspace(*argy) )
   argy++;
   }
   if ( strlen(buf) + strlen(argy) >= STD_LENGTH - 500 )
   {
   send_to_char( "Description too long.\n\r", ch );
   return;
   }
   strcpy(tdesc,buf);
   for (k=0; k<(STD_LENGTH+10) ;k++)
   {
   if (tdesc[k]=='\0') break;
   if (tdesc[k]=='\n') countcr++;
   }
   if (countcr>10)
   {
   send_to_char("You description can only be 10 lines maximum.\n\r",ch);
   return;
   }
   strcat( buf, argy );
   strcat( buf, "\n\r" );
   free_string( ch->description );
   ch->description = str_dup( buf );
   }
   send_to_char( "Your description is:\n\r", ch );
   send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
 */
  return;
}
