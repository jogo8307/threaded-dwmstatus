//#define ALSAMIXER
//#define TEST
#ifdef ALSAMIXER

#include <unistd.h>
#include <alsa/asoundlib.h>
#include "components.h"
#include "util.h"
#define VOLUME_BOUND 100u
typedef enum {
	AUDIO_VOLUME_SET,
	AUDIO_VOLUME_GET,
} audio_volume_action;
// Stolen from https://stackoverflow.com/questions/7657624/get-master-sound-volume-in-c-in-linux 

/*Drawbacks. Sets volume on both channels but gets volume on one. Can be easily adapted.
 */

/*Returns or sets audio volume*/
int audio_volume(audio_volume_action action, long* outvol)
{
	int ret = 0;
	snd_mixer_t* handle;
	snd_mixer_elem_t* elem;
	snd_mixer_selem_id_t* sid;

	static const char* mix_name = "Master";
	static const char* card = "pulse";
	//static const char* card = "hw:0";
	static int mix_index = 0;

	//long pmin, pmax;
	//long get_vol, set_vol;
	//float f_multi;

	snd_mixer_selem_id_alloca(&sid);

	//sets simple-mixer index and name
	snd_mixer_selem_id_set_index(sid, mix_index);
	snd_mixer_selem_id_set_name(sid, mix_name);

	if ((snd_mixer_open(&handle, 0)) < 0)
		return -1;
	//	This function leaks
	if ((snd_mixer_attach(handle, card)) < 0) {
		snd_mixer_close(handle);
		return -2;
	}
	if ((snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
		snd_mixer_close(handle);
		return -3;
	}
	ret = snd_mixer_load(handle);
	if (ret < 0) {
		snd_mixer_close(handle);
		return -4;
	}
	elem = snd_mixer_find_selem(handle, sid);
	if (!elem) {
		snd_mixer_close(handle);
		return -5;
	}
	long minv, maxv;

	snd_mixer_selem_get_playback_volume_range (elem, &minv, &maxv);
	//fprintf(stderr, "Volume range <%li,%li>\n", minv, maxv);

	if(action == AUDIO_VOLUME_GET) {
		int muteswitch;
		if(snd_mixer_selem_get_playback_volume(elem, 0, outvol) < 0 || 
		snd_mixer_selem_get_playback_switch( elem, 0, &muteswitch) < 0 ) {
			snd_mixer_close(handle);
			return -6;
		}
		//fprintf(stderr, "Get volume %li with status %i\n", *outvol, ret);
		/* make the value bound to 100 */
		*outvol -= minv;
		maxv -= minv;
		//fprintf(stderr, "Minswitch %i", muteswitch);
		*outvol =((long)muteswitch) * 100 * (*outvol) / maxv; // make the value bound from 0 to 100
	}
	//else if(action == AUDIO_VOLUME_SET) {
	//	if(*outvol < 0 || *outvol > VOLUME_BOUND) // out of bounds
	//		return -7;
	//	*outvol = (*outvol * (maxv - minv) / (100-1)) + minv;

	//	if(snd_mixer_selem_set_playback_volume(elem, 0, *outvol) < 0) {
	//		snd_mixer_close(handle);
	//		return -8;
	//	}
	//	if(snd_mixer_selem_set_playback_volume(elem, 1, *outvol) < 0) {
	//		snd_mixer_close(handle);
	//		return -9;
	//	}
	//	//fprintf(stderr, "Set volume %li with status %i\n", *outvol, ret);
	//}
	snd_config_update_free_global();
	snd_mixer_close(handle);
	return 0;
}

char* get_vol(void){

	long vol;
	audio_volume(AUDIO_VOLUME_GET, &vol);
	if(vol > 0){
	return smprintf("%li",vol);
	}
	else{
	return smprintf("%s","mute");
	}
}

#else

#include <stdio.h>
#include "components.h"
#include "util.h"

//Bash
#define BUFSIZE 128
char* get_vol(void){
static char *cmd = "changeVolume.sh";    
    char buf[BUFSIZE];
    FILE *fp;

    if ((fp = popen(cmd, "r")) == NULL) {
	fprintf(stderr,"Error opening pipe!\n");
	return smprintf("%s",NULL);
    }

    //while (fgets(buf, BUFSIZE, fp) != NULL) {
    //    // Do whatever you want here...
    //    printf("OUTPUT: %s", buf);
    //}

    fgets(buf, BUFSIZE, fp);
        //printf("OUTPUT: %s", buf);

    if(pclose(fp))  {
	fprintf(stderr,"Command not found or exited with error status\n");
	return smprintf("%s",NULL);
    }

	return smprintf("%s",buf);
}

#endif

#ifdef TEST
//Compile with gcc volume.c util.c -o volume -lasound
#include <stdlib.h>
int main(void)
{
    long vol = -1;
    char* volstr;
    //printf("Ret %i\n", audio_volume(AUDIO_VOLUME_GET, &vol));
    //printf("Master volume is %li\n", vol);
    volstr = get_vol();
    printf("Master string:  %s\n", volstr);
free(volstr);
    //vol = 100;
    //printf("Ret %i\n", audio_volume(AUDIO_VOLUME_SET, &vol));

    return 0;
}
#endif
