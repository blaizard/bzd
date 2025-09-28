import typing
import time
import json
from contextlib import contextmanager
from dataclasses import dataclass
from urllib.parse import urlparse

from bzd.http.utils import encodeURIComponent
from bzd.logging.handler import Log, LoggerHandler, LoggerHandlerData, LoggerHandlerFlow
from apps.artifacts.api.python.common import ArtifactsBase
from apps.artifacts.api.config import defaultNodeVolume


class NodePublishNoRemote(RuntimeError):
	pass


class PublisherProtocol(typing.Protocol):

	def __call__(self, timestampMs: float, data: typing.Dict[str, typing.Any]) -> None:
		...


@dataclass
class BufferEntryBulk:
	"""A buffer entry for a bulk publishing request."""

	# The path to publish to.
	uri: str
	# The data to be published and their associated timestamp.
	data: typing.List[typing.Tuple[float, typing.Any]]
	# If true, it means that the timestamp given for each entry are the client timestamp in milliseconds.
	# They might be adjusted to match the exact time at which they will be sent.
	# If false, they will not be modified.
	isClientTimestamp: bool


class Node(ArtifactsBase):

	def __init__(self,
	             path: typing.Optional[typing.List[str]] = None,
	             maxBufferSize: int = 0,
	             blockForS: typing.Optional[int] = None,
	             **kwargs: typing.Any) -> None:
		"""Initialize the Node object.
		
		Args:
			path: The default path to be used.
			maxBufferSize: The maximal number of entries that can be buffered if the server cannot be reached.
			Once the server can be reached again, all entries will be sent again.
			blockForS: The publish call will be blocking for a number of seconds if no remote can be accessed.
		"""

		super().__init__(**kwargs)
		self.path = path or []
		self.maxBufferSize = maxBufferSize
		self.blockForS = blockForS
		self.buffer: typing.List[BufferEntryBulk] = []

	@staticmethod
	def fromUrl(url: str) -> "Node":
		"""Create a node object from a URL."""

		parsedUrl = urlparse(url)

		remote = f"{parsedUrl.scheme}://{parsedUrl.netloc}"
		key = Node.pathToKey(parsedUrl.path)
		if len(key) > 0 and key[0] == "view":
			key.pop(0)
		volume = key.pop(0) if len(key) > 0 else None
		uid = key.pop(0) if len(key) > 0 else None
		if len(key) > 0 and key[0] == "data":
			key.pop(0)
		path = key if len(key) > 0 else None

		return Node(remotes=[remote], volume=volume, uid=uid, path=path)

	def _repr(self) -> typing.List[str]:
		content = super()._repr()
		if len(self.path):
			content.append(f"path=/{'/'.join(self.path)}")
		return content

	def _publish(self, entry: BufferEntryBulk) -> None:
		"""Publish data to a remote.

		Args:
			entry: The entry to be published.
		"""

		headers = {}
		if self.token:
			headers["authorization"] = f"basic {self.token}"

		self.buffer.append(entry)
		timestampStart = time.time()

		while True:

			for remote, retry, nbRetries in self.remotes:

				try:
					while len(self.buffer):
						entry = self.buffer[0]
						content: typing.Dict[str, typing.Any] = {"data": entry.data}
						url = remote + entry.uri
						if entry.isClientTimestamp:
							content["timestamp"] = time.time() * 1000
						self.httpClient.post(url, json=content, query={"bulk": 1}, headers=headers)
						self.buffer.pop(0)
					return

				except Exception as e:
					if retry == nbRetries:
						self.logger.warning(f"Exception while publishing {url} after {nbRetries} retry: {str(e)}")
					pass

			# The buffer can contain maxBufferSize + 1 element.
			if len(self.buffer) < self.maxBufferSize + 1:
				return

			# If blockForS is not set or expired, exit the loop.
			if self.blockForS is None:
				break
			timestampElapsed = (time.time() - timestampStart)
			if timestampElapsed > self.blockForS:
				break

			# Else wait for 30s max before retrying.
			timestampRemaining = max(self.blockForS - timestampElapsed, 0)
			time.sleep(min(timestampRemaining, 30))

		self.buffer = self.buffer[-self.maxBufferSize:]
		raise NodePublishNoRemote("Unable to publish to any of the remotes.")

	def _makeURI(self,
	             uid: typing.Optional[str],
	             volume: typing.Optional[str] = None,
	             path: typing.Optional[typing.List[str]] = None,
	             endpoint: str = "data") -> str:
		"""Generate the URI from the parameters."""

		endpointToComponent = {"data": "", "export": "@export/"}

		actualUid = uid or self.uid
		assert actualUid is not None, f"No UID were specified."
		assert endpoint in endpointToComponent, f"The endpoint type '{endpoint}' does not exist."
		subPath = "/".join([encodeURIComponent(s) for s in (self.path + (path or []))])
		return f"/x/{volume or self.volume}/{endpointToComponent[endpoint]}{actualUid}/data" + (f"/{subPath}" if subPath
		                                                                                        else "") + "/"

	def publishMultiNodes(self, data: typing.Dict[str, typing.Any], volume: typing.Optional[str] = None) -> None:
		"""Publish data to a remote to multiple nodes.

		Args:
			data: The node uid data map to be published.
			volume: The volume to which the data should be sent.
		"""

		timestampMs = time.time() * 1000
		self._publish(
		    BufferEntryBulk(uri=f"/x/{volume or self.volume}/", data=[(
		        timestampMs,
		        data,
		    )], isClientTimestamp=True))

	def publish(self,
	            data: typing.Any,
	            uid: typing.Optional[str] = None,
	            volume: typing.Optional[str] = None,
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
		self._publish(BufferEntryBulk(uri=uri, data=[(
		    time.time() * 1000,
		    data,
		)], isClientTimestamp=True))

	@contextmanager
	def publishBulk(self,
	                uid: typing.Optional[str] = None,
	                volume: typing.Optional[str] = None,
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
		self._publish(BufferEntryBulk(uri=uri, data=bulk, isClientTimestamp=isClientTimestamp))

	def makeLoggerHandler(self) -> "LoggerHandlerNode":
		"""Create a logger handler that sends data to the node."""

		return LoggerHandlerNode(node=self)

	def export(self,
	           uid: typing.Optional[str] = None,
	           volume: typing.Optional[str] = None,
	           path: typing.Optional[typing.List[str]] = None,
	           after: typing.Optional[int] = None,
	           before: typing.Optional[int] = None,
	           children: int = 0,
	           format: str = "csv") -> bytes:
		"""Export data from the given node.

		Args:
			uid: The unique identifier of the node.
			volume: The volume corresponding to the data.
			path: The path at which the data shall be exported.
			after: The starting timestamp when to gather the data.
			before: The ending timestamp when to gather the data.
			children: The number of depth level to include.
			format: The export format of the data.

		Returns:
			Data as bytes.
		"""

		uri = self._makeURI(uid=uid, volume=volume, path=path, endpoint="export")
		headers = {}
		if self.token:
			headers["authorization"] = f"basic {self.token}"

		query = {"children": str(children), "format": format}
		if after is not None:
			query["after"] = str(after)
		if before is not None:
			query["before"] = str(before)

		for remote, retry, nbRetries in self.remotes:
			url = remote + uri
			response = self.httpClient.get(url, headers=headers, query=query)
			return response.content  #  type: ignore

		raise NodePublishNoRemote("Unable to export from any of the remotes.")

	def get(self,
	        uid: typing.Optional[str] = None,
	        volume: typing.Optional[str] = None,
	        path: typing.Optional[typing.List[str]] = None,
	        after: typing.Optional[int] = None,
	        before: typing.Optional[int] = None,
	        children: typing.Optional[int] = None,
	        keys: typing.Optional[bool] = None) -> typing.Any:
		"""Get data from the nodes remote volumes.."""

		uri = self._makeURI(uid=uid, volume=volume, path=path)
		headers = {}
		if self.token:
			headers["authorization"] = f"basic {self.token}"

		query = {}
		if children is not None:
			query["children"] = str(children)
		if after is not None:
			query["after"] = str(after)
		if before is not None:
			query["before"] = str(before)
		if keys is not None:
			query["keys"] = str(1 if keys else 0)

		for remote, retry, nbRetries in self.remotes:
			url = remote + uri
			response = self.httpClient.get(url, headers=headers, query=query)
			return response.json

		raise NodePublishNoRemote("Unable to export from any of the remotes.")


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
