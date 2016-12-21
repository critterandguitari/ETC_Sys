#!/bin/sh
sudo systemctl stop serial-getty@ttymxc0.service
sudo dmesg -n 1
sudo echo performance > /sys/devices/system/cpu/cpu0/cpufreq/scaling_govenor
amixer set PCM 192-
amixer set PCM 170+
amixer set Headphone 127-
amixer set Headphone 127+
amixer set 'Capture Mux' 'LINE_IN'
/root/ETC_Sys/scripts/mount.sh
/root/ETC_Daemon/etcd &
/root/ETC_Sys/scripts/fsquares
export SDL_VIDEODRIVER=fbcon

sleep 1

/root/ETC_Sys/scripts/start-etc.sh

if lsusb | grep RT5370
    then 
    echo "wifi usb adapter found"
   # /root/ETC_Sys/scripts/setup-ap.sh
    /usbdrive/WiFi
    cd /root/ETC_Web
    ./run.sh &
fi

