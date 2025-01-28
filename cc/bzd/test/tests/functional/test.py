import unittest
import sys

from bzd.utils.run import localBazelBinary


class TestRun(unittest.TestCase):
	FAILURE_PATH = None
	SUCCESS_PATH = None

	def testAssertFailed(self) -> None:
		result = localBazelBinary(
		    path=TestRun.FAILURE_PATH,
		    stdout=True,
		    stderr=True,
		    ignoreFailure=True,
		)
		self.assertTrue(result.isFailure())

	def testAssertSuccess(self) -> None:
		result = localBazelBinary(
		    path=TestRun.SUCCESS_PATH,
		    stdout=True,
		    stderr=True,
		    ignoreFailure=True,
		)
		self.assertTrue(result.isSuccess())


if __name__ == "__main__":
	TestRun.SUCCESS_PATH = sys.argv.pop()
	TestRun.FAILURE_PATH = sys.argv.pop()
	unittest.main()
