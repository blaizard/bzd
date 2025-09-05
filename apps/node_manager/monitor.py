import psutil
import typing
import json
import time

from apps.node_manager.rest_server import RESTServerContext
from apps.node_manager.nvidia import Nvidia
from apps.node_manager.ups import UPS
from apps.node_manager.config import Config


class Monitor:
	"""Data format output:

	```json
	{
		"temperature": {
			"cpu": [60, 56],
			"gpu": [67, 63, 57, 59]
		},
		"cpu": {
			"main": [12.1, 0, 4, 34.5]
		}
	}
	```

	Temperatures are in Celsius.
	CPU loads are in percent.
	"""

	def __init__(self, config: Config) -> None:
		self.config = config
		self.nvidia = Nvidia()
		self.ups = UPS(config=config)
		self.networkPrevious: typing.Dict[str, typing.Any] = {}
		self.ioPrevious: typing.Dict[str, typing.Any] = {}

	def temperatures(self) -> typing.Any:
		data = {}
		temperatures = psutil.sensors_temperatures()  # type: ignore
		for name, group in temperatures.items():
			data[name] = [item.current for item in group]
		data.update(self.nvidia.temperatures())
		return data

	def cpus(self) -> typing.Any:
		return {"main": [cpu / 100. for cpu in psutil.cpu_percent(interval=1, percpu=True)]}  # type: ignore

	def gpus(self) -> typing.Any:
		data = {}
		data.update(self.nvidia.gpus())
		return data

	def batteries(self) -> typing.Any:
		try:
			maybeBattery = psutil.sensors_battery()  # type: ignore
			if maybeBattery is None:
				return None
		except FileNotFoundError:
			# On Synology for example, /sys/class/power_supply does not exists and it makes the application crash.
			return None
		return {"main": [maybeBattery[0] / 100.]}

	def memories(self) -> typing.Any:
		virtual = psutil.virtual_memory()  # type: ignore
		swap = psutil.swap_memory()  # type: ignore
		memories = {}
		if virtual.total:
			# Using virtual.used doesn't give the same result as reported by the OS.
			memories["ram"] = {"used": (virtual.total - virtual.available), "total": virtual.total}
		if swap.total:
			memories["swap"] = {"used": swap.used, "total": swap.total}
		memories.update(self.nvidia.memories())
		return memories

	def disks(self) -> typing.Any:
		ignoreWithOps = {
		    "ro"  # Ignore read-only.
		}
		disks = {}
		for partition in psutil.disk_partitions():  # type: ignore
			# Do some filtering as some cloud vps mount a lot of devices.
			if any(opt in ignoreWithOps for opt in partition.opts.split(",")):
				continue
			if partition.device not in disks:
				usage = psutil.disk_usage(partition.mountpoint)  # type: ignore
				disks[partition.device] = {"path": [], "data": {"used": usage.used, "total": usage.total}}
			disks[partition.device]["path"].append(partition.mountpoint)  # type: ignore

		dictionary = {",".join(sorted(data["path"])): data["data"] for data in disks.values()}
		return self.config.disksMappings(dictionary)

	def network(self) -> typing.Any:

		# Calculate the time diff in seconds from the previous run.
		timestampS = time.time()
		timestampDiffS = timestampS - self.networkPrevious.get("_timestampS", timestampS)
		self.networkPrevious["_timestampS"] = timestampS

		# Only consider interfaces that are UP and the ones that are not loopback.
		statsAll = psutil.net_if_stats()  # type: ignore
		validInterfaces = {
		    interface
		    for interface, stats in statsAll.items() if stats.isup and "loopback" not in stats.flags.lower()
		}

		# Get the stats
		netStat = psutil.net_io_counters(pernic=True, nowrap=True)  # type: ignore
		network = {}
		for name, snetio in netStat.items():
			if name in validInterfaces:
				self.networkPrevious.setdefault(name, {})
				diffIn = snetio.bytes_recv - self.networkPrevious[name].get("in", snetio.bytes_recv)
				diffOut = snetio.bytes_sent - self.networkPrevious[name].get("out", snetio.bytes_sent)
				self.networkPrevious[name] = {
				    "in": snetio.bytes_recv,
				    "out": snetio.bytes_sent,
				}
				network[name] = {
				    "in": (diffIn / timestampDiffS) if timestampDiffS > 0.01 else 0,
				    "out": (diffOut / timestampDiffS) if timestampDiffS > 0.01 else 0
				}
		return network

	def io(self) -> typing.Any:

		# Calculate the time diff in seconds from the previous run.
		timestampS = time.time()
		timestampDiffS = timestampS - self.ioPrevious.get("_timestampS", timestampS)
		self.ioPrevious["_timestampS"] = timestampS

		ioStat = psutil.disk_io_counters(perdisk=True, nowrap=True)  # type: ignore
		io = {}
		for name, sdiskio in ioStat.items():
			self.ioPrevious.setdefault(name, {})
			diffIn = sdiskio.write_bytes - self.ioPrevious[name].get("in", sdiskio.write_bytes)
			diffOut = sdiskio.read_bytes - self.ioPrevious[name].get("out", sdiskio.read_bytes)
			self.ioPrevious[name] = {
			    "in": sdiskio.write_bytes,
			    "out": sdiskio.read_bytes,
			}
			io[name] = {
			    "in": (diffIn / timestampDiffS) if timestampDiffS > 0.01 else 0,
			    "out": (diffOut / timestampDiffS) if timestampDiffS > 0.01 else 0
			}
		return io

	def upTime(self) -> float:
		return time.time() - psutil.boot_time()  # type: ignore

	def all(self) -> typing.Any:

		content = {}

		def assignIfSet(key: str, value: typing.Any) -> None:
			if value:
				content[key] = value

		def mergeDictIfSet(key: str, value: typing.Any) -> None:
			if value:
				content.setdefault(key, {})
				content[key].update(value)

		mergeDictIfSet("cpu", self.cpus())
		mergeDictIfSet("gpu", self.gpus())
		mergeDictIfSet("temperature", self.temperatures())
		mergeDictIfSet("battery", self.batteries())
		mergeDictIfSet("battery", self.ups.batteries())
		mergeDictIfSet("memory", self.memories())
		mergeDictIfSet("network", self.network())
		mergeDictIfSet("io", self.io())
		mergeDictIfSet("disk", self.disks())
		assignIfSet("uptime", self.upTime())

		return content

	def handlerMonitor(self, context: RESTServerContext) -> None:
		context.header("Content-type", "application/json")
		context.write(json.dumps(self.all()))
