#!/bin/sh
oscsend localhost 4001 /shutdown i 1
killall python2
killall create_ap
killall wpa_supplicant
sleep .1
cp /root/ETC_Sys/scripts/shutdown-splash /dev/fb0
shutdown -h now
