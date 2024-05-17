import argparse
import json
import pathlib
import typing
import sys
import re
import dataclasses
import os
import time

from bdl.generators.json.ast.ast import Ast, Parameters, Expression
from bzd.template.template import Template
from bzd.utils.ssh import SSH
from bzd_oci.run import ociPush


class CommonParameters:

	def __init__(self, port: int) -> None:
		self.port = port

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


class Docker:

	def __init__(self, ast: Ast, symbol: str) -> None:
		self.ast = ast
		self.expressions = {
		    fqn: entity.expression.parameters
		    for fqn, entity in ast.registry.items() if entity.expression.symbol == symbol
		}

	def makeDockerComposeForAllInstances(self) -> typing.Tuple[typing.Dict[str, str], typing.Set[str]]:
		"""Generate content for the docker compose files.
		
		This includes the docker compose yaml content as string keyed per instance and a set
		containing all images.
		"""
		dockerComposeFiles = {}
		images = set()
		for fqn, expression in self.expressions.items():
			currentDockerCompose, currentImages = self.makeDockerCompose(fqn, expression)
			dockerComposeFiles[fqn] = currentDockerCompose
			images.update(currentImages)
		return dockerComposeFiles, images

	def getInterfaces(self, fqn: str, interface: str) -> typing.List[Parameters]:
		"""Gather object instances of a specific type."""

		return [
		    workload.expression.parameters for workload in self.ast.workloads
		    if workload.expression.symbol == f"{fqn}.{interface}"
		]


class DockerTraefik(Docker):

	def __init__(self, ast: Ast, common: CommonParameters) -> None:
		super().__init__(ast=ast, symbol="docker.Traefik")
		self.common = common

	def makeDockerCompose(self, fqn: str, expression: Parameters) -> typing.Tuple[str, typing.Set[str]]:
		dockers = self.getInterfaces(fqn, "add")
		template = Template.fromPath(pathlib.Path(__file__).parent / "traefik.yml.btl")
		content = template.render({"dockers": dockers}, self.common)
		return content, set([docker.image for docker in dockers])


class DockerCompose(Docker):

	def __init__(self, ast: Ast, common: CommonParameters) -> None:
		super().__init__(ast=ast, symbol="docker.Compose")
		self.common = common

	def makeDockerCompose(self, fqn: str, expression: Parameters) -> typing.Tuple[str, typing.Set[str]]:
		dockers = self.getInterfaces(fqn, "image")
		return "", set([docker.image for docker in dockers])


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Traefik deployment.")
	parser.add_argument("--json", type=pathlib.Path, help="Path of the manifest.")
	parser.add_argument("--directory",
	                    type=pathlib.Path,
	                    default=pathlib.Path("bzd-deployment"),
	                    help="The name of the directory to be used.")
	parser.add_argument("--registry-port", type=int, default=5000, help="Port for the registry.")
	parser.add_argument("transport", type=str, help="Location where to deploy the target.")
	args = parser.parse_args()

	data = json.loads(args.json.read_text())
	ast = Ast(data)

	common = CommonParameters(port=args.registry_port)

	# Generate docker compose content
	traefikDockerCompose, traefikImages = DockerTraefik(ast=ast, common=common).makeDockerComposeForAllInstances()
	composeDockerCompose, composeImages = DockerCompose(ast=ast, common=common).makeDockerComposeForAllInstances()
	dockerCompose = traefikDockerCompose  #| composeDockerCompose
	images = traefikImages | composeImages

	ssh = SSH.fromString(args.transport)

	# Variants are:
	# transport: none, ssh
	# type: traefik, docker-compose, docker-registry

	registry = Template.fromPath(pathlib.Path(__file__).parent / "registry.yml.btl").render(common)

	applicationsDirectory = args.directory / "apps"

	with ssh.interactive() as handle:

		handle.command(["mkdir", "-p", str(applicationsDirectory)])

		print(f"Copying '{args.directory}/docker-compose.yml'.")
		handle.uploadContent(registry, f"{args.directory}/docker-compose.yml")

		print(f"Starting docker registry.")
		handle.command(["docker", "compose", "-f", f"{args.directory}/docker-compose.yml", "up", "-d", "registry"])

		# Updating images
		with ssh.forwardPort(args.registry_port, waitHTTP=f"http://localhost:{args.registry_port}/v2/"):
			for image in images:
				print(f"Pushing {image}...")
				ociPush(image, f"localhost:{args.registry_port}/{image}", stdout=True, stderr=True, timeoutS=600)

		# Find all applications.
		applications = set(dockerCompose.keys())
		result = handle.command(["find", str(applicationsDirectory), "-maxdepth", "1", "-type", "d"])
		currentApplications = set(filter(bool, [d.strip().split("/")[-1] for d in result.getOutput().split("\n")][1:]))
		print(f"Applications currently deployed: {', '.join(currentApplications)}")
		print(f"Applications: {', '.join(applications)}")

		# Remove applications that are currently present but not needed anymore.
		removeApplications = currentApplications - applications
		for application in removeApplications:
			directory = f"{applicationsDirectory}/{application}"
			print(f"Stopping '{directory}'...")
			handle.command(["docker", "compose", "-f", f"{directory}/docker-compose.yml", "down"], ignoreFailure=True)
			handle.command(["rm", "-rfd", f"{directory}"])

		# Start applications
		for application, content in dockerCompose.items():
			directory = f"{applicationsDirectory}/{application}"
			print(f"Copying '{directory}/docker-compose.yml'.")
			handle.command(["mkdir", "-p", directory])
			handle.uploadContent(content, f"{directory}/docker-compose.yml")
			print(f"Pulling new images.")
			handle.command(["docker", "compose", "-f", f"{directory}/docker-compose.yml", "pull"])
			print(f"Restarting containers.")
			handle.command(["docker", "compose", "-f", f"{directory}/docker-compose.yml", "up", "-d"])

	sys.exit(0)
