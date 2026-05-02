import argparse
import sys
import typing

from apps.node_manager.private.client.command.wake_on_lan import commandWakeOnLan
from apps.node_manager.private.client.command.suspend import commandSuspend
from apps.node_manager.private.client.command.shutdown import commandShutdown
from apps.node_manager.private.client.command.lease import commandLease
from apps.node_manager.private.client.command.lease_period import commandLeasePeriod


def exceptionToString(exception: Exception) -> str:
	lines = [f"{type(exception).__name__}: {str(exception)}"]
	cause = exception.__cause__
	count = 0
	while cause:
		count += 1
		lines.append(f"{'  ' * count}-> Caused by {type(cause).__name__}: {str(cause)}")
		cause = cause.__cause__
		if count > 10:
			lines.append("Too many nested exceptions, stopping here.")
			break
	return "\n".join(lines)


def commandWoLLeasePeriod(
	mac: str, broadcast: str, service: str, wait: typing.List[str], timeout: int, server: str, name: str, ttl: int
) -> None:

	for attempt in range(1, 4):
		try:
			commandLeasePeriod(server=server, name=name, ttl=ttl)
			print(f"Lease secured for '{name}' for {ttl}s.", flush=True)
			return
		except Exception as e:
			print(exceptionToString(e), file=sys.stderr, flush=True)

		print(f"Lease attempt {attempt} for '{name}' failed. Sending WOL...", flush=True)
		try:
			commandWakeOnLan(mac=mac, broadcast=broadcast, service=service, wait=wait, timeout=timeout)
		except Exception as e:
			print(exceptionToString(e), file=sys.stderr, flush=True)

	raise Exception(f"Error: Could not secure lease for '{name}' after 3 attempts.")


def setupArgs(parser: argparse.ArgumentParser, args: typing.List[str]) -> None:
	all_args: typing.Dict[str, typing.Dict[str, typing.Any]] = {
		"broadcast": {
			"default": "255.255.255.255",
			"help": "The broadcast address to be used.",
		},
		"service": {
			"help": "A service to proxy the WOL call.",
		},
		"timeout": {
			"default": 60,
			"type": int,
			"help": "Timeout in seconds until which the check returns.",
		},
		"wait": {
			"action": "append",
			"default": [],
			"help": "Wait until a specific TCP connection is open.",
		},
		"mac": {"required": True, "help": "The mac address for the machine to wake up."},
		"server": {"required": True, "help": "The ip:port address for the machine to register the workload."},
		"undefine": {
			"action": "append",
			"default": [],
			"help": "Environment variable to undefine before running the workload.",
		},
		"name": {
			"default": "unknown",
			"help": "The name of the workload.",
		},
		"ttl": {
			"default": 60,
			"type": int,
			"help": "The time-to-live of the workload lease.",
		},
	}

	for arg in args:
		assert arg in all_args, f"Unknown argument '{arg}'."
		parser.add_argument(
			f"--{arg}",
			**all_args[arg],
		)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Wake On Lan client.")
	subparsers = parser.add_subparsers(help="Available sub-commands.", dest="command")

	wolParser = subparsers.add_parser("wol", help="Wake-up a machine from its MAC address.")
	setupArgs(wolParser, ["broadcast", "service", "timeout", "wait", "mac"])

	suspendParser = subparsers.add_parser("suspend", help="Suspend a machine running the server.")
	setupArgs(suspendParser, ["server"])

	shutdownParser = subparsers.add_parser("shutdown", help="Shutdown a machine running the server.")
	setupArgs(shutdownParser, ["server"])

	leaseParser = subparsers.add_parser(
		"lease", help="Register a workload, run a heartbeat and release the workload when completed."
	)
	setupArgs(leaseParser, ["server", "undefine", "name", "ttl"])
	leaseParser.add_argument("workload", nargs=argparse.REMAINDER, help="The command to execute.")

	leasePreriodParser = subparsers.add_parser("lease-period", help="Register a workload for a predefined period.")
	setupArgs(leasePreriodParser, ["server", "name", "ttl"])

	wolLeasePeriodParser = subparsers.add_parser("wol-lease-period", help="Wake-up a machine from its MAC address.")
	setupArgs(wolLeasePeriodParser, ["broadcast", "service", "timeout", "wait", "mac", "server", "name", "ttl"])

	args = parser.parse_args()

	try:
		if args.command == "wol":
			commandWakeOnLan(
				mac=args.mac,
				broadcast=args.broadcast,
				service=args.service,
				wait=args.wait,
				timeout=args.timeout,
			)
		elif args.command == "suspend":
			commandSuspend(server=args.server)
		elif args.command == "shutdown":
			commandShutdown(server=args.server)
		elif args.command == "lease":
			returnCode = commandLease(
				server=args.server,
				name=args.name,
				ttl=args.ttl,
				undefine=args.undefine,
				command=args.workload,
			)
			sys.exit(returnCode)
		elif args.command == "lease-period":
			commandLeasePeriod(
				server=args.server,
				name=args.name,
				ttl=args.ttl,
			)
		elif args.command == "wol-lease-period":
			commandWoLLeasePeriod(
				mac=args.mac,
				broadcast=args.broadcast,
				service=args.service,
				wait=args.wait,
				timeout=args.timeout,
				server=args.server,
				name=args.name,
				ttl=args.ttl,
			)
		else:
			assert False, f"Unknown command: '{args.command}'."
	except Exception as e:
		print(exceptionToString(e), file=sys.stderr, flush=True)
		sys.exit(1)
