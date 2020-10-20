import unittest

from bzd.template.template import Template


class TestRun(unittest.TestCase):

	def testSimpleSubsitution(self) -> None:
		template = Template("Hello {world}")
		result = template.process({"world": "World"})
		self.assertEqual("Hello World", result)

	def testNestedSubsitution(self) -> None:
		template = Template("Hello {names.you}")
		result = template.process({"names": {"you": "me"}})
		self.assertEqual("Hello me", result)

	def testIfBlock(self) -> None:
		template = Template("Hello {if condition == \"2\"}Me!{end}")
		result = template.process({"condition": "1"})
		self.assertEqual("Hello ", result)
		result = template.process({"condition": "2"})
		self.assertEqual("Hello Me!", result)

	def testForBlock(self) -> None:
		template = Template("Hello{for name in names} {name}{end}")
		result = template.process({"names": ["me", "you"]})
		self.assertEqual("Hello me you", result)
		result = template.process({"names": []})
		self.assertEqual("Hello", result)

	def testForBlockKeyValue(self) -> None:
		template = Template("Hello{for name, age in names} ({name}, age={age}){end}")
		result = template.process({"names": {"me": 42, "you": 35}})
		self.assertEqual("Hello (me, age=42) (you, age=35)", result)
		result = template.process({"names": []})
		self.assertEqual("Hello", result)

	def testForBlockNested(self) -> None:
		template = Template("{for group in message}{for letter in group}{letter}{end} {end}")
		result = template.process({"message": [["H", "e", "l", "l", "o"], ["W", "o", "r", "l", "d"]]})
		self.assertEqual("Hello World ", result)

	def testInclude(self) -> None:
		template = Template("-->[{include python/bzd/template/tests/nested_template.txt}]<--")
		result = template.process({"name": "Hello World"})
		self.assertEqual("-->[Nested Hello World template]<--", result)


if __name__ == '__main__':
	unittest.main()
