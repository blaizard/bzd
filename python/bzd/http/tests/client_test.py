import unittest
import typing
import json

from bzd.http.client_mock import HttpClientMock


class TestRun(unittest.TestCase):

	def testSimple(self) -> None:

		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.assertEqual(method, "GET")
			self.assertEqual(url, "http://hello.com")
			self.assertTrue(body is None)
			self.calledCounter += 1

		HttpClientMock(callback=callback).get("http://hello.com")
		self.assertEqual(self.calledCounter, 1)

	def testQuery(self) -> None:

		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], **kwargs: typing.Any) -> None:
			self.assertEqual(method, "DELETE")
			self.assertEqual(url, "http://hello.com?hello=world")
			self.assertTrue(body is None)
			self.calledCounter += 1

		HttpClientMock(callback=callback).delete("http://hello.com", query={"hello": "world"})
		self.assertEqual(self.calledCounter, 1)

	def testJson(self) -> None:

		self.calledCounter = 0

		def callback(method: str, url: str, body: typing.Optional[bytes], headers: typing.Dict[str, str],
		             **kwargs: typing.Any) -> None:
			self.assertEqual(method, "PUT")
			self.assertEqual(url, "http://hello.com")
			assert body
			self.assertEqual(body, b"{\"hello\": \"world\"}")
			self.assertEqual(headers, {
			    "content-type": "application/json; charset=utf-8",
			    "content-length": str(len(body))
			})
			self.calledCounter += 1

		HttpClientMock(callback=callback).put("http://hello.com", json={"hello": "world"})
		self.assertEqual(self.calledCounter, 1)

	def testResponse(self) -> None:

		def callback(**kwargs: typing.Any) -> bytes:
			return b"{\"hello\": \"world\"}"

		response = HttpClientMock(callback=callback).get("http://hello.com")
		self.assertEqual(response.status, 200)
		self.assertEqual(response.content, b"{\"hello\": \"world\"}")
		self.assertEqual(response.json, {"hello": "world"})


if __name__ == "__main__":
	unittest.main()
