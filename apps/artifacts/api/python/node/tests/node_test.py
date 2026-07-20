import unittest
import typing
import json

from apps.artifacts.api.python.node.node import Node, NodePublishNoRemote
from bzd.http.client_mock import HttpClientMock


class TestRun(unittest.TestCase):
	def testPublish(self) -> None:
		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.calledCounter += 1
			self.assertEqual(method, "POST")
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
			data = json.loads(body.decode())
			self.assertEqual(
				data["data"],
				[[[], [[1, {"hello": "world1"}]]], [[], [[2, {"hello": "world2"}]]]],
			)
			self.assertIn("timestamp", data)

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		with node.publishBulk() as accessor:
			accessor(1, {"hello": "world1"})
			accessor(2, {"hello": "world2"})
		self.assertEqual(self.calledCounter, 1)

	def testPublishBulkFixedTimestamp(self) -> None:
		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.calledCounter += 1
			data = json.loads(body.decode())
			self.assertEqual(
				data,
				{"data": [[[], [[1, {"hello": "world1"}]]], [[], [[2, {"hello": "world2"}]]]]},
			)
			self.assertNotIn("timestamp", data)

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback))
		with node.publishBulk(isClientTimestamp=False) as accessor:
			accessor(1, {"hello": "world1"})
			accessor(2, {"hello": "world2"})
		self.assertEqual(self.calledCounter, 1)

	def testPublishMultiNodes(self) -> None:
		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.calledCounter += 1
			self.assertEqual(method, "POST")
			self.assertEqual(url, "http://localhost:8081/x/nodes/?bulk=1")
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
			data = json.loads(body.decode())
			values.append(data["data"][0][1][0][1])

		node = Node(uid="testuid", httpClient=HttpClientMock(callback=callback), maxBufferSize=2)
		node.publish(2)
		node.publish(3)
		self.assertEqual(self.calledCounter, 0)
		self.assertEqual(len(node.buffer), 2)

		self.callbackRaise = False
		node.publish(4)
		self.assertEqual(self.calledCounter, 3)
		self.assertEqual(values, [2, 3, 4])

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
