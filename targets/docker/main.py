import argparse
import json
import pathlib
import typing
import sys
import re

from bdl.generators.json.ast.ast import Ast
from bzd.template.template import Template
from bzd_oci.run import ociPush

from targets.docker.deployment.compose import DockerCompose
from targets.docker.deployment.traefik import DockerTraefik

from targets.docker.transport.ssh import TransportSSH
from bzd.utils.ssh import SSH


class CommonParameters:

	def __init__(self, port: int) -> None:
		self.port = port

	def imageResolve(self, image: str) -> str:
		"""Convert an image name into its resolve url pointing to the registry."""
		return f"localhost:{self.port}/{image}"

	@staticmethod
	def hostToName(host: str) -> str:
		"""Convert a hostname into a sanitized name."""
		return re.sub(r"[^a-zA-Z0-9]+", '_', host)

	@staticmethod
	def needsWWWRedirect(hostname: str) -> bool:
		"""If the docker service needs to redirect the www.<hostname> to <hostname>.
		
		This is needed in case of a hostname with 2 parts <1>.<2> only.
		"""
		return len(hostname.split(".")) == 2


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
	dockerCompose = traefikDockerCompose | composeDockerCompose
	images = traefikImages | composeImages

	registry = Template.fromPath(pathlib.Path(__file__).parent / "registry.yml.btl").render(common)
	applicationsDirectory = args.directory / "apps"

	if args.transport.startswith("ssh://"):
		connection = args.transport[6:]
		print(f"Transport SSH at {connection}", flush=True)
		transport = TransportSSH(connection)

	else:
		print("Error", flush=True)
		raise Exception(f"Unknown transport type for '{args.transport}'.")

	with transport.session() as handle:

		handle.command(["mkdir", "-p", str(applicationsDirectory)])

		print(f"Copying '{args.directory}/docker-compose.yml'.", flush=True)
		handle.uploadContent(registry, f"{args.directory}/docker-compose.yml")

		print("Starting docker registry.", flush=True)
		handle.command(["docker", "compose", "--file", f"{args.directory}/docker-compose.yml", "up", "-d", "registry"])

		# Push the new images
		print(f"Forwarding port {args.registry_port}...", flush=True)
		with handle.forwardPort(args.registry_port, waitS=5, waitHTTP=f"http://localhost:{args.registry_port}/v2/"):
			for image in images:
				print(f"Pushing {image}...", flush=True)
				ociPush(image, f"localhost:{args.registry_port}/{image}", stdout=True, stderr=True, timeoutS=600)

		# Find all applications.
		applications = set(dockerCompose.keys())
		result = handle.command(["find", str(applicationsDirectory), "-maxdepth", "1", "-type", "d"])
		currentApplications = set(filter(bool, [d.strip().split("/")[-1] for d in result.getOutput().split("\n")][1:]))
		print(f"Applications currently deployed: {', '.join(currentApplications)}", flush=True)
		print(f"Applications: {', '.join(applications)}", flush=True)

		# Remove applications that are currently present but not needed anymore.
		removeApplications = currentApplications - applications
		for application in removeApplications:
			directory = f"{applicationsDirectory}/{application}"
			print(f"Stopping '{directory}'...", flush=True)
			handle.command(["docker", "compose", "--file", f"{directory}/docker-compose.yml", "down"],
			               ignoreFailure=True)
			handle.command(["rm", "-rfd", f"{directory}"])

		# Start applications
		for application, content in dockerCompose.items():
			directory = f"{applicationsDirectory}/{application}"
			print(f"Copying '{directory}/docker-compose.yml'.", flush=True)
			handle.command(["mkdir", "-p", directory])
			handle.uploadContent(content, f"{directory}/docker-compose.yml")
			print(f"Pulling new images.", flush=True)
			handle.command(["docker", "compose", "--file", f"{directory}/docker-compose.yml", "pull"])
			print(f"Restarting containers.", flush=True)
			handle.command(["docker", "compose", "--file", f"{directory}/docker-compose.yml", "up", "-d"])

	sys.exit(0)
