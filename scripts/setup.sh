#!/bin/sh

# check if override run.sh
if [ -f /usbdrive/run.sh ]; then
    echo "running /usbdrive/run.sh"
    /usbdrive/run.sh &
else
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
	
	export SDL_VIDEODRIVER=fbcon

	/root/ETC_Sys/scripts/start-etc.sh

	# try to start wifi if there is a config file
	# and start the web interface
	if [ -f /usbdrive/WiFi.sh ]; then
		echo "wifi usb adapter found"
		/usbdrive/WiFi.sh
		cd /root/ETC_Web
		./run.sh &
	fi
fi


