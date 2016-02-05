/*Module Crsh.c -- Crash protection */
/*The idea behind it is to assign noticeable, dummy info to REALCHAR_DATA for NPC's
   and noticable NREALCHAR_DATA to PC's */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "emlen.h"
char *
jumble_name (char *st, bool unjumble)
{
  static char jr[500];
  char *tmp;
  char tt;
  tt = st[0];
  jr[0] = tt;
  jr[1] = '\0';
  if (!unjumble)
    {
      for (tmp = st + 1; *tmp != '\0'; tmp++)
	{
	  *tmp = *tmp + (tt / 8);
	}
    }
  if (unjumble)
    {
      for (tmp = st + 1; *tmp != '\0'; tmp++)
	{
	  *tmp = *tmp - (tt / 10);
	}
    }
  strcat (jr, st);
  return jr;
}

void 
jumble (char *name)
{
  char c;
  FILE *fpp;
  FILE *fpo;
  char buffr[500];
  char buffr2[500];
  char co;
  sprintf (buffr, "%s%s.u", PLAYER_DIR, capitalize (name));
  sprintf (buffr2, "%s%s", PLAYER_DIR, jumble_name (capitalize (name), FALSE));
  if ((fpp = fopen (buffr, "r")) == NULL)
    return;
  fpo = fopen (buffr2, "w+");
  do
    {
      c = getc (fpp);
      co = c;
      c += UPPER (name[0]) / 8;
      fprintf (fpo, "%c", c);
    }
  while (co != EOF);
  fclose (fpp);
  fclose (fpo);
  return;
  return;
}

void 
unjumble (char *name, bool convert)
{
  char c;
  char co;
  FILE *fpp;
  FILE *fpo;
  char buffr[500];
  char buffr2[500];
  char buffr3[1000];
  char buffr4[500];
  strcpy (buffr4, name);
  sprintf (buffr, "%s%s.u", PLAYER_DIR, capitalize (name));
  sprintf (buffr2, "%s%s", PLAYER_DIR, jumble_name (capitalize (name), FALSE));
  if ((fpp = fopen (buffr2, "r")) == NULL)
    return;
  fpo = fopen (buffr, "w+");
  do
    {
      c = getc (fpp);
      co = c;
      c -= UPPER (name[0]) / 8;
      fprintf (fpo, "%c", c);
    }
  while (co != EOF);
  fclose (fpp);
  fclose (fpo);
  if (convert)
    {
      sprintf (buffr3, "mv %s %s%s", buffr, PLAYER_DIR, buffr4);
      fprintf (stderr, "%s\n", buffr3);
      system (buffr3);
      sprintf (buffr3, "rm %s", buffr2);
      fprintf (stderr, "%s\n", buffr3);
      system (buffr3);
    }
  return;
}

void 
dummy_pc (void)
{
  int i;
  CHANNEL *c;
  base_pc.next = NULL;
  base_pc.pwd = str_dup ("Bad_data");
  base_pc.rpwd = str_dup ("Nonhere");
  base_pc.rprompt = str_dup ("*hn*h *mn*m >");
#ifdef ALLOW_AUCTIONS
  base_pc.listen_auction = FALSE;
#endif
  base_pc.beamin = str_dup ("Bad_bamfin");
  base_pc.beamout = str_dup ("Bad_bamfout");
  base_pc.title = str_dup ("Bad_title_on_NPC");
  for (i = 0; i < MAXST; i++)
    base_pc.storage[i] = NULL;
  base_pc.chan_ignore = 0;
  i = 0;
  for (c = chan_first; c != NULL; c = c->next)
    {
      strcpy (base_pc.chan_color[i], c->color);
      i++;
    }

  for (i = 0; i < MAX_IGNORE; i++)
    base_pc.ignore[i] = NULL;
  for (i = 0; i < MAX_COLORS; i++)
    base_pc.colors[i] = 7;
  base_pc.voting_on = -10;
  base_pc.nat_armor = 100;
  base_pc.nat_abilities = 0;
  base_pc.email[0] = '\0';
  base_pc.questflag = 0;
  base_pc.cool_skills = 0;

  base_pc.remort_times = 0;
  base_pc.tickcounts = 0;
  base_pc.auction_count = 0;
  base_pc.warpoints = 0;
  base_pc.challenge_on = FALSE;
  base_pc.arena_msg = 0;
  base_pc.challenged = 0;
  base_pc.challenged_by = NULL;
  base_pc.challenge_time = 0;
  base_pc.temp = NULL;
  base_pc.number_pets = 0;
  base_pc.pagelen = 9999;
  base_pc.guilds = 0;
 
  base_pc.transport_quest = 0;
  base_pc.no_quit_pk = 0; /* JRAJRA */
  for (i = 0; i < MAX_TROPHY; i++)
    base_pc.trophy_name[i][0] = '\0';
  for (i = 0; i < MAX_TROPHY; i++)
    base_pc.trophy_times[i] = 0;
  for (i = 0; i < MAX_TROPHY; i++)
    base_pc.trophy_level[i] = 0;
  base_pc.resist_summon = 0;
  base_pc.plus_kick = 0;
  base_pc.plus_sneak = 0;
  base_pc.plus_hide = 0;
  base_pc.saving_throw = 0;
  base_pc.logon = 0;
  base_pc.created = 0;
  base_pc.last_note = 0;
  base_pc.practice = 0;
  base_pc.learn = 0;
  base_pc.deaf = 0;
  base_pc.armor[0] = 0;
  base_pc.armor[1] = 0;
  base_pc.armor[2] = 0;
  base_pc.armor[3] = 0;
  for(i = 0; i < NUM_STATS; i++)
    {
      base_pc.stat[i] = -5;
      base_pc.stat_mod[i] = -5;
    }
  base_pc.wizinvis = 0;
  base_pc.security = 9;
 
  base_pc.n_mana = 5000;
  base_pc.n_max_mana = 5000;
  
  for (i = 0; i < SKILL_COUNT; i++)
    base_pc.learned[i] = -100;
  base_pc.bank = 0;
  base_pc.totalkills = 0;
  base_pc.killpoints = 0;
  base_pc.deaths = 0;
  base_pc.no_spam = 0;
  base_pc.pkhelpers = 0;
  base_pc.pkilled = 0;
  base_pc.pkers = 0;
  base_pc.pkills = 0;
  base_pc.hard_pkills = 0;
  base_pc.warmth = 0;
  base_pc.quiet = 0;
  for (i = 0; i < MAXALIAS; i++)
    {
      base_pc.aliasname[i] = str_dup ("Bad_aliasname");
      base_pc.alias[i] = str_dup ("Bad_alias");
    }
  base_pc.stance = 0;
  return;
}
