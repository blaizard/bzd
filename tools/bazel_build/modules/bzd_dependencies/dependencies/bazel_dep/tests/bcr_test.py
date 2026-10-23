import json
import typing
import unittest

from bzd.http.client import HttpClientException
from bzd.http.client_mock import HttpClientMock, HttpResponseMock

from dependencies.bazel_dep.bcr import BcrRegistry


def _makeResponse(payload: typing.Dict[str, typing.Any]) -> HttpResponseMock:
	return HttpResponseMock(200, json.dumps(payload).encode("utf-8"), {})


class TestBcrRegistry(unittest.TestCase):
	def testLatestStable(self) -> None:
		urls: typing.List[str] = []

		def callback(**kwargs: typing.Any) -> HttpResponseMock:
			urls.append(kwargs["url"])
			return _makeResponse(
				{
					"versions": ["1.0.0", "1.0.1", "1.1.0", "1.2.0-rc1"],
					"yanked_versions": {"1.0.1": "broken"},
				}
			)

		registry = BcrRegistry(get=HttpClientMock(callback).get)
		self.assertEqual(registry.getLatestStable("module", "1.0.0"), "1.1.0")
		self.assertIsNone(registry.getLatestStable("module", "1.1.0"))
		self.assertEqual(urls, ["https://bcr.bazel.build/modules/module/metadata.json"])

	def testNotInRegistry(self) -> None:
		def callback(**kwargs: typing.Any) -> HttpResponseMock:
			raise HttpClientException("not found", 404, "Not Found", b"")

		registry = BcrRegistry(get=HttpClientMock(callback).get)
		self.assertIsNone(registry.getVersions("bzd_lib"))
		self.assertIsNone(registry.getLatestStable("bzd_lib", "1.0"))

	def testServerError(self) -> None:
		def callback(**kwargs: typing.Any) -> HttpResponseMock:
			raise HttpClientException("error", 500, "Internal Server Error", b"")

		registry = BcrRegistry(get=HttpClientMock(callback).get)
		with self.assertRaises(HttpClientException):
			registry.getVersions("module")


if __name__ == "__main__":
	unittest.main()
