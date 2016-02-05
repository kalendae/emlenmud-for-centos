/* Property of Owen Emlen and Simon Lee - some borrowed from older code, 90% new */
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
bool is_note_to (CHAR_DATA * ch, NOTE_DATA * pnote);
void note_attach (CHAR_DATA * ch);
void note_remove (CHAR_DATA * ch, NOTE_DATA * pnote);
void note_delete (NOTE_DATA * pnote);

#define MAX_BOARDS 4
char *boards[MAX_BOARDS] = {
"Blank",
"Immortal Notice Board",
"General Chat Board",
"Ideas/Complaint Board"
};

bool 
is_note_to (CHAR_DATA * ch, NOTE_DATA * pnote)
{
  if (!str_cmp (NAME (ch), pnote->sender))
    return TRUE;
  if ((is_name ("all", pnote->to_list)) || is_name ("clan", pnote->to_list))
    return TRUE;
  if (IS_IMMORTAL (ch) && (is_name ("immortal", pnote->to_list) ||is_name ("imm", pnote->to_list) ||is_name ("imms", pnote->to_list) ||is_name ("admin", pnote->to_list) ||is_name ("admins", pnote->to_list)))
    return TRUE;
  if (is_name (NAME (ch), pnote->to_list))
    return TRUE;
  if (LEVEL (ch) >= MAX_LEVEL)
    return TRUE;
  return FALSE;
}

void 
note_attach (CHAR_DATA * ch)
{
  NOTE_DATA *pnote;
  check_ced (ch);
  if (ch->ced->pnote != NULL)
    return;
  if (note_free == NULL)
    {
      pnote = mem_alloc (sizeof (*ch->ced->pnote));
    }
  else
    {
      pnote = note_free;
      note_free = note_free->next;
    }
  pnote->next = NULL;
  pnote->sender = str_dup (NAME (ch));
  pnote->date = str_dup ("");
  pnote->to_list = str_dup ("");
  pnote->subject = str_dup ("");
  pnote->text = str_dup ("");
  pnote->lines = 0;
  ch->ced->pnote = pnote;
  return;
}

void 
note_remove (CHAR_DATA * ch, NOTE_DATA * pnote)
{
  char to_new[SML_LENGTH];
  char to_one[SML_LENGTH];
  FILE *fp;
  NOTE_DATA *prev;
  char *to_list;
/*
   * Build a new to_list.
   * Strip out this recipient.
 */
  to_new[0] = '\0';
  to_list = pnote->to_list;
  while (*to_list != '\0')
    {
      to_list = one_argy (to_list, to_one);
      if (to_one[0] != '\0' && str_cmp (NAME (ch), to_one))
	{
	  strcat (to_new, " ");
	  strcat (to_new, to_one);
	}
    }
/*
   * Just a simple recipient removal?
 */
  if (LEVEL (ch) < MAX_LEVEL && !is_note_to (ch, pnote))
    {
      if (str_cmp (NAME (ch), pnote->sender) && to_new[0] != '\0')
	{
	  free_string (pnote->to_list);
	  pnote->to_list = str_dup (to_new + 1);
	  return;
	}
    }
/*
   * Remove note from linked list.
 */
  if (pnote == note_list)
    {
      note_list = pnote->next;
    }
  else
    {
      for (prev = note_list; prev != NULL; prev = prev->next)
	{
	  if (prev->next == pnote)
	    break;
	}
      if (prev == NULL)
	{
	  bug ("Note_remove: pnote not found.", 0);
	  return;
	}
      prev->next = pnote->next;
    }
  free_string (pnote->text);
  free_string (pnote->subject);
  free_string (pnote->to_list);
  free_string (pnote->date);
  free_string (pnote->sender);
  pnote->next = note_free;
  note_free = pnote;
/*
   * Rewrite entire list.
 */
#ifndef WINDOWS
//  fclose (fpReserve);
#endif
  if ((fp = fopen (NOTE_FILE, "w")) == NULL)
    {
      perror (NOTE_FILE);
    }
  else
    {
      for (pnote = note_list; pnote != NULL; pnote = pnote->next)
	{
	  fprintf (fp, "Sender %s~\n", fix_string (pnote->sender));
	  fprintf (fp, "TBoard %d\n", pnote->board_num);
	  fprintf (fp, "Date %s~\n", pnote->date);
	  fprintf (fp, "Stamp %ld\n", pnote->date_stamp);
	  fprintf (fp, "To %s~\n", fix_string (pnote->to_list));
	  fprintf (fp, "Subject %s~\n", fix_string (pnote->subject));
	  fprintf (fp, "Text\n%s~\n", fix_string (pnote->text));
	  fprintf (fp, "End\n");
	}
      fclose (fp);
    }
#ifndef WINDOWS
//  fpReserve = fopen (NULL_FILE, "r");
#endif
  return;
}

