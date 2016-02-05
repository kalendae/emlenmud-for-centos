
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef netBSD
#include <unistd.h>
#endif
#include "emlen.h"

#ifndef WINDOWS
#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#else
#include <winsock.h>
#endif

char bfy[1000];

bool 
read_from_descriptor (DESCRIPTOR_DATA * d)
{
  char *t;
  int ct = 0;
  int iStart;
  int nRead;
  char minbuf[100];
  int tmpi;
/* wait if command already here */
  if (d->incomm[0] != '\0')
    return TRUE;
  iStart = strlen (d->inbuf);
  tmpi = iStart;
  if (iStart >= sizeof (d->inbuf) - 100)
    {
      sprintf (log_buf, "%s input overflow!", d->host);
      log_string (log_buf);
      write_to_descriptor (d->descriptor,
			   "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
      return FALSE;
    }

  for (;;)
    {
#ifdef WINDOWS
    nRead = recv(d->descriptor, d->inbuf+iStart, /*space_left*/500, 0);
      if (nRead<0 && WSAGetLastError() != WSAEWOULDBLOCK) {
		  d->inbuf[0]='\0';
		  fprintf(stderr,"EWOULDBLOCK.\n");
		  return FALSE;
	  }
	  
#else
      nRead = read (d->descriptor, d->inbuf + iStart,
		    500);
#endif
      if (nRead >= 500 && find_connection_num (d) < 0)
	{
	  fprintf (stderr, "Input too large.\n");
	  d->inbuf[0] = '\0';
	  return FALSE;
	}
      if (nRead > 0)
	{
	  iStart += nRead;
	  if (d->host[29] == 1 && d->connected != CON_FSEDITOR &&
	      d->connected != CON_GET_OLD_PASSWORD && d->connected != CON_GET_NEW_PASSWORD &&
	      d->connected != CON_CONFIRM_NEW_PASSWORD)
	    {
	      d->inbuf[iStart] = '\0';
	      bfy[0] = '\0';
	      ct = 0;
	      for (t = &d->inbuf[iStart - nRead]; *t != '\0'; t++)
		{
		  if (d->connected == CON_STATE_ONE)
		    {
		      if (*t == 13)
			write_to_descriptor2 (d, "\n\r", 0);
		      if (*t < 'A' || *t > 'z')
			continue;
		    }
		  while (*t == '\b' || *t == 16 || *t == 127)
		    {
		      if (d->character && d->character->pcdata->cursor > 0)
			{
			  d->character->pcdata->cursor--;
			  sprintf (minbuf, "\x1B[%d;%df\x1B[K\x1B[%d;%df",
				   d->character->pcdata->pagelen,
				   d->character->pcdata->cursor,
				   d->character->pcdata->pagelen,
				   d->character->pcdata->cursor);
			  write_to_descriptor2 (d, minbuf, 0);
			}
		      if (ct > 0)
			{
			  ct--;
			}
		      t++;
		      if (*t == '\0')
			goto doin;
		    }
		  if (*t == '\x1B')
		    {
		      t++;
		      if (*t == '\0')
			return TRUE;
		      t++;
		      if (*t == '\0')
			return TRUE;
		      if (*t == 'A')
			{
			  *(t - 2) = '\0';
			  strcat (d->inbuf, "north\n\r");
			  continue;
			}
		      if (*t == 'B')
			{
			  *(t - 2) = '\0';
			  strcat (d->inbuf, "south\n\r");
			  continue;
			}
		      if (*t == 'C')
			{
			  *(t - 2) = '\0';
			  strcat (d->inbuf, "east\n\r");
			  continue;
			}
		      if (*t == 'D')
			{
			  *(t - 2) = '\0';
			  strcat (d->inbuf, "west\n\r");
			  continue;
			}
		    }
		  if (((signed char) *t) < 0)
		    {
		      continue;
		    }
		  bfy[ct] = *t;
		  ct++;
		  bfy[ct] = '\0';
		  if (d->character)
		    {
		      d->character->pcdata->cursor++;
		    }

		  bfy[ct] = '\0';
		}
	    doin:
	      /*write_to_descriptor2(d,bfy,0); */
	      return TRUE;
	    }
	  if (d->character && (!IS_SET (d->character->pcdata->act2, PLR_TELNET_GA)))
	    {
	      if (d->inbuf[iStart - 1] < 17 &&
	      d->inbuf[iStart - 1] != '\b' && d->inbuf[iStart - 1] != '\n' &&
	      d->inbuf[iStart - 1] != '\r' && d->inbuf[iStart - 1] != '\0' &&
		  d->inbuf[iStart - 1] != 13 && d->inbuf[iStart - 1] != 5 &&
		  d->inbuf[iStart - 1] != 6 && d->inbuf[iStart - 1] != 16)
		{
		  d->inbuf[0] = '\0';
		  return TRUE;
		}
	      if (d->inbuf[iStart - 1] == 18)
		{
		  if (nRead > 1 && d->inbuf[iStart - 2] == 18)
		    {
		      d->inbuf[0] = 18;
		      d->inbuf[1] = '\0';
		      character_add (d->character, d->inbuf);
		      d->inbuf[0] = '\0';
		    }
		  d->inbuf[0] = 18;
		  d->inbuf[1] = '\0';
		  character_add (d->character, d->inbuf);
		  d->inbuf[0] = '\0';
		  break;
		}
	      if ((d->inbuf[iStart - 1] == '\b' || d->inbuf[iStart - 1] == 127 || d->inbuf[iStart - 1] == 16) && d->connected == CON_FSEDITOR)
		{
		  if (nRead > 1 && (d->inbuf[iStart - 2] == '\b' || d->inbuf[iStart - 2] == 127))
		    {
		      d->inbuf[0] = 18;
		      d->inbuf[1] = '\0';
		      character_add (d->character, d->inbuf);
		      d->inbuf[0] = '\0';
		    }
		  d->inbuf[0] = 9;
		  d->inbuf[1] = '\0';
		  character_add (d->character, d->inbuf);
		  d->inbuf[0] = '\0';
		  break;
		}
	      if (d->inbuf[iStart - 1] == '@')
		{
		  d->inbuf[1] = '\n';
		  d->inbuf[2] = '\r';
		  d->inbuf[3] = '\0';
		  if (d->connected == CON_FSEDITOR)
		    {
		      d->inbuf[1] = '\0';
		      character_add (d->character, d->inbuf);
		      d->inbuf[0] = '\0';
		      break;
		    }
		  break;
		}
	      if (iStart > 1)
		{
		  if (		/*Up arrow */
		       d->inbuf[0] == 27 &&
		       d->inbuf[1] == '[' &&
		       d->inbuf[2] == 'A')
		    {
		      d->inbuf[0] = 1;
		      d->inbuf[1] = '\n';
		      d->inbuf[2] = '\r';
		      d->inbuf[3] = '\0';
		      if (d->connected == CON_FSEDITOR)
			{
			  d->inbuf[1] = '\0';
			  character_add (d->character, d->inbuf);
			  d->inbuf[0] = '\0';
			  break;
			}
		      break;
		    }
		  if (		/*Down arrow */
		       d->inbuf[0] == 27 &&
		       d->inbuf[1] == '[' &&
		       d->inbuf[2] == 'B')
		    {
		      d->inbuf[0] = 2;
		      d->inbuf[1] = '\n';
		      d->inbuf[2] = '\r';
		      d->inbuf[3] = '\0';
		      if (d->connected == CON_FSEDITOR)
			{
			  d->inbuf[1] = '\0';
			  character_add (d->character, d->inbuf);
			  d->inbuf[0] = '\0';
			  break;
			}
		      break;
		    }
		  if (		/*Right arrow */
		       d->inbuf[0] == 27 &&
		       d->inbuf[1] == '[' &&
		       d->inbuf[2] == 'C')
		    {
		      d->inbuf[0] = 3;
		      d->inbuf[1] = '\n';
		      d->inbuf[2] = '\r';
		      d->inbuf[3] = '\0';
		      if (d->connected == CON_FSEDITOR)
			{
			  d->inbuf[1] = '\0';
			  character_add (d->character, d->inbuf);
			  d->inbuf[0] = '\0';
			  break;
			}
		      break;
		    }
		  if (		/*Left arrow */
		       d->inbuf[0] == 27 &&
		       d->inbuf[1] == '[' &&
		       d->inbuf[2] == 'D')
		    {
		      d->inbuf[0] = 4;
		      d->inbuf[1] = '\n';
		      d->inbuf[2] = '\r';
		      d->inbuf[3] = '\0';
		      if (d->connected == CON_FSEDITOR)
			{
			  d->inbuf[1] = '\0';
			  character_add (d->character, d->inbuf);
			  d->inbuf[0] = '\0';
			  break;
			}
		      break;
		    }
		}
	      if (d->inbuf[iStart - 2] == '\n' || d->inbuf[iStart - 2] == '\r')
		{
		  if (d->connected == CON_FSEDITOR)
		    {
		      character_add (d->character, "\n\r");
		      d->inbuf[0] = '\0';
		      return TRUE;
		    }
		  break;
		}
	      d->inbuf[iStart] = '\0';
	      if (d->character && !IS_SET (d->character->pcdata->act2, PLR_TELNET_GA) &&
		  d->connected <= CON_PLAYING)
		{
		  /*          if (*d->inbuf+iStart-nRead>26)
		     write_to_descriptor2(d,d->inbuf+iStart-nRead,0); */
		  if (d->connected == CON_FSEDITOR)
		    {
		      character_add (d->character, d->inbuf);
		      d->inbuf[0] = '\0';
		      break;
		    }
		}
	    }
	  if (d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r')
	    break;
	}
      else if (nRead == 0 || nRead > 1000)
	{
	  char buf[STD_LENGTH];
	  /*log_string( "EOF encountered on read." ); */
	  sprintf (buf, "Notify> %s has lost link.", (d != NULL && d->character != NULL)
		   ? d->character->pcdata->name : "New_char");
	  NOTIFY (buf, LEVEL_IMMORTAL, WIZ_NOTIFY_LOGIN);
	  return FALSE;
	}
#ifdef WINDOWS
	d->inbuf[iStart] = '\0';
	return TRUE;
#else
      else if (errno == EWOULDBLOCK)
	break;
	        else
#endif

	{
	  perror ("Read_from_descriptor");
	  return FALSE;
	}
    }
  d->inbuf[iStart] = '\0';
  return TRUE;
}
