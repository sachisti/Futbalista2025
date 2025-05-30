#!/bin/bash

wget -q --output-document=- https://capek.ii.fmph.uniba.sk/rcj/ifconfig_robocup$1.txt | grep 192.168 | sed "s/.*inet //g" | sed "s/ .*//g"
