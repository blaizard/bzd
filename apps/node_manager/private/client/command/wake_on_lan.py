import re
import struct
import socket
import time
import typing

from apps.node_manager.private.client.command.common import getHostPort


def checkMAC(mac: str) -> bool:
	"""Check that a mac address format is correct.
	It should be: XX:XX:XX:XX:XX:XX
	"""

	return bool(re.match(r"^([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}$", mac))


def wakeOnLan(mac: str, broadcast: str) -> None:
	"""Send a packet to wake on lan a specific host."""

	# Pad the synchronization stream.
	data = "".join(["FFFFFFFFFFFF", mac.replace(":", "") * 20])
	sendData = b""

	# Split up the hex values and pack.
	for j in range(0, len(data), 2):
		sendData = b"".join([sendData, struct.pack("B", int(data[j : j + 2], 16))])

	# Broadcast it to the LAN.
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
	sock.sendto(sendData, (broadcast, 7))


def wakeOnLanProxy(mac: str, service: str) -> None:
	"""Send a packet to wake on lan a specific host via a proxy."""

	service_host, service_port = getHostPort(service)
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((service_host, service_port))
	try:
		sock.sendall(mac.encode("utf-8"))
		print(f"Successfully sent '{mac}' to {service_host}:{service_port}", flush=True)

	finally:
		sock.close()


def checkConnection(host: str, port: int, timeoutS: int = 1) -> bool:
	"""Check that a connection exists."""

	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.settimeout(timeoutS)
	try:
		sock.connect((host, port))
	except Exception:
		return False
	else:
		sock.close()
		return True


def commandWakeOnLan(mac: str, broadcast: str, service: str, wait: typing.List[str], timeout: int) -> None:
	"""Wake on LAN command."""

	assert checkMAC(mac), f"Mac address '{mac}' should have the following format: XX:XX:XX:XX:XX:XX"

	# Wake up the machine.
	if service:
		wakeOnLanProxy(mac, service)
	else:
		wakeOnLan(mac, broadcast)

	# Wait for services to be ready.
	for entry in wait:
		host, port = getHostPort(entry)
		print(f"Waiting for {host}:{port} to be ready...", flush=True)
		connectionOpen = False
		startTime = int(time.perf_counter())
		for timeLimit in range(startTime, startTime + timeout, 5):
			while time.perf_counter() < timeLimit:
				if checkConnection(host, port, timeoutS=2):
					connectionOpen = True
					break
				time.sleep(1)
			# Send a new WOL signal every 5s
			wakeOnLan(mac, broadcast)
		assert connectionOpen, f"Connection for {entry} timed out after {timeout}s"
