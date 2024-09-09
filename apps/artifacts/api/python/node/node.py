import typing

from bzd.http.client import HttpClient
from bzd.http.utils import encodeURIComponent
from bzd.utils.logging import Logger
from apps.artifacts.api.python.common import ArtifactsBase

logger = Logger("node")


class Node(ArtifactsBase):

	def publish(self, uid: str, data: typing.Dict[str, typing.Any], path: str = "") -> None:
		"""Publish data to a remote.

		Args:
			uid: The unique identifier of the node.
			data: The data to be published.
			path: The dotted path to publish to.
		"""

		# Cleanup and encode the dotted path if needed.
		subPath = "/".join([encodeURIComponent(s) for s in filter(len, path.split("."))])

		for remote in self.remotes:

			url = f"{remote}/{uid}/data" + (f"/{subPath}" if subPath else "")
			try:
				HttpClient.post(url, json=data)
				return
			except Exception as e:
				logger.error(f"Exception while publishing {url}: {str(e)}")
				pass

		raise RuntimeError("Unable to publish to any of the remotes.")
