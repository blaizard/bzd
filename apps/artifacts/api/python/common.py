import typing
import os

from apps.artifacts.api.config import remotes as configRemotes, token as configToken
from bzd.logging import Logger

assert len(configRemotes) > 0, f"'remotes' from the API config cannot be empty."


class ArtifactsBase:
	"""Common libraries for the Artifacts API library."""

	def __init__(
	    self,
	    uid: typing.Optional[str] = None,
	    remotes: typing.List[str] = configRemotes,
	    token: typing.Optional[str] = os.environ.get("BZD_NODE_TOKEN", configToken),
	    logger: Logger = Logger("artifacts.api")
	) -> None:
		"""Construct the Artifacts API object.

		Args:
			uid: The unique identifier of the caller.
			token: The application token to be used.
			logger: The logger to be used.
		"""

		self.uid = uid
		self.remoteSources = remotes
		self.token = token
		self.logger = logger
		self.remote: typing.Optional[str] = None

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
