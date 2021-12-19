import serial
import serial.tools.list_ports
import os
import typing

Device = str


class Devices:
	"""List and filter the available serial devices."""

	def __init__(self, devices: typing.Optional[typing.List[typing.Any]] = None) -> None:
		self.devices = sorted(serial.tools.list_ports.comports()) if devices is None else devices

	def filterByVidPids(self, vidPids: typing.Sequence[typing.Tuple[int, int]]) -> "Devices":
		"""Filter ports by VIDs and PIDs."""
		return Devices([
			info for info in self.devices
			if any([True for vid, pid in vidPids if (info.vid == vid and info.pid == pid)])
		])

	def filterByAccess(self) -> "Devices":
		"""Filter ports by user permission."""
		return Devices([info for info in self.devices if os.access(info.device, os.R_OK | os.W_OK)])

	def get(self) -> typing.List[Device]:
		"""Get the list of devices that remains."""
		return [info.device for info in self.devices]

	def getInfo(self) -> typing.Dict[Device, str]:
		"""Get information about the remaining devices."""
		return {info.device: "{}, {}".format(info.description, info.hwid) for info in self.devices}


if __name__ == "__main__":

	devices = Devices()
	print(devices.getInfo())
