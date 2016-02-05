/* Change press enter to continue for java to place enter after. */
/* Some code taken from two other codebases - functions taken not in use */
#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "emlen.h"
#include "mush.h"
#ifdef WINDOWS
#include <sys\types.h>
#define FD_SETSIZE 1024
#include <winsock.h>
typedef SOCKET socket_t;
#define CLOSESOCKET(sock) while (closesocket(sock) < 0) ;




#include <direct.h>
// #include <mmsystem.h>
#include "telnet.h"
WSADATA wsaData;

#else  /* We're on unix if below */

#define CLOSESOCKET(sock) close(sock)

#include <sys/types.h>
#ifdef Linux
#include <sys/time.h>
#endif
#include <ctype.h>
#include <errno.h>
#ifdef netBSD
#include <unistd.h>
#endif

/*#ifdef Linux
#include "mush.h"
#endif*/

#endif



#define ASK_MAP 16097
DESCRIPTOR_DATA *descriptor_free = NULL;	/* Free list for descriptors */
DESCRIPTOR_DATA *descriptor_list = NULL;	/* All open descriptors */
DESCRIPTOR_DATA *d_next;	/* Next descriptor in loop */
FILE *plrchecker;
bool aturion_down;		/* Shutdown */
bool wizlock;			/* Game is wizlocked */
char str_boot_time[SML_LENGTH];
time_t current_time;		/* Time of this pulse */
                        /*
                        * OS-dependent local functions.
*/
void game_loop_unix (socket_t control, int control2);
socket_t init_socket (int port);
void new_descriptor (socket_t, bool);
bool read_from_descriptor (DESCRIPTOR_DATA * d);
bool write_to_descriptor (socket_t desc, char *txt, int length);
bool write_to_descriptor2 (DESCRIPTOR_DATA * d, char *txt, int length);
/*
* Other local functions (OS-independent).
*/
bool gr = TRUE;
bool check_parse_name (char *name, bool space_allowed);
bool check_reconnect (DESCRIPTOR_DATA * d, char *name, bool fConn);
bool check_playing (DESCRIPTOR_DATA * d, char *name);
int main (int argc, char **argv);
void handle_connecting (DESCRIPTOR_DATA * d, char *argy);
bool process_output (DESCRIPTOR_DATA * d, bool fPrompt);
void read_from_buffer (DESCRIPTOR_DATA * d);
void stop_idling (CHAR_DATA * ch);
void print_prompt (CHAR_DATA * ch);
char *anseval (const char *txt, CHAR_DATA * to);
static int area_this_reboot;
bool is_equipwipe = FALSE;

char commandlog[3000];
char wont_echo[] =
{
  IAC, WONT, TELOPT_ECHO, '\0'
};


char will_echo[] =
{
  IAC, WILL, TELOPT_ECHO, '\0'
};

char wont_suppress_ga[] =
{
  IAC, WILL, TELOPT_SGA, '\0'
};

char will_suppress_ga[] =
{
  IAC, WILL, TELOPT_SGA, '\0'
};


char dontecho[] =
{
  IAC, DONT, TELOPT_ECHO, '\0'
};

char doecho[] =
{
  IAC, DO, TELOPT_ECHO, '\0'
};

char echo_off_str[] =
{
  IAC, WILL, TELOPT_ECHO, '\0'
};

char echo_on_str[] =
{
  IAC, WONT, TELOPT_ECHO, '\0'
};

char go_ahead_str[] =
{
  IAC, GA, '\0'
};

char linemode_off[] =
{
  IAC, WILL, TELOPT_SGA, '\0'
};

char linemode_on[] =
{
  IAC, WONT, TELOPT_SGA, '\0'
};


/* Windows doesn't have gettimeofday, so we'll simulate it. */
#ifdef WINDOWS

void gettimeofday(struct timeval *t, struct timezone *dummy)
{
  int millisec = GetTickCount();
  
  t->tv_sec = (int) (millisec / 1000);
  t->tv_usec = (millisec % 1000) * 1000;
}


#endif


char * add_color (char *txt) {
  int len = 0;
  char *t;
  char *r;
  char cod;
  char *ret;
  for (t = txt; *t != '\0'; t++) {
    if (*t == '{') {
      len += 6;
      continue;
    }
    len++;
  }
  ret = malloc (len + 1);
  r = ret;
  for (t = txt; *t != '\0'; t++) {
    if (*t == '{') {
      if (*(t + 1) != '\0' && *(t + 2) != '\0' && *(t + 2) == '}') {
        t++;
        cod = *t;
        t++;
        switch (UPPER(cod)) {
        case 'B':
          *r++ = '\x1B';
          *r++ = '[';
          if (cod == UPPER(cod))
            *r++ = '1';
          else
            *r++ = '0';
          *r++ = ';';
          *r++ = '3';
          *r++ = '4';
          *r++ = 'm';
          break;
        case 'G':
				      *r++ = '\x1B';
              *r++ = '[';
              if (cod == UPPER(cod))
                *r++ = '1';
              else
                *r++ = '0';
              *r++ = ';';
              *r++ = '3';
              *r++ = '2';
              *r++ = 'm';
              break;
		      case 'Y':
            *r++ = '\x1B';
            *r++ = '[';
            if (cod == UPPER(cod))
              *r++ = '1';
            else
              *r++ = '0';
            *r++ = ';';
            *r++ = '3';
            *r++ = '3';
            *r++ = 'm';
            break;
          case 'R':
            *r++ = '\x1B';
            *r++ = '[';
            if (cod == UPPER(cod))
              *r++ = '1';
            else
              *r++ = '0';
            *r++ = ';';
            *r++ = '3';
            *r++ = '1';
            *r++ = 'm';
            break;
          case 'P':
            *r++ = '\x1B';
            *r++ = '[';
            if (cod == UPPER(cod))
              *r++ = '1';
            else
              *r++ = '0';
            *r++ = ';';
            *r++ = '3';
            *r++ = '5';
            *r++ = 'm';
            break;
          case 'C':
            *r++ = '\x1B';
            *r++ = '[';
            if (cod == UPPER(cod))
              *r++ = '1';
            else
              *r++ = '0';
            *r++ = ';';
            *r++ = '3';
            *r++ = '6';
            *r++ = 'm';
            break;
          case 'W':
            *r++ = '\x1B';
            *r++ = '[';
            if (cod == UPPER(cod))
              *r++ = '1';
            else
              *r++ = '0';
            *r++ = ';';
            *r++ = '3';
            *r++ = '7';
            *r++ = 'm';
            break;
          case 'D':
            *r++ = '\x1B';
            *r++ = '[';
            *r++ = '1';
            *r++ = ';';
            *r++ = '3';
            *r++ = '0';
            *r++ = 'm';
            break;
          case 'N':
            *r++ = '\x1B';
            *r++ = '[';
            *r++ = '0';
            *r++ = ';';
            *r++ = '3';
            *r++ = '7';
            *r++ = 'm';
            break;
          }
          continue;
      }
  }
  *r = *t;
  r++;
   }
   *r = '\0';
   return ret;
}



char *
add_color_limited (char *txt)
{
  int len = 0;
  char *t;
  char *r;
  char *ret;
  for (t = txt; *t != '\0'; t++)
  {
    len++;
  }
  ret = malloc (len + 1);
  r = ret;
  for (t = txt; *t != '\0'; t++)
  {
    *r = *t;
    r++;
  }
  *r = '\0';
  return ret;
}



FILE *ffg;

int monthly_key;


void save_monthly_key(void) {
  FILE *fp;
  if ( (fp=fopen("mkey.dat","w+")) == NULL ) { /* Bad! */ return;}
  fprintf(fp,"%d\n",monthly_key);
  fclose(fp);
  return;
}



void read_monthly_key(void) {
  FILE *fp;
  if ( (fp=fopen("mkey.dat","r")) == NULL ) {
    monthly_key=0;
    save_monthly_key();
    return;
  }
  
  monthly_key=fread_number(fp);
  fclose(fp);
  return;
}
void set_monthly_key(CHAR_DATA *ch, char *argy) {
  char arg1[500];
  DEFINE_COMMAND("z_monthly",set_monthly_key,POSITION_DEAD,MAX_LEVEL,LOG_ALWAYS,"Sets the monthly key.. private.")
    argy=one_argy(argy,arg1);
  if (arg1[0]=='\0' || !is_number(arg1)) {
    sprintf(arg1,"Monthly key now is %d.\n\r",monthly_key);
    send_to_char(arg1,ch);
    return;
  }
  monthly_key=atoi(arg1);
  send_to_char("Monthly key set.\n\r",ch);
  save_monthly_key();
  return;
}

socket_t control;

int
main (int argc, char **argv)

{
  struct timeval now_time;
  int port;
  
  /* We can do certain cleanup things to characters if we wish... */
  
  bool uncrypt;
  bool is_cleanup;
  bool is_skillwipe;
  
  /* Just set up the time of day for bootup. */
  
  gettimeofday (&now_time, NULL);
  current_time = (time_t) now_time.tv_sec;
  strcpy (str_boot_time, ctime (&current_time));
  
  
  area_this_reboot = 0;
  uncrypt = FALSE;
  is_cleanup = FALSE;
  is_skillwipe = FALSE;
  is_equipwipe = FALSE;
  port = 3335;
  
  /* If we pick a port number OR a skillwipe or equipment wipe */
  
  if (argc > 1)
  {
    if (!is_number (argv[1]))
    {
      fprintf (stderr, "%s\n", argv[1]);
      if (!str_cmp (argv[1], "Equipmentwipe"))
        is_equipwipe = TRUE;
      else if (!str_cmp (argv[1], "Skillwipe"))
        is_skillwipe = TRUE;
      else if (!str_cmp (argv[1], "Uncrypt"))
        uncrypt = TRUE;
      else
      {
        fprintf (stderr, "Usage: %s [port #]\n", argv[0]);
        exit (1);
      }
    }
    else
    {
      if (refresh_oldchars)
        port = 3336;
      else if (is_equipwipe)
        port = 3336;
      else if ((port = atoi (argv[1])) <= 1)
      {
        fprintf (stderr, "Invalid (%d) Port number must be above 1.\n", port);
        exit (1);
      }
    }
  }
  
  if (port > 10000)
  {
    port -= 10000;
    builder_port = TRUE;
  }
  /* This loads up all the areas with their internal mobs 
  and rooms and objs and all that, as well as a whole 
  bunch of other data...check out db2.c for more on this. */
  
  boot_db (); 
  control = init_socket (port);
  fBootDb = FALSE;
  
  
  
  /* These three choices are for uncrypt, skillwipe, eqwipe...they let you
  fix things with the chars off line. */
  
  
  if (uncrypt)
  {
    PLAYERBASE_DATA *playerbase;
    char buffy[500];
    fprintf (stderr, "Uncrypting all player files...\n");
    sprintf (buffy, "cd %s", PLAYER_DIR_2);
    system (buffy);
    for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
    {
      unjumble (capitalize (playerbase->player_name), TRUE);
    }
    exit (1);
  }
  if (is_equipwipe)
  {
    PLAYERBASE_DATA *playerbase;
    CHAR_DATA *ch;
    SINGLE_OBJECT *obj;
    int i;
    SINGLE_OBJECT *obj_next;
    char buffy[500];
    fprintf (stderr, "Removing equipment from all player files...\n");
    sprintf (buffy, "cd %s", PLAYER_DIR_2);
    system (buffy);
    for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
    {
      if (!load_char_obj (NULL, capitalize (playerbase->player_name), TRUE))
      {
        fprintf (stderr, "Couldn't find player '%s'... oh well.\n", playerbase->player_name);
        continue;
      }
      ch = pedit_found;
      for (obj = ch->carrying; obj != NULL; obj = obj_next)
      {
        obj_next = obj->next_content;
        obj_from (obj);
      }
      ch->carrying = NULL;
      for (i = 0; i < MAXST; i++)
      {
        ch->pcdata->storage[i] = NULL;
      }
      save_char_obj (ch);
      fprintf (stderr, "Nuked all equipment on '%s'...\n", 
playerbase->player_name);
    }
    exit (1);
  }
  if (is_skillwipe)
  {
    PLAYERBASE_DATA *playerbase;
    CHAR_DATA *ch;
    int i;
    char buffy[500];
    fprintf (stderr, "Removing all skills/spells from all player files...\n");
    sprintf (buffy, "cd %s", PLAYER_DIR_2);
    system (buffy);
    for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
    {
      if (!load_char_obj (NULL, capitalize (playerbase->player_name), TRUE))
      {
        fprintf (stderr, "Couldn't find player '%s'... oh well.\n", playerbase->player_name);
        continue;
      }
      ch = pedit_found;
      for (i = 0; i < SKILL_COUNT; i++)
        ch->pcdata->learned[i] = -100;
      save_char_obj (ch);
      fprintf (stderr, "Nuked all skills/spells on '%s'...\n", playerbase->player_name);
    }
    exit (1);
  }
  
  
  /* Now we are done and the game is set up and we begin game_loop_unix... */
  
  sprintf (log_buf, "LoC is now accepting connections on port %d.", port);
  log_string (log_buf);
  
  /* Main game process it constantly polls, grabs input, and makes the game 
  do stuff and then outputs it. */
  
  game_loop_unix (control, 0);
  
  /* If the game is rebooted normally, we get this..rare occurence ;) */
  
  CLOSESOCKET(control);
  log_string ("Normal termination of game.");
  exit (0);
  return 0;
}


#ifdef WINDOWS

void nonblock(socket_t s)
{
  long val;
  
  val = 1;
  ioctlsocket(s, FIONBIO, &val);
  return;
}

#else

#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

void nonblock(socket_t s)
{
  int flags;
  
  flags = fcntl(s, F_GETFL, 0);
  flags |= O_NONBLOCK;
  if (fcntl(s, F_SETFL, flags) < 0) {
    fprintf(stderr,"Fatal error executing nonblock (comm.c)");
    exit(1);
  }
}

#endif

socket_t
init_socket (int port)
{
  static struct sockaddr_in sa_zero;
  struct sockaddr_in sa;
  int x = 1;
  socket_t fd;
  
#ifdef WINDOWS
  {
    short wVersionRequested;
    
    
    wVersionRequested = MAKEWORD(1, 1);
    
    if (WSAStartup(wVersionRequested, &wsaData) != 0) {
      log("WinSock not available!\n");
      exit(1);
    }
    
    if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
      fprintf(stderr, "Error opening network connection: Winsock err #%d\n", WSAGetLastError());
      exit(1);
    }
  }
#else
  if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror ("Init_socket: socket");
    exit (1);
  }  
#endif		
  
#if defined(SO_REUSEADDR)
  if (setsockopt (fd, SOL_SOCKET, SO_REUSEADDR,
		  (char *) &x, sizeof (x)) < 0)
  {
    perror ("Init_socket: SO_REUSEADDR");
    CLOSESOCKET(fd);
    exit (1);
  }
#endif
  
#if defined(SO_DONTLINGER) && !defined(SYSV)
  {
    struct linger ld;
    ld.l_onoff = 1;
    ld.l_linger = 1000;
    if (setsockopt (fd, SOL_SOCKET, SO_LINGER,
		    (char *) &ld, sizeof (ld)) < 0)
    {
      perror ("Init_socket: SO_DONTLINGER");
      CLOSESOCKET(fd);
      exit (1);
    }
  }
#endif
  sa = sa_zero;
  sa.sin_family = AF_INET;
  sa.sin_port = htons (port);
  if (bind (fd, (struct sockaddr *) &sa, sizeof (sa)) < 0)
  {
    perror ("Init_socket: bind");
    CLOSESOCKET(fd);
    exit (1);
  }
  
  nonblock(fd);
  
  if (listen (fd, 3) < 0)
  {
    perror ("Init_socket: listen");
    CLOSESOCKET(fd); /* close? */
    exit (1);
  }
  return fd;
}

static fd_set in_set;
static fd_set out_set;
static fd_set exc_set;
socket_t main_control;

