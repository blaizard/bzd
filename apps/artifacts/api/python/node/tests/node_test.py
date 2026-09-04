import unittest
import typing
import json

from apps.artifacts.api.python.common import NodePublishNoRemote
from apps.artifacts.api.python.node.node import Node
from bzd.http.client_mock import HttpClientMock, HttpResponseMock


class TestRun(unittest.TestCase):
	def testPublish(self) -> None:
		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.calledCounter += 1
			self.assertEqual(method, "POST")
			assert body is not None
			data = json.loads(body.decode())
			# Navigate: data["data"][0] = [subKey, pairs]; pairs[0] = [ts, value]; value is [1]
			self.assertEqual(data["data"][0][1][0][1], "hello")
			self.assertIn("timestamp", data)

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		node.publish("hello")
		self.assertEqual(self.calledCounter, 1)

	def testPublishBulk(self) -> None:
		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.calledCounter += 1
			assert body is not None
			data = json.loads(body.decode())
			self.assertEqual(
				data["data"],
				[[[], [[1, {"hello": "world1"}]]], [[], [[2, {"hello": "world2"}]]]],
			)
			self.assertIn("timestamp", data)

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		with node.publishBulk() as accessor:
			accessor(timestampMs=1, value={"hello": "world1"})
			accessor(timestampMs=2, value={"hello": "world2"})
		self.assertEqual(self.calledCounter, 1)

	def testPublishBulkFixedTimestamp(self) -> None:
		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.calledCounter += 1
			assert body is not None
			data = json.loads(body.decode())
			self.assertEqual(
				data,
				{"data": [[[], [[1, {"hello": "world1"}]]], [[], [[2, {"hello": "world2"}]]]]},
			)
			self.assertNotIn("timestamp", data)

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		with node.publishBulk(isClientTimestamp=False) as accessor:
			accessor(timestampMs=1, value={"hello": "world1"})
			accessor(timestampMs=2, value={"hello": "world2"})
		self.assertEqual(self.calledCounter, 1)

	def testPublishMultiNodes(self) -> None:
		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.calledCounter += 1
			self.assertEqual(method, "POST")
			self.assertEqual(url, "http://localhost:8081/x/nodes/?bulk=1")
			assert body is not None
			data = json.loads(body.decode())
			self.assertEqual(data["data"]["hello"][0][1][0][1], "1")
			self.assertEqual(data["data"]["world"][0][1][0][1], "2")
			self.assertIn("timestamp", data)

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		node.publishMultiNodes(data={"hello": "1", "world": "2"})
		self.assertEqual(self.calledCounter, 1)

	def testPublishNoServer(self) -> None:

		def callback(**kwargs: typing.Any) -> None:
			raise Exception("Unreachable")

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		self.assertRaises(NodePublishNoRemote, lambda: node.publish(1))

	def testPublishNoServerWithBuffer(self) -> None:
		self.calledCounter = 0
		values: typing.List[typing.Any] = []
		self.callbackRaise = True

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			if self.callbackRaise:
				raise Exception("Unreachable")
			self.calledCounter += 1
			assert body is not None
			data = json.loads(body.decode())
			values.append(data["data"][0][1][0][1])

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback), maxBufferSize=2)
		self.assertRaises(NodePublishNoRemote, lambda: node.publish(2))
		self.assertEqual(len(node.buffer), 1)
		self.assertRaises(NodePublishNoRemote, lambda: node.publish(3))
		self.assertEqual(len(node.buffer), 2)
		self.assertRaises(NodePublishNoRemote, lambda: node.publish(4))
		# The buffer is capped at maxBufferSize, the oldest entry is dropped.
		self.assertEqual(len(node.buffer), 2)
		self.assertEqual(self.calledCounter, 0)

		self.callbackRaise = False
		node.publish(5)
		self.assertEqual(self.calledCounter, 2)
		self.assertEqual(values, [4, 5])

	def testPublishErrorNextRemote(self) -> None:
		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			if url.startswith("http://remote1"):
				raise Exception("Unreachable")
			self.calledCounter += 1

		node = Node(uid="testuid", remotes=["http://remote1", "http://remote2"], httpClient=HttpClientMock(callback=callback))
		node.publish("hello")
		self.assertEqual(self.calledCounter, 1)

	def testPublishErrorAllRemotes(self) -> None:

		def callback(**kwargs: typing.Any) -> None:
			raise Exception("Unreachable")

		node = Node(uid="testuid", remotes=["http://remote1", "http://remote2"], httpClient=HttpClientMock(callback=callback))
		self.assertRaises(NodePublishNoRemote, lambda: node.publish(1))

	def testPublishCachedRemoteRetries(self) -> None:
		self.calledCounter = 0
		self.callbackRaise = True

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> typing.Any:
			self.calledCounter += 1
			if self.callbackRaise:
				raise Exception("Network is down")
			return "ok"

		node = Node(uid="testuid", remotes=["http://remote1"], httpClient=HttpClientMock(callback=callback))

		# First publish succeeds, caching the remote.
		self.callbackRaise = False
		node.publish(1)
		self.assertEqual(self.calledCounter, 1)

		# The cached remote is now retried 3 times before trying the sources.
		self.callbackRaise = True
		self.assertRaises(NodePublishNoRemote, lambda: node.publish(2))
		self.assertEqual(self.calledCounter, 5)

	def testPublishBlockForSRetries(self) -> None:
		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.calledCounter += 1
			raise Exception("Unreachable")

		node = Node(uid="testuid", remotes=["http://remote1"], httpClient=HttpClientMock(callback=callback), blockForS=0.05)
		self.assertRaises(NodePublishNoRemote, lambda: node.publish(1))
		self.assertGreaterEqual(self.calledCounter, 2)

	def testExportGetErrorContinues(self) -> None:

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> typing.Any:
			if url.startswith("http://remote1"):
				raise Exception("Unreachable")
			if method == "GET":
				return HttpResponseMock(status=200, content=b'{"key": "value"}', headers={})
			raise Exception("Unexpected method")

		node = Node(uid="testuid", remotes=["http://remote1", "http://remote2"], httpClient=HttpClientMock(callback=callback))

		data = node.get(path=["a"])
		self.assertEqual(data, {"key": "value"})

		content = node.export(path=["a"])
		self.assertEqual(content, b'{"key": "value"}')

	def testPublishBulkEmpty(self) -> None:
		self.calledCounter = 0

		def callback(**kwargs: typing.Any) -> None:
			self.calledCounter += 1

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		with node.publishBulk() as _:
			pass
		self.assertEqual(self.calledCounter, 0)

	def testPublishMultiNodesEmpty(self) -> None:
		self.calledCounter = 0

		def callback(**kwargs: typing.Any) -> None:
			self.calledCounter += 1

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		node.publishMultiNodes(data={})
		self.assertEqual(self.calledCounter, 0)


if __name__ == "__main__":
	unittest.main()
