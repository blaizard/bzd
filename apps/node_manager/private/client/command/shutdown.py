from apps.node_manager.private.common import getHostPort
from bzd.http.client import HttpClient
from bzd.logging import Logger


def commandShutdown(server: str, logger: Logger) -> None:
	"""Shutdown a machine running the server."""

	host, port = getHostPort(server)

	try:
		HttpClient.get(f"http://{host}:{port}/shutdown")
	except Exception as e:
		logger.error(str(e))
