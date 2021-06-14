import unittest

from bzd.template2.template2 import Template


class TestRun(unittest.TestCase):

	def testForControl(self) -> None:
		template = Template("{% for char in sequence %}{{char}}{% end %}")
		result = template.process({"sequence": ["a", "b", "c", "d"]})
		self.assertEqual("abcd", result)

	def testForControlWithText(self) -> None:
		template = Template("{% for words in sequence %}I am {{words}} control, {% end %}")
		result = template.process({"sequence": ["in", "not in"]})
		self.assertEqual("I am in control, I am not in control, ", result)

	def testForControlNested(self) -> None:
		template = Template("{% for group in message %}{% for letter in group %}{{letter}}{%end%} {%end%}")
		result = template.process({"message": [["H", "e", "l", "l", "o"], ["W", "o", "r", "l", "d"]]})
		self.assertEqual("Hello World ", result)

	def testForWithIndexControl(self) -> None:
		template = Template("{% for index, char in sequence %}{{index}}:{{char}},{% end %}")
		result = template.process({"sequence": ["a", "b", "c", "d"]})
		self.assertEqual("0:a,1:b,2:c,3:d,", result)


if __name__ == '__main__':
	unittest.main()
