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
    "cpus": [12.1, 0, 4, 34.5]
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
	return [*psutil.cpu_percent(interval=1, percpu=True)]


def monitor() -> typing.Any:
	return {"cpus": monitorCPUs(), "temperatures": monitorTemperatures()}


def handlerMonitor(context: RESTServerContext) -> None:

	context.header("Content-type", "application/json")
	context.write(json.dumps(monitor()))


handlersMonitor = {"/monitor": handlerMonitor}
