import unittest
from bzd.utils.timeout import Timeout, TimeoutError
import time


class TestRun(unittest.TestCase):

	def testNoTimeout(self) -> None:

		def nop() -> int:
			return 42

		result = Timeout(3600).run(nop)
		self.assertEqual(result, 42)

	def testTimeout(self) -> None:

		def throws() -> None:
			Timeout(1).run(time.sleep, 10)

		self.assertRaises(TimeoutError, throws)


if __name__ == "__main__":
	unittest.main()
