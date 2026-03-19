#!/usr/bin/env python3
"""Development container."""

import argparse
import getpass
import grp
import hashlib
import json
import os
import pathlib
import re
import tempfile
import typing
import subprocess
import sys
from functools import cached_property

imageBase = "ubuntu:24.04"
imagePrefix = "devcontainer-"


class Feature:
	"""Base class for a feature."""

	def __init__(self, args: argparse.Namespace) -> None:
		self.args = args
		self.isAvailable: bool = True
		self.context: typing.Optional["DevelopmentContainer"] = None

	@staticmethod
	def cli() -> typing.Dict[str, typing.Dict[str, typing.Any]]:
		return {}

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
	def cli() -> typing.Dict[str, typing.Dict[str, typing.Any]]:
		return {
			"volume": {
				"dest": "volumes",
				"type": str,
				"action": "append",
				"help": "Add one or more docker volume The volumes will be mounted under /bzd/volumes/<name>.",
			}
		}

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
		return {"volumes": [f'{volume}:\n  name: "{volume}"' for volume in self.args.volumes]}


class FeatureSession(Feature):
	"""Feature: Add support for sessions."""

	@property
	def dockerFile(self) -> typing.List[str]:
		assert self.context is not None
		return [
			"RUN sudo apt install -y tmux",
			f"""RUN <<EOF cat > {self.context.home}/.tmux.conf
set -g default-command "/bin/bash"
EOF""",
			"""RUN sudo tee /usr/local/bin/session <<EOF
#!/bin/bash
exec tmux new-session -A -s "${1:-main}"
EOF""",
			"RUN sudo chmod +x /usr/local/bin/session",
		]


class FeatureIsolation(Feature):
	"""Feature: Add sandbox limitations to the container."""

	@staticmethod
	def cli() -> typing.Dict[str, typing.Dict[str, typing.Any]]:
		return {
			"isolate": {
				"action": "store_true",
				"help": "Isolate the container from the host.",
			}
		}

	@property
	def volumes(self) -> typing.List[str]:
		if self.args.isolate:
			return []

		assert self.context
		volumes = [
			# Share the cache.
			f"{self.context.home}/.cache:{self.context.home}/.cache",
		]
		# Add some files as volumes only if they exist (otherwise docker-compose will create an empty directory).
		volumes += [
			f"{file}:{file}" for file in [self.context.home / ".bash_history", self.context.home / ".netrc"] if file.is_file()
		]
		return volumes

	@property
	def dockerCompose(self) -> typing.Dict[str, typing.List[str]]:
		if self.args.isolate:
			assert self.context is not None
			return {
				"service": [
					"cap_drop:",
					"  - ALL",
					"security_opt:",
					"  - no-new-privileges:true",
				]
			}
		return {"service": ["network_mode: host"]}


class FeatureDevcontainerCLI(Feature):
	"""Feature: Add the devcontainer CLI to the container."""

	def __init__(self, args: argparse.Namespace) -> None:
		super().__init__(args)
		self.isAvailable = args.devcontainer_cli

	@staticmethod
	def cli() -> typing.Dict[str, typing.Dict[str, typing.Any]]:
		return {
			"devcontainer_cli": {
				"action": "store_true",
				"help": "Include the devcontainer CLI to the container.",
			}
		}

	@property
	def dockerFile(self) -> typing.List[str]:
		return [
			"RUN sudo apt install -y nodejs npm",
			"RUN sudo npm install -g @devcontainers/cli",
		]


