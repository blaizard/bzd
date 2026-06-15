import os
import typing
import threading
import subprocess
import shlex
from contextlib import contextmanager

from apps.node_manager.private.common import getHostPort
from apps.node_manager.private.client.command.lease_period import leasePeriod
from bzd.http.client import HttpClient
from bzd.logging import Logger


@contextmanager
def leaseContext(
	name: str,
	server: str,
	ttl: int,
	logger: Logger,
	leaseId: typing.Optional[str] = None,
	httpClient: typing.Any = HttpClient,
) -> typing.Generator[threading.Event, None, None]:
	"""Register a workload, run a heartbeat and release the workload when completed."""

	host, port = getHostPort(server)
	baseUrl = f"http://{host}:{port}"
	stop = threading.Event()

	if leaseId is None:
		# Register the workload
		leaseId = leasePeriod(baseUrl=baseUrl, name=name, ttl=ttl, httpClient=httpClient)

	def heartbeatLoop() -> None:
		consecutiveFailures = 0
		while not stop.is_set():
			# Sleep for half of the TTL to ensure we heartbeat in time,
			# but at most 10 seconds.
			# Also don't wait forever if we want to stop.
			if stop.wait(min(ttl / 2, 10)):
				break
			try:
				httpClient.post(f"{baseUrl}/workload/heartbeat", json={"id": leaseId, "ttl": ttl})
				consecutiveFailures = 0
			except Exception as e:
				consecutiveFailures += 1
				logger.error(f"Heartbeat failed: {e}")
				if consecutiveFailures > 2:
					stop.set()

	heartbeatThread = threading.Thread(target=heartbeatLoop, daemon=True)
	heartbeatThread.start()

	try:
		yield stop

	finally:
		stop.set()
		try:
			httpClient.post(f"{baseUrl}/workload/release", json={"id": leaseId})
		except Exception as e:
			logger.error(f"Failed to release workload: {e}")


def commandLease(
	server: str,
	name: str,
	ttl: int,
	command: typing.List[str],
	logger: Logger,
	leaseId: typing.Optional[str] = None,
	undefine: typing.Optional[typing.List[str]] = None,
	httpClient: typing.Any = HttpClient,
) -> int:
	"""Register a workload, run a heartbeat and release the workload when completed."""

	with leaseContext(name=name, server=server, ttl=ttl, leaseId=leaseId, logger=logger, httpClient=httpClient) as stop:
		env = os.environ.copy()
		for name in undefine or []:
			env.pop(name, None)

		# Run the process in a shell, to mimic what a user would do when calling the script.
		# This is needed by some program which need to think they are in an interactive shell.
		with subprocess.Popen(
			shlex.join(command), cwd=os.environ.get("BUILD_WORKSPACE_DIRECTORY", None), env=env, shell=True
		) as process:
			while not stop.is_set():
				try:
					process.wait(timeout=1.0)
					return process.returncode
				except subprocess.TimeoutExpired:
					continue
			process.terminate()
			logger.error("Aborting.")
			return 1
