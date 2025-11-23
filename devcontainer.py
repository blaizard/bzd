#!/usr/bin/env python3
"""Development container."""

import argparse
import getpass
import os
import pathlib
import tempfile
import subprocess
import typing


class Feature:
	"""Base class for a feature."""

	isAvailable: bool = False

	def __init__(self, container: "DevelopmentContainer") -> None:
		self.container = container

	@property
	def groups(self) -> typing.List[str]:
		return []

	@property
	def volumes(self) -> typing.List[str]:
		return []

	@property
	def dockerFile(self) -> typing.List[str]:
		return []


class FeatureDocker(Feature):
	"""Feature: Docker inside the container."""

	def __init__(self, **kwargs: typing.Any) -> None:
		super().__init__(**kwargs)
		self.mayberDockerSocket = FeatureDocker.getDockerSocket()
		self.isAvailable = self.mayberDockerSocket is not None

	@staticmethod
	def getDockerSocket() -> typing.Optional[pathlib.Path]:
		"""Get the docker socket path if available."""

		for dockerSocket in [
		    pathlib.Path(os.getenv("XDG_RUNTIME_DIR", "") + "/docker.sock"),
		    pathlib.Path("/var/run/docker.sock")
		]:
			if dockerSocket.exists():
				return dockerSocket.resolve()
		return None

	def getDockerGid(self) -> str:
		"""Get the docker group.
		
		This method is also compatible with macos, where the docker socket is created only with docker.
		"""

		result = subprocess.run([
		    "docker", "run", "--rm", "-v", f"{self.mayberDockerSocket}:/var/run/docker.sock", self.container.imageBase,
		    "stat", "-c", "'%g'", "/var/run/docker.sock"
		],
		                        capture_output=True)
		assert result.returncode == 0, "Failed to get docker group id from container."
		return str(result.stdout.decode().strip().strip("'\""))

	@property
	def groups(self) -> typing.List[str]:
		return [self.getDockerGid()]

	@property
	def volumes(self) -> typing.List[str]:
		return [f"{self.mayberDockerSocket}:/var/run/docker.sock"]

	@property
	def dockerFile(self) -> typing.List[str]:
		return ["RUN curl -fsSL https://get.docker.com | sh"]


