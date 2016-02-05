#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"

void 
check_pkdata (CHAR_DATA * ch)
{
  int k, i;
  int oldslot[PKN];
  int newslot[PKN];
  float charinfo[PKN];
  bool in_pkdata_already[PKN];
  bool foundnew[PKN];
  if (IS_MOB (ch))
    return;
  if (LEVEL(ch) > 90) return;

/* Prevent Neutrals from appearing in pkdata */
  if (ALIGN (ch) == 0 )
    return;

  for (i = 0; i < PKN; i++)
    charinfo[i] = 0.0;

  if (ch->pcdata->warpoints <0) ch->pcdata->warpoints = 0;
  if (current_time - ch->pcdata->created > 0 &&  current_time - startup_time > 0) 
    {
       charinfo[10] = (float)((float) (ch->pcdata->played + (current_time
- ch->pcdata->logon)))*100.0/((float)(current_time-ch->pcdata->created));
      charinfo[11] = (float)((float)
(ch->pcdata->played)*100.0)/((float)(current_time-startup_time));
    }
  if (ch->pcdata->pkills > 20)
    {
      charinfo[0] = (float) ch->pcdata->pkills;
      charinfo[1] = (float) ((float) ch->pcdata->pklevels /
			       (float) ch->pcdata->pkills);
      charinfo[2] = (float) ((float) ch->pcdata->pkills /
			     (float) ch->pcdata->pkhelpers);
      charinfo[5] = (float)   ch->pcdata->total_wps;
      charinfo[6] =  (float) ((float) ch->pcdata->pkills /
			       (float) ch->pcdata->pklevels);
      charinfo[7] = (float) ((float) ch->pcdata->pkhelpers/
			     (float) ch->pcdata->pkills); 
      charinfo[8] = (float) ch->pcdata->warpoints;
      charinfo[9] = (float) ch->pcdata->hard_pkills;
     
      
      

    }
  if (ch->pcdata->pkilled >=1)
    {
      charinfo[3] = (float) ((float) ch->pcdata->pkilled);
      charinfo[4] = (float) ((float) ch->pcdata->pkers /
				    (float) ch->pcdata->pkilled);
    }
  for (k=0; k < PKN; k++)
    {
      oldslot[k] = -1;
      in_pkdata_already[k] = FALSE;
      newslot[k] = -1;
      foundnew[k] = FALSE;
    }
  for (i=0; i < PKN; i++)
    {
      for (k = 0; k < MAX_PKDATA; k++)
	{
          if (!str_cmp (capitalize (NAME (ch)), pkdata[k][i].name))
	    {   
	      in_pkdata_already[i] = TRUE;
	      oldslot[i] = k;  
	    }
	}
    }
  send_to_char("\n\r", ch);
  for (i = 0; i < PKN; i++)
    {
      if (in_pkdata_already[i] && (oldslot[i] <= (MAX_PKDATA-2)) && (oldslot[i] >=0))
	{
	  for (k = oldslot[i]; k < (MAX_PKDATA -1); k++)
	    {
	      if (k > (MAX_PKDATA-2))
		break;
	      strcpy (pkdata[k][i].name, pkdata[k+1][i].name);
	      pkdata[k][i].value = pkdata[k+1][i].value;
	      pkdata[k][i].alignment = pkdata[k+1][i].alignment;
	    }
	  strcpy (pkdata[MAX_PKDATA-1][i].name, "Free-Slot");
	  pkdata[MAX_PKDATA-1][i].value = 0.0;
	  pkdata[MAX_PKDATA-1][i].alignment = FALSE;
	}
    }
  for(i=0; i < PKN; i++)
    {
      if (charinfo[i] > 0)
	{
	  for (k=0; k < MAX_PKDATA; k++)
	    { 
	      if(charinfo[i] > pkdata[k][i].value)
		{
		  newslot[i] = k;
		  break;
		}
	    }
	}
      else
	newslot[i] = -1;
    }
  for(i=0; i < PKN;i ++)
    {
      if ((newslot[i] >=0) && (newslot[i] < (MAX_PKDATA-1)))
	{
	  for (k = (MAX_PKDATA -1); k > newslot[i]; k--)
	    {
	      strcpy (pkdata[k][i].name, pkdata[k-1][i].name);
	      pkdata[k][i].value = pkdata[k-1][i].value;
	      pkdata[k][i].alignment = pkdata[k-1][i].alignment;
	    }
	}
      if(newslot >=0)
	{
	  strcpy (pkdata[newslot[i]][i].name, capitalize (NAME (ch)));
	  pkdata[newslot[i]][i].alignment = ch->pcdata->alignment;
	  pkdata[newslot[i]][i].value = charinfo[i];
	  strcpy (pkdata[newslot[i]][i].name, capitalize (NAME (ch)));
	}
    }
  return;
}





