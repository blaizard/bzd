import typing

from apps.artifacts.api.config import remotes

assert len(remotes) > 0, f"'remotes' from the API config cannot be empty."


class ArtifactsBase:
	"""Common libraries for the Artifacts API library."""

	def __init__(self) -> None:
		self.remote: typing.Optional[str] = None

	@property
	def remotes(self) -> typing.Generator[str, None, None]:
		"""Generator for the remotes.
		
		The usage pattern expects the user to exit the generator at the first valid remote.
		"""

		# There is a valid remote, use it.
		if self.remote:
			yield self.remote

		# No valid remote, test the potential ones.
		for remote in remotes:
			self.remote = remote
			yield self.remote

		# No remote was valid.
		self.remote = None
