import typing
import time
from contextlib import contextmanager

from bzd.http.utils import encodeURIComponent
from bzd.logging.handler import Log, LoggerHandler, LoggerHandlerData, LoggerHandlerFlow
from apps.artifacts.api.python.common import ArtifactsBase
from apps.artifacts.api.config import defaultNodeVolume


class NodePublishNoRemote(RuntimeError):
	pass


class PublisherProtocol(typing.Protocol):

	def __call__(self, timestampMs: float, data: typing.Dict[str, typing.Any]) -> None:
		...


class Node(ArtifactsBase):

	def __init__(self, buffer: int = 0, **kwargs: typing.Any) -> None:
		"""Initialize the Node object.
		
		Args:
			buffer: The number of entries that can be buffered if the server cannot be reached.
			        Once the server can be reached again, all entries will be sent again.
		"""

		super().__init__(**kwargs)
		self.bufferSize = buffer

	def _publishBulk(self, uri: str, data: typing.Optional[typing.List[typing.Tuple[float, typing.Any]]],
	                 isClientTimestamp: bool) -> None:
		"""Publish data to a remote.

		Args:
			uri: The path to publish to.
			data: The data to be published which are using the.
			isClientTimestamp: If true, it means that the timestamp given for each entry are the client timestamp in milliseconds.
			                   They might be adjusted to match the exact time at which they will be sent.
							   If false, they will not be modified.
		"""

		headers = {}
		if self.token:
			headers["authorization"] = f"basic {self.token}"

		content: typing.Dict[str, typing.Any] = {"data": data}

		for remote, retry, nbRetries in self.remotes:

			url = remote + uri

			try:
				if isClientTimestamp:
					content["timestamp"] = time.time() * 1000
				self.httpClient.post(url, json=content, query={"bulk": 1}, headers=headers)
				return
			except Exception as e:
				if retry == nbRetries:
					self.logger.warning(f"Exception while publishing {url} after {nbRetries} retry: {str(e)}")
				pass

		raise NodePublishNoRemote("Unable to publish to any of the remotes.")

	def _makeURI(self, uid: typing.Optional[str], volume: str, path: typing.Optional[typing.List[str]]) -> str:
		"""Generate the URI from the parameters."""

		actualUid = uid or self.uid
		assert actualUid is not None, f"No UID were specified."
		subPath = "/".join([encodeURIComponent(s) for s in path or []])
		return f"/x/{volume}/{actualUid}/data" + (f"/{subPath}" if subPath else "") + "/"

	def publishMultiNodes(self, data: typing.Dict[str, typing.Any], volume: str = defaultNodeVolume) -> None:
		"""Publish data to a remote to multiple nodes.

		Args:
			data: The node uid data map to be published.
			volume: The volume to which the data should be sent.
		"""

		timestampMs = time.time() * 1000
		self._publishBulk(uri=f"/x/{volume}/", data=[(timestampMs, data,)], isClientTimestamp=True)

	def publish(self,
	            data: typing.Any,
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

		uri = self._makeURI(uid=uid, volume=volume, path=path)

		# Use the bulk method to publish, that way we control the timestamp. This is needed as
		# data published might be published later in case of connection problem.
		self._publishBulk(uri=uri, data=[(time.time() * 1000, data,)], isClientTimestamp=True)

	@contextmanager
	def publishBulk(self,
	                uid: typing.Optional[str] = None,
	                volume: str = defaultNodeVolume,
	                path: typing.Optional[typing.List[str]] = None,
	                isClientTimestamp: bool = True) -> typing.Generator[PublisherProtocol, None, None]:
		"""Publish a bulk of data to remote.
		
		Args:
			uid: The unique identifier of the node.
			volume: The volume to which the data should be sent.
			path: The path to publish to.
			isClientTimestamp: If true, it means that the timestamp given for each entry are the client timestamp in milliseconds.
			                   They might be adjusted to match the exact time at which they will be sent.
							   If false, they will not be modified.
		"""

		bulk: typing.List[typing.Tuple[float, typing.Any]] = []

		def publisher(timestampMs: float, data: typing.Dict[str, typing.Any]) -> None:
			bulk.append((
			    timestampMs,
			    data,
			))

		yield publisher

		uri = self._makeURI(uid=uid, volume=volume, path=path)
		self._publishBulk(uri=uri, data=bulk, isClientTimestamp=isClientTimestamp)

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
