#!/usr/bin/python

import argparse
import serial
import serial.tools.list_ports
import sys
from typing import Any
"""
Configure and return an instance of a Serial interface.
"""


def configureSerial(args: argparse.Namespace) -> serial.Serial:

	dataBitsMapping = {5: serial.FIVEBITS, 6: serial.SIXBITS, 7: serial.SEVENBITS, 8: serial.EIGHTBITS}
	parityMapping = {
		"none": serial.PARITY_NONE,
		"even": serial.PARITY_EVEN,
		"odd": serial.PARITY_ODD,
		"mark": serial.PARITY_MARK,
		"space": serial.PARITY_SPACE
	}
	stopBitsMapping = {1: serial.STOPBITS_ONE, 1.5: serial.STOPBITS_ONE_POINT_FIVE, 2: serial.STOPBITS_TWO}

	return serial.Serial(port=args.dev,
		baudrate=args.baud_rate,
		bytesize=dataBitsMapping[args.data_bits],
		parity=parityMapping[args.parity],
		stopbits=stopBitsMapping[args.stop_bits],
		xonxoff=True if args.flow_control == "xonxoff" else False,
		rtscts=True if args.flow_control == "rtscts" else False,
		dsrdtr=True if args.flow_control == "dsrdtr" else False,
		timeout=1)


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Terminal.")
	parser.add_argument("--dev", type=str, default="/dev/ttyUSB0", help="Device.")
	parser.add_argument("--baud_rate", type=int, default=115200, help="Baudrate of the communication.")
	parser.add_argument("--data_bits", type=int, default=8, help="Data bits.")
	parser.add_argument("--parity", choices=["none", "even", "odd", "mark", "space"], default="none", help="Parity.")
	parser.add_argument("--stop_bits", type=float, default=1, help="Number of stop bits.")
	parser.add_argument("--flow_control",
		choices=["none", "xonxoff", "rtscts", "dsrdtr"],
		default="none",
		help="Flow control.")

	args = parser.parse_args()

	ports = serial.tools.list_ports.comports()
	print("Available ports:")
	for info in sorted(ports):
		print("{}: {} [{}:{}]".format(info.device, info.description, info.vid, info.pid))

	with configureSerial(args) as serialInstance:
		while True:
			line = serialInstance.readline()
			if line:
				sys.stdout.write(line.decode("ascii", errors="ignore"))
