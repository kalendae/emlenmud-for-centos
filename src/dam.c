#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emlen.h"


#ifndef WINDOWS
#include <sys/times.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif  


long mytime(void) {
#ifndef WINDOWS
     struct rusage resources;
     int who = RUSAGE_SELF;   

     int ierr;   

     long utime;   
     long stime;   

     ierr = getrusage (who, &resources);

/* -------------------------------------- */
/* In case of error, return garbage value */
/* -------------------------------------- */

     if (ierr != 0) 
      {
         utime = -1.0;
         stime = -1.0;
         return (utime + stime);
      }

/* ------------------------------------------------ */
/* Otherwise, unpack structure for return of values */
/* ------------------------------------------------ */

     utime = (resources.ru_utime.tv_sec*1000000) +
       resources.ru_utime.tv_usec;

     stime = (resources.ru_stime.tv_sec*1000000) +
       resources.ru_stime.tv_usec;

      return (utime + stime);
#else
return 0;
#endif

}

long info_mem(int i) 
{ /* 1 = shared, 2 = unshared data, 3 = stack */
#ifndef WINDOWS
  struct rusage resources;
  int who = RUSAGE_SELF;   
  int ierr;
  ierr = getrusage (who, &resources);
  fprintf(stderr,"%ld %ld %ld\n",resources.ru_ixrss,resources.ru_idrss,
	  resources.ru_isrss);
  if (i==1) return resources.ru_ixrss;
  if (i==2) return resources.ru_idrss;
  if (i==3) return resources.ru_isrss; 
  else 
#endif
    return 0;
}

#define GENERIC 0
#define ARMS 1
#define HEAD 2
#define BODY 3
#define LEGS 4

int tx,ty;
CHAR_DATA *c;


void NEW_POSITION(CHAR_DATA *ch,int nw) 
{
  ch->position=nw;
  return;
}



void SUBHIT(CHAR_DATA *ch,int i)
{
  if (!ch || ch->data_type==50) 
    {
      return;
    }
ch->hit-=i;
return;
}

void SUBMOVE(CHAR_DATA *ch,int i) 
{
  if (!ch || ch->data_type==50) 
    {
      fprintf(stderr,"NULL character or free'd character in SUBMOVE.\n");
      return;
    }
  ch->move-=i;
  
  return;
}

void ADDHIT(CHAR_DATA *ch,int i) 
{
ch->hit+=i;
return;
}

void ADDMOVE(CHAR_DATA *ch,int i) 
{
  if (!ch) return;
  ch->move+=i;
  
  return;
}

void MAXHIT(CHAR_DATA *ch) 
{
  ch->hit=ch->max_hit;
  
  return;
}

void MAXMOVE(CHAR_DATA *ch) {
  ch->move=ch->max_move;
  
  return;
}



void
read_damages (void)
{
  FILE *df;
  char buf[1000];
  int lastmark= 0;
  bzero (&dmsg, sizeof (dmsg));
  if ((df = fopen ("dam.dat", "r")) == NULL)
    {

      damt.kick_dam = str_dup ("o+2d/4m40+k/2");
      write_damages ();
    }
  for (;;)
    {
      strcpy (buf, fread_word (df));
      if (!str_cmp (buf, "END"))
	break;
      if (!str_cmp (buf, "Kickd"))
	damt.kick_dam = str_dup (fread_word (df));
      if (!str_prefix ("#", buf))
	{
	  fread_to_eol (df);
	  continue;
	}
      if (!str_cmp (buf, "GENERIC"))
	{
	  dmsg.generic_range[lastmark][0] = fread_number (df);
	  dmsg.generic_range[lastmark][1] = fread_number (df);
	  dmsg.generic_percent[lastmark][0] = fread_number (df);
	  dmsg.generic_percent[lastmark][1] = fread_number (df);
	  fread_to_eol (df);
	  dmsg.gen_m[lastmark] = fread_string_eol (df);
	  dmsg.gen_m1[lastmark] = fread_string_eol (df);
	  dmsg.gen_m2[lastmark]= fread_string_eol (df);
	  lastmark++;
	}
    }
  fclose (df);
  return;
}

char *
star_a (int dam, int maxhit)
{
  static char rs[100];
  int percent = (dam * 100) / maxhit;
  rs[0] = '\0';
  if (percent < 1)
    strcpy (rs, "barely scratches");
  else if (percent < 2)
    strcpy (rs, "lightly scratches");
  else if (percent < 3)
    strcpy (rs, "scratches");
  else if (percent < 5)
    strcpy (rs, "mildly wounds");
  else if (percent < 8)
    strcpy (rs, "wounds");
  else if (percent < 11)
    strcpy (rs, "badly wounds");
  else if (percent < 15)
    strcpy (rs, "causes major injury to");
  else if (percent < 20)
    strcpy (rs, "mangles");
  else if (percent < 35)
    strcpy (rs, "devastates");
  else if (percent < 70)
    strcpy (rs, "PULVERIZES");
  else
    strcpy (rs, "OBLITERATES");
  return rs;
}

char *
star_b (int dam, int maxhit)
{
  static char rst[100];
  int percent = (dam * 100) / maxhit;
  rst[0] = '\0';
  if (percent < 1)
    strcpy (rst, "barely scratching");
  else if (percent < 2)
    strcpy (rst, "lightly scratching");
  else if (percent < 3)
    strcpy (rst, "scratching");
  else if (percent < 5)
    strcpy (rst, "mildly wounding");
  else if (percent < 8)
    strcpy (rst, "wounding");
  else if (percent < 11)
    strcpy (rst, "badly wounding");
  else if (percent < 15)
    strcpy (rst, "causing major injury to");
  else if (percent < 20)
    strcpy (rst, "mangling");
  else if (percent < 35)
    strcpy (rst, "devastating");
  else if (percent < 70)
    strcpy (rst, "PULVERIZING");
  else
    strcpy (rst, "OBLITERATING");
  return rst;
}


void
display_dam_stuff (CHAR_DATA * ch)
{
  char buff[500];
  sprintf (buff, "Kick_dam....: %s --> %ld %ld %ld %ld\n", damt.kick_dam,
	   translate (damt.kick_dam,
		      LEVEL (ch), ch),
	   translate (damt.kick_dam,
		      LEVEL (ch), ch),
	   translate (damt.kick_dam,
		      LEVEL (ch), ch),
	   translate (damt.kick_dam,
		      LEVEL (ch), ch));
  send_to_char (buff, ch);
  return;
}

void
do_dam (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("damage", do_dam, POSITION_DEAD, MAX_LEVEL, LOG_NORMAL, "Checks kick damage.")
    display_dam_stuff (ch);
  return;
}
