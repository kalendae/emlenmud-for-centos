#define VERSION_STR "DikuMUD by Hans Staerfeldt, Katja Nyboe, Tom Madsen,
Michael Seifert, and Sebastian Hammer. MERC 2.1 by Hatchet, Furey, and
Kahn. NiMUD by Locke. EmlenMud by Owen Emlen. Loc 2.0 by Lonath and 
Flatline.\n\r\n\rWelcome to The Lands of Chaos!\n\r" 
/*
#define VERSION_STR "DikuMUD by Hans Staerfeldt, Katja Nyboe, Tom Madsen, 
Michael Seifert, and Sebastian Hammer. Emlenmud (Owen Emlen),  Lands of  
Chaos: Flatline Lonath\n\r\n\rWelcome to The Lands of Chaos\n\r"
*/

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif


#if defined(unix)
#include <signal.h>
#endif

#ifndef WINDOWS
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>

#endif


/*int read  ( int fd, char *buf, int nbyte  );
int select  ( int width, fd_set *readfds, fd_set *writefds,
fd_set *exceptfds, struct timeval *timeout  );
int write  ( int fd, char *buf, int nbyte  );
int setsockopt  ( int s, int level, int optname, caddr_t optval,
int optlen  );
int socket  ( int domain, int type, int protocol  );
int write  ( int fd, char *buf, int nbyte  );
*/

#if defined(sun)
int accept  ( int s, struct sockaddr *addr, int *addrlen  );
int bind  ( int s, struct sockaddr *name, int namelen  );
void bzero  ( char *b, int length  );
int close  ( int fd  );
int getpeername  ( int s, struct sockaddr *name, int *namelen  );
int getsockname  ( int s, struct sockaddr *name, int *namelen );
int listen  ( int s, int backlog  );
/*int read  ( int fd, char *buf, int nbyte  );
*/int select  ( int width, fd_set *readfds, fd_set *writefds,
fd_set *exceptfds, struct timeval *timeout  );

#if defined(SYSV)
int setsockopt  ( int s, int level, int optname, 
const char *optval, int optlen  );
#else 
/*int setsockopt  ( int s, int level, int optname, void *optval,
int optlen  );*/
#endif

/*int socket  ( int domain, int type, int protocol  );*/
/*int write  ( int fd, char *buf, int nbyte  );
*/#endif

