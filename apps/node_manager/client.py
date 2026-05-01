import argparse
import sys

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


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Wake On Lan client.")
	subparsers = parser.add_subparsers(help="Available sub-commands.", dest="command")

	broadcast_kwargs = {
		"default": "255.255.255.255",
		"help": "The broadcast address to be used.",
	}
	service_kwargs = {
		"help": "A service to proxy the WOL call.",
	}
	timeout_kwargs = {
		"default": 60,
		"type": int,
		"help": "Timeout in seconds until which the check returns.",
	}
	wait_kwargs = {
		"action": "append",
		"default": [],
		"help": "Wait until a specific TCP connection is open.",
	}
	server_kwargs = {"required": True, "help": "The ip:port address for the machine to register the workload."}
	undefine_kwargs = {
		"action": "append",
		"default": [],
		"help": "Environment variable to undefine before running the workload.",
	}
	name_kwargs = {
		"default": "unknown",
		"help": "The name of the workload.",
	}
	ttl_kwargs = {
		"default": 60,
		"type": int,
		"help": "The time-to-live of the workload lease.",
	}

	wolParser = subparsers.add_parser("wol", help="Wake-up a machine from its MAC address.")
	wolParser.add_argument(
		"--broadcast",
		**broadcast_kwargs,  # type: ignore
	)
	wolParser.add_argument(
		"--service",
		**service_kwargs,  # type: ignore
	)
	wolParser.add_argument(
		"--timeout",
		**timeout_kwargs,  # type: ignore
	)
	wolParser.add_argument(
		"--wait",
		**wait_kwargs,  # type: ignore
	)
	wolParser.add_argument("mac", help="The mac address for the machine to wake up.")

	suspendParser = subparsers.add_parser("suspend", help="Suspend a machine running the server.")
	suspendParser.add_argument(
		"--server",
		**server_kwargs,  # type: ignore
	)

	shutdownParser = subparsers.add_parser("shutdown", help="Shutdown a machine running the server.")
	shutdownParser.add_argument(
		"--server",
		**server_kwargs,  # type: ignore
	)

	leaseParser = subparsers.add_parser(
		"lease", help="Register a workload, run a heartbeat and release the workload when completed."
	)
	leaseParser.add_argument(
		"--server",
		**server_kwargs,  # type: ignore
	)
	leaseParser.add_argument(
		"--undefine",
		**undefine_kwargs,  # type: ignore
	)
	leaseParser.add_argument(
		"--name",
		**name_kwargs,  # type: ignore
	)
	leaseParser.add_argument(
		"--ttl",
		**ttl_kwargs,  # type: ignore
	)
	leaseParser.add_argument("workload", nargs=argparse.REMAINDER, help="The command to execute.")

	leasePreriodParser = subparsers.add_parser("lease-period", help="Register a workload for a predefined period.")
	leasePreriodParser.add_argument(
		"--server",
		**server_kwargs,  # type: ignore
	)
	leasePreriodParser.add_argument(
		"--name",
		**name_kwargs,  # type: ignore
	)
	leasePreriodParser.add_argument(
		"--ttl",
		**ttl_kwargs,  # type: ignore
	)

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
		else:
			assert False, f"Unknown command: '{args.command}'."
	except Exception as e:
		print(exceptionToString(e), file=sys.stderr)
		sys.exit(1)