class DevelopmentContainer:

	imageBase = "ubuntu:24.04"

	def __init__(self, platform: typing.Optional[str], temporaryPath: pathlib.Path) -> None:
		self.workspace = pathlib.Path(__file__).resolve().parent
		self.cwd = pathlib.Path.cwd()
		self.home = pathlib.Path.home()
		self.platform = platform
		self.temporaryPath = temporaryPath
		self.uid = os.getuid()
		self.gid = os.getgid()
		self.userNamespaceRemapping = DevelopmentContainer.isUserNamespaceRemapping()
		self.user = "root" if self.userNamespaceRemapping else getpass.getuser()
		self.features = [feature for feature in [FeatureDocker(container=self)] if feature.isAvailable]

		if self.platform:
			self.startEmulation()

		# Write the docker files.
		self.dockerFilePath.write_text(self.dockerFile)
		self.dockerComposePath.write_text(self.dockerCompose)

	@staticmethod
	def isUserNamespaceRemapping() -> bool:
		"""Check if user namespace remapping is enabled.

		A rootless container will have its file permissions mapped to the root user and translated to the host via docker.
		"""

		output = subprocess.check_output(["docker", "info", "-f", "{{println .SecurityOptions}}"])
		return b"rootless" in output

	@property
	def namePrefix(self) -> str:
		prefix = self.user
		if self.platform:
			prefix += f"-{self.platform}"
		return prefix

	@property
	def imageName(self) -> str:
		return f"devcontainer-{self.namePrefix}"

	@property
	def dockerFilePath(self) -> pathlib.Path:
		return self.temporaryPath / f"bzd-DockerFile-{self.namePrefix}"

	@property
	def dockerComposePath(self) -> pathlib.Path:
		return self.temporaryPath / f"bzd-docker-compose-{self.namePrefix}.yaml"

	@property
	def dockerFile(self) -> str:

		instructions = []

		if self.userNamespaceRemapping:
			instructions += [
			    f"RUN mkdir -p {self.home}",
			    f"ENV HOME={self.home}",
			]
		else:

			groups = [str(self.gid), "sudo"] + [group for feature in self.features for group in feature.groups]
			instructions += [
			    f"RUN groupadd -o --gid {self.gid} {self.user}",
			    # Delete any existing user with the same uid if any.
			    f"RUN id -u {self.uid} &>/dev/null && userdel -r $(id -un {self.uid}) || true",
			    f"RUN useradd --create-home -d {self.home} --no-log-init --uid {self.uid} --gid {self.gid} --groups {','.join(groups)} {self.user}",
			    f"""RUN echo "\"{self.user}\" ALL=(ALL) NOPASSWD:ALL\" >> /etc/sudoers""",
			    f"USER {self.user}"
			]

		instructions += [instruction for feature in self.features for instruction in feature.dockerFile]

		return f"""
FROM {self.imageBase}
RUN apt update -y
RUN apt upgrade -y
RUN apt install -y \
	ca-certificates \
	python3 \
	git \
	wget \
	curl \
	patchelf \
	sudo

RUN wget -O /usr/local/bin/bazel https://raw.githubusercontent.com/blaizard/bzd/refs/heads/master/tools/bazel && chmod +x /usr/local/bin/bazel

{"\n".join(instructions)}

RUN wget -O {self.home}/.bashrc https://raw.githubusercontent.com/blaizard/bzd/refs/heads/master/tools/shell/sh/bashrc.sh
RUN echo "PS1=\\"(devcontainer) \\$PS1\\"" >> {self.home}/.bashrc
"""

	@property
	def dockerCompose(self) -> str:

		# Volumes
		volumes = [
		    f"{self.workspace}:{self.workspace}", f"{self.home}/.bash_history:{self.home}/.bash_history",
		    f"{self.home}/.cache:{self.home}/.cache", f"user-volume:/bzd-user-volume"
		]
		for feature in self.features:
			volumes += feature.volumes

		# Additional options.
		extra = []
		if self.platform == "amd64":
			extra += ["platform: linux/amd64"]
		elif self.platform == "arm64":
			extra += ["platform: linux/arm64"]
		if self.userNamespaceRemapping == False:
			extra += [
			    f"command: sh -c \"sudo chown {self.uid}:{self.gid} /bzd-user-volume && sudo chmod 755 /bzd-user-volume && bash\""
			]

		# Docker compose.
		return f"""
services:
  bzd:
    build:
      context: {self.dockerFilePath.parent}
      dockerfile: {self.dockerFilePath.name}
    image: bzd/{self.imageName}
    container_name: {self.imageName}
    hostname: {self.imageName}
    tty: true
    stdin_open: true
{"\n".join([f"    {line}" for line in extra])}
    volumes:
{"\n".join([f"      - {volume}" for volume in volumes])}

volumes:
  user-volume:
    name: "volume-{self.imageName}"
"""

	def startEmulation(self) -> None:
		# See: https://stackoverflow.com/questions/72444103/what-does-running-the-multiarch-qemu-user-static-does-before-building-a-containe
		if self.isRootless:
			subprocess.run(["sudo", "systemctl", "start", "docker"])
			subprocess.run(
			    ["sudo", "docker", "run", "--rm", "--privileged", "multiarch/qemu-user-static", "--reset", "-p", "yes"])
		else:
			subprocess.run(
			    ["docker", "run", "--rm", "--privileged", "multiarch/qemu-user-static", "--reset", "-p", "yes"])

	def build(self) -> None:
		subprocess.run([
		    "docker",
		    "compose",
		    "--file",
		    str(self.dockerComposePath),
		    "build",
		    "--no-cache",
		],
		               check=True)

	def run(self) -> None:
		subprocess.run([
		    "docker",
		    "compose",
		    "--file",
		    str(self.dockerComposePath),
		    "up",
		    "--detach",
		], check=True)

		workdir = self.cwd if self.cwd.is_relative_to(self.workspace) else self.workspace
		subprocess.run(["docker", "exec", "-it", "--workdir", str(workdir), self.imageName, "/bin/bash"])


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Development container.")
	parser.add_argument("-b", "--build", action="store_true", help="Re-build the container if set.")
	parser.add_argument("--temp",
	                    type=pathlib.Path,
	                    default=pathlib.Path(tempfile.gettempdir()),
	                    help="A temporary directory where to store the docker related configuration files.")
	parser.add_argument("-p",
	                    "--platform",
	                    choices=(
	                        "amd64",
	                        "arm64",
	                    ),
	                    help="The architecture on which the docker should run.")
	args = parser.parse_args()

	devContainer = DevelopmentContainer(args.platform, temporaryPath=args.temp)
	if args.build:
		devContainer.build()
	devContainer.run()
