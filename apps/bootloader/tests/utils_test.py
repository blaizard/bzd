import unittest
from apps.bootloader.utils import RollingNamedTemporaryFile


class TestRun(unittest.TestCase):
	def testRollingNamedTemporaryFile(self) -> None:

		temp = RollingNamedTemporaryFile(namespace="hello", maxFiles=2)
		temp.reset()
		self.assertEqual(len(temp.all()), 0)

		path1 = temp.get()
		self.assertEqual(len(temp.all()), 1)
		self.assertEqual(temp.all(), [path1])

		path2 = temp.get()
		self.assertEqual(len(temp.all()), 2)
		self.assertEqual(temp.all(), [path2, path1])

		path3 = temp.get()
		self.assertEqual(len(temp.all()), 2)
		self.assertEqual(temp.all(), [path3, path2])

		temp2 = RollingNamedTemporaryFile(namespace="hello", maxFiles=2)
		self.assertEqual(len(temp.all()), 2)
		self.assertEqual(len(temp2.all()), 2)
		path4 = temp.get()
		self.assertEqual(len(temp.all()), 2)
		self.assertEqual(len(temp2.all()), 2)
		self.assertEqual(temp.all(), [path4, path3])
		self.assertEqual(temp2.all(), [path4, path3])

		temp2.reset()
		self.assertEqual(len(temp.all()), 0)

	def testConcurrentCleanupTolerance(self) -> None:

		temp = RollingNamedTemporaryFile(namespace="concurrent_cleanup", maxFiles=2)
		temp.reset()
		self.assertEqual(len(temp.all()), 0)

		# Create leftover files, as if left behind by a crashed or concurrent process.
		paths = [
			temp.temporaryDirectory / "tmp.1000.aaaa",
			temp.temporaryDirectory / "tmp.1001.bbbb",
			temp.temporaryDirectory / "tmp.1002.cccc",
			temp.temporaryDirectory / "tmp.1003.dddd",
		]
		for path in paths:
			path.touch()

		# Simulate another process having already removed a file that get() will try to prune.
		paths[1].unlink()

		# This used to raise FileNotFoundError when pruning files concurrently.
		path = temp.get()
		self.assertEqual(len(temp.all()), 2)
		self.assertEqual(temp.all(), [path, paths[3]])

		# The lock file must be ignored by all().
		lockPath = temp.temporaryDirectory / ".lock"
		self.assertTrue(lockPath.exists())
		self.assertNotIn(lockPath, temp.all())

		# reset() must not raise when files were already removed from the namespace.
		leftovers = [
			temp.temporaryDirectory / "tmp.2000.eeee",
			temp.temporaryDirectory / "tmp.2001.ffff",
		]
		for leftover in leftovers:
			leftover.touch()
		leftovers[0].unlink()
		temp.reset()
		self.assertEqual(len(temp.all()), 0)


if __name__ == "__main__":
	unittest.main()
