import unittest

from bzd.template2.template2 import Template


class TestRun(unittest.TestCase):

	def testSimpleMacro(self) -> None:
		template = Template("{% macro hello() %}  Hello {% end %}{{ hello }}")
		result = template.process({})
		self.assertEqual("  Hello ", result)

	def testSimMacroStrip(self) -> None:
		template = Template("{% macro hello() -%}  Hello  {%- end %}{{ hello }}")
		result = template.process({})
		self.assertEqual("Hello", result)

	def testMacroWithSubstitutions(self) -> None:
		template = Template(
			"{% macro hello(data) %}Vector<{{data.type}}, {{data.size}}>{% end %}{{ hello(small) }}\n{{ hello(large) }}"
		)
		result = template.process({"small": {"type": "int", "size": 12}, "large": {"type": "float", "size": 34}})
		self.assertEqual("Vector<int, 12>\nVector<float, 34>", result)


if __name__ == '__main__':
	unittest.main()
