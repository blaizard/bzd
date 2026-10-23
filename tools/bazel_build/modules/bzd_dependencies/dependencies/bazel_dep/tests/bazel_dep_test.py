import json
import pathlib
import tempfile
import typing
import unittest

from bzd.http.client import HttpClientException
from bzd.http.client_mock import HttpClientMock, HttpResponseMock

from dependencies.bazel_dep.bazel_dep import BazelDep
from dependencies.bazel_dep.bcr import BcrRegistry

MODULE = (
	b'module(name = "root")\n'
	b"\n"
	b'bazel_dep(name = "rules_python", version = "1.0.0")\n'
	b'#bazel_dep(name = "commented", version = "9.9.9")\n'
	b'bazel_dep(name = "local", version = "1.0")\n'
)


def _registry(versions: typing.Dict[str, typing.List[str]]) -> BcrRegistry:
	def callback(**kwargs: typing.Any) -> HttpResponseMock:
		module = str(kwargs["url"]).split("/modules/")[1].split("/")[0]
		if module not in versions:
			raise HttpClientException("not found", 404, "Not Found", b"")
		payload = {"versions": versions[module], "yanked_versions": {}}
		return HttpResponseMock(200, json.dumps(payload).encode("utf-8"), {})

	return BcrRegistry(get=HttpClientMock(callback).get)


class TestBazelDep(unittest.TestCase):
	def testGet(self) -> None:
		with tempfile.TemporaryDirectory() as directory:
			workspace = pathlib.Path(directory)
			(workspace / "MODULE.bazel").write_bytes(MODULE)
			(workspace / "sub").mkdir()
			(workspace / "sub" / "MODULE.bazel").write_bytes(b'bazel_dep(name = "rules_python", version = "1.0.0")\n')

			dependencies = BazelDep(workspace, _registry({})).get()

			self.assertEqual(
				sorted((name, version, len(extras)) for name, version, extras in dependencies.items()),
				[("local", "1.0", 1), ("rules_python", "1.0.0", 2)],
			)

	def testUpgrade(self) -> None:
		with tempfile.TemporaryDirectory() as directory:
			workspace = pathlib.Path(directory)
			(workspace / "MODULE.bazel").write_bytes(MODULE)
			dependency = BazelDep(workspace, _registry({"rules_python": ["1.0.0", "1.2.0"]}))

			dependency.upgrade(dependency.get())

			self.assertEqual(
				(workspace / "MODULE.bazel").read_bytes(),
				b'module(name = "root")\n'
				b"\n"
				b'bazel_dep(name = "rules_python", version = "1.2.0")\n'
				b'#bazel_dep(name = "commented", version = "9.9.9")\n'
				b'bazel_dep(name = "local", version = "1.0")\n',
			)

	def testUpgradeIsIdempotent(self) -> None:
		with tempfile.TemporaryDirectory() as directory:
			workspace = pathlib.Path(directory)
			(workspace / "MODULE.bazel").write_bytes(MODULE)
			dependency = BazelDep(workspace, _registry({"rules_python": ["1.0.0", "1.2.0"]}))
			dependency.upgrade(dependency.get())
			upgraded = (workspace / "MODULE.bazel").read_bytes()

			dependency.upgrade(dependency.get())

			self.assertEqual((workspace / "MODULE.bazel").read_bytes(), upgraded)

	def testUpgradePreservesFormatting(self) -> None:
		with tempfile.TemporaryDirectory() as directory:
			workspace = pathlib.Path(directory)
			(workspace / "MODULE.bazel").write_bytes(b"bazel_dep(name = 'rules_python', version = '1.0.0')\r\n")
			dependency = BazelDep(workspace, _registry({"rules_python": ["1.0.0", "1.2.0"]}))

			dependency.upgrade(dependency.get())

			self.assertEqual(
				(workspace / "MODULE.bazel").read_bytes(),
				b"bazel_dep(name = 'rules_python', version = '1.2.0')\r\n",
			)

	def testUpgradeNeverDowngrades(self) -> None:
		with tempfile.TemporaryDirectory() as directory:
			workspace = pathlib.Path(directory)
			(workspace / "MODULE.bazel").write_bytes(b'bazel_dep(name = "rules_python", version = "2.0.0")\n')
			dependency = BazelDep(workspace, _registry({"rules_python": ["1.0.0", "1.2.0"]}))

			dependency.upgrade(dependency.get())

			self.assertEqual(
				(workspace / "MODULE.bazel").read_bytes(),
				b'bazel_dep(name = "rules_python", version = "2.0.0")\n',
			)

	def testUpgradeIsAtomicOnFailure(self) -> None:
		def callback(**kwargs: typing.Any) -> HttpResponseMock:
			raise HttpClientException("error", 500, "Internal Server Error", b"")

		with tempfile.TemporaryDirectory() as directory:
			workspace = pathlib.Path(directory)
			(workspace / "MODULE.bazel").write_bytes(MODULE)
			dependency = BazelDep(workspace, BcrRegistry(get=HttpClientMock(callback).get))

			with self.assertRaises(HttpClientException):
				dependency.upgrade(dependency.get())

			self.assertEqual((workspace / "MODULE.bazel").read_bytes(), MODULE)


if __name__ == "__main__":
	unittest.main()
