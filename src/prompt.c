#include <sys/types.h>
#ifdef Linux
#include <sys/time.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"


bool 
check_pr (char *txt)
{
  char *t;
  bool last_a = FALSE;
  for (t = txt; *t != '\0'; t++)
    {
      if (*t == '%')
	return FALSE;
      if (*t == '$')
	{
	  last_a = TRUE;
	  continue;
	}
      if (last_a && (*t < '0' || *t > '9') && *t != 'B' && *t != 'R')
	{
	  return FALSE;
	}
      last_a = FALSE;
    }
  return TRUE;
}


extern bool prmpt;

void 
print_prompt (CHAR_DATA * ch)
{
  char buf[STD_LENGTH];
  const char *i;
  char prompt[2*STD_LENGTH];
  char rest[STD_LENGTH];
  char *t;
  int ii;
  int xii;
  prompt[0] = '\0';
  rest[0] = '\0';
  if (IS_MOB(ch)) return;
  if (ch->wait > 1)
    strcat(prompt, "*");
  if (ch->pcdata->tickcounts > 11)
    strcat(prompt, "+");
  if (ch->fgt && ch->fgt->ears > 10)
    {
      sprintf (buf, "[%d] ", ch->fgt->ears - 10);
      strcat(prompt, buf);
    }
  if (ch->position == POSITION_CASTING && FIGHTING (ch) != NULL)
    strcat(prompt, "(Casting) ");
  if (ch->position == POSITION_BASHED)
    strcat (prompt, "(Bashed) ");
  if (ch->position == POSITION_SHOOTING)
    strcat(prompt, "(Aiming) ");
  wwrap = FALSE;
  ii = ch->position;
  ch->position = POSITION_STANDING;
  ch->pcdata->cursor = 0;
  strcpy (rest, interpret_line (ch->pcdata->rprompt, ch));
  for (t = rest; *t != '\0'; t++)
    {
      if (*t == '$')
	{
	  t++;
	  continue;
	}
      if (*t == '\x1B')
	{
	  t++;
	  while (*t != 'm' && *t != 'f' && *t != 'H' && *t != 'r' && *t != 'J' && *t != '\0')
	    t++;
	  continue;
	}
      if (*t == '\n' || ch->pcdata->cursor > 79)
	{
	  ch->pcdata->cursor = 0;
	}
      ch->pcdata->cursor++;
    }
  ch->pcdata->cursor++;
  strcat (prompt, rest);
  act (prompt, ch, NULL, ch, TO_CHAR);
  ch->position = ii;
  wwrap = TRUE;
  if (ch->desc && ch->desc->host[29] == 1)
    gotoxy (ch, ch->pcdata->cursor, ch->pcdata->pagelen);
  if (ch->desc && ch->desc->host[29] == 1)
    {
      send_to_char (ch->desc->inbuf, ch);
      ch->pcdata->cursor += strlen (ch->desc->inbuf);
    }
  return;
}

void 
try_prompt (char *tx, CHAR_DATA * ch)
{
  static char hugebuf[2048];
  int ii;
  const char *i;
  hugebuf[0] = '\0';
  i = hugebuf;

  send_to_char ("\x1B[37;0m", ch);
  wwrap = FALSE;
  ii = ch->position;
  ch->position = POSITION_STANDING;
  act (interpret_line (tx, ch), ch, NULL, ch, TO_CHAR);
  ch->position = ii;
  wwrap = TRUE;
  send_to_char ("\n\r", ch);
  return;
}


char *prompts[] =
{
  "$B$1*hn*/$1*hm*(*hw*) $R$7Mv:$B$2*mn*/*mm* $R$7Ma:$B$5*mnn*/*mmm* $R$7XP:$B$4*xl* $R$7> *fi* *tk* ",
  "$R$6LvL-*lvl* $2Hp*hn*/*hm*$3(*hw*) $6Mv*mn*/*mm* $2Ma*mnn*/*mmm* $3Xp*xl* $2Apb*ap*$R *fi* *tk* ",
  "$B$2*hn*/$2*hm*hp (*hw*$2) $3*mn*/$3*mm*$3mv $5*mnn*$5mn$4 [$3*lvl*$4] $4(*xl*)$R *fi* *tk* ",
  "$B$1*hn*/*hm*$R$7hp$B$2 *mn*/*mm*$R$7mv $B$5*mnn*/*mmm*$R$7m *fi**tk*> ",
  "$R$3*hn*$7/$3*hm*$7h$2 *mn*$7/$2*mm*$7mv $5*mnn*$7/$5*mmm*$7m *fi**tk*> ",
  "$R*tk**hn*H *mn*M *fi*> ",
  "$R*tk**fi**hn*H *mn*Mv *mnn*Ma> ",
  "$B$2*hn*$R$7H $B$5*mn*$R$7Mv $B$6*mnn*$R$7Ma *fi**tk*> ",
  "$R*hn*/*hm*H *mn*/*mm*Mv *mnn*Ma *fi**tk*> ",
  "$R*hn*h *mn*m XL:*xl* *fi**tk*> ",
  "$R*hn*/*hm*H *mn*/*mm*Mv *mnn*Ma XL(*xl*) *fi**tk*> ",
  "$B$2*hn*$R$7hp $B$6*mn*$R$7mv $B$3*mnn*$R$7ma $B$5(*xl*)$R$7 *fi**tk*> ",
  "$RH:*hn*/*hm* M:*mn*/*mm* XL:*xl* *fi**tk*> ",
  "$RH:*hn* M:*mn* Ma:*mnn* XL:*xl* *fi**tk*> ",
  "$B$2*hn*$R$7h $B$6*mn*$R$7m $B$1(*xl*)$R$7 *fi**tk*> ",

  "$R$2*hn*$7h $6*mn*$7m $5(*xl*)$7 *fi**tk*> ",
  "$R$2*hn*$7h $6*mn*$7mv $B$7*mnn*$R$7ma $5(*xl*)$7 *fi**tk*> ",
  "$R$2H:*hn*/*hm* $3M:*mn*/*mm* $5XL:*xl*$7 *fi**tk*> ",
  "$B$2H:*hn*/*hm* $B$6M:*mn*/*mm* $B$1XL:*xl*$R$7 *fi**tk*> ",
  "END"
};


void 
set_default_prompt (CHAR_DATA * ch, char option)
{
  if (!ch)
    return;
  option = UPPER (option);
  if (option < 'A' || option > 'P')
    option = 'A';
  if (ch->pcdata->rprompt)
    free_string (ch->pcdata->rprompt);
  ch->pcdata->rprompt = str_dup (prompts[(option - 'A')]);

  return;
}
