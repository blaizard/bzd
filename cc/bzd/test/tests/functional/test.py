import unittest
import sys
import typing

from bzd.utils.run import localBazelBinary


class TestRun(unittest.TestCase):
	FAILURE_PATH: typing.Optional[str] = None
	SUCCESS_PATH: typing.Optional[str] = None

	def testAssertFailed(self) -> None:
		assert TestRun.FAILURE_PATH is not None
		result = localBazelBinary(
		    path=TestRun.FAILURE_PATH,
		    stdout=True,
		    stderr=True,
		    ignoreFailure=True,
		)
		self.assertTrue(result.isFailure())

	def testAssertSuccess(self) -> None:
		assert TestRun.SUCCESS_PATH is not None
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