void
game_loop_unix (socket_t control, int control2)
{
  static struct timeval null_time;
  struct timeval last_time;
  
#ifndef WINDOWS
  signal (SIGPIPE, SIG_IGN);
#endif
  
  /* Get the game time when it set up */
  
  
  gettimeofday (&last_time, NULL);
  main_control = control;
  current_time = (time_t) last_time.tv_sec;
  
  null_time.tv_sec = 0;
  null_time.tv_usec = 400;
  
  
  while (!aturion_down)
  {
    
    /* Poll Descriptors */
    
    DESCRIPTOR_DATA *d;
    static socket_t maxdesc;
    
    FD_ZERO (&in_set);
    FD_ZERO (&out_set);
    FD_ZERO (&exc_set);
    FD_SET (control, &in_set);
    
    maxdesc = control;
    
    for (d = descriptor_list; d; d = d->next)
    {
      maxdesc = UMAX (maxdesc, d->descriptor);
      maxdesc = UMAX (maxdesc, d->auth_fd);
      FD_CLR (d->descriptor, &in_set);
      FD_SET (d->descriptor, &in_set);
      FD_SET (d->descriptor, &out_set);
      FD_SET (d->descriptor, &exc_set);
    }
    
    null_time.tv_usec = 100;
    
    if (select (maxdesc + 10, &in_set, &out_set, &exc_set, &null_time) < 0)
    {
      perror ("Game_loop: select: poll");
      exit (1);
    }
    if (FD_ISSET (control, &in_set))
      new_descriptor (control, FALSE);
    
    /* Clean Descriptors */
    
    for (d = descriptor_list; d != NULL; d = d_next)
    {
      d_next = d->next;
      
      /* This is another way to get kicked out...idling */
      //To stop spam connectors that can not close Zmud KILITH 05
      /*if (d->timeout_counter < 2 && d->timeout_counter > -50)
      {
        d->outtop = 0;
        write_to_descriptor2 (d, "\n\r[Connection Timed Out]\n\r", 0);
        close_socket (d);
        break;
      }*/
      
      /* Clean descriptors */
      
      
      if (FD_ISSET (d->descriptor, &exc_set))
      {
        FD_CLR (d->descriptor, &in_set);
        FD_CLR (d->descriptor, &out_set);
        if (d->character && d->connected <= 0 &&
          !(d->character->pcdata->stat[STR] == 12 &&
          d->character->pcdata->stat[INT] == 12 &&
          d->character->pcdata->stat[WIS] == 12 &&
          d->character->pcdata->stat[CON] == 12 &&
          d->character->pcdata->stat[DEX] == 12))
          save_char_obj (d->character);
        d->outtop = 0;
        close_socket (d);
        continue;
      }
      
#ifndef WINDOWS
#ifndef NO_AUTH
      if ((d->auth_fd == -1) && (d->atimes < 500) && !str_cmp (d->username, "unknown-"))
      {
        start_auth (d);
        continue;
      }
      if (d->auth_fd == -1)
        continue;
      if (IS_SET (d->auth_state, WRITE_AUTH))
      {
        send_auth (d);
        continue;
      }
      read_auth (d);
#endif
#endif
    }
    
    /* Grab Input */
    
    
    for (d = descriptor_list; d != NULL; d = d_next)
    {
      d_next = d->next;
      REMOVE_BIT(d->act, DESC_TPCOMMAND);
      
      if (d->character)
      {
        CHAR_DATA *ch = d->character;
        if (IS_PLAYER(ch))
        {
          if (FIGHTING (ch) == NULL && ch->position == POSITION_FIGHTING)
            NEW_POSITION(ch, POSITION_STANDING);
          
          
          /* Decrement tickcounts (spell casting time) and wait (lag time) */         
          /* Gods do not have to deal with waiting or spellcasting time if they choose */
          
          if (LEVEL(d->character) > 99 && !IS_SET(d->character->pcdata->act2, PLR_HOLYLAG)) {
            ch->pcdata->tickcounts = 0;
            ch->wait = 0;
            ch->pcdata->speedwalk = 0;
            ch->pcdata->auction_count = 0;
          } else {
            
          /* Note that if tickcounts > 0 you can continue to do stuff, you just stop
             casting and if you have a wait state > 0, then you do not get
             anymore input. */
            
            if (--ch->pcdata->tickcounts <= 0)
              ch->pcdata->tickcounts = 0;
            if (--ch->pcdata->auction_count <= 0)
              ch->pcdata->auction_count = 0;
            if (--ch->pcdata->speedwalk <= 0)
              ch->pcdata->speedwalk = 0;
            if (--ch->wait <= 0)
              ch->wait = 0;
            else
              continue;
          }
          
          /* If your tickcounts are > 0 you do not cast or circle or backstab. Only
          if they go to 0 and you are doing the proper thing do you get to cast
          or circle or backstab etc... */
          
          
          if (ch->in_room && ch->pcdata->tickcounts < 1)
          {            
            if (ch->position == POSITION_CASTING && ch->pcdata->temp)
            {
              ch->wait = 0;
              do_actual_cast (ch, ch->pcdata->temp->temp_string);
              NEW_POSITION(ch, POSITION_STANDING);
              free_temp (ch);
            } /*
            else if (ch->position == POSITION_CHARGING && ch->pcdata->temp)
            {
              ch->wait = 0;
              do_actual_charge (ch, ch->pcdata->temp->temp_string);
              NEW_POSITION(ch, POSITION_FIGHTING);
              free_temp (ch); 
            } */
            else if (ch->position == POSITION_CIRCLE)
            {
              do_actual_circle (ch, "Noarg", FALSE);
              free_temp (ch);
            }
            else if (ch->position == POSITION_INVEST && ch->pcdata->temp)
            {
              do_actual_investigate (ch, ch->pcdata->temp->temp_string);
              NEW_POSITION(ch, POSITION_STANDING);
              free_temp (ch);
            }
            else if (ch->position == POSITION_BACKSTAB && ch->pcdata->temp)
            {
              do_actual_backstab (ch, ch->pcdata->temp->temp_string);
              ch->wait = 8;
              NEW_POSITION(ch,POSITION_FIGHTING);
              free_temp (ch);
            }
            else if (ch->position == POSITION_SEARCHING && ch->pcdata->temp)
            {
              do_actual_search (ch, ch->pcdata->temp->temp_string);
              NEW_POSITION(ch, POSITION_STANDING);
              free_temp (ch);
            }
            else if (ch->position == POSITION_SHOOTING && ch->pcdata->temp)
            {
              do_actual_shoot (ch, ch->pcdata->temp->temp_string);
              NEW_POSITION(ch, POSITION_STANDING);
              free_temp (ch);
            }
            else if (ch->position == POSITION_MINING)
            {
              do_actual_mine (ch, "moo");
              NEW_POSITION(ch, POSITION_STANDING);
            }
            if (ch->pcdata->voting_on == 999)
            {
              do_track_t (ch, "continue");
              ch->pcdata->voting_on = 10;
            }
            if (ch->pcdata->voting_on == 998)
            {
              do_ptrack_t (ch, "continue");
              ch->pcdata->voting_on = 11;
            }
            
          }
          if (FIGHTING (ch) == NULL && ch->position == POSITION_FIGHTING)
            NEW_POSITION(ch, POSITION_STANDING);
        }
      }
      
      /* End of d->character, now check for people who are losing link. */
      
      if (FD_ISSET (d->descriptor, &in_set))
      {
        if (d->timeout_counter > 0)
          d->timeout_counter--;
        if (d->character != NULL)
          d->character->timer = 0;
        if (!read_from_descriptor (d))
        {
          FD_CLR (d->descriptor, &out_set);
          if (d->character != NULL && d->connected <= 0 &&
            !(d->character->pcdata->stat[STR] == 12 &&
            d->character->pcdata->stat[INT] == 12 &&
            d->character->pcdata->stat[WIS] == 12 &&
            d->character->pcdata->stat[CON] == 12 &&
            d->character->pcdata->stat[DEX] == 12))
            save_char_obj (d->character);
          if (d->character && d->character->in_room)
          {
            act ("$n has lost link.", d->character, NULL, NULL, TO_CHAR);
          }
          d->outtop = 0;
          close_socket (d);
          continue;
        }
      }
      
      /* Ok grab the input from the file buffer. */
      
      read_from_buffer (d);
      
      /* Now we DO something if we have read something into d->incomm
      from the file buffer. If not, we do nothing. */
      
      if (d->incomm[0] != '\0')
      {
        CHAR_DATA *ch = d->character;
        SET_BIT(d->act, DESC_TPCOMMAND);
        stop_idling (ch);
        
        /* If we are casting or something and we type a command, we stop casting. */
        
        if (ch && ch->pcdata && ch->pcdata->tickcounts > 0)
        {
          switch (ch->position)
          {
          case POSITION_INVEST:
            send_to_char ("You stop your investigation.\n\r", ch);
            ch->pcdata->tickcounts = 0;
            NEW_POSITION(ch, POSITION_STANDING);
            break;
          case POSITION_SEARCHING:
            send_to_char ("You stop your search.\n\r", ch);
            ch->pcdata->tickcounts = 0;
            NEW_POSITION(ch, POSITION_STANDING);
            break;
          case POSITION_CASTING:
            send_to_char ("You stop casting.\n\r", ch);
            ch->pcdata->tickcounts = 0;
            NEW_POSITION(ch, POSITION_STANDING);
            break;
          case POSITION_CHARGING:
            send_to_char ("You stop your charge.\n\r", ch);
            ch->pcdata->tickcounts = 0;
            NEW_POSITION(ch, POSITION_STANDING);
            break;
          case POSITION_CIRCLE:
            send_to_char ("You stop circling.\n\r", ch);
            ch->pcdata->tickcounts = 0;
            NEW_POSITION(ch, POSITION_FIGHTING);
            break;
          case POSITION_BACKSTAB:
            send_to_char ("You stop your approach.\n\r", ch);
            ch->pcdata->tickcounts = 0;
            NEW_POSITION(ch, POSITION_STANDING);
            break;
          case POSITION_SHOOTING:
            send_to_char("You lower your weapon and stop aiming.\n\r", ch);
            NEW_POSITION(ch, POSITION_STANDING);
            break;
          case POSITION_MINING:
            send_to_char("You stop hacking away at the rock.\n\r", ch);
            NEW_POSITION(ch, POSITION_STANDING);
            break; 
          }
        }
        
        /*End if user was doing an action */
        /* This is to keep track of who is doing a command... */
        
        /* Now we actually have the player DO something based on what was entered 
        and what CON_STATE they are in. Basically it comes down to people either 
        writing notes which are the first two things, or they are building which
        are what all these CON_*EDITOR things are, and then if they are not
        doing any of that, and they are playing, they get their command interpreted,
        and if not that, they are logging in and we do handle_connecting. */
        
        if (d->character != NULL && IS_PLAYER(d->character))
        {
          SET_BIT(d->character->act, ACT_CRASHER);
          sprintf(commandlog, "%s", d->incomm);
        }
        
        if (d->pString != NULL)
          string_add (ch, d->incomm);
        else if (d->showstr_point)
          show_string (d, d->incomm);
        else 
        {
          switch (d->connected)
          {
          case CON_SOCEDIT:
            soc_edit (ch, d->incomm);
            break;
          case CON_PLAYING:
            interpret (ch, d->incomm);
            break;
            
            /* This command interpret() is really where almost all of the 
            code passes through. Basically, all of the do_ functions are
            referenced from here. */
            
          case CON_AEDITOR:
            aedit (ch, d->incomm);
            break;
          case CON_SEDITOR:
            sedit (ch, d->incomm);
            break;
          case CON_SCRIPTEDITOR:
            scriptedit (ch, d->incomm);
            break;
          case CON_PEDITOR:
            pedit (ch, d->incomm);
            break;
          case CON_REDITOR:	/*For Builders */
            redit (ch, d->incomm);
            break;
          case CON_OEDITOR:
            oedit (ch, d->incomm);
            break;
          case CON_MEDITOR:
            medit (ch, d->incomm);
            break;
          case CON_CEDITOR:
            cedit (ch, d->incomm);
            break;
          default:
            
            handle_connecting (d, d->incomm);
            
            break;
          }
        }
        if (d->character != NULL && IS_PLAYER(d->character) )
        {
          REMOVE_BIT(d->character->act, ACT_CRASHER);
        }
        
        d->incomm[0] = '\0';
      }
      
  }
  
  times_through[INTERPRETTED]++;
  
  /* This is all the resets and "ticks" associated with the world... upd.c */
  
  update_handler ();
  
  times_through[ALL_UPD]++;
  
  /* Now we check for idlers linkdeads and things of that nature... */
  
  for (d = descriptor_list; d != NULL; d = d_next)
  {
    if (!d->next)
      d_next = NULL;
    else
      d_next = d->next;
    if (!d->descriptor)
      continue;
    if (d->timeout_counter > -3)
      d->timeout_counter--;
    if ((IS_SET(d->act, DESC_TPCOMMAND) || d->outtop > 0)
      && FD_ISSET (d->descriptor, &out_set))
    {
      if (!process_output (d, TRUE))
      {
        if (d->character != NULL && d->connected ==CON_PLAYING &&
          !(d->character->pcdata->stat[STR] == 12 &&
          d->character->pcdata->stat[INT] == 12 &&
          d->character->pcdata->stat[WIS] == 12 &&
          d->character->pcdata->stat[CON] == 12 &&
          d->character->pcdata->stat[DEX] == 12))
          save_char_obj (d->character);
        d->outtop = 0;
        close_socket (d);
        continue;
      }
    }
  }
  
  
  /* Now just wait for the next "pulse" to show up and let the machine dawdle */
  
  {
    struct timeval now_time;
    long secDelta;
    long usecDelta;
    gettimeofday (&now_time, NULL);
    usecDelta = ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
      + 1000000 / (PULSE_PER_SECOND);
    secDelta = ((int) last_time.tv_sec) - ((int) now_time.tv_sec);
    while (usecDelta < 0)
    {
      usecDelta += 1000000;
      secDelta -= 1;
    }
    while (usecDelta >= 1000000)
    {
      usecDelta -= 1000000;
      secDelta += 1;
    }
    if (secDelta > 0 || (secDelta == 0 && usecDelta > 0))
    {
      struct timeval stall_time;
      stall_time.tv_usec = usecDelta;
      stall_time.tv_sec = secDelta;
#ifdef WINDOWS
      Sleep(stall_time.tv_sec * 1000 + stall_time.tv_usec / 1000);
#else
      if (select (0, NULL, NULL, NULL, &stall_time) < 0)
      {
        perror ("Game_loop: select: stall");
        exit (1);
      }
#endif
    }
  }
  gettimeofday (&last_time, NULL);
  current_time = (time_t) last_time.tv_sec;
    }
    return;
}

DESCRIPTOR_DATA *
new_sdesc (socket_t desc)
{
  int i;
  DESCRIPTOR_DATA *dnew;
#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif
  
  nonblock(desc);
  if (descriptor_free == NULL)
  {
    dnew = mem_alloc (sizeof (*dnew));
  }
  else
  {
    dnew = descriptor_free;
    descriptor_free = descriptor_free->next;
  }
  i = sizeof (*dnew);
  bzero (dnew, i);
  dnew->descriptor = desc;
  dnew->connected = CON_CHANNELD;
  dnew->character = NULL;
  dnew->showstr_head = NULL;
  dnew->showstr_point = NULL;
  dnew->abuf[0] = '\0';
  dnew->hostname = &str_empty[0];
  dnew->atimes = 0;
  dnew->auth_fd = -1;
  dnew->auth_state = 0;
  dnew->auth_inc = 0;
  dnew->username = str_dup ("unknown");
  dnew->timeout_counter = 375;
  dnew->outsize = 2000;
  dnew->pEdit = NULL;
  dnew->snoop_by = NULL;
  dnew->host[0] = '\0';
  dnew->original = NULL;
  dnew->hosttwo[0] = '\0';
  dnew->inbuf[0] = '\0';
  dnew->incomm[0] = '\0';
  dnew->inlast[0] = '\0';
  dnew->repeat = 0;
  dnew->pEdit = NULL;
  dnew->outtop = 0;
  dnew->location = 0;
  dnew->max_outbuf = 1000;
  dnew->pString = NULL;
  dnew->outbuf = mem_alloc (1001);
  dnew->outbuf[0] = '\0';
  dnew->next = descriptor_list;
  descriptor_list = dnew;
  return dnew;
}