void 
check_topten (CHAR_DATA * ch)
{
  int k;
  int oldslot;
  int newslot;
  bool in_topten_already;
  if (IS_MOB (ch))
    return;
  if (LEVEL (ch) > 90 ) return;

/* Prevent Neutrals from appearing on topten */
  if (ALIGN (ch)== 0 )
    return;

  oldslot = -1;
  in_topten_already = FALSE;
  if (ch->pcdata->warpoints  < 0) ch->pcdata->warpoints = 0;
  for (k = 0; k < MAX_TOPTEN; k++)
    {
      if (!str_cmp (capitalize (NAME (ch)), topten.topten_name[k]))
         {
            in_topten_already = TRUE;
            oldslot = k;
            break;  
         }
    }
   if (in_topten_already && (oldslot <= (MAX_TOPTEN-2)))
      {
	  for (k = oldslot; k < (MAX_TOPTEN -1); k++)
	    {
	      if (k > (MAX_TOPTEN-2))
		break;
	      strcpy (topten.topten_name[k], topten.topten_name[k + 1]);
	      topten.topten_warpoints[k] = topten.topten_warpoints[k + 1];
	      topten.topten_alignment[k] = topten.topten_alignment[k + 1];
	    }
          strcpy (topten.topten_name[MAX_TOPTEN-1], "Free-Slot");
          topten.topten_warpoints[MAX_TOPTEN-1] = 0;
          topten.topten_alignment[MAX_TOPTEN-1] = TRUE;
	}
      newslot = -1;
      for (k=0; k < MAX_TOPTEN; k++)
	{
          if (ch->pcdata->warpoints > topten.topten_warpoints[k])
             {  
               newslot = k;
               break;
             }
        }
 if (newslot >=0)
   {
     /*Move everyone after that slot down on the list */
     for (k = (MAX_TOPTEN -1); k > newslot; k--)
       {
	 strcpy (topten.topten_name[k], topten.topten_name[k - 1]);
	 topten.topten_warpoints[k] = topten.topten_warpoints[k - 1];
	 topten.topten_alignment[k] = topten.topten_alignment[k - 1];
       }
     topten.topten_warpoints[newslot] = ch->pcdata->warpoints;
     strcpy (topten.topten_name[newslot], capitalize (NAME (ch)));
   
     topten.topten_alignment[newslot] = ch->pcdata->alignment;
    
     
   }
 if (number_range(1,3) == 2)
   save_topten ();
 return;
 
}



