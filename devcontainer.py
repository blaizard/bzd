#!/usr/bin/env python3
"""Development container."""

import argparse
import getpass
import grp
import hashlib
import os
import pathlib
import re
import tempfile
import subprocess
import typing
from functools import cached_property

imageBase = "ubuntu:24.04"


class Feature:
	"""Base class for a feature."""

	def __init__(self, args: argparse.Namespace) -> None:
		self.args = args
		self.isAvailable: bool = False
		self.context: typing.Optional["DevelopmentContainer"] = None

	@staticmethod
	def cli(parser: argparse.ArgumentParser) -> None:
		pass

	@property
	def namePrefix(self) -> typing.List[str]:
		return []

	@property
	def groups(self) -> typing.List[str]:
		return []

	@property
	def volumes(self) -> typing.List[str]:
		return []

	@property
	def dockerFile(self) -> typing.List[str]:
		return []

	@property
	def dockerCompose(self) -> typing.Dict[str, typing.List[str]]:
		return {}

	def initialize(self, context: "DevelopmentContainer") -> None:
		self.context = context


class FeatureVolume(Feature):
	"""Feature: Additional volume inside the container."""

	def __init__(self, args: argparse.Namespace) -> None:
		super().__init__(args)
		self.isAvailable = self.args.volumes is not None

	@staticmethod
	def cli(parser: argparse.ArgumentParser) -> None:
		parser.add_argument("--volume",
		                    dest="volumes",
		                    type=str,
		                    action="append",
		                    help="Add one or more docker volume The volumes will be mounted under /bzd/volumes/<name>.")

	@property
	def volumes(self) -> typing.List[str]:
		return [f"{volume}:/bzd/volumes/{volume}" for volume in self.args.volumes]

	@property
	def dockerFile(self) -> typing.List[str]:
		assert self.context is not None
		if not self.context.userNamespaceRemapping:
			return [
			    f"RUN echo 'sudo chown {self.context.uid}:{self.context.gid} /bzd/volumes/{volume} && sudo chmod 755 /bzd/volumes/{volume}' >> /bzd/startup.sh"
			    for volume in self.args.volumes
			]
		return []

	@property
	def dockerCompose(self) -> typing.Dict[str, typing.List[str]]:
		return {"volumes": [f"{volume}:\n  name: \"{volume}\"" for volume in self.args.volumes]}

	@property
	def namePrefix(self) -> typing.List[str]:
		return [f"volume-{volume}" for volume in self.args.volumes]


class FeatureDevcontainerCLI(Feature):
	"""Feature: Add the devcontainer CLI to the container."""

	def __init__(self, args: argparse.Namespace) -> None:
		super().__init__(args)
		self.isAvailable = args.devcontainer_cli

	@staticmethod
	def cli(parser: argparse.ArgumentParser) -> None:
		parser.add_argument("--devcontainer_cli",
		                    action="store_true",
		                    help="Include the devcontainer CLI to the container.")

	@property
	def dockerFile(self) -> typing.List[str]:
		return ["RUN sudo apt install -y nodejs npm", "RUN sudo npm install -g @devcontainers/cli"]

	@property
	def namePrefix(self) -> typing.List[str]:
		return ["devctonainer-cli"]


class FeatureDocker(Feature):
	"""Feature: Docker outside the container."""

	def __init__(self, args: argparse.Namespace) -> None:
		super().__init__(args)
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
		    "docker", "run", "--rm", "-v", f"{self.mayberDockerSocket}:/var/run/docker.sock", imageBase, "stat", "-c",
		    "'%g'", "/var/run/docker.sock"
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
		return ["RUN command -v docker >/dev/null 2>&1 || curl -fsSL https://get.docker.com | sh"]


class FeaturePlatform(Feature):
	"""Feature: Platform emulation inside the container."""

	def __init__(self, args: argparse.Namespace) -> None:
		super().__init__(args)
		self.platform = args.platform
		self.isAvailable = self.platform is not None

	@staticmethod
	def cli(parser: argparse.ArgumentParser) -> None:
		parser.add_argument("--platform",
		                    choices=(
		                        "amd64",
		                        "arm64",
		                    ),
		                    help="The architecture on which the docker should run.")

	@property
	def namePrefix(self) -> typing.List[str]:
		return [self.platform]

	def initialize(self, context: "DevelopmentContainer") -> None:
		super().initialize(context=context)
		# See: https://stackoverflow.com/questions/72444103/what-does-running-the-multiarch-qemu-user-static-does-before-building-a-containe
		#if self.isRootless:
		subprocess.run(["sudo", "systemctl", "start", "docker"])
		subprocess.run(
		    ["sudo", "docker", "run", "--rm", "--privileged", "multiarch/qemu-user-static", "--reset", "-p", "yes"])
		#else:
		#subprocess.run(["docker", "run", "--rm", "--privileged", "multiarch/qemu-user-static", "--reset", "-p", "yes"])

	@property
	def dockerCompose(self) -> typing.Dict[str, typing.List[str]]:
		if self.platform == "amd64":
			return {"service": ["platform: linux/amd64"]}
		elif self.platform == "arm64":
			return {"service": ["platform: linux/arm64"]}
		return {}


