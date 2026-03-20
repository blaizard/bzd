from apps.node_manager.private.client.command.common import getHostPort
from bzd.http.client import HttpClient


def commandShutdown(server: str) -> None:
	"""Shutdown a machine running the server."""

	host, port = getHostPort(server)

	try:
		HttpClient.get(f"http://{host}:{port}/shutdown")
	except Exception as e:
		print(str(e))
