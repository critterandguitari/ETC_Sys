#!/bin/sh
oscsend localhost 4001 /shutdown i 1
/root/scripts/killpd.sh
/root/scripts/killmother.sh
killall create_ap
killall wpa_supplicant
shutdown -h now
#echo "shjutting down"