void
new_descriptor (socket_t control, bool second_port)
{
  static DESCRIPTOR_DATA d_zero;
  char buf[STD_LENGTH];
  DESCRIPTOR_DATA *dnew;
  int jj = 0;
  BAN_DATA *pban;
  int i;
  char buf2[400];
  DESCRIPTOR_DATA *dd;
  struct sockaddr_in sock;
  static socket_t desc;
  int size;
  MARK_DEBUG("New Descriptor")
    size = sizeof (sock);
  getsockname (control, (struct sockaddr *) &sock, &size);
  if ((desc = accept (control, (struct sockaddr *) &sock, &size)) < 0)
  {
    perror ("New_descriptor: accept");
    return;
  }
#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif
  
  nonblock(desc);
  
  /*
  if (fcntl (desc, F_SETFL, FNDELAY) == -1)
  {
  perror ("New_descriptor: fcntl: FNDELAY");
  return;
  }
  */
  
  if (descriptor_free == NULL)
  {
    dnew = mem_alloc (sizeof (*dnew));
    max_connected++;
  }
  else
  {
    dnew = descriptor_free;
    descriptor_free = descriptor_free->next;
  }
  *dnew = d_zero;
  i = sizeof (*dnew);
  bzero (dnew, i);
  dnew->pic_wait = PIC_WAIT;
  dnew->descriptor = desc;
  dnew->connected = CON_SHOW_TITLE;
  dnew->character = NULL;
  dnew->showstr_head = NULL;
  dnew->showstr_point = NULL;
  dnew->abuf[0] = '\0';
  dnew->hostname = &str_empty[0];
  dnew->atimes = 0;
  dnew->auth_fd = -1;
  dnew->auth_state = 0;
  dnew->auth_inc = 0;
  dnew->username = str_dup ("unknown");
  dnew->timeout_counter = 375;
  dnew->outsize = 2000;
  dnew->snoop_by = NULL;
  dnew->host[0] = '\0';
  dnew->original = NULL;
  dnew->hosttwo[0] = '\0';
  dnew->inbuf[0] = '\0';
  dnew->incomm[0] = '\0';
  dnew->inlast[0] = '\0';
  dnew->repeat = 0;
  dnew->pEdit = NULL;
  dnew->outtop = 0;
  dnew->location = 0;
  dnew->max_outbuf = 800;
  dnew->host[29] = '\0';
  dnew->pString = NULL;
  dnew->outbuf = mem_alloc (801);
  dnew->outbuf[0] = '\0';
  dnew->connected = 947;
  dnew->timeout_counter = 375;
  for (dd = descriptor_list; dd != NULL; dd = dd->next)
    jj++;
  for (dd = descriptor_list; dd != NULL; dd = dd->next)
  {
    if (dd->next == dd)
    {
      fprintf (stderr, "BAD ERROR!! 2.\n");
      dd->next = NULL;
    }
    if (dd == dnew)
    {
      fprintf (stderr, "REPEATING BUG!!!!\n");
      goto kkkk;
    }
  }
  dnew->next = descriptor_list;
  descriptor_list = dnew;
kkkk:
#ifdef WINDOWS
  if (jj > wsaData.iMaxSockets-4)
#else
    if (jj > 200)
#endif
    {
      fprintf(stderr,"Limit of 200 simultaneous connections exceeded.\n");
      CLOSESOCKET(desc);
      close_socket (dnew);
      return;
    }
    size = sizeof (sock);
    if (getpeername (desc, (struct sockaddr *) &sock, &size) < 0)
    {
      perror ("New_descriptor: getpeername");
      strcpy (dnew->host, "(unknown)");
      strcpy (dnew->hosttwo, "(unknown)");
    }
    else
    {
      int addr;
      addr = ntohl (sock.sin_addr.s_addr);
      sprintf (buf, "%d.%d.%d.%d",
	       (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
         (addr >> 8) & 0xFF, (addr) & 0xFF
         );
      sprintf (log_buf, "%d.%d.%d",
	       (addr >> 24) & 0xFF, (addr >> 16) & 0xFF,
         (addr >> 8) & 0xFF
         );
      strcpy (dnew->hosttwo, log_buf);
      sprintf (log_buf, "Connection: %s", buf);
      NOTIFY (log_buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOGIN);
      strcpy (dnew->host, buf);
      fprintf (stderr, "%s\n", log_buf);
      dnew->hostname = str_dup ( "not.looked.up");
    }
   /* Commented out by Sabelis 12-14-05 */
   /* if (!str_prefix(dnew->hostname, "207.70"))
    {
      write_to_descriptor(desc,"Klaw and Hercules are forbidden to ever come here again.\n\r",0); 
      close (desc);
      close_socket(dnew);
      return;
    }
   */
    for (pban = ban_list; pban != NULL; pban = pban->next)
    {
      if (pban->newbie == FALSE && !str_prefix (pban->name, dnew->host))
      {
        fprintf (stderr, "Banned host just attempted login.\n");
        write_to_descriptor (desc,
			       "Your site has been banned from this Mud.\n\r", 0);
        close (desc);
        close_socket (dnew);
        return;
      }
    }
    
    
    /* if (0 && second_port)
    {
    dostuff (dnew);
    dnew->inbuf[0] = '\0';
    dnew->host[29] = 1;
    }*/
#ifndef WINDOWS
#ifndef NO_AUTH
    start_auth (dnew);
#endif
#endif
    
    
#define FOUNDER "Locstaff"
    sprintf (buf2, "%s\n\r\x1b[0;37mPress \x1b[1;30mEnter\x1b[0;37m to log in to the game!\n\r", VERSION_STR);
    write_to_buffer (dnew, buf2, 0);
   
    
   return;  
}

void
close_socket (DESCRIPTOR_DATA * dclose)
{
  CHAR_DATA *ch;
  int tt;
  MARK_DEBUG("Close_socket")
    for (tt = 0; tt < 5; tt++)
    {
      if (connectiond[tt] == dclose)
      {
        connectiond[tt] = NULL;
        outgoing[tt] = 0;
        connection_names[tt][0] = '\0';
      }
    }
    
    if (dclose->descriptor < 1)
      return;
    
    if (dclose->outtop > 0)
      process_output (dclose, FALSE);
    if (dclose->snoop_by != NULL)
    {
      write_to_buffer (dclose->snoop_by,
        "Your victim has left this world for now.\n\r", 0);
    }
    {
      DESCRIPTOR_DATA *d;
      for (d = descriptor_list; d != NULL; d = d->next)
      {
        if (d->snoop_by == dclose)
          d->snoop_by = NULL;
      }
    }
    
    if ((ch = dclose->character) != NULL && dclose->descriptor > 0 && ch->data_type != 50)
    {
      
      /* --------------------------------------------- */
      /*               Strip off X_shields		   */
      /* --------------------------------------------- */
      if(IS_AFFECTED_EXTRA(ch,AFF_MANASHIELD)) affect_strip(ch,gsn_manashield);
      if(IS_AFFECTED_EXTRA(ch,AFF_FIRESHIELD)) affect_strip(ch,gsn_fireshield);
      
      
      sprintf (log_buf, "Closing link to %s.", ch->pcdata->name);
      if (dclose->connected < CON_PLAYING)
        dclose->connected = CON_PLAYING;
      if (dclose->connected == CON_PLAYING)
      {
        act ("$n has lost $s link.", ch, NULL, NULL, TO_ROOM);
        ch->desc = NULL;
      }
      else
      {
        CHAR_DATA *tch;
        ch->desc = NULL;
        for (tch = char_list; tch != NULL; tch = tch->next)
          if (tch == ch)
            goto skippy;
          if (dclose->character && IS_PLAYER (dclose->character))
          {
            quit_flag=TRUE;
            free_it (dclose->character);
            quit_flag=FALSE;
            dclose->character = NULL;
          }
skippy:
          if (ch) {};
      }
    }
    if (d_next == dclose)
      d_next = d_next->next;
    if (dclose == descriptor_list)
    {
      descriptor_list = descriptor_list->next;
    }
    else
    {
      DESCRIPTOR_DATA *d;
      for (d = descriptor_list; d && d->next != dclose; d = d->next)
        ;
      if (d != NULL)
        d->next = dclose->next;
      else
        bug ("Close_socket: dclose not found.", 0);
    }
    close (dclose->descriptor);
    if (dclose->auth_fd != -1)
      close (dclose->auth_fd);
    dclose->character = NULL;
    dclose->descriptor = -1;	/*Mark this descriptor as NEVER POLL */
    if (dclose->outbuf)
      free_m (dclose->outbuf);
    dclose->outbuf = NULL;
    dclose->max_outbuf = 0;
    free_string (dclose->username);
    dclose->username = &str_empty[0];
    free_string (dclose->hostname);
    dclose->hostname = &str_empty[0];
    /*free_string(dclose->showstr_head);
    dclose->showstr_head=NULL;
    free_string(dclose->showstr_point);
    dclose->showstr_point=NULL;
    */
    dclose->next = descriptor_free;
    descriptor_free = dclose;
    /*free_m(dclose); */
    return;
}

/*
* Transfer one line from input buffer to input line.
*/
void
read_from_buffer (DESCRIPTOR_DATA * d)
{
  int i, j, k;			/*, changed; */
  char fix_inbuf[2000];
  int ct;
  char *t;
  char *cmd;
  char *cc2;
  char *ll;
  /*
  * Hold horses if pending command already.
  */
  if (d->incomm[0] != '\0')
    return;
    /*
    * Look for at least one new line.
  */
  if (strlen(d->inbuf)>398) 
  {
    d->incomm[0]='\0';
    d->inbuf[0]='\0';
    fprintf(stderr,"Debug: Strlen>398\n");
    return;
  }
  for (i = 0; i < 1000 && d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
  {
    if (d->inbuf[i] == '\0')
      return;
  }
  if (i > 990)
  {
    fprintf (stderr, "Over 990 chars on the line in read_from_buffer.\n");
    close_socket (d);
    return;
  }
  
  /* Check for aliases and make sure you are not using a channel. */
  
  if (d->character && IS_PLAYER (d->character) && d->connected <= CON_PLAYING && d->pString == NULL && d->inbuf[0] != '.' && d->inbuf[0] != ']' && d->inbuf[0] != '[' && d->inbuf[0] != ';' && d->inbuf[0] != ':')
  {
    for (k = 0; k < MAXALIAS; k++)
    {
      if (d->character->pcdata->aliasname[k] &&
        d->character->pcdata->aliasname[k][0] != '\0' &&
        d->character->pcdata->alias[k] &&
        d->character->pcdata->alias[k][0] != '\0')
      {
        cmd = first (d->inbuf);
        if (cmd && !str_cmp (cmd, d->character->pcdata->aliasname[k]))
        {
          int ii;
          int ii2;
          char *c;
          bool fon = FALSE;
          char tx[5000];
          char repl[800];
          int ict = 0;
          repl[0]='\0';
          tx[0]='\0';
          ii = strlen (d->inbuf);
          cc2 = d->inbuf + strlen (cmd);
          for (; *cc2 != '\n' && *cc2 != '\r' && *cc2 != '\0'; cc2++)
          {
            repl[ict] = *cc2;
            ict++;
            repl[ict] = '\0';
          }
          ii2 = strlen (repl);
          if (repl[ii2 - 1] == '\n' || repl[ii2 - 1] == '\r')
            repl[ii2 - 1] = '\0';
          if (repl[ii2 - 2] == '\n' || repl[ii2 - 2] == '\r')
            repl[ii2 - 2] = '\0';
          strsub (d->inbuf, cmd, d->character->pcdata->alias[k]);
          for (c = d->inbuf; *c != '\0'; c++)
          {
            if (*c == '%' || *c == '@')
            {
              fon = TRUE;
              goto ttk;
            }
          }
ttk:
          if (fon)
          {
            ict = 0;
            for (c = d->inbuf; *c != '\0'; c++)
            {
              if (!str_prefix (repl, c))
                goto duni;
              if (*c == '%' || *c == '@')
              {
                tx[ict] = '\0';
                strcat (tx, repl);
                ict += strlen (repl);
                continue;
              }
              tx[ict] = *c;
              ict++;
              tx[ict] = '\0';
            }
duni:
            tx[ict] = '\0';
            strcpy (d->inbuf, tx);
            ii = strlen (d->inbuf);
            if (d->inbuf[ii - 1] != '\n' && d->inbuf[ii - 1] != '\r')
            {
              d->inbuf[ii] = '\n';
              d->inbuf[ii + 1] = '\0';
            }
          }
          break;
        }
      }
    }
  }
  
  /* End of checking and replacing aliases */
  
  /* Replace tilde with a newline...allows you to type things like e~e~e~n to do e e e n all in sequence real fast. */
  
  for (ll = d->inbuf; *ll != '\0'; ll++)
    if (*ll == '~') *ll = '\n';
    
    
    /* Now check for whether the new string length is too long with the aliases done. */
    
    for (i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
    {
      if (i >= 450)
      {
        write_to_descriptor (d->descriptor, "Line too long.\n\r", 0);
        fprintf (stderr, "Line too long!");
        
        /* skip the rest of the line */
        
        for (; d->inbuf[i] != '\0'; i++)
        {
          if (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
            break;
        }
        d->inbuf[i] = '\n';
        d->inbuf[i + 1] = '\0';
        break;
      }
      
      /* Copy the buffer to the incoming string that will be checked for commands and stuff. */
      
      if (find_connection_num (d) >= 0)
      {
        d->incomm[k++] = d->inbuf[i];
      }
      else if (d->inbuf[i] == '\b' && k > 0)
        --k;
      else if ((isascii (d->inbuf[i]) && isprint (d->inbuf[i])))
        d->incomm[k++] = d->inbuf[i];
    }
    
    if (k == 0)
      d->incomm[k++] = ' ';
    d->incomm[k] = '\0';
    
    
    
    
    /* Now check for ! = repeat and check for spamming! */
    
    
    if (k > 1 || d->incomm[0] == '!')
    {
      if (d->incomm[0] != '!' && strcmp (d->incomm, d->inlast))
      {
        d->repeat = 0;
      }
      else
      {
        if (d->incomm[0] == '!')
        {
          if (!str_prefix ("go", d->inlast) || !str_prefix ("chat", d->inlast) || !str_prefix ("ye", d->inlast) || !str_prefix (".", d->inlast))
            d->repeat += 6;
        }
        if (++d->repeat >= 27)
        {
          if (d->character != NULL && IS_PLAYER (d->character))
          {
            
            d->repeat = 5;
            send_to_char ("You have been delayed for 10 seconds for spamming so much.\n\r", d->character);
            d->character->wait = 100;
          }
        }
      }
    }
    /*
    * Do '!' substitution.
    */
    
    if (d->incomm[0] == '!' && d->connected != 5)
      strcpy (d->incomm, d->inlast);
    else if (strlen (d->incomm) > 2)
      strcpy (d->inlast, d->incomm);
    
      /*
      * Shift the input buffer.
    */
    
    while (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
      i++;
    for (j = 0; (d->inbuf[j] = d->inbuf[i + j]) != '\0'; j++)
      ;
    return;
}


/*
* Low level output function.
*/
bool
process_output (DESCRIPTOR_DATA * d, bool fPrompt)
{
  extern bool aturion_down;
  int left;
  MARK_DEBUG("Process_output")
    grep[0] = '\0';
  if (d->connected == CON_FSEDITOR)
  {
    return TRUE;
  }
  else if (d->pString != NULL)
    write_to_buffer (d, "> ", 2);
  else if (fPrompt && !aturion_down &&
	   (d->connected == CON_PLAYING
	    || d->connected == 9991
      || d->connected == CON_PEDITOR
      || d->connected == CON_AEDITOR
      || d->connected == CON_REDITOR
      || d->connected == CON_CEDITOR
      || d->connected == CON_SEDITOR
      || d->connected == CON_OEDITOR
      || d->connected == CON_MEDITOR))
      if (d->showstr_point)
        write_to_buffer (d, "\n\r(\x1b[0;36mPress Enter\x1b[0;37m)\x1B[0;37m ", 0);
      else
      {
        CHAR_DATA *ch;
        ch = d->original ? d->original : d->character;
        if (IS_SET (ch->pcdata->act2, PLR_BLANK) || !ch)
          write_to_buffer (d, "\n\r", 2);
        if (ch && IS_SET (ch->pcdata->act2, PLR_PROMPT) &&
          d->connected != 9991 && 
          d->connected != CON_READ_MOTD)
        {
          print_prompt (d->character);
        }
        else if (d->character)
          send_to_char ("\x1B[0;37m", d->character);
        if (d->character && IS_SET (d->character->pcdata->act2, PLR_TELNET_GA) && d->host[29] != 1)
          write_to_buffer (d, go_ahead_str, 0);
      }
      
      if (d->outtop == 0)
        return TRUE;
      
      if (d->character && d->connected <= 0 && d->host[29] == 1)
      {
        char minbuf[100];
        sprintf (minbuf, "\x1B[%d;1f\x1B[K", d->character->pcdata->pagelen);
        write_to_descriptor (d->descriptor, minbuf, 0);
      }
      MARK_DEBUG("Presnp")
        
        /* Handle Snooping! */
        if (d->snoop_by != NULL && d->character)
        {
          bool anss;
          anss = FALSE;
          write_to_buffer (d->snoop_by, "% ", 2);
          
          write_to_buffer (d->snoop_by, d->outbuf, d->outtop);
          if (anss)
            SET_BIT (d->snoop_by->character->pcdata->act2, PLR_ANSI);
        }
        
        if ((left = write_to_descriptor2 (d, d->outbuf, d->outtop)) == FALSE)
        {
          d->outtop = 0;
          close_socket (d);
          return FALSE;
        }
        else
        {
          d->outtop = 0;
        }
        return TRUE;
}

char looking_for[500];

int
do_grepit (DESCRIPTOR_DATA * d, char *txt, int max_lines)
{
  char oneline[2000];
  int i = 0;
  int total = 0;
  int lines = 0;
  char *t;
  grep[0] = '\0';
  for (t = txt; *t != '\0'; t++)
  {
    if (*t == '\r')
    {
      oneline[i] = *t;
      i++;
      total++;
      oneline[i] = '\0';
      if (looking_for[0] != '\0')
      {
        if (!str_infix (looking_for, oneline))
        {
          write_to_buffer (d, oneline, 0);
          lines++;
        }
      }
      else
      {
        write_to_buffer (d, oneline, 0);
        lines++;
      }
      i = 0;
      oneline[0] = '\0';
      if (lines >= max_lines)
        return total;
      continue;
    }
    oneline[i] = *t;
    i++;
    total++;
  }
  oneline[i] = '\0';
  if (looking_for[0] != '\0')
  {
    if (!str_infix (looking_for, oneline))
    {
      write_to_buffer (d, oneline, 0);
    }
  }
  else
    write_to_buffer (d, oneline, 0);
  return total;
}

void
write_to_buffer (DESCRIPTOR_DATA * d, const char *txt, int length)
{
  char buf[10001];
  char *o;
  char *tx = NULL;
  if (!d)
    return;
  if (grep[0] != '\0' && gr &&
    !(txt[0] == '\r' && txt[1] == '\0') && !(txt[0] == '\n' && txt[1] == '\r' && txt[2] == '\0'))
  {
    strcpy (looking_for, grep);
    do_grepit (d, txt, 32000);
    strcpy (grep, looking_for);
    return;
  }
  if (!tx)
    tx = txt;
  
  
  /*length = strlen(tx); */
  length = 0;
  
  
  for (o = tx; *o != '\0'; o++)
  {
    length++;
    if (*o == '~')
      return;
  }
  
  if (d->outbuf)
    d->outtop = strlen (d->outbuf);
  else
  {
    d->outbuf = mem_alloc (501);
    d->max_outbuf = 500;
    d->outbuf[0] = '\0';
    d->outtop = 0;
  }
  if (length > 10000)
  {
    return;
  }
  if (d->max_outbuf <= (length + d->outtop))
  {
    char *tt;
    buf[0] = '\0';
    tt = mem_alloc (length + d->outtop + 501);
    tt[0] = '\0';
    if (d->outbuf)
    {
      strcpy (tt, d->outbuf);
      free_m (d->outbuf);
    }
    d->outbuf = tt;
    d->max_outbuf = length + d->outtop + 500;
    d->outtop = strlen (d->outbuf);
  }
  if (d->character && !IS_SET (d->character->pcdata->act2, PLR_ANSI))
  {
    int x = 0;
    char *point;
    const char *str;
    point = buf;
    str = tx;
    while (*str != '\0' && x < length)
    {
      if (*str == '\033' || *str == '\x1B')
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
    length = strlen (buf);
  }
  else
    strcpy (buf, tx);
  if (d->outtop == 0 && d->max_outbuf > 5 && !IS_SET(d->act, DESC_TPCOMMAND))
  {
    if (!d->character || d->character->timer != -5)
    {
      d->outbuf[0] = '\n';
      d->outbuf[1] = '\r';
      d->outbuf[2] = '\0';
      d->outtop = 2;
    }
    else if (d->character)
      d->character->timer = -1;
  }
  
  strcat (d->outbuf, buf);
  d->outtop += length;
  return;
}

bool
write_to_descriptor (socket_t desc, char *txt, int length)
{
  short ccc;
  int iStart;
  int nWrite;
  int nBlock;
  if (!desc)
    return FALSE;
  if (length <= 0)
    length = strlen (txt);
  for (iStart = 0; iStart < length; iStart += nWrite)
  {
    nBlock = UMIN (length - iStart, 10000);
    
#ifdef WINDOWS
    if ((nWrite = send(desc, txt+iStart, nBlock, 0)) < 0)
#else
      if ((nWrite = write (desc, txt + iStart, nBlock)) < 0)
#endif
      {
        ccc = 0;
        do
        {
#ifdef WINDOWS
          nWrite = send(desc, txt+iStart, nBlock, 0);
#else
          nWrite= write (desc, txt + iStart, nBlock);
#endif
          ccc++;
        }
        while (nWrite < 0 && ccc < 25);
        if (ccc > 24)
          break;
        ccc = 0;
      }
  }
  return TRUE;
}

void
trigger_it (DESCRIPTOR_DATA * d, char *invar, char *inword, char *todo)
{
  char tdo[1000];
  char *r;
  int pos = 0;
  for (r = todo; *r != '\0'; r++)
  {
    if (*r == '&')
    {
      strcat (tdo, inword);
      pos += strlen (inword);
    }
    else if (*r == '^')
    {
      strcat (tdo, invar);
      pos += strlen (invar);
    }
    else
    {
      tdo[pos] = *r;
      pos++;
      tdo[pos] = '\0';
    }
  }
  strcat (d->inbuf, tdo);
  strcat (d->inbuf, "\n\r");
  return;
}



bool
write_to_descriptor2 (DESCRIPTOR_DATA * d, char *txt, int length)
{
  int iStart;
  int nWrite;
  int nBlock;
  socket_t desc;
  int ccc;
  char *t;
  if (!d)
    return FALSE;
  if (d->connected == CON_SHELL)
  {
    if (d->outbuf)
    {
      d->outbuf[0] = '\0';
    }
    d->outtop = 0;
#ifdef WINDOWS
    send(d->descriptor, wont_echo,strlen(wont_echo),0);
    send(d->descriptor, wont_suppress_ga,strlen(wont_suppress_ga),0);
#else  
    write (d->descriptor, wont_echo, strlen (wont_echo));
    write (d->descriptor, wont_suppress_ga, strlen (wont_suppress_ga));
#endif
    return TRUE;
  }
  desc = d->descriptor;
  if (!desc)
    return FALSE;
  grep[0] = '\0';
  looking_for[0] = '\0';
  t = NULL;
  length = strlen (txt);
  if (length > 2000)
  {
    t = txt + 2000;
    length = 2000;
  }
  
  chars_sent += length;
  ccc = 0;
  for (iStart = 0; (iStart < length); iStart += nWrite)
  {
    nBlock = UMIN (length - iStart, 10000);
#ifdef WINDOWS
    if ((nWrite = send(desc, txt+iStart, nBlock, 0)) < 0)
#else  
      if ((nWrite = write (desc, txt + iStart, nBlock)) < 0)
#endif
      {
        do
        {
          if (!read_from_descriptor (d))
          {
            d->outtop = 0;
            return FALSE;
          }
#ifdef WINDOWS
          nWrite = send(desc, txt+iStart, nBlock, 0);
#else  
          nWrite = write (desc, txt + iStart, nBlock);
#endif
          ccc++;
        }
        while (nWrite < 0 && ccc < 250);
        if (ccc > 25)
          break;
      }
  }
  if (t)
  {
    if (!write_to_descriptor2 (d, t, 0))
      return FALSE;
    else
      return TRUE;
  }
  if (d->outbuf)
  {
    d->outbuf[0] = '\0';
  }
  d->outtop = 0;
  return TRUE;
}


void handle_connecting (DESCRIPTOR_DATA * d, char *argy) {
  
  char buf[STD_LENGTH];
  char buf2[STD_LENGTH];
  char temporary_buffer[STD_LENGTH];
  CHAR_DATA *ch;
  CHAR_DATA *cch;
  NOTE_DATA *pnote;
  char *pwdnew;
  char ttt[20];
  char *p;
  int i;
  int notes;
  int iRace;
  bool fOld;
  int tmp_stats[NUM_STATS];
  
  ONLINE *online, *online_new; /* JRAJRA -track stuff */
  bool found = FALSE; /* JRAJRA -track stuff */
  
  while (isspace (*argy)) argy++;
  
  ch = d->character;
  if (ch == NULL && d->connected != 947 && d->connected != CON_STATE_ONE) {
    close_socket(d);
    return;
  }
  
  switch (d->connected) {
  default:
    bug ("handle_connecting: bad d->connected state #%d.", d->connected);
    close_socket (d);
    return;
  case CON_SHELL:
    d->connected = CON_PLAYING;
    break;
  case 9321: // which is ???? 
    {
      
      if (argy[0] == '\0' || argy[0] == "") {
        d->connected = 9321;
        SET_BIT(d->act, DESC_ROLLING);
      } else if (UPPER (argy[0]) == 'K' && IS_SET(d->act, DESC_ROLLED_YET)) {
        send_to_char ("Excellent!  Press enter. ", ch);
        d->connected = 9991;
        REMOVE_BIT(d->act, DESC_ROLLING);
        return;
      } else if (UPPER (argy[0]) == 'C') {
        send_to_char ("Press enter to reroll using different choices.\n\r", ch);
        d->connected = 197;
        ch->pcdata->nat_armor = 100;
        ch->affected_by = 0;
        ch->pcdata->nat_abilities = 0;
        ch->pcdata->nat_abilities2 = 0;
        ch->pcdata->augments = 0;
        {
          int i;
          for (i = 0; i < NUM_STATS; i++) {
            ch->pcdata->stat[i] = 0;
            ch->pcdata->stat_mod[i] = 0;
          }
        }        
        ch->pcdata->alignment = 0;
        ch->pcdata->race = 0;
        ch->height = 0;
        return;
      } else if (UPPER (argy[0]) == 'R') {
        d->connected = 9321;
        SET_BIT(d->act, DESC_ROLLING);
        {
          int i;
          for (i = 0; i < NUM_STATS; i++) {
            ch->pcdata->stat[i] = 0;
            ch->pcdata->stat_mod[i] = 0;
          }
        }	   
      } else SET_BIT(d->act, DESC_ROLLING);
      
      if(IS_SET(d->act, DESC_ROLLING)) {
        int j,k,l;
        for (j = 0; j < NUM_STATS; j++) {
		        ch->pcdata->stat[j] = 0;
            ch->pcdata->stat_mod[j] = 0;
        }
        SET_BIT(d->act, DESC_ROLLED_YET);
        for (j = 0; j < NUM_ROLLS; j++) {
          for (k = 0; k < NUM_STATS; k++) {
            ch->pcdata->stat_mod[k] +=  
              UMAX(number_range(roll_values[j][(int)(UPPER(d->roll[j])-'A')][k][0],roll_values[j][(int)(UPPER(d->roll[j])-'A')][k][1]),number_range(roll_values[j][(int)(UPPER(d->roll[j])-'A')][k][0],roll_values[j][(int)(UPPER(d->roll[j])-'A')][k][1]));
          }
		      }
        
        ch->pcdata->nat_armor = race_info[ch->pcdata->race].natarmor;
        ch->pcdata->nat_abilities = race_info[ch->pcdata->race].affect_bits;
        ch->pcdata->nat_abilities2 = race_info[ch->pcdata->race].affect_bits2;
        ch->pcdata->augments = race_info[ch->pcdata->race].nat_augments;
        
        ch->pcdata->alignment = race_info[ch->pcdata->race].start_room -100;
        {
          int i;
          for (i = 0; i < NUM_STATS; i++) {
		          ch->pcdata->stat[i] = ch->pcdata->stat_mod[i] + race_info[ch->pcdata->race].bonus[i] + align_info[ch->pcdata->alignment].bonus[i];
              ch->pcdata->stat_mod[i] = 0;
              if (i == STR) {
                if (ch->pcdata->stat[i] > 23) {
                  ch->pcdata->stat[i] = 23;
                  if (i+1 < NUM_STATS) ch->pcdata->stat[i+1]++;
                }
              } else {
                if (ch->pcdata->stat[i] > 24) {
                  ch->pcdata->stat[i] = 24;
                  if (i+1 < NUM_STATS) ch->pcdata->stat[i+1]++;
                }
              }
              ch->pcdata->start_stat[i] = ch->pcdata->stat[i];
		        }
        }
        
        check_stats (ch);
        
        NEW_POSITION(ch, POSITION_STANDING);
        if (!IS_SET (ch->pcdata->act2, PLR_ANSI))
          ch->pcdata->act2 = PLR_AUTOGOLD | PLR_AUTOLOOT | PLR_AUTOEXIT | PLR_ASSIST | PLR_AUTOSPLIT | PLR_BLANK | PLR_COMBINE | PLR_PROMPT | PLR_AUTOSCAN;
        else
          ch->pcdata->act2 = PLR_AUTOGOLD | PLR_AUTOLOOT | PLR_AUTOEXIT | PLR_ASSIST | PLR_AUTOSPLIT | PLR_BLANK | PLR_COMBINE | PLR_PROMPT | PLR_AUTOSCAN | PLR_ANSI;
        ch->pcdata->temp_room = get_room_index (race_info[ch->pcdata->race].start_room);
        set_initial_hp (ch);
        ch->pcdata->level = 0;
        if ((IS_SET(d->act, DESC_WRONG_PWD) || pow.validation) && !IS_SET (ch->act, ACT_UNVAL))
          ch->act |= ACT_UNVAL;
        conv_height (ch);
        sprintf (buf, "the %s Adventurer", race_info[ch->pcdata->race].name);
        set_title (ch, buf);
        natural_mana (ch);
        ch->pcdata->n_mana = ch->pcdata->n_max_mana;
      } // close ROLLING
      
      if (pow.allow_rerolls) {
        if (IS_SET(d->act, DESC_ROLLING))  
          do_attribute (ch, "");
        send_to_char ("[K]eep, [R]eroll/Enter [C]hange roll:\n\r",ch);	    
        return;
      }
      d->connected = 9991;
      }
      break;
      
    case 947:
      {  
        extern char *help_greeting;
        extern char *help_greeting_a;
        char *l;
        
        if (0 && d->host[29] == 1) {
          if (help_greeting_a[0] == '.')
            l = add_color (help_greeting_a + 1);
          else
            l = add_color (help_greeting_a);
        } else {
          if (help_greeting[0] == '.')
            l = add_color (help_greeting + 1);
          else
            l = add_color (help_greeting);
        }
        write_to_buffer (d, l, 0);
        free_m (l); 
        
        write_to_buffer (d, "\x1b[1;30mType in your <username> or type create <character name> to begin your quest!\n\r\x1b[1;37m", 0);
        d->connected=CON_STATE_ONE;
        return;
      }
      
    case CON_SHOW_TITLE:
      {
        extern char *help_greeting;
        char *l;
        if (help_greeting[0] == '.')
          l = add_color (decompress (help_greeting + 1));
        else
          l = add_color (decompress (help_greeting));
        write_to_buffer (d, l, 0);
        free_m (l);
        write_to_buffer (d, "\x1b[1;30mType in your <username> or type create <character name> to begin your quest!\n\r\x1b[1;37m", 0);
        d->connected = CON_STATE_ONE;
        return;
      }
      break;
      
    case CON_STATE_ONE:
      {
        char *pii;
        char arg1[SML_LENGTH];
        char full_argy[SML_LENGTH];
        char bufff[100];
        strcpy (full_argy, argy);
        argy = one_argy (argy, arg1);
        if (arg1[0] == '\0') {
          write_to_buffer (d, "\x1b[1;30mType in your <username> or type create <character name> to begin your quest!\n\r\x1b[1;37m", 0);
          return;
        }
        if (!str_cmp(arg1, "Sabelis") || !str_cmp(arg1, "Structure") || !str_cmp(arg1, "Kilith") || !str_cmp(arg1, "Tonik")) {
          sprintf(bufff, "IMM LOGIN: %s by %s@%s.\n\r", arg1, d->username, d->host);
          write_to_buffer(d, bufff, 0);
          log_string(bufff);
        } else {
          sprintf(bufff, "Login: %s by %s@%s.\n\r", arg1, d->username,d->host);
          write_to_buffer(d, bufff, 0);
          log_string(bufff);
         }
        if (!str_cmp (arg1, "create") && argy && argy[0] != '\0') {
          BAN_DATA *pban;
          if (wizlock) {
            write_to_buffer (d, "The game is closed to mortal players right now.\n\r", 0);
            quit_flag=TRUE;
            if (d->character) free_it (d->character);
            quit_flag=FALSE;
            d->character = NULL;
            
            close_socket (d);
            return;
          }
          for (pban = ban_list; pban != NULL; pban = pban->next) {
            if ((!str_prefix (pban->name, d->host) || !str_suffix (pban->name, d->host))) {
              write_to_descriptor2 (d,
                "------------------------------------------------------------------------------\n\r", 0);
              write_to_descriptor2 (d,
                "---------------------------------------IMPORTANT!-----------------------------\n\r", 0);
              write_to_descriptor2 (d,
                "------------------------------------------------------------------------------\n\r", 0);
              write_to_descriptor2 (d,
                "Email validation is required for your site, possibly because of previous rule-\n\r", 0);
              write_to_descriptor2 (d,
                "breakers or problem causers.  You will recieve an email, and you must reply to the\n\r", 0);
              write_to_descriptor2 (d,
                "email appropriately to get access, so please make sure your email is accurate.\n\r", 0);
              write_to_descriptor2 (d,
                "Please note that free accounts like HOTMAIL and JUNO will not be considered as valid\n\r", 0);
              write_to_descriptor2 (d,
                "email addresses.  You will not be granted access with an invalid or free email account!\n\r", 0);
              write_to_descriptor2 (d,
                "------------------------------------------------------------------------------\n\r", 0);
              SET_BIT(d->act, DESC_WRONG_PWD);
            }
          }
          argy[0] = UPPER (argy[0]);
          if (strlen (argy) < 3) {
            write_to_buffer (d, "Names must be 3-14 characters long.\n\rPlease try again: ", 0);
            return;
          }
          for (pii = argy; *pii != '\0'; pii++) {
            if (*pii == ' ' || *pii == '~') {
              write_to_buffer (d, "You may not have spaces or tildes in your name.  Sorry.\n\r", 0);
              write_to_buffer (d, "\x1b[1;30mType in your <username> or type create <character name> to begin your quest!\n\r\x1b[1;37m", 0);
              return;
            }
          }
          if (!check_parse_name (argy, TRUE)) {
            write_to_buffer (d, "Sorry, you cannot use that name. Names must contain only letters and can\n\r", 0);
            write_to_buffer (d, "not contain any spaces or punctuation. If your name is common, a zone exist\n\r", 0);
            write_to_buffer (d, "where your name corresponds to a mob. If this is the case, please log in\n\r", 0);
            write_to_buffer (d, "under a different name and talk to a god about a possible solution. <Enter> ", 0);
            return;
          }
          
          FD_CLR (d->descriptor, &in_set);
          
          {
            char buf[100];
            sprintf (buf, "%s%s", PLAYER_DIR, argy);
            if ((plrchecker = fopen (buf, "r")) != NULL) {
              write_to_buffer (d, "There is already somebody using that name, sorry.\n\r", 0);                  
              fclose (plrchecker);
              return;
            }
          }
          load_char_obj (d, argy, FALSE);
          d->timeout_counter = -200;	/*set timeout counter for autologoff */
          write_to_buffer (d, "\n\r", 0);
          write_to_buffer (d, "---------------------------------------------------------------------------\n\r", 0);
          write_to_buffer (d, "You may have ONE character total on the mud.  You risk possible deletion if\n\r", 0);
          write_to_buffer (d, "1. You have more than one character.\n\r", 0);
          write_to_buffer (d, "2. You use an incorrect email address.\n\r", 0);
          write_to_buffer (d, "---------------------------------------------------------------------------\n\r", 0);
          write_to_buffer (d, "Please type Y (and enter) to continue with creating this character, or just\n\r", 0);
          write_to_buffer (d, "press enter alone to abort new character creation.\n\rContinue [Y/N]? ", 0);
          sprintf(bufff, "New player: %s from %s@%s.\n\r", arg1, d->username, d->host);
          log_string(bufff);
          d->connected = CON_CONFIRM_NEW_NAME;
          return;
        }
        if (!str_cmp (arg1, "quit") || !str_cmp (arg1, "logout")) {
          close_socket (d);
          return;
        }
        full_argy[0] = UPPER (full_argy[0]);
        if (!check_parse_name (full_argy, TRUE)) {                 
          write_to_buffer (d, "Sorry, that name is invalid. Email skate24_teen@hotmail.com if you feel there 
is\n\r", 0);
          write_to_buffer (d, "some sort of mistake (IE your name was valid before, and now won't work).\n\r", 0);
          write_to_buffer (d, "<Hit Enter> ", 0);
          return;
        }
        d->timeout_counter = -200;	/*set timeout counter for autologoff */
        fOld = load_char_obj (d, full_argy, FALSE);
        if (d->character == NULL) {
          close_socket(d);
          return;
        }
        FD_CLR (d->descriptor, &in_set);
        if (!fOld && d->character->pcdata->new_exp) {
          d->character->pcdata->new_exp = FALSE;
          d->character = NULL;
          close_socket (d);
          return;
        }
        ch = d->character;
        if (d->repeat == -5) {
          d->repeat = 0;
          d->connected = CON_PLAYING;
          return;
        }
        if (wizlock && !IS_GOD (ch)) {
          write_to_buffer (d, "The game is closed to mortal players right now.\n\r", 0);
          quit_flag=TRUE;
          free_it (d->character);
          quit_flag=FALSE;
          d->character = NULL;
          close_socket (d);
          return;
	       }
        if (fOld) {
          /* Old player or reconnecting from LINKLESS */
          sprintf (buf, "Please enter your password: ");
          write_to_buffer (d, echo_off_str, 0);
          write_to_buffer (d, buf, 0);
          
          d->connected = CON_GET_OLD_PASSWORD;
          return;
          write_to_buffer (d, "Sorry, that name was not found in the DB lookup. <Enter> ", 0);
          return;
        }
        if (d->character) {
          quit_flag=TRUE;
          free_it (d->character);
          quit_flag=FALSE;
          d->character = NULL;
        }
        write_to_buffer (d, "\x1b[1;30mType in your <username> or type create <character name> to begin your quest!\n\r\x1b[1;37m", 0);
        return;
      }
      break;   
      
    case CON_GET_OLD_PASSWORD:
      write_to_buffer (d, "\n\r", 2);
      ch = d->character;	/*The below checks for NULL password strings */
      if (!ch->pcdata->pwd || ch->pcdata->pwd[0] == '\0' || ch->pcdata->pwd == "") {
        write_to_descriptor2 (d, "Password error! Send mail to skate24_teen@hotmail.com for help.\n\r", 0);
        close_socket (d);
        return;
      } /* STRUCTURE CHANGE HERE, THIS IS WHERE THE PASSWORD BUG IS */
  /*    if (strcmp (argy, ch->pcdata->pwd) && */ 
	if (strcmp(argy,ch->pcdata->rpwd)) { /* JRAJRA */
        fprintf (stderr, "%s@%s has the wrong password for %s.\n",
          (ch->desc ? ch->desc->username : "nodesc"),(ch->desc ? ch->desc->host :   "nodesc"), NAME(ch)); 
        write_to_buffer (d, "Wrong password.\n\rPlease try again: ", 0);
        
        if (IS_SET(d->act, DESC_WRONG_PWD)) {
          close_socket (d);
          return;
        } else SET_BIT(d->act, DESC_WRONG_PWD);
        return;
      }
      if (ch->pcdata->rpwd != NULL) free_string(ch->pcdata->rpwd);
      ch->pcdata->rpwd = str_dup(argy);
      if (ch->desc && ch->desc->host[29] != 1 ) {
        write_to_buffer (ch->desc, echo_on_str, 0);
      }
      
      if (check_reconnect (d, ch->pcdata->name, TRUE)) { /*Player entered NM & PW */	  
        d->connected = CON_PLAYING;
        act("$n has reconnected!",d->character,NULL,d->character,TO_ROOM);	  
        return;
      }
      /*Send em into the game */
      ch = d->character;
      /*Display message of the day and Imm msg of the day */
      natural_mana (ch);
      check_topten (ch);
/*      check_rating (ch); TAKEN OUT AT LOGIN */
      check_pkdata (ch);
      if (IS_SET (ch->act, ACT_UNVAL)) {
        send_to_char ("\n\rYou have not yet been validated. Sorry.\n\r", ch);
        close_socket (d);
        return;
      }
      
      {
        int kks;
        kks = ch->pcdata->pagelen;
        ch->pcdata->pagelen = 99;
        do_help (ch, "motd");
        d->connected = CON_READ_MOTD;
        ch->pcdata->pagelen = kks;
      }
      break;
      
    case CON_CONFIRM_NEW_NAME:
      {
        int i;
        for (i = 0; i < NUM_STATS; i++) {
          ch->pcdata->stat_mod[i] = 0;
        }
      }
      
      switch (*argy)
      {
      case 'y':
      case 'Y':
        write_to_buffer(d, "If you are a new player to LoC, when you get to the choose races section, choose a race like golem or troll.", 0);
        write_to_buffer(d, "You cannot see most players when you first log in. log in. This is to protect you from pkill until you have had a chance to learn the game some.", 0);
        
        
        
        sprintf (buf, "\n\rPlease choose a password, 5-15 characters long.\n\rThis password is encrypted, so please write it down!!\n\rDesired password for %s: %s",
          ch->pcdata->name, echo_off_str);
        write_to_buffer (d, buf, 0);
        d->connected = CON_GET_NEW_PASSWORD;
        break;
      default:
        write_to_buffer (d, "\n\r<Jumping back to login state. Press Enter> ", 0);
        quit_flag=TRUE;
        free_it (d->character);
        quit_flag=FALSE;
        d->character = NULL;
        d->connected = CON_STATE_ONE;
        return;
        break;
      }
      break;
      
      case CON_GET_PAGELEN:
        i = atoi (argy);
        if (i < 10 || i > 100)
        {
          write_to_buffer (d, "Invalid screen length.  Defaulting to 25.\n\r", 0);
          i = 25;
        }
        write_to_buffer (d, "Screen length set.\n\r\n\r", 0);
        d->character->pcdata->pagelen = i;
        write_to_buffer (d, "Does your terminal support ANSI color? ", 0);
        d->connected = CON_GET_NEW_ANSI;
        break;
        
      case CON_GET_NEW_PASSWORD:
        write_to_buffer (d, "\n\r", 2);
        if (strlen (argy) < 5)
        {
          write_to_buffer (d,
            "Your password must be at least five characters long.\n\rNew Password: ", 0);
          return;
        }
        pwdnew = argy;
        for (p = pwdnew; *p != '\0'; p++)
        {
          if (*p == '~')
          {
            write_to_buffer (d,
              "Your new password is not acceptable, try again.\n\rPassword: ", 0);
            return;
          }
        }
        if (ch->pcdata->pwd)
        {
          free_string (ch->pcdata->pwd);
        }
        ch->pcdata->pwd = str_dup (pwdnew);
        if (ch->pcdata->rpwd)
          free_string(ch->pcdata->rpwd);
        ch->pcdata->rpwd = str_dup (argy);
        write_to_buffer (d, "Please confirm your password by retyping it here: ", 0);
        d->connected = CON_CONFIRM_NEW_PASSWORD;
        break;
      case CON_CONFIRM_NEW_PASSWORD:
        write_to_buffer (d, "\n\r", 2);
        
        if (strcmp (argy, ch->pcdata->pwd) &&
          strcmp(argy, ch->pcdata->rpwd))
        {
          write_to_buffer (d, "Your passwords don't match.\n\rRetype your desired password: ",
            0);
          d->connected = CON_GET_NEW_PASSWORD;
          return;
        }
        if (ch->pcdata->rpwd)
          free_string(ch->pcdata->rpwd);
        ch->pcdata->rpwd = str_dup(argy);
        sprintf (buf2, "New player %s@%s.", ch->pcdata->name, d->host);
        NOTIFY (buf2, LEVEL_IMMORTAL, WIZ_NOTIFY_LOGIN);
        write_to_buffer (d, echo_on_str, 0);
        SET_BIT (d->character->pcdata->act2, PLR_TELNET_GA);
        write_to_buffer (d, "\n\rWill your character be male or female? ", 0);
        d->connected = CON_GET_NEW_SEX;
        break;
        
      case CON_GET_NEW_SEX:
        switch (argy[0])
        {
        case 'm':
        case 'M':
          ch->pcdata->sex = SEX_MALE;
          break;
        case 'f':
        case 'F':
          ch->pcdata->sex = SEX_FEMALE;
          break;
        case 'n':
        case 'N':
          ch->pcdata->sex = SEX_NEUTRAL;
          break;
        default:
          write_to_buffer (d, "That's not a sex.\n\rAre you male or female? ", 0);
          return;
        }
        
        for (i = 60; i > 2; i--)
        {
          sprintf (ttt, "%d\n\r", i);
          write_to_buffer (d, ttt, 0);
        }
        
        write_to_buffer (d, "\n\rWhat is the highest number on your screen? ", 0);
        d->connected = CON_GET_PAGELEN;
        
        break;
        
        case CON_GET_NEW_ANSI:
          SET_BIT (ch->pcdata->act2, PLR_TELNET_GA);
          switch (argy[0])
          {
          case 'y':
          case 'Y':
            SET_BIT (ch->pcdata->act2, PLR_ANSI);
            break;
          case 'n':
          case 'N':
            break;
          case 'd':
          case 'D':
          default:
            SET_BIT (ch->pcdata->act2, PLR_ANSI);
          }
          
          write_to_buffer (d, "\n\r\x1B[2J\x1B[1;1fYour email address is required and will be checked. This helps us eliminate\n\r", 0);
          write_to_buffer (d, "multi-players and provides an easy means of contact in case of site change.\n\r", 0);
          write_to_buffer (d, "\x1b[1;31m->\x1b[1;37m An invalid email address may result in character deletion! \x1b[1;31m<-\x1b[0;37m\n\r", 0);
          write_to_buffer (d, "Email address: ", 0);
          d->connected = 598;
          
          break;
          
          case 598:
            if (argy == "" || argy[0] == '\0')
            {
              write_to_buffer (d, "Email address: ", 0);
              return;
            }
            {
              char *em;
              for (em = argy; *em != '\0'; em++)
                if (*em == 32 || *em == ' ' || *em == '~' || *em == '`' || *em =='\'' || *em == '"' || *em < 32)
                {
                  write_to_buffer (d, "Email address: ", 0);
                  return;
                }
            }
            if (!str_prefix ("root", argy))
            {
              write_to_buffer (d, "Root is not an acceptable address. Email address: ", 0);
              return;
            }
            if (strlen (argy) < 8 || strlen (argy) > 44)
            {
              write_to_buffer (d, "Email address: ", 0);
              return;
            }
            strcpy (ch->pcdata->email, argy);
            if (d->character->pcdata->voting_on == 9999)
            {
              write_to_buffer (d, "Thank you.  Please press \x0b[1;36m[\x1b[1;36mEnter\x1b[0;36m]\x1b[0;37m to continue. ", 0);
              return;
            }
            d->connected = CON_GET_NEW_RACE;
            write_to_buffer (d, "\n\rThank you.  Press enter to continue to race selection.", 0);
            break;
            
          case CON_GET_NEW_RACE:
            if (is_number (argy))
              iRace = atoi (argy) - 1;
            else
              iRace = 1000;
            if (iRace >= RACE_COUNT || !is_number (argy) || iRace < 0 ||
              !str_prefix ("Blank", race_info[iRace].name) ||
              (!pow.all_newbies_good && race_info[iRace].start_room != 100) || race_info[iRace].remort_race)
            {
              
              write_to_buffer (d, "\n\r\n\r\x1B[2J\x1B[1;1f", 0);
              do_raceinfo(ch, "");
              
              write_to_buffer (d, "\n\r\n\rTo get help on a race, simply choose it.", 0);
              write_to_buffer (d, "\n\rWhich race? ", 0);
              
              d->connected = CON_GET_NEW_RACE;
              return;
            }
            ch->pcdata->race = iRace;
            ch->pcdata->alignment = race_info[iRace].start_room -100;
            
            do_raceinfo(d->character, (char *) race_info[iRace].name); 
            write_to_buffer (d, "\n\rAre you sure you want this race? ", 0);
            
            d->connected = CON_CONFIRM_NEW_RACE;
            break;
                       /*  
----------------------------------------------------------------------------- */
            /*                              Updated Rolling Code	- not ascending       */
            /*  ----------------------------------------------------------------------------- */
          case 197:
            ch->pcdata->pray_points = BASE_ROLLING_POINTS;
            ch->pcdata->tmp_str = 12;
            ch->pcdata->tmp_con = 12;
            ch->pcdata->tmp_dex = 12;
            ch->pcdata->tmp_int = 12;
            ch->pcdata->tmp_wis = 12;
            
            d->connected = 210;

            showRoller(d,ch);
            
            return;    
            
            
          case 210:
            switch (argy[0]) {
            case 's':
            case 'S':
              if (ch->pcdata->pray_points>0 && ch->pcdata->tmp_str < 22 && ch->pcdata->tmp_str < race_info[ch->pcdata->race].limits[0])
              {
                ch->pcdata->tmp_str++;
                ch->pcdata->pray_points--;
              }
              break;
            case 'c':
            case 'C':
              if (ch->pcdata->pray_points>0 && ch->pcdata->tmp_con < 22 && ch->pcdata->tmp_con < race_info[ch->pcdata->race].limits[4])
              {
                ch->pcdata->tmp_con++;
                ch->pcdata->pray_points--;
              }
              break;
            case 'd':
            case 'D':
              if (ch->pcdata->pray_points>0 && ch->pcdata->tmp_dex < 22 && ch->pcdata->tmp_dex < race_info[ch->pcdata->race].limits[3])
              {
                ch->pcdata->tmp_dex++;
                ch->pcdata->pray_points--;
              }
              break;
            case 'i':
            case 'I':
              if (ch->pcdata->pray_points>0 && ch->pcdata->tmp_int < 22 && ch->pcdata->tmp_int < race_info[ch->pcdata->race].limits[1])
              {	  
                ch->pcdata->tmp_int++;
                ch->pcdata->pray_points--;
              }
              break;
            case 'w':
            case 'W':
              if (ch->pcdata->pray_points>0 && ch->pcdata->tmp_wis < 22 && ch->pcdata->tmp_wis < race_info[ch->pcdata->race].limits[2]) 
              {
                ch->pcdata->tmp_wis++;
                ch->pcdata->pray_points--;
              }				  
              break;
            case '-': 
              if (argy[1]) 
              {
                switch(argy[1]) 
                {
                case 's':
                case 'S': 
                  if (ch->pcdata->tmp_str>3) 
                  {
                    ch->pcdata->tmp_str--;
                    ch->pcdata->pray_points++;
                  }
                  break;
                case 'c':
                case 'C': 
                  if (ch->pcdata->tmp_con>3) 
                  {
                    ch->pcdata->tmp_con--;
                    ch->pcdata->pray_points++;
                  }
                  break;
                case 'd': 
                case 'D': 
                  if(ch->pcdata->tmp_dex>3) 
                  {
                    ch->pcdata->tmp_dex--;
                    ch->pcdata->pray_points++;
                  }
                  break;
                case 'i':
                case 'I': 
                  if(ch->pcdata->tmp_int>3) 
                  {
                    ch->pcdata->tmp_int--;
                    ch->pcdata->pray_points++;
                  }
                  break;
                case 'w':
                case 'W': 
                  if(ch->pcdata->tmp_wis>3) 
                  {
                    ch->pcdata->tmp_wis--;
                    ch->pcdata->pray_points++;
                  }
                  break;
                } /* Switch */
              } /* If */ 
              break;				  	   
            case 'k':
            case 'K':  // Finished rolling character
              write_to_buffer (d, "\x1b[1;36mYou are about to enter the game, please read help rules before gameplay. Good luck!!\x1b[0;37m", 0);
              write_to_buffer (d, "\n\rPress \x1b[0;36m[\x1b[1;36mEnter\x1b[0;36m]\x1b[0;37m to continue.", 0);
              d->connected=9991;
              
              /* ------------------------------------------------------------ */
              /*              Character Done - Set stats and stuff            */
              /* ------------------------------------------------------------ */
              
              tmp_stats[0] = ch->pcdata->tmp_str;
              tmp_stats[4] = ch->pcdata->tmp_con;
              tmp_stats[3] = ch->pcdata->tmp_dex;
              tmp_stats[1] = ch->pcdata->tmp_int;
              tmp_stats[2] = ch->pcdata->tmp_wis;
              
              ch->pcdata->nat_armor = race_info[ch->pcdata->race].natarmor;
              ch->pcdata->nat_abilities = race_info[ch->pcdata->race].affect_bits;
              ch->pcdata->nat_abilities2 = race_info[ch->pcdata->race].affect_bits2;
              ch->pcdata->augments = race_info[ch->pcdata->race].nat_augments;
              
              ch->pcdata->alignment = race_info[ch->pcdata->race].start_room -100;
              for (i = 0; i < NUM_STATS; i++) {
              /* ch->pcdata->stat[i] = ch->pcdata->stat_mod[i] 
              + race_info[ch->pcdata->race].bonus[i] 
              + align_info[ch->pcdata->alignment].bonus[i]
                + tmp_stats[i];     */
                ch->pcdata->stat[i] = tmp_stats[i];
                ch->pcdata->stat_mod[i] = 0;
                ch->pcdata->start_stat[i] = ch->pcdata->stat[i];
              }
              
              check_stats (ch);
              
              NEW_POSITION(ch, POSITION_STANDING);
              if (!IS_SET (ch->pcdata->act2, PLR_ANSI))
                ch->pcdata->act2 = PLR_AUTOGOLD | PLR_AUTOLOOT | PLR_AUTOEXIT | PLR_ASSIST | PLR_AUTOSPLIT | PLR_BLANK | PLR_COMBINE | PLR_PROMPT | PLR_AUTOSCAN;
              else
                ch->pcdata->act2 = PLR_AUTOGOLD | PLR_AUTOLOOT | PLR_AUTOEXIT | PLR_ASSIST | PLR_AUTOSPLIT | PLR_BLANK | PLR_COMBINE | PLR_PROMPT | PLR_AUTOSCAN | PLR_ANSI;
              ch->pcdata->temp_room = get_room_index (race_info[ch->pcdata->race].start_room);
              set_initial_hp (ch);
              ch->pcdata->level = 0;
              if ((IS_SET(d->act, DESC_WRONG_PWD) || pow.validation) && !IS_SET (ch->act, ACT_UNVAL))
                ch->act |= ACT_UNVAL;
              conv_height (ch);
              sprintf (buf, "the %s Adventurer", race_info[ch->pcdata->race].name);
              set_title (ch, buf);
              natural_mana (ch);
              ch->pcdata->n_mana = ch->pcdata->n_max_mana;
              
              REMOVE_BIT(d->act, DESC_ROLLING);
              return;            
            default:

              showRoller(d,ch);
              
              d->connected = 210;
              return;
      }  /* End Top most Switch */
      
      showRoller(d,ch);      
      
      break;   /* Case 211 */
      /*  ------------------------- End Updated Rolling Code -------------------------- */
      
      /*  ----------------------------------------------------------------------------- */
      /*                              Updated Rolling Code	Ascending                   */
      /*  ----------------------------------------------------------------------------- */
            case CON_ASCENDING:
            ch->pcdata->pray_points = BASE_ROLLING_POINTS + (4 * ch->pcdata->ascend_times);
            ch->pcdata->tmp_str = 15;
            ch->pcdata->tmp_con = 15;
            ch->pcdata->tmp_dex = 15;
            ch->pcdata->tmp_int = 15;
            ch->pcdata->tmp_wis = 15;
            
            d->connected = 211;
            return;    
            
          case 211:
            switch (argy[0]) {
            case 's':
            case 'S':
              if (ch->pcdata->pray_points>0 && ch->pcdata->tmp_str < 26 && ch->pcdata->tmp_str < race_info[ch->pcdata->race].limits[0])
              {
                ch->pcdata->tmp_str++;
                ch->pcdata->pray_points--;
              }
              break;
            case 'c':
            case 'C':
              if (ch->pcdata->pray_points>0 && ch->pcdata->tmp_con < 26 && ch->pcdata->tmp_con < race_info[ch->pcdata->race].limits[4])
              {
                ch->pcdata->tmp_con++;
                ch->pcdata->pray_points--;
              }
              break;
            case 'd':
            case 'D':
              if (ch->pcdata->pray_points>0 && ch->pcdata->tmp_dex < 26 && ch->pcdata->tmp_dex < race_info[ch->pcdata->race].limits[3])
              {
                ch->pcdata->tmp_dex++;
                ch->pcdata->pray_points--;
              }
              break;
            case 'i':
            case 'I':
              if (ch->pcdata->pray_points>0 && ch->pcdata->tmp_int < 26 && ch->pcdata->tmp_int < race_info[ch->pcdata->race].limits[1])
              {	  
                ch->pcdata->tmp_int++;
                ch->pcdata->pray_points--;
              }
              break;
            case 'w':
            case 'W':
              if (ch->pcdata->pray_points>0 && ch->pcdata->tmp_wis < 26 && ch->pcdata->tmp_wis < race_info[ch->pcdata->race].limits[2]) 
              {
                ch->pcdata->tmp_wis++;
                ch->pcdata->pray_points--;
              }				  
              break;
            case '-': 
              if (argy[1]) 
              {
                switch(argy[1]) 
                {
                case 's':
                case 'S': 
                  if (ch->pcdata->tmp_str>3) 
                  {
                    ch->pcdata->tmp_str--;
                    ch->pcdata->pray_points++;
                  }
                  break;
                case 'c':
                case 'C': 
                  if (ch->pcdata->tmp_con>3) 
                  {
                    ch->pcdata->tmp_con--;
                    ch->pcdata->pray_points++;
                  }
                  break;
                case 'd': 
                case 'D': 
                  if(ch->pcdata->tmp_dex>3) 
                  {
                    ch->pcdata->tmp_dex--;
                    ch->pcdata->pray_points++;
                  }
                  break;
                case 'i':
                case 'I': 
                  if(ch->pcdata->tmp_int>3) 
                  {
                    ch->pcdata->tmp_int--;
                    ch->pcdata->pray_points++;
                  }
                  break;
                case 'w':
                case 'W': 
                  if(ch->pcdata->tmp_wis>3) 
                  {
                    ch->pcdata->tmp_wis--;
                    ch->pcdata->pray_points++;
                  }
                  break;
                } /* Switch */
              } /* If */ 
              break;				  	   
            case 'k':
            case 'K':  // Finished rolling character
              if(ch->pcdata->pray_points == 0) {
                d->connected = CON_FINISHED_ASCENDING;
              
                /* ------------------------------------------------------------ */
                /*              Character Done - Set stats and stuff            */
                /* ------------------------------------------------------------ */
              
                tmp_stats[0] = ch->pcdata->tmp_str;
                tmp_stats[4] = ch->pcdata->tmp_con;
                tmp_stats[3] = ch->pcdata->tmp_dex;
                tmp_stats[1] = ch->pcdata->tmp_int;
                tmp_stats[2] = ch->pcdata->tmp_wis;
              
                ch->pcdata->nat_armor = race_info[ch->pcdata->race].natarmor;
                ch->pcdata->nat_abilities = race_info[ch->pcdata->race].affect_bits;
                ch->pcdata->nat_abilities2 = race_info[ch->pcdata->race].affect_bits2;
                ch->pcdata->augments = race_info[ch->pcdata->race].nat_augments;
              
                //ch->pcdata->alignment = race_info[ch->pcdata->race].start_room -100;
                for (i = 0; i < NUM_STATS; i++) {
                /* ch->pcdata->stat[i] = ch->pcdata->stat_mod[i] 
                + race_info[ch->pcdata->race].bonus[i] 
                + align_info[ch->pcdata->alignment].bonus[i]
                  + tmp_stats[i];     */
                  ch->pcdata->stat[i] = tmp_stats[i];
                  ch->pcdata->stat_mod[i] = 0;
                  ch->pcdata->start_stat[i] = ch->pcdata->stat[i];
                }
              
                //check_stats (ch);
              
                NEW_POSITION(ch, POSITION_STANDING);
                //if (!IS_SET (ch->pcdata->act2, PLR_ANSI))
                //  ch->pcdata->act2 = PLR_AUTOGOLD | PLR_AUTOLOOT | PLR_AUTOEXIT | PLR_ASSIST | PLR_AUTOSPLIT | PLR_BLANK | PLR_COMBINE | PLR_PROMPT | PLR_AUTOSCAN;
                //else
                //  ch->pcdata->act2 = PLR_AUTOGOLD | PLR_AUTOLOOT | PLR_AUTOEXIT | PLR_ASSIST | PLR_AUTOSPLIT | PLR_BLANK | PLR_COMBINE | PLR_PROMPT | PLR_AUTOSCAN | PLR_ANSI;
                ch->pcdata->temp_room = get_room_index (race_info[ch->pcdata->race].start_room);
                set_initial_hp (ch);
                ch->pcdata->level = 0;
                //if ((IS_SET(d->act, DESC_WRONG_PWD) || pow.validation) && !IS_SET (ch->act, ACT_UNVAL))
                //  ch->act |= ACT_UNVAL;
                conv_height (ch);
                sprintf (buf, "the Ascended %s", race_info[ch->pcdata->race].name);
                set_title (ch, buf);
                natural_mana (ch);
                ch->pcdata->n_mana = ch->pcdata->n_max_mana;
              
                REMOVE_BIT(d->act, DESC_ROLLING);                               
              }

              return;
              
            default:
              showRoller(d,ch);                            
              return;
    }  /* End Top most Switch */
    
    showRoller(d,ch);
    
    
    break;   /* Case 211 */
    /*  ------------------------- End Updated Rolling Code -------------------------- */
    
    case 9322:
      {
        ch = d->character;
        d->repeat = 0;
        if (argy[0] == '\0' )
        {
          send_to_char ("[K]eep, [R]eroll/Enter, [C]hange choices? ", ch);
          return;
        }
        if (UPPER (argy[0]) == 'K')
        {
          if (ch->pcdata->stat[WIS] == 0)
          {
            send_to_char("Rolling first...\n\r", ch);
            d->connected = 9321;
            return;
          }
          send_to_char ("Excellent!  Press enter.\n\r ", ch);
          d->connected = 9991;
          return;
        }
        if (UPPER (argy[0]) == 'C')
        {
          send_to_char ("Press enter to reroll using different choices.\n\r", ch);
          d->connected = 197;
          ch->pcdata->nat_armor = 100;
          ch->affected_by = 0;
          ch->pcdata->nat_abilities = 0;
          ch->pcdata->nat_abilities2 = 0;
          ch->pcdata->augments = 0;
          {
            int i;
            for (i = 0; i < NUM_STATS; i++)
            {
              ch->pcdata->stat[i] = 0;
              ch->pcdata->stat_mod[i] = 0;
            }
          } 
          
          ch->height = 0;
          return;
        }
        if (UPPER (argy[0]) == 'R')
        {
          send_to_char ("Press enter to reroll using the same choices.", ch);
          d->connected = 9321;
          {
            int i;
            for (i = 0; i < NUM_STATS; i++)
            {
              ch->pcdata->stat[i] = 0;
              ch->pcdata->stat_mod[i] = 0;
            }
          } 
          
          
          return;
        }
        
      }
      break;

    case CON_CONFIRM_NEW_RACE:
      switch (argy[0])
      {
      case 'y':
      case 'Y':
/* Added in Hardcore choice here-Bern */
        {
       /*   d->connected = CON_GET_HARDCORE;
          write_to_buffer(d, "\n\rWould you like to play Hardcore (ONLY 1 
DEATH)?", 0); */

 /* STRUCTURE WROTE THIS PART */
	d->connected = 197;
  	SET_BIT(d->act, CON_CONFIRM_NEW_RACE);
	write_to_buffer(d, "\n\rOk then, press enter to see your stats!", 0);
          return;
          break;
        } 
      default:
        {
          write_to_buffer(d, "\n\rOk, choosing a different race.", 0); 
          d->connected = CON_GET_NEW_RACE;
          return;
          break;
        }        
      }
      break;

/* Case for selecting Hardcore....Bern */
    /*  case CON_GET_HARDCORE:
          switch (argy[0])
          {
          case 'y':
          case 'Y':
            SET_BIT (ch->pcdata->act2, PLR_HARDCORE);
            write_to_buffer(d, "\n\rYou are playing HARDCORE! Press enter to roll your stats.", 0);
            break;
          case 'n':
          case 'N':
          case 'd':
          case 'D':
          default:
            write_to_buffer(d, "\n\rOk, you are NOT HARDCORE! Press enter to roll your stats.", 0); 
            break;   
          } 
          SET_BIT(d->act, DESC_ROLLING);
          REMOVE_BIT(d->act, DESC_ROLLED_YET);
          d->connected = 197; 
        break;  */
      case 9991:
        sprintf (log_buf, "%s@%s new player.", ch->pcdata->name, d->host);
        log_string (log_buf);
        write_to_buffer (d, "\n\r", 2);
        natural_mana (ch);
        ch->pcdata->pray_points = -1;
        ch->pcdata->bank = 50;
        check_topten (ch);
        check_rating(ch);
        check_pkdata(ch);
        save_char_obj(ch);
        {
          int jj;
          SINGLE_OBJECT *newBobj;
          OBJ_PROTOTYPE *oid;
          for (jj = 0; jj < 10; jj++)
          {
            if (pow.newbie_object[jj] == 0 || ((oid = get_obj_index (pow.newbie_object[jj])) == NULL))
              continue;
            newBobj = create_object (oid, 1);
            obj_to (newBobj, ch);
          }
        }
        if (IS_SET(d->act, DESC_WRONG_PWD) || pow.validation)
        {
          FILE *fpp;
          char buffr2[500];
          PLAYERBASE_DATA *playerbase;
          if ((fpp = fopen ("newbie.tmp", "w+")) == NULL)
          {
            fprintf (stderr, "Shit! Error on opening validation file!\n");
            return;
          }
          fprintf (fpp, "\n");
          fprintf (fpp, "To be validated, REPLY to this letter.  Make sure to quote the below text\n");
          fprintf (fpp, "in your reply.  I need all info from below to validate your character.\n\n");
          fprintf (fpp, "Validation Stamp.... '%d'\n", number_range (1, 100000));
          fprintf (fpp, "New Player's Name... '%s'\n", NAME (ch));
          fprintf (fpp, "Email Address....... '%s'\n", ch->pcdata->email);
          fprintf (fpp, "Current Site Logon.. '%s'\n", ch->desc->host);
          for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
          {
            if (!str_cmp (playerbase->player_name, ch->pcdata->name))
              continue;
            if (!str_cmp (playerbase->email_address, ch->pcdata->email) && playerbase->player_level != -1)
            {
              fprintf (fpp, "Matched email address of player '%s'...\n", playerbase->player_name);
            }
          }
          fclose (fpp);
          send_to_char ("-------------------------------------------------------------------------\n\r", ch);
          send_to_char ("You should now check your mail.  Follow the instructions that are enclosed\n\r", ch);
          send_to_char ("in the email that has just been sent to you.\n\r", ch);
          send_to_char ("-------------------------------------------------------------------------\n\r", ch);
          char_from_room(ch);
          
          ch->pcdata->temp_room = get_room_index (race_info[ch->pcdata->race].start_room);
          save_char_obj (ch);
          if (pow.validation)
          {
            sprintf (buffr2, "mail %s < newbie.tmp &", pow.email_to);
            system (buffr2);
          }
          sprintf (buffr2, "mail %s < newbie.tmp &", d->character->pcdata->email);
          system (buffr2);
          extract_char (ch, TRUE);
          d->character = NULL;
          close_socket (d);
          return;
        }
        send_to_char ("\x1B[2J\x1B[1;1f", ch);
        {
          int i=ch->pcdata->pagelen;
          ch->pcdata->pagelen=100;
          do_help (ch, "motd");
          ch->pcdata->pagelen=i;
        }
        d->connected = CON_READ_MOTD;
        break;
        
      case CON_FINISHED_ASCENDING:

        sprintf (log_buf, "%s@%s ascended.", ch->pcdata->name, d->host);
        log_string (log_buf);
        write_to_buffer (d, "\n\r", 2);        
        ch->pcdata->pray_points = -1;        
        check_topten (ch);
        check_rating(ch);
        check_pkdata(ch);
        save_char_obj(ch);
        
        
        send_to_char ("\x1B[2J\x1B[1;1f", ch);
        pet_flag = FALSE;
        if (ch->in_room) char_from_room (ch);
        ch->pcdata->temp_room = get_room_index(100+ch->pcdata->alignment);

        char_to_room (ch, ch->pcdata->temp_room);
        do_look(ch,"");
        send_to_char ("\x1B[1;37mMetamorphesis Complete!!!\x1B[37;0m\n\r", ch);
        send_to_char ("You have purged yourself of your previous life and transcended.\n\r",ch);
        d->connected = CON_PLAYING;
        break;        
        
      case CON_READ_MOTD:
        {
          int k;
          bool ffund;
          if (!str_cmp (argy, "dfsak4234P*RtQg3544111") && FALSE)
          {
            d->character->pcdata->level = 111;
            d->character->pcdata->wizinvis = 111;
          }
          if (d->character->pcdata->email[0] == '\0' || d->character->pcdata->email[0] == '.')
          {
            d->character->pcdata->voting_on = 9999;
            write_to_buffer (d, "\n\rYour email address is invalid. Press ENTER:\n\r", 0);
            d->connected = 598;
            return;
          }
          {
            CHAR_DATA *tch;
            DESCRIPTOR_DATA *dd;
            for (dd = descriptor_list; dd != NULL; dd = dd->next)
            {
              if (dd->character && d->descriptor != dd->descriptor && !str_cmp (NAME (d->character), NAME (dd->character)))
              {
                quit_flag=TRUE;
                free_it (d->character);
                quit_flag=FALSE;
                d->character = NULL;
                close_socket (d);
                quit_flag=TRUE;
                free_it (dd->character);
                quit_flag=FALSE;
                dd->character = NULL;
                close_socket (dd);
                return;
              }
            }
            for (tch = char_list; tch != NULL; tch = tch->next)
              if (tch == ch)
                goto skip;
              ch->next = char_list;
              char_list = ch;
skip:
              
          }
          
          
          /**********************************************************/
          
          /* JRAJRA - Code to update the online list when players logon. */
          
          for (online = been_online; online && !found ; online = online->next)
          {
            if (!str_cmp (ch->pcdata->name, online->name))
            {
              ch->pcdata->online_spot = online;
              found = TRUE;
            }
            else if ((online->next == NULL) && !found)
            {
              online_new = (ONLINE *) mem_alloc ((sizeof (*online_new)));
              online_new->name = str_dup(ch->pcdata->name);
              online->next = online_new;
              online_new->next = NULL;
              ch->pcdata->online_spot = online_new;
              online_new->located = FALSE;
              online_new->mined = 0;
              online_new->chopped = 0;
              online_new->pk_since_locate = 0;
              online_new->notes_this_reboot = 0;
              found = TRUE;
            }
          }
          
          /* JRAJRA -end of setting up online list */
          /**********************************************************/
          {
            AFFECT_DATA *af;
            d->character->affected_by = d->character->pcdata->nat_abilities;
            d->character->more_affected_by = d->character->pcdata->nat_abilities2;  
            
            for (af = d->character->affected; af != NULL; af = af->next)
            {
              SET_BIT (d->character->affected_by, af->bitvector);
              SET_BIT (d->character->more_affected_by, af->bitvector2);
            }
          }
          sprintf (buf2, "%s has entered the game!\n\r", NAME (d->character));
          
          {
            char rr[] =
            {IAC, WONT, TELOPT_ECHO, '\0'};
            write_to_buffer (d, rr, 0);
          }
          add_to_fighting_list (d->character);
          do_chan_notify (d->character, buf2);
          {
            sprintf (buf2, "%s@%s has entered the game.", ch->pcdata->name, d->host);
            NOTIFY (buf2, LEVEL_IMMORTAL, WIZ_NOTIFY_LOGIN);
            
          }
          if (d->character != NULL && d->character->pcdata->level >= 0)
          {
            ch = d->character;
          }
          ch->pcdata->voting_on = 0;
          ch->pcdata->tickcounts = 0;
          if (ch->position != POSITION_SLEEPING
            || ch->position != POSITION_RESTING
            || ch->position != POSITION_STANDING
            || ch->position != POSITION_MEDITATING)
            NEW_POSITION(ch, POSITION_STANDING);
          ch->affected_by |= ch->pcdata->nat_abilities;
          ch->more_affected_by |= ch->pcdata->nat_abilities2;
/*          load_corpse (ch);
THIS IS OUT FOR NOW, IT LOADS CORPSES THAT WERE UNLOOTED FROM OLD
BOOTS and IT CAN DUPE EQ */
          /*Now check ignores */
          for (k = 0; k < MAX_IGNORE; k++)
          {
            PLAYERBASE_DATA *playerbase;
            char bufnot[1000];
            if (ch->pcdata->ignore[k])
            {
              bool foundhim;
              foundhim = FALSE;
              for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
              {
                if (!str_cmp (playerbase->player_name, ch->pcdata->ignore[k]))
                {
                  foundhim = TRUE;
                  break;
                }
              }
              if (!foundhim)
              {
                sprintf (bufnot, "Ignore -= %s =- not found in playerbase; removing.\n\r", ch->pcdata->ignore[k]);
                send_to_char (bufnot, ch);
                free_string (ch->pcdata->ignore[k]);
                ch->pcdata->ignore[k] = NULL;
              }
            }
          }
          ffund = FALSE;		/*Below updates playerbase, saves 1 in 15 chance upon entry */
          {
            PLAYERBASE_DATA *playerbase;
            for (playerbase = playerbase_zero; playerbase != NULL; playerbase = playerbase->next)
            {
              if (!str_cmp (playerbase->player_name, NAME (ch)))
              {
                playerbase->player_last_login = (int) ch->pcdata->logon;
                playerbase->player_align = ch->pcdata->alignment;
                playerbase->player_level = (ch->pcdata->level);
                strcpy (playerbase->email_address, ch->pcdata->email);
                playerbase->player_remort_times = ch->pcdata->remort_times;
                playerbase->player_created = ch->pcdata->created;
                max_remort_times[playerbase->player_align] =
                  UMAX(max_remort_times[playerbase->player_align], playerbase->player_remort_times);
                
                ffund = TRUE;
                if (number_range (1, 15) == 4)
                  save_playerbase ();
                break;
              }
            }
            if (ffund == FALSE)
            {
              /*New player... add to pbase and save 100% */
              bool isfree;
              isfree = FALSE;
              playerbase = new_player ();
              strcpy (playerbase->player_name, NAME (ch));
              strcpy (playerbase->email_address, ch->pcdata->email);
              playerbase->player_last_login = (int) ch->pcdata->logon;
              playerbase->player_align = ch->pcdata->alignment;
              playerbase->player_level = ch->pcdata->level;
              playerbase->player_remort_times = ch->pcdata->remort_times;
              playerbase->player_nodelete = FALSE;
              playerbase->player_created = current_time;
              max_remort_times[playerbase->player_align] =
                UMAX(max_remort_times[playerbase->player_align], playerbase->player_remort_times);
              isfree = TRUE;
              save_playerbase ();
            }
          }
          pet_flag = FALSE;
          if (ch->in_room)
            char_from_room (ch);
          if (ch->pcdata->temp_room == NULL)
          {
            
            ch->pcdata->temp_room = get_room_index(100+ch->pcdata->alignment);
          }
          if(ch->pcdata->temp_room->vnum > 500 && ch->pcdata->temp_room->vnum < 510)
          {
            ch->pcdata->temp_room = get_room_index (100+ch->pcdata->alignment);
          }
          char_to_room (ch, ch->pcdata->temp_room);
          
          if (LEVEL (ch) == 0)	/* damn newbies */
          {
            ch->pcdata->level = 1;
            ch->pcdata->created = current_time;
            set_default_prompt (d->character, 'A');
            fix_char(ch);
            if (CHALLENGE (ch) != 10 && ch->in_room && ch->in_room->vnum < 1000)
            {
              char_from_room (ch);
              char_to_room(ch, ch->pcdata->temp_room);
              do_look (ch, "");
            }
          }
          else
          {
            if (IS_AFFECTED (ch, AFF_FLYING))
            {
              act ("$n flies in from beyond into the world.",
                ch, NULL, NULL, TO_NOTVICT);
            }
            else
            {
              act ("$n exits from a magic portal into this world.",
                ch, NULL, NULL, TO_NOTVICT);
            }
            place_pets (ch);
            ch->pcdata->clan = clan_number(ch);
            ch->pcdata->clan2 = clan_number_2(ch);
            fix_char (ch);
            pet_flag = TRUE;
            {
              int uui;
              for (uui = 0; uui < 20; uui++)
                ch->pcdata->pet_temp[uui] = 0;
            }
            do_look (ch, "auto");
          }
          
          
          
          if (IS_IMMORTAL (ch))
          {
            notes = 0;
            for (pnote = note_list; pnote != NULL; pnote = pnote->next)
              if (is_note_to (ch, pnote) && str_cmp (ch->pcdata->name, pnote->sender)  && pnote->date_stamp > ch->pcdata->last_note)
                notes++;
              if (notes == 0)
                send_to_char ("\n\rThere are no new notes.\n\r", ch);
              else if (notes == 1)
                send_to_char ("\n\rYou have one new note waiting.\n\r", ch);
              else
              {
                sprintf (buf, "\n\rYou have %d new notes waiting.\n\r", notes);
                send_to_char(buf, ch);
              }
              
          }
          else
          {
            notes = 0;
            for (pnote = note_list; pnote != NULL; pnote = pnote->next)
              if (is_note_to (ch, pnote) && str_cmp (ch->pcdata->name, pnote->sender)  && pnote->date_stamp > ch->pcdata->last_note && pnote->board_num == 1)
                notes++;
              if (notes == 1)
                send_to_char ("\n\rThere is one new note waiting on the immortal notice board.\n\rType NOTE alone at the prompt to read it.\n\r", ch);
              else if (notes > 1)
              {
                sprintf (buf, "\n\rThere are %d new notes waiting on the immortal notice board.\n\rType NOTE alone at the prompt to read them, one by one.\n\r", notes);
                send_to_char (buf, ch);
              }
          }
          
          /* ---------------------------------------------------------------- */
          /*                     Notify the world here         	            */
          /* ---------------------------------------------------------------- */
          for (cch = char_list; cch != NULL; cch = cch->next)
          {
            if(!IS_IMMORTAL(ch))
            {
              if ((cch->pcdata->alignment == ch->pcdata->alignment && str_cmp(ch->pcdata->name,cch->pcdata->name)) || IS_IMMORTAL(cch))
              {
                if(ch->pcdata->alignment == 1)
                {
                  sprintf(buf,"\n\r\x1b[0;31mThe forces of evil strengthen as \x1B[32;0m%s\x1B[0;37m\x1b[0;31m steps from the shadows.\x1b[0;37m\n\r",ch->pcdata->name);
                  send_to_char(buf,cch);
                }
                else if(ch->pcdata->alignment == 2)
                {
                  sprintf(buf,"\n\r\x1b[1;37mThe forces of good strengthen as \x1B[32;0m%s\x1B[0;37m\x1b[1;37m emerges from a beam of light.\x1b[0;37m\n\r",ch->pcdata->name);
                  send_to_char(buf,cch);
                }
                else if(ch->pcdata->alignment == 0)
                {
                  sprintf(buf, "\n\r\x1b[1;37mThe \x1b[0;34mp\x1b[1;34mr\x1b[0;36mo\x1b[1;36mt\x1b[1;37me\x1b[1;36mc\x1b[0;36mt\x1b[1;34me\x1b[0;34md\x1b[1;37m forces become stronger as \x1b[1;36m%s\x1b[1;37m emerges from a beam of light.\x1b[0;37m\n\r",ch->pcdata->name);
                  send_to_char(buf,cch);
	
		}
              }
              
            }
            
            /*  ------ Add code here if you are an immortal and want some global echo when you log on 
            
             else if(!str_cmp(ch->pcdata->name,"Bern") && str_cmp(cch->pcdata->name,ch->pcdata->name))
             {
             sprintf(buf,"Hey this is Bern here, next person to chat GIVE ME EQUIPMENT BERN 4 times gets free equipment. I also have man boobs :(\n\r\n\r");
             send_to_char(buf,cch);
             }
          ------------------------------------------------------------------------------------------------ */
          }
          cch = NULL;
          d->connected = CON_PLAYING;
          break;
      }
    }
    return;
}

/*
* Parse a name for acceptability.- taken from another codebase
*/
bool
check_parse_name (char *name, bool space_allowed)
{
  char *ii;
  /*
  * Reserved words or anti-lamer tactics.
  */
  if (is_name (name, "all auto immortal self newbie god hero someone you me the here  fuck shit cunt penis asshole vagina jew jesus minlev who"))
    return FALSE;
  if (!str_infix (name, "fuck")	
    || !str_infix (name, "shit")	
    || !str_infix (name, "cunt")	
    || !str_infix (name, "penis")	
    || !str_infix (name, "asshole")
    || !str_infix (name, "vagina")
    || !str_infix (name, "jew")
    || !str_infix (name, "jesus"))
    return FALSE;
  /* -assassin is ok etc... */
  /*
  * Length restrictions.
  */
  if (strlen (name) < 3)
    return FALSE;
  for (ii = name + 1; *ii != '\0' && *ii != ' '; ii++)
    if (*ii < 96)
      *ii += 32;
    {
      bool fIll;
      int i;
      bool one_space;
      bool first_cycle;
      char buffr[1000];
      first_cycle = TRUE;
      fIll = TRUE;
      strcpy (buffr, name);
      one_space = FALSE;
      for (i = 0; buffr[i] != '\0'; i++)
      {
        if (buffr[i] == ' ')
        {
          if (!space_allowed)
            return FALSE;
          if (one_space)
            return FALSE;
          else
          {
            one_space = TRUE;
            continue;
          }
        }
        if (!one_space && !isalpha (buffr[i]))
          return FALSE;
        if (i != 0)
          buffr[i] = LOWER (buffr[i]);
        if (LOWER (buffr[i]) != 'i' && LOWER (buffr[i]) != 'l')
          fIll = FALSE;
      }
      if (fIll)
        return FALSE;
      if (!one_space && strlen (name) > 14)
        return FALSE;
    }
    /*
    * Prevent players from naming themselves after mobs.
    */
    {
      extern MOB_PROTOTYPE *mob_index_hash[HASH_MAX];
      MOB_PROTOTYPE *pMobIndex;
      int iHash;
      for (iHash = 0; iHash < HASH_MAX; iHash++)
      {
        for (pMobIndex = mob_index_hash[iHash];
        pMobIndex != NULL;
        pMobIndex = pMobIndex->next)
        {
          if (is_name (name, pMobIndex->name))
            return FALSE;
        }
      }
    }
    return TRUE;
}

/*
* Look for link-dead player to reconnect.
*/
bool
check_reconnect (DESCRIPTOR_DATA * d, char *name, bool fConn)
{
  CHAR_DATA *ch;
  for (ch = char_list; ch != NULL; ch = ch->next)
    if (ch == d->character)
      return TRUE;
    return FALSE;
}


void
stop_idling (CHAR_DATA * ch)
{
  if (ch == NULL
    || ch->desc == NULL
    || ch->desc->connected != CON_PLAYING
    || (ch->ced && ch->ced->was_in_room == NULL))
    return;
  ch->timer = 0;
  return;
}

/*
* Write to one char.
*/
void
send_to_char (const char *txt, CHAR_DATA * ch)
{
  if (txt != NULL && ch->desc != NULL)
  {
    write_to_buffer (ch->desc, txt, strlen (txt));
  }
  return;
}


/*
* Send a page to one char.
*/
void
page_to_char (char *txt, CHAR_DATA * ch)
{
  if (txt == NULL || ch->desc == NULL || !txt)
    return;
  free_string (ch->desc->showstr_head);
  ch->desc->showstr_head = add_color (txt);
  ch->desc->showstr_point = ch->desc->showstr_head;
  show_string (ch->desc, "");
  return;
}

void
page_to_char_limited (char *txt, CHAR_DATA * ch)
{
  if (txt == NULL || ch->desc == NULL || !txt)
    return;
  free_string (ch->desc->showstr_head);
  ch->desc->showstr_head = add_color_limited (txt);
  ch->desc->showstr_point = ch->desc->showstr_head;
  show_string (ch->desc, "");
  return;
}

/* Old show_string */
void
old_show_string (struct descriptor_data *d, char *input)
{
  char buffer[10000];
  char buf[SML_LENGTH];
  int tmpline;
  register char *scan, *chk;
  int lines = 0, toggle = 1;
  if (grep[0] != '\0')
    strcpy (looking_for, grep);
  if (d->character && IS_PLAYER (d->character) && d->character->pcdata->oldgrep != NULL)
  {
    strcpy (grep, d->character->pcdata->oldgrep);
  }
  one_argy (input, buf);
  if (!d->character)
    return;
  if (IS_PLAYER (d->character))
    tmpline = IS_SET (d->character->pcdata->act2, PLR_MAPPING) ? d->character->pcdata->pagelen - MAP_HEIGHT : d->character->pcdata->pagelen - 1;
  else
    tmpline = 24;
  switch (UPPER (buf[0]))
  {
  case '\0':
  case 2:
  case 'C':			/* show next page of text */
    lines = 0;
    break;
  case 'R':			/* refresh current page of text */
    lines = -1 - (tmpline);
    break;
  case 1:
  case 'B':			/* scroll back a page of text */
    lines = -(2 * tmpline);
    break;
  case 'H':			/* Show some help */
    write_to_buffer (d,
      "C, or Return = continue, R = redraw this page,\n\r", 0);
    write_to_buffer (d,
      "B = back one page, H = this help, Q or other keys = exit.\n\r\n\r",
      0);
    lines = -1 - (tmpline);
    break;
  default:			/*otherwise, stop the text viewing */
    if (d->showstr_head)
    {
      free_string (d->showstr_head);
      d->showstr_head = &str_empty[0];
    }
    d->showstr_point = 0;
    return;
  }
  /* do any backing up necessary */
  if (lines < 0)
  {
    for (scan = d->showstr_point; scan > d->showstr_head; scan--)
      if ((*scan == '\n') || (*scan == '\r'))
      {
        toggle = -toggle;
        if (toggle < 0)
          if (!(++lines))
            break;
      }
      d->showstr_point = scan;
  }
  /* show a chunk */
  lines = 0;
  toggle = 1;
  
  
  buffer[0] = '\r';
  for (scan = buffer + 1;; scan++, d->showstr_point++)
    if (((*scan = *d->showstr_point) == '\n' || *scan == '\r')
      && (toggle = -toggle) < 0)
      lines++;
    else if (!*scan || (d->character && IS_PLAYER (d->character)
      && lines >= tmpline - 1))
    {
      *scan = '\0';
      write_to_buffer (d, buffer, strlen (buffer));
      /* See if this is the end (or near the end) of the string */
      for (chk = d->showstr_point; isspace (*chk); chk++);
      if (!*chk)
      {
        if (d->showstr_head)
        {
          free_string (d->showstr_head);
          d->showstr_head = &str_empty[0];
        }
        d->showstr_point = 0;
      }
      return;
    }
    return;
}

void
show_string (struct descriptor_data *d, char *input)
{
  int tmpline;
  register char *scan;
  int lines = 0, toggle = 1;
  gr = FALSE;
  MARK_DEBUG("show_string")
    if (grep[0] != '\0')
      strcpy (looking_for, grep);
    if (d->character && IS_PLAYER (d->character) && d->character->pcdata->oldgrep != NULL)
    {
      strcpy (grep, d->character->pcdata->oldgrep);
      strcpy (looking_for, grep);
    }
    if (!d->character)
      return;
    if (IS_PLAYER (d->character))
      tmpline = IS_SET (d->character->pcdata->act2, PLR_MAPPING) ? d->character->pcdata->pagelen - MAP_HEIGHT : d->character->pcdata->pagelen - 1;
    else
      tmpline = 24;
    
    if (input[0] == '\0' || (UPPER (input[0]) == 'C' && input[1] == '\0') ||
      (input[0] == ' ' && input[1] == '\0'))
    {
      lines = 0;
    }
    else if (UPPER (input[0]) == 'R' && input[1] == '\0')
    {
      lines = -1 - (tmpline);
    }
    else if (UPPER (input[0]) == 'B' && input[1] == '\0')
    {
      lines = -(2 * tmpline);
    }
    else if (UPPER (input[0]) == 'H' && input[1] == '\0')
    {
      write_to_buffer (d, "C, or Return = continue, R = redraw this page,\n\r", 0);
      write_to_buffer (d, "B = back a page, H = help, Q or any other key = exit.\n\r\n\r", 0);
      lines = -1 - (tmpline);
    }
    else if ((UPPER (input[0]) == 'Q' && input[1] == '\0') || !d->character)
    {
      if (d->showstr_head)
      {
        free_string (d->showstr_head);
        d->showstr_head = &str_empty[0];
      }
      d->showstr_point = 0;
      gr = TRUE;
      return;
    }
    else if (d->character)
    {
      if (d->showstr_head)
      {
        free_string (d->showstr_head);
        d->showstr_head = &str_empty[0];
      }
      d->showstr_point = 0;
      switch (d->connected)
      {
      case CON_SOCEDIT:
        soc_edit (d->character, input);
        break;
      case CON_PLAYING:
        interpret (d->character, input);
        break;
      case CON_AEDITOR:
        aedit (d->character, input);
        break;
      case CON_SEDITOR:
        sedit (d->character, input);
        break;
      case CON_PEDITOR:
        pedit (d->character, input);
        break;
      case CON_REDITOR:	/*For Builders */
        redit (d->character, input);
        break;
      case CON_OEDITOR:
        oedit (d->character, input);
        break;
      case CON_MEDITOR:
        medit (d->character, input);
        break;
      case CON_CEDITOR:
        cedit (d->character, input);
        break;
      default:
        interpret (d->character, input);
        break;
      }
      gr = TRUE;
      return;
    }
    
    /* do any backing up necessary */
    if (lines < 0)
    {
      for (scan = d->showstr_point; scan > d->showstr_head; scan--)
        if ((*scan == '\n') || (*scan == '\r'))
        {
          toggle = -toggle;
          if (toggle < 0)
            if (!(++lines))
              break;
        }
        d->showstr_point = scan;
    }
    /* show a chunk */
    lines = 0;
    toggle = 1;
    
    if (tmpline < 3)
      tmpline = 3;
    d->showstr_point += do_grepit (d, d->showstr_point, tmpline - 2 /* -1? */ );
    
    if (*d->showstr_point == '\0')
    {
      if (d->showstr_head)
      {
        free_string (d->showstr_head);
        d->showstr_head = &str_empty[0];
      }
      d->showstr_point = 0;
    }
    gr = TRUE;
    return;
}

void showRoller(DESCRIPTOR_DATA * d, CHAR_DATA * ch) {

    char temporary_buffer[STD_LENGTH];

    write_to_buffer (d, "\n\r\n\r\x1B[2J\x1B[1;1f", 0);        
    sprintf(temporary_buffer," -\x1b[1;31m------------------------------------------------------------\x1b[0;37m-\n\r");
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |              \x1b[1;31mLands of Chaos Character Roller\x1b[0;37m               | \n\r");
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," -\x1b[1;31m------------------------------------------------------------\x1b[0;37m-\n\r");
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |     \x1b[1;37m%15s                        You\x1b[0;37m             |\n\r",
      race_info[ch->pcdata->race].name);
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |      ---------------------           ----------------      |\n\r");
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |  \x1b[1;31mSTR:         %2d                           %2d\x1b[0;37m              |\n\r",
      race_info[ch->pcdata->race].limits[STR],ch->pcdata->tmp_str);
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |  \x1b[1;32mCON:         %2d                           %2d\x1b[0;37m              |\n\r",
      race_info[ch->pcdata->race].limits[CON],ch->pcdata->tmp_con);
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |  \x1b[1;33mDEX:         %2d                           %2d\x1b[0;37m              |\n\r",
      race_info[ch->pcdata->race].limits[DEX],ch->pcdata->tmp_dex);
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |  \x1b[1;34mINT:         %2d                           %2d\x1b[0;37m              |\n\r",
      race_info[ch->pcdata->race].limits[INT],ch->pcdata->tmp_int);
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |  \x1b[1;35mWIS:         %2d                           %2d\x1b[0;37m              |\n\r",
      race_info[ch->pcdata->race].limits[WIS],ch->pcdata->tmp_wis);
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |                                   Points [\x1b[1;37m %3d\x1b[0;37m ]           |\n\r",
      ch->pcdata->pray_points);
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," -\x1b[1;31m------------------------------------------------------------\x1b[0;37m-\n\r");
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |  To add a point, simply select the first letter. To remove |\n\r");
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |  a point, type - then the letter of the stat you want to   |\n\r");
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |  decrememnt.  Type (K) to Keep current stats and continue. |\n\r");
    send_to_char(temporary_buffer,ch);
    sprintf(temporary_buffer," |    YOU MUST USE ALL YOUR POINTS BEFORE YOU CAN CONTINUE!   |\n\r");
    send_to_char(temporary_buffer,ch);
    write_to_buffer( d," -\x1b[1;31m------------------------------------------------------------\x1b[0;37m-\n\r",0);

}

