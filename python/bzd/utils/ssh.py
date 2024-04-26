import typing
import pathlib
import tempfile
import time
from contextlib import contextmanager

from bzd.utils.run import localCommand, ExecuteResult
from bzd.bin import ssh, scp
from bzd.http.client import HttpClient


class SSH:

	def __init__(self,
	             host: str,
	             username: str,
	             identity: typing.Optional[pathlib.Path] = None,
	             port: int = 22) -> None:
		self.host = host
		self.username = username
		self.port = port

		self.commonCommands = ["-o", "StrictHostKeyChecking=no", "-o", "UserKnownHostsFile=/dev/null"]
		if identity is not None:
			self.commonCommands += ["-i", str(identity)]

	def command(self,
	            args: typing.Optional[typing.List[str]] = None,
	            sshArgs: typing.Optional[typing.List[str]] = None,
	            **kwargs: typing.Any) -> ExecuteResult:
		"""Execute a remote command via SSH."""

		command = [ssh] + self.commonCommands + (
		    sshArgs or []) + ["-p", str(self.port), "-o", "LogLevel=ERROR", f"{self.username}@{self.host}"]
		return localCommand(command + (args or []), **kwargs)

	def copyContent(self, content: str, destination: pathlib.Path) -> None:
		"""Copy the content of a file to the remote."""

		with tempfile.NamedTemporaryFile() as f:
			f.write(content.encode())
			f.flush()
			self.copy(f.name, destination)

	def copy(self, source: pathlib.Path, destination: pathlib.Path) -> None:
		"""Copy a file to the remote."""

		command = [scp] + self.commonCommands + [
		    "-P", str(self.port), str(source), f"{self.username}@{self.host}:{destination}"
		]
		localCommand(command)

	@contextmanager
	def forwardPort(self, port: int, waitHTTP: typing.Optional[str] = None, waitTimeoutS: int = 20) -> None:
		"""Forward a port."""

		socketName = f"bzd-forward-port-{time.time()}"
		try:
			self.command(sshArgs=["-M", "-S", socketName, "-fnNT", "-L", f"{port}:{self.host}:{port}"])
			if waitHTTP:
				# TODO: loop until timeout here.
				HttpClient.get(waitHTTP)
			yield
		finally:
			self.command(sshArgs=["-S", socketName, "-O", "exit"])
