
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
#include <fcntl.h>

#ifdef netBSD
#include <unistd.h>
#endif
#include "emlen.h"
#ifdef Linux
#include "mush.h"
#endif

#ifndef WINDOWS
#include <termio.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#undef ioctl
#include <sys/ioctl.h>

/* ain't workin - screw this*/

int route_io (int, int);
int open_pty_master (char *);
int open_pty_slave (int, const char *);


#define DEBUG 1

const int ST_DATA = 0;
const int ST_IAC = 1;
const int ST_SE = 2;
const int ST_OPT = 3;
const int ST_HOW = 4;
const int ST_WILL = 5;
const int ST_WONT = 6;
const int ST_DO = 7;
const int ST_DONT = 8;
const int ST_CRLF = 9;
const int ST_IP = 10;

int 
route_io (int fd1, int fd2)
{
  return 0;
}

int 
open_pty_master (char *pty)
{

  return -1;
}


int 
open_pty_slave (int master_fd, const char *pty)
{
      return -1;


}
/*#endif */
#endif
