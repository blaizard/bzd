import unittest

from bzd.template2.template2 import Template


class TestRun(unittest.TestCase):

	def testSimpleInclude(self) -> None:
		template = Template("{% include \"python/bzd/template2/tests/support/include.btl\" %}")
		result = template.render({"name": "You"})
		self.assertEqual("Hello You\n", result)

	def testIncludeMacro(self) -> None:
		template = Template(
			"{% include \"python/bzd/template2/tests/support/include_macro.btl\" %} {{ goodbye(\"you\") }}")
		result = template.render({})
		self.assertEqual(" Goodbye you", result)


if __name__ == '__main__':
	unittest.main()
