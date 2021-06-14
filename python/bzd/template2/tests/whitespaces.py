import unittest

from bzd.template2.template2 import Template


class TestRun(unittest.TestCase):

	def testNoStrip(self) -> None:
		template = Template("   Hello {{world}}    I am   happy ")
		result = template.process({"world": "World"})
		self.assertEqual("   Hello World    I am   happy ", result)

	def testStripLeft(self) -> None:
		template = Template("hel  {{- value }}")
		result = template.process({"value": "lo"})
		self.assertEqual("hello", result)

if __name__ == '__main__':
	unittest.main()
