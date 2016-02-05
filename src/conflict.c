#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

const char *align_name[]= {
"Newbie",
"Alckar",
"Borlog",
"Candel",
"Diffler",
"Exorla",
"Fenglo",
"Goora",
"Heblorn",
"Invect",
"Jurator",
};

void 
read_conflict_data (void)
{
  FILE *fp;
  char t[50];
  int i = 0;
  return;
  for (i = 0; i < NUM_ALIGN; i++)
    align_name[i][0] = '\0';
  if ((fp = fopen ("conflict.dat", "r")) == NULL)
    {
      fprintf (stderr, "Could not open conflict.dat!\n");
      exit (5);
    }
  for (;;)
    {
      strcpy (t, fread_word (fp));
      if (!str_cmp (t, "END"))
	break;
      if (i >= NUM_ALIGN) break;
      if (!str_prefix ("#", t))
	{
	  fread_to_eol (fp);
	  continue;
	}
      if (!str_cmp(t, "ALIGN") && i < NUM_ALIGN)
	{
	  strcpy(align_name[i],fread_word (fp));
	  continue;
	}
    }
  fclose (fp);
  return;
}
