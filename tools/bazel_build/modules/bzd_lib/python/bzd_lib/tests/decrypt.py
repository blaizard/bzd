import unittest
import pathlib

from bzd_lib.secret import decrypt


class TestRun(unittest.TestCase):
	def testDecrypt(self) -> None:
		keyFile = pathlib.Path(__file__).parent / "test_key.txt"
		secret = pathlib.Path(__file__).parent / "test_secret.txt"
		output = decrypt(payload=secret.read_text(), keyFile=keyFile)
		self.assertEqual(output, "hello world")


if __name__ == "__main__":
	unittest.main()
