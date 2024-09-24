import typing

from bzd.http.client import HttpClient
from bzd.http.utils import encodeURIComponent
from bzd.utils.logging import Logger
from apps.artifacts.api.python.common import ArtifactsBase
from apps.artifacts.api.config import defaultNodeVolume

logger = Logger("node")


class Node(ArtifactsBase):

	def publish(self,
	            data: typing.Dict[str, typing.Any],
	            uid: typing.Optional[str] = None,
	            volume: str = defaultNodeVolume,
	            path: str = "") -> None:
		"""Publish data to a remote.

		Args:
			data: The data to be published.
			uid: The unique identifier of the node.
			volume: The volume to which the data should be sent.
			path: The dotted path to publish to.
		"""

		actualUid = uid or self.uid
		assert actualUid is not None, f"No UID were specified."

		# Cleanup and encode the dotted path if needed.
		subPath = "/".join([encodeURIComponent(s) for s in filter(len, path.split("."))])

		for remote in self.remotes:

			url = f"{remote}/x/{volume}/{actualUid}/data" + (f"/{subPath}" if subPath else "")
			try:
				HttpClient.post(url, json=data)
				return
			except Exception as e:
				logger.error(f"Exception while publishing {url}: {str(e)}")
				pass

		raise RuntimeError("Unable to publish to any of the remotes.")
