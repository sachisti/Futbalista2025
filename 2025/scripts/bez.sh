#!/bin/bash

trap "/home/pi/bez.sh" EXIT

sleep 1
echo starting futbalista...
/home/pi/Futbalista2025/2025/rpi/futbalista headless
#/home/pi/bez.sh
