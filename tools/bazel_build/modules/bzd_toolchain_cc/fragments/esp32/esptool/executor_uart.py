import argparse
import typing
import sys
import shlex
import pathlib

from bzd.utils.uart import Devices, Device, Uart
from bzd.utils.run import localPython
from bzd.utils import logging
from fragments.esp32.esptool.targets import targets


def getDevice(args: argparse.Namespace) -> Device:
	"""Search for the most suitable device to program the target."""

	if args.device is not None:
		return typing.cast(Device, args.device)

	target = args.target
	devices = Devices()
	if not devices.empty():
		filteredDevices = devices.filterByVidPids(targets.get(target, {}).get("usbIds", []))  # type: ignore
		if not filteredDevices.empty():
			accessibleDevices = filteredDevices.filterByAccess()
			if accessibleDevices.empty():
				logging.error(
				    f"The device(s) {str(filteredDevices.get())} is(are) a match for {target}, but you don't have the necessary permission for access."
				)
				logging.error("Use the following command and reboot: sudo usermod -a -G dialout $USER")
				sys.exit(1)
			return accessibleDevices.get()[0]

		logging.info(f"No device compatible with {target}, please use one from the following list:")
		for device, description in devices.getInfo().items():
			logging.info(f"- {device}: {description}")
		sys.exit(1)
	logging.error(f"No device found.")
	sys.exit(1)


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="ESP32 executor script.")
	parser.add_argument("--device", type=Device, help="Device.")
	parser.add_argument("--baud_rate", type=int, default=115200, help="Baudrate of the communication.")
	parser.add_argument("--data_bits", type=int, default=8, help="Data bits.")
	parser.add_argument("--parity", choices=["none", "even", "odd", "mark", "space"], default="none", help="Parity.")
	parser.add_argument("--stop_bits", type=float, default=1, help="Number of stop bits.")
	parser.add_argument("--flow_control",
	                    choices=["none", "xonxoff", "rtscts", "dsrdtr"],
	                    default="none",
	                    help="Flow control.")
	parser.add_argument("--target", choices=targets.keys(), default="esp32", help="Target.")
	parser.add_argument("--esptool", default="esptool.py", help="Path of the esptool.")
	parser.add_argument("elf", type=str, help="Binary in ELF format to be executed.")
	parser.add_argument("image", type=str, help="Binary image to be executed.")

	args = parser.parse_args()

	assert pathlib.Path(args.esptool).is_file(), f"Invalid path for esptool: {args.esptool}"

	target = targets[args.target]
	device = getDevice(args)

	logging.info(f"Programming {args.target} target through device {device}...")

	commandArgs = str(target["args"]).format(
	    device=device,
	    memory=" ".join([
	        "0x{:x} {}".format(offset, f.format(binary=args.image))
	        for offset, f in target["memoryMap"].items()  # type: ignore
	    ]))

	result = localPython(script=args.esptool,
	                     args=shlex.split(commandArgs),
	                     ignoreFailure=True,
	                     stdout=True,
	                     stderr=True)

	if result.getReturnCode() != 0:
		logging.error(f"Operation failed with return code {result.getReturnCode()}.")
		logging.info("Troubleshooting:")
		logging.info("- Connection failed `Timed out waiting for packet header`: try to press the BOOT button.")
		sys.exit(1)

	uart = Uart(device=device,
	            baudrate=args.baud_rate,
	            dataBits=args.data_bits,
	            stopBits=args.stop_bits,
	            parity=args.parity,
	            controlFlow=args.flow_control)
	uart.start()
