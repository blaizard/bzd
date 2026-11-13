import subprocess
import unittest

from config_python import a, b, b_c, e, embed, bin_hello_world


class TestRun(unittest.TestCase):
	def testSimple(self) -> None:
		self.assertEqual(a(), 42)
		self.assertEqual(b_c(), "hello")
		self.assertEqual(b(), {"c": "hello", "d": False})
		self.assertEqual(e(), [1, {"f": 2}, {'{"f":2}': "quoted"}])

	def testBinary(self) -> None:
		binaryPath = bin_hello_world()
		self.assertTrue(binaryPath.is_file())
		result = subprocess.run([binaryPath.as_posix()], capture_output=True, text=True, check=True)
		self.assertEqual(result.stdout.strip(), "hello world")

	def testEmbed(self) -> None:
		self.assertEqual(embed(), b"hello\n")


if __name__ == "__main__":
	unittest.main()
