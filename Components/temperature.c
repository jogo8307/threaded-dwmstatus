/*
 * gettemperature("/sys/class/hwmon/hwmon0", "temp1_input");
 */
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include "components.h"
char* gettemperature(const char *base, const char *sensor)
{
	char *co;
	int temp;
	const int convert = 1000;
	co = readfile(base, sensor);
	if (co == NULL)
		return smprintf("NA");
	temp = atoi(co);
	free(co);
	return smprintf("%d°C", (temp / convert));
}

//Compile with gcc temperature.c -o temperature
//int main(void)
//{
//    char* volstr;
//    volstr = gettemperature("/sys/class/hwmon/hwmon8", "temp1_input");
//    printf("Master string:  %s\n", volstr);
//free(volstr);
//
//    return 0;
//}
