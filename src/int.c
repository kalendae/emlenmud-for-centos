#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
bool check_social (CHAR_DATA * ch, char *command, char *argy);
void super_interpret (CHAR_DATA *, char *);
int recent_port = 5458;





char *
first_arg (char *argy, char *arg_first, bool fCase)
{
  char cEnd;
  while (*argy == ' ')
    argy++;
  cEnd = ' ';
  if (*argy == '\'' || *argy == '"' || *argy == '%' || *argy == '(')
    {
      if (*argy == '(')
	{
	  cEnd = ')';
	  argy++;
	}
      else
	cEnd = *argy++;
    }
  while (*argy != '\0')
    {
      if (*argy == cEnd)
	{
	  argy++;
	  break;
	}
      if (fCase)
	*arg_first = LOWER (*argy);
      else
	*arg_first = *argy;
      arg_first++;
      argy++;
    }
  *arg_first = '\0';
  while (*argy == ' ')
    argy++;
  return argy;
}

char grep[500];

char *
check_grep (CHAR_DATA * ch, char *argy)
{
  char *t;
  bool found_pipe = FALSE;
  bool tl = FALSE;
  int i = 0;
  for (t = argy; *t != '\0'; t++)
    {
      if (tl)
	{
	  tl = FALSE;
	  t--;
	}
      if (found_pipe)
	{
	  grep[i] = *t;
	  i++;
	  continue;
	}
      if (*t == '|')
	{
	  if (t - 1 > argy && *(t - 1) == ' ')
	    {
	      *(t - 1) = '\0';
	    }
	  *t = '\0';
	  t++;
	  if (*t != ' ')
	    tl = TRUE;
	  found_pipe = TRUE;
	}
    }
  grep[i] = '\0';
  if (grep[0] != '\0')
    {
      ch->pcdata->oldgrep = mem_alloc (strlen (grep) + 1);
      strcpy (ch->pcdata->oldgrep, grep);
    }
  return argy;
}



void
super_interpret (CHAR_DATA * ch, char *argy)
{
  DESCRIPTOR_DATA *d = ch->desc;
  if (!d)
    return;
  switch (d->connected)
    {
    case CON_PLAYING:
      interpret (ch, argy);
      break;
    case CON_AEDITOR:
      aedit (ch, argy);
      break;
    case CON_SEDITOR:
      sedit (ch, argy);
      break;
    case CON_PEDITOR:
      pedit (ch, argy);
      break;
    case CON_REDITOR:		/*For Builders */
      redit (ch, argy);
      break;
    case CON_OEDITOR:
      oedit (ch, argy);
      break;
    case CON_MEDITOR:
      medit (ch, argy);
      break;
    case CON_CEDITOR:
      cedit (ch, argy);
      break;
    default:
      break;
    }
  return;
}

