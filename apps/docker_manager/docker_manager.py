import argparse
import json
import sys
import socket
import pathlib
import typing
import os
import datetime
import time
import threading

from bzd.http.parser import HttpParser
from bzd.utils.scheduler import Scheduler
from apps.artifacts.api.python.node.node import Node

Container = typing.Dict[str, typing.Any]
DiskSpace = typing.Dict[str, typing.Any]

T = typing.TypeVar("T")


class AsyncFunction(typing.Generic[T]):

	def __init__(self, function: typing.Callable[[], T], defaultValue: T) -> None:
		self.function = function
		self.worker: typing.Optional[threading.Thread] = None
		self.result = defaultValue

	def _wrapper(self) -> None:
		self.result = self.function()

	def trigger(self) -> bool:
		"""Trigger the function.
		
		Returns:
			True if the function was triggered, False if it is already running from a previous trigger.
		"""

		if (self.worker is None) or (not self.worker.is_alive()):
			self.worker = threading.Thread(target=self._wrapper, daemon=True)
			self.worker.start()
			return True
		return False

	def get(self, waitMaxS: int = 0) -> T:
		"""Get the result.

		Args:
			waitMaxS: Wait for a mxiumum of seconds for the result. If no new result is present by that time,
			return the previous or default value.
		"""

		if self.trigger():
			assert self.worker is not None
			endTime = time.time() + waitMaxS
			while time.time() < endTime and self.worker.is_alive():
				time.sleep(0.1)
		return self.result