class DevelopmentContainer:

	def __init__(self, features: typing.Sequence[Feature], temporaryPath: pathlib.Path) -> None:
		# Do not resolve symlinks so that we can use symlinks to create multiple devcontainers
		# from the same source file, but we still want a normalized absolute path.
		self.workspace = pathlib.Path(os.path.normpath(__file__)).absolute().parent
		self.cwd = pathlib.Path.cwd()
		self.home = pathlib.Path.home()
		self.temporaryPath = temporaryPath
		self.uid = os.getuid()
		self.gid = os.getgid()
		self.userNamespaceRemapping = DevelopmentContainer.isUserNamespaceRemapping()
		self.user = "root" if self.userNamespaceRemapping else getpass.getuser()
		self.features = [feature for feature in features if feature.isAvailable]

	def initialize(self) -> None:

		# Initialize features.
		for feature in self.features:
			feature.initialize(context=self)

		# Write the docker files.
		self.dockerFilePath.parent.mkdir(parents=True, exist_ok=True)
		self.dockerFilePath.write_text(self.dockerFile)
		self.dockerComposePath.parent.mkdir(parents=True, exist_ok=True)
		self.dockerComposePath.write_text(self.dockerCompose)

	@staticmethod
	def isUserNamespaceRemapping() -> bool:
		"""Check if user namespace remapping is enabled.

		A rootless container will have its file permissions mapped to the root user and translated to the host via docker.
		"""

		output = subprocess.check_output(["docker", "info", "-f", "{{println .SecurityOptions}}"])
		return b"rootless" in output

	@cached_property
	def namePrefix(self) -> str:
		toHash = [self.workspace.as_posix(), self.user]
		toHash += [prefix for feature in self.features for prefix in feature.namePrefix]
		pathHash = hashlib.md5("-".join(toHash).encode()).hexdigest()[:6]
		directory = self.workspace.name[-10:]
		return "-".join([pathHash, re.sub(r'[^a-zA-Z0-9]+', "-", directory).strip("-")])

	@cached_property
	def imageName(self) -> str:
		return f"devcontainer-{self.namePrefix}"

	@cached_property
	def dockerFilePath(self) -> pathlib.Path:
		return self.temporaryPath / f"bzd-devcontainer-{self.namePrefix}" / "DockerFile"

	@cached_property
	def dockerComposePath(self) -> pathlib.Path:
		return self.temporaryPath / f"bzd-devcontainer-{self.namePrefix}" / "docker-compose.yaml"

	@cached_property
	def dockerFile(self) -> str:

		instructions = []

		if self.userNamespaceRemapping:
			instructions += [
			    f"RUN mkdir -p {self.home}",
			]
		else:

			user_groups = {gid: grp.getgrgid(gid).gr_name for gid in os.getgrouplist(self.user, self.gid)}
			groups = [str(gid) for gid in user_groups.keys()
			          ] + ["sudo"] + [group for feature in self.features for group in feature.groups]
			# Create all user groups if not existing.
			instructions += [
			    f"RUN getent group {gid} || groupadd -g {gid} '{name}'" for gid, name in user_groups.items()
			]
			instructions += [
			    # Delete any existing user with the same uid if any.
			    f"RUN id -u {self.uid} &>/dev/null && userdel -r $(id -un {self.uid}) || true",
			    f"RUN useradd --create-home -d {self.home} --no-log-init --uid {self.uid} --gid {self.gid} --groups {','.join(groups)} {self.user}",
			    f"""RUN echo "\"{self.user}\" ALL=(ALL) NOPASSWD:ALL\" >> /etc/sudoers""",
			    f"RUN chown \"{self.user}\" /bzd/startup.sh",
			    f"USER {self.user}",
			]
		instructions += [
		    f"ENV USER=\"{self.user}\"",
		    f"ENV HOME=\"{self.home}\"",
		]

		instructions += [instruction for feature in self.features for instruction in feature.dockerFile]
		instructionsStr = "\n".join(instructions)

		return f"""
FROM {imageBase}
RUN apt update -y
RUN apt upgrade -y
RUN apt install -y \
	ca-certificates \
	python3 \
	git \
	wget \
	curl \
	patchelf \
	sudo \
	build-essential

RUN wget -O /usr/local/bin/bazel https://raw.githubusercontent.com/blaizard/bzd/refs/heads/master/tools/bazel && chmod +x /usr/local/bin/bazel

# Entry point script that will run once upon container start.
RUN mkdir /bzd && echo "#!/usr/bin/env bash" > /bzd/startup.sh && chmod +x /bzd/startup.sh

{instructionsStr}

RUN wget -O {self.home}/.bashrc https://raw.githubusercontent.com/blaizard/bzd/refs/heads/master/tools/shell/sh/bashrc.sh
RUN echo "PS1=\\"(devcontainer) \\$PS1\\"" >> {self.home}/.bashrc
"""

	@cached_property
	def dockerCompose(self) -> str:

		# Add some directories as volumes.
		volumes = [
		    f"{self.workspace}:{self.workspace}",
		    f"{self.home}/.cache:{self.home}/.cache",
		]

		# Add some files as volumes only if they exist (otherwise docker-compose will create an empty directory).
		volumes += [f"{file}:{file}" for file in [self.home / ".bash_history", self.home / ".netrc"] if file.is_file()]

		# Add some volumes from features.
		for feature in self.features:
			volumes += feature.volumes
		volumesStr = "\n".join([f"      - {volume}" for volume in volumes])

		# Additional options.
		extraService = [f"command: sh -c '/bzd/startup.sh && tail -f /dev/null'"]
		extraService += [line for feature in self.features for line in feature.dockerCompose.get("service", [])]
		extraServiceStr = "\n".join([f"    {line}" for line in extraService])

		extraStr = ""
		extraVolumes = "\n".join(
		    [line for feature in self.features for line in feature.dockerCompose.get("volumes", [])])
		if extraVolumes:
			extraStr += "volumes:\n" + "\n".join([f"  {line}" for line in extraVolumes.splitlines()])

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
    extra_hosts:
      - "{self.imageName}:127.0.0.1"
    network_mode: host
    init: true
    tty: true
    stdin_open: true
{extraServiceStr}
    volumes:
{volumesStr}

