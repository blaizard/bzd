import unittest

from bzd.template.template import Template


class TestRun(unittest.TestCase):

	def testSubstitutionEscape(self) -> None:
		template = Template("a {{{ b }} c")
		result = template.render({})
		self.assertEqual("a {{ b }} c", result)

	def testCommentEscape(self) -> None:
		template = Template("a {{# b #} c")
		result = template.render({})
		self.assertEqual("a {# b #} c", result)

	def testCommandEscape(self) -> None:
		template = Template("a {{% b %} c")
		result = template.render({})
		self.assertEqual("a {% b %} c", result)


if __name__ == "__main__":
	unittest.main()
