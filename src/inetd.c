// mostly borrowed from freebie code

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emlen.h"
#ifdef WINDOWS

void start_auth (struct descriptor_data *d) {return;}
void read_auth (struct descriptor_data *d) {return;}
void send_auth (struct descriptor_data *d) {return;}

#else

#include "mush.h"
#include <sys/types.h>
#include <ctype.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/telnet.h>
#include <errno.h>
#include <netdb.h>
void nonblock (int s);
void start_auth (struct descriptor_data *d);
void read_auth (struct descriptor_data *d);
void send_auth (struct descriptor_data *d);


void 
start_auth (struct descriptor_data *d)
{
  struct sockaddr_in sock;
  int err;			/* error & result stuffs */
  int tlen;

  /*if ( !str_prefix( "130.63.236", d->host ) 
     || !str_prefix( "130.63", d->host ) )
     {
     free_string(d->user);
     d->user = str_dup( "(ncsa format)" );
     return;
     } */

  d->auth_fd = socket (AF_INET, SOCK_STREAM, 0);
  err = errno;

  if (d->auth_fd < 0 && err == EAGAIN)
    bug ("Can't allocate fd for authorization check", 0);
  nonblock (d->auth_fd);

  /* Clone incoming host address */
  tlen = sizeof (sock);
  getpeername (d->descriptor, (struct sockaddr *) &sock, &tlen);
  sock.sin_port = htons (113);
  sock.sin_family = AF_INET;

  if ((connect (d->auth_fd, (struct sockaddr *) &sock, sizeof (sock)) == -1)
      && (errno != EINPROGRESS))
    {
      /* Identd Denied */
      close (d->auth_fd);
      free_string (d->username);
      d->username = str_dup ("no_inetd");
      d->auth_fd = -1;
      d->auth_state = 0;
      d->atimes = 500;
      return;
    }

  if (errno == ECONNREFUSED)
    {
      close (d->auth_fd);
      d->auth_fd = -1;
      free_string (d->username);
      d->username = str_dup ("no_inetd");
      d->auth_state = 0;
      d->atimes = 500;
      return;
    }

  d->auth_state |= (WRITE_AUTH | AUTH);		/* Successful, but not sent */
  return;
}

/* send_auth */

void 
send_auth (struct descriptor_data *d)
{
  struct sockaddr_in us, them;
  char authbuf[32];
  int ulen, tlen, z;

  tlen = ulen = sizeof (us);

  if (getsockname (d->descriptor, (struct sockaddr *) &us, &ulen)
      || getpeername (d->descriptor, (struct sockaddr *) &them, &tlen))
    {
      goto authsenderr;
    }

  /* compose request */
  sprintf (authbuf, "%u , %u\r\n",
	   (unsigned int) ntohs (them.sin_port),
	   (unsigned int) ntohs (us.sin_port));

/*  
   sprintf( log_buf, "sending [%u , %u] to auth port %d:113",
   (unsigned int)ntohs(them.sin_port),
   (unsigned int)ntohs(us.sin_port), d->auth_fd );
   log( log_buf, 0 );
 */

  nonblock (d->auth_fd);
  z = write (d->auth_fd, authbuf, strlen (authbuf));

  if (errno == ECONNREFUSED || (z == -1 && d->atimes > 10))
    {
      close (d->auth_fd);
      d->auth_fd = -1;
      free_string (d->username);
      d->username = str_dup ("no_identd");
      d->auth_state = 0;
      return;
    }

  if (z != strlen (authbuf))
    {
      if (d->atimes >= 15)
	{
	  sprintf (log_buf, "auth request, broken pipe [%d/%d]", z, errno);
	  bug (log_buf, 0);
	  close (d->auth_fd);
	  d->auth_fd = -1;
	  d->auth_state -= WRITE_AUTH;
	  d->auth_state &= ~AUTH;
	  d->auth_inc = 0;
	}
    authsenderr:
      d->atimes++;
      return;
    }
  d->auth_state -= WRITE_AUTH;	/* Successfully sent request */
  d->atimes = 0;
  return;
}

/* read_auth */
void 
read_auth (struct descriptor_data *d)
{
  char *s, *t;
  int len;			/* length read */
  char ruser[100], system[100];	/* remote userid */
  u_short remp = 0, locp = 0;	/* remote port, local port */
  *system = *ruser = '\0';

  /*
   * Can't allow any other reads from client fd while waiting on the
   * authfd to return a full valid string.  Use the client's input buffer
   * to buffer the authd reply.  May take more than one read.
   */

  nonblock (d->auth_fd);
  if ((len = read (d->auth_fd, d->abuf + d->auth_inc,
		   sizeof (d->abuf) - 1 - d->auth_inc)) >= 0)
    {
      d->auth_inc += len;
      if (d->auth_inc > 90)
	{
	  close (d->auth_fd);
	  d->auth_state = 0;
	  d->auth_fd = -1;
	  free_string (d->username);
	  d->username = str_dup ("no_identd");
	  d->atimes = 500;
	  return;
	}
      d->abuf[d->auth_inc] = '\0';
    }
  if (d->atimes < 20)
    {
      d->atimes++;
      return;
    }
  if (d->abuf[0] != '\0')

    if (
	 (sscanf (d->abuf, "%hd , %hd : USERID : %*[^:]: %10s",
		  &remp, &locp, ruser) == 3))
      {
	s = rindex (d->abuf, ':');
	*s++ = '\0';
	for (t = (rindex (d->abuf, ':') + 1); *t; t++)
	  if (!isspace (*t))
	    break;
	strncpy (system, t, sizeof (system));

	if (!str_prefix ("OTHER", system))
	  {
	    close (d->auth_fd);
	    d->auth_state = 0;
	    d->auth_fd = -1;
	    free_string (d->username);
	    d->username = str_dup ("no_identd");
	    d->atimes = 500;
	    return;
	  }

	for (t = ruser; *s && (t < ruser + sizeof (ruser)); s++)
	  if (!isspace (*s) && *s != ':')
	    *t++ = *s;
	*t = '\0';

	sprintf (log_buf, "auth reply ok, incoming user: [%s]", ruser);
	log_string (log_buf);
      }
    else if (len != 0)
      {
	if (!index (d->abuf, '\n') && !index (d->abuf, '\r'))
	  return;
	sprintf (log_buf, "bad auth reply: %s", d->abuf);
	*ruser = '\0';
      }
  close (d->auth_fd);
  d->auth_inc = 0;
  *d->abuf = '\0';
  d->auth_fd = -1;
  d->auth_state = 0;
  if (ruser[0] == '\0')
    strcpy (ruser, "no_inetd");
  free_string (d->username);
  d->username = str_dup (ruser);
  return;
}

#ifdef WINDOWS
void 
nonblock (int s)
{
  if (fcntl (s, F_SETFL, FNDELAY) == -1)
    {
      perror ("Noblock");
/*    exit(2); */
      bug ("Noblock", 0);
    }
}
#endif

#endif