void 
do_note (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH * 17];
  char buf1[STD_LENGTH * 17];
  char arg[SML_LENGTH];
  NOTE_DATA *pnote;
  int vnum;
  int board_here;		/*Boards and board nums by Owen Emlen */
  SINGLE_OBJECT *obj;
  int anum;
  DEFINE_COMMAND ("note", do_note, POSITION_DEAD, 0, LOG_NORMAL, "This command is used to post/read/remove notes.")

    if (IS_MOB (ch))
    return;
  board_here = 0;
  obj = get_obj_here (ch, "board", SEARCH_ROOM_FIRST);
  if (obj == NULL)
    {
      board_here = 1;
    }
  else
    board_here = obj->pIndexData->value[9];
  buf1[0] = '\0';
  argy = one_argy (argy, arg);
  smash_tilde (argy);
  if (arg[0] == '\0')
    {
      do_note (ch, "read");
      return;
    }
  if (!str_cmp (arg, "list"))
    {
      vnum = 0;
      
 	sprintf(buf1,"\x1B[1;30m------------------------------------------------------------------------\x1B[37;0m\n\r");
      sprintf(buf1+strlen(buf1),"\x1B[1;36m New  ###  Sender/From         Subject\x1B[37;0m\n\r");
	sprintf(buf1+strlen(buf1),"\x1B[1;30m------------------------------------------------------------------------\x1B[37;0m\n\r");

      for (pnote = note_list; pnote != NULL; pnote = pnote->next)
	{
	  if (is_note_to (ch, pnote) &&
	      (board_here == pnote->board_num || IS_IMMORTAL (ch)))
	    {
		  if (strlen(pnote->subject)>35) {
			pnote->subject[35]='\0';
			}
	      sprintf (buf, "%s \x1B[1;36m%3d\x1B[37;0m  From \x1b[1;37m%-14s\x1B[37;0m '\x1B[1;36m%s\x1B[37;0m'\n\r",
		       (pnote->date_stamp > ch->pcdata->last_note
			&& str_cmp (pnote->sender, NAME (ch))) ? "\x1B[1;34m(New)\x1B[37;0m" : "     ",
		       vnum,
		pnote->sender, pnote->subject);
	      strcat (buf1, buf);
	      vnum++;
	    }
	  else
	    vnum++;
	  if (strlen (buf1) > (STD_LENGTH * 16))
	    {
	      strcat (buf1, "\n\rTOO MANY NOTES.. WAIT UNTIL A GOD REMOVES SOME!\n\r");
	      break;
	    }
	}
      page_to_char_limited (buf1, ch); 
      return;
    }
  if (!str_cmp (arg, "read"))
    {
      bool fAll;
      if (!str_cmp (argy, "all"))
	{
	  fAll = TRUE;
	  anum = 0;
	}
      else if (argy[0] == '\0' || !str_prefix (argy, "next"))
	/* read next unread note */
	{
	  vnum = 0;
	  for (pnote = note_list; pnote != NULL; pnote = pnote->next)
	    {
	      if (is_note_to (ch, pnote) &&
		  (board_here == pnote->board_num || IS_IMMORTAL (ch))
		  && str_cmp (NAME (ch), pnote->sender)
		  && ch->pcdata->last_note < pnote->date_stamp)
		{
		  sprintf (buf,"\x1B[1;30m---------------------------------------------------------------------------\x1B[37;0m\n\r");
		  sprintf (buf+strlen(buf), "[Note #\x1B[1;37m%d:%d\x1B[37;0m] From: \x1B[1;37m%s\x1B[37;0m  To: \x1B[1;37m%s\x1B[37;0m\n\r",
			   vnum,
			   pnote->board_num,
			   pnote->sender,
			   pnote->to_list);
		  if (strlen(pnote->subject)>35) {
			pnote->subject[35]='\0';
			}
		  sprintf (buf+strlen(buf),"%s.  Subject: \x1B[1;36m%s\x1B[37;0m\n\r",
			   pnote->date,
			   pnote->subject);
		  sprintf (buf+strlen(buf),"\x1B[1;30m---------------------------------------------------------------------------\x1B[37;0m\n\r");
		  strcat (buf1, buf);
		  strcat (buf1, pnote->text);
		  ch->pcdata->last_note = UMAX (ch->pcdata->last_note, pnote->date_stamp);
		
                  page_to_char_limited (buf1, ch); 
		  return;
		}
	      else
		vnum++;
	    }
	  send_to_char ("You have no unread notes.\n\r", ch);
	  return;
	}
      else if (is_number (argy))
	{
	  fAll = FALSE;
	  anum = atoi (argy);
	}
      else
	{
	  send_to_char ("Note read which number?\n\r", ch);
	  return;
	}
      vnum = 0;
      for (pnote = note_list; pnote != NULL; pnote = pnote->next)
	{
	  if ((vnum++ == anum || fAll) && is_note_to (ch, pnote) &&
	      (board_here == pnote->board_num || IS_IMMORTAL (ch)))
	    {
		  sprintf (buf,"\x1B[1;30m---------------------------------------------------------------------------\x1B[37;0m\n\r");
		  sprintf (buf+strlen(buf), "[Note #\x1B[1;37m%d:%d\x1B[37;0m] From: \x1B[1;37m%s\x1B[37;0m  To: \x1B[1;37m%s\x1B[37;0m\n\r",
			   vnum - 1,
			   pnote->board_num,
			   pnote->sender,
			   pnote->to_list);
		  if (strlen(pnote->subject)>35) {
			pnote->subject[35]='\0';
			}
		  sprintf (buf+strlen(buf),"Dated %s.  Subject: \x1B[1;36m%s\x1B[37;0m\n\r",
			   pnote->date,
			   pnote->subject);
		  sprintf (buf+strlen(buf),"\x1B[1;30m---------------------------------------------------------------------------\x1B[37;0m\n\r");
	      strcat (buf1, buf);
	      strcat (buf1, pnote->text);
	  
              page_to_char_limited (buf1, ch); 
	      return;
	    }
	}
      send_to_char ("No such note.\n\r", ch);
      return;
    }
  if (!str_cmp (arg, "write"))
    {
      if (LEVEL (ch) < IMM_LEVEL && board_here == 1)
	{
	  send_to_char ("You can't write on the immortal notice board.\n\r", ch);
	  return;
	}
      if (IS_SET(ch->act, PLR_LAMER))
	{
	  send_to_char("You have lost your board writing privileges.\n\r", ch);
	  send_to_char("Maybe if you grow up you will be allowed to start\n\r", ch);
	  send_to_char("posting on the board again, however since you have\n\r", ch);
	  send_to_char("consistently shown yourself to be lame this is not likely.\n\r", ch);
	  return;
	}
      check_ced (ch);
      note_attach (ch);
      string_append (ch, &ch->ced->pnote->text);
      return;
    }
  if (!str_cmp (arg, "fwrite"))
    {
      if (IS_SET(ch->act, PLR_LAMER))
	{
	  send_to_char("You have lost your board writing privileges.\n\r", ch);
	  send_to_char("Maybe if you grow up you will be allowed to start\n\r", ch);
	  send_to_char("posting on the board again, however since you have\n\r", ch);
	  send_to_char("consistently shown yourself to be lame this is not likely.\n\r", ch);
	  return;
	}
      if (LEVEL (ch) < IMM_LEVEL && board_here == 1)
	{
	  send_to_char ("You can't write on the immortal notice board.\n\r", ch);
	  return;
	}
      check_ced (ch);
      note_attach (ch);
      fullscreen_editor (ch, &ch->ced->pnote->text);
      return;
    }

  if (!str_cmp (arg, "+"))
    {
      
      check_ced (ch);
      note_attach (ch);
      strcpy (buf, ch->ced->pnote->text);
      if (strlen (buf) + strlen (argy) >= STD_LENGTH - 200)
	{
	  send_to_char ("Note too long.\n\r", ch);
	  return;
	}
      strcat (buf, argy);
      strcat (buf, "\n\r");
      free_string (ch->ced->pnote->text);
      ch->ced->pnote->text = str_dup (buf);
      ch->ced->pnote->lines += 1;
      send_to_char ("Ok.\n\r", ch);
      return;
    }  
  if (!str_cmp (arg, "-"))
    {
      int num, cnt;
      char new_buf[STD_LENGTH];
      const char *str;
      check_ced (ch);
      if (ch->ced->pnote == NULL)
	{
	  send_to_char ("You have to start a note first.\n\r", ch);
	  return;
	}
      if (ch->ced->pnote->lines == 0)
	{
	  send_to_char ("Nothing to delete.\n\r", ch);
	  return;
	}
      new_buf[0] = '\0';
      str = ch->ced->pnote->text;
      cnt = 1;
      for (num = 1; num <= ch->ced->pnote->lines; num++)
	{
	  while (*str != '\r' && *str != '\0')
	    {
	      ++str;
	      ++cnt;
	    }
	}
      strncpy (new_buf, ch->ced->pnote->text, cnt);
      new_buf[cnt] = '\0';
      free_string (ch->ced->pnote->text);
      ch->ced->pnote->text = str_dup (new_buf);
      ch->ced->pnote->lines -= 1;
      send_to_char ("Ok.\n\r", ch);
      return;
    }
  if (!str_cmp (arg, "subject"))
    {
      if (IS_SET(ch->act, PLR_LAMER))
	{
	  send_to_char("You have lost your board writing privileges.\n\r", ch);
	  send_to_char("Maybe if you grow up you will be allowed to start\n\r", ch);
	  send_to_char("posting on the board again, however since you have\n\r", ch);
	  send_to_char("consistently shown yourself to be lame this is not likely.\n\r", ch);
	  return;
	}
      check_ced (ch);
      note_attach (ch);
      free_string (ch->ced->pnote->subject);
      ch->ced->pnote->subject = str_dup (argy);
      send_to_char ("Ok.\n\r", ch);
      return;
    }
  if (!str_cmp (arg, "to"))
    {
      if (IS_SET(ch->act, PLR_LAMER))
	{
	  send_to_char("You have lost your board writing privileges.\n\r", ch);
	  send_to_char("Maybe if you grow up you will be allowed to start\n\r", ch);
	  send_to_char("posting on the board again, however since you have\n\r", ch);
	  send_to_char("consistently shown yourself to be lame this is not likely.\n\r", ch);
	  return;
	}
      check_ced (ch);
      note_attach (ch);
      free_string (ch->ced->pnote->to_list);
      ch->ced->pnote->to_list = str_dup (argy);
      send_to_char("\x1b[1;31mBe sure to put 'pkill' or 'flame' if this is a pkill flame note.\x1b[0;37m", ch);
      send_to_char ("Ok.\n\r", ch);
      return;
    }
  if (!str_cmp (arg, "clear"))
    {
      if (IS_SET(ch->act, PLR_LAMER))
	{
	  send_to_char("You have lost your board writing privileges.\n\r", ch);
	  send_to_char("Maybe if you grow up you will be allowed to start\n\r", ch);
	  send_to_char("posting on the board again, however since you have\n\r", ch);
	  send_to_char("consistently shown yourself to be lame this is not likely.\n\r", ch);
	  return;
	}
      check_ced (ch);
      if (ch->ced->pnote != NULL)
	{
	  free_string (ch->ced->pnote->text);
	  free_string (ch->ced->pnote->subject);
	  free_string (ch->ced->pnote->to_list);
	  free_string (ch->ced->pnote->date);
	  free_string (ch->ced->pnote->sender);
	  ch->ced->pnote->next = note_free;
	  note_free = ch->ced->pnote;
	  ch->ced->pnote = NULL;
	}
      send_to_char ("Ok.\n\r", ch);
      return;
    }
  if (!str_cmp (arg, "show"))
    {
      if (IS_SET(ch->act, PLR_LAMER))
	{
	  send_to_char("You have lost your board writing privileges.\n\r", ch);
	  send_to_char("Maybe if you grow up you will be allowed to start\n\r", ch);
	  send_to_char("posting on the board again, however since you have\n\r", ch);
	  send_to_char("consistently shown yourself to be lame this is not likely.\n\r", ch);
	  return;
	}
      check_ced (ch);
      if (ch->ced->pnote == NULL)
	{
	  send_to_char ("You have no note in progress.\n\r", ch);
	  return;
	}
      sprintf (buf, "%s: %s\n\rTo: %s\n\rLines: %d\n\r",
	       ch->ced->pnote->sender,
	       ch->ced->pnote->subject,
	       ch->ced->pnote->to_list,
	       ch->ced->pnote->lines
	);
      strcat (buf1, buf);
      strcat (buf1, ch->ced->pnote->text);

      page_to_char_limited (buf1, ch); 
      return;
    }
  if (!str_cmp (arg, "post") || !str_prefix (arg, "send"))
    {
      FILE *fp;
      char *strtime;
      
      check_ced (ch);
      
      if (ch->ced->pnote == NULL)
	{
	  send_to_char ("You have no note in progress.\n\r", ch);
	  return;
	}
      if (board_here == 1 && LEVEL (ch) < IMM_LEVEL)
	{
	  send_to_char ("This board is reserved for immortals posting messages to mortals.\n\r", ch);
	  send_to_char ("Please post on a public board if you wish to write a note.\n\r", ch);
	  return;
	}
      if (!str_cmp (ch->ced->pnote->to_list, ""))
	{
	  send_to_char (
	    "You need to provide a recipient (name, all, or immortal).\n\r",
			 ch);
	  return;
	}
      if (!str_cmp (ch->ced->pnote->subject, ""))
	{
	  send_to_char ("You need to provide a subject.\n\r", ch);
	  return;
	}
      if (IS_SET(ch->act, PLR_LAMER))
	{
	  send_to_char("You have lost your board writing privileges.\n\r", ch);
	  send_to_char("Maybe if you grow up you will be allowed to start\n\r", ch);
	  send_to_char("posting on the board again, however since you have\n\r", ch);
	  send_to_char("consistently shown yourself to be lame this is not likely.\n\r", ch);
	  return;
	}
      ch->pcdata->online_spot->notes_this_reboot++;
      if(ch->pcdata->online_spot->notes_this_reboot > 8)
	{
	  send_to_char("You have already written too many notes this reboot.\n\r", ch);
	  return;
	}

      ch->ced->pnote->next = NULL;
      strtime = ctime (&current_time);
      strtime[strlen (strtime) - 1] = '\0';
      ch->ced->pnote->board_num = board_here;
      ch->ced->pnote->date = str_dup (strtime);
      ch->ced->pnote->date_stamp = current_time;
      if (note_list == NULL)
	{
	  note_list = ch->ced->pnote;
	}
      else
	{
	  for (pnote = note_list; pnote->next != NULL; pnote = pnote->next)
	    ;
	  pnote->next = ch->ced->pnote;
	}
      pnote = ch->ced->pnote;
      ch->ced->pnote = NULL;
#ifndef WINDOWS
//      fclose (fpReserve);
#endif
      if ((fp = fopen (NOTE_FILE, "a")) == NULL)
	{
	  perror (NOTE_FILE);
	}
      else
	{
	  fprintf (fp, "Sender %s~\n", fix_string (pnote->sender));
	  fprintf (fp, "TBoard %d\n", pnote->board_num);
	  fprintf (fp, "Date %s~\n", pnote->date);
	  fprintf (fp, "Stamp %ld\n", pnote->date_stamp);
	  fprintf (fp, "To %s~\n", fix_string (pnote->to_list));
	  fprintf (fp, "Subject %s~\n", fix_string (pnote->subject));
	  fprintf (fp, "Text\n%s~\n", pnote->text);
	  fprintf (fp, "End\n\n");
	  fclose (fp);
	}
#ifndef WINDOWS
//      fpReserve = fopen (NULL_FILE, "r");
#endif
      send_to_char ("Ok.\n\r", ch);
      return;
    }
  if (!str_cmp (arg, "remove"))
    {
      if (!is_number (argy))
	{
	  send_to_char ("Note remove which number?\n\r", ch);
	  return;
	}
      anum = atoi (argy);
      vnum = 0;
      for (pnote = note_list; pnote != NULL; pnote = pnote->next)
	{
	  if (vnum++ == anum && (
				  (is_note_to (ch, pnote) && !is_name ("all", pnote->to_list)) || LEVEL (ch) > 109))
	    {
	      note_remove (ch, pnote);
	      send_to_char ("Ok.\n\r", ch);
	      return;
	    }
	}
      send_to_char ("No such note.\n\r", ch);
      return;
    }
  send_to_char ("Huh? Type 'help note' for usage.\n\r", ch);
  return;
}
