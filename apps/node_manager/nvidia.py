import pynvml
import typing


class Nvidia:

	def __init__(self) -> None:
		try:
			pynvml.nvmlInit()  # type: ignore
			self.init = True
		except pynvml.NVMLError:
			self.init = False

	@property
	def handles(self) -> typing.Generator[typing.Tuple[str, typing.Any], None, None]:
		if not self.init:
			return None
		deviceCount = pynvml.nvmlDeviceGetCount()  # type: ignore
		for i in range(deviceCount):
			handle = pynvml.nvmlDeviceGetHandleByIndex(i)  # type: ignore
			name = pynvml.nvmlDeviceGetName(handle).lower()
			yield name, handle

	def memories(self) -> typing.Any:
		output = {}
		for name, handle in self.handles:
			info = pynvml.nvmlDeviceGetMemoryInfo(handle)  # type: ignore
			output[name] = {"used": info.used, "total": info.total}
		return output

	def temperatures(self) -> typing.Any:
		output = {}
		for name, handle in self.handles:
			temperature = pynvml.nvmlDeviceGetTemperature(handle, pynvml.NVML_TEMPERATURE_GPU)  # type: ignore
			output[name] = [temperature]
		return output

	def gpus(self) -> typing.Any:
		output = {}
		for name, handle in self.handles:
			utilization = pynvml.nvmlDeviceGetUtilizationRates(handle)  # type: ignore
			output[name] = [utilization.gpu / 100.]
		return output
