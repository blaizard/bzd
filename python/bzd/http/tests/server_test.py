import json
import unittest

from bzd.http.server_mock import makeRESTServerContext


class TestRESTServerContextRead(unittest.TestCase):
	def testReadEmptyBody(self) -> None:
		context = makeRESTServerContext(data=b"")
		self.assertEqual(context.read(), b"")

	def testReadRawBytesWithoutContentLength(self) -> None:
		context = makeRESTServerContext(data=b"hello world", headers={})
		self.assertEqual(context.read(), b"")

	def testReadWithContentLength(self) -> None:
		context = makeRESTServerContext(data=b"hello world", headers={"Content-Length": "5"})
		self.assertEqual(context.read(), b"hello")

	def testReadIsCached(self) -> None:
		context = makeRESTServerContext(data=b"once")
		first = context.read()
		self.assertEqual(first, b"once")
		second = context.read()
		self.assertEqual(second, b"once")

	def testReadWithZeroContentLength(self) -> None:
		context = makeRESTServerContext(data=b"hello world", headers={"Content-Length": "0"})
		self.assertEqual(context.read(), b"")


class TestRESTServerContextReadJson(unittest.TestCase):
	def testReadJsonObject(self) -> None:
		context = makeRESTServerContext(data=b'{"ttl":900,"name":"hello"}')
		self.assertEqual(context.readJson(), {"ttl": 900, "name": "hello"})

	def testReadJsonRaisesOnMalformed(self) -> None:
		context = makeRESTServerContext(data=b"not json")
		with self.assertRaises(json.JSONDecodeError):
			context.readJson()

	def testReadJsonEmptyRaises(self) -> None:
		context = makeRESTServerContext(data=b"")
		with self.assertRaises(json.JSONDecodeError):
			context.readJson()


class TestRESTServerContextWrite(unittest.TestCase):
	def testWriteString(self) -> None:
		context = makeRESTServerContext()
		context.write("hello")
		self.assertEqual(context.response.data, [b"hello"])

	def testWriteBytes(self) -> None:
		context = makeRESTServerContext()
		context.write(b"raw")
		self.assertEqual(context.response.data, [b"raw"])

	def testWriteMultiple(self) -> None:
		context = makeRESTServerContext()
		context.write(b"raw")
		context.write("not raw")
		self.assertEqual(context.response.data, [b"raw", b"not raw"])

	def testHeader(self) -> None:
		context = makeRESTServerContext()
		context.header("Content-Type", "application/json")
		self.assertIn(("Content-Type", "application/json"), context.response.headers)


if __name__ == "__main__":
	unittest.main()