void
check_rating (CHAR_DATA * ch)
{
  int k;
  int oldslot = -1;
  int newslot;
  bool in_rating_already;
  int ratng = rating(ch);
  if (IS_MOB (ch))
    return;
  if (LEVEL(ch) > 91)
     return;

/* Prevent Neutrals from appearing on rating */
  if (ALIGN(ch)==0)
     return;

  in_rating_already = FALSE;
  for (k = 0; k < MAX_RATING; k++)
    {
      if (!str_cmp (capitalize (NAME (ch)), ratingd.rating_name[k]))
	{
	  in_rating_already = TRUE;
	  oldslot = k;
	  break;  
	}
    }
  if (in_rating_already && (oldslot < (MAX_RATING-1)))
    {
      for (k = oldslot; k < (MAX_RATING -1); k++)
	{
	  if (k > (MAX_RATING-1))
	    break;
	  strcpy (ratingd.rating_name[k], ratingd.rating_name[k + 1]);
	  ratingd.rating_rating[k] = ratingd.rating_rating[k + 1];
	  ratingd.rating_alignment[k] = ratingd.rating_alignment[k + 1];
	}
      
      strcpy (ratingd.rating_name[MAX_RATING-1], "<Free-Slot>");
      ratingd.rating_rating[MAX_RATING-1] = 0;
      ratingd.rating_alignment[MAX_RATING-1] = TRUE;
    }
  if (in_rating_already)
    ratingd.rating_rating[MAX_RATING-1] = 0;
  newslot = -1;
  for (k=0; k < MAX_RATING; k++)
    {
      if (ratng > ratingd.rating_rating[k])
	{  
	  newslot = k;
	  break;
	}
      
    }
  if (newslot >=0)
    {
      if (newslot < (MAX_RATING-1))
	{
	  /*Move everyone after that slot down on the list */
	  for (k = (MAX_RATING -1); k > newslot; k--)
	    {
	      strcpy (ratingd.rating_name[k], ratingd.rating_name[k - 1]);
	      ratingd.rating_rating[k] = ratingd.rating_rating[k - 1];
	      ratingd.rating_alignment[k] = ratingd.rating_alignment[k - 1];
	    }
	}
      ratingd.rating_rating[newslot] = rating(ch);
      strcpy (ratingd.rating_name[newslot], capitalize (NAME (ch)));
      ratingd.rating_alignment[newslot] = ch->pcdata->alignment;
      
      if (number_range(1,3) == 2)
	save_rating ();
    }
  return;
}


void 
do_trophy (CHAR_DATA * ch, char *argy)
{
  bool foundy;
  char buffy[500];
  int i;
  DEFINE_COMMAND ("trophy", do_trophy, POSITION_SLEEPING, 0, LOG_NORMAL, "Shows your mob or pkill trophy.")

    if (IS_MOB (ch))
    return;

  foundy = FALSE;
  send_to_char (" \x1B[1;34m[\x1B[30mTrophy of Kills\x1B[34m]\x1B[1;37m\n\r\n\r", ch);
  sprintf (buffy, "%5s %6s %5s\n\r",
	   "\x1B[1;30m[\x1B[32mLvl\x1B[30m]",
	   "\x1B[1;31m[\x1B[37mName\x1B[31m]",
	   "\x1B[1;36m[\x1B[35mTimes\x1B[36m]");
  send_to_char (buffy, ch);
  for (i = 0; i < MAX_TROPHY; i++)
    if ((ch->pcdata->trophy_name[i] != '\0') && (ch->pcdata->trophy_level[i] >=1) && (ch->pcdata->trophy_times[i] >=1))
      {
	foundy = TRUE;
	sprintf (buffy,
		 "\x1B[37;0m[\x1B[1;34m%3d\x1B[37;0m] %-23s\x1B[1;37m*%-4d\n\r", 
		 ch->pcdata->trophy_level[i],
		 ch->pcdata->trophy_name[i], ch->pcdata->trophy_times[i]);
	send_to_char (buffy, ch);
      }
  if (!foundy)
    {
      send_to_char ("\n\rNone.\n\r", ch);
    }
  send_to_char ("\x1B[37;0m", ch);
  return;
}


