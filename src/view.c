#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"



char *
rNAME (CHAR_DATA * to, CHAR_DATA * ch)
{
  char tmprace[150];
  static char ret[500];
  bool a_an = FALSE;
  ret[0] = '\0';
  tmprace[0] = '\0';
  if (IS_PLAYER (ch) && IS_PLAYER (to) && DIFF_ALIGN(ch, to) && LEVEL(to) < IMM_LEVEL && LEVEL(ch) < IMM_LEVEL && (can_see_nextdoor (ch, to) || can_see (ch, to)))
    {
		// ARC: show neutral race as any other mob
      if( ALIGN (to) == 0 )
	  	  {
                    if (to->pcdata->remort_times == 0)			
                      sprintf(tmprace, "%s", race_info[to->pcdata->race].name);
                   /* else if (to->pcdata->remort_times < 3)
	  	      sprintf(tmprace, "%s %s", PLAYERSIZE2, race_info[to->pcdata->race].name);
                    else if (to->pcdata->remort_times < 6)
      	              sprintf(tmprace, "%s %s", PLAYERSIZE3, race_info[to->pcdata->race].name);
                    else if (to->pcdata->remort_times < 8 && to->pcdata->sex == SEX_FEMALE)
                      sprintf(tmprace, "%s %s", PLAYERSIZE4F, race_info[to->pcdata->race].name);
                    else if (to->pcdata->remort_times < 8)
                      sprintf(tmprace, "%s %s", PLAYERSIZE4, race_info[to->pcdata->race].name);
                    else if (to->pcdata->remort_times == 8 && to->pcdata->sex == SEX_FEMALE)
	              sprintf(tmprace, "%s %s", PLAYERSIZE5F, race_info[to->pcdata->race].name);
                    else if (to->pcdata->remort_times == 8)
	              sprintf(tmprace, "%s %s", PLAYERSIZE5, race_info[to->pcdata->race].name);
*/
			if (!is_first_letter_vowel(race_info[to->pcdata->race].name))
	  			sprintf(tmprace, "A %s", race_info[to->pcdata->race].name);
	  		else
	  			sprintf(tmprace, "An %s", race_info[to->pcdata->race].name);

	  	  }

	  	  else
                    if (to->pcdata->remort_times >= 0)			
				sprintf(tmprace, "+* %s %s *+", PLAYERSIZE1, race_info[to->pcdata->race].name);
                    /*else if (to->pcdata->remort_times < 3)
				sprintf(tmprace, "+* %s %s *+", PLAYERSIZE2, race_info[to->pcdata->race].name);
                    else if (to->pcdata->remort_times < 6)
				sprintf(tmprace, "+* %s %s *+", PLAYERSIZE3, race_info[to->pcdata->race].name);
                    else if (to->pcdata->remort_times < 8 && to->pcdata->sex == SEX_FEMALE)
				sprintf(tmprace, "+* %s %s *+", PLAYERSIZE4F, race_info[to->pcdata->race].name);
                    else if (to->pcdata->remort_times < 8)
				sprintf(tmprace, "+* %s %s *+", PLAYERSIZE4, race_info[to->pcdata->race].name);
                    else if (to->pcdata->remort_times == 8 && to->pcdata->sex == SEX_FEMALE)
				sprintf(tmprace, "+* %s %s *+", PLAYERSIZE5F, race_info[to->pcdata->race].name);
                    else if (to->pcdata->remort_times == 8)
				sprintf(tmprace, "+* %s %s *+", PLAYERSIZE5, race_info[to->pcdata->race].name);

	  	 */ {
			if (!is_first_letter_vowel(race_info[to->pcdata->race].name))
				sprintf(tmprace, "+* A %s *+", race_info[to->pcdata->race].name);
			else
				sprintf(tmprace, "+* An %s *+", race_info[to->pcdata->race].name);

	  	  }

      strcpy (ret, tmprace);
    }
  else
    strcpy (ret, PERS (to, ch));
  return ret;
}

void
replace (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("replace", replace, POSITION_DEAD, 0, LOG_ALWAYS, "This command allows you to replace 1 learn for 10 practices or 10 practices for 1 learn.")

    if (!argy || argy[0] == '\0' || (str_cmp (argy, "learns") && str_cmp (argy, "practices")))
    {
      send_to_char ("Format is 'replace <learns>' to convert 1 learn to 10 practices.\n\r", ch);
      send_to_char ("Format is 'replace <practices>' to convert 10 practices to 1 learn.\n\r", ch);
      return;
    }
  if (!str_cmp (argy, "learns"))
    {
      if (ch->pcdata->learn < 1)
	{
	  send_to_char ("You don't have any learns to convert.\n\r", ch);
	  return;
	}
      ch->pcdata->learn--;
      ch->pcdata->practice += 10;
      send_to_char ("Done.\n\r", ch);
      return;
    }
  if (!str_cmp (argy, "practices"))
    {
      if (ch->pcdata->practice < 10)
	{
	  send_to_char ("You don't have 10 practices to convert.\n\r", ch);
	  return;
	}
      ch->pcdata->learn++;
      ch->pcdata->practice -= 10;
      send_to_char ("Done.\n\r", ch);
      return;
    }
  return;
}

