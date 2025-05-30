#!/bin/bash

/home/pi/bez.sh &
sleep 10
rm -f /home/pi/send_ip/ifconfig_robocup.txt
ifconfig >/home/pi/send_ip/ifconfig_robocup.txt
until scp /home/pi/send_ip/ifconfig_robocup.txt petrovic@capek.ii.fmph.uniba.sk:
do
   sleep 5
   ifconfig >/home/pi/send_ip/ifconfig_robocup.txt
done 


sleep 5
ifconfig >/home/pi/send_ip/ifconfig_robocup.txt
scp /home/pi/send_ip/ifconfig_robocup.txt petrovic@capek.ii.fmph.uniba.sk:
