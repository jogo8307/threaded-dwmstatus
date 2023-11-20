/*
 * Fork me if you can.
 * by 20h
 */

/* Includes System */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>

/* Includes project */
#include "config.h"
#include "Components/components.h"
#include "Components/util.h"

/* Local variables declaration */
static char* currentTime;
static char* masterVol;
static char* tempCPU;
static char* statBAT0;
static char* baseTemp;

static pthread_mutex_t  status_lock;

/* Local function declarations */
static Display *dpy;
static void setstatus(void);
static void* update_1m(void* arg);
static void* update_2s(void* arg);
static void update_signal(int sig);
static void init(void);

int main(void)
{
  if (!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "dwmstatus: cannot open display.\n");
    return 1;
  }
  init();
  pthread_t cyclicthread1, cyclicthread2;
  /* Initialize mutex and condition variable objects */
  pthread_mutex_init(&status_lock, NULL);
  pthread_create(&cyclicthread1,NULL,update_2s,NULL);
  pthread_create(&cyclicthread2,NULL,update_1m,NULL);
  pthread_join(cyclicthread1,NULL);
  pthread_join(cyclicthread2,NULL);
  XCloseDisplay(dpy);

  free(statBAT0);
  free(tempCPU);
  free(masterVol);
  free(currentTime);
  free(baseTemp);
  pthread_mutex_destroy(&status_lock);
  exit(0);
}

void setstatus(void){
  char* str;
  str = smprintf(" BAT0: %s | Temp: %s | Vol: %s | %s ", statBAT0, tempCPU, masterVol, currentTime);
  XStoreName(dpy, DefaultRootWindow(dpy), str);
  free(str);
  XSync(dpy, False);
}

void* update_2s(void* arg){
  //int k = 0;
  while(1){
    pthread_mutex_lock(&status_lock);
    free(tempCPU);
    tempCPU = gettemperature(baseTemp, TEMP_FILE_NAME);
    setstatus();
    pthread_mutex_unlock(&status_lock);
    sleep(2);
    //k++;
    //if(k==10) break;
  }
  pthread_exit(0);
}

void* update_1m(void* arg){
  //int k = 0;
  while(1){
    pthread_mutex_lock(&status_lock);
    free(currentTime);
    free(statBAT0);
    //free(masterVol);
    //masterVol = get_vol();
    statBAT0 = getbattery(BATTERY_BASE);
    currentTime = getTime("%a %d %b %Y | %H:%M ", TIME_ZONE);
    setstatus();
    pthread_mutex_unlock(&status_lock);
    //k++;
    //if(k==10) break;
    sleep(60);
  }
  pthread_exit(0);
}

void update_signal(int sig){
  signal(SIGUSR1, update_signal);
  pthread_mutex_lock(&status_lock);
  free(masterVol);
  masterVol = get_vol();
  setstatus();
  pthread_mutex_unlock(&status_lock);
  //fprintf(stderr,"Vol: %s",masterVol);
}
void init(void)
{
  char *co = NULL;
  char basecheck[100];
  char num[sizeof(unsigned int) + 1];
  unsigned int i;
  for(i = 0; i <= 10 ; i++)
  {
    strcpy(basecheck, TEMP_FOLDER_BASE);
    sprintf(num, "%u", i);
    strcat(basecheck, num);
    //fprintf(stderr,"CPU_Temp_base: %s \n",basecheck);
    if( access(basecheck, R_OK) == 0 )
    {
      co = readfile(basecheck, TEMP_NAME_FILE);
      co[strlen(co)-1] = '\0'; //Replace '\n' with '\0'
                               //fprintf(stderr,"CPU_Temp_base: '%s'",co);
      int result = strcmp(co, TEMP_NAME);
      free(co);
      //			fprintf(stderr,"Result: %d \n",result);
      if ( result == 0 )
      {
        baseTemp = smprintf(basecheck);
        //fprintf(stderr,"CPU_Temp_base: '%s' \n",baseTemp);
        break;
      }

    }
    usleep(1000);
  }

  signal(SIGUSR1, update_signal);
  statBAT0 = getbattery(BATTERY_BASE);
  tempCPU = gettemperature(baseTemp, TEMP_FILE_NAME);
  masterVol = get_vol();
  currentTime = getTime(" %a %d %b %Y | %H:%M | %Z ", TIME_ZONE);
  //fprintf(stderr,"Vol: %s",masterVol);
  setstatus();
}
