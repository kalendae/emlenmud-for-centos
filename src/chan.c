#include <ctype.h>
#include <stdio.h>
#ifdef SYSV
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

#ifndef WINDOWS
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#endif

#include "mush.h"


DESCRIPTOR_DATA *connectiond[5];
int outgoing[5];
char connection_names[5][5];








void 
read_channel_data (void)
{
  FILE *fp;
  CHANNEL *c;
  CHANNEL *c_next;
  char buf[500];

  for (c = chan_first; c != NULL; c = c_next)
    {
      c_next = c->next;
      free_m (c);
    }
  chan_first = NULL;

  if ((fp = fopen ("channel.dat", "r")) == NULL)
    {
      fprintf (stderr, "Could not read channel.dat!\n");
      return;
    }

  c = NULL;
  for (;;)
    {
      strcpy (buf, fread_word (fp));
      if (!str_cmp ("END", buf))
	{
	  break;
	}
      if (!str_cmp ("#", buf))
	{
	  fread_to_eol (fp);
	  continue;
	}
      if (!str_cmp ("NEW_CHANNEL", buf))
	{
	  c = mem_alloc (sizeof (*c));
	  c->clan = FALSE;
	  c->clan2 = FALSE;
	  c->channel_name = &str_empty[0];
	  c->moves = 0;
	  c->commands[0] = NULL;
	  c->commands[1] = NULL;
	  c->commands[2] = NULL;
	  c->level = 0;
	  c->to_level = 0;
	  c->color = &str_empty[0];
	  c->to_align = TO_SAME;
	  c->next = chan_first;
	  chan_first = c;
	  continue;
	}
      if (!str_cmp ("NAME", buf))
	{
	  c->channel_name = str_dup (fread_word (fp));
	  continue;
	}
      if (!str_cmp ("LEVEL", buf))
	{
	  c->level = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("MOVES", buf))
	{
	  c->moves = fread_number (fp);
	  continue;
	}
      if (!str_cmp ("CLAN", buf))
	{
	  char t[20];
	  strcpy (t, fread_word (fp));
	  if (!str_cmp (t, "yes"))
	    c->clan = TRUE;
	  continue;
	}
      if (!str_cmp ("CLAN2", buf))
	{
	  char t[20];
	  strcpy (t, fread_word (fp));
	  if (!str_cmp (t, "yes"))
	    c->clan2 = TRUE;
	  continue;
	}
      if (!str_cmp ("COMMAND", buf))
	{
	  char cmd[40];
	  int i = 0;
	  strcpy (cmd, fread_word (fp));
	  while (c->commands[i])
	    i++;
	  c->commands[i] = str_dup (cmd);
	  continue;
	}
      if (!str_cmp ("COLOR", buf))
	{
	  c->color = str_dup (fread_word (fp));
	  continue;
	}
      if (!str_cmp ("ALIGNMENT", buf))
	{
	  char t[20];
	  strcpy (t, fread_word (fp));
	  
	  if (!str_cmp ("both", t))
	    c->to_align = TO_ALL;
	  else if (!str_cmp ("same", t))
	    c->to_align = TO_SAME;
	  else c->to_align = TO_SAME;
	  continue;
	}
      if (!str_cmp ("TO_LEVEL", buf))
	{
	  c->to_level = fread_number (fp);
	  continue;
	}
      fprintf (stderr, "Unknown read in channel.dat: %s\n", buf);
      fread_to_eol (fp);
    }

  fclose (fp);
  return;
}


void 
show_social (SOCIAL * s, CHAR_DATA * ch)
{
  char buf[500];
  int pp;
  pp = ch->position;
  ch->position = POSITION_STANDING;
  sprintf (buf, "Social \x1B[1;37mname\x1B[37;0m: %s\n\r", s->name);
  send_to_char (buf, ch);
  sprintf (buf, "\x1B[1;37mTo_char\x1B[37;0m w/no arg: %s\n\r", (s->char_no_arg ? s->char_no_arg : "none"));
  send_to_char (buf, ch);
  sprintf (buf, "\x1B[1;37mTo_others\x1B[37;0m w/no arg: %s\n\r", (s->others_no_arg ? s->others_no_arg : "none"));
  send_to_char (buf, ch);
  sprintf (buf, "To char when \x1B[1;37mChar_found\x1B[37;0m: %s\n\r", (s->char_found ? s->char_found : "none"));
  send_to_char (buf, ch);
  sprintf (buf, "To \x1B[1;37mothers\x1B[37;0m when char found: %s\n\r", (s->others_found ? s->others_found : "none"));
  send_to_char (buf, ch);
  sprintf (buf, "\x1B[1;37mTo_vict\x1B[37;0m when char found: %s\n\r", (s->vict_found ? s->vict_found : "none"));
  send_to_char (buf, ch);
  sprintf (buf, "\x1B[1;37mChar_not\x1B[37;0m found: %s\n\r", (s->char_not_found ? s->char_not_found : "none"));
  send_to_char (buf, ch);
  sprintf (buf, "\x1B[1;37mChar_self\x1B[37;0m specified (to char): %s\n\r", (s->char_auto ? s->char_auto : "none"));
  send_to_char (buf, ch);
  sprintf (buf, "\x1B[1;37mOthers_self\x1B[37;0m specified: %s\n\r", (s->others_auto ? s->others_auto : "none"));
  send_to_char (buf, ch);
  ch->position = POSITION_STANDING;
  return;
}
