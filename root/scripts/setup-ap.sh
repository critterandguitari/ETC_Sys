
#!/bin/sh

killall python2
killall create_ap

create_ap wlan0 eth0 ETCETC &
cd /usbdrive/MVP2/mvp/web
./run.sh &

