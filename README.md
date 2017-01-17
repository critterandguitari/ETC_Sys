# System files and scripts for ETC

most important stuff is in the scripts folder.

fastsquares.c gets called to initialize video hardware.  for some reason pygame will not double buffer unless this program is run first.

fbinit.service and fbthing.service go here:

/etc/systemd/system

these initialize and launch the ETC