class Docker:

	def __init__(self, socket: pathlib.Path) -> None:
		self.socket = socket
		self.previousIO: typing.Dict[str, typing.Any] = {}
		self.previousNetwork: typing.Dict[str, typing.Any] = {}
		self.diskSpace: AsyncFunction[DiskSpace] = AsyncFunction(self.getDiskSpace, {})

	def request(self, method: str, endpoint: str, timeoutS: int = 10) -> typing.Any:
		with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as sock:
			sock.connect(self.socket.as_posix())

			# Get the list of running containers
			request = f"{method} {endpoint} HTTP/1.1\r\nHost: docker\r\n\r\n"
			sock.sendall(request.encode())

			parser = HttpParser(lambda: sock.recv(4096), timeoutS=timeoutS)
			content = parser.readAll().decode()
			return json.loads(content)

	def getContainers(self) -> typing.List[Container]:
		raw = self.request("GET", "/containers/json?all=true&size=true")

		containers = []
		for data in raw:

			info = {
			    "id": data["Id"],
			    "image": data["Image"],
			    "name": data["Names"][0],
			    "active": True if data["State"].lower() == "running" else False,
			    "version": {},
			    "volumes": {
			        mount["Name"]: mount["Destination"]
			        for mount in data.get("Mounts", []) if mount["Type"] == "volume"
			    },
			    "size": data["SizeRootFs"]
			}

			if "Image" in data:
				info["version"]["image"] = data["Image"]
			if "Created" in data:
				datetimeObj = datetime.datetime.fromtimestamp(data["Created"])
				info["version"]["created"] = datetimeObj.strftime("%Y-%m-%d %H:%M:%S")

			containers.append(info)

		return containers

	def getDiskSpace(self) -> DiskSpace:

		# This takes time.
		raw = self.request("GET", f"/system/df?type=volume", timeoutS=600)

		output: DiskSpace = {"volumes": {}}
		for volume in raw.get("Volumes", []):
			output["volumes"][volume["Name"]] = {"used": volume["UsageData"]["Size"]}
		return output

	def _cpuInfo(self, containerId: str, stats: typing.Any) -> typing.Any:
		"""Extracts CPU usage stats from the Docker stats dictionary.

		Args:
			stats: The Docker stats dictionary.

		See: https://docs.docker.com/reference/api/engine/version/v1.44/#tag/Container/operation/ContainerExport

		Returns:
			A dictionary containing CPU usage stats.
		"""

		try:
			cpuDelta = stats["cpu_stats"]["cpu_usage"]["total_usage"] - stats["precpu_stats"]["cpu_usage"]["total_usage"]
			systemDelta = stats["cpu_stats"]["system_cpu_usage"] - stats["precpu_stats"]["system_cpu_usage"]
			onlineCpus = len(stats["cpu_stats"]["cpu_usage"].get("percpu_usage",
			                                                     [])) or stats["cpu_stats"]["online_cpus"]
			cpuUsage = cpuDelta / systemDelta
			cpuPercentages = []

			for i in range(onlineCpus):
				cpuPercentages.append(cpuUsage)

			return {"system": cpuPercentages}

		except KeyError as e:
			pass

		return None

	def _memoryInfo(self, containerId: str, stats: typing.Any) -> typing.Any:
		"""Extracts memory usage stats from the Docker stats dictionary.

		See: https://docs.docker.com/reference/api/engine/version/v1.44/#tag/Container/operation/ContainerExport

		Args:
			stats: The Docker stats dictionary.

		Returns:
			A dictionary containing memory usage stats.
		"""

		try:
			memoryUsage = stats["memory_stats"]["usage"] - stats["memory_stats"]["stats"].get("cache", 0)
			memoryTotal = stats["memory_stats"]["limit"]

			return {"ram": {"used": memoryUsage, "total": memoryTotal}}

		except KeyError as e:
			pass

		return None

	def _returnRate(self, previousData: typing.Any, data: typing.Any) -> typing.Any:
		"""Calculate the rate from a dictionary containing in/out numbers."""

		timestampS = time.time()
		previousTimestampS = previousData.get("_timestampS", timestampS)
		previousData["_timestampS"] = timestampS
		diffTimestampS = timestampS - previousTimestampS

		dataWithRates = {}
		for name, rate in data.items():
			if diffTimestampS < 0.01:
				dataWithRates[name] = {
				    "in": 0,
				    "out": 0,
				}
			else:
				previousDataRates = previousData.get(name, {})
				dataWithRates[name] = {
				    "in": ((rate["in"] - previousDataRates.get("in", rate["in"])) / diffTimestampS),
				    "out": ((rate["out"] - previousDataRates.get("out", rate["out"])) / diffTimestampS),
				}
			previousData[name] = rate

		return dataWithRates

	def _ioInfo(self, containerId: str, stats: typing.Any) -> typing.Any:
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
					if entry["op"] == 'read':
						readBytes += entry["value"]
					elif entry["op"] == "write":
						writeBytes += entry["value"]

				return self._returnRate(self.previousIO.setdefault(containerId, {}),
				                        {"block": {
				                            "in": writeBytes,
				                            "out": readBytes
				                        }})

		except KeyError as e:
			pass

		return None

	def _networkInfo(self, containerId: str, stats: typing.Any) -> typing.Any:
		"""Extracts network throughput stats from the Docker stats dictionary.

		Args:
			stats: The Docker stats dictionary.

		Returns:
			A dictionary containing network throughput stats.
		"""

		try:
			output = {}
			for interface, data in stats["networks"].items():
				output[interface] = {"in": data["rx_bytes"], "out": data["tx_bytes"]}
			return self._returnRate(self.previousNetwork.setdefault(containerId, {}), output)

		except KeyError as e:
			pass

		return None

	def getContainerStats(self, container: Container) -> typing.Any:
		containerId = container["id"]
		raw = self.request("GET", f"/containers/{containerId}/stats?stream=false")
		output = {
		    "active": container["active"],
		    "version": container["version"],
		    "disk": {
		        "/": {
		            "used": container["size"]
		        }
		    }
		}

		for volume, destination in container["volumes"].items():
			diskSpace = self.diskSpace.get(1).get("volumes", {})
			if volume in diskSpace:
				output["disk"][destination] = diskSpace[volume]

		def addIfNotNone(key: str, data: typing.Any) -> None:
			if data is not None:
				output[key] = data

		addIfNotNone("cpu", self._cpuInfo(containerId, raw))
		addIfNotNone("memory", self._memoryInfo(containerId, raw))
		addIfNotNone("io", self._ioInfo(containerId, raw))
		addIfNotNone("network", self._networkInfo(containerId, raw))
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

	def getDockerData(self) -> typing.Any:
		containers = self.getContainers()
		data = {}
		for container in containers:
			stats = self.getContainerStats(container)
			name = container["name"].replace("/", "")
			data[name] = {
			    "data": stats,
			}
		return data


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
	parser.add_argument("--measure-time", type=float, default=None, help="Measure for x seconds.")
	parser.add_argument(
	    "uid",
	    nargs="?",
	    default=os.environ.get("BZD_NODE_UID"),
	    help=
	    "The UID of this node. If no UID is provided, the application will report the monitoring on the command line.")

	args = parser.parse_args()

	docker = Docker(args.docker_socket)

	if args.uid is None:
		if args.measure_time is not None:
			docker.getDockerData()
			time.sleep(args.measure_time)
		data = docker.getDockerData()
		print(json.dumps(data, indent=4))
		sys.exit(0)

	node = Node(uid=args.uid, token=args.node_token)

	# Start the thread to monitor the node.
	def monitorWorkload() -> None:
		data = docker.getDockerData()
		updatedData = {f"{args.uid}.{key}": value for key, value in data.items()}
		try:
			node.publishMultiNodes(data=updatedData)
		except:
			# Ignore any errors, we don't want to crash if something is wrong on the server side.
			pass

	scheduler = Scheduler(blocking=True)
	scheduler.add("monitor", args.report_rate, monitorWorkload)
	print(f"Monitoring {args.docker_socket} every {args.report_rate}s...", flush=True)
	scheduler.start()
