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

		if DevelopmentContainer.isRootless():
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

	def isRunning(self) -> bool:
		"""Check if the docker container is running."""

		output = subprocess.run(["docker", "container", "inspect", "-f", "{{.State.Status}}", self.imageName], capture_output=True)
		return output.returncode == 0 and b"running" in output.stdout

	def isExited(self) -> bool:
		"""Check if the docker exited."""

		output = subprocess.run(["docker", "container", "inspect", "-f", "{{.State.Status}}", self.imageName], capture_output=True)
		return output.returncode == 0 and b"exited" in output.stdout

	@property
	def imageName(self) -> str:
		return f"devcontainer-{self.user}"

	@property
	def dockerFile(self) -> bytes:
		return f"""
FROM ubuntu:22.04

RUN apt update -y
RUN apt upgrade -y
RUN apt install -y \
	ca-certificates \
	python3 \
	git \
	wget \
	sudo

# Set the environment.
COPY ./tools/shell/sh/bashrc.sh {self.home}/.bashrc

# Add convenience links.
RUN ln -s {self.workspace}/tools/bazel /usr/local/bin/bazel
""".encode()

	def build(self) -> None:
		self.stop()
		with tempfile.NamedTemporaryFile() as dockerFile:
			dockerFile.write(self.dockerFile)
			dockerFile.flush()
			subprocess.run([
				"docker", "build",
				"--pull",
				"-f", dockerFile.name,
				"-t", f"bzd/{self.imageName}",
				self.workspace
			], check=True)

	def stop(self) -> None:
		subprocess.run(["docker", "kill", self.imageName], capture_output=True)
		subprocess.run(["docker", "rm", self.imageName], capture_output=True)

	def run(self) -> None:
		if self.isExited():
			subprocess.run([
				"docker", "start", self.imageName,
			])

		if not self.isRunning():
			self.stop()
			subprocess.run([
				"docker", "run",
				"--detach",
				"-it",
				"--name", self.imageName,
				"--hostname", self.imageName,
				"-v", f"{self.workspace}:{self.workspace}:rw",
				"-v", f"{self.homeHost}/.bash_history:{self.home}/.bash_history",
				"--workdir", self.workspace,
				f"bzd/{self.imageName}",
			])

		subprocess.run([
			"docker", "exec",
			"-it",
			self.imageName,
			"/bin/bash"
		])


if __name__== "__main__":

	parser = argparse.ArgumentParser(description="Development container.")
	parser.add_argument("-b", "--build", action="store_true", help="Re-build the container if set.")
	args = parser.parse_args()

	devContainer = DevelopmentContainer()
	if args.build:
		devContainer.build()
	devContainer.run()
