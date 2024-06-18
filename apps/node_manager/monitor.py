import psutil
import typing
import json

from apps.node_manager.rest_server import RESTServerContext
"""Data format output:

```json
{
    "temperatures": {
        "cpu": [60, 56],
        "gpu": [67, 63, 57, 59]
    },
    "cpus": {
		"main": [12.1, 0, 4, 34.5]
	}
}
```

Temperatures are in Celsius.
CPU loads are in percent.
"""


def monitorTemperatures() -> typing.Any:
	data = {}
	temperatures = psutil.sensors_temperatures()
	for name, group in temperatures.items():
		data[name] = [item.current for item in group]
	return data


def monitorCPUs() -> typing.Any:
	return {"main": [*psutil.cpu_percent(interval=1, percpu=True)]}


def monitorBatteries() -> typing.Any:
	maybeBattery = psutil.sensors_battery()
	if maybeBattery is None:
		return []
	else:
		return {"main": [maybeBattery[0]]}


def monitorMemories() -> typing.Any:
	virtual = psutil.virtual_memory()
	swap = psutil.swap_memory()
	return {"ram": [virtual.used, virtual.total], "swap": [swap.used, swap.total]}


def monitorDisks() -> typing.Any:
	disks = {}
	for partition in psutil.disk_partitions():
		if partition.device not in disks:
			usage = psutil.disk_usage(partition.mountpoint)
			disks[partition.device] = [usage.used, usage.total]
	return disks


def monitor() -> typing.Any:
	return {
	    "cpus": monitorCPUs(),
	    "temperatures": monitorTemperatures(),
	    "batteries": monitorBatteries(),
	    "memories": monitorMemories(),
	    "disks": monitorDisks()
	}


def handlerMonitor(context: RESTServerContext) -> None:
	context.header("Content-type", "application/json")
	context.write(json.dumps(monitor()))


handlersMonitor = {"/monitor": handlerMonitor}
