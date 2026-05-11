import os
import typing
import threading
import subprocess

from apps.node_manager.private.client.command.common import getHostPort
from apps.node_manager.private.client.command.lease_period import leasePeriod
from bzd.http.client import HttpClient


def commandLease(
	server: str,
	name: str,
	ttl: int,
	command: typing.List[str],
	undefine: typing.Optional[typing.List[str]] = None,
	httpClient: typing.Any = HttpClient,
) -> int:
	"""Register a workload, run a heartbeat and release the workload when completed."""

	host, port = getHostPort(server)
	baseUrl = f"http://{host}:{port}"
	stop = threading.Event()

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
				print(f"Heartbeat failed: {e}")
				if consecutiveFailures > 2:
					stop.set()

	heartbeatThread = threading.Thread(target=heartbeatLoop, daemon=True)
	heartbeatThread.start()

	env = os.environ.copy()
	for name in undefine or []:
		env.pop(name, None)

	try:
		with subprocess.Popen(command, cwd=os.environ.get("BUILD_WORKSPACE_DIRECTORY", None), env=env) as process:
			while not stop.is_set():
				try:
					returnCode = process.wait(timeout=1.0)
					return returnCode
				except subprocess.TimeoutExpired:
					continue
			process.terminate()
			print("Aborting.")

	finally:
		stop.set()
		try:
			httpClient.post(f"{baseUrl}/workload/release", json={"id": leaseId})
		except Exception as e:
			print(f"Failed to release workload: {e}")
