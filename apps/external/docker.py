import argparse
import typing
import socket
from contextlib import closing

from bzd.utils.run import localDocker
from bzd.utils.scheduler import Scheduler
from bzd.http.client import HttpClient


def findFreePort() -> int:
	"""Find a free port on the local machine."""

	with closing(socket.socket(socket.AF_INET, socket.SOCK_STREAM)) as s:
		s.bind(('localhost', 0))
		s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		return s.getsockname()[1]


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


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Run a docker image.")
	parser.add_argument("--expose", type=int, default=None, help="Port to expose.")
	parser.add_argument("image", type=str, help="The docker image to run.")
	args = parser.parse_args()

	exposedPort: typing.Optional[int] = None

	command = ["run", "--rm"]  #, "-it"]
	if args.expose is not None:
		exposedPort = findFreePort()
		command += ["-p", f"{exposedPort}:{args.expose}"]
	command += [args.image]

	scheduler = Scheduler(blocking=False)
	scheduler.add(name="ping", intervalS=0.5, callback=httpServerReady, args=(exposedPort, ))
	scheduler.start()

	print(f"Seting up application...")
	localDocker(command, timeoutS=None)  #, stdin=True, stdout=True, stderr=True)
