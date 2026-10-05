import unittest
import pathlib

from bzd.utils.secret import decrypt, maybeDecrypt


class TestRun(unittest.TestCase):
	def testDecrypt(self) -> None:
		keyFile = pathlib.Path(__file__).parent / "test_key.txt"
		secret = pathlib.Path(__file__).parent / "test_secret.txt"
		output = decrypt(payload=secret.read_text(), keyFile=keyFile)
		self.assertEqual(output, "hello world")

	def testMaybeDecryptSecret(self) -> None:
		keyFile = pathlib.Path(__file__).parent / "test_key.txt"
		secret = pathlib.Path(__file__).parent / "test_secret.txt"
		output = maybeDecrypt(payload=secret.read_text(), keyFile=keyFile)
		self.assertEqual(output, "hello world")

	def testMaybeDecryptPlain(self) -> None:
		output = maybeDecrypt(payload="not a secret")
		self.assertEqual(output, "not a secret")


if __name__ == "__main__":
	unittest.main()
