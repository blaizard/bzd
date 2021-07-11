import unittest

from bzd.validation.validation import Validation


class TestRun(unittest.TestCase):

	def testSimpleSubsitution(self) -> None:
		template = Validation({"test": "integer"})
		self.assertTrue(template.validate({"test": "2"}))
		self.assertFalse(template.validate({"test": "2.1"}))
		self.assertTrue(template.validate({"test": "2.0"}))
		self.assertTrue(template.validate({"test": "-73"}))

	def testMinSubsitution(self) -> None:
		template = Validation({"test": "integer min(2)"})
		self.assertTrue(template.validate({"test": "3"}))
		self.assertFalse(template.validate({"test": "1"}))

if __name__ == '__main__':
	unittest.main()
