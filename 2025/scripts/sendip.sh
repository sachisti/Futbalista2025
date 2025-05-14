#!/bin/bash

/home/robotika/bez.sh &
sleep 10
rm -f /home/robotika/send_ip/ifconfig_robocup.txt
ifconfig >/home/robotika/send_ip/ifconfig_robocup.txt
scp /home/robotika/send_ip/ifconfig_robocup.txt petrovic@capek.ii.fmph.uniba.sk:

