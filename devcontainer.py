#!/usr/bin/env python3
"""Development container."""

import argparse
import getpass
import os
import pathlib
import tempfile
import subprocess
import typing


class DevelopmentContainer:

	def __init__(self, platform: typing.Optional[str] = None) -> None:
		self.workspace = pathlib.Path(__file__).resolve().parent
		self.homeHost = pathlib.Path.home()
		self.platform = platform
		self.rootless = DevelopmentContainer.isRootless()

		if self.rootless:
			self.user = "root"
			self.home = "/root"

		else:
			self.user = getpass.getuser()
			self.home = f"/home/{self.user}"
			self.uid = os.getuid()
			self.gid = os.getgid()

		if self.platform:
			self.startEmulation()

	@staticmethod
	def isRootless() -> bool:
		"""Check if docker is rootless."""

		output = subprocess.check_output(["docker", "info", "-f", "{{println .SecurityOptions}}"])
		return b"rootless" in output

	def getStatus(self) -> str:
		"""Get the status of the container."""

		output = subprocess.run(["docker", "container", "inspect", "-f", "{{.State.Status}}", self.imageName],
		                        capture_output=True)
		if output.returncode == 0:
			return output.stdout.decode().lower().strip()
		return "missing"

	@property
	def dockerPlatformArgs(self) -> typing.List[str]:
		if self.platform == "amd64":
			return ["--platform", "linux/amd64"]
		if self.platform == "arm64":
			return ["--platform", "linux/arm64"]
		return []

	@property
	def imageName(self) -> str:
		name = f"devcontainer-{self.user}"
		if self.platform:
			name += f"-{self.platform}"
		return name

	@property
	def dockerFile(self) -> bytes:
		dockerFile = f"""
FROM ubuntu:24.04

RUN apt update -y
RUN apt upgrade -y
RUN apt install -y \
	ca-certificates \
	python3 \
	git \
	wget \
	sudo

RUN wget -O /usr/local/bin/bazel https://raw.githubusercontent.com/blaizard/bzd/refs/heads/master/tools/bazel && chmod +x /usr/local/bin/bazel
"""

		if not self.rootless:
			# Add a new user.
			dockerFile += f"""
RUN groupadd -o --gid {self.gid} {self.user}
RUN useradd --create-home --no-log-init --uid {self.uid} --gid {self.gid} {self.user}
USER {self.user}
"""

		# Set the environment.
		dockerFile += f"""
RUN wget -O {self.home}/.bashrc https://raw.githubusercontent.com/blaizard/bzd/refs/heads/master/tools/shell/sh/bashrc.sh
RUN echo "PS1=\\"(devcontainer) \$PS1\\"" >> {self.home}/.bashrc
"""

		return dockerFile.encode()

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
		self.stop()
		with tempfile.NamedTemporaryFile() as dockerFile:
			dockerFile.write(self.dockerFile)
			dockerFile.flush()
			subprocess.run(["docker", "build", "--pull", "-f", dockerFile.name, "-t", f"bzd/{self.imageName}"] +
			               self.dockerPlatformArgs + [self.workspace],
			               check=True)

	def stop(self) -> None:
		subprocess.run(["docker", "kill", self.imageName], capture_output=True)
		subprocess.run(["docker", "rm", self.imageName], capture_output=True)

	def run(self) -> None:

		status = self.getStatus()
		if status == "exited":
			print("Container stopped, restarting.")
			subprocess.run(["docker", "start", self.imageName], capture_output=True, check=True)
		elif status == "paused":
			print("Container paused, unpausing.")
			subprocess.run(["docker", "unpause", self.imageName], capture_output=True, check=True)
		elif status != "running":
			print(f"Container in '{status}' state, spawning a clean instance.")
			self.stop()
			subprocess.run(
			    [
			        "docker",
			        "run",
			        "--detach",
			        "-it",
			        "--name",
			        self.imageName,
			        "--hostname",
			        self.imageName,
			    ] + self.dockerPlatformArgs + [
			        "-v",
			        f"{self.workspace}:{self.workspace}",
			        "-v",
			        f"{self.homeHost}/.bash_history:{self.home}/.bash_history",
			        # To access bazel cache.
			        "-v",
			        f"{self.homeHost}/.cache:{self.homeHost}/.cache",
			        "--workdir",
			        self.workspace,
			        f"bzd/{self.imageName}",
			    ],
			    check=True)

		subprocess.run(["docker", "exec", "-it", self.imageName, "/bin/bash"])


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Development container.")
	parser.add_argument("-b", "--build", action="store_true", help="Re-build the container if set.")
	parser.add_argument("-p",
	                    "--platform",
	                    choices=(
	                        "amd64",
	                        "arm64",
	                    ),
	                    help="The architecture on which the docker should run.")
	args = parser.parse_args()

	devContainer = DevelopmentContainer(args.platform)
	if args.build:
		devContainer.build()
	devContainer.run()
