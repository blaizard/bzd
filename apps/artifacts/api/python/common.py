import typing
import os
import time

from bzd.http.client import HttpClient
from apps.artifacts.api.python.config import (
	configRemotes,
	configToken,
	configDefaultNodeVolume,
)
from bzd.logging import Logger

assert len(configRemotes()) > 0, "'remotes' from the API config cannot be empty."

T = typing.TypeVar("T")


class NodePublishNoRemote(RuntimeError):
	pass


class ArtifactsBase:
	"""Common libraries for the Artifacts API library."""

	def __init__(
		self,
		uid: typing.Optional[str] = None,
		remotes: typing.List[str] = configRemotes(),
		volume: str = configDefaultNodeVolume(),
		token: typing.Optional[str] = os.environ.get("BZD_NODE_TOKEN", configToken()),
		logger: Logger = Logger("artifacts.api"),
		httpClient: typing.Any = HttpClient,
	) -> None:
		"""Construct the Artifacts API object.

		Args:
		        uid: The unique identifier of the caller.
		        remotes: The remotes to be used.
		        volume: The volume to be used.
		        token: The application token to be used.
		        logger: The logger to be used.
		        httpClient: The http client object to be used.
		"""

		self.uid = uid
		self.remoteSources = remotes
		self.volume = volume
		self.token = token
		self.logger = logger
		self.remote: typing.Optional[str] = None
		self.httpClient = httpClient

	@property
	def remotes(self) -> typing.Generator[typing.Tuple[str, int, int], None, None]:
		"""Generator for the remotes.

		The usage pattern expects the user to exit the generator at the first valid remote.
		"""

		# There is a valid remote, use it.
		if self.remote:
			for i in range(3):  # Retries.
				yield self.remote, i, 2

		# No valid remote, test the potential ones.
		for remote in self.remoteSources:
			self.remote = remote
			yield self.remote, 0, 0

		# No remote was valid.
		self.remote = None

	def _tryRemotes(
		self,
		callback: typing.Callable[[str], T],
		errorMessage: str,
		retryForS: typing.Optional[float] = None,
	) -> T:
		"""Run the callback against each remote until one succeeds.

		Any exception raised by the callback is logged, with the remote and retry information, and the next remote is tried.
		If retryForS is set, all the remotes are retried every 30 s at most until the given deadline.
		If nothing succeeded, raise NodePublishNoRemote.

		Args:
		        callback: The function to be called with the remote as argument.
		        errorMessage: The error message of the raised NodePublishNoRemote.
		        retryForS: The maximal number of seconds to retry before giving up.
		"""

		timestampStart = time.time()
		while True:
			for remote, retry, nbRetries in self.remotes:
				try:
					return callback(remote)
				except Exception as e:
					self.logger.error(f"Remote '{remote}' failed (attempt {retry + 1}/{nbRetries + 1}): {str(e)}")
			if retryForS is None:
				break
			timestampElapsed = time.time() - timestampStart
			if timestampElapsed > retryForS:
				break
			time.sleep(min(retryForS - timestampElapsed, 30))
		raise NodePublishNoRemote(errorMessage)

	@staticmethod
	def pathToKey(path: str) -> typing.List[str]:
		"""Convert a path into a normalized sequence of keys.

		For example: /a/b//../c -> [a,c]
		"""
		normalizedPath = os.path.normpath(path)
		return [element for element in normalizedPath.split("/") if len(element)]

	def _repr(self) -> typing.List[str]:
		content = []
		if self.uid is not None:
			content.append(f"uid={self.uid}")
		content.append(f"remotes={','.join(self.remoteSources)}")
		if self.volume is not None:
			content.append(f"volume={self.volume}")
		if self.token is not None:
			content.append("token=xxx")
		return content

	def __repr__(self) -> str:
		"""String representation of the object."""

		return f"<{self.__class__.__name__} {' '.join(self._repr())}>"
