import typing
from contextlib import contextmanager

from bzd.http.client import HttpClient
from bzd.http.utils import encodeURIComponent
from bzd.logging.handler import Log, LoggerHandler, LoggerHandlerData, LoggerHandlerFlow
from apps.artifacts.api.python.common import ArtifactsBase
from apps.artifacts.api.config import defaultNodeVolume


class NodePublishNoRemote(RuntimeError):
	pass


class Node(ArtifactsBase):

	def publish(self,
	            data: typing.Dict[str, typing.Any],
	            uid: typing.Optional[str] = None,
	            volume: str = defaultNodeVolume,
	            path: typing.Optional[typing.List[str]] = None,
	            query: typing.Optional[typing.Dict[str, str]] = None) -> None:
		"""Publish data to a remote.

		Args:
			data: The data to be published.
			uid: The unique identifier of the node.
			volume: The volume to which the data should be sent.
			path: The path to publish to.
			query: Query URL to be used while posting.
		"""

		actualUid = uid or self.uid
		assert actualUid is not None, f"No UID were specified."

		subPath = "/".join([encodeURIComponent(s) for s in path or []])

		for remote in self.remotes:

			url = f"{remote}/x/{volume}/{actualUid}/data" + (f"/{subPath}" if subPath else "")
			try:
				HttpClient.post(url, json=data, query=query)
				return
			except Exception as e:
				self.logger.warning(f"Exception while publishing {url}: {str(e)}")
				pass

		raise NodePublishNoRemote("Unable to publish to any of the remotes.")

	@contextmanager
	def publishBulk(
	    self,
	    uid: typing.Optional[str] = None,
	    volume: str = defaultNodeVolume,
	    path: typing.Optional[typing.List[str]] = None,
	    timestamp: typing.Optional[float] = None
	) -> typing.Generator[typing.Callable[[float, typing.Dict[str, typing.Any]], None], None, None]:
		"""Publish a bulk of data to remote."""

		bulk: typing.List[typing.Tuple[float, typing.Any]] = []

		def publisher(timestamp: float, data: typing.Dict[str, typing.Any]) -> None:
			bulk.append((
			    timestamp,
			    data,
			))

		yield publisher

		# Calculate the timestamp.
		timestamp = max([t for [t, _] in bulk]) if timestamp is None else timestamp

		self.publish(data={
		    "timestamp": timestamp,
		    "data": bulk
		},
		             uid=uid,
		             volume=volume,
		             path=path,
		             query={"bulk": "1"})

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
		try:
			with self.node.publishBulk(path=["log"]) as publisher:
				for log in data:
					publisher(
					    log.timestamp,
					    {
					        self.name: {
					            "name": log.name,
					            "level": log.level,
					            "source": f"{log.filename}:{log.line}",
					            "message": log.message,
					        },
					    },
					)
		except NodePublishNoRemote as e:
			self.node.logger.error(str(e))

		flow.next(data=data)
