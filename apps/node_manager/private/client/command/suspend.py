from apps.node_manager.private.common import getHostPort
from bzd.http.client import HttpClient
from bzd.logging import Logger


def commandSuspend(server: str, logger: Logger) -> None:
	"""Suspend a machine running the server."""

	host, port = getHostPort(server)

	try:
		HttpClient.get(f"http://{host}:{port}/suspend")
	except Exception as e:
		logger.error(str(e))
