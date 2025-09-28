import typing
import os

from bzd.http.client import HttpClient
from apps.artifacts.api.config import remotes as configRemotes, token as configToken, defaultNodeVolume
from bzd.logging import Logger

assert len(configRemotes) > 0, f"'remotes' from the API config cannot be empty."


class ArtifactsBase:
	"""Common libraries for the Artifacts API library."""

	def __init__(
	        self,
	        uid: typing.Optional[str] = None,
	        remotes: typing.List[str] = configRemotes,
	        volume: str = defaultNodeVolume,
	        token: typing.Optional[str] = os.environ.get("BZD_NODE_TOKEN", configToken),
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
