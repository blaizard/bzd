#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import serial

if __name__== "__main__":

	count = 20
	with serial.Serial('/dev/ttyUSB0', 115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, xonxoff=1, timeout=1) as ser:
		while count:
			line = ser.readline()
			print(line)
			count -= 1
