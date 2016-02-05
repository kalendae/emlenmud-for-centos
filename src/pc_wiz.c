/* Taken from some freebie code - doesn't work, isn't used.  blah!!! why is it here?? */


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
#ifdef netBSD
#include <unistd.h>
#endif
#include "emlen.h"
#ifdef Linux
#include "mush.h"
#endif


void //Kilith 2005
do_random_split (CHAR_DATA * ch , char *argy) {
 DESCRIPTOR_DATA *d;
  char arg[SML_LENGTH];
  char buf[5000];
  int i,x;
  ROOM_DATA *troom;
  static ROOM_DATA *first;
  CHAR_DATA *mob,*victim;
  SINGLE_OBJECT *obj;
  DEFINE_COMMAND("dice", do_random_split, POSITION_DEAD, 0, LOG_NORMAL,"This does a random split between people in a room to replace 0 high")
   i=1;
      if (IS_MOB(ch) || !ch->desc)
              return;

      one_argy (argy, arg);
           if (arg[0] == '\0')
    {
      send_to_char ("Dice a split on what?\n\r", ch);
      return;
    }
    troom=ch->in_room;
     if ((obj = get_obj_inv (ch, arg)) == NULL)
            {
              send_to_char ("You can't find it.\n\r", ch);
              return;
            }

       mob=NULL;

       for (mob = troom->more->people; mob != NULL; mob = mob->next_in_room) {
            i++;
            }



         x=number_range(1,i)-1;
    if (x < 1)
         x=1;
         i=1;
       troom=NULL;
       troom=ch->in_room;
       for (mob = troom->more->people; mob != NULL; mob = mob->next_in_room)  {

         if (i !=x) {
              i++;
              continue;
              }
          else
             break;

             }
        if (mob->desc == NULL) {
              do_random_split(ch,argy);
              return;
           }
        if (IS_PLAYER(mob) && LEVEL(mob) > 90 && LEVEL(ch) < 91) {
              do_random_split(ch,argy);
              return;
        }

       sprintf(buf,"\x1B[0;37m... You roll the dice on %s\x1B[0;37m.\n\r",obj->pIndexData->short_descr);
       send_to_char(buf,ch);
       act ("\x1B[1;31m$n \x1B[0;37mrolls the dice on $p\x1B[0;37m.", ch, obj, ch, TO_NOTVICT);
        sprintf(buf,"The winner is:\x1B[1;31m %s\x1B[37;0m. \n\r",
        mob->desc == NULL ?  "Nobody" : RNAME(mob));
        for (d = descriptor_list; d; d = d->next)
    {
      if (d->connected == CON_PLAYING
          && d->character->in_room == ch->in_room)
        {
          send_to_char (buf, d->character);
        }
    }

return;
}


#ifdef WINDOWS

int startShell (CHAR_DATA * ch, char *arg1, char *arg2) {return 0;}
void do_shell (CHAR_DATA * ch, char *argy) {return;}
void do_spico_help (CHAR_DATA * ch, char *argy) {return;}

#else


extern char wont_echo[];
extern char wont_suppress_ga[];
extern char will_echo[];
extern char will_suppress_ga[];

void 
do_spico_help (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("edit_help", do_spico_help, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Remote Help Editing.")
    if (!ch->desc)
    return;
  ch->desc->connected = CON_SHELL;
  startShell (ch, "spico", "help.are");
  return;
}


/* would just be a security hole - scrap this */
void 
do_shell (CHAR_DATA * ch, char *argy)
{
  DEFINE_COMMAND ("shell", do_shell, POSITION_DEAD, MAX_LEVEL, LOG_ALWAYS, "Do not use.")
    if (!ch->desc)
    return;
  return;
}


int open_pty_master (char *);
int open_pty_slave (int, const char *);
int route_io (int, int);

int 
startShell (CHAR_DATA * ch, char *arg1, char *arg2)
{
  DESCRIPTOR_DATA *c;
  int master_fd;
  int slave_fd;
  int temp_fds[2];
  char pty[12];
  int pid;

  if (!ch->desc)
    return -1;

  if (pipe (temp_fds) < 0)
    {
      perror ("pipe");
      return -1;
    }


  ch->desc->fdpair[0] = temp_fds[0];	/* Pipe in */
  fcntl (ch->desc->fdpair[0], F_SETFL, O_NONBLOCK);
  ch->desc->fdpair[1] = temp_fds[1];	/* Pipe out */

  if ((pid = fork ()) > 0)
    {
      close (ch->desc->fdpair[1]);
      fcntl (ch->desc->fdpair[0], F_SETFL, O_NONBLOCK);
      return 0;
    }
  else if (pid < 0)
    {
      close (ch->desc->fdpair[1]);
      close (ch->desc->fdpair[0]);
      return -1;
    }

  close (main_control);
/*::server.close(); ????????? */
  close (ch->desc->fdpair[0]);

  for (c = descriptor_list; c != NULL; c = c->next)
    {
      if (c == ch->desc)
	continue;
      close (c->descriptor);
    }

  if ((int) (master_fd = open_pty_master (pty)) < 0)
    {
      return -1;
    }

  pid = fork ();

  if (pid == 0)
    {
      struct sigaction sa;
      sa.sa_flags = SA_RESETHAND;
      sa.sa_handler = 0;
      if (sigaction (SIGCHLD, &sa, 0) < 0)
	{
	  /*bummer */
	}

      if ((int) (slave_fd = open_pty_slave ((int) master_fd, pty)) < 0)
	{
	  perror ("open_pty_slave:");
	  exit (0);
	}

      close (master_fd);
      close (0);
      close (1);
      close (2);
      if (dup ((int) slave_fd) != 0 || dup ((int) slave_fd) != 1
	  || dup ((int) slave_fd) != 2)
	{
	  exit (0);
	}

      close (slave_fd);
      if (arg1[0] != '\0')
	execlp (arg1, arg1, arg2, (char *) 0);
      else
	execl ("/bin/sh", "sh", (char *) 0);
      exit (0);
    }
  else if (pid < 0)
    {
      return -1;
    }

  fcntl (master_fd, F_SETFL, O_NONBLOCK);

  write (ch->desc->descriptor, will_echo, strlen (will_echo));
  write (ch->desc->descriptor, will_suppress_ga, strlen (will_suppress_ga));


  route_io (ch->desc->descriptor, (int) master_fd);

  close (ch->desc->fdpair[1]);
  exit (0);
  return 0;
}
#endif
