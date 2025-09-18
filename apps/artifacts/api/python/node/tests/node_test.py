import unittest
import typing
import json
import time

from apps.artifacts.api.python.node.node import Node
from bzd.http.client_mock import HttpClientMock


class TestRun(unittest.TestCase):

	def _assertJsonBodyEqual(self,
	                         body: typing.Optional[bytes],
	                         expects: typing.Optional[typing.Any] = None,
	                         callback: typing.Optional[typing.Callable[[typing.Any], None]] = None) -> None:
		self.assertTrue(isinstance(body, bytes))
		assert body
		try:
			actual = json.loads(body.decode())
			if expects is not None:
				self.assertEqual(actual, expects)
			if callback is not None:
				callback(actual)
		except json.JSONDecodeError as e:
			self.fail(f"Decoding valid JSON raised an unexpected error: {e}, with content: {body.decode()}")

	def testPublish(self) -> None:

		self.calledCounter = 0
		timestampStart = time.time() * 1000

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.assertEqual(method, "POST")
			self.assertEqual(url, "http://localhost:8081/x/nodes/testuid/data/?bulk=1")

			def check(data: typing.Any) -> None:
				timestamp, value = data["data"][0]
				self.assertGreaterEqual(timestamp, timestampStart)
				self.assertLessEqual(timestamp, time.time() * 1000)
				self.assertEqual(value, "hello")
				self.assertGreaterEqual(data["timestamp"], timestampStart)
				self.assertLessEqual(data["timestamp"], time.time() * 1000)

			self._assertJsonBodyEqual(body, callback=check)
			self.calledCounter += 1

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		node.publish(data="hello")
		self.assertEqual(self.calledCounter, 1)

	def testPublishBulk(self) -> None:

		self.calledCounter = 0
		timestampStart = time.time() * 1000

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.assertEqual(method, "POST")
			self.assertEqual(url, "http://localhost:8081/x/nodes/testuid/data/?bulk=1")

			def check(data: typing.Any) -> None:
				self.assertEqual(data["data"], [[1, {"hello": "world1"}], [2, {"hello": "world2"}]])
				self.assertGreaterEqual(data["timestamp"], timestampStart)
				self.assertLessEqual(data["timestamp"], time.time() * 1000)

			self._assertJsonBodyEqual(body, callback=check)
			self.calledCounter += 1

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		with node.publishBulk() as accessor:
			accessor(timestampMs=1, data={"hello": "world1"})
			accessor(timestampMs=2, data={"hello": "world2"})
		self.assertEqual(self.calledCounter, 1)

	def testPublishBulkFixeTimestamp(self) -> None:

		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.assertEqual(method, "POST")
			self.assertEqual(url, "http://localhost:8081/x/nodes/testuid/data/?bulk=1")
			self._assertJsonBodyEqual(body, {"data": [[1, {"hello": "world1"}], [2, {"hello": "world2"}]]})
			self.calledCounter += 1

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		with node.publishBulk(isClientTimestamp=False) as accessor:
			accessor(timestampMs=1, data={"hello": "world1"})
			accessor(timestampMs=2, data={"hello": "world2"})
		self.assertEqual(self.calledCounter, 1)

	def testPublishMultiNodes(self) -> None:

		self.calledCounter = 0
		timestampStart = time.time() * 1000

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.assertEqual(method, "POST")
			self.assertEqual(url, "http://localhost:8081/x/nodes/?bulk=1")

			def check(data: typing.Any) -> None:
				self.assertEqual(len(data["data"]), 1)
				timestamp, value = data["data"][0]
				self.assertEqual(value, {"hello": "1", "world": "2"})
				self.assertEqual(len(data["data"]), 1)

				self.assertGreaterEqual(timestamp, timestampStart)
				self.assertLessEqual(timestamp, time.time() * 1000)

				self.assertGreaterEqual(data["timestamp"], timestampStart)
				self.assertLessEqual(data["timestamp"], time.time() * 1000)

			self._assertJsonBodyEqual(body, callback=check)
			self.calledCounter += 1

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		node.publishMultiNodes(data={"hello": "1", "world": "2"})
		self.assertEqual(self.calledCounter, 1)


if __name__ == "__main__":
	unittest.main()
