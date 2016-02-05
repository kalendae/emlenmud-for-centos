#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

/*char *itoa (int);*/

void remove_trigger_from_list (SINGLE_TRIGGER * tr);


/* Finds a trigger given a set trigger id name */
SINGLE_TRIGGER *
find_trigger (char *trigname)
{
  int i;
  SINGLE_TRIGGER *tr;
  MARK_DEBUG ("Find_trigger")
    for (i = 0; i < TMAX_TRIGGERS; i++)
    {
      for (tr = trigger_list[i]; tr != NULL; tr = tr->next)
        {
          if (!str_cmp (tr->trigger_id, trigname))
            return tr;
        }
    }
  return NULL;
}

/* Creates a new trigger, names it, and puts it in the trigger list with type 1 */
SINGLE_TRIGGER *
new_trigger (char *trigname)
{
  SINGLE_TRIGGER *tr;
  tr = mem_alloc (sizeof (*tr));
  bzero (tr, sizeof (*tr));
  if (strlen (trigname) > 19)
    trigname[19] = '\0';
  strcpy (tr->trigger_id, trigname);
  tr->keywords = &str_empty[0];
  tr->trigger_type = 1;
  tr->next = trigger_list[1];
  trigger_list[1] = tr;
  return tr;
}

/* returns the type of the trigger given a specific trigger name */
int
trigger_name (char *trig)
{
  int i = TMAX_TRIGGERS;
  if (!str_cmp (trig, "Enters"))
    i = TENTERS;
  else if (!str_cmp (trig, "Leaves"))
    i = TLEAVES;
  else if (!str_cmp (trig, "Created"))
    i = TCREATED;
  else if (!str_cmp (trig, "Command"))
    i = TCOMMAND;
  else if (!str_cmp (trig, "Moves"))
    i = TMOVES;
  else if (!str_cmp (trig, "Destroyed"))
    i = TDESTROYED;
  else if (!str_cmp (trig, "Say"))
    i = TSAYS;
  else if (!str_cmp (trig, "Tell"))
    i = TTELLS;
  else if (!str_cmp (trig, "Hourly"))
    i = TEVERY_HOUR;
  else if (!str_cmp (trig, "Quicktick"))
    i = TEVERY_QUICKTICK;
  else if (!str_cmp (trig, "Regentick"))
    i = TEVERY_REGENTICK;
  else if (!str_cmp (trig, "At_hour"))
    i = TAT_HOUR;
  if (!str_cmp (trig, "Given"))
    i = TGIVEN;
  else if (!str_cmp (trig, "Dropped"))
    i = TDROPPED;
  else if (!str_cmp (trig, "Equipped"))
    i = TEQUIPPED;
  else if (!str_cmp (trig, "Attacked"))
    i = TATTACKED;
  else if (!str_cmp (trig, "Sacced"))
    i = TOBJ_SACCED;
  else if (!str_cmp (trig, "Money"))
    i = TGIVEN_MONEY;

  return i;
}



/* returns the name of the trigger given a specific trigger number */
char *
trigger_type (int trig)
{
  static char trigret[100];
  strcpy (trigret, "Bad_Trgger");
  if (trig == TENTERS)
    strcpy (trigret, "Enters    ");
  else if (trig == TLEAVES)
    strcpy (trigret, "Leaves    ");
  else if (trig == TCREATED)
    strcpy (trigret, "Created   ");
  else if (trig == TCOMMAND)
    strcpy (trigret, "Command   ");
  else if (trig == TMOVES)
    strcpy (trigret, "Moves     ");
  else if (trig == TDESTROYED)
    strcpy (trigret, "Destroyed ");
  else if (trig == TSAYS)
    strcpy (trigret, "Say       ");
  else if (trig == TTELLS)
    strcpy (trigret, "Tell      ");
  else if (trig == TEVERY_HOUR)
    strcpy (trigret, "Hourly    ");
  else if (trig == TEVERY_QUICKTICK)
    strcpy (trigret, "Quicktick ");
  else if (trig == TEVERY_REGENTICK)
    strcpy (trigret, "Regentick ");
  else if (trig == TAT_HOUR)
    strcpy (trigret, "At_hour   ");
  else if (trig == TGIVEN)
    strcpy (trigret, "Given     ");
  else if (trig == TDROPPED)
    strcpy (trigret, "Dropped   ");
  else if (trig == TEQUIPPED)
    strcpy (trigret, "Equipped  ");
  else if (trig == TATTACKED)
    strcpy (trigret, "Attacked  ");
  else if (trig == TOBJ_SACCED)
    strcpy (trigret, "Sacced  ");
  else if (trig == TGIVEN_MONEY)
    strcpy (trigret, "Money  ");

  return trigret;
}

