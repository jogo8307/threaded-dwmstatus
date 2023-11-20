
//#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "components.h"

char * getTime(const char *fmt,const char *tzname)
{
  char buf[129];
  time_t tim;
  struct tm *timtm;

  if ( tzname != NULL )
  {
    setenv("TZ", tzname, 1);
  }
  tim = time(NULL);
  timtm = localtime(&tim);
  if (timtm == NULL)
    return smprintf("");

  if ( (timtm == NULL) || (strftime(buf, sizeof(buf) - 1, fmt, timtm)) == 0) 
  {
    fprintf(stderr, "timtm == NULL || strftime == 0\n");
    return smprintf("");
  }

  return smprintf("%s", buf);
}
