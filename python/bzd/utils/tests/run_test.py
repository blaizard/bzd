import unittest
import bzd.utils.run
import threading
import time


class TestRun(unittest.TestCase):

	def testLocalCommand(self) -> None:
		result = bzd.utils.run.localCommand(["echo", "123"])
		self.assertEqual(result.getReturnCode(), 0)
		self.assertEqual(result.getStdout().strip(), "123")
		self.assertEqual(result.getStderr().strip(), "")
		self.assertEqual(result.getOutput().strip(), "123")

	def testLocalCommandTimeout(self) -> None:
		result = bzd.utils.run.localCommand(["sleep", "5"], ignoreFailure=True, timeoutS=0.1)
		self.assertNotEqual(result.getReturnCode(), 0)
		self.assertEqual(result.getStdout().strip(), "")
		self.assertIn("timed out", result.getStderr())  # codespell:ignore
		self.assertIn("timed out", result.getOutput())  # codespell:ignore

	def testLocalCommandException(self) -> None:
		with self.assertRaises(Exception):
			bzd.utils.run.localCommand(["sleep", "5"], timeoutS=0.1)

	def testLocalCommandCancellation(self) -> None:
		cancellation = bzd.utils.run.Cancellation()

		def threadCancel():
			cancellation.cancel()

		threading.Thread(target=threadCancel).start()
		result = bzd.utils.run.localCommand(["sleep", "5"], ignoreFailure=True, cancellation=cancellation)
		self.assertNotEqual(result.getReturnCode(), 0)
		self.assertEqual(result.getStdout().strip(), "")
		self.assertTrue(cancellation.triggered)
		self.assertIn("cancelled", result.getStderr())  # codespell:ignore
		self.assertIn("cancelled", result.getOutput())  # codespell:ignore

		cancellation.reset()
		self.assertFalse(cancellation.triggered)


if __name__ == "__main__":
	unittest.main()
