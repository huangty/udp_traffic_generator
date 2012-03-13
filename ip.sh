#!/bin/sh
sudo ifconfig wlan0 172.24.74.212 netmask 255.255.255.0
sudo route add default gw 172.24.74.1
