import argparse
import typing
import sys
import logging
import shlex
import bzd.env

from python.bzd.utils.uart import Devices, Device, Uart
from python.bzd.utils.run import localPython

targets = {
	"esp32": {
	"usbIds": [(0x10c4, 0xea60)  # Silicon Labs CP210x devices (USB/UART bridge)
				],
	"args":
	"--chip esp32 --port {device} --baud 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 toolchains/cc/fragments/esptool/bin/esp32/bootloader.bin 0x10000 {binary} 0x8000 toolchains/cc/fragments/esptool/bin/esp32/partitions_singleapp.bin"
	}
}


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
	logging.info(f"No device found.")
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
	parser.add_argument("binary", type=str, help="Binary to be executed.")

	args = parser.parse_args()

	device = getDevice(args)
	logging.info(f"Programming {args.target} target through device {device}...")

	commandArgs = str(targets[args.target]["args"]).format(device=device, binary=args.binary)

	result = localPython(script="external/esptool/esptool.py",
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
