import typing

from apps.artifacts.api.config import remotes
from bzd.logging import Logger

assert len(remotes) > 0, f"'remotes' from the API config cannot be empty."


class ArtifactsBase:
	"""Common libraries for the Artifacts API library."""

	def __init__(self, uid: typing.Optional[str] = None, logger: Logger = Logger("artifacts.api")) -> None:
		"""Construct the Artifacts API object.

		Args:
			uid: The unique identifier of the caller.
		"""

		self.uid = uid
		self.logger = logger
		self.remote: typing.Optional[str] = None

	@property
	def remotes(self) -> typing.Generator[str, None, None]:
		"""Generator for the remotes.
		
		The usage pattern expects the user to exit the generator at the first valid remote.
		"""

		# There is a valid remote, use it.
		if self.remote:
			for i in range(3):  # Retries.
				yield self.remote, i, 2

		# No valid remote, test the potential ones.
		for remote in remotes:
			self.remote = remote
			yield self.remote, 0, 0

		# No remote was valid.
		self.remote = None
