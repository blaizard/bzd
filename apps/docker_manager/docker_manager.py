import argparse
import json
import sys
import socket
import pathlib
import typing
import os
import datetime

from bzd.http.parser import HttpParser

Container = typing.Dict[str, typing.Any]


class Docker:

	def __init__(self, socket: pathlib.Path) -> None:
		self.socket = socket

	def request(self, method: str, endpoint: str) -> typing.Any:
		with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as sock:
			sock.connect(self.socket.as_posix())

			# Get the list of running containers
			request = f"{method} {endpoint} HTTP/1.1\r\nHost: docker\r\n\r\n"
			sock.sendall(request.encode())

			parser = HttpParser(lambda: sock.recv(4096))
			content = parser.readAll().decode()
			return json.loads(content)

	def getContainers(self) -> typing.List[Container]:
		raw = self.request("GET", "/containers/json?all=true")

		containers = []
		for data in raw:
			containers.append({
			    "id": data["Id"],
			    "image": data["Image"],
			    "name": data["Names"][0],
			    "active": True if data["State"].lower() == "running" else False,
			})

		return containers

	@staticmethod
	def _cpuInfo(stats: typing.Any) -> typing.Any:
		"""Extracts CPU usage stats from the Docker stats dictionary.

		Args:
			stats: The Docker stats dictionary.

		Returns:
			A dictionary containing CPU usage stats.
		"""

		try:
			cpuUsage = stats['cpu_stats']['cpu_usage']['total_usage']
			systemCpuUsage = stats['cpu_stats']['system_cpu_usage']
			onlineCpus = stats['cpu_stats']['online_cpus']
			cpuPercentages = []

			cpuDelta = cpuUsage - stats['precpu_stats']['cpu_usage']['total_usage']
			systemDelta = systemCpuUsage - stats['precpu_stats']['system_cpu_usage']

			if systemDelta > 0 and onlineCpus > 0:
				if "percpu_usage" in stats['cpu_stats']['cpu_usage']:
					for i in range(onlineCpus):
						cpuDeltaPerCpu = stats['cpu_stats']['cpu_usage']['percpu_usage'][i] - stats['precpu_stats'][
						    'cpu_usage']['percpu_usage'][i]
						cpuPercent = (cpuDeltaPerCpu / systemDelta) * 100.0
						cpuPercentages.append(cpuPercent)
				else:
					cpuPercentages = [cpuDelta / systemDelta] * onlineCpus

			return {"system": cpuPercentages}

		except KeyError as e:
			pass

		return None

	@staticmethod
	def _memoryInfo(stats: typing.Any) -> typing.Any:
		"""Extracts memory usage stats from the Docker stats dictionary.

		Args:
			stats: The Docker stats dictionary.

		Returns:
			A dictionary containing memory usage stats.
		"""

		try:
			memoryUsage = stats['memory_stats']['usage']
			memoryTotal = stats['memory_stats']['limit']

			return {"ram": {"used": memoryUsage, "total": memoryTotal}}

		except KeyError as e:
			pass

		return None

	@staticmethod
	def _ioInfo(stats: typing.Any) -> typing.Any:
		"""Extracts I/O throughput stats from the Docker stats dictionary.

		Args:
			stats: The Docker stats dictionary.

		Returns:
			A dictionary containing I/O throughput stats.
		"""

		try:
			readBytes = 0
			writeBytes = 0

			# This can be None.
			if stats["blkio_stats"]["io_service_bytes_recursive"]:
				for entry in stats["blkio_stats"]["io_service_bytes_recursive"]:
					if entry['op'] == 'read':
						readBytes += entry['value']
					elif entry['op'] == 'write':
						writeBytes += entry['value']

				return {"block": {"in": writeBytes, "out": readBytes}}

		except KeyError as e:
			pass

		return None

	@staticmethod
	def _networkInfo(stats: typing.Any) -> typing.Any:
		"""Extracts network throughput stats from the Docker stats dictionary.

		Args:
			stats: The Docker stats dictionary.

		Returns:
			A dictionary containing network throughput stats.
		"""

		try:
			output = {}
			for interface, data in stats['networks'].items():
				rxBytes = data['rx_bytes']
				txBytes = data['tx_bytes']
				output[interface] = {"in": rxBytes, "out": txBytes}
			return output

		except KeyError as e:
			pass

		return None

	def getContainerStats(self, container: Container) -> typing.Any:
		raw = self.request("GET", f"/containers/{container['id']}/stats?stream=false")
		output = {"active": container["active"]}

		def addIfNotNone(key: str, data: typing.Any) -> None:
			if data is not None:
				output[key] = data

		addIfNotNone("cpu", self._cpuInfo(raw))
		addIfNotNone("memory", self._memoryInfo(raw))
		addIfNotNone("io", self._ioInfo(raw))
		addIfNotNone("network", self._networkInfo(raw))
		addIfNotNone("uptime", self.getContainerUpTime(container))

		return output

	def getContainerUpTime(self, container: Container) -> typing.Optional[float]:
		"""Retrieves the uptime of a Docker container.

		Args:
			container: The Docker container.

		Returns:
			A float representing the container's uptime in seconds, or None if an error occurs.
		"""
		raw = self.request("GET", f"/containers/{container['id']}/json")

		try:

			startedAtStr = raw["State"]["StartedAt"]
			startedAt = datetime.datetime.fromisoformat(startedAtStr.replace("Z", "+00:00"))
			uptime = datetime.datetime.now(datetime.timezone.utc) - startedAt

			return uptime.total_seconds()

		except KeyError as e:
			pass

		return None


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Docker Manager.")
	parser.add_argument("--report-rate",
	                    default=5,
	                    type=int,
	                    help="The rate (in s) at which status messages are emitted.")
	parser.add_argument("--node-token",
	                    type=str,
	                    default=os.environ.get("BZD_NODE_TOKEN"),
	                    help="A token to be used to access the node server.")
	parser.add_argument("--docker-socket",
	                    type=pathlib.Path,
	                    default=pathlib.Path("/run/user/1000/docker.sock"),
	                    help="Docker socket to connect to the docker daemon.")
	parser.add_argument(
	    "uid",
	    nargs="?",
	    default=os.environ.get("BZD_NODE_UID"),
	    help=
	    "The UID of this node. If no UID is provided, the application will report the monitoring on the command line.")

	args = parser.parse_args()

	docker = Docker(args.docker_socket)
	containers = docker.getContainers()

	data = {}
	for container in containers:
		stats = docker.getContainerStats(container)
		name = container["name"].replace("/", "")
		data[name] = {"data": stats}

	if args.uid is None:
		print(json.dumps(data, indent=4))
		sys.exit(0)

	sys.exit(0)
