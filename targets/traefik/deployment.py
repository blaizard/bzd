import argparse
import json
import pathlib
import typing
import sys
import re
import dataclasses
import os
import time

from tools.bdl.generators.json.ast.ast import Ast, Parameters
from bzd.template.template import Template
from bzd.utils.ssh import SSH
from bzd_oci.run import ociPush


class Getter:

	def __init__(self, ast: Ast) -> None:
		self.ast = ast

	@property
	def dockers(self) -> typing.Iterable[Parameters]:
		for workload in ast.workloads:
			if workload.expression.symbol == "traefik.docker":
				yield workload.expression.parameters

	@staticmethod
	def hostToName(host: str) -> str:
		"""Convert a hostname into a sanitized name."""
		return re.sub(r"[^a-zA-Z0-9]+", '_', host)


def waitForTCP(url: str, retries: int = 20, delay: int = 1.0) -> None:
	for _ in range(retries):
		time.sleep(delay)

		try:
			response = urllib2.urlopen(url, timeout=5)
		except (socket.error, urllib2.URLError, httplib.BadStatusLine):
			continue

		if response.getcode() == 200:
			return True

	return False


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Traefik deployment.")
	parser.add_argument("--json", type=pathlib.Path, help="Path of the manifest.")
	parser.add_argument("--directory", default="bzd-deployment-traefik", help="The name of the directory to be used.")
	parser.add_argument("deploy", type=str, help="Location where to deploy the target.")
	args = parser.parse_args()

	data = json.loads(args.json.read_text())
	ast = Ast(data)
	accessor = Getter(ast)

	template = Template.fromPath(pathlib.Path(__file__).parent / "docker_compose.yml.btl")
	content = template.render(accessor)

	# This script should:
	# 1. Connect to the client and make sure a registry is running, if not, run it.
	# 2. Push the image to the local registry.
	# 3. Update docker-compose.yml file
	# 4. pull
	# 5. run
	# See: https://stackoverflow.com/questions/31575546/docker-image-push-over-ssh-distributed

	ssh = SSH.fromString(args.deploy)

	print(f"Copying {args.directory}/docker-compose.yml")
	ssh.command(["mkdir", "-p", args.directory])
	ssh.copyContent(content, f"{args.directory}/docker-compose.yml")
	print(f"Starting docker registry")
	ssh.command(["docker", "compose", "-f", f"{args.directory}/docker-compose.yml", "up", "-d", "registry"])

	with ssh.forwardPort(55555, waitHTTP=f"http://localhost:55555/v2/"):
		for docker in accessor.dockers:
			print(f"Pushing {docker.image}...")
			ociPush(docker.image, f"localhost:55555/{docker.image}", stdout=True, stderr=True, timeoutS=600)

	print(f"Pulling new images...")
	ssh.command(["docker", "compose", "-f", f"{args.directory}/docker-compose.yml", "pull"],
	            stdout=True,
	            stderr=True,
	            timeoutS=600)
	print(f"Restarting containers...")
	ssh.command(["docker", "compose", "-f", f"{args.directory}/docker-compose.yml", "up", "-d"])

	sys.exit(0)
