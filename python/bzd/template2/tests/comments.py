import unittest

from bzd.template2.template2 import Template


class TestRun(unittest.TestCase):

	def testSimpleComment(self) -> None:
		template = Template("a {# hello #} b")
		result = template.render({})
		self.assertEqual("a  b", result)

	def testStripComment(self) -> None:
		template = Template("a {#- hello -#} b")
		result = template.render({})
		self.assertEqual("ab", result)

	def testEmptyComment(self) -> None:
		template = Template("em{# #}pty")
		result = template.render({})
		self.assertEqual("empty", result)

	def testAutoStripComment(self) -> None:
		template = Template("{# Comment #}  \n here ")
		result = template.render({})
		self.assertEqual(" here ", result)


if __name__ == '__main__':
	unittest.main()
