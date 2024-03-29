import argparse
import typing
import sys
import pathlib

from bzd.utils.uart import Devices, Device, Uart
from bzd.utils.run import localPython
from bzd.utils.logging import Logger
from bzd.utils.binary_builder import makeBinary


def getDevice(vidPids: typing.List[typing.Tuple[int, int]], device=typing.Optional[Device]) -> Device:
	"""Search for the most suitable device to program the target."""

	if device is not None:
		return device

	devices = Devices()
	if not devices.empty():
		filteredDevices = devices.filterByVidPids(vidPids)
		if not filteredDevices.empty():
			accessibleDevices = filteredDevices.filterByAccess()
			if accessibleDevices.empty():
				Logger.error(
				    f"The device(s) {str(filteredDevices.get())} is(are) a match, but you don't have the necessary permission for access."
				)
				Logger.error("Use the following command and reboot: sudo usermod -a -G dialout $USER")
				sys.exit(1)
			return accessibleDevices.get()[0]

		Logger.info(f"No device compatible, please use one from the following list:")
		for device, description in devices.getInfo().items():
			Logger.info(f"- {device}: {description}")
		sys.exit(1)
	Logger.error(f"No device found.")
	sys.exit(1)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="ESP32 executor script.")
	parser.add_argument("--device", type=Device, default=None, help="Device.")
	parser.add_argument("--device_vid_pid",
	                    dest="deviceIds",
	                    nargs=2,
	                    action="append",
	                    default=[],
	                    help="Device identifiers to be used for auto-detection (VID + PID).")
	parser.add_argument("--baud_rate", type=int, default=115200, help="Baudrate of the communication.")
	parser.add_argument("--data_bits", type=int, default=8, help="Data bits.")
	parser.add_argument(
	    "--parity",
	    choices=["none", "even", "odd", "mark", "space"],
	    default="none",
	    help="Parity.",
	)
	parser.add_argument("--stop_bits", type=float, default=1, help="Number of stop bits.")
	parser.add_argument(
	    "--flow_control",
	    choices=["none", "xonxoff", "rtscts", "dsrdtr"],
	    default="none",
	    help="Flow control.",
	)
	parser.add_argument("--esptool", default="esptool.py", help="Path of the esptool.")
	parser.add_argument("--size", default=None, help="The total size of the binary in bytes.")
	parser.add_argument(
	    "--chunk",
	    dest="chunks",
	    action="append",
	    default=[],
	    help="Add chunks at specific addresses using the following format: 'path,address[,max-size]'.",
	)

	args, esptoolArgs = parser.parse_known_args()
	if esptoolArgs[0] == "--":
		esptoolArgs.pop(0)

	assert pathlib.Path(args.esptool).is_file(), f"Invalid path for esptool: {args.esptool}"

	binary = makeBinary(chunks=args.chunks, size=args.size)
	device = getDevice(vidPids=[[int(vid, 0), int(pid, 0)] for vid, pid in args.deviceIds], device=args.device)

	Logger.info(f"Programming target through device {device}...")

	esptoolArgs = ["--port", device] + esptoolArgs
	for chunk in binary.chunks:
		esptoolArgs += ["0x{:x}".format(chunk.address), str(chunk.path)]

	result = localPython(
	    script=args.esptool,
	    args=esptoolArgs,
	    ignoreFailure=True,
	    stdout=True,
	    stderr=True,
	)

	if result.getReturnCode() != 0:
		Logger.error(f"Operation failed with return code {result.getReturnCode()}.")
		Logger.info("Troubleshooting:")
		Logger.info("- Connection failed `Timed out waiting for packet header`: try to press the BOOT button.")
		sys.exit(1)

	uart = Uart(
	    device=device,
	    baudrate=args.baud_rate,
	    dataBits=args.data_bits,
	    stopBits=args.stop_bits,
	    parity=args.parity,
	    controlFlow=args.flow_control,
	)
	uart.start()
