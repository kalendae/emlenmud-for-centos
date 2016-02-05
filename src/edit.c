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
void 
position_cursor (CHAR_DATA * ch, int loc, char *strin)
{
  int col;
  int row;
  int ctr;
  char buf[500];
  row = 1;
  col = 1;
  for (ctr = 0; strin[ctr] != '\0'; ctr++)
    {
      if (ctr == loc)
	break;
      if (strin[ctr] == '\n')
	{
	  ctr++;
	  col++;
	  row = 1;
	  continue;
	}
      row++;
      if (row > 80)
	{
	  col++;
	  row = 1;
	}
    }
  sprintf (buf, "\x1B[24;1f%d,%d [%d] ", row, col, ch->desc->location);
  write_to_descriptor2 (ch->desc, buf, 0);
  sprintf (buf, "\x1B[%d;%df", col, row);
  write_to_descriptor2 (ch->desc, buf, 0);
  return;
}

void 
character_add (CHAR_DATA * ch, char *to_add)
{
  char buf[1524];
  int i;
  if (!ch->desc->pString)
    {
      ch->desc->connected = CON_PLAYING;
      return;
    }
  for (i = 0; i < 1524; i++)
    buf[i] = '\0';
  strcpy (buf, *ch->desc->pString);
  if (!ch->desc)
    return;
  if (!ch->pcdata->name)
    return;
  if (*to_add == 1)		/*All this to move up a line.. heh */
    {
      int ll;
      int tpos;
      int pos_from_beginning_of_line;
      int loc_of_prev_line;
      int ct1;
      int ct2;
      tpos = ch->desc->location;
      ct1 = tpos;
      for (pos_from_beginning_of_line = 0;; pos_from_beginning_of_line++)
	{
	  if (ct1 < 1)
	    {
	      return;
	    }
	  if (buf[ct1] == '\r')
	    {
	      break;
	    }
	  ct1--;
	}
      for (ct2 = ct1 - 3;; ct2--)
	{
	  if (ct2 < 1)
	    {
	      loc_of_prev_line = 0;
	      break;
	    }
	  if (buf[ct2] == '\n')
	    {
	      loc_of_prev_line = ct2 + 1;
	      break;
	    }
	}
      for (ll = loc_of_prev_line;; ll++)
	{
	  if (pos_from_beginning_of_line == 0)
	    break;
	  pos_from_beginning_of_line--;
	  if (buf[ll] == '\n')
	    {
	      break;
	    }
	}
      if (loc_of_prev_line == 0 && ll - 1 >= 0)
	ll--;
      ch->desc->location = ll;
      position_cursor (ch, ch->desc->location, *ch->desc->pString);
      return;
    }
  if (*to_add == 2)		/*Heh, all this just to move down a line */
    {
      int max;
      int tpos;
      int pos_from_prev_line;
      int pos_of_next_line;
      int chars_in_next_line;
      int spl;
      int snl;
      int ppp;
      max = strlen (buf);
      tpos = ch->desc->location;
      /*Find start previous line */
      pos_from_prev_line = 0;
      for (spl = ch->desc->location;; spl--)
	{
	  if (spl <= 0)
	    {
	      spl = 0;
	      pos_from_prev_line++;
	      break;
	    }
	  if (buf[spl] == '\r')
	    {
	      spl++;
	      break;
	    }
	  pos_from_prev_line++;
	}
      /*Find next line, if any */
      for (snl = ch->desc->location;; snl++)
	{
	  if (snl > max)
	    return;
	  if (buf[snl] == '\n')
	    {
	      snl -= 3;
	      if (snl > max)
		return;
	      break;
	    }
	}
      pos_of_next_line = snl;
      ppp = snl;
      for (chars_in_next_line = 0;; chars_in_next_line++)
	{
	  if (ppp > max)
	    {
	      ppp = max;
	      break;
	    }
	  if (buf[ppp] == '\n')
	    {
	      break;
	    }
	  if (chars_in_next_line == pos_from_prev_line)
	    {
	      break;
	    }
	  ppp++;
	}
      if (ppp + 4 <= max)
	ppp += 4;
      ch->desc->location = ppp;
      position_cursor (ch, ch->desc->location, *ch->desc->pString);
      return;
    }
  if (*to_add == 3)
    {
      int max;
      max = strlen (buf);
      if (ch->desc->location + 1 <= max)
	ch->desc->location++;
      if (ch->desc->location > 1 && buf[ch->desc->location - 1] == '\n')
	{
	  if (ch->desc->location + 1 <= max)
	    ch->desc->location += 1;
	}
      position_cursor (ch, ch->desc->location, *ch->desc->pString);
      return;
    }
  if (*to_add == 4)
    {
      if (ch->desc->location - 1 >= 0)
	ch->desc->location--;
      if (buf[ch->desc->location] == '\r')
	{
	  if (ch->desc->location - 1 >= 0)
	    ch->desc->location--;
	}
      position_cursor (ch, ch->desc->location, *ch->desc->pString);
      return;
    }
  if (*to_add == 5)
    {
      free_string (*ch->desc->pString);
      *ch->desc->pString = str_dup ("");
      ch->desc->location = 0;
      write_to_descriptor2 (ch->desc, "\x1B[2J", 0);
      write_to_descriptor2 (ch->desc, "\x1B[24;15fCTRL-E = CLEAR @ = Save/Quit CTRL-R = Redraw CTRL-F = Format", 0);
      write_to_descriptor2 (ch->desc, "\x1B[1;1f", 0);
      return;
    }
  if (*to_add == 6)		/*Format text */
    {
      *ch->desc->pString = format_string (*ch->desc->pString);
      ch->desc->location = 0;
      write_to_descriptor2 (ch->desc, "\x1B[2J", 0);
      write_to_descriptor2 (ch->desc, "\x1B[1;1f", 0);
      for (i = 0; i < 1524; i++)
	buf[i] = '\0';
      strcpy (buf, *ch->desc->pString);
      write_to_descriptor2 (ch->desc, buf, 0);
      write_to_descriptor2 (ch->desc, "\x1B[24;15fCTRL-E = CLEAR @ = Save/Quit CTRL-R = Redraw CTRL-F = Format", 0);
      position_cursor (ch, ch->desc->location, buf);
      return;
    }
  if (*to_add == 18)		/*redraw request */
    {
      write_to_descriptor2 (ch->desc, "\x1B[2J\x1B[1;1f", 0);
      write_to_descriptor2 (ch->desc, buf, 0);
      write_to_descriptor2 (ch->desc, "\x1B[24;15fCTRL-E = CLEAR @ = Save/Quit CTRL-R = Redraw CTRL-F = Format", 0);
      position_cursor (ch, ch->desc->location, buf);
      return;
    }
  if (*to_add == 9)		/*Backspace encountered */
    {
      int ct;
      int ii;
      int ctr;
      char teol[1000];
      int oldlen;
      bool is_cr;
      is_cr = FALSE;
      if (ch->desc->location - 1 < 0)
	return;
      if (ch->desc->location - 1 >= 0 && buf[ch->desc->location - 1] == '\r')
	is_cr = TRUE;
      oldlen = strlen (buf);
      for (ii = ch->desc->location - 1; ii < oldlen; ii++)
	{
	  buf[ii] = buf[ii + 1];
	}
      buf[oldlen] = '\0';
      ch->desc->location--;
      if (is_cr)
	{
	  oldlen = strlen (buf);
	  for (ii = ch->desc->location - 1; ii < oldlen; ii++)
	    {
	      buf[ii] = buf[ii + 1];
	    }
	  buf[oldlen] = '\0';
	  ch->desc->location--;
	}
      free_string (*ch->desc->pString);
      *ch->desc->pString = str_dup (buf);
      if (is_cr)
	{
	  write_to_descriptor2 (ch->desc, "\x1B[2J", 0);
	  write_to_descriptor2 (ch->desc, "\x1B[1;1f", 0);
	  write_to_descriptor2 (ch->desc, buf, 0);
	  write_to_descriptor2 (ch->desc, "\x1B[24;15fCTRL-E = CLEAR @ = Save/Quit CTRL-R = Redraw CTRL-F = Format", 0);
	  position_cursor (ch, ch->desc->location, buf);
	  return;
	}
      for (i = 0; i < 1524; i++)
	buf[i] = '\0';
      strcpy (buf, *ch->desc->pString);
      ct = 0;
      for (ctr = ch->desc->location;; ctr++)
	if (buf[ctr] != '\0' && buf[ctr] != '\n')
	  {
	    teol[ct] = buf[ctr];
	    ct++;
	  }
	else
	  break;
      teol[ct] = '\0';
      position_cursor (ch, ch->desc->location, *ch->desc->pString);
      write_to_descriptor2 (ch->desc, "\x1B[K", 0);
      write_to_descriptor2 (ch->desc, teol, 0);
      position_cursor (ch, ch->desc->location, *ch->desc->pString);
      return;
    }
  if (*to_add == '~' || *to_add == '@')
    {
      ch->desc->pString = NULL;
      ch->desc->location = 0;
      ch->desc->connected = CON_PLAYING;
      SET_BIT (ch->pcdata->act2, PLR_TELNET_GA);
      write_to_descriptor2 (ch->desc, linemode_on, 0);
      write_to_descriptor2 (ch->desc, echo_on_str, 0);
      write_to_descriptor2 (ch->desc, "\x1B[24;1f\x1B[KText Saved. Press Enter. ", 0);
      return;
    }
  if (strlen (buf) > 1500)
    {
      return;
    }
  if (!IS_SET(ch->desc->act, DESC_INSERT))
    {
      /*BAH, always insert mode... */
      int i;
      int tt;
      int pteol;
      int slb;
      char ts[1500];
      int ctr;
      for (i = 0; i < strlen (to_add); i++)
	{
	  if (buf[ch->desc->location] != '\0')
	    {
	      slb = strlen (buf);
	      position_cursor (ch, ch->desc->location, buf);
	      for (tt = slb; tt >= ch->desc->location; tt--)
		buf[tt + 1] = buf[tt];
	      buf[ch->desc->location] = to_add[i];
	      ctr = 0;
	      for (pteol = ch->desc->location; pteol < slb; pteol++)
		{
		  if (buf[pteol] == '\n')
		    {
		      ctr--;
		      break;
		    }
		  ts[ctr] = buf[pteol];
		  ctr++;
		}
	      ts[ctr + 1] = '\0';
	      if (to_add[i] == '\r')
		{
		  write_to_descriptor2 (ch->desc, "\x1B[2J", 0);
		  write_to_descriptor2 (ch->desc, "\x1B[1;1f", 0);
		  write_to_descriptor2 (ch->desc, buf, 0);
		  write_to_descriptor2 (ch->desc, "\x1B[24;15fCTRL-E = CLEAR @ = Save/Quit CTRL-R = Redraw CTRL-F = Format", 0);
		}
	      else
		{
		  write_to_descriptor2 (ch->desc, "\x1B[K", 0);
		  write_to_descriptor2 (ch->desc, ts, 0);
		}
	      position_cursor (ch, ch->desc->location + 1, buf);
	    }
	  else
	    buf[ch->desc->location] = to_add[i];
	  ch->desc->location++;
	}
    }
  position_cursor (ch, ch->desc->location, buf);
  free_string (*ch->desc->pString);
  *ch->desc->pString = str_dup (buf);
  return;
}

