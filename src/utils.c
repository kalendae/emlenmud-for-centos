#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emlen.h"
#include "file.c"

PLAYERBASE_DATA *playerbase_zero;
bool found = FALSE;


void 
save_playerbase (void)
{
  FILE *fp;
  PLAYERBASE_DATA *player;
  if ((fp = fopen ("pbase.dat", "w")) == NULL)
    {
      fprintf (stderr, "Error on playerbase write.\n");
      exit (2);
    }
  
  for (player = playerbase_zero; player != NULL; player = player->next)
    {
      if (player->player_level == -1)
	continue;
      fprintf (fp, "%s\n", player->player_name);
      fprintf (fp, "%s\n", player->email_address);
      fprintf (fp, "%ld\n", player->player_last_login);
      fprintf (fp, "%d\n", player->player_align);
      fprintf (fp, "%d\n", player->player_level);
      fprintf (fp, "%d\n", player->player_nodelete);
      fprintf (fp, "%d\n", player->player_remort_times);
      fprintf (fp, "%ld\n", player->player_created);
    }
  fprintf (fp, "\nEND\n");
  fclose (fp);
  return;
}

void 
load_playerbase (void)
{
  FILE *fp;
  PLAYERBASE_DATA *play;
  char buff[500];
  playerbase_zero = NULL;

  if ((fp = fopen ("pbase.dat", "r")) == NULL)
    {
      return;
    }
  {
    int jj;
    for (jj = 0 ; jj < NUM_ALIGN; jj++)
      {
	max_remort_times[jj] = 0;
      }
  }
  while (1)
    {
      strcpy (buff, fread_word (fp));
      if (!str_cmp (buff, "END"))
	break;
      play = (PLAYERBASE_DATA *) mem_alloc (sizeof (*play));
      play->next = playerbase_zero;
      playerbase_zero = play;
      strcpy (play->player_name, buff);
      strcpy (buff, fread_word (fp));
      strcpy (play->email_address, buff);
      play->player_last_login = fread_number (fp);
      play->player_align = fread_number (fp);
      play->player_level = fread_number (fp);
      play->player_nodelete = fread_number (fp);
      play->player_remort_times = fread_number (fp);
      play->player_created = fread_number(fp);
      if (play->player_remort_times > max_remort_times[play->player_align])
	max_remort_times[play->player_align] = play->player_remort_times;
    }

  fclose (fp);

  return;
}

void 
do_deleteold (void)
{
  time_t now;
  long maxlimit;
  char buf[500];
  struct timeval now_time;
  PLAYERBASE_DATA *pb;
  gettimeofday (&now_time, NULL);
  now = (time_t) now_time.tv_sec;
  found = TRUE;
  for (pb = playerbase_zero; pb != NULL; pb = pb->next)
    {
      if (pb->player_level == -1)
	continue;
      if (pb->player_nodelete)
	continue;
      maxlimit = pb->player_level * 140000 * (pb->player_remort_times +1);
      if (maxlimit > 2500000)
	maxlimit = 2500000;
      if (maxlimit < 200000)
	maxlimit = 200000;
      if ((int) (now - pb->player_last_login) > maxlimit)
	{
	  sprintf (buf, "rm \"%s%s\" &", PLAYER_DIR_2, pb->player_name);
	  system (buf);
	  sprintf (buf, "rm \"%s%s.cor\" &", PLAYER_DIR_2, pb->player_name);
	  system (buf);
	  sprintf (buf, "rm \"%s%s.tro\" &", PLAYER_DIR_2, pb->player_name);
	  system (buf);
	  pb->player_level = -1;
	}
    }
  save_playerbase ();
  return;
}

void 
do_cleanup (void)
{
  PLAYERBASE_DATA *pb;
  char buf[500];
  found = TRUE;
  system ("mkdir cleandir");
  for (pb = playerbase_zero; pb != NULL; pb = pb->next)
    {
      if (!strcmp (pb->player_name, "Blank"))
	continue;
      if (pb->player_level == -1)
	continue;
      sprintf (buf, "mv %s%s.cor cleandir", PLAYER_DIR, capitalize (pb->player_name));
      system (buf);
      sprintf (buf, "mv %s%s.tro cleandir", PLAYER_DIR, capitalize (pb->player_name));
      system (buf);
      sprintf (buf, "mv %s%s cleandir", PLAYER_DIR, capitalize (pb->player_name));
      system (buf);
    }

  sprintf (buf, "rm %s*", PLAYER_DIR);
  system (buf);
  sprintf (buf, "cp cleandir/* %s", PLAYER_DIR);
  system (buf);
  system ("rm cleandir/*");
  system ("rmdir cleandir");
  return;
}

void 
do_refresh (void)
{
  time_t now;
  struct timeval now_time;
  PLAYERBASE_DATA *pb;
  found = TRUE;
  gettimeofday (&now_time, NULL);
  now = (time_t) now_time.tv_sec;

  for (pb = playerbase_zero; pb != NULL; pb = pb->next)
    {
      pb->player_last_login = now;
    }
  save_playerbase ();
  return;
}

int 
main (int argc, char **argv)
{
  char cmd[500];
  char selection = ' ';
  bool refresh = FALSE;
  bool cleanup = FALSE;
  bool deleteold = FALSE;
  int i;
  fprintf (stderr, "Utils for Owen Emlen's Aturion Dynasty code, written by Owen Emlen.\n");
  fprintf (stderr, "Loading playerbase into memory.\n");
  load_playerbase ();

  for (i = 1; i < argc; i++)
    {
      if (!strcmp (argv[i], "-refresh"))
	refresh = TRUE;
      if (!strcmp (argv[i], "-cleanup"))
	cleanup = TRUE;
      if (!strcmp (argv[i], "-deleteold"))
	deleteold = TRUE;
    }

  if (refresh)
    do_refresh ();
  if (cleanup)
    do_cleanup ();
  if (deleteold)
    do_deleteold ();

  if (!found)
    {
      fprintf (stderr, "Valid options are: -refresh -cleanup -deleteold\n");
      fprintf (stderr, "-----------------------------------------------\n");
      fprintf (stderr, "-refresh     sets all to have just 'logged in'\n");
      fprintf (stderr, "-cleanup     removes all people not on record in the pbase.\n");
      fprintf (stderr, "-deleteold   removes all people who haven't played in a while.\n");
      return 1;
    }

  return 0;
}