{extraStr}
"""

	def build(self, force: bool) -> None:
		subprocess.run([
		    "docker",
		    "compose",
		    "--file",
		    str(self.dockerComposePath),
		    "build",
		] + (["--no-cache"] if force else []),
		               check=True)

	def run(self, args: typing.List[str], env: typing.List[str]) -> None:
		subprocess.run([
		    "docker",
		    "compose",
		    "--file",
		    str(self.dockerComposePath),
		    "up",
		    "--detach",
		], check=True)

		extra = [arg for e in env for arg in ("--env", e)]

		workdir = self.cwd if self.cwd.is_relative_to(self.workspace) else self.workspace
		subprocess.run(
		    ["docker", "exec", *extra, "-it", "--workdir",
		     str(workdir), self.imageName, *(args or ["/bin/bash"])])


if __name__ == "__main__":

	Features = [FeatureDocker, FeaturePlatform, FeatureVolume, FeatureDevcontainerCLI]

	parser = argparse.ArgumentParser(description="Development container.")
	parser.add_argument("--build", action="store_true", help="Re-build the container if set.")
	parser.add_argument("--build-force", action="store_true", help="Force re-build the container if set.")
	parser.add_argument("--temp",
	                    type=pathlib.Path,
	                    default=pathlib.Path(tempfile.gettempdir()),
	                    help="A temporary directory where to store the docker related configuration files.")
	parser.add_argument("--env", action="append", default=[], help="Environment variables to pass to the container.")
	parser.add_argument("rest", nargs=argparse.REMAINDER, help="Additional arguments to pass to the container.")

	for FeatureClass in Features:
		FeatureClass.cli(parser)

	args = parser.parse_args()

	features = [FeatureClass(args) for FeatureClass in Features]
	devContainer = DevelopmentContainer(features=features, temporaryPath=args.temp)
	devContainer.initialize()
	if args.build_force:
		devContainer.build(force=True)
	if args.build:
		devContainer.build(force=False)
	devContainer.run(args=args.rest, env=args.env)
