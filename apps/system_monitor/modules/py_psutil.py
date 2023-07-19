import psutil
import typing


class PyPsUtil:

	def available(self) -> bool:
		return True

	def getCPUs(self) -> typing.Any:
		return [*psutil.cpu_percent(interval=1, percpu=True)]

	def getTemperatures(self) -> typing.Any:

		data = {}
		temperatures = psutil.sensors_temperatures()
		for name, group in temperatures.items():
			data[name] = [item.current for item in group]
		return data

	def get(self) -> typing.Any:

		data = {"cpus": self.getCPUs(), "temperatures": self.getTemperatures()}

		return data
