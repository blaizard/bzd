import typing

from apps.node_manager.private.client.command.common import getHostPort
from bzd.http.client import HttpClient


def leasePeriod(baseUrl: str, name: str, ttl: int, httpClient: typing.Any) -> str:
	# Register the workload
	try:
		response = httpClient.post(f"{baseUrl}/workload/register", json={"name": name, "ttl": ttl})
		return str(response.text)
	except Exception as e:
		raise RuntimeError(f"Workload '{name}' failed to register") from e


def commandLeasePeriod(
	server: str,
	name: str,
	ttl: int,
	httpClient: typing.Any = HttpClient,
) -> None:
	"""Register a workload for a given period of time.

	Args:
		server: The ip:port address for the machine to register the workload.
		name: The name of the workload.
		ttl: The time-to-live of the workload lease in seconds.
		httpClient: The http client to use for the request, defaults to HttpClient.

	Returns:
		The UID of the workload on the server.
	"""

	host, port = getHostPort(server)
	baseUrl = f"http://{host}:{port}"
	leasePeriod(baseUrl=baseUrl, name=name, ttl=ttl, httpClient=httpClient)
