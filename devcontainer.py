#!/usr/bin/env python3
"""Development container."""

import argparse
import getpass
import os
import pathlib
import tempfile
import subprocess


class DevelopmentContainer:

	def __init__(self) -> None:
		self.workspace = pathlib.Path(__file__).resolve().parent
		self.homeHost = pathlib.Path.home()
		self.rootless = DevelopmentContainer.isRootless()

		if self.rootless:
			self.user = "root"
			self.home = "/root"

		else:
			self.user = getpass.getuser()
			self.home = f"/home/{self.user}"
			self.uid = os.getuid()
			self.gid = os.getgid()

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
	def imageName(self) -> str:
		return f"devcontainer-{self.user}"

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

# Add convenience links.
RUN ln -s {self.workspace}/tools/bazel /usr/local/bin/bazel
"""

		if not self.rootless:
			dockerFile += f"""
RUN groupadd -o --gid {self.gid} {self.user}
RUN useradd --create-home --no-log-init --uid {self.uid} --gid {self.gid} {self.user}
USER {self.user}
"""

		dockerFile += f"""
# Set the environment.
COPY ./tools/shell/sh/bashrc.sh {self.home}/.bashrc
RUN echo "PS1=\\"(devcontainer) \$PS1\\"" >> {self.home}/.bashrc
"""

		return dockerFile.encode()

	def build(self) -> None:
		self.stop()
		with tempfile.NamedTemporaryFile() as dockerFile:
			dockerFile.write(self.dockerFile)
			dockerFile.flush()
			subprocess.run(
			    ["docker", "build", "--pull", "-f", dockerFile.name, "-t", f"bzd/{self.imageName}", self.workspace],
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
		elif status == "running":
			pass
		else:
			print(f"Container in '{status}' state, spawning a clean instance.")
			self.stop()
			subprocess.run([
			    "docker",
			    "run",
			    "--detach",
			    "-it",
			    "--name",
			    self.imageName,
			    "--hostname",
			    self.imageName,
			    "-v",
			    f"{self.workspace}:{self.workspace}",
			    "-v",
			    f"{self.homeHost}/.bash_history:{self.home}/.bash_history",
			    "--workdir",
			    self.workspace,
			    f"bzd/{self.imageName}",
			],
			               check=True)

		subprocess.run(["docker", "exec", "-it", self.imageName, "/bin/bash"])


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Development container.")
	parser.add_argument("-b", "--build", action="store_true", help="Re-build the container if set.")
	args = parser.parse_args()

	devContainer = DevelopmentContainer()
	if args.build:
		devContainer.build()
	devContainer.run()
