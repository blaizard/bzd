import psutil
import typing
import json
import time

from apps.node_manager.rest_server import RESTServerContext
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


def monitorTemperatures() -> typing.Any:
	data = {}
	temperatures = psutil.sensors_temperatures()  # type: ignore
	for name, group in temperatures.items():
		data[name] = [item.current for item in group]
	return data


def monitorCPUs() -> typing.Any:
	return {"main": [cpu / 100. for cpu in psutil.cpu_percent(interval=1, percpu=True)]}  # type: ignore


def monitorBatteries() -> typing.Any:
	maybeBattery = psutil.sensors_battery()  # type: ignore
	if maybeBattery is None:
		return None
	return {"main": [maybeBattery[0] / 100.]}


def monitorMemories() -> typing.Any:
	virtual = psutil.virtual_memory()  # type: ignore
	swap = psutil.swap_memory()  # type: ignore
	memories = {}
	if virtual.total:
		memories["ram"] = [virtual.used, virtual.total]
	if swap.total:
		memories["swap"] = [swap.used, swap.total]
	return memories


def monitorDisks() -> typing.Any:
	disks = {}
	for partition in psutil.disk_partitions():  # type: ignore
		if partition.device not in disks:
			usage = psutil.disk_usage(partition.mountpoint)  # type: ignore
			disks[partition.device] = [usage.used, usage.total]
	return disks


def upTime() -> float:
	return time.time() - psutil.boot_time()


def monitor() -> typing.Any:

	content = {}

	def assignIfSet(key: str, value: typing.Any) -> None:
		if value:
			content[key] = value

	assignIfSet("cpu", monitorCPUs())
	assignIfSet("temperature", monitorTemperatures())
	assignIfSet("battery", monitorBatteries())
	assignIfSet("memory", monitorMemories())
	assignIfSet("disk", monitorDisks())
	assignIfSet("uptime", upTime())

	return content


def handlerMonitor(context: RESTServerContext) -> None:
	context.header("Content-type", "application/json")
	context.write(json.dumps(monitor()))


handlersMonitor = {"/monitor": handlerMonitor}
