#!/bin/sh
sudo systemctl stop serial-getty@ttymxc0.service
sudo dmesg -n 1
/root/ETC_Sys/scripts/fsquares
