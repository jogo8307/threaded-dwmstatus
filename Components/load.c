#include "util.h"
#include <stdlib.h>
#include "components.h"

char * loadavg(void)
{
	double avgs[3];

	if (getloadavg(avgs, 3) < 0)
		return smprintf("");

	return smprintf("%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}


//Compile with gcc temperature.c -o temperature
//int main(void)
//{
//    long vol = -1;
//    char* volstr;
//    volstr = loadavg(); 
//    printf("Master string:  %s\n", volstr);
//free(volstr);
//
//    return 0;
//}
