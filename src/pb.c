#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
bool 
is_cheating (CHAR_DATA * ch)	/*TRUE IF CHEAT, FALSE IF NO */
{
  return FALSE;
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
    for (jj = 0; jj < NUM_ALIGN; jj++)
      {
	max_remort_times[jj] = 0;
      }
  }
  while (1)
    {
      strcpy (buff, fread_word (fp));
      if (!str_cmp (buff, "END"))
	break;
      play = new_player ();
      strcpy (play->player_name, buff);
      strcpy (buff, fread_word (fp));
      strcpy (play->email_address, buff);
      play->player_last_login = fread_number (fp);
      play->player_align = fread_number (fp);
      play->player_level = fread_number (fp);
      play->player_nodelete = fread_number (fp);
      play->player_remort_times = fread_number (fp);
      play->player_created = fread_number(fp);
      max_remort_times[play->player_align] = UMAX(max_remort_times[play->player_align], play->player_remort_times);
     
    }

  fclose (fp);
  
  return;
}


void 
reload_pbase (CHAR_DATA * ch, char *argy)
{
  PLAYERBASE_DATA *play;
  PLAYERBASE_DATA *pn;
  DEFINE_COMMAND ("z_reload_pbase", reload_pbase, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "This command reloads the pbase datafile.")
    for (play = playerbase_zero; play != NULL; play = pn)
    {
      pn = play->next;
      free (play);
    }
  load_playerbase ();
  return;
}


void 
p_refresh (CHAR_DATA * ch, char *argy)
{
  char buf[200];
  DEFINE_COMMAND ("z_refresh", p_refresh, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Resets all players' inactivity counters.")

    sprintf (buf, "util -refresh");
  system (buf);
  reload_pbase (ch, argy);
  return;
}



void 
clean_up (CHAR_DATA * ch, char *argy)
{
  char buf[200];
  DEFINE_COMMAND ("z_clean_pfile", clean_up, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Deletes pfiles of all people not in the pfile database!!  Dangerous.")

    sprintf (buf, "util -cleanup &");
  system (buf);
  return;
}



void 
clean_oldchars (CHAR_DATA * ch, char *argy)
{
  char buf[200];
  DEFINE_COMMAND ("z_clean_oldchars", clean_oldchars, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Cleans up people who haven't logged in for quite a while.  Lags the mud.")

    sprintf (buf, "util -deleteold");
  system (buf);
  reload_pbase (ch, argy);
  return;
}

void 
update_pbase (CHAR_DATA * ch)
{
  bool ffund;
  PLAYERBASE_DATA *player;
  ffund = FALSE;
  for (player = playerbase_zero; player != NULL; player = player->next)
    {
      if (player->player_level != -1 && !str_cmp (player->player_name, NAME (ch)))
	{
	  if (ch->pcdata->level != (player->player_level))
	    {
	      player->player_last_login = (int) ch->pcdata->logon;
	      if (player->email_address[0] == '\0')
		strcpy (player->email_address, ch->pcdata->email);
	      player->player_align = ALIGN(ch);
	      player->player_level = ch->pcdata->level;
	      player->player_remort_times = ch->pcdata->remort_times;
	      player->player_created = ch->pcdata->created;
	      save_playerbase ();
	      return;
	    }
	  ffund = TRUE;
	  break;
	}
    }
  if (ffund == FALSE)
    {
      bool isfree;
      isfree = FALSE;
      player = new_player ();
      strcpy (player->player_name, NAME (ch));
      player->player_last_login = (int) ch->pcdata->logon;
      player->player_align = ALIGN(ch);
      strcpy (player->email_address, ch->pcdata->email);
      player->player_level = ch->pcdata->level;
      player->player_nodelete = FALSE;
      player->player_remort_times = ch->pcdata->remort_times;
      player->player_created = ch->pcdata->created;
      isfree = TRUE;
      save_playerbase ();
    }
  return;
}

void 
fcheck_pbase (char *n)
{
  bool ffund;
  PLAYERBASE_DATA *player;
  ffund = FALSE;
  for (player = playerbase_zero; player != NULL; player = player->next)
    {
      if (!str_cmp (player->player_name, n))
	return;
    }
  player = new_player ();
  strcpy (player->player_name, n);
  player->player_last_login = (int) current_time;
  player->player_align = 0;
  strcpy (player->email_address, "not_recorded_yet");
  player->player_level = 5;
  player->player_nodelete = FALSE;
  player->player_remort_times = 0;
  player->player_created = current_time;
  save_playerbase ();
  return;
}
