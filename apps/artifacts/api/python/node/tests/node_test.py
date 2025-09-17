import unittest
import typing
import json

from apps.artifacts.api.python.node.node import Node
from bzd.http.client_mock import HttpClientMock, HttpResponseMock


class TestRun(unittest.TestCase):

	def _assertJsonBodyEqual(self, body: typing.Optional[bytes], expects: typing.Any) -> None:
		self.assertTrue(isinstance(body, bytes))
		assert body
		try:
			actual = json.loads(body.decode())
			self.assertEqual(actual, expects)
		except json.JSONDecodeError as e:
			self.fail(f"Decoding valid JSON raised an unexpected error: {e}, with content: {body.decode()}")

	def testPublish(self) -> None:

		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.assertEqual(method, "POST")
			self.assertEqual(url, "http://localhost:8081/x/nodes/testuid/data/")
			self._assertJsonBodyEqual(body, "hello")
			self.calledCounter += 1

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		node.publish(data="hello")
		self.assertEqual(self.calledCounter, 1)

	def testPublishBulk(self) -> None:

		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.assertEqual(method, "POST")
			self.assertEqual(url, "http://localhost:8081/x/nodes/testuid/data/?bulk=1")
			self._assertJsonBodyEqual(body, {
			    "data": [[1, {
			        "hello": "world1"
			    }], [2, {
			        "hello": "world2"
			    }]],
			    "timestamp": 2
			})
			self.calledCounter += 1

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		with node.publishBulk() as accessor:
			accessor(timestamp=1, data={"hello": "world1"})
			accessor(timestamp=2, data={"hello": "world2"})
		self.assertEqual(self.calledCounter, 1)

	def testPublishBulkFixeTimestamp(self) -> None:

		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.assertEqual(method, "POST")
			self.assertEqual(url, "http://localhost:8081/x/nodes/testuid/data/?bulk=1")
			self._assertJsonBodyEqual(body, {"data": [[1, {"hello": "world1"}], [2, {"hello": "world2"}]]})
			self.calledCounter += 1

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		with node.publishBulk(isFixedTimestamp=True) as accessor:
			accessor(timestamp=1, data={"hello": "world1"})
			accessor(timestamp=2, data={"hello": "world2"})
		self.assertEqual(self.calledCounter, 1)


if __name__ == "__main__":
	unittest.main()
