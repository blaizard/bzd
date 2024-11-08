import pynvml
import typing


class Nvidia:

	def __init__(self) -> None:
		try:
			pynvml.nvmlInit()
			self.init = True
		except pynvml.NVMLError:
			self.init = False

	@property
	def handles(self) -> typing.Generator[typing.Tuple[str, typing.Any], None, None]:
		if not self.init:
			return None
		deviceCount = pynvml.nvmlDeviceGetCount()
		for i in range(deviceCount):
			handle = pynvml.nvmlDeviceGetHandleByIndex(i)
			name = pynvml.nvmlDeviceGetName(handle)
			yield name, handle

	def memories(self) -> typing.Any:
		output = {}
		for name, handle in self.handles:
			info = pynvml.nvmlDeviceGetMemoryInfo(handle)
			output[name] = [info.used, info.total]
		return output

	def gpus(self) -> typing.Any:
		for handle in self.handles:
			pass
