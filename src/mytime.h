#include <sys/times.h>
#include <sys/types.h>
#include <sys/resource.h>
  
double mytime(void) {

     struct rusage resources;
     int who = RUSAGE_SELF;   

     int ierr;       
/*     int i;*/

     double utime;   
     double stime;   

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

      utime = (double) (resources.ru_utime.tv_sec*1000000) +
     (double) resources.ru_utime.tv_usec;

      stime = (double) (resources.ru_stime.tv_sec*1000000) +
     (double) resources.ru_stime.tv_usec;

      return (utime + stime);
}

