import unittest

from bzd.validation.validation import Validation, ExceptionValidation


class TestRun(unittest.TestCase):

	def testInteger(self) -> None:
		template = Validation({"test": "integer"})
		template.validate({"test": "2"})
		with self.assertRaisesRegex(ExceptionValidation, r"expects.*integer"):
			template.validate({"test": "2.1"})
		template.validate({"test": "2.0"})
		template.validate({"test": "-73"})

	def testFloat(self) -> None:
		template = Validation({"test": "float"})
		template.validate({"test": "2"})
		template.validate({"test": "2.1"})
		template.validate({"test": "2.0"})
		template.validate({"test": "-73"})
		with self.assertRaisesRegex(ExceptionValidation, r"expects.*float"):
			template.validate({"test": "hsi"})

	def testString(self) -> None:
		template = Validation({"test": "string"})
		template.validate({"test": "2"})
		template.validate({"test": "dsdsd"})

	def testMinMax(self) -> None:
		template = Validation({"test": "integer min(2) max(10)"})
		self.assertDictEqual(template.validate({"test": "3"}).values, {"test": 3})
		with self.assertRaisesRegex(ExceptionValidation, r"lower than"):
			template.validate({"test": "1"})
		with self.assertRaisesRegex(ExceptionValidation, r"higher than"):
			template.validate({"test": "11"})
		self.assertDictEqual(template.validate({"test": "10"}).values, {"test": 10})
		self.assertDictEqual(template.validate({"test": "2"}).values, {"test": 2})

	def testMinMaxLength(self) -> None:
		template = Validation({"test": "string min(2) max(10)"})
		self.assertDictEqual(template.validate({"test": "hello"}).values, {"test": "hello"})
		with self.assertRaisesRegex(ExceptionValidation, r"shorter than"):
			template.validate({"test": "h"})
		with self.assertRaisesRegex(ExceptionValidation, r"longer than"):
			template.validate({"test": "hello world"})

	def testOutputReturn(self) -> None:
		template = Validation({"test": "integer"})
		self.assertTrue(template.validate({"test": "2"}, output="return"))
		self.assertFalse(template.validate({"test": "2.1"}, output="return"))


if __name__ == '__main__':
	unittest.main()