void 
do_topten (CHAR_DATA * ch, char *argy)
{
  char buffy[500];
  char buf[200];
  int position;
  int i;
  DEFINE_COMMAND ("topten", do_topten, POSITION_SLEEPING, 0, LOG_NORMAL, "Shows the top PKillers.")
    
    if (IS_MOB (ch))
      return;
  position = ch->position;
  ch->position = POSITION_STANDING;
  if (LEVEL(ch) < 90)
    check_topten(ch);

  if (!str_cmp(argy, "reset") && LEVEL(ch) == MAX_LEVEL)
    {
      send_to_char("Resetting topten.\n\r", ch);
      for (i = 0; i < MAX_RATING; i++)
	{
           strcpy(topten.topten_name[i], "Free-Slot");
           topten.topten_warpoints[i] = 0;
           topten.topten_alignment[i] = 0;      
         }
      return;
    }

  for ( i=0; i < MAX_TOPTEN; i++)
    {
      sprintf (buffy, "\x1b[1;3%dm* $B$0#%-2d $B$7%-35s $B$0-->$B$7 %-6ld$4 $B$4Warpoints$7",
	       (URANGE(0,(topten.topten_alignment[i]-1)*7, 7)),i + 1,  
	       topten.topten_name[i], topten.topten_warpoints[i]);
      act (buffy, ch, NULL, ch, TO_CHAR);
    }
  sprintf(buffy, "\n\rYou have \x1b[1;31m%ld\x1b[0;37m warpoints.\n\r", ch->pcdata->warpoints);
  send_to_char(buffy, ch);
  ch->position = position;
  return;
}