char doiecho[] =
{255, 253, 1, '\0'};
char dontiecho[] =
{255, 254, 1, '\0'};
char iwillecho[] =
{255, 251, 1, '\0'};
char iwontecho[] =
{255, 252, 1, '\0'};
char dontlinemode[] =
{255, 254, 34, '\0'};
char dolinemode[] =
{255, 253, 34, '\0'};


void 
fullscreen_editor (CHAR_DATA * ch, char **pString)
{
  write_to_descriptor2 (ch->desc, dontecho, 0);
  write_to_descriptor2 (ch->desc, echo_off_str, 0);
  write_to_descriptor2 (ch->desc, linemode_off, 0);
  write_to_descriptor2 (ch->desc, echo_off_str, 0);
  write_to_descriptor2 (ch->desc, "\x1B[2J", 0);
/* @ saves the text */
  REMOVE_BIT (ch->pcdata->act2, PLR_TELNET_GA);
  if (*pString == NULL)
    {
      *pString = str_dup ("");
    }
  if (strlen (*pString) > 1450)
    {
      send_to_char ("You cannot edit this string with the fullscreen editor. Sorry.\n\r", ch);
      return;
    }
  write_to_descriptor2 (ch->desc, "\x1B[1;1f", 0);
  write_to_descriptor2 (ch->desc, *pString, 0);
  write_to_descriptor2 (ch->desc, "\x1B[24;15fCTRL-E = CLEAR @ = Save/Quit CTRL-R = Redraw CTRL-F = Format", 0);
  write_to_descriptor2 (ch->desc, "\x1B[1;1f", 0);
  ch->desc->location = 0;
  ch->desc->pString = pString;
  ch->desc->connected = CON_FSEDITOR;
  return;
}
