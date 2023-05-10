#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "components.h"
#include "util.h"

char* getbattery(const char *base)
{
	char *co, status;
	int descap, remcap;

	descap = -1;
	remcap = -1;

	co = readfile(base, "present");
	if (co == NULL)
		return smprintf("");
	if (co[0] != '1') {
		free(co);
		return smprintf("not present");
	}
	free(co);

	co = readfile(base, "energy_full_design");
	if (co == NULL) {
		co = readfile(base, "charge_full_design");
		if (co == NULL)
			return smprintf("");
	}
	sscanf(co, "%d", &descap);
	free(co);

	co = readfile(base, "energy_now");
	if (co == NULL) {
		co = readfile(base, "charge_now");
		if (co == NULL)
			return smprintf("");
	}
	sscanf(co, "%d", &remcap);
	free(co);

	co = readfile(base, "status");
	if (!strncmp(co, "Discharging", 11)) {
		status = '-';
	} else if(!strncmp(co, "Charging", 8)) {
		status = '+';
	} else {
		status = 'F';
	}
	free(co);
	if (remcap < 0 || descap < 0)
		return smprintf("invalid");

	return smprintf("%.0f%% %c", ((float)remcap / (float)descap) * 100.0, status);
}

//Compile with gcc battery.c util.c -o battery
//int main(void)
//{
//    long vol = -1;
//    char* volstr;
//    volstr = getbattery("/sys/class/power_supply/BAT0");
//
//    printf("Master string:  %s\n", volstr);
//free(volstr);
//
//    return 0;
//}
