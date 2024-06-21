import typing
from contextlib import contextmanager

from bzd.utils.ssh import SSH


class TransportSSHHandle:

	def __init__(self, handle: typing.Any, ssh: typing.Any) -> None:
		self.handle = handle
		self.ssh = ssh

	def command(self, *args: typing.Any, **kwargs: typing.Any) -> None:
		return self.handle.command(*args, **kwargs)

	def uploadContent(self, *args: typing.Any, **kwargs: typing.Any) -> None:
		return self.handle.uploadContent(*args, **kwargs)

	def forwardPort(self, *args: typing.Any, **kwargs: typing.Any) -> None:
		return self.ssh.forwardPort(*args, **kwargs)


class TransportSSH:

	def __init__(self, connection: str) -> None:
		self.ssh = SSH.fromString(connection)

	def forwardPort(self, *args: typing.Any, **kwargs: typing.Any) -> None:
		return self.ssh.forwardPort(*args, **kwargs)

	@contextmanager
	def session(self) -> None:
		with self.ssh.interactive() as handle:
			yield TransportSSHHandle(handle, self.ssh)
