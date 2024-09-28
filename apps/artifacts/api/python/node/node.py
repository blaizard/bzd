import typing

from bzd.http.client import HttpClient
from bzd.http.utils import encodeURIComponent
from bzd.logging import Logger
from bzd.logging.handler import Log, LoggerHandler, LoggerHandlerData, LoggerHandlerFlow
from apps.artifacts.api.python.common import ArtifactsBase
from apps.artifacts.api.config import defaultNodeVolume

logger = Logger("node")


class Node(ArtifactsBase):

	def publish(self,
	            data: typing.Dict[str, typing.Any],
	            uid: typing.Optional[str] = None,
	            volume: str = defaultNodeVolume,
	            path: typing.Optional[typing.List[str]] = None) -> None:
		"""Publish data to a remote.

		Args:
			data: The data to be published.
			uid: The unique identifier of the node.
			volume: The volume to which the data should be sent.
			path: The path to publish to.
		"""

		actualUid = uid or self.uid
		assert actualUid is not None, f"No UID were specified."

		subPath = "/".join([encodeURIComponent(s) for s in path or []])

		for remote in self.remotes:

			url = f"{remote}/x/{volume}/{actualUid}/data" + (f"/{subPath}" if subPath else "")
			try:
				HttpClient.post(url, json=data)
				return
			except Exception as e:
				logger.error(f"Exception while publishing {url}: {str(e)}")
				pass

		raise RuntimeError("Unable to publish to any of the remotes.")

	def makeLoggerHandler(self) -> "LoggerHandlerNode":
		"""Create a logger handler that sends data to the node."""

		return LoggerHandlerNode(node=self)


class LoggerHandlerNode(LoggerHandler):
	"""Print to a remote node."""

	def __init__(self, node: Node) -> None:
		"""Construct the Artifacts node logger object.

		Args:
			node: The node instance associated with this logger.
		"""
		self.node = node
		self.name: typing.Optional[str] = None

	def constructor(self, name: str) -> None:
		self.name = name

	def handler(self, data: LoggerHandlerData, flow: LoggerHandlerFlow) -> None:

		assert self.name
		self.node.publish(data={"log": {self.name: [log.message for log in data], }}, )
		flow.next(data=data)
