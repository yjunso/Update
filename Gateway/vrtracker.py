#!/usr/bin/env python
import websocketserver

#Add ip address of the device running this code (pc or RPi) between the single quotes
#Port number is controlled in websocketserver.py
myserver = websocketserver.websocketserver('ipaddress')