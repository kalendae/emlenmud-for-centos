#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

void 
do_commands (CHAR_DATA * ch, char *argy)
{
  int h;
  COMMAND *c;
  int sz = 0;
  char tt[500];
  char arg[500];
  int col = 1;
  int minlev = -1;
  int maxlev = MAX_LEVEL;

  DEFINE_COMMAND ("commands", do_commands, POSITION_DEAD, 0, LOG_NORMAL, "Help on commands, or a command list.")
    argy = one_argy (argy, arg);
  if (is_number (arg))
    {
      minlev = atoi (arg);
    }
  if (argy[0] == '\0' && !is_number (arg))
    argy = arg;
  else if (is_number (argy))
    {
      maxlev = atoi (argy);
      argy[0] = '\0';
    }
  hugebuf_o[0] = '\0';

  if (argy[0] == '\0')
    {
      for (h = 0; h < 256; h++)
	{
	  for (c = command_hash[h]; c != NULL; c = c->next)
	    {
	      if (LEVEL (ch) < c->level)
		continue;
	      if (c->level < minlev)
		continue;
	      if (c->level > maxlev)
		continue;
	      if (col == 4)
		{
		  col = 1;
		  sprintf (hugebuf_o + sz, "\n\r");
		  sz += 2;
		}
	      sprintf (tt, "%-20s", c->name);
	      sprintf (hugebuf_o + sz, "%s", tt);
	      sz += strlen (tt);
	      col++;
	    }
	}
      sprintf (hugebuf_o + sz, "\n\r");
      page_to_char (hugebuf_o, ch);
      return;
    }

  for (c = command_hash[UPPER (argy[0])]; c != NULL; c = c->next)
    {
      if (LEVEL (ch) < c->level)
	continue;
      if (!str_prefix (argy, c->name))
	{
	  int pp = ch->position;
	  ch->position = POSITION_STANDING;
	  sprintf (tt, "\x1B[1;32m\"%s\"\x1B[1;34m\n\r", c->name);
	  send_to_char (tt, ch);
	  send_to_char ("------------------------------------------------------------------------------\x1B[1;37m\n\r", ch);
	  sprintf (tt, "%s", c->help);
	  act (tt, ch, NULL, ch, TO_CHAR);
	  send_to_char ("\x1B[1;34m------------------------------------------------------------------------------\x1B[37;0m\n\r", ch);
	  ch->position = pp;
	  return;
	}
    }

  send_to_char ("Command not found.\n\r", ch);
  return;
}

void 
do_socials (CHAR_DATA * ch, char *argy)
{
  char buf[STD_LENGTH];
  SOCIAL *s;
  int col;
  DEFINE_COMMAND ("socials", do_socials, POSITION_DEAD, 0, LOG_NORMAL, "This command shows all socials currently in the database.")

    col = 0;
  for (s = social_list; s != NULL; s = s->next)
    {
/*for ( iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++ ) { */
      sprintf (buf, "%-12s", s->name);
      send_to_char (buf, ch);
      if (++col % 6 == 0)
	send_to_char ("\n\r", ch);
    }

  if (col % 6 != 0)
    send_to_char ("\n\r", ch);
  return;
}
