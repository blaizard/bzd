import unittest

from python.bzd.utils.run import localBazelBinary


class TestRun(unittest.TestCase):

	def testAssertFailed(self) -> None:
		result = localBazelBinary(
		    path="cc/bzd/test/tests/functional/failure",
		    stdout=True,
		    stderr=True,
		    ignoreFailure=True,
		)
		self.assertTrue(result.isFailed())

	def testAssertSuccess(self) -> None:
		result = localBazelBinary(
		    path="cc/bzd/test/tests/functional/success",
		    stdout=True,
		    stderr=True,
		    ignoreFailure=True,
		)
		self.assertTrue(result.isSuccess())


if __name__ == "__main__":
	unittest.main()
