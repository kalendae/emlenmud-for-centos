
/* Some of this isn't my code - 99.9 of the percent that isn't, isn't used */

#include <memory.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "emlen.h"
/* strtoint() converts the given string to an integer. */
int 
strtoint (char *string)
{
  int k = 0;
  sscanf (string, "%d", &k);
  return k;
}

bool 
affect_worn (CHAR_DATA * ch, int bit)
{
  SINGLE_OBJECT *obj, *obj_next;
  
    return FALSE;
}

int 
words (char *s)
{
  int count;
  count = 0;
  while (*s != '\0')
    {
      if (isalpha (*s) || isdigit (*s))
	{
	  while (isalpha (*s) || isdigit (*s))
	    s++;
	  count++;
	}
      else
	{
	  while (!isalpha (*s))
	    {
	      if (*s == '\0')
		break;
	      s++;
	    }
	}
    }
  return count;
}

bool 
aliasok (char *aliasname, char *s)
{
  if (!isalpha (*s) && !isdigit (*s))
    while (*s && !isalpha (*s) && !isdigit (*s))
      s++;
  if (*s && !str_cmp (first (s), aliasname))
    return FALSE;
  while (*s != '\0')
    {
      if (!str_cmp (first (s), aliasname))
	return FALSE;
      while (*s && *s != '*')
	s++;
      if (*s == '*')
	s++;
    }
  return TRUE;
}

char *
word (int wd, char *s)
{
  int current = 0;
  while (*s != '\0' && current < wd)
    {
      if (*s != ' ')
	{
	  current++;
	  while (*s != ' ')
	    s++;
	}
      else
	{
	  while (*s == ' ')
	    s++;
	  if (*s == '\0')
	    break;
	}
    }
  return s;
}

/* strrepl() will replace each occurence of "old" with "new" in
   * the string "s". Mainly coded for use in comm.c (aliases etc)
   * but may be useful other places.
 */
void 
strrepl (char *s, char new, char old)
{
  char *ptr;
  int len;
  len = strlen (s);
  while (len)
    {
      ptr = memchr (s, old, len);
      if (ptr)
	{
	  *ptr = new;
	  len -= (ptr - s);
	  s = ptr + 1;
	}
      else
	{
	  break;
	}
    }
  return;
}

/* first() will return the first word in string s
   * (different in one argy in the way that it recognizes \n's)
 */
char *
first (char *s)
{
  static char tmp[4096];
  char *new;
  new = tmp;
  while (!isalpha (*s) && !isdigit (*s) && *s)
    s++;
  while ((isalpha (*s) || isdigit (*s)) && *s)
    *new++ = *s++;
  *new = '\0';
  return tmp;
}

/* strsub() will replace all occurences of old with new in string s
 */
bool 
strsub (char *s, char *old, char *new)
{
  char *ptr;
  static char tptr[5000];
  int plen, rlen;
  strcpy (tptr, s);
  ptr = s;
  plen = strlen (old);
  rlen = strlen (new);
  if ((ptr = strstr (s, old)) == NULL)
    return FALSE;
  strcpy (tptr, ptr + plen);
  while (rlen--)
    *ptr++ = *new++;
  strcpy (ptr, tptr);
  return TRUE;
}
