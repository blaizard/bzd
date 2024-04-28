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


@dataclasses.dataclass
class ConnectionSSH:
	host: str
	port: int
	username: typing.Optional[str] = None
	password: typing.Optional[str] = None


class DeploySSH:

	def __init__(self, string: str) -> None:
		self.connection = DeploySSH.fromString(string)

	@staticmethod
	def fromString(string: str) -> ConnectionSSH:
		"""Decompose a SSH string and gets its components.
		
		A SSH string must have the following format:
		<username>:<password>@<host>:<port>, where only <host> is mandatory.
		"""

		def splitLocation(location: str) -> typing.Tuple[str, int]:
			items = location.split(":")
			if len(items) == 1:
				return items[0], 22
			elif len(items) == 2:
				return items[0], int(items[1])
			raise Exception("Location parsing error")

		def splitAuthentication(authentication: str) -> typing.Tuple[str, typing.Optional[str]]:
			items = authentication.split(":")
			if len(items) == 1:
				return items[0], None
			elif len(items) == 2:
				return items[0], items[1]
			raise Exception("Authentication parsing error")

		items = string.split("@")
		try:
			if len(items) == 1:
				host, port = splitLocation(items[0])
				return ConnectionSSH(host=host, port=port)
			elif len(items) == 2:
				username, password = splitAuthentication(items[0])
				host, port = splitLocation(items[1])
				return ConnectionSSH(host=host, port=port, username=username, password=password)
			raise Exception("String parsing error")
		except Exception as e:
			raise Exception(
			    f"{str(e)}: SSH string must have the following format: '<username>:<password>@<host>:<port>', not '{string}'."
			)


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
	parser.add_argument("deploy", type=str, help="Location where to deploy the target.")
	args = parser.parse_args()

	data = json.loads(args.json.read_text())
	ast = Ast(data)

	template = Template.fromPath(pathlib.Path(__file__).parent / "docker_compose.yml.btl")
	content = template.render(Getter(ast))

	# This script should:
	# 1. Connect to the client and make sure a registry is running, if not, run it.
	# 2. Push the image to the local registry.
	# 3. Update docker-compose.yml file
	# 4. pull
	# 5. run
	# See: https://stackoverflow.com/questions/31575546/docker-image-push-over-ssh-distributed

	print(content)

	sshArgs = DeploySSH.fromString(args.deploy)
	ssh = SSH(host=sshArgs.host, username=sshArgs.username)
	"""
	ssh.command(["mkdir", "-p", "bzd-deployment"])
	ssh.copyContent(content, "./bzd-deployment/docker-compose.yml")
	ssh.command(["docker", "compose", "-f", "./bzd-deployment/docker-compose.yml", "up", "-d", "registry"])
	"""

	ociPush("--help", "ssa", stdout=True, stderr=True)

	with ssh.forwardPort(55555, waitHTTP=f"http://localhost:55555/v2/"):
		# Push the images...
		# docker push localhost:55555/image:latest
		print("FORWARD!")
		time.sleep(1)

	sys.exit(0)
