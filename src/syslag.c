/*"syslag" with help from Omicron */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

SHELL *shell_first = NULL;

#ifdef WINDOWS

SHELL *new_shell (void) {return NULL;}
void finger (CHAR_DATA * ch, char *argy) {return;}
void syslag (CHAR_DATA * ch, char *argy) {return;}
void check_background_processes (void) {return;}

#else

SHELL *
new_shell (void)
{
  SHELL *n;
  n = mem_alloc (sizeof (*n));
  n->next = shell_first;
  shell_first = n;
  n->wait = 1;
  n->caller[0] = '\0';
  n->fname[0] = '\0';
  return n;
}

void 
logsearch (CHAR_DATA * ch, char *argy)
{
  SHELL *sh;
  char tmpln[500];
  char fn[50];
  int i;
  DEFINE_COMMAND ("logsearch", logsearch, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Allows you to spawn a log grepping process...")
    sh = new_shell ();
  fn[0] = number_range ('a', 'z');
  fn[1] = number_range ('a', 'z');
  fn[2] = number_range ('a', 'z');
  fn[3] = number_range ('a', 'z');
  fn[4] = number_range ('a', 'z');
  fn[5] = '.';
  fn[6] = 'F';
  fn[7] = '\0';
  for (i = 0; i < strlen(argy); i++)
  {
    if (argy[i] == ';')
    return;
  }
  strcpy (sh->fname, fn);
  strcpy (sh->caller, NAME (ch));
  sprintf (tmpln, "grep %s ../log/* > %s &", argy, fn);
  system (tmpln);
  send_to_char ("Log search started.\n\r", ch);
  return;
}

void 
finger (CHAR_DATA * ch, char *argy)
{
  SHELL *sh;
  char tmpln[500];
  char fn[50];
  DEFINE_COMMAND ("sysfinger", finger, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Allows you to spawn a finger process...")
    sh = new_shell ();
  fn[0] = number_range ('a', 'z');
  fn[1] = number_range ('a', 'z');
  fn[2] = number_range ('a', 'z');
  fn[3] = number_range ('a', 'z');
  fn[4] = number_range ('a', 'z');
  fn[5] = '.';
  fn[6] = 'F';
  fn[7] = '\0';
  strcpy (sh->fname, fn);
  strcpy (sh->caller, NAME (ch));
  sprintf (tmpln, "finger %s > %s &", argy, fn);
  system (tmpln);
  send_to_char ("Sysfinger subprocess started.\n\r", ch);
  return;
}

void 
syslag (CHAR_DATA * ch, char *argy)
{
  SHELL *sh;
  char tmpln[500];
  char fn[50];
  DEFINE_COMMAND ("syslag", syslag, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Shows system memory reqs, etc...")

    sh = new_shell ();
  fn[0] = number_range ('a', 'z');
  fn[1] = number_range ('a', 'z');
  fn[2] = number_range ('a', 'z');
  fn[3] = number_range ('a', 'z');
  fn[4] = number_range ('a', 'z');
  fn[5] = '.';
  fn[6] = 'X';
  fn[7] = '\0';
  strcpy (sh->fname, fn);
  strcpy (sh->caller, NAME (ch));

#ifdef SUN
  sprintf (tmpln, "/usr/ucb/ps -aux | grep Emlen > %s &", fn);
#else
  sprintf (tmpln, "ps -ux | grep LoC > %s &", fn);
#endif
  system (tmpln);
  send_to_char ("Syslag subprocess started.\n\r", ch);
  return;
}

void 
check_background_processes (void)
{
  CHAR_DATA *ch;
  SHELL *sh;
  SHELL *sh_n;
  SHELL *s;
  FILE *fp;
  char buf[500];
  char tmpln[8024];

  for (sh = shell_first; sh != NULL; sh = sh_n)
    {
      sh_n = sh->next;
      if ((fp = fopen (sh->fname, "a")) == NULL)
	continue;
      if (sh->wait > 0)
	{
	  sh->wait--;
	  fclose (fp);
	  continue;
	}
      fprintf (fp, "\n\rEND\n\r");
      fclose (fp);
      if ((fp = fopen (sh->fname, "r")) == NULL)
	continue;
      if ((ch = get_char_world_2 (char_list, sh->caller)) == NULL)
	{
	  goto xx;
	  continue;
	}
      if (sh->fname[6] == 'X')
	{
	  send_to_char ("\nUSER      PID   %CPU %MEM SIZE RSS TTY STAT START TIME COMMAND\n\r", ch);
	}
      for (;;)
	{
	  strcpy (tmpln, fread_string_eol (fp));
	  if (!str_prefix ("END", tmpln) || !str_cmp (tmpln, "END"))
	    goto dun;
	  send_to_char (tmpln, ch);
	  send_to_char ("\n\r", ch);
	}
    dun:
      fclose (fp);
    xx:
      sprintf (buf, "%s", sh->fname);
      unlink (buf);
      if (sh == shell_first)
	shell_first = sh->next;
      else
	for (s = shell_first; s != NULL; s = s->next)
	  {
	    if (s->next == sh)
	      {
		s->next = sh->next;
		goto dun2;
	      }
	  }
    dun2:
      free_m (sh);
    }
  return;
}
#endif
