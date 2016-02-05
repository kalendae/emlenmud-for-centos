#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"
#ifndef WINDOWS
#include <sys/time.h>
#endif


char *
strip_ansi_codes (char *txty)
{
  static char buf[2000];
  int x = 0;
  char *point;
  const char *str;
  int length;
  if (txty == NULL)
    {
      sprintf (buf, "...");
      return buf;
    }
  length = strlen (txty);
  if (length >= 1900)
    return txty;
  point = buf;
  str = txty;
  while (*str != '\0' && x < length)
    {
      if (*str == '\033' || *str == '\e' || *str == '\x1B')
	{
	  while (((*str >= '0' && *str <= '9') || *str == '\x1B' || *str == '[' || *str == ';') && *str)
	    {
	      ++x;
	      ++str;
	    }
	  if (*str)
	    {
	      ++x;
	      ++str;
	    }
	}
      else
	{
	  *point = *str;
	  point++;
	  str++;
	  x++;
	}
    }
  *point = '\0';
  return buf;
}

void 
ansi_color (const char *txt, CHAR_DATA * ch)
{
  if (txt != NULL && ch->desc != NULL)
    {
      if (!IS_SET (ch->pcdata->act2, PLR_ANSI) && !IS_SET (ch->pcdata->act2, PLR_VT100))
	return;
      else if (IS_SET (ch->pcdata->act2, PLR_VT100) && !IS_SET (ch->pcdata->act2, PLR_ANSI))
	{
	  if (!str_cmp (txt, GREEN)
	      || !str_cmp (txt, RED)
	      || !str_cmp (txt, BLUE)
	      || !str_cmp (txt, BLACK)
	      || !str_cmp (txt, CYAN)
	      || !str_cmp (txt, GREY)
	      || !str_cmp (txt, YELLOW)
	      || !str_cmp (txt, PURPLE))
	    return;
	}
      write_to_buffer (ch->desc, txt, strlen (txt));
      return;
    }
  return;
}