class FeatureOpenCode(Feature):
	"""Feature: Add opencode CLI to the container."""

	def __init__(self, args: argparse.Namespace) -> None:
		super().__init__(args)
		self.isAvailable = args.opencode

	@staticmethod
	def cli() -> typing.Dict[str, typing.Dict[str, typing.Any]]:
		return {
			"opencode": {
				"action": "store_true",
				"help": "Include the opencode to the container.",
			}
		}

	@property
	def dockerFile(self) -> typing.List[str]:
		return [
			"RUN sudo apt install -y nodejs npm",
			"RUN sudo npm install -g opencode-ai@latest",
		]

	@property
	def volumes(self) -> typing.List[str]:
		assert self.context
		configPaths = [
			self.context.home / ".config/opencode",
			self.context.home / ".local/share/opencode",
			self.context.home / ".local/state/opencode",
		]
		return [f"{path}:{path}" for path in configPaths if path.exists()]


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
			pathlib.Path("/var/run/docker.sock"),
		]:
			if dockerSocket.exists():
				return dockerSocket.resolve()
		return None

	def getDockerGid(self) -> str:
		"""Get the docker group.

		This method is also compatible with macos, where the docker socket is created only with docker.
		"""

		result = subprocess.run(
			[
				"docker",
				"run",
				"--rm",
				"-v",
				f"{self.mayberDockerSocket}:/var/run/docker.sock",
				imageBase,
				"stat",
				"-c",
				"'%g'",
				"/var/run/docker.sock",
			],
			capture_output=True,
		)
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
	def cli() -> typing.Dict[str, typing.Dict[str, typing.Any]]:
		return {
			"platform": {
				"choices": (
					"amd64",
					"arm64",
				),
				"help": "The architecture on which the docker should run.",
			}
		}

	def initialize(self, context: "DevelopmentContainer") -> None:
		super().initialize(context=context)
		# See: https://stackoverflow.com/questions/72444103/what-does-running-the-multiarch-qemu-user-static-does-before-building-a-containe
		# if self.isRootless:
		subprocess.run(["sudo", "systemctl", "start", "docker"])
		subprocess.run(
			[
				"sudo",
				"docker",
				"run",
				"--rm",
				"--privileged",
				"multiarch/qemu-user-static",
				"--reset",
				"-p",
				"yes",
			]
		)
		# else:
		# subprocess.run(["docker", "run", "--rm", "--privileged", "multiarch/qemu-user-static", "--reset", "-p", "yes"])

	@property
	def dockerCompose(self) -> typing.Dict[str, typing.List[str]]:
		if self.platform == "amd64":
			return {"service": ["platform: linux/amd64"]}
		elif self.platform == "arm64":
			return {"service": ["platform: linux/arm64"]}
		return {}


class DevelopmentContainer:
	"""Development container."""

	def __init__(self, args: argparse.Namespace, features: typing.Sequence[Feature], temporaryPath: pathlib.Path) -> None:

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
		self.args = args

	@cached_property
	def toHash(self) -> typing.List[str]:
		toHash = [f"workspace={self.workspace.as_posix()}", f"user={getpass.getuser()}"]
		toHash += sorted([feature.__class__.__name__ for feature in self.features])
		argNames = sorted([kwargs.get("dest", name) for feature in self.features for name, kwargs in feature.cli().items()])
		toHash += [f"{name}={getattr(self.args, name)}" for name in argNames]
		return toHash

	@cached_property
	def namePrefix(self) -> str:
		# Generate the hash.
		pathHash = hashlib.md5("-".join(self.toHash).encode()).hexdigest()[:6]

		names = [self.args.prefix] if self.args.prefix else []
		directory = self.workspace.name[-10:]
		names += [pathHash, re.sub(r"[^a-zA-Z0-9]+", "-", directory).strip("-")]
		return "-".join(names)

	def initialize(self, dry: bool) -> None:

		# Initialize features.
		for feature in self.features:
			feature.initialize(context=self)

		if dry:
			print("==== Hash ====")
			print("\n".join(self.toHash))
			print("==== DockerFile ====")
			print(self.dockerFile)
			print("==== DockerCompose ====")
			print(self.dockerCompose)

		else:
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

		try:
			output = subprocess.check_output(["docker", "info", "-f", "{{println .SecurityOptions}}"])
			return b"rootless" in output

		# If docker is not present, return a default value, this is needed for testing.
		except FileNotFoundError:
			return True

	@cached_property
	def imageName(self) -> str:
		return f"{imagePrefix}{self.namePrefix}"

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
			groups = (
				[str(gid) for gid in user_groups.keys()]
				+ ["sudo"]
				+ [group for feature in self.features for group in feature.groups]
			)
			# Create all user groups if not existing.
			instructions += [f"RUN getent group {gid} || groupadd -g {gid} '{name}'" for gid, name in user_groups.items()]
			instructions += [
				# Delete any existing user with the same uid if any.
				f"RUN id -u {self.uid} &>/dev/null && userdel -r $(id -un {self.uid}) || true",
				f"RUN useradd --create-home -d {self.home} --no-log-init --uid {self.uid} --gid {self.gid} --groups {','.join(groups)} {self.user}",
				f"""RUN echo "\"{self.user}\" ALL=(ALL) NOPASSWD:ALL\" >> /etc/sudoers""",
				f'RUN chown "{self.user}" /bzd/startup.sh',
				f"USER {self.user}",
			]
		instructions += [
			f'ENV USER="{self.user}"',
			f'ENV HOME="{self.home}"',
			"ENV SHELL=/bin/bash",
		]

		instructions += [instruction for feature in self.features for instruction in feature.dockerFile]
		instructionsStr = "\n".join(instructions)

		return f"""
FROM {imageBase}
RUN apt update -y && apt upgrade -y && apt install -y \
	ca-certificates \
	python3 \
	git \
	wget \
	curl \
	patchelf \
	sudo \
	vim \
	build-essential \
	locales

RUN locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8 LC_ALL=en_US.UTF-8

RUN wget -O /usr/local/bin/bazel https://raw.githubusercontent.com/blaizard/bzd/refs/heads/master/tools/bazel && chmod +x /usr/local/bin/bazel

# Entry point script that will run once upon container start.
RUN mkdir /bzd && echo "#!/usr/bin/env bash" > /bzd/startup.sh && chmod +x /bzd/startup.sh

{instructionsStr}

RUN wget -O {self.home}/.bashrc https://raw.githubusercontent.com/blaizard/bzd/refs/heads/master/tools/shell/sh/bashrc.sh
RUN echo "__session_names+=(devcontainer)" >> {self.home}/.bashrc
"""

	@cached_property
	def dockerCompose(self) -> str:

		volumes = [
			f"{self.workspace}:{self.workspace}",
		]
		# Add some volumes from features.
		for feature in self.features:
			volumes += feature.volumes
		volumesStr = "\n".join([f"      - {volume}" for volume in volumes])

		# Additional options.
		extraService = ["command: sh -c '/bzd/startup.sh && tail -f /dev/null'"]
		extraService += [line for feature in self.features for line in feature.dockerCompose.get("service", [])]
		extraServiceStr = "\n".join([f"    {line}" for line in extraService])

		extraStr = ""
		extraVolumes = "\n".join([line for feature in self.features for line in feature.dockerCompose.get("volumes", [])])
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
    labels:
      com.bzd.devcontainer.workspace: "{self.workspace}"
      com.bzd.devcontainer.user: "{getpass.getuser()}"
      com.bzd.devcontainer.command: "{" ".join(sys.argv[1:])}"
    init: true
    tty: true
    stdin_open: true
{extraServiceStr}
    volumes:
{volumesStr}

