import typing
import pathlib
import tempfile
import threading
import typing
import time
import math
from contextlib import contextmanager
import paramiko
import shlex
import sys

from bzd.utils.run import localCommand, ExecuteResult, ExecuteResultStreamWriter
from bzd.bin import ssh, scp
from bzd.http.client import HttpClient


class _SSHInteractiveHandle:
	"""Handler to be used with the interactive shell."""

	def __init__(self, transport: typing.Any) -> None:
		self.transport = transport

	def command(
	    self,
	    cmds: typing.List[str],
	    stdout: bool = False,
	    stderr: bool = False,
	    ignoreFailure: bool = False,
	    maxOutputSize: int = 1000000,
	) -> ExecuteResult:
		"""Execute a command on a remote host."""

		session = self.transport.open_session()
		session.exec_command(shlex.join(cmds))
		session.shutdown_write()

		stream = ExecuteResultStreamWriter(stdout=stdout, stderr=stderr, maxSize=maxOutputSize)

		def populateStreamer() -> None:
			while session.recv_ready():
				stream.addStdout(session.recv(4096))
			while session.recv_stderr_ready():
				stream.addStderr(session.recv_stderr(4096))

		while True:
			populateStreamer()
			if session.exit_status_ready():
				populateStreamer()
				break

		session.close()

		result = ExecuteResult(stream=stream, returncode=session.recv_exit_status())
		assert (ignoreFailure or result.isSuccess()
		        ), f"While executing {' '.join(cmds)}\nReturn code {result.getReturnCode()}\n{result.getOutput()}"

		return result

	def uploadContent(self, content: str, destination: pathlib.Path) -> None:
		"""Copy the content of a file to the remote."""

		with tempfile.NamedTemporaryFile() as f:
			f.write(content.encode())
			f.flush()
			self.upload(pathlib.Path(f.name), destination)

	def upload(self, source: pathlib.Path, destination: pathlib.Path) -> None:
		"""Copy a file to the remote."""

		client = self.transport.open_sftp_client()
		client.put(str(source), str(destination))
		client.close()


class SSH:

	def __init__(self,
	             host: str,
	             username: str,
	             identity: typing.Optional[pathlib.Path] = None,
	             port: int = 22) -> None:
		self.host = host
		self.username = username
		self.port = port

		self.commonCommands = ["-o", "StrictHostKeyChecking=accept-new", "-o", "UserKnownHostsFile=/dev/null"]
		if identity is not None:
			self.commonCommands += ["-i", str(identity)]

	@staticmethod
	def fromString(string: str) -> "SSH":
		"""Decompose a SSH string and gets its components.
		
		A SSH string must have the following format:
		<username>@<host>:<port>, where only <host> is mandatory.
		"""

		def splitLocation(location: str) -> typing.Tuple[str, int]:
			items = location.split(":")
			if len(items) == 1:
				return items[0], 22
			elif len(items) == 2:
				return items[0], int(items[1])
			raise Exception("Location parsing error")

		items = string.split("@")
		try:
			if len(items) == 2:
				username = items[0]
				host, port = splitLocation(items[1])
				return SSH(host=host, port=port, username=username)
			raise Exception("String parsing error")
		except Exception as e:
			raise Exception(
			    f"{str(e)}: SSH string must have the following format: '<username>@<host>:<port>', not '{string}'.")

	@contextmanager
	def interactive(self) -> typing.Generator[_SSHInteractiveHandle, None, None]:
		"""Spawn an interactive connection."""

		client = paramiko.SSHClient()  # type: ignore
		client.load_system_host_keys()  # type: ignore
		client.set_missing_host_key_policy(paramiko.AutoAddPolicy())  # type: ignore
		client.connect(hostname=self.host, port=self.port, username=self.username)  # type: ignore

		transport = client.get_transport()  # type: ignore
		if transport.is_active():
			try:
				transport.send_ignore()
			except Exception as _e:
				sys.stdout.write(str(_e))
				sys.exit(1)
		else:
			sys.exit(1)

		yield _SSHInteractiveHandle(transport)

		client.close()  # type: ignore

	def command(self,
	            args: typing.Optional[typing.List[str]] = None,
	            sshArgs: typing.Optional[typing.List[str]] = None,
	            **kwargs: typing.Any) -> ExecuteResult:
		"""Execute a remote command via SSH."""

		command = [ssh] + self.commonCommands + (sshArgs or []) + ["-p", str(self.port), f"{self.username}@{self.host}"]
		return localCommand(command + (args or []), **kwargs)

	def uploadContent(self, content: str, destination: pathlib.Path) -> None:
		"""Copy the content of a file to the remote."""

		with tempfile.NamedTemporaryFile() as f:
			f.write(content.encode())
			f.flush()
			self.upload(pathlib.Path(f.name), destination)

	def upload(self, source: pathlib.Path, destination: pathlib.Path) -> None:
		"""Copy a file to the remote."""

		command = [scp] + self.commonCommands + [
		    "-P", str(self.port), str(source), f"{self.username}@{self.host}:{destination}"
		]
		localCommand(command)

	@contextmanager
	def forwardPort(self,
	                port: int,
	                waitHTTP: typing.Optional[str] = None,
	                waitTimeoutS: int = 20) -> typing.Generator[None, None, None]:
		"""Forward a port."""

		socketName = f"bzd-forward-port-{time.time()}"
		sshThread = threading.Thread(
		    target=self.command,
		    kwargs={
		        "sshArgs": ["-M", "-S", socketName, "-fnNT", "-L", f"{port}:{self.host}:{port}"],
		        "ignoreFailure": True
		    })
		try:
			sshThread.start()
			startS = time.time()
			while True:
				timeLeftS = waitTimeoutS - (time.time() - startS)
				if timeLeftS <= 0:
					raise TimeoutError(f"Port forwarding timed-out within {waitTimeoutS}s.")
				if waitHTTP:
					try:
						result = HttpClient.get(waitHTTP, timeoutS=math.ceil(timeLeftS))
						if result.status == 200:
							break
					except:
						time.sleep(1)
						pass
				else:
					time.sleep(1)
			yield
		finally:
			self.command(sshArgs=["-S", socketName, "-O", "exit"], ignoreFailure=True)
			sshThread.join()
