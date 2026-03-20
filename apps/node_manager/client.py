import argparse

from apps.node_manager.private.client.command.wake_on_lan import commandWakeOnLan
from apps.node_manager.private.client.command.suspend import commandSuspend
from apps.node_manager.private.client.command.shutdown import commandShutdown

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Wake On Lan client.")
	subparsers = parser.add_subparsers(help="Available sub-commands.", dest="command")

	wolParser = subparsers.add_parser("wol", help="Wake-up a machine from its MAC address.")
	wolParser.add_argument(
		"-b",
		"--broadcast",
		default="255.255.255.255",
		help="The broadcast address to be used.",
	)
	wolParser.add_argument(
		"-s",
		"--service",
		help="A service to proxy the WOL call.",
	)
	wolParser.add_argument(
		"-t",
		"--timeout",
		default=60,
		type=int,
		help="Timeout in S until which the check returns.",
	)
	wolParser.add_argument(
		"-w",
		"--wait",
		action="append",
		default=[],
		help="Wait until a specific TCP connection is open.",
	)
	wolParser.add_argument("mac", help="The mac address for the machine to wake up.")

	suspendParser = subparsers.add_parser("suspend", help="Suspend a machine running the server.")
	suspendParser.add_argument("ip", help="The ip:port address for the machine to suspend.")

	shutdownParser = subparsers.add_parser("shutdown", help="Shutdown a machine running the server.")
	shutdownParser.add_argument("ip", help="The ip:port address for the machine to shutdown.")

	args = parser.parse_args()

	if args.command == "wol":
		commandWakeOnLan(
			mac=args.mac,
			broadcast=args.broadcast,
			service=args.service,
			wait=args.wait,
			timeout=args.timeout,
		)
	elif args.command == "suspend":
		commandSuspend(server=args.ip)
	elif args.command == "shutdown":
		commandShutdown(server=args.ip)
	else:
		assert False, f"Unknown command: '{args.command}'."