void
list_triggers (CHAR_DATA * ch, char *argy)
{
  int i;
  SINGLE_TRIGGER *tr;
  DEFINE_COMMAND ("listtriggers", list_triggers, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Lists all triggers.")
    hugebuf_o[0] = '\0';
  sprintf (hugebuf_o, "%-20s %s %5s %5s %5s\n\r", "Trigger ID", "TrigType  ", "Room", "Mob", "Objct");
  strcat (hugebuf_o, "------------------------------------------------------------------------------\n\r");
  for (i = 0; i < TMAX_TRIGGERS; i++)
    {
      for (tr = trigger_list[i]; tr != NULL; tr = tr->next)
        {
          sprintf (hugebuf_o + strlen (hugebuf_o), "%-20s %s %5d %5d %5d\n\r",
                   tr->trigger_id,
                   trigger_type (tr->trigger_type),
                   tr->attached_to_room,
                   tr->attached_to_mob,
                   tr->attached_to_obj);
        }
    }
  page_to_char (hugebuf_o, ch);
  return;
}

void
script_editor (CHAR_DATA * ch, char *argy)
{
  char arg1[500];
  char arg2[500];
  SINGLE_TRIGGER *tr;
  DEFINE_COMMAND ("scriptedit", script_editor, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "For Admin's use only right now.")
    if (!ch->desc)
    return;
  argy = one_argy (argy, arg1);
  argy = one_argy (argy, arg2);
  if (!str_cmp (arg1, "create") && arg2[0] != '\0')
    {
      if (find_trigger (arg2) != NULL)
        {
          send_to_char ("That trigger already exists!\n\r", ch);
          return;
        }
      tr = new_trigger (arg2);
      ch->desc->pEdit = (void *) tr;
      ch->desc->connected = CON_SCRIPTEDITOR;
      return;
    }
  if (!str_cmp (arg1, "delete") && arg2[0] != '\0')
    {
      if ((tr = find_trigger (arg2)) == NULL)
        {
          send_to_char ("That trigger doesn't exist!\n\r", ch);
          return;
        }
      remove_trigger_from_list (tr);
      free_string (tr->keywords);
      free (tr);
      send_to_char ("Trigger deleted.\n\r", ch);
      return;
    }
  if (arg1[0] == '\0')
    return;
  tr = find_trigger (arg1);
  if (!tr)
    {
      send_to_char ("That trigger was not found.\n\r", ch);
      return;
    }
  ch->desc->pEdit = (void *) tr;
  ch->desc->connected = CON_SCRIPTEDITOR;
  return;
}

void
show_this_trigger (CHAR_DATA * ch, char *trig)
{
  SINGLE_TRIGGER *tr;
  char buf[500];
  if ((tr = find_trigger (trig)) == NULL)
    {
      send_to_char ("Trigger not found.\n\r", ch);
      return;
    }
  sprintf (buf, "Trigger ID: \x1B[1;37m%s\x1B[37;0m  Trigger Type: \x1B[1;37m%s\x1B[37;0m\n\r",
           tr->trigger_id,
           trigger_type (tr->trigger_type));
  send_to_char (buf, ch);
  sprintf (buf, "Room: %d  Mob: %d  Object: %d  Interruptable: %s\n\r",
           tr->attached_to_room,
           tr->attached_to_mob,
           tr->attached_to_obj,
           (tr->interrupted == 1 ? "Yes" : (tr->interrupted == 2 ? "Yes, and multiple!" : "No")));
  send_to_char (buf, ch);
  sprintf (buf, "\x1B[1;37mPlayer\x1B[37;0m only: %s  \x1B[1;37mStop\x1B[37;0ms if player leaves: %s\n\r",
           (tr->players_only == 1 ? "Yes" : "No"),
           (tr->leaves_room == 1 ? "Yes" : "No"));
  send_to_char (buf, ch);
  sprintf (buf, "Calls code label: \x1B[1;34m%s\x1B[37;0m\n\r", tr->code_label);
  send_to_char (buf, ch);
  sprintf (buf, "Keywords: \x1B[1;37m%s\x1B[37;0m\n\r", tr->keywords);
  send_to_char (buf, ch);
  return;
}

void
remove_trigger_from_list (SINGLE_TRIGGER * tr)
{
  SINGLE_TRIGGER *tt;
  if (tr == trigger_list[tr->trigger_type])
    {
      trigger_list[tr->trigger_type] = tr->next;
      tr->next = NULL;
      return;
    }
  for (tt = trigger_list[tr->trigger_type]; tt != NULL; tt = tt->next)
    {
      if (tt->next == tr)
        {
          tt->next = tr->next;
          tr->next = NULL;
          return;
        }
    }
  fprintf (stderr, "Trigger not found to remove from trigger list!!\n");
  return;
}

void
scriptedit (CHAR_DATA * ch, char *argy)
{
  SINGLE_TRIGGER *tr;
  int tmp;
  char orig_argy[500];
  char arg1[500];
  if (!ch->desc)
    return;
  if (!ch->desc->pEdit)
    {
      ch->desc->connected = CON_PLAYING;
      return;
    }
  tr = (SINGLE_TRIGGER *) ch->desc->pEdit;
  strcpy (orig_argy, argy);
  if (!str_cmp (argy, "done"))
    {
      ch->desc->pEdit = NULL;
      ch->desc->connected = CON_PLAYING;
      return;
    }
  if (argy[0] == '\0' || argy[0] == ' ')
    {
      show_this_trigger (ch, tr->trigger_id);
      return;
    }
  argy = one_argy (argy, arg1);
  if ((tmp = trigger_name (arg1)) > 0 && tmp < TMAX_TRIGGERS)
    {
      remove_trigger_from_list (tr);
      tr->trigger_type = tmp;
      /* Must place in the new hash position */
      tr->next = trigger_list[tmp];
      trigger_list[tmp] = tr;
      send_to_char ("Trigger type set.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "code") && argy[0] != '\0')
    {
      if (strlen (argy) > 9)
        {
          send_to_char ("Invalid code label.  Must be less than 10 letters.\n\r", ch);
          return;
        }
      strcpy (tr->code_label, argy);
      send_to_char ("Code label set.\n\r", ch);
      return;
    }
  if (!str_prefix ("inter", arg1))
    {
      if (argy[0] != '\0' && is_number (argy))
        {
          tr->interrupted = atoi (argy);
          return;
        }
      if (tr->interrupted)
        {
          tr->interrupted = FALSE;
          send_to_char ("Script cannot be interrupted.\n\r", ch);
        }
      else
        {
          tr->interrupted = TRUE;
          send_to_char ("Script can now be interrupted.\n\r", ch);
        }
      return;
    }
  if (!str_prefix ("player", arg1))
    {
      if (argy[0] != '\0' && is_number (argy))
        {
          tr->players_only = atoi (argy);
          return;
        }
      if (tr->players_only)
        {
          tr->players_only = FALSE;
          send_to_char ("Script can now be triggered by mobs as well.\n\r", ch);
        }
      else
        {
          tr->players_only = TRUE;
          send_to_char ("Script can only be triggered by players now.\n\r", ch);
        }
      return;
    }
  if (!str_prefix ("stop", arg1))
    {
      if (argy[0] != '\0' && is_number (argy))
        {
          tr->leaves_room = atoi (argy);
          return;
        }
      if (tr->leaves_room)
        {
          tr->leaves_room = FALSE;
          send_to_char ("Script continues if char leaves the room.\n\r", ch);
        }
      else
        {
          tr->leaves_room = TRUE;
          send_to_char ("Script now stops if the character leaves the room.\n\r", ch);
        }
      return;
    }

  if (!str_prefix ("keyword", arg1))
    {
      free_string (tr->keywords);
      tr->keywords = str_dup (argy);
      send_to_char ("Key words set.\n\r", ch);
      return;
    }
  if (!str_cmp ("obj", arg1) && is_number (argy))
    {
      tr->attached_to_obj = atoi (argy);
      send_to_char ("Object vnum set.\n\r", ch);
      return;
    }
  if (!str_cmp ("room", arg1) && is_number (argy))
    {
      tr->attached_to_room = atoi (argy);
      send_to_char ("Room vnum set.\n\r", ch);
      return;
    }
  if (!str_cmp ("mob", arg1) && is_number (argy))
    {
      tr->attached_to_mob = atoi (argy);
      send_to_char ("Mob vnum set.\n\r", ch);
      return;
    }
  interpret (ch, orig_argy);
  return;
}

/* Finds a code label */
CODE *
find_code (char *id)
{
  CODE *c;
  int i;
  for (i = 0; i < 256; i++)
    {
      for (c = code_list[i]; c != NULL; c = c->next)
        {
          if (!str_cmp (id, c->label))
            return c;
        }
    }
  return NULL;
}

CODE *
new_code (char *id)
{
  CODE *c;
  c = mem_alloc (sizeof (*c));
  bzero (c, sizeof (*c));
  if (strlen (id) > 9)
    id[9] = '\0';
  strcpy (c->label, id);
  c->next = code_list[c->label[0]];
  code_list[c->label[0]] = c;
  c->code=&str_empty[0];
  return c;
}

void
online_coding (CHAR_DATA * ch, char *argy)
{
  char arg1[500];
  CODE *cd;
  CODE *c;
  DEFINE_COMMAND ("edit", online_coding, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Add/Modify online script code.")
    argy = one_argy (argy, arg1);
  if (!str_cmp (arg1, "delete") && argy[0] != '\0')
    {
      if ((cd = find_code (argy)) == NULL)
        {
          send_to_char ("That label does not exist!!\n\r", ch);
          return;
        }
      if (code_list[cd->label[0]] == cd)
        {
          code_list[cd->label[0]] = cd->next;
        }
      else
        for (c = code_list[cd->label[0]]; c != NULL; c = c->next)
          {
            if (c->next == cd)
              {
                c->next = cd->next;
                goto duu;
              }
          }
    duu:
      free_string (cd->code);
      free (cd);
      send_to_char ("Code label and code freed.\n\r", ch);
      return;
    }
  if (!str_cmp (arg1, "create") && argy[0] != '\0')
    {
      if (find_code (argy) != NULL)
        {
          send_to_char ("That label already exists!!\n\r", ch);
          return;
        }
      cd = new_code (argy);
      string_append (ch, &cd->code);
      return;
    }
  if ((cd = find_code (arg1)) == NULL)
    {
      send_to_char ("Code label was not found.\n\r", ch);
      return;
    }
  string_append (ch, &cd->code);
  return;
}

void
list_code (CHAR_DATA * ch, char *argy)
{
  int i;
  int col = 0;
  CODE *tr;
  DEFINE_COMMAND ("listcode", list_code, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Lists all code labels.")
    hugebuf_o[0] = '\0';
  for (i = 0; i < 256; i++)
    {
      for (tr = code_list[i]; tr != NULL; tr = tr->next)
        {
          col++;
          if (col == 7)
            {
              strcat (hugebuf_o, "\n\r");
              col = 1;
            }
          sprintf (hugebuf_o + strlen (hugebuf_o), "%-10s  ", tr->label);
        }
    }
  strcat (hugebuf_o, "\n\r");
  page_to_char (hugebuf_o, ch);
  return;
}

void
save_triggers (void)
{
  FILE *fp;
  SINGLE_TRIGGER *tr;
  int i;
  if ((fp = fopen ("trig.dat", "w+")) == NULL)
    {
      fprintf (stderr, "Error on trigger write open!!  NOT GOOD!\n");
      return;
    }
  for (i = 0; i < TMAX_TRIGGERS; i++)
    {
      for (tr = trigger_list[i]; tr != NULL; tr = tr->next)
        {
          fprintf (fp, "ID %s~\n", tr->trigger_id);
          fprintf (fp, "Keywords %s~\n", tr->keywords);
          fprintf (fp, "TT   %d\n", tr->trigger_type);
          fprintf (fp, "Room %d\n", tr->attached_to_room);
          fprintf (fp, "Mob  %d\n", tr->attached_to_mob);
          fprintf (fp, "Obj  %d\n", tr->attached_to_obj);
          fprintf (fp, "Int  %d\n", tr->interrupted);
          fprintf (fp, "Plo  %d\n", tr->players_only);
          fprintf (fp, "Clr  %d\n", tr->leaves_room);
          fprintf (fp, "Code %s~\n", tr->code_label);
        }
    }
  fprintf (fp, "END\n");
  fclose (fp);
  return;
}

void
load_triggers (void)
{
  SINGLE_TRIGGER *tr = NULL;
  FILE *fp;
  int ntr;
  char txt[100];
  if ((fp = fopen ("trig.dat", "r")) == NULL)
    {
      return;
    }

  for (;;)
    {
      strcpy (txt, fread_word (fp));
      if (!str_cmp (txt, "END"))
        break;
      if (!str_cmp (txt, "ID"))
        {
          strcpy (txt, fread_string2 (fp, NULL));
          tr = new_trigger (txt);
          continue;
        }

      if (!str_cmp (txt, "TT"))
        {
          ntr = fread_number (fp);
          remove_trigger_from_list (tr);
          tr->trigger_type = ntr;
          tr->next = trigger_list[tr->trigger_type];
          trigger_list[tr->trigger_type] = tr;
          continue;
        }

      if (!str_cmp (txt, "Keywords"))
        {
          tr->keywords = fread_string (fp, NULL);
          continue;
        }
      if (!str_cmp (txt, "Room"))
        {
          tr->attached_to_room = fread_number (fp);
          continue;
        }
      if (!str_cmp (txt, "Mob"))
        {
          tr->attached_to_mob = fread_number (fp);
          continue;
        }
      if (!str_cmp (txt, "Obj"))
        {
          tr->attached_to_obj = fread_number (fp);
          continue;
        }
      if (!str_cmp (txt, "Plo"))
        {
          tr->players_only = fread_number (fp);
          continue;
        }
      if (!str_cmp (txt, "Clr"))
        {
          tr->leaves_room = fread_number (fp);
          continue;
        }
      if (!str_cmp (txt, "Int"))
        {
          tr->interrupted = fread_number (fp);
          continue;
        }
      if (!str_cmp (txt, "Code"))
        {
          strcpy (tr->code_label, fread_string2 (fp, NULL));
          continue;
        }
    }

  fclose (fp);
  return;
}



void
save_code (void)
{
  FILE *fp;
  CODE *c;
  int i;
  if ((fp = fopen ("code.dat", "w+")) == NULL)
    {
      fprintf (stderr, "Error on code write open!!  NOT GOOD!\n");
      return;
    }
  for (i = 0; i < 256; i++)
    {
      for (c = code_list[i]; c != NULL; c = c->next)
        {
          fprintf (fp, "LABEL %s~\n", c->label);
          if (c->code)
            fprintf (fp, "CODE %s~\n", fix_string (c->code));
          else
            fprintf (fp, "CODE done~\n");
        }
    }
  fprintf (fp, "END\n");
  fclose (fp);
  return;
}

void
load_code (void)
{
  CODE *c = NULL;
  char txt[1100];
  FILE *fp;
  if ((fp = fopen ("code.dat", "r")) == NULL)
    {
      return;
    }

  for (;;)
    {
      strcpy (txt, fread_word (fp));
      if (!str_cmp (txt, "END"))
        break;
      if (!str_cmp (txt, "LABEL"))
        {
          strcpy (txt, fread_string2 (fp, NULL));
          c = new_code (txt);
          continue;
        }
      if (!str_cmp (txt, "CODE"))
        {
          c->code = fread_string (fp, NULL);
          continue;
        }
    }

  fclose (fp);
  return;
}


/* ----------------------------------------------------- */
/* Stuff below here is for actually running the scripts! */
/* ----------------------------------------------------- */

CHAR_DATA *debugger = NULL;     /* This holds who debugging should go to */
char e_buf[500];
int executed_lines = 0;         /* Keeps track of lines executed.. traps repeating loops */


void
debug (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("debug", debug, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Sets all script error messages to go to you.")
    debugger = ch;
  send_to_char ("You now will recieve all script error messages.\n\r", ch);
  return;
}



void
end_script (SCRIPT_INFO * scr)
{
  SCRIPT_INFO *ls;
  MARK_DEBUG ("End_script")
    scr->called_by->running_info = NULL;
  if (scr == info_list)
    {
      info_list = scr->next;
      scr->next = NULL;
    }
  else
    for (ls = info_list; ls != NULL; ls = ls->next)
      {
        if (ls->next == scr)
          {
            ls->next = scr->next;
            scr->next = NULL;
            break;
          }
      }
  free (scr);
  return;
}

void
e_goto_invalid_line_number (SCRIPT_INFO * scr, char *ln, int linenum)
{
  sprintf (e_buf, "Invalid goto line number '%s' in code label %s, line %d.  Script ended.\n\r",
           ln, scr->code_seg, linenum);
  if (debugger)
    send_to_char (e_buf, debugger);
  end_script (scr);
  return;
}

void
e_couldnt_find_label (SCRIPT_INFO * scr, char *lab, int linenum)
{
  sprintf (e_buf, "Couldn't find code label %s, line %d, in trigger %s.  Script ended.\n\r",
           lab, linenum, scr->called_by->trigger_id);
  if (debugger)
    send_to_char (e_buf, debugger);
  end_script (scr);
  return;
}

void
e_bad_wait_value (SCRIPT_INFO * scr, char *cmd)
{
  sprintf (e_buf, "Bad wait value of '%s', line %d, code label %s.  Script ended.\n\r",
           cmd, scr->current_line - 1, scr->code_seg);
  if (debugger)
    send_to_char (e_buf, debugger);
  end_script (scr);
  return;
}

void
e_bad_wait_type (SCRIPT_INFO * scr, char *typ)
{
  sprintf (e_buf, "Bad wait type '%s', line %d, code label %s.  Script ended.\n\r",
           typ, scr->current_line - 1, scr->code_seg);
  if (debugger)
    send_to_char (e_buf, debugger);
  end_script (scr);
  return;
}

void
e_repeating_loop (SCRIPT_INFO * scr)
{
  sprintf (e_buf, "Apparent repeating loop in code label %s, trigger %s.  Script ended.\n\r",
           scr->code_seg, scr->called_by->trigger_id);
  if (debugger)
    send_to_char (e_buf, debugger);
  end_script (scr);
  return;
}

void
e_invalid_create_obj (SCRIPT_INFO * scr, char *ln)
{
  sprintf (e_buf, "Invalid create object vnum '%s', code label %s line %d.  Script ended.\n\r",
           ln, scr->code_seg, scr->current_line - 1);
  if (debugger)
    send_to_char (e_buf, debugger);
  end_script (scr);
  return;
}

void
e_invalid_create_mob (SCRIPT_INFO * scr, char *ln)
{
  sprintf (e_buf, "Invalid create mob vnum '%s', code label %s line %d.  Script ended.\n\r",
           ln, scr->code_seg, scr->current_line - 1);
  if (debugger)
    send_to_char (e_buf, debugger);
  end_script (scr);
  return;
}

void
e_bad_if (SCRIPT_INFO * scr, char *ln)
{
  sprintf (e_buf, "Invalid if statement '%s', code label %s line %d.  Script ended.\n\r",
           ln, scr->code_seg, scr->current_line - 1);
  if (debugger)
    send_to_char (e_buf, debugger);
  end_script (scr);
  return;
}

void
e_bad_destroy_num (SCRIPT_INFO * scr, char *ln)
{
  sprintf (e_buf, "Bad destroy vnum: %s, code label %s line %d.  Script ended.\n\r",
           ln, scr->code_seg, scr->current_line - 1);
  if (debugger)
    send_to_char (e_buf, debugger);
  end_script (scr);
  return;
}



char *
substitue_vars (SCRIPT_INFO * scr, char *ln)
{
  static char curln[8000];
  char *t;
  int pos = 0;
  curln[0] = '\0';
  MARK_DEBUG ("Substitute_vars")
    for (t = ln; *t != '\0'; t++)
    {
      if (*t == '@')
        {
          t++;
          if (*t == '\0')
            break;
          if (UPPER (*t) == 'C')
            {
              char toad[8000];
              toad[0] = '\0';
              curln[pos] = '\0';
              t++;
              if (*t == 'd') {
                sprintf(toad,"%d",time_info.day);
                } else
              if (*t == 't') {
                sprintf(toad,"%d",time_info.hour);
                } else
              if (*t == 'n') {
                sprintf(toad,"%d",time_info.month);
                } else
              if (*t == 'r') {
                sprintf(toad,"%d",scr->current->pcdata->remort_times);
                } else
              if (*t == 'K') {
                sprintf(toad,"%d",scr->current->pcdata->totalkills);
                } else

              if (*t == 'k') {
                sprintf(toad,"%d",scr->current->pcdata->killpoints);
                } else
              if (*t == 'c') {
                sprintf(toad,"%d",clan_number(scr->current));
                } else
              if (*t == 'C') {
                sprintf(toad,"%d",clan_number_2(scr->current));
                } else
              if (*t == 'm') {
                sprintf(toad,"%d",scr->current->move);
                } else
              if (*t == 'M') {
                sprintf(toad,"%d",scr->current->max_move);
	      } else
		  if (*t == 'H') {
                sprintf(toad,"%d",scr->current->max_hit);
		  } else
		    if (*t == 'h') {
                sprintf(toad,"%d",scr->current->hit);
	      } else
              if (*t == 'w') {
                sprintf(toad,"%d",(IS_PLAYER(scr->current)?scr->current->pcdata->warpoints:0));
	      } 
              else if (*t == 'l')
                {
                          sprintf(toad,"%d",LEVEL(scr->current));

                }

              strcat (curln, toad);
              pos += strlen (toad);

              continue;
            }
          if (UPPER (*t) == 'V')
            {
                char vv[500];
              curln[pos] = '\0';
        if (scr->obj && scr->obj->pIndexData->item_type==ITEM_CORPSE_NPC) {
                sprintf(vv,"%d",((I_CONTAINER *) (scr->obj->more))->key_vnum);
                strcat(curln,vv);
                pos += strlen(vv);
                }
                else
              if (scr->mob)
                {
                          char tmp[50];
                          sprintf(tmp,"%d",scr->mob->pIndexData->vnum);
                  strcat (curln,tmp);
                  pos += strlen (tmp);
                }
              else if (scr->obj)
                {
                          char tmp[50];
                          sprintf(tmp,"%d",scr->obj->pIndexData->vnum);
                  strcat (curln, tmp);
                  pos += strlen (tmp);
                }
              else if (scr->room)
                {
                          char tmp[50];
                          sprintf(tmp,"%d",scr->room->vnum);
                  strcat (curln, tmp);
                  pos += strlen (tmp);
                }
              else
                continue;
            }
          if (UPPER (*t) == 'P')
            {                   /* Player name */
              curln[pos] = '\0';
              if (scr->current)
                {
                  strcat (curln, NAME (scr->current));
                  pos += strlen (NAME (scr->current));
                }
              continue;
            }
          if (UPPER (*t) == 'M')
            {                   /* Mob name */
              curln[pos] = '\0';
              if (scr->mob)
                {
                  strcat (curln, NAME (scr->mob));
                  pos += strlen (NAME (scr->mob));
                }
              continue;
            }
          if (UPPER (*t) == 'O')
            {                   /* Obj name */
              curln[pos] = '\0';
              if (scr->obj)
                {
                  strcat (curln, scr->obj->pIndexData->short_descr);
                  pos += strlen (scr->obj->pIndexData->short_descr);
                }
              continue;
            }
          if (UPPER (*t) == 'R')
            {                   /* Random number eg... @R(1,10) */
              char num1[50];
              char num2[50];
              int n1;
              int n2;
              int rn;
              int tt = 0;
              curln[pos] = '\0';
              t++;
              if (*t != '(')
                continue;
              t++;
              for (; *t != ',' && *t != ')'; t++)
                {
                  num1[tt] = *t;
                  tt++;
                }
              num1[tt] = '\0';
              t++;
              tt = 0;
              for (; *t != ')'; t++)
                {
                  if (*t != ' ')
                    num2[tt] = *t;
                  tt++;
                }
                  {
                  char tmp[50];
              num2[tt] = '\0';
              n1 = atoi (num1);
              n2 = atoi (num2);
              rn = number_range (n1, n2);
                  sprintf(tmp,"%d",rn);
              strcat (curln, tmp);
              pos += strlen (tmp);
              continue;
                  }
            }
          continue;
        }
      curln[pos] = *t;
      pos++;
    }

  curln[pos] = '\0';
  return curln;
}




void
execute_code (SCRIPT_INFO * scr)
{
  CODE *c;
  char cur_line[5000];
  char arg1[5000];
  char curl2[5000];
  char *ln;
  int i;
  int linenum;
  char *t;
  char argg[5000];
  executed_lines = 0;

  MARK_DEBUG ("Execute_code")

    if (!scr->current || !scr->called_by || !scr->current->in_room)
    {
      end_script (scr);
      return;
    }
/*sprintf(cl,":Current (%s) (%d)",NAME(scr->current),scr->current->in_room->vnum);
sprintf(cl+strlen(cl),":Called_by (%s)",scr->called_by->trigger_id);
*/
/* Halt script if character left the room and the script says to do so */
  if (scr->called_by && scr->called_by->leaves_room &&
      ((scr->mob && scr->mob->in_room != scr->current->in_room) ||
       (scr->room && scr->room != scr->current->in_room)))
    {
      end_script (scr);
      return;
    }
execute:                        /* Main execute loop */

  cur_line[0] = '\0';
  i = 0;
  linenum = 0;
  t = NULL;
  ln = NULL;
  arg1[0] = '\0';
  scr->tick_type = 0;
  scr->delay_ticks = 0;

  c = NULL;

  if ((c = find_code (scr->code_seg)) == NULL)
    {
      e_couldnt_find_label (scr, scr->code_seg, 0);
      return;
    }



  if (scr->current_line == 0)
    scr->current_line = 1;

  for (t = c->code; *t != '\0'; t++)
    {
      if (*t == ':')
        linenum++;
      if (linenum == scr->current_line)
        {
          t++;
          for (; *t != ' ' && *t != ':' && *t != '\0'; t++)
            {
            }
          if (*t != '\0')
            t++;
          for (; *t != '\n' && *t != ':' && *t != '\0'; t++)
            {
              cur_line[i] = *t;
              i++;
            }
          cur_line[i] = '\0';
          goto foundit;
        }
    }
  end_script (scr);             /* Past the max line numbers */
  return;

foundit:

  strcpy (curl2, substitue_vars (scr, cur_line));
  strcpy (cur_line, curl2);
  one_argy (curl2, argg);
  ln = cur_line;

/*sprintf(cl,"Scriptline: %s\n",cur_line);
*/
  scr->current_line++;          /* Increment 'program counter' */

/* Begin interpretting commands */

  if (!str_cmp (argg, "call"))
    {                           /* Call <other code label> */
        char clab[500];
      ln = one_argy (ln, arg1);
      if ((c = find_code (ln)) == NULL)
        {
          e_couldnt_find_label (scr, ln, scr->current_line - 1);
          return;
        }
      strcpy(clab,scr->code_seg);
      strcpy (scr->code_seg, ln);
      scr->current_line = 1;
        execute_code(scr);
        strcpy(scr->code_seg,clab);
        return;
    }

  else if (!str_cmp (argg, "goto"))
    {
      int totl = 0;
      int got = 0;
      char *tt;
      ln = one_argy (ln, arg1);
      for (tt = c->code; *tt != '\0'; tt++)
        {
          if (*tt == ':')
            totl++;
        }
      if (!is_number (ln))
        {
          e_goto_invalid_line_number (scr, ln, scr->current_line - 1);
          return;
        }
      got = atoi (ln);
      if (got < 1 || got > totl)
        {
          e_goto_invalid_line_number (scr, ln, scr->current_line - 1);
          return;
        }
      scr->current_line = got;
    }

  else if (!str_cmp (argg, "wait"))
    {
      char *l;
      char rg1[500];
      char rg2[500];
      int wait;
      l = cur_line;
      l = one_argy (l, rg1);
      l = one_argy (l, rg2);    /* rg1 holds wait, rg2 holds number, l holds tick type */
      if ((wait = atoi (rg2)) < 1)
        {
          e_bad_wait_value (scr, rg2);
          return;
        }
      if (!str_cmp (l, "superquick"))
        {
          scr->tick_type = 4;
          scr->delay_ticks = wait;
        }
      else if (!str_cmp (l, "quick"))
        {
          scr->tick_type = 1;
          scr->delay_ticks = wait;
        }
      else if (!str_prefix ("heart", l))
        {
          scr->tick_type = 2;
          scr->delay_ticks = wait;
        }
      else if (!str_cmp (l, "hour"))
        {
          scr->tick_type = 3;
          scr->delay_ticks = wait;
        }
      else
        {
          e_bad_wait_type (scr, l);
        }
      return;
    }

  else if (!str_cmp (cur_line, "done"))
    {
      end_script (scr);
      return;
    }

  else if (!str_prefix ("create_mob", argg))
    {
      int mnum;
      MOB_PROTOTYPE *m;
      CHAR_DATA *mob;
      char *l = cur_line;
      l = one_argy (l, argg);
      if (!is_number (l))
        {
          e_invalid_create_mob (scr, l);
          return;
        }
      mnum = atoi (l);
      if ((m = get_mob_index (mnum)) == NULL)
        {
          e_invalid_create_mob (scr, l);
          return;
        }
      mob = create_mobile (m);
      /* Created the mob.. now place in room.. */
      if (scr->mob)
        {
          char_to_room (mob, scr->mob->in_room);
        }
      else if (scr->room)
        {
          char_to_room (mob, scr->room);
        }
      if (scr->obj)
        {
          if (scr->obj->carried_by != NULL)
            char_to_room (mob, scr->current->in_room);
          else if (scr->obj->in_room != NULL)
            char_to_room (mob, scr->obj->in_room);
        }
    }

  else if (!str_prefix ("destroy_mob", argg))
    {
      char *l = cur_line;
      l = one_argy (l, argg);
      if (scr->current)
        {
          stop_fighting (scr->current, TRUE);
          char_from_room (scr->current);
          extract_char (scr->current, TRUE);
          return;
        }
    }

  else if (!str_prefix ("destroy_obj", argg))
    {
      char *l = cur_line;
      OBJ_PROTOTYPE *op;
      l = one_argy (l, argg);
      if (scr->obj)
        {
          obj_from (scr->obj);
          free_it (scr->obj);   /*free_it also stops ends script! */
          return;
        }
      if (!is_number (l))
        {
          e_bad_destroy_num (scr, l);
          return;
        }
      if ((op = get_obj_index (atoi (l))) == NULL)
        {
          e_bad_destroy_num (scr, l);
          return;
        }
      if (scr->mob)
        {
          SINGLE_OBJECT *o;
          SINGLE_OBJECT *o_next_content;
          bool des = FALSE;
          for (o = scr->mob->carrying; o != NULL && !des; o = o_next_content)
            {
              o_next_content = o->next_content;
              if (o->pIndexData == op)
                {
                  obj_from (o);
                  free_it (o);
                  des = TRUE;
                }
            }
        }
      else if (scr->room)
        {
          SINGLE_OBJECT *o;
          SINGLE_OBJECT *o_next_content;
          bool des = FALSE;
          if (scr->room->more)
            for (o = scr->room->more->contents; o != NULL && !des; o = o_next_content)
              {
                o_next_content = o->next_content;
                if (o->pIndexData == op)
                  {
                    obj_from (o);
                    free_it (o);
                    des = TRUE;
                  }
              }
        }
    }

  else if (!str_prefix ("create_obj_char", argg))
  {
      int onum;
      OBJ_PROTOTYPE *o;
      SINGLE_OBJECT *obj;
      char *l = cur_line;
      l = one_argy (l, argg);
      if (!is_number (l))
        {
          e_invalid_create_obj (scr, l);
          return;
        }
      onum = atoi (l);
      if ((o = get_obj_index (onum)) == NULL)
        {
          e_invalid_create_obj (scr, l);
          return;
        }
      obj = create_object (o, 1);
      if (scr->current)
        {
        obj_to (obj, scr->current);
        }
    }

  else if (!str_prefix ("create_obj", argg))
    {
      int onum;
      OBJ_PROTOTYPE *o;
      SINGLE_OBJECT *obj;
      char *l = cur_line;
      l = one_argy (l, argg);
      if (!is_number (l))
        {
          e_invalid_create_obj (scr, l);
          return;
        }
      onum = atoi (l);
      if ((o = get_obj_index (onum)) == NULL)
        {
          e_invalid_create_obj (scr, l);
          return;
        }
      obj = create_object (o, 1);
      /* Created the object.. now place in room, on char, on mob.. */
      if (scr->mob)
        {
          obj_to (obj, scr->mob);
        }
      else if (scr->room)
        {
          obj_to (obj, scr->room);
        }
      if (scr->obj)
        {
          if (scr->obj->carried_by != NULL)
            obj_to (obj, scr->obj->carried_by);
          else if (scr->obj->in_room != NULL)
            obj_to (obj, scr->obj->in_room);
          else if (scr->obj->in_obj != NULL)
            obj_to (obj, scr->obj->in_obj);
        }
    }

  else if (!str_cmp (argg, "if"))
    {
      char *l = cur_line;
      char *tc;
      char t;
      char buf[500];
      char leftnum[50];
      int cci = 0;
      int count = 0;
      bool beq = FALSE;
      bool bge = FALSE;
      bool bgt = FALSE;
      bool ble = FALSE;
      bool blt = FALSE;
      bool bne = FALSE;
      char rightnum[50];
      l = one_argy (l, argg);
      if (*l == 'D')
        {
          int ones;
          l++;
	  if (*l == 'W')
	  {
	     l++;
	     {
		int number=atoi(l);
	        if (scr->current->pcdata->warpoints >= number)
		  scr->current_line++;
	     }
	  }
	  if (*l == 'A')
	  {
              l++;
              if (ALIGN(scr->current) == atoi(l))
              	scr->current_line++;
	  }
/* Added in check for hardcore Bern */
        if (*l == 'H')
        {
              l++;
              if (IS_SET(scr->current->pcdata->act2, PLR_HARDCORE))
                  scr->current_line++;
        }
	  if (*l == 'G')
	     {
		l++;
		{
		int number = atoi(l);
		int guild = -1;
		switch (number)
		  {
		  case 0:
		    guild = GUILD_WARRIOR;
		    break;
		  case 1:
		    guild = GUILD_BATTLEMASTER;
		    break;
		  case 2:
		    guild = GUILD_THIEFG;
		    break;
		  case 3:
		    guild = GUILD_ROGUE;
		    break;
		  case 4:
		    guild = GUILD_WIZARD;
		    break;
		  case 5:
		    guild = GUILD_CONJURER;
		    break;
		  case 6:
		    guild = GUILD_HEALER;
		    break;
		  case 7:
		    guild = GUILD_MYSTIC;
		    break;
		  case 8:
		    guild = GUILD_RANGER;
		    break;
		  case 9:
		    guild = GUILD_TINKER;
		    break;
		  default:
		    break;
		};



		if (guild > -1)
		{
		    if (!is_member(scr->current, guild))
			scr->current_line++;
		}
		}
	     }
/* Added in check for Monk and Necro-Bern */
	  if (*l == 'I')
	     {
		l++;
		{
		int number = atoi(l);
		int guild = -1;
		switch (number)
		  {
		  case 0:
		    guild = GUILD_NECROMANCER;
		    break;
              case 1:
                guild = GUILD_MONK;
                break;
            		  default:
		    break;
		};



		if (guild > -1)
		{
		    if (!is_member(scr->current, guild))
			scr->current_line++;
		}
		}
	     }
           
         }                     
    else
      {
      if (*l == '(')
        {
          int st;
          bool ne = FALSE;
          l++;
     
		  if (*l == '!')
            {
              ne = TRUE;
              l++;
            }
          
		  for (tc = l; *tc != ')'; tc++)
		    {
              if (*tc == '\0')
                {
                  e_bad_if (scr, cur_line);
                  return;
                }
              leftnum[cci] = *tc;
              cci++;
            }
          
		  leftnum[cci] = '\0';

          for (st = 0; st < MAX_SCRIPT_FLAGS; st++)
            {
              if (!scr->current->pcdata->script_flags[st] || scr->current->pcdata->script_flags[st][0] == '\0')
                continue;
              if (!str_cmp (scr->current->pcdata->script_flags[st], leftnum))
                goto matched;
            }
          scr->current_line++;
          goto dunw;
        matched:
          if (ne)
            scr->current_line++;
        dunw:
          if (scr) {};
        }
      else
        {
          for (tc = l; *tc != '=' && *tc != '>' && *tc != '<' && *tc != '!'; tc++)
            {
              if (*tc == '\0')
                {
                  e_bad_if (scr, cur_line);
                  return;
                }
              if (*tc == ' ')
                continue;
              leftnum[cci] = *tc;
              cci++;
            }
          leftnum[cci] = '\0';

          if (*tc == '=')
            {
              if (*(tc + 1) == '=')
                tc++;
              beq = TRUE;
            }
          else if (*tc == '>')
            {
              if (*(tc + 1) == '=')
                {
                  tc++;
                  bge = TRUE;
                }
              else
                bgt = TRUE;
            }
          else if (*tc == '<')
            {
              if (*(tc + 1) == '=')
                {
                  tc++;
                  ble = TRUE;
                }
              else
                blt = TRUE;
            }
          else if (*tc == '!')
            {
              if (*(tc + 1) == '=')
                {
                  tc++;
                }
              bne = TRUE;
            }
          else
            {
              e_bad_if (scr, cur_line);
              return;
            }

          tc++;
          cci = 0;

          for (; *tc != ' '; tc++)
            {
              if (*tc == '\0')
                {
                  e_bad_if (scr, cur_line);
                  return;
                }
              rightnum[cci] = *tc;
              cci++;
            }
          rightnum[cci] = '\0';

          if (is_number (leftnum))
            {
              int lnm;
              int rnm;
              lnm = atoi (leftnum);
              rnm = atoi (rightnum);
              if (beq && lnm != rnm)
                {
                  scr->current_line++;
                }
              else if (bgt && lnm <= rnm)
                {
                  scr->current_line++;
                }
              else if (bge && lnm < rnm)
                {
                  scr->current_line++;
                }
              else if (blt && lnm >= rnm)
                {
                  scr->current_line++;
                }
              else if (ble && lnm > rnm)
                {
                  scr->current_line++;
                }
              else if (bne && lnm == rnm)
                {
                  scr->current_line++;
                }
            }
          else
            {
              if (bne && !str_cmp (leftnum, rightnum))
                {
                  scr->current_line++;
                }
              else if (!bne && str_cmp (leftnum, rightnum))
                {
                  scr->current_line++;
                }
            }
        }
    }
   }
  else if (!str_cmp (argg, "recho"))
    {
      char *l = cur_line;
      l = one_argy (l, argg);
      do_recho (scr->current, l);
    }

  else if (!str_cmp (argg, "show"))
    {
      char *l = cur_line;
      l = one_argy (l, argg);
      send_to_char (l, scr->current);
      send_to_char ("\n\r", scr->current);
    }

  else if (!str_cmp (argg, "show_to_room"))
    {
      char *l = cur_line;
      l = one_argy (l, argg);
      act (l, scr->current, NULL, scr->current, TO_ROOM);
    }

  else if (scr->mob)
    {
      interpret (scr->mob, cur_line);
    }

  else if (scr->room || scr->obj)
    {                           /* Warning.. should ONLY be used for pset, etc.. not recho! */
      CHAR_DATA *sch;
      sch=create_mobile(get_mob_index(1));
      if (scr->room) char_to_room(sch,scr->room);
         else char_to_room(sch,get_room_index(1));
      interpret (sch, cur_line);
        extract_char(sch,TRUE);
    }

/* End interpretting commands */

  executed_lines++;
  if (executed_lines > 300)
    {
      e_repeating_loop (scr);
      return;
    }

  goto execute;

  return;
}

char *
string_unpad (char *argy)
{
  char buf[STD_LENGTH];
  char *s;
  s = argy;
  while (*s == ' ')
    s++;
  strcpy (buf, s);
  s = buf;
  if (*s != '\0')
    {
      while (*s != '\0')
        s++;
      s--;
      while (*s == ' ')
        s--;
      s++;
      *s = '\0';
    }
  free_string (argy);
  return str_dup (buf);
}

char *
string_proper (char *argy)
{
  char *s;
  s = argy;
  while (*s != '\0')
    {
      if (*s != ' ')
        {
          *s = UPPER (*s);
          while (*s != ' ' && *s != '\0')
            s++;
        }
      else
        {
          s++;
        }
    }
  return argy;
}

char *
string_replace (char *orig, char *old, char *new)
{
  char xbuf[STD_LENGTH];
  int i;
  xbuf[0] = '\0';
  strcpy (xbuf, orig);
  if (strstr (orig, old) != NULL)
    {
      i = strlen (orig) - strlen (strstr (orig, old));
      xbuf[i] = '\0';
      strcat (xbuf, new);
      strcat (xbuf, &orig[i + strlen (old)]);
      free_string (orig);
    }
  return str_dup (xbuf);
}

void
string_edit (CHAR_DATA * ch, char **pString)
{
  send_to_char ("-=======================================-\n\r", ch);
  send_to_char (" Entering line editing mode.\n\r", ch);
  send_to_char (" Terminate with a @ on a blank line.\n\r", ch);
  send_to_char ("-=======================================-\n\r", ch);
  if (*pString == NULL)
    {
      *pString = str_dup ("");
    }
  else
    {
      free_string (*pString);
      *pString = str_dup ("");
    }
  ch->desc->pString = pString;
  return;
}

void
string_append (CHAR_DATA * ch, char **pString)
{
  send_to_char ("------------------------ LINE Editor -------------------------\n\r", ch);
  send_to_char (" Type .c to clear the whole text, .f to format the text,\n\r", ch);
  send_to_char (" .r 'orig' 'new' to replace, and .s to view the string so far.\n\r", ch);
  send_to_char (" Use @ on a blank line to save and quit the editor.\n\r", ch);
  send_to_char ("--------------------------------------------------------------\n\r", ch);
  if (*pString == NULL)
    {
      *pString = str_dup ("");
    }
  send_to_char ("Editing description:\n\r", ch);
  send_edit_char (*pString, ch);
  send_to_char ("-=======================================-\n\r", ch);
  ch->desc->pString = pString;
  return;
}

void
string_add (CHAR_DATA * ch, char *argy)
{
  char *ii;
  char buf[40000];
  if (!ch->desc)
    return;
  if (!ch->pcdata->name)
    return;
  if (!argy || argy == "")
    return;
  else /* JRAJRA */
    {
      for (ii = argy; *ii != '\0'; ii++ )
	{
	  if (*ii == '{' || *ii == '}')
	    {
	      send_to_char ("No curly braces allowed!.\n\r", ch);
	      return;
	    }
	}
    }
  if (*argy == ':')
    {
      char texts[202][255];
      char *lne;
      char *t;
      int i = 0;
      int mrk = 0;
      int curline = -1;
      int linenum = 0;
      buf[0] = '\0';
      for (t = argy; *t != ' ' && *t != '\0'; t++)
        {
          mrk++;
        }
      *t = '\0';
      lne = (argy + 1);
      if ((linenum = atoi (lne)) < 1 || linenum > 199)
        {
          send_to_char ("Invalid line number!\n\r", ch);
          return;
        }
      argy += mrk + 1;
      for (t = *ch->desc->pString; *t != '\0'; t++)
        {
          if (*t == ':' || t == *ch->desc->pString)
            {
              for (; *(t + 1) != '\0' && *t != ' '; t++)
                {
                };
              if (curline >= 0)
                texts[curline][i] = '\0';
              curline++;
              i = 0;
              continue;
            }
          if (curline < 0)
            curline = 0;
          if (*t == '\r')
            continue;
          if (*t == '\n')
            continue;
          texts[curline][i] = *t;
          i++;
        }
      texts[curline][i] = '\0';

      /* Insert into correct place */
      for (i = curline; i >= linenum - 1; i--)
        {
          strcpy (texts[i + 1], texts[i]);
        }
      strcpy (texts[linenum - 1], argy);
      curline++;

      /* Now reconstruct in the char buffer */
      buf[0] = '\0';
      for (i = 0; i <= curline; i++)
        {
          sprintf (buf + strlen (buf), ":%d ", i + 1);
          strcat (buf, texts[i]);
        }
      strcat (buf, "\n\r");
      free_string (*ch->desc->pString);
      *ch->desc->pString = str_dup (buf);
      return;
    }
  if (*argy == '.')
    {
      char arg1[SML_LENGTH];
      char arg2[SML_LENGTH];
      char arg3[SML_LENGTH];
      argy = one_argy (argy, arg1);
      argy = first_arg (argy, arg2, FALSE);
      argy = first_arg (argy, arg3, FALSE);

      if (!str_cmp (arg1, ".c"))
        {
          if (is_number (arg2))
            {
              char texts[202][150];
              int i = 0;
              char *t;
              int curline = -1;
              int linenum = atoi (arg2);
              buf[0] = '\0';
              if (linenum < 1 || linenum > 199)
                {
                  send_to_char ("Invalid line number!\n\r", ch);
                  return;
                }
              for (t = *ch->desc->pString; *t != '\0'; t++)
                {
                  if (*t == ':' || t == *ch->desc->pString)
                    {
                      for (; *(t + 1) != '\0' && *t != ' '; t++)
                        {
                        };
                      if (curline >= 0)
                        texts[curline][i] = '\0';
                      curline++;
                      i = 0;
                      continue;
                    }
                  if (curline < 0)
                    curline = 0;
                  if (*t == '\r')
                    continue;
                  if (*t == '\n')
                    continue;
                  texts[curline][i] = *t;
                  i++;
                }
              texts[curline][i] = '\0';
              buf[0] = '\0';
              for (i = linenum; i <= curline; i++)
                {
                  strcpy (texts[i - 1], texts[i]);
                }
              texts[i][0] = '\0';
              curline--;
              buf[0] = '\0';
              for (i = 0; i <= curline; i++)
                {
                  sprintf (buf + strlen (buf), ":%d ", i + 1);
                  strcat (buf, texts[i]);
                }
              strcat (buf, "\n\r");
              free_string (*ch->desc->pString);
              *ch->desc->pString = str_dup (buf);
              return;
            }
          send_to_char ("String cleared.\n\r", ch);
          free_string (*ch->desc->pString);
          *ch->desc->pString = str_dup ("");
          return;
        }
      if (!str_cmp (arg1, ".s"))
        {
          send_to_char ("String so far:\n\r", ch);
          send_edit_char (*ch->desc->pString, ch);
          return;
        }
        if (!str_cmp (arg1, ".r"))
        {
          if (arg2[0] == '\0')
            {
              send_to_char ("usage: .r \"old text\" \"new text\"\n\r", ch);
              return;
            }
		  //ARC: check over all size before replace, no notes longer than STD_LENTGH allowed
		  if(strlen(*ch->desc->pString)+strlen(arg3) > STD_LENGTH)
		  {
			  send_to_char ("The note would become too long.\n\r", ch);
			  return;
		  }

          *ch->desc->pString = string_replace (*ch->desc->pString, arg2, arg3);
          sprintf (buf, "'%s' replaced with '%s'.\n\r", arg2, arg3);
          send_to_char (buf, ch);
          return; 
        }
      if (!str_cmp (arg1, ".f"))
        {
          *ch->desc->pString = format_string (*ch->desc->pString);
          send_to_char ("String formatted.\n\r", ch);
          return;
        }
      if (!str_cmp (arg1, ".h"))
        {
          send_to_char ("Help (commands): \n\r", ch);
          send_to_char (".r 'old text' 'new text' - replace a string with a new string \n\r", ch);
          send_to_char (" (requires '' OR \"\" around the argys)\n\r", ch);
          send_to_char (".h - this info \n\r", ch);
          send_to_char (".s - show the string \n\r", ch);
          send_to_char (".f - format (word wrap/capitalize and space) string \n\r", ch);
          send_to_char (".c - clear the entire string! \n\r", ch);
          send_to_char ("@ - end string editor \n\r", ch);
          return;
        }
      send_to_char ("Command unrecognized.\n\r", ch);
      return;
    }
  if (*argy == '~' || *argy == '@')
    {
      ch->desc->pString = NULL;
      return;
    }
  if (LEVEL (ch) < MAX_LEVEL && *ch->desc->pString && strlen (*ch->desc->pString) > 5000)
    {
      send_to_char ("Edited text too long. Aborting.\n\r", ch);
      free_string (*ch->desc->pString);
      *ch->desc->pString = &str_empty[0];
      ch->desc->pString = NULL;
      return;
    }
  strcpy (buf, *ch->desc->pString);
  if (strlen(buf)>4096) {
      send_to_char ("String too long. Aborting.\n\r", ch);
      free_string (*ch->desc->pString);
      *ch->desc->pString = str_dup("");
      ch->desc->pString = NULL;
      return;
  }

  if (strlen (buf) + strlen (argy) >= (STD_LENGTH - 4))
    {
      send_to_char ("String too long. Aborting.\n\r", ch);
      free_string (*ch->desc->pString);
      *ch->desc->pString = str_dup("");
      ch->desc->pString = NULL;
      return;
    }
  strcat (buf, argy);
  strcat (buf, "\n\r");
  free_string (*ch->desc->pString);
  *ch->desc->pString = str_dup (buf);
  return;
}

char *
format_string (char *oldstring)
{
  char xbuf[40000];
  char xbuf2[40000];
  char *rdesc;
  char *t;
  int i = 0;
  bool cap = TRUE;
  xbuf[0] = xbuf2[0] = 0;
  i = 0;
  for (t = oldstring; *t; t++)
    {
      if (*t == ':' && *(t + 1) >= '0' && *(t + 1) <= '9')
        {
          xbuf[0] = '\0';
          i = 0;
          for (t = oldstring; *t; t++)
            {
              if (*t == '\n' || *t == '\r')
                continue;
              xbuf[i] = *t;
              i++;
            }
          xbuf[i] = '\n';
          xbuf[i + 1] = '\r';
          xbuf[i + 2] = '\0';
          free_string (oldstring);
          return (str_dup (xbuf));
        }
    }
  for (rdesc = oldstring; *rdesc; rdesc++)
    {
      if (*rdesc == '\n')
        {
          if (xbuf[i - 1] != ' ')
            {
              xbuf[i] = ' ';
              i++;
            }
        }
      else if (*rdesc == '\r');
      else if (*rdesc == ' ')
        {
          if (xbuf[i - 1] != ' ')
            {
              xbuf[i] = ' ';
              i++;
            }
        }
      else if (*rdesc == ')')
        {
          if (xbuf[i - 1] == ' ' && xbuf[i - 2] == ' ' &&
           (xbuf[i - 3] == '.' || xbuf[i - 3] == '?' || xbuf[i - 3] == '!'))
            {
              xbuf[i - 2] = *rdesc;
              xbuf[i - 1] = ' ';
              xbuf[i] = ' ';
              i++;
            }
          else
            {
              xbuf[i] = *rdesc;
              i++;
            }
        }
      else if ((*rdesc == '.' && *(rdesc + 1) < '0' && *(rdesc + 1) > '9')
               || *rdesc == '?' || *rdesc == '!')
        {
          if (xbuf[i - 1] == ' ' && xbuf[i - 2] == ' ' &&
           (xbuf[i - 3] == '.' || xbuf[i - 3] == '?' || xbuf[i - 3] == '!'))
            {
              xbuf[i - 2] = *rdesc;
              if (*(rdesc + 1) != '\"')
                {
                  xbuf[i - 1] = ' ';
                  xbuf[i] = ' ';
                  i++;
                }
              else
                {
                  xbuf[i - 1] = '\"';
                  xbuf[i] = ' ';
                  xbuf[i + 1] = ' ';
                  i += 2;
                  rdesc++;
                }
            }
          else
            {
              xbuf[i] = *rdesc;
              if (*(rdesc + 1) != '\"')
                {
                  xbuf[i + 1] = ' ';
                  xbuf[i + 2] = ' ';
                  i += 3;
                }
              else
                {
                  xbuf[i + 1] = '\"';
                  xbuf[i + 2] = ' ';
                  xbuf[i + 3] = ' ';
                  i += 4;
                  rdesc++;
                }
            }
          cap = TRUE;
        }
      else
        {
          xbuf[i] = *rdesc;
          if (cap)
            {
              cap = FALSE;
              xbuf[i] = UPPER (xbuf[i]);
            }
          i++;
        }
    }
  xbuf[i] = 0;
  strcpy (xbuf2, xbuf);
  rdesc = xbuf2;
  xbuf[0] = 0;
  for (;;)
    {
      for (i = 0; i < 77; i++)
        {
          if (!*(rdesc + i))
            break;
        }
      if (i < 77)
        {
          break;
        }
      for (i = (xbuf[0] ? 76 : 73); i; i--)
        {
          if (*(rdesc + i) == ' ')
            break;
        }
      if (i)
        {
          *(rdesc + i) = 0;
          strcat (xbuf, rdesc);
          strcat (xbuf, "\n\r");
          rdesc += i + 1;
          while (*rdesc == ' ')
            rdesc++;
        }
      else
        {
          bug ("No spaces", 0);
          *(rdesc + 75) = 0;
          strcat (xbuf, rdesc);
          strcat (xbuf, "-\n\r");
          rdesc += 76;
        }
    }
  while (*(rdesc + i) && (*(rdesc + i) == ' ' ||
                          *(rdesc + i) == '\n' ||
                          *(rdesc + i) == '\r'))
    i--;
  *(rdesc + i + 1) = 0;
  strcat (xbuf, rdesc);
  if (xbuf[strlen (xbuf) - 2] != '\n')
    strcat (xbuf, "\n\r");
  free_string (oldstring);
  return (str_dup (xbuf));
}

/*
   * Replace a substring with a new substring, return the new version.
   * (make sure to str_dup() when needed)
 */
char *
replace_string (char *orig, char *old, char *new)
{
  char xbuf[STD_LENGTH];
  int i;
  xbuf[0] = '\0';
  strcpy (xbuf, orig);
  if (strstr (orig, old) != NULL)
    {
      i = strlen (orig) - strlen (strstr (orig, old));
      xbuf[i] = '\0';
      strcat (xbuf, new);
      strcat (xbuf, &orig[i + strlen (old)]);
      free_string (orig);
    }
  return (str_dup (xbuf));
}


bool
one_is_of_two (char *looking_for, char *line)
{
  char arg[8000];
  char *r;
  arg[0]='\0';
  while (line[0] != '\0')
    {
      line = one_argy (line, arg);
      r = arg;
      if (!str_cmp (looking_for, r) ||
          (strlen (looking_for) > 4 && !str_prefix (looking_for, r)) ||
          (strlen (looking_for) > 4 && !str_prefix (r, looking_for)) ||
          !str_infix (r, looking_for))
        return TRUE;
    }
  return FALSE;
}

bool
one_is_in_list_two (char *looking_for, char *line)
{
  char arg[8000];
  char *r;
  arg[0]='\0';
  while (line[0] != '\0')
    {
      line = one_argy (line, arg);
      r = arg;
      if (!str_cmp (looking_for, r))
        return TRUE;
    }
  return FALSE;
}

