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


if __name__ == "__main__":
	unittest.main()