void 
act (const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type)
{
  char buf[STD_LENGTH * 2];
  char fname[SML_LENGTH];
  char buf2[STD_LENGTH * 2];
  CHAR_DATA *to;
  SINGLE_OBJECT *obj1 = (SINGLE_OBJECT *) arg1;
  CHAR_DATA *victim = (CHAR_DATA *) arg2;
  SINGLE_OBJECT *obj2 = (SINGLE_OBJECT *) arg2;
  const char *str;
  const char *i;
  char *point;
  int x;

  if (type > 999)
    {
      type -= 1000;
    }

  /* You must act () something */

  if (format == NULL || format[0] == '\0')
    return;

  /* The person must exist, and be in a room, and if the message is to
     the character, the character must not be linkdead. */


  if(!ch || !ch->in_room || (!ch->desc && (type == TO_CHAR || type ==
    TO_CHAR_SPAM)))
     return;

  check_room_more(ch->in_room);
  to = ch->in_room->more->people;

  if (type == TO_VICT)
    {
      if (victim == NULL || !victim->in_room)
        return;
    }
   
  for (; to != NULL; to = to->next_in_room)
    {
      if (to->desc == NULL || !IS_AWAKE (to))
	continue;
      if (type > 3 && to->pcdata->no_spam == 1) 
	/* Gets rid of all the _SPAM things */
	continue;
      switch (type % 4)
	{
	case 0: /* TO_ROOM */
	  if (to == ch)
	    continue;
	  break;
	case 1: /* TO_NOTVICT */
	  if (to == ch || to == victim)
	    continue;
	  break;
	case 2: /* TO_VICT */
	  if (to != victim || ch == victim)
	    continue;
	  break;
	case 3: /* TO_CHAR */
	  if (to != ch)
	    continue;
	  break;
	}
      point = buf;
      str = format;
      while (*str != '\0')
	{
	  if (*str != '$')
	    {
	      *point++ = *str++;
	      continue;
	    }
	  ++str;
	  if (arg2 == NULL && *str >= 'A' && *str <= 'Z'
	      && *str >= '0' && *str <= '9')
	    {
	      bug ("Missing second character for code %d.", *str);
	      i = " !!! ";
	    }
	  else
	    {
	      switch (*str)
		{
		default:
		  i = " !NotGood! ";
		  break;
		case '$':
		  i = "$";
		  break;
		case '1':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI))
		    i = BLUE;
		  break;
		case '2':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI))
		    i = GREEN;
		  break;
		case '3':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI))
		    i = CYAN;
		  break;
		case '4':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI))
		    i = RED;
		  break;
		case '5':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI))
		    i = PURPLE;
		  break;
		case '6':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI))
		    i = YELLOW;
		  break;
		case '7':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI))
		    i = GREY;
		  break;
		case '0':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI))
		    i = BLACK;
		  break;
		case 'B':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI)
		      || IS_SET (to->pcdata->act2, PLR_VT100))
		    i = BOLD;
		  break;
		case 'I':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI)
		      || IS_SET (to->pcdata->act2, PLR_VT100))
		    i = INVERSE;
		  break;
		case 'F':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI)
		      || IS_SET (to->pcdata->act2, PLR_VT100))
		    i = FLASH;
		  break;
		case 'R':
		  i = "";
		  if (IS_SET (to->pcdata->act2, PLR_ANSI)
		      || IS_SET (to->pcdata->act2, PLR_VT100))
		    i = NTEXT;
		  break;
		case 't':
		  i = "<blank>";
		  if (arg1 != NULL)
		    i = (char *) arg1;
		  break;
		case 'T':
		  i = "<BLANK>";
		  if (arg2 != NULL)
		    i = (char *) arg2;
		  break;
		case 'n':
		  {
		    i = rNAME (ch, to);
		    break;
		  }
		case 'N':
		  {
		    i = rNAME (victim, to);
		    break;
		  }
		case 'e':
		  i = HE_SHE (ch);
		  break;
		case 'E':
		  i = HE_SHE (victim);
		  break;
		case 'm':
		  i = HIM_HER (ch);
		  break;
		case 'M':
		  i = HIM_HER (victim);
		  break;
		case 's':
		  i = HIS_HER (ch);
		  break;
		case 'S':
		  i = HIS_HER (victim);
		  break;
		case 'p':
		  if (!obj1)
		    i = "";
		  else
		    i = can_see_obj (to, obj1)
		      ? OOSTR (obj1, short_descr)
		      : "something";
		  break;
		case 'P':
		  if (!obj2)
		    i = "";
		  else
		    i = can_see_obj (to, obj2)
		      ? OOSTR (obj2, short_descr)
		      : "something";
		  break;
		case 'd':
		  if (arg2 == NULL || ((char *) arg2)[0] == '\0')
		    {
		      i = "door";
		    }
		  else
		    {
		      one_argy ((char *) arg2, fname);
		      i = fname;
		    }
		  break;
		}
	    }
	  ++str;
	  while ((*point = *i) != '\0')
	    ++point, ++i;
	}
      if (wwrap)
	{
	  *point++ = '\n';
	  *point++ = '\r';
	}
      *point++ = '\0';
      x = 0;
      strcpy (buf2, buf);
      if (buf2[0] == '\x1B')
	{
	  int cc;
	  cc = 0;
	cap:
	  if (cc < 10)
	    {
	      while (buf2[x] != 'm' && buf2[x] != '\0')
		x++;
	      if (buf2[x] == '\0')
		{
		  x = 0;
		}
	      else
		{
		  x++;
		  if (buf2[x] == '\x1B')
		    {
		      cc++;
		      goto cap;
		    }
		}
	    }
	  /*cc>10 */ 
	  else
	    x = 0;
	}
      buf2[x] = UPPER (buf2[x]);
      if (to->desc != NULL)
	{
	  if (wwrap) 
	    {
	      write_to_buffer (to->desc, wordwrap(buf2,79), 0);
	    }
	  else
	    write_to_buffer (to->desc, buf2, 0);
	}
    }
  return;
}




char *
anseval (const char *txt, CHAR_DATA * to)
{
  static char buf[STD_LENGTH];
  const char *str;
  const char *i;
  char *point;
  char tmp[10];
  if (txt == NULL || txt[0] == '\0')
    return NULL;
  if (to == NULL)
    {
      return NULL;
    }
  point = buf;
  str = txt;
  while (*str)
    {
      while (*str && *str != '$')
	*point++ = *str++;
      if (*str == '$')
	{
	  ++str;
	  switch (*str)
	    {
	    default:
	      sprintf (tmp, "$%c", *str);
	      i = tmp;
	      break;
	    case '1':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI))
		i = BLUE;
	      break;
	    case '2':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI))
		i = GREEN;
	      break;
	    case '3':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI))
		i = CYAN;
	      break;
	    case '4':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI))
		i = RED;
	      break;
	    case '5':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI))
		i = PURPLE;
	      break;
	    case '6':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI))
		i = YELLOW;
	      break;
	    case '7':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI))
		i = GREY;
	      break;
	    case '0':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI))
		i = BLACK;
	      break;
	    case 'B':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI)
		  || IS_SET (to->pcdata->act2, PLR_VT100))
		i = BOLD;
	      break;
	    case 'I':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI)
		  || IS_SET (to->pcdata->act2, PLR_VT100))
		i = INVERSE;
	      break;
	    case 'F':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI)
		  || IS_SET (to->pcdata->act2, PLR_VT100))
		i = FLASH;
	      break;
	    case 'N':
	      i = "";
	      if (IS_SET (to->pcdata->act2, PLR_ANSI)
		  || IS_SET (to->pcdata->act2, PLR_VT100))
		i = NTEXT;
	      break;
	    }
	  *point = '\0';
	  point = buf + strlen (buf);
	  sprintf (point, i);
	  point = buf + strlen (buf);
	  str++;
	}
    }
  *point = '\0';
  return buf;
}