{extraStr}
"""

	def build(self, force: bool) -> None:
		subprocess.run(
			[
				"docker",
				"compose",
				"--file",
				str(self.dockerComposePath),
				"build",
			]
			+ (["--no-cache"] if force else []),
			check=True,
		)

	def run(self, args: typing.List[str], env: typing.List[str]) -> None:
		subprocess.run(
			[
				"docker",
				"compose",
				"--file",
				str(self.dockerComposePath),
				"up",
				"--detach",
			],
			check=True,
		)

		env += [
			"LANG",
			"LC_CTYPE",
			"TERM",
			"COLORTERM",
		]
		extra = [arg for e in env for arg in ("--env", e)]

		workdir = self.cwd if self.cwd.is_relative_to(self.workspace) else self.workspace
		subprocess.run(
			[
				"docker",
				"exec",
				*extra,
				"-it",
				"--workdir",
				str(workdir),
				self.imageName,
				*(args or ["/bin/bash"]),
			]
		)

	@staticmethod
	def ls() -> None:
		result = subprocess.run(
			["docker", "ps", "--filter", f"name={imagePrefix}", "-a", "--format", "json"],
			capture_output=True,
			text=True,
			check=True,
		)
		containers = []
		for container in map(json.loads, result.stdout.splitlines()):
			labels = {
				label.split("=", 1)[0]: label.split("=", 1)[1]
				for label in container["Labels"].split(",")
				if label.startswith("com.bzd.devcontainer.")
			}
			try:
				workspace = pathlib.Path(labels["com.bzd.devcontainer.workspace"]).resolve()
				relativeWorkspace = pathlib.Path(os.path.relpath(workspace, pathlib.Path.cwd().resolve()))
				lines = [
					f"Command: {relativeWorkspace / pathlib.Path(__file__).name} {labels['com.bzd.devcontainer.command']}",
					f"User: {labels['com.bzd.devcontainer.user']}",
				]
				order = len(relativeWorkspace.parts)
			except Exception as e:
				lines, order = [f"{e.__class__.__name__}: {e}"], 999
			containers.append(
				{
					"name": container["Names"],
					"status": container["Status"],
					"active": container["State"] == "running",
					"lines": lines,
					"order": order,
				}
			)

		result = subprocess.run(
			["docker", "stats", "--no-stream", "--format", "json"] + [container["name"] for container in containers],
			capture_output=True,
			text=True,
			check=True,
		)
		allStats = [json.loads(line) for line in result.stdout.splitlines()]
		for container in sorted(containers, key=lambda u: u["order"]):
			color = "\033[32m" if container["active"] else "\033[31m"
			stats: typing.Dict[str, str] = next((d for d in allStats if d.get("Container") == container["name"]), {})
			infos = [
				container["status"].lower(),
				f"cpu={stats.get('CPUPerc', '?')}",
				f"mem={stats.get('MemPerc', '?')}",
				f"block.io={stats.get('BlockIO', '?').replace(' ', '')}",
				f"net.io={stats.get('NetIO', '?').replace(' ', '')}",
			]
			print(f"- {color}{container['name']}\033[0m: {', '.join(infos)}")
			for line in container["lines"]:
				print(f"    {line}")


if __name__ == "__main__":
	allFeatures = {
		"docker": FeatureDocker,
		"platform": FeaturePlatform,
		"volume": FeatureVolume,
		"isolation": FeatureIsolation,
		"devcontainer": FeatureDevcontainerCLI,
		"opencode": FeatureOpenCode,
		"session": FeatureSession,
	}

	allPresets = {
		f"agent{i}": [
			"--enable",
			"opencode",
			"--enable",
			"session",
			"--enable",
			"isolation",
			"--opencode",
			"--isolate",
			"--prefix",
			f"agent{i}",
		]
		for i in range(1, 10)
	}

	parser = argparse.ArgumentParser(description="Development container.")
	parser.add_argument("--build", action="store_true", help="Re-build the container if set.")
	parser.add_argument(
		"--build-force",
		action="store_true",
		help="Force re-build the container if set.",
	)
	parser.add_argument(
		"--temp",
		type=pathlib.Path,
		default=pathlib.Path(tempfile.gettempdir()),
		help="A temporary directory where to store the docker related configuration files.",
	)
	parser.add_argument(
		"--env",
		action="append",
		default=[],
		help="Environment variables to pass to the container.",
	)
	parser.add_argument(
		"--dry",
		action="store_true",
		help="Only show the DockerFile and docker-compose.yaml that it would use.",
	)
	parser.add_argument(
		"--disable",
		action="append",
		default=[],
		choices=allFeatures.keys(),
		help="Disable some of the features.",
	)
	parser.add_argument(
		"--enable",
		action="append",
		default=[],
		choices=allFeatures.keys(),
		help="Enable some of the features.",
	)
	parser.add_argument(
		"--prefix",
		help="Add a prefix in name of the container, note that this might create a new container, each container is uniquely identified by its name.",
	)
	parser.add_argument(
		"-p",
		"--preset",
		choices=allPresets.keys(),
		help="Use a predefined argument set.",
	)
	parser.add_argument(
		"rest",
		nargs=argparse.REMAINDER,
		help="Additional arguments to pass to the container.",
	)

	for FeatureClass in allFeatures.values():
		for name, kwargs in FeatureClass.cli().items():
			parser.add_argument(f"--{name}", **kwargs)

	# Read the arguments.
	args = parser.parse_args()
	if args.preset:
		args = parser.parse_args([*allPresets[args.preset], *sys.argv[1:]])
	if args.rest:
		if args.rest[0] == "ls":
			DevelopmentContainer.ls()
			sys.exit(0)
		if args.rest[0] == "--":
			args.rest = args.rest[1:]

	# Select only the features asked.
	featureNames = set(args.enable if len(args.enable) else allFeatures.keys()) - set(args.disable)
	features = [allFeatures[name](args) for name in featureNames]

	devContainer = DevelopmentContainer(args=args, features=features, temporaryPath=args.temp)
	devContainer.initialize(dry=args.dry)
	if not args.dry:
		if args.build_force:
			devContainer.build(force=True)
		if args.build:
			devContainer.build(force=False)
		devContainer.run(args=args.rest, env=args.env)
