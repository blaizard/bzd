import argparse
import json
import pathlib
import typing
import sys
import re
import time

from bdl.generators.json.ast.ast import Ast
from bzd.template.template import Template
from bzd_oci.run import ociPush

from targets.docker.deployment.compose import DeploymentDockerCompose
from targets.docker.deployment.traefik import DeploymentDockerTraefik

from targets.docker.transport.ssh import TransportSSH
from targets.docker.registry import DockerRegistry
from targets.docker.docker_compose import DockerCompose
from targets.docker.config import STABLE_VERSION


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


def getAllImagesFromDockerComposeFiles(
        handle: typing.Any, dockerComposeFiles: typing.List[pathlib.Path]) -> typing.Dict[str, typing.Set[str]]:
	"""Gather all images referred to in the docker compose files."""

	images: typing.Dict[str, typing.Set[str]] = {}
	for path in dockerComposeFiles:
		result = handle.command(["docker", "compose", "--file", str(path), "config", "--format", "json"])
		output = json.loads(result.getOutput())
		imagesCurrentFile = DockerCompose(output).getImages()

		# Merge.
		for image, tags in imagesCurrentFile.items():
			images.setdefault(image, set())
			images[image].update(tags)

	return images


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
	traefikDockerCompose, traefikImages = DeploymentDockerTraefik(ast=ast,
	                                                              common=common).makeDockerComposeForAllInstances()
	composeDockerCompose, composeImages = DeploymentDockerCompose(ast=ast,
	                                                              common=common).makeDockerComposeForAllInstances()
	dockerCompose = traefikDockerCompose | composeDockerCompose
	images = traefikImages | composeImages

	registry = Template.fromPath(pathlib.Path(__file__).parent / "registry.yml.btl").render(common)
	applicationsDirectory = args.directory / "apps"

	if args.transport.startswith("ssh://"):
		connection = args.transport[6:]
		print(f"Transport SSH at {connection}", flush=True)
		transport = TransportSSH(connection)

	else:
		raise Exception(f"Unknown transport type for '{args.transport}'.")

	with transport.session() as handle:

		handle.command(["mkdir", "-p", str(applicationsDirectory)])

		print(f"Copying '{args.directory}/docker-compose.yml'.", flush=True)
		handle.uploadContent(registry, f"{args.directory}/docker-compose.yml")

		print("Starting docker registry.", flush=True)
		handle.command(["docker", "compose", "--file", f"{args.directory}/docker-compose.yml", "up", "-d", "registry"])

		# Push the new images
		print(f"Forwarding port {args.registry_port}...", flush=True)
		with handle.forwardPort(args.registry_port, waitHTTP=f"http://localhost:{args.registry_port}/v2/"):

			for image in images:
				print(f"Pushing {image}...", flush=True)
				ociPush(pathlib.Path(image),
				        f"localhost:{args.registry_port}/{image}",
				        stdout=True,
				        stderr=True,
				        timeoutS=30 * 60)

		# Find all applications.
		applications = set(dockerCompose.keys())
		result = handle.command(["find", str(applicationsDirectory), "-maxdepth", "1", "-type", "d"])
		currentApplications = set(filter(bool, [d.strip().split("/")[-1] for d in result.getOutput().split("\n")][1:]))
		print(f"Applications currently deployed: {', '.join(currentApplications)}", flush=True)
		print(f"Applications: {', '.join(applications)}", flush=True)

		# Remove applications that are currently present but not needed anymore.
		removeApplications = currentApplications - applications
		for application in removeApplications:
			directory = applicationsDirectory / application
			print(f"Stopping '{directory}'...", flush=True)
			handle.command(["docker", "compose", "--file",
			                str(directory / "docker-compose.yml"), "down"],
			               ignoreFailure=True)
			handle.command(["rm", "-rfd", str(directory)])

		# Start applications
		allDockerComposeFiles: typing.List[pathlib.Path] = []
		for application, content in dockerCompose.items():

			directory = applicationsDirectory / application
			handle.command(["mkdir", "-p", str(directory)])

			dockerComposeDirectory = directory / "docker-compose"
			dockerComposeFile = directory / "docker-compose.yml"
			print(f"Copying and using '{dockerComposeDirectory}/{STABLE_VERSION}.yml'.", flush=True)
			handle.command(["mkdir", "-p", str(dockerComposeDirectory)])
			handle.uploadContent(content, f"{dockerComposeDirectory / STABLE_VERSION}.yml")
			# Note, it is important to copy instead of symlink, otherwise docker compose up might lead to orphan
			# containers if some are deleted as docker will consider it as a new project.
			handle.command(["cp", "-f", f"{dockerComposeDirectory / STABLE_VERSION}.yml", str(dockerComposeFile)])

			print(f"Pulling new images.", flush=True)
			# Note --ignore-pull-failures: if there is no need for updating a certain image, it can be that it is not in the registry.
			# not having --ignore-pull-failures will result in a failure.
			handle.command(["docker", "compose", "--file", str(dockerComposeFile), "pull", "--ignore-pull-failures"])
			print(f"Restarting containers.", flush=True)
			handle.command(["docker", "compose", "--file", str(dockerComposeFile), "up", "-d"])

			# Gather all the docker compose files from existing and previous versions and delete the too old ones.
			dockerComposeFiles = sorted([
			    s for s in handle.command(["ls", "-1", str(dockerComposeDirectory)]).getOutput().split("\n")
			    if s.endswith(".yml")
			],
			                            reverse=True)
			print(f"Identified {len(dockerComposeFiles)} version(s): {', '.join(dockerComposeFiles)}", flush=True)
			keepDockerComposeFiles = dockerComposeFiles[:2]
			removeDockerComposeFiles = dockerComposeFiles[2:]
			for name in removeDockerComposeFiles:
				print(f"- Removing {dockerComposeDirectory / name}", flush=True)
				handle.command(["rm", str(dockerComposeDirectory / name)])

			# Keep track of all current docker compose files.
			allDockerComposeFiles += [dockerComposeDirectory / name for name in keepDockerComposeFiles]

		# Garbage collect some of the images if needed.
		print(f"Identifying unused images...", flush=True)
		with handle.forwardPort(args.registry_port, waitHTTP=f"http://localhost:{args.registry_port}/v2/"):

			usedImages = getAllImagesFromDockerComposeFiles(handle, allDockerComposeFiles)
			dockerRegistry = DockerRegistry(f"http://localhost:{args.registry_port}")
			allImages = dockerRegistry.getImages()

			# Only keep unused image tags.
			for repository, tags in usedImages.items():
				if repository in allImages:
					for tag in tags:
						if tag in allImages[repository]:
							del allImages[repository][tag]

			# Remove unused image tags.
			for repository, tagsDigest in allImages.items():
				for tag, digest in tagsDigest.items():
					print(f"- Removing image {repository}:{tag} ({digest}).", flush=True)
					dockerRegistry.delete(repository, digest)

		# Cleanup docker register and docker
		print(f"Cleaning up dangling images...", flush=True)
		handle.command([
		    "docker", "exec", "registry", "/bin/registry", "garbage-collect", "--delete-untagged",
		    "/etc/docker/registry/config.yml"
		])
		handle.command(["docker", "image", "prune", "--force"])

	sys.exit(0)
