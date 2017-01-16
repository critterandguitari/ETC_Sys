#!/bin/sh
oscsend localhost 4001 /shutdown i 1
killall create_ap
killall wpa_supplicant
sleep .1
shutdown -h now
