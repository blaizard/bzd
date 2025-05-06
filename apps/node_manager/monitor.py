import psutil
import typing
import json
import time

from apps.node_manager.rest_server import RESTServerContext
from apps.node_manager.nvidia import Nvidia


class _Monitor:
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

	def __init__(self) -> None:
		self.nvidia = Nvidia()

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
		maybeBattery = psutil.sensors_battery()  # type: ignore
		if maybeBattery is None:
			return None
		return {"main": [maybeBattery[0] / 100.]}

	def memories(self) -> typing.Any:
		virtual = psutil.virtual_memory()  # type: ignore
		swap = psutil.swap_memory()  # type: ignore
		memories = {}
		if virtual.total:
			memories["ram"] = {"used": virtual.used, "total": virtual.total}
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

		return {",".join(sorted(data["path"])): data["data"] for data in disks.values()}

	def upTime(self) -> float:
		return time.time() - psutil.boot_time()  # type: ignore

	def all(self) -> typing.Any:

		content = {}

		def assignIfSet(key: str, value: typing.Any) -> None:
			if value:
				content[key] = value

		assignIfSet("cpu", self.cpus())
		assignIfSet("gpu", self.gpus())
		assignIfSet("temperature", self.temperatures())
		assignIfSet("battery", self.batteries())
		assignIfSet("memory", self.memories())
		assignIfSet("disk", self.disks())
		assignIfSet("uptime", self.upTime())

		return content


monitor = _Monitor()


def handlerMonitor(context: RESTServerContext) -> None:
	context.header("Content-type", "application/json")
	context.write(json.dumps(monitor.all()))


handlersMonitor = {"/monitor": handlerMonitor}
