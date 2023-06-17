import unittest
import sys


class PythonTest(unittest.TestCase):

	def test(self):
		self.assertEqual(sys.version_info, (3, 8, 16, "final", 0))
		self.assertIn("python3_linux_x86_64/bin/python3", sys.executable)


if __name__ == '__main__':
	unittest.main()
