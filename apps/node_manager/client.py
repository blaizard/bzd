import argparse
import re
import struct
import socket
import typing
import time
import urllib.request


def checkMAC(mac: str) -> bool:
	"""Check that a mac address format is correct.
    It should be: XX:XX:XX:XX:XX:XX
    """

	return bool(re.match(r"^([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}$", mac))


def getHostPort(hostport: str) -> typing.Tuple[str, int]:
	"""Separate the hostname from the port and return it."""

	split = hostport.split(":")
	assert (len(split) == 2), f"The string does not have a correct format, should be 'hostname:port', not '{hostport}'."
	return split[0], int(split[1])


def wakeOnLan(mac: str, broadcast: str) -> None:
	"""Send a packet to wake on lan a specific host."""

	# Pad the synchronization stream.
	data = "".join(["FFFFFFFFFFFF", mac.replace(":", "") * 20])
	sendData = b""

	# Split up the hex values and pack.
	for j in range(0, len(data), 2):
		sendData = b"".join([sendData, struct.pack("B", int(data[j:j + 2], 16))])

	# Broadcast it to the LAN.
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
	sock.sendto(sendData, (broadcast, 7))


def checkConnection(host: str, port: int, timeoutS: int = 1) -> bool:
	"""Check that a connection exists."""

	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.settimeout(timeoutS)
	try:
		sock.connect((host, port))
	except:
		return False
	else:
		sock.close()
		return True


def commandWol(args: argparse.Namespace) -> None:
	"""Wake on LAN command."""

	assert checkMAC(args.mac), f"Mac address '{args.mac}' should have the following format: XX:XX:XX:XX:XX:XX"

	# Wake up the machine.
	wakeOnLan(args.mac, args.broadcast)

	# Wait for services to be ready.
	for entry in args.wait:
		host, port = getHostPort(entry)
		timeLimit = time.perf_counter() + args.timeout
		connectionOpen = False
		while time.perf_counter() < timeLimit:
			if checkConnection(host, port, timeoutS=2):
				connectionOpen = True
				break
			time.sleep(1)
		assert connectionOpen, f"Connection for {entry} timed out after {args.timeout}s"


def commandSuspend(args: argparse.Namespace) -> None:
	"""Suspend a machine running the server."""

	host, port = getHostPort(args.ip)

	try:
		urllib.request.urlopen(f"http://{host}:{port}/suspend").read()
	except Exception as e:
		print(str(e))


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Wake On Lan client.")
	subparsers = parser.add_subparsers(help="Available sub-commands.", dest="command")

	wolParser = subparsers.add_parser("wol", help="Wake-up a machine from its MAC address.")
	wolParser.add_argument(
	    "-b",
	    "--broadcast",
	    default="255.255.255.255",
	    help="The broadcast address to be used.",
	)
	wolParser.add_argument(
	    "-t",
	    "--timeout",
	    default=60,
	    type=int,
	    help="Timeout in S until which the check returns.",
	)
	wolParser.add_argument(
	    "-w",
	    "--wait",
	    action="append",
	    default=[],
	    help="Wait until a specific TCP connection is open.",
	)
	wolParser.add_argument("mac", help="The mac address for the machine to wake up.")

	shutdownParser = subparsers.add_parser("suspend", help="Suspend a machine running the server.")
	shutdownParser.add_argument("ip", help="The ip:port address for the machine to suspend.")

	args = parser.parse_args()

	if args.command == "wol":
		commandWol(args)
	elif args.command == "suspend":
		commandSuspend(args)
	else:
		assert False, f"Unknown command: '{args.command}'."