/* NOT USED ATM
void do_towers (CHAR_DATA * ch, char *argy)
{
  char buf[100];
  char buff[100];
  DEFINE_COMMAND ("towers", do_towers, POSITION_STANDING, 1, LOG_NORMAL, "This command lists the Tower align bonuses.")
  if (IS_MOB(ch)) return;
  send_to_char("\x1b[1;37m Status of the Towers \n\r", ch);
  send_to_char("\x1b[0;37m----------------------\n\r", ch);
  send_to_char("\x1b[1;37mAttribute        Align\n\n\r", ch);
  sprintf(buf, "\x1b[0;37m   Str             %d\n\r", str_tower);
  send_to_char(buf, ch);
  sprintf(buf, "   Dex             %d\n\r", dex_tower);
  send_to_char(buf, ch);
  sprintf(buf, "   Con             %d\n\r", con_tower);
  send_to_char(buf, ch);
  sprintf(buf, "   Int             %d\n\r", int_tower);
  send_to_char(buf, ch);
  sprintf(buf, "   Wis             %d\n\r", wis_tower);
  send_to_char(buf, ch);
  return;
} */


void do_alliance (CHAR_DATA * ch, char *argy)
{
  char arg1[SML_LENGTH];
  char arg2[SML_LENGTH];
  char buf[500];
  char buff[100];
  int num1 = -1;
  int num2 = -1;

  DEFINE_COMMAND ("alliance", do_alliance, POSITION_STANDING, 1, LOG_NORMAL, "This command lists all of the current alliances and lets imms set alliances.")
    if (IS_MOB(ch)) return;

  if (argy[0] == '\0' || argy == "" || LEVEL(ch) < MAX_LEVEL)
    {
      int i;
      int j;
      buf[0] = '\0';
      send_to_char("This chart tells which sides are allied with which. \n\n\r", ch);
      sprintf (buf, "                    ");
      for (j = 0; j < NUM_ALIGN; j ++)
	{
	  sprintf(buff, "%2d   ", j);
	  strcat(buf, buff);
	}
      strcat(buf, "\n\r");
      send_to_char(buf, ch);
      for (i = 0; i < NUM_ALIGN; i++)
	{
	  sprintf(buf, "\x1b[0;37m%2d. %-15s  ", i, align_info[i].name);
	  for (j = 0; j < NUM_ALIGN; j++)
	    {
	      if (alliance[i][j] == TRUE)
		strcat(buf, "\x1b[1;37mY    ");
	      else
		strcat(buf, "\x1b[1;30mN    ");
	    }
	  strcat(buf, "\n\r");
	  send_to_char(buf, ch);
	}
      return;
    }
  argy = one_argy(argy, arg1);
  argy = one_argy(argy, arg2);
  if (!is_number(arg1) || !is_number(arg2))
    {
      send_to_char ("Syntax: alliance <num1> <num2>\n\r", ch);
      return;
    }
  num1 = atoi(arg1);
  num2 = atoi(arg2);
  if (num1 < 0 || num1 >= NUM_ALIGN || num2 <0 || num2 >= NUM_ALIGN)
    {
      send_to_char("The numbers you pick for the alignments must be at least 0 and less than the number of alignments.\n\r", ch);
      return;
    }
  if (num1 == num2)
    {
      send_to_char("A side cannot be set against itself\n\r", ch);
      alliance[num1][num2] = TRUE;
      fwrite_alliance_data();
    }
  if (alliance[num1][num2] && alliance[num2][num1])
    {
      sprintf(buf, "Ok, ending the alliance between the %s and the %s\n\r", align_info[num1].name, align_info[num2].name);
      send_to_char(buf, ch);
      alliance[num1][num2] = FALSE;
      alliance[num2][num1] = FALSE;
      fwrite_alliance_data();
      return;
    }
  else
    {
       sprintf(buf, "Ok, creating an alliance between the %s and the %s\n\r", align_info[num1].name, align_info[num2].name);
      send_to_char(buf, ch);
      alliance[num1][num2] = TRUE;
      alliance[num2][num1] = TRUE;
      fwrite_alliance_data();
      return;
    }
  return;
}

/* ARC:
	Included this utility for better english.
 */

bool is_first_letter_vowel ( char* sample )
{
	if( sample[0] == 'a' ||
		sample[0] == 'A' ||
		sample[0] == 'e' ||
		sample[0] == 'E' ||
		sample[0] == 'i' ||
		sample[0] == 'I' ||
		sample[0] == 'o' ||
		sample[0] == 'O' ||
		sample[0] == 'u' ||
		sample[0] == 'U' )

		return true;

	return false;

}
