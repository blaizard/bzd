import unittest
import typing
import json
import time

from apps.artifacts.api.python.node.node import Node, NodePublishNoRemote
from bzd.http.client_mock import HttpClientMock


class TestRun(unittest.TestCase):

	def _assertJsonBodyEqual(self,
	                         body: typing.Optional[bytes],
	                         expects: typing.Optional[typing.Any] = None,
	                         callback: typing.Optional[typing.Callable[..., None]] = None,
	                         args: typing.List[typing.Any] = []) -> None:
		self.assertTrue(isinstance(body, bytes))
		assert body
		try:
			actual = json.loads(body.decode())
			if expects is not None:
				self.assertEqual(actual, expects)
			if callback is not None:
				callback(actual, *args)
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

	def testPublishNoServer(self) -> None:

		def callback(**kwargs: typing.Any) -> None:
			raise Exception("Unreachable")

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		self.assertRaises(NodePublishNoRemote, lambda: node.publish(1))

	def testPublishNoServerWithBuffer(self) -> None:

		self.callbackRaise = True
		self.capture: typing.List[bytes] = []
		timestampStart = time.time() * 1000

		def callback(body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			if self.callbackRaise:
				raise Exception("Unreachable")
			else:
				assert body
				self.capture.append(body)

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback), maxBufferSize=1)

		# Expected to do nothing, the value is buffered.
		node.publish(1)
		self.assertEqual(self.capture, [])

		# Expected to throw, the buffer is full.
		self.assertRaises(NodePublishNoRemote, lambda: node.publish(2))

		# Expected to publish remaining buffered values.
		self.callbackRaise = False
		node.publish(3)
		self.assertEqual(len(self.capture), 2)
		entry1, entry2 = self.capture

		def check(data: typing.Any, expectedValue: int) -> None:
			self.assertEqual(len(data["data"]), 1)
			timestamp, value = data["data"][0]
			self.assertEqual(value, expectedValue)
			self.assertEqual(len(data["data"]), 1)

			self.assertGreaterEqual(timestamp, timestampStart)
			self.assertLessEqual(timestamp, time.time() * 1000)

			self.assertGreaterEqual(data["timestamp"], timestampStart)
			self.assertLessEqual(data["timestamp"], time.time() * 1000)

		self._assertJsonBodyEqual(entry1, callback=check, args=[2])
		self._assertJsonBodyEqual(entry2, callback=check, args=[3])


if __name__ == "__main__":
	unittest.main()
