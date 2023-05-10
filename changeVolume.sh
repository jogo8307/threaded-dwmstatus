#!/bin/bash

if [[ $1 == "mute" ]]
then
	pactl set-sink-mute @DEFAULT_SINK@ toggle
	kill -SIGUSR1 $(pidof dwmstatus) # Send signal to status bar. Makes it value for the value
	exit 0
fi

#CURVOL=$(amixer get Master | awk '$0~/%/ {print $6=="[off]"?$6:$4}' | tr -d '[]'i | head -n1 | cut -d " " -f1)
CURVOL=$(pulsemixer --get-volume | head -n1 | cut -d " " -f1) # Get current volume

if [[ $# -eq 0 ]]
then
	if [[ $(pulsemixer --get-mute) == "1" ]]
	then
		printf "%s" "mute"
	else
		printf "%s" "$CURVOL%"
	fi
else
	# echo "$CURVOL"
	# notify-send "$CURVOL"
	if [[ $1 = "up" ]]
	then
		if [ $CURVOL -lt 110 ]
		then
			pactl set-sink-volume @DEFAULT_SINK@ +5%
		fi
	else

		pactl set-sink-volume @DEFAULT_SINK@ -5%
	fi

	kill -SIGUSR1 $(pidof dwmstatus) # Send signal to status bar. Makes it value for the value
fi
exit 0

