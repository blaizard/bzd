import serial
import serial.tools.list_ports
import os
import typing
import sys

Device = str


class Devices:
	"""List and filter the available serial devices."""

	def __init__(self, devices: typing.Optional[typing.List[typing.Any]] = None) -> None:
		self.devices = (sorted(serial.tools.list_ports.comports()) if devices is None else devices)  # type: ignore

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

	def empty(self) -> bool:
		"""Check if the set of remaining devices is empty or not."""
		return len(self.devices) == 0


class Uart:
	"""Creates a terminal as a proxy of a UART serial port."""

	dataBitsMapping = {
	    5: serial.FIVEBITS,
	    6: serial.SIXBITS,
	    7: serial.SEVENBITS,
	    8: serial.EIGHTBITS,
	}
	parityMapping = {
	    "none": serial.PARITY_NONE,
	    "even": serial.PARITY_EVEN,
	    "odd": serial.PARITY_ODD,
	    "mark": serial.PARITY_MARK,
	    "space": serial.PARITY_SPACE,
	}
	stopBitsMapping = {
	    1: serial.STOPBITS_ONE,
	    1.5: serial.STOPBITS_ONE_POINT_FIVE,
	    2: serial.STOPBITS_TWO,
	}
	controlFlowMapping = {"none", "xonxoff", "rtscts", "dsrdtr"}

	def __init__(
	    self,
	    device: Device,
	    baudrate: int = 115200,
	    dataBits: int = 8,
	    stopBits: float = 1.0,
	    parity: str = "none",
	    controlFlow: str = "none",
	) -> None:
		assert dataBits in self.dataBitsMapping, f"Unsupported data bits: {dataBits}"
		assert stopBits in self.stopBitsMapping, f"Unsupported stop bits: {stopBits}"
		assert parity in self.parityMapping, f"Unsupported parity: {parity}"
		assert (controlFlow in self.controlFlowMapping), f"Unsupported control flow: {controlFlow}"

		self.serial = serial.Serial(  # type: ignore
		    port=device,
		    baudrate=baudrate,
		    bytesize=self.dataBitsMapping[dataBits],
		    parity=self.parityMapping[parity],
		    stopbits=self.stopBitsMapping[stopBits],
		    xonxoff=(controlFlow == "xonxoff"),
		    rtscts=(controlFlow == "rtscts"),
		    dsrdtr=(controlFlow == "dsrdtr"),
		    timeout=1,
		)

	def start(self) -> None:
		with self.serial as serialInstance:
			while True:
				line = serialInstance.readline()
				if line:
					sys.stdout.write(line.decode(errors="ignore"))
					sys.stdout.flush()
