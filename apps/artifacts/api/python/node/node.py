import typing
from contextlib import contextmanager

from bzd.http.utils import encodeURIComponent
from bzd.logging.handler import Log, LoggerHandler, LoggerHandlerData, LoggerHandlerFlow
from apps.artifacts.api.python.common import ArtifactsBase
from apps.artifacts.api.config import defaultNodeVolume


class NodePublishNoRemote(RuntimeError):
	pass


class PublisherProtocol(typing.Protocol):

	def __call__(self, timestamp: float, data: typing.Dict[str, typing.Any]) -> None:
		...


class Node(ArtifactsBase):

	def __init__(self, buffer: typing.Optional[int] = None, **kwargs: typing.Any) -> None:
		"""Initialize the Node object.
		
		Args:
			buffer: The number of entries that can be buffered if the server cannot be reached.
			        Once the server can be reached again, all entries will be sent again.
		"""

		super().__init__(**kwargs)
		self.buffer = buffer

	def publishMultiNodes(self, data: typing.Dict[str, typing.Any], volume: str = defaultNodeVolume) -> None:
		"""Publish data to a remote to multiple nodes.

		Args:
			data: The node uid data map to be published.
			volume: The volume to which the data should be sent.
		"""

		query = {}
		if self.token:
			query["t"] = self.token

		for remote, retry, nbRetries in self.remotes:

			url = f"{remote}/x/{volume}/"

			try:
				self.httpClient.post(url, json=data, query=query)
				return
			except Exception as e:
				if retry == nbRetries:
					self.logger.warning(f"Exception while publishing {url} after {nbRetries} retry: {str(e)}")
				pass

		raise NodePublishNoRemote("Unable to publish to any of the remotes.")

	def publish(self,
	            data: typing.Any,
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

		query = query or {}
		if self.token:
			query["t"] = self.token

		for remote, retry, nbRetries in self.remotes:

			url = f"{remote}/x/{volume}/{actualUid}/data" + (f"/{subPath}" if subPath else "") + "/"

			try:
				self.httpClient.post(url, json=data, query=query)
				return
			except Exception as e:
				if retry == nbRetries:
					self.logger.warning(f"Exception while publishing {url} after {nbRetries} retry: {str(e)}")
				pass

		raise NodePublishNoRemote("Unable to publish to any of the remotes.")

	@contextmanager
	def publishBulk(self,
	                uid: typing.Optional[str] = None,
	                volume: str = defaultNodeVolume,
	                path: typing.Optional[typing.List[str]] = None,
	                timestamp: typing.Optional[float] = None,
	                isFixedTimestamp: bool = False) -> typing.Generator[PublisherProtocol, None, None]:
		"""Publish a bulk of data to remote."""

		if isFixedTimestamp:
			assert timestamp == None, "Fixed timestamp cannot be set with the remote timestamp being set, it doesn't make sense."

		bulk: typing.List[typing.Tuple[float, typing.Any]] = []

		def publisher(timestamp: float, data: typing.Dict[str, typing.Any]) -> None:
			bulk.append((
			    timestamp,
			    data,
			))

		yield publisher

		# Calculate the timestamp.
		content: typing.Dict[str, typing.Any] = {"data": bulk}
		if not isFixedTimestamp:
			content["timestamp"] = max([t for [t, _] in bulk]) if timestamp is None else timestamp

		self.publish(data=content, uid=uid, volume=volume, path=path, query={"bulk": "1"})

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