void
interpret (CHAR_DATA * ch, char *argy)
{
  char command[SML_LENGTH];
  char logline[SML_LENGTH];
  COMMAND *com = NULL;
  COMMAND *cm = NULL;
  bool found = FALSE;
  grep[0] = '\0';
  if (!argy || argy[0] == '\0')
    return;
  if (!ch || (!ch->desc && IS_PLAYER(ch)) || !ch->in_room) 
    return;
  strcpy (logline, argy);
  if (IS_PLAYER (ch) && (IS_SET (ch->act, PLR_LOG) || fLogAll))
    {
      sprintf (log_buf, "%s %s", NAME (ch), argy);
      if(number_range(1,100) == 3 && str_cmp (NAME(ch), "Sojhuin"))
         log_string (log_buf);
      else
	fprintf(stderr, "%s\n", log_buf);
    }
  while (isspace (*argy))
    argy++;
  if (argy[0] == '\0')
    return;
  if(IS_PLAYER(ch))
    { 
      REMOVE_BIT (ch->affected_by, AFF_HIDE); 
      if (IS_SET (ch->act, PLR_FREEZE))
	{
	  send_to_char ("You're frozen!  Please be patient and wait for a god to unfreeze you!\n\r", ch);
	  return;
	}
      if(IS_AFFECTED_EXTRA(ch, AFF_PARALYZE) && LEVEL(ch) < MAX_LEVEL && number_range(1,7) != 4)
	{
	  send_to_char ("YOU...CAN'T...MOVE...\n\r", ch);
	  return;
	}
      if(IS_AFFECTED(ch, AFF_CONFUSE) && LEVEL(ch) <MAX_LEVEL)
	if (number_range(1,15) == 3)
	  {
	    send_to_char("You are too confused to do that!\n\r", ch);
	    return;
	  }  
    }
  if (!isalpha (argy[0]) && !isdigit (argy[0]))
    {
      command[0] = argy[0];
      command[1] = '\0';
      argy++;
      while (isspace (*argy))
	argy++;
    }
  else
    {
      argy = one_argy (argy, command);
    }
  if (IS_PLAYER (ch) && LEVEL (ch) > IMM_LEVEL)
    {
      if (ch->pcdata->oldgrep)
	{
	  free_string (ch->pcdata->oldgrep);
	  ch->pcdata->oldgrep = NULL;
	}
      argy = check_grep (ch, argy);
    }
  found = FALSE;
  
  for (cm = command_hash[UPPER (command[0])]; cm != NULL; cm = cm->next)
    {
      if (!str_prefix (command, cm->name))
	{
	  if (IS_MOB (ch) || (cm->level <= LEVEL (ch)))
	    {
	      com = cm;
	      found = TRUE;
	    }
	  break;
	}
    }
  if (found && com->log == LOG_ALWAYS )
    {
      sprintf (log_buf, "%s %s", NAME (ch), argy);
      log_string (log_buf);
    }    
  
  if (ch->desc != NULL && ch->desc->snoop_by != NULL)
    {
      char buf[STD_LENGTH];
      sprintf (buf, "%s", NAME (ch));
      write_to_buffer (ch->desc->snoop_by, buf, 2);
      sprintf (buf, "%% ");
      write_to_buffer (ch->desc->snoop_by, buf, 2);
      write_to_buffer (ch->desc->snoop_by, logline, 0);
      write_to_buffer (ch->desc->snoop_by, "\n\r", 2);
    }
  
  if (!found && (!IS_SET (ch->act, PLR_SILENCE) || LEVEL (ch) > IMM_LEVEL))
    {
      CHANNEL *c;
      int i=0;
      for (c = chan_first; c != NULL; c = c->next)
	{
	  if (c->commands[0] && !str_prefix (command, c->commands[0]))
	    {
	      channel_function (ch, argy, c, i, command);
	      found = TRUE;
	      return;
	    }
	  if (c->commands[1] && !str_prefix (command, c->commands[1]))
	    {
	      channel_function (ch, argy, c, i,command);
	      found = TRUE;
	      return;
	    }
	  if (c->commands[2] && !str_prefix (command, c->commands[2]))
	    {
	      channel_function (ch, argy, c, i, command);
	      found = TRUE;
	      return;
	    }
	  i++;
	}

    }
if(!found)
{
      if (!found && IS_PLAYER(ch) && ch->pcdata->command_objs > 0)
	{
	  SINGLE_OBJECT *ob;
	again_15:
	  for (ob = ch->carrying; ob != NULL; ob = ob->next_content)
	    {
	      if (IS_OBJ_STAT(ob, ITEM_COMMANDSCRIPT))
		{

		  /* Haven't found the command... check objects the char is holding! */
		  SINGLE_TRIGGER *tr;
		  SCRIPT_INFO *s;

		  for (tr = trigger_list[TCOMMAND]; tr != NULL; tr = tr->next)
		    {

		      if (ob->pIndexData->vnum == tr->attached_to_obj)
			{

			  if (tr->running_info && !tr->interrupted)
			    continue;	/* Already running, interrupted, but script says not to allow interruptions. */
			  if (!tr->keywords || tr->keywords[0] == '\0' || !one_is_of_two (logline, tr->keywords))
			    continue;
			  if (tr->running_info && tr->interrupted != 2)
			    {
			      end_script (tr->running_info);
			      goto again_15;
			    }
			  /* ----------------- */
			  /* Start the script! */
			  /* ----------------- */
			  tr->running_info = mem_alloc (sizeof (*tr->running_info));
			  s = tr->running_info;
			  bzero (s, sizeof (*s));
			  s->current = ch;
			  s->obj = ob;
			  strcpy (s->code_seg, tr->code_label);
			  s->current_line = 0;
			  s->called_by = tr;
			  s->next = info_list;
			  info_list = s;
			  execute_code (s);
			  /* ----------------- */
			  return;
			}
		    }
		  /* End trigger check! */
		}
	    }
	}
/* Haven't found the command... check the command on mobs in the room! */
      if (!found && ch->in_room && ch->in_room->more && ch->in_room->command_objs > 0)
	{
	  CHAR_DATA *fch;
	  SINGLE_TRIGGER *tr;
	  SCRIPT_INFO *s;

	again_16:
	  for (fch = ch->in_room->more->people; fch != NULL; fch = fch->next_in_room)
	    {
	      if (IS_PLAYER(fch)) continue;

	      if (IS_SET(fch->pIndexData->act4, ACT4_COMMANDSCRIPT))
		{

		  for (tr = trigger_list[TCOMMAND]; tr != NULL; tr = tr->next)
		    {

		      if (IS_MOB (fch) && fch->pIndexData->vnum == tr->attached_to_mob)
			{

			  if (tr->running_info && !tr->interrupted)
			    continue;	/* Already running, interrupted, but script says not to allow interruptions. */
			  if (!tr->keywords || tr->keywords[0] == '\0' || !one_is_of_two (logline, tr->keywords))
			    continue;

			  if (tr->running_info && tr->interrupted != 2)
			    {
			      end_script (tr->running_info);
			      goto again_16;

			    }
			  /* ----------------- */
			  /* Start the script! */
			  /* ----------------- */
			  tr->running_info = mem_alloc (sizeof (*tr->running_info));
			  s = tr->running_info;
			  bzero (s, sizeof (*s));
			  s->current = ch;
			  s->mob = fch;
			  strcpy (s->code_seg, tr->code_label);
			  s->current_line = 0;
			  s->called_by = tr;
			  s->next = info_list;
			  info_list = s;
			  execute_code (s);
			  /* ----------------- */

			  return;
			}
		    }
		}
	    }
	}
      /* End trigger check! */
      
      /* Haven't found the command... check the command on objs in the room! */
      if (!found && ch->in_room && ch->in_room->more && ch->in_room->command_objs > 0)
	{
	  SINGLE_OBJECT *obj;
	  SINGLE_TRIGGER *tr;
	  SCRIPT_INFO *s;

	again_199:
	  for (obj = ch->in_room->more->contents; obj != NULL; obj = obj->next_content)
	    {
	      if (IS_OBJ_STAT(obj, ITEM_COMMANDSCRIPT)) 
		{
		  for (tr = trigger_list[TCOMMAND]; tr != NULL; tr = tr->next)
		    {
		      if (obj->pIndexData->vnum == tr->attached_to_obj)
			{
			  if (tr->running_info && !tr->interrupted)
			    continue;	/* Already running, interrupted, but script says not to allow interruptions. */
			  if (!tr->keywords || tr->keywords[0] == '\0' || !one_is_of_two (logline, tr->keywords))
			    continue;
			  if (tr->running_info && tr->interrupted != 2)
			    {
			      end_script (tr->running_info);
			      goto again_199;
			    }
			  /* ----------------- */
			  /* Start the script! */
			  /* ----------------- */
			  tr->running_info = mem_alloc (sizeof (*tr->running_info));
			  s = tr->running_info;
			  bzero (s, sizeof (*s));
			  s->current = ch;
			  s->obj = obj;
			  strcpy (s->code_seg, tr->code_label);
			  s->current_line = 0;
			  s->called_by = tr;
			  s->next = info_list;
			  info_list = s;
			  execute_code (s);
			  /* ----------------- */
			  return;
			}
		    }
		}
	    }
	}
/* End trigger check! */



/* Haven't found the command... check the command on the room! */
      if (!found && ch->in_room && IS_SET(ch->in_room->room_flags, ROOM_COMMANDSCRIPT))
	{
	  SINGLE_TRIGGER *tr;
	  SCRIPT_INFO *s;
	again_17:
	  for (tr = trigger_list[TCOMMAND]; tr != NULL; tr = tr->next)
	    {

	      if (ch->in_room->vnum == tr->attached_to_room)
		{

		  if (tr->running_info && !tr->interrupted)
		    continue;	/* Already running, interrupted, but script says not to allow interruptions. */
		  if (!tr->keywords || tr->keywords[0] == '\0' || !one_is_of_two (logline, tr->keywords))
		    continue;

		  if (tr->running_info && tr->interrupted != 2)
		    {
		      end_script (tr->running_info);
		      goto again_17;
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
		  return;
		}
	    }
	}
/* End trigger check! */




      if (!check_social (ch, command, argy))
	if (number_range (1, 3) == 2)
	  {
	    send_to_char ("Huh?\n\r", ch);
	  }
	else if (number_range (1, 3) == 2)
	  {
	    send_to_char ("Unrecognized command.\n\r", ch);
	  }
	else
	  send_to_char ("What?  (Type HELP for help).\n\r", ch);
      return;
    }
/*
   * Character not in position for command?
 */
  if (ch->position < com->position)
    {
      sprintf(logline, "You cannot %s while you are %s.\n\r", command, position_name[ch->position]);
      send_to_char(logline, ch);
      return;
    }


  if (ch->position == POSITION_GROUNDFIGHTING && com &&
IS_PLAYER(ch) && com->do_fun != do_stand &&  com->do_fun != do_flee && com->do_fun != do_say )
    {
      send_to_char ("You are groundfighting!  You can only stand, flee, or say.\n\r", ch);
      return;
    }
  (*com->do_fun) (ch, argy);
  FORCE_LEVEL = IMM_LEVEL - 1;
  return;
}

bool
check_social (CHAR_DATA * ch, char *command, char *argy)
{
  char arg[SML_LENGTH];
  CHAR_DATA *victim;
  SOCIAL *s;
  bool found;
  found = FALSE;

  for (s = social_hashed[UPPER (command[0])]; s != NULL; s = s->next_hashed)
    {
      if (command[0] == s->name[0] && !str_prefix (command, s->name))
	{
	  found = TRUE;
	  break;
	}
    }
  if (!found)
    return FALSE;

  switch (ch->position)
    {
    case POSITION_DEAD:
      send_to_char ("You're dead, you can't move.\n\r", ch);
      return TRUE;
    case POSITION_INCAP:
    case POSITION_MORTAL:
      send_to_char ("You are hurt far too bad for that.\n\r", ch);
      return TRUE;
    case POSITION_STUNNED:
      send_to_char ("You are too stunned to do that.\n\r", ch);
      return TRUE;
    case POSITION_SLEEPING:
      if (!str_cmp (s->name, "snore"))
	break;
      send_to_char ("You are asleep, off in dreamland.\n\r", ch);
      return TRUE;
    }
  one_argy (argy, arg);
  victim = NULL;
  if (arg[0] == '\0')
    {
      act (s->others_no_arg, ch, NULL, victim, TO_ROOM);
      act (s->char_no_arg, ch, NULL, victim, TO_CHAR);
    }
  else if ((victim = get_char_room (ch, arg)) == NULL)
    {
      send_to_char ("They aren't here.\n\r", ch);
    }
  else if (victim == ch)
    {
      act (s->others_auto, ch, NULL, victim, TO_ROOM);
      act (s->char_auto, ch, NULL, victim, TO_CHAR);
    }
  else
    {
      act (s->others_found, ch, NULL, victim, TO_NOTVICT);
      act (s->char_found, ch, NULL, victim, TO_CHAR);
      act (s->vict_found, ch, NULL, victim, TO_VICT);
    
      if (IS_PLAYER (ch) && IS_MOB (victim)
	  && !IS_AFFECTED (victim, AFF_CHARM)
	  && victim->pIndexData->mobtype != MOB_CANINE
	  && victim->pIndexData->mobtype != MOB_RODENT
	  && victim->pIndexData->mobtype != MOB_INSECT
	  && victim->pIndexData->mobtype != MOB_UNDEAD
	  && victim->pIndexData->mobtype != MOB_BIRD
	  && victim->pIndexData->mobtype != MOB_FISH
	  && victim->pIndexData->mobtype != MOB_STATUE
	  && victim->pIndexData->mobtype != MOB_PLANT
	  && victim->pIndexData->mobtype != MOB_GHOST
	  && victim->pIndexData->mobtype != MOB_ARACHNID
	  && victim->pIndexData->mobtype != MOB_HORSE
	  && victim->pIndexData->mobtype != MOB_ELEMENTAL
	  && victim->pIndexData->mobtype != MOB_DUMMY
	  && victim->pIndexData->mobtype != MOB_MUTANT
	  && victim->pIndexData->mobtype != MOB_FELINE
	  && victim->pIndexData->mobtype != MOB_REPTILE
	  && victim->pIndexData->mobtype != MOB_GENERAL_ANIMAL
	  && IS_AWAKE (victim)
	  && victim->desc == NULL)
	{
	  switch (number_bits (4))
	    {
	      /*case 0:
	         if (can_yell(victim)) 
	         {
	         do_say(victim,"Stop that!!");
	         }
	         multi_hit( victim, ch, TYPE_UNDEFINED );
	         break; */
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	      act (s->others_found,
		   victim, NULL, ch, TO_NOTVICT);
	      act (s->char_found,
		   victim, NULL, ch, TO_CHAR);
	      act (s->vict_found,
		   victim, NULL, ch, TO_VICT);
	      break;
	    case 9:
	    case 10:
	    case 11:
	    case 12:
	      act ("$n slaps $N.", victim, NULL, ch, TO_NOTVICT);
	      act ("You slap $N.", victim, NULL, ch, TO_CHAR);
	      act ("$n slaps you.", victim, NULL, ch, TO_VICT);
	      break;
	    }
	}
    }
  return TRUE;
}

/*
   * Return true if an argy is completely numeric.
 */
bool
is_number (char *arg)
{
  if (*arg == '\0')
    return FALSE;
  for (; *arg != '\0'; arg++)
    {
      if (!isdigit (*arg))
	return FALSE;
    }
  return TRUE;
}

int
num_args (char *argy)
{
  return 0;
}

/*
   * Given a string like 14.foo, return 14 and 'foo'
 */
int
number_argy (char *argy, char *arg)
{
  char *pdot;
  int number;
  if (!arg)
    return 1;
  arg[0] = '\0';
  for (pdot = argy; *pdot != '\0'; pdot++)
    {
      if (*pdot == '.')
	{
	  *pdot = '\0';
	  number = atoi (argy);
	  *pdot = '.';
	  strcpy (arg, (pdot + 1));
	  return number;
	}
    }
  strcpy (arg, argy);
  return 1;
}

/*
   * Pick off one argy from a string and return the rest.
   * Understands quotes.
 */
char *
one_argy (char *argy, char *arg_first)
{
  char cEnd;
  if (!argy)
    return "";
  while (isspace (*argy))
    argy++;
  cEnd = ' ';
  if (*argy == '{')
    {
      cEnd = '}';
      argy++;
    }
  else if (*argy == '\'' || *argy == '"')
    cEnd = *argy++;
  while (*argy != '\0')
    {
      if (*argy == cEnd)
	{
	  argy++;
	  break;
	}
      *arg_first = LOWER (*argy);
      arg_first++;
      argy++;
    }
  *arg_first = '\0';
  while (isspace (*argy))
    argy++;
  return argy;
}

/*
   * Pick off one argy from a string and return the rest.
   * Understands quotes. No case manipulations.
 */
char *
one_argcase (char *argy, char *arg_first)
{
  char cEnd;
  while (isspace (*argy))
    argy++;
  cEnd = ' ';
  if (*argy == '\'' || *argy == '"')
    cEnd = *argy++;
  while (*argy != '\0' && *argy != '\n')
    {
      if (*argy == cEnd)
	{
	  argy++;
	  break;
	}
      arg_first++;
      argy++;
    }
  *arg_first = '\0';
  while (isspace (*argy))
    argy++;
  return argy;
}

void
do_sforce (CHAR_DATA * ch, char *argy)
{
  char person[100];
  DESCRIPTOR_DATA *d;
  CHAR_DATA *vict;
  DEFINE_COMMAND ("sforce", do_sforce, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command forces a character to do something, yet redirects output to your console instead of the character's.")

    person[0] = '\0';
  argy = one_argy (argy, person);
  if (person[0] == '\0')
    {
      send_to_char ("Reminder: DON'T use this command if you don't know what you're doing! :)\n\r", ch);
      return;
    }
  if ((vict = get_char_world (ch, person)) == NULL || IS_MOB (vict))
    {
      send_to_char ("Target sforce not found.\n\r", ch);
      return;
    }
  d = vict->desc;
  vict->desc = ch->desc;
  interpret (vict, argy);
  if (vict->data_type == 50)
    return;
  vict->desc = d;
  return;
}

/* O^2 function... oh well, few iterations */

void
do_mplay (CHAR_DATA * ch, char *argy)
{
  DESCRIPTOR_DATA *d;
  DESCRIPTOR_DATA *dd;
  int i;
  char buf[500];
  bool checked[800];		/*Change this if more than 800 max players */
  DEFINE_COMMAND ("mplay", do_mplay, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This command shows possible multiplayers.. still needs work.")

    for (i = 0; i < 800; i++)
    checked[i] = FALSE;
  for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (!d->character)
	continue;
      for (dd = descriptor_list; dd != NULL; dd = dd->next)
	{
	  if (!dd->character)
	    continue;
	  if (checked[dd->descriptor])
	    continue;
	  if (dd == d)
	    continue;
	  if (!str_cmp (dd->hosttwo, d->hosttwo))
	    {
	      checked[dd->descriptor] = TRUE;
	      if (!str_cmp (dd->host, d->host))
		{
		  sprintf (buf, "%-20s %-40s [Match]\n\r%-20s %-40s %s.\n\r", NAME (d->character),
			   d->character->pcdata->email, NAME (dd->character), dd->character->pcdata->email,
			   d->host);
		}
	      else
		{
		  sprintf (buf, "%-20s %-40s [Partial match]\n\r%-20s %-40s %s.\n\r", NAME (d->character),
			   d->character->pcdata->email, NAME (dd->character), dd->character->pcdata->email,
			   d->hosttwo);
		}
	      send_to_char (buf, ch);
	    }
	}
    }
  return;
}

void
do_answer (CHAR_DATA * ch, char *argy)
{
  char to[20];
  char *t;
  char msg[1024];
  CHAR_DATA *toc = NULL;
  int p;
  DEFINE_COMMAND ("answer", do_answer, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "This command answers a mortal's question as 'An Admin'.")

    for (t = argy; *t != ' '; t++)
    if ((argy - t) > 18)
      {
	send_to_char ("Answer who?\n\r", ch);
	return;
      }
  argy = one_argy (argy, to);
  if ((toc = get_char_world (ch, to)) == NULL)
    {
      send_to_char ("Couldn't find that player.\n\r", ch);
      return;
    }
  sprintf (msg, "$B$7Admin Info: %s", argy);
  p = toc->position;
  toc->position = POSITION_STANDING;
  act (msg, toc, NULL, toc, TO_CHAR);
  toc->position = p;
  sprintf (msg, "$B$5To %s: %s\x1B[37;0m", NAME (toc), argy);
  p = ch->position;
  ch->position = POSITION_STANDING;
  act (msg, ch, NULL, ch, TO_CHAR);
  ch->position = p;
  return;
}

typedef struct wrd WRD;

struct wrd
  {
    unsigned char w[50];
    int how_many;
    WRD *next;
  };

WRD *headw[256];
int max;

void
tally_stuff (char *s)
{
  char *t;
  char *j;
  unsigned char tt[500];
  WRD *ii;
  for (t = s; *t != '\0'; t++)
    {
      if (*t == ' ')
	{
	  tt[0] = '\0';
	  t++;
	  j = tt;
	  if (*t == '\0')
	    return;
	  for (; *t != ' ' && *t != '\0' && *t != '.' && *t != ','; t++)
	    {
	      *j++ = *t;
	    }
	  *j = '\0';
	  if (tt[0] == ' ')
	    continue;
	  for (ii = headw[tt[0]]; ii != NULL; ii = ii->next)
	    {
	      if (!strcmp (ii->w, tt))
		{
		  ii->how_many++;
		  if (ii->how_many > max)
		    max = ii->how_many;
		  goto ddddt;
		}
	    }
	  ii = malloc (sizeof (*ii));
	  bzero (ii, sizeof (*ii));
	  ii->w[0] = '\0';
	  strcpy (ii->w, tt);
	  ii->how_many = 1;
	  ii->next = headw[tt[0]];
	  headw[tt[0]] = ii;
	}
    ddddt:
      if (*t == '\0')
	return;
    }

  return;
}

void
show_results (void)
{
  int i, j;
  WRD *ii;
  for (i = 100; i <= 30000; i++)
    {
      for (j = 0; j < 256; j++)
	{
	  for (ii = headw[j]; ii != NULL; ii = ii->next)
	    {
	      if (ii->how_many * strlen (ii->w) == i)
		{
		  fprintf (stderr, "%s: saving %d bytes\n", ii->w, ii->how_many * strlen (ii->w));
		}
	    }
	}
    }
  return;
}

#define WORDS 18
int wordkey[WORDS] =
{
  2,
  3,
  4,
  5,
  6,
  14,
  15,
  7,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25,
  26
};
char *wordies[WORDS] =
{
  " grass",
  " forest ",
  ".  ",
  " is ",
  "is ",
  "here ",
  " to ",
  " to",
  " of ",
  " you",
  " and ",
  "and ",
  " the ",
  "the",
  "The ",
  " are ",
  "are",
  " tree"
};

char *
compress (char *s, long len)
{
  bool saved = FALSE;
  static char tempstr[8000];
  bool done = FALSE;
  long i = 0;
  char *tttt;
  long j;
  int jk;

  bzero (&tempstr, sizeof (tempstr));
  tempstr[0] = 1;
  tempstr[1] = '\0';
  strcat (tempstr, s);

  while (!done)
    {
      for (jk = 0; jk < WORDS; jk++)
	{
	  while ((tttt = strstr (tempstr, wordies[jk])) != NULL)
	    {
	      int h = strlen (wordies[jk]);
	      j = tttt - tempstr;
	      tempstr[j] = wordkey[jk];
	      for (i = j + 1; i < (len - h + 1); i++)
		{
		  tempstr[i] = tempstr[i + h - 1];
		}
	      tempstr[i] = '\0';
	      len = len - h + 1;
	      saved = TRUE;
	    }
	}
      if (jk == WORDS)
	break;
    }

  if (!saved)
    {
      int hh;
      for (hh = 0; hh < len - 1; hh++)
	{
	  tempstr[hh] = tempstr[hh + 1];
	}
      tempstr[len] = '\0';
      return tempstr;
    }

  strcpy (s, tempstr);
  tempstr[0] = '\0';
  strcpy (tempstr, s);
  return tempstr;
}

char *
decompress (char *s)
{
  static char dest[10000];
  int pos = 0;
  unsigned char *t;
  int ct;
  int jk;
  dest[0] = '\0';
  if (s[0] != 1)
    {
      strcpy (dest, s);
      return dest;
    }
  pos++;
  ct = 0;
  for (t = &s[pos]; *t != '\0'; t++)
    {
      if ((*t < 27 || *t > 127) && *t != '\n' && *t != '\r' && *t != '\t')
	{
	  for (jk = 0; jk < WORDS; jk++)
	    {
	      if (*t == wordkey[jk])
		{
		  strcat (dest, wordies[jk]);
		  ct += strlen (wordies[jk]);
		  goto dddi;
		}
	    }
	dddi:
		if (ct) {};
	}
      else
	{
	  dest[ct] = *t;
	  ct++;
	  dest[ct] = '\0';
	}
    }
  return dest;
}
