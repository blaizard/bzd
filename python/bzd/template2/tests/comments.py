import unittest

from bzd.template2.template2 import Template


class TestRun(unittest.TestCase):

	def testSimpleComment(self) -> None:
		template = Template("a {# hello #} b")
		result = template.process({})
		self.assertEqual("a  b", result)

	def testStripComment(self) -> None:
		template = Template("a {#- hello -#} b")
		result = template.process({})
		self.assertEqual("ab", result)

	def testEmptyComment(self) -> None:
		template = Template("em{# #}pty")
		result = template.process({})
		self.assertEqual("empty", result)


if __name__ == '__main__':
	unittest.main()