void 
do_pkstat (CHAR_DATA * ch, char *argy)
{
  char buffy[SML_LENGTH];
  int position;
  int i, k;
  char arg1[SML_LENGTH];
  DEFINE_COMMAND ("pkstat", do_pkstat, POSITION_SLEEPING, 0, LOG_NORMAL,
"Shows the pkstat info.")

    if (IS_MOB (ch))
    return;
    if (LEVEL(ch) < 91) check_pkdata (ch);
  position = ch->position;
  ch->position = POSITION_STANDING;
  if (number_range(1,5) == 4)
    save_pkdata();
  if (!str_cmp(argy, "reset") && LEVEL(ch) == MAX_LEVEL)
    {
      for (i = 0; i < MAX_PKDATA; i++)
        {
          for (k = 0; k < PKN; k++)
            {
               strcpy(pkdata[i][k].name, "Free-Slot");
               pkdata[i][k].alignment = 0;
               pkdata[i][k].value = 0.0;
            }
        }
      send_to_char("Resetting pkdata.\n\r", ch);
      return;
    }    


  argy = one_argy(argy, arg1);

  if (!str_cmp(arg1, "pkills"))
    {
      i=0;
      send_to_char("\x1b[0;31mTotal number of pkills:\n\n\r", ch);
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  int pkillz = (int) pkdata[k][i].value;
	  sprintf (buffy, "\x1b[1;3%dm* $R$4#%-2d $3%-35s $0-->$5 %-7d",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, pkillz);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
      sprintf(buffy, "\n\rYour total number of pkills is: \x1b[1;32m%d\x1b[0;37m.\n\r", ch->pcdata->pkills);
      send_to_char(buffy, ch);
      ch->position = position;
      return;
    }

 if (!str_cmp(arg1, "ave"))
    {
      send_to_char("\x1b[0;31mAverage level of pkill victims:\n\n\r",ch);
      i=1;
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  sprintf (buffy, "\x1b[1;3%dm* $B$2%-2d $0%-35s $4-->$1 %2.4f",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, pkdata[k][i].value);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
      sprintf(buffy, "\n\rYour average level of pkill victims is \x1b[1;34m%2.4f\x1b[0;37m.\n\r", (float) ((float)ch->pcdata->pklevels)/((float) (UMAX(ch->pcdata->pkills, 1))));
      send_to_char(buffy, ch);
      ch->position = position;
      return;
    } 
 
 if (!str_cmp(arg1, "num"))
    { 
      i=2;
      send_to_char("\x1b[0;31mYour average groupsize when you pkill someone.\n\n\r", ch);
      for ( k=0; k < MAX_PKDATA; k++)
	{
	
	  float groupsize;
	if (pkdata[k][i].value > 0)
           groupsize = 1 / pkdata[k][i].value;
        else groupsize = 0;
	  sprintf (buffy, "\x1b[1;3%dm* $B$1#%-2d $2%-35s $5-->$0% 2.4f",
	     (pkdata[k][i].alignment), k + 1,  
            pkdata[k][i].name, groupsize);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
      sprintf(buffy, "\n\rYour average groupsize when you pkill someone is \x1b[1;34m%2.4f\x1b[0;37m.\n\r", (float) ((float)ch->pcdata->pkhelpers)/((float) (UMAX(ch->pcdata->pkills, 1))));
	send_to_char(buffy, ch);
      ch->position = position;
      return;
    }

 if (!str_cmp(arg1, "pdeaths") && LEVEL(ch) < 111)
    {
      i=3;
      send_to_char("\x1b[0;31mThe people who have been pkilled the most.\n\n\r", ch);
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  int pdeaths = (int) pkdata[k][i].value;
	  sprintf (buffy, "\x1b[1;3%dm* $B$7#%-2d $5%-35s $B$4-->$1 %-6d$R$7",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, pdeaths);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
         sprintf(buffy, "\n\rYour total number of times you have been pkilled is: \x1b[1;32m%d\x1b[0;37m.\n\r", ch->pcdata->pkilled);
      send_to_char(buffy, ch);
      send_to_char ("\x1B[37;0m", ch);
      ch->position = position;
      return;
    }

 if (!str_cmp(arg1, "dif"))
    {
      i=4;
      send_to_char("\x1b[0;31mThe people who need the most opponents to kill them.\n\n\r", ch);
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  float diff = pkdata[k][i].value;
	  sprintf (buffy, "\x1b[1;3%dm* $B$4#%-2d $3%-35s $0--> $1%2.3f",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, diff);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
      sprintf(buffy, "\n\rYou require \x1b[1;34m%2.4f\x1b[0;37m people to kill you on average.\n\r", (float) ((float)ch->pcdata->pkers)/((float) (UMAX(ch->pcdata->pkilled, 1))));
      send_to_char(buffy, ch);
      ch->position = position;
      return;
    }

 if (!str_cmp(arg1, "total_wps"))
    {
      send_to_char("\x1b[0;31mTotal Warpoints:\n\n\r",ch);
      i=5;
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  int twps = (int) pkdata[k][i].value;
	  sprintf (buffy, "\x1b[1;3%dm* $B$2%-2d $0%-35s $4-->$1 %-3d",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, twps);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
         sprintf(buffy, "\n\rYour total number of warpoints is: \x1b[1;32m%ld\x1b[0;37m.\n\r", ch->pcdata->total_wps);
      send_to_char(buffy, ch);
      send_to_char ("\x1B[37;0m", ch);
      ch->position = position;
      return;
    } 
if (!str_cmp(arg1, "hard"))
    {
      send_to_char("\x1b[0;31mHard Pkills:\n\n\r",ch);
      i=9;
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  int twps = (int) pkdata[k][i].value;
	  sprintf (buffy, "\x1b[1;3%dm* $B$2%-2d $0%-35s $4-->$1 %-3d",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, twps);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
         sprintf(buffy, "\n\rYour total number of hard pkills is: \x1b[1;32m%ld\x1b[0;37m.\n\r", ch->pcdata->hard_pkills);
      send_to_char(buffy, ch);
      send_to_char ("\x1B[37;0m", ch);
      ch->position = position;
      return;
    }
 /*if (!str_cmp(arg1, "warpoints"))
    {
      send_to_char("\x1b[0;31mWarpoints:\n\n\r",ch);
      i=8;
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  int twps = (int) pkdata[k][i].value;
	  sprintf (buffy, "\x1b[1;3%dm* $B$2%-2d $0%-35s $4-->$1 %-3d",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, twps);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
	   sprintf(buffy, "\n\rYour number of warpoints is: \x1b[1;32m%d\x1b[0;37m.\n\r", ch->pcdata->warpoints);
      send_to_char(buffy, ch);
      send_to_char ("\x1B[37;0m", ch);
      ch->position = position;
      return;
    }*/ 

 if (!str_cmp(arg1, "newbie_killers") && LEVEL(ch) > 100)
    {
      send_to_char("\x1b[0;31mNewbie Killers:\n\n\r",ch);
      i=6;
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  float groupsize;
	  if (pkdata[k][i].value > 0)
	    groupsize = 1 / pkdata[k][i].value;
	  else groupsize = 0;
	  sprintf (buffy, "\x1b[1;3%dm* $B$2%-2d $0%-35s $4-->$1 %2.4f",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, groupsize);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
         sprintf(buffy, "\n\rYour average level of pkill victims is \x1b[1;34m%2.4f\x1b[0;37m.\n\r", (float) ((float)ch->pcdata->pklevels)/((float) (UMAX(ch->pcdata->pkills, 1))));
      send_to_char(buffy, ch);
      ch->position = position;
      return;
    } 

 if (!str_cmp(arg1, "bgpkw") && LEVEL(ch) < 111)
    {
      i=7;
      send_to_char("\x1b[0;31mBig Group Pkill Whores:\n\n\r",ch);
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  sprintf (buffy, "\x1b[1;3%dm* $B$2%-2d $0%-35s $4-->$1 %2.4f",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, pkdata[k][i].value);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
          sprintf(buffy, "\n\rYour average groupsize when you pkill someone is \x1b[1;34m%2.4f\x1b[0;37m.\n\r", (float) ((float)ch->pcdata->pkhelpers)/((float) (UMAX(ch->pcdata->pkills, 1))));
      send_to_char(buffy, ch);
      ch->position = position;
      return;
    } 
/*if (!str_cmp(arg1, "losers"))
    {
      i=10;
      send_to_char("\x1b[0;31mMost time on since they created:\n\n\r",ch);
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  sprintf (buffy, "\x1b[1;3%dm* $B$2%-2d $0%-35s $4-->$1 %2.4f percent",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, pkdata[k][i].value);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
      sprintf(buffy, "\n\rYour percentage of playing time since your creation is:\x1b[1;34m%2.4f\x1b[0;37m.\n\r", (float) ((float)(ch->pcdata->played + current_time - ch->pcdata->logon)*100.0/((float) (current_time - ch->pcdata->created))));
      send_to_char(buffy, ch);
      ch->position = position;
      return;
    } */
if (!str_cmp(arg1, "no_life_dorks") && LEVEL(ch) < 111)
    {
      i=11;
      send_to_char("\x1b[0;31mMost time on since beginning of the game:\n\n\r",ch);
      for ( k=0; k < MAX_PKDATA; k++)
	{
	  sprintf (buffy, "\x1b[1;3%dm* $B$2%-2d $0%-35s $4-->$1 %2.4f percent",
	     (pkdata[k][i].alignment),k + 1,  
            pkdata[k][i].name, pkdata[k][i].value);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
      sprintf(buffy, "\n\rYour percentage of playing time since the game originally started is:\x1b[1;34m%2.4f\x1b[0;37m.\n\r", (float) ((float)(ch->pcdata->played + current_time - ch->pcdata->logon)*100.0/((float) (current_time - startup_time))));
      send_to_char(buffy, ch);
      ch->position = position;
      return;
    }
 send_to_char("Choose one of: pkills, ave, num, pdeaths, dif, total_wps, bgpkw, newbie_killers.\n\r", ch);
 ch->position = position;
return;

}


void 
do_rating (CHAR_DATA * ch, char *argy)
{
  bool foundy;
  char buffy[500];
  int i;
  int position;
  DEFINE_COMMAND ("rating", do_rating, POSITION_SLEEPING, 0, LOG_NORMAL, "Shows you your current rating.")

    if (IS_MOB (ch))
    return;
  if (LEVEL (ch) < 100)
    check_rating (ch);
  if (!str_cmp(argy, "reset") && LEVEL(ch) == MAX_LEVEL)
    {
      send_to_char("Resetting rating.\n\r", ch);
      for (i = 0; i < MAX_RATING; i++)
	{
           strcpy(ratingd.rating_name[i], "Free-Slot");
           ratingd.rating_rating[i] = 0;
           ratingd.rating_alignment[i] = 0;      
         }
      return;
    }


  position = ch->position;
  ch->position = POSITION_STANDING;
  foundy = FALSE;
  act ("$B$0[$7Top Ten Rated Players$0]", ch, NULL, ch, TO_CHAR);
  send_to_char ("\n\r", ch);
  for (i = 0; i < MAX_RATING; i++)
	{
	  sprintf (buffy, "\x1b[1;3%dm* $B$0#%-2d $B$7%-35s $B$0-->$B$7 %-6ld $B$4Rating$R$7",
	    URANGE(0,(ratingd.rating_alignment[i]-1)*7, 7),
     i + 1, ratingd.rating_name[i], ratingd.rating_rating[i]);
	  act (buffy, ch, NULL, ch, TO_CHAR);
	}
  sprintf (buffy, "\n\r\x1B[0;37mYour current rating: \x1B[1;37m%d\x1B[0;37m\n\r", rating (ch));
  send_to_char (buffy, ch);
  ch->position = position;
  return;
}

int 
free_trophy_slot (CHAR_DATA * ch, CHAR_DATA * victim)
{
  int i;
  int templev;
  int tempnum;
  if (IS_MOB (ch))
    return -1;
  for (i = 0; i < MAX_TROPHY; i++)
    {
      if (ch->pcdata->trophy_name[i][0] == '\0')
	return i;
    }
/*Okay, no free slots, replace lowest level */
  templev = 1000;
  tempnum = -1;
  for (i = 0; i < MAX_TROPHY; i++)
    {
      if (ch->pcdata->trophy_level[i] < templev)
	{
	  tempnum = i;
	  templev = ch->pcdata->trophy_level[i];
	}
    }
  if (ch->pcdata->trophy_level[tempnum] <= LEVEL (victim))
    return tempnum;
  else
    return -1;
}

void 
check_add_trophy (CHAR_DATA * ch, CHAR_DATA * victim, int same_align, int 
diff_align)
{
  int i;
  int nummy;
  int wpstogive;
  float warpoints_mult = 1.0;
  float level_mult = 1.0;
  float groupsize_mult = 1.0;
  float remorts_mult = 1.0;
  float same = (float) same_align;
  float diff = (float) diff_align;
  if (IS_MOB (ch) || IS_MOB (victim))
    return;
/* JRAJRA */
  check_pkdata(ch);

  check_pkdata(victim);
  ch->pcdata->online_spot->pk_since_locate++;
level_mult = (float) (((float)LEVEL (victim)))/((float)(LEVEL (ch)));
  if (level_mult > 2)
    level_mult = 2;
  if (level_mult < .2)
    level_mult = .2;
  groupsize_mult = diff/same;
  if (groupsize_mult >= 1.5)
      groupsize_mult = 1.5;
  if (groupsize_mult < .05)
      groupsize_mult = .05;
  remorts_mult=(float)((float)(5+victim->pcdata->remort_times))/((float)(5+ch->pcdata->remort_times));
  if (remorts_mult > 2)
     remorts_mult = 2;
  if (remorts_mult < .3)
     remorts_mult = .3;  


  warpoints_mult = level_mult * groupsize_mult * remorts_mult;
  if (warpoints_mult < .03)
    warpoints_mult = .03;
  if (warpoints_mult > 1.5)
    warpoints_mult = 1.5;
  /* Large Group Discouragement
  if (same_align > diff_align && same_align > 15)
    {
	if (same_align - 2 > diff_align)
        {
	    warpoints_mult *= .1;
        }
	else {
            warpoints_mult *= .05;
	}
  }
  */
  else if (same_align > 6)
  {
	if (same_align <= diff_align)
	{
	   warpoints_mult *= 1.1;
	}
	else if (same_align <= diff_align + 4)
	{
	   warpoints_mult *= 1;
	}
	else if (same_align <= diff_align + 5)
	{
	   warpoints_mult *= .8;
	}
	else if (same_align < diff_align + 7)
	{
	   warpoints_mult *= .7;
	}
	else
	{
	   warpoints_mult *= .5;
        }
  }
  if (same_align == 1)
    warpoints_mult = 1.7;
  if (warpoints_mult < .03)
    warpoints_mult = .03;
  if (warpoints_mult > 2)
    warpoints_mult = 2;
  wpstogive = (int)((warpoints_mult*((float)LEVEL(victim))));
/* This was set by STRUCTURE to allow all players to get 1.5 the amount of 
xp per kill on a person then normal WARPOINTS */
/*  if (IS_SET(ch->pcdata->act2, PLR_HARDCORE)) */
    wpstogive *= 1.5;
  if (victim->pcdata->died_recently > 2)
  {
    wpstogive = 0;
    send_to_char("Your victim is spam dying, you gain 1 warpoint.\n\r", ch);
    send_to_char("YOU ARE SPAM DYING, STOP OR BE FROZEN! NO WARPOINTS AWARDED!\n\r", victim);
    send_to_char("YOU HAVE LOST 50 WARPOINTS!\n\r", victim);
    victim->pcdata->warpoints -=50;
    if (victim->pcdata->warpoints < 0)
      victim->pcdata->warpoints = 0;
  }
  if (wpstogive > 1)
  {
   ch->pcdata->warpoints +=wpstogive;
   ch->pcdata->total_wps +=wpstogive;
  }
  else
  {
    ch->pcdata->total_wps += 1;
    ch->pcdata->warpoints += 1;
  }
  if (LEVEL (victim) > 59 && victim->pcdata->remort_times > 7)
    {
      ch->pcdata->total_wps += 1;
      ch->pcdata->warpoints += 1;
    }
  if (LEVEL(ch) <= 90)
   check_topten (ch); 



  for (i = 0; i < MAX_TROPHY; i++)
    {
      if (ch->pcdata->trophy_name[i][0] == '\0')
	continue;
      if (!str_cmp (RNAME (victim), ch->pcdata->trophy_name[i]))
	{
	  ch->pcdata->trophy_times[i]++;
	  ch->pcdata->trophy_level[i] = LEVEL (victim);
	  sort_trophy(ch, "moo");
	  return;
	}
   }
  nummy = free_trophy_slot (ch, victim);
    if (nummy == -1) return;
  strcpy (ch->pcdata->trophy_name[nummy], RNAME (victim));
  ch->pcdata->trophy_times[nummy] = 1;
  ch->pcdata->trophy_level[nummy] = LEVEL (victim);
  sort_trophy(ch, "moo");
  return;
}


void 
sort_trophy (CHAR_DATA *ch, char *argy)
{
  int i,k;
  int templev;
  int tempnum;
  char tempname[20];
  for (k = 0; k < (MAX_TROPHY - 1); k++)
    {
      if ((ch->pcdata->trophy_level[k] > 0) &&
(ch->pcdata->trophy_times[k] > 0) && (ch->pcdata->trophy_name[k][0] !=
'\0'))
      for (i = (k+1); i < MAX_TROPHY; i++)
	{
          if ((ch->pcdata->trophy_level[k]) <
(ch->pcdata->trophy_level[i]))
	    {
	      templev = ch->pcdata->trophy_level[i];
	      ch->pcdata->trophy_level[i] = ch->pcdata->trophy_level[k];
	      ch->pcdata->trophy_level[k] = templev;
	      tempnum = ch->pcdata->trophy_times[i];
	      ch->pcdata->trophy_times[i] = ch->pcdata->trophy_times[k];
	      ch->pcdata->trophy_times[k] = tempnum;
	      strcpy(tempname, ch->pcdata->trophy_name[i]);
	      strcpy(ch->pcdata->trophy_name[i],
ch->pcdata->trophy_name[k]);
	      strcpy(ch->pcdata->trophy_name[k], tempname);
	    }
	}
    }
  return;
}
