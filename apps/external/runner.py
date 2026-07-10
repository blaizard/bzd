import argparse
import typing
import socket
import pathlib
import json
from contextlib import closing

from bzd.utils.run import localCommand
from bzd.utils.scheduler import Scheduler
from bzd.http.client import HttpClient


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
	except Exception:
		pass
	return False


class DockerComposeService:
	"""Modifier class for docker compose service."""

	def __init__(self, data: typing.Dict[str, typing.Any], projectDirectory: pathlib.Path) -> None:
		self.data = data
		self.projectDirectory = projectDirectory

	def set(self, key: str, value: typing.Any) -> None:
		"""Set a key in the service."""

		self.data[key] = value

	def toCLI(self) -> typing.Tuple[typing.List[str], typing.List[int]]:
		"""Generate the CLI arguments for the service."""

		args = []
		exposedPorts = []
		image = None
		for key, value in self.data.items():
			if key == "container_name":
				args += ["--name", value]
			elif key == "image":
				image = value
			elif key == "volumes":
				for volume in value:
					if volume.startswith("."):
						volume = str(self.projectDirectory / volume)
					elif volume.startswith("~"):
						volume = str(pathlib.Path.home() / volume[2:])
					args += ["-v", volume]
			elif key == "environment":
				if isinstance(value, dict):
					for env, envValue in value.items():
						args += ["-e", f"{env}={envValue}"]
				elif isinstance(value, list):
					for env in value:
						args += ["-e", env]
			elif key == "ports":
				for port in value:
					if ":" not in str(port):
						exposedPort = findFreePort()
						port = f"{exposedPort}:{port}"
						exposedPorts.append(exposedPort)
					args += ["-p", str(port)]
			elif key == "devices":
				for device in value:
					args += ["--device", str(device)]
			elif key == "cap_add":
				for cap in value:
					args += ["--cap-add", str(cap)]
			elif key == "cap_drop":
				for cap in value:
					args += ["--cap-drop", str(cap)]
			elif key == "init":
				if value:
					args += ["--init"]
			elif key == "logging":
				assert value is None, f"Unsupported logging configuration: {value}"
				args += ["--log-driver", "none"]
			else:
				raise Exception(f"Unsupported key: {key}={value}")
		assert image is not None, "The service must have an image."
		args += [image]
		return args, exposedPorts


class DockerCompose:
	"""Modifier class for docker compose files."""

	def __init__(self, path: pathlib.Path, projectDirectory: pathlib.Path) -> None:
		self.data = json.loads(path.read_text())
		self.projectDirectory = projectDirectory

	def getService(self, service: str) -> DockerComposeService:
		"""Get a specific service from the docker-compose schema."""

		assert service in self.data["services"], f"The service '{service}' is not present in the docker-compose.yaml file."
		return DockerComposeService(self.data["services"][service], projectDirectory=self.projectDirectory)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Run an OCI image.")
	parser.add_argument("--oci-runner", type=pathlib.Path, required=True, help="The OCI runner to use.")
	parser.add_argument(
		"--docker-compose", type=pathlib.Path, required=True, help="The docker-compose file in json format."
	)
	parser.add_argument("--project-directory", type=pathlib.Path, required=True, help="The project directory.")
	parser.add_argument("-v", "--verbose", action="store_true", help="Display the output as it goes.")
	parser.add_argument("service", type=str, help="The name of the service.")
	args = parser.parse_args()

	dockerCompose = DockerCompose(args.docker_compose, projectDirectory=args.project_directory)
	service = dockerCompose.getService(args.service)
	# This tells Docker to use a lightweight init process as the container's main entrypoint.
	# This special process is designed to do one thing well: receive signals (like Ctrl+C)
	# and properly forward them to your application.
	service.set("init", True)
	# Disable logging.
	service.set("logging", None)
	argsOCI, exposedPorts = service.toCLI()

	if exposedPorts:
		scheduler = Scheduler(blocking=False)
		for exposedPort in exposedPorts:
			scheduler.add(name=f"ping-{exposedPort}", intervalS=0.5, callback=httpServerReady, args=(exposedPort,))
		scheduler.start()

	kwargs: typing.Dict[str, typing.Any] = {}
	if args.verbose:
		kwargs["stdout"] = True
		kwargs["stderr"] = True

	localCommand([str(args.oci_runner)] + argsOCI, timeoutS=None, **kwargs)
