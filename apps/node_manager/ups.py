import typing
import socket
import shlex
from contextlib import contextmanager

from apps.node_manager.config import Config


class NUTClientAccessor:

	def __init__(self, sock: socket.socket) -> None:
		self.sock = sock
		self.buffer: bytes = b""

	def send(self, command: str) -> None:
		"""Sends a command to the server."""

		self.sock.sendall(f"{command}\n".encode("utf-8"))

	def readLine(self) -> str:
		"""Reads a single line from the socket buffer."""

		while b"\n" not in self.buffer:
			try:
				chunk = self.sock.recv(1024)
				if not chunk:
					raise ConnectionError("Connection closed by server.")
				self.buffer += chunk
			except socket.timeout:
				raise TimeoutError("Server response timed out.")

		line, self.buffer = self.buffer.split(b"\n", 1)
		return line.decode("utf-8").strip()

	def getUPSDevices(self) -> typing.Dict[str, str]:
		"""Retrieves a list of UPS devices from the server.
		
		Returns:
			A dictionary where keys are UPS names and values are their descriptions.
		
		Raises:
			IOError: If the server returns an error.
		"""
		self.send("LIST UPS")

		upses = {}

		line = self.readLine()
		if not line.startswith("BEGIN LIST UPS"):
			raise IOError(f"Unexpected server response for LIST UPS: {line}")

		while True:
			line = self.readLine()
			if line.startswith("END LIST UPS"):
				break
			if line.startswith("ERR"):
				raise IOError(f"Server returned an error: {line}")

			# Line format is: UPS <upsname> "<description>"
			# shlex.split handles the quotes safely
			parts = shlex.split(line)
			if len(parts) == 3 and parts[0] == "UPS":
				ups_name = parts[1]
				description = parts[2]
				upses[ups_name] = description

		return upses

	def get(self, device: str, variable: str) -> str:
		"""Retrieves the value of a specific variable for a given UPS device.
		
		Args:
			device: The name of the UPS.
			variable: The name of the variable to retrieve (e.g., 'battery.charge').
			
		Returns:
			The value of the variable as a string.
			
		Raises:
			IOError: If the variable is not found or the server returns an error.
		"""
		self.send(f"GET VAR {device} {variable}")
		line = self.readLine()

		if line.startswith("ERR"):
			raise IOError(f"Server returned an error for GET VAR: {line}")

		# Line format is: VAR <upsname> <varname> "<value>"
		parts = shlex.split(line)
		if len(parts) == 4 and parts[0] == "VAR":
			return parts[3]

		raise IOError(f"Could not parse server response for GET VAR: {line}")


class NUTClient:
	"""A simple Python NUT (Network UPS Tools) client without external libraries.
	
	This class allows you to connect to a upsd server, list available UPS devices,
	and retrieve specific variables from them."""

	def __init__(self,
	             hostname: str = "localhost",
	             port: int = 3493,
	             timeout: int = 5,
	             username: typing.Optional[str] = None,
	             password: typing.Optional[str] = None) -> None:
		"""Initializes the client.
		
		Args:
			hostname: The hostname or IP address of the NUT server.
			port: The port number for the NUT server (default is 3493).
			timeout: Socket connection timeout in seconds.
			username: Username for authentication. Defaults to None.
			password: Password for authentication. Defaults to None.
		"""
		self.hostname = hostname
		self.port = port
		self.timeout = timeout
		self.username = username
		self.password = password

	@contextmanager
	def connect(self) -> typing.Generator[NUTClientAccessor, None, None]:
		"""Establishes a connection to the NUT server and handles authentication."""

		sock: typing.Optional[socket.socket] = None
		accessor: typing.Optional[NUTClientAccessor] = None
		try:
			sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			sock.settimeout(self.timeout)
			sock.connect((self.hostname, self.port))

			accessor = NUTClientAccessor(sock)

			# Handle authentication if credentials are provided
			if self.username:
				accessor.send(f"USERNAME {self.username}")
				accessor.readLine()  # Should get an "OK"
			if self.password:
				accessor.send(f"PASSWORD {self.password}")
				accessor.readLine()  # Should get an "OK"

			yield accessor

		except socket.error as e:
			raise ConnectionError(f"Failed to connect to {self.hostname}:{self.port}: {e}")

		finally:
			if accessor:
				try:
					accessor.send("LOGOUT")
				except socket.error:
					pass
			if sock:
				sock.close()


class UPS:

	def __init__(self, config: Config) -> None:
		self.clients: typing.Dict[str, typing.Any] = {}
		for name, ups in config.ups().items():
			client = NUTClient(hostname=ups.hostname, port=ups.port, username=ups.username, password=ups.password)
			try:
				with client.connect() as accessor:
					devices = accessor.getUPSDevices().keys()
					if devices:
						self.clients[name] = {"client": client, "devices": devices}
			except Exception as e:
				# Default UPS server should not report an error.
				if name != "":
					raise

	def batteries(self) -> typing.Any:

		batteries = {}
		for name, data in self.clients.items():
			with data["client"].connect() as accessor:
				for ups in data["devices"]:
					batteries[f"{name}.{ups}" if name else ups] = float(accessor.get(ups, "battery.charge")) / 100.

		return batteries if batteries else None
