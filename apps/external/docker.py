import argparse
import typing
import socket
import pathlib
import json
import tempfile
from contextlib import closing

from bzd.utils.run import localCommand
from bzd.utils.scheduler import Scheduler
from bzd.http.client import HttpClient
from bzd.utils.to_yaml import toYaml


def findFreePort() -> int:
	"""Find a free port on the local machine."""

	with closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as s:
		s.bind(("localhost", 0))
		s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		return int(s.getsockname()[1])


def httpServerReady(port: int) -> bool:
	"""Check that the HTTP server is ready."""

	try:
		url = f"http://localhost:{port}"
		HttpClient.get(url)
		print(f"Application exposed at {url}")
		return True
	except Exception as e:
		pass
	return False


class DockerCompose:
	"""Modifier class for docker compose files."""

	def __init__(self, path: pathlib.Path) -> None:
		self.data = json.loads(path.read_text())

	def _getService(self, service: str) -> typing.Dict[str, typing.Any]:
		"""Get a specific service from the docker-compose schema."""

		assert service in self.data[
		    "services"], f"The service '{service}' is not present in the docker-compose.yaml file."
		return self.data["services"][service]  # type: ignore

	def prepare(self, service: str) -> None:
		"""Prepare the service for being used."""

		serviceData = self._getService(service)
		# This tells Docker to use a lightweight init process as the container's main entrypoint.
		# This special process is designed to do one thing well: receive signals (like Ctrl+C)
		# and properly forward them to your application.
		serviceData["init"] = True

	def addExposedPort(self, service: str, local: int, docker: int) -> None:
		"""Add an exposed port to a specific service."""

		serviceData = self._getService(service)
		serviceData.setdefault("ports", []).append(f"{local}:{docker}")

	def toYaml(self) -> str:
		"""Get the updated data of the docker-compose schema."""

		return toYaml(self.data)


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Run a docker image.")
	parser.add_argument("--expose", type=int, default=None, help="Port to expose.")
	parser.add_argument("-v", "--verbose", action="store_true", help="Display the output as it goes.")
	parser.add_argument("service", type=str, help="The name of the service.")
	parser.add_argument("docker_compose", type=pathlib.Path, help="The docker-compose file in json format.")
	args = parser.parse_args()

	exposedPort: typing.Optional[int] = None
	dockerCompose = DockerCompose(args.docker_compose)
	dockerCompose.prepare(args.service)

	if args.expose is not None:
		exposedPort = findFreePort()
		dockerCompose.addExposedPort(args.service, exposedPort, args.expose)

	yaml = dockerCompose.toYaml()

	scheduler = Scheduler(blocking=False)
	if exposedPort is not None:
		scheduler.add(name="ping", intervalS=0.5, callback=httpServerReady, args=(exposedPort, ))
	scheduler.start()

	kwargs: typing.Dict[str, typing.Any] = {}
	if args.verbose:
		kwargs["stdout"] = True
		kwargs["stderr"] = True

	with tempfile.NamedTemporaryFile(delete_on_close=False) as f:
		f.write(yaml.encode())
		f.close()

		print(f"Setting up service '{args.service}', please wait...")
		try:
			localCommand(["docker", "compose", "--file", f.name, "run", "--service-ports", "--rm", args.service],
			             timeoutS=None,
			             **kwargs)
		finally:
			localCommand(["docker", "compose", "--file", f.name, "down", "--remove-orphans"], ignoreFailure=True)
