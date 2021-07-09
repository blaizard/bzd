import unittest

from bzd.validation.validation import Validation


class TestRun(unittest.TestCase):

	def testSimpleSubsitution(self) -> None:
		template = Validation({"test": "integer"})
		self.assertTrue(template.validate({"test": "2"}))


if __name__ == '__main__':
	unittest.main()
