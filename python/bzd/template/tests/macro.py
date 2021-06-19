import unittest

from bzd.template.template import Template


class TestRun(unittest.TestCase):

	def testSimpleMacro(self) -> None:
		template = Template("{% macro hello() %}  Hello {% end %}{{ hello }}")
		result = template.render({})
		self.assertEqual("  Hello ", result)

	def testMacroStrip(self) -> None:
		template = Template("{% macro hello() -%}  Hello  {%- end %}{{ hello }}")
		result = template.render({})
		self.assertEqual("Hello", result)

	def testMacroWithSubstitutions(self) -> None:
		template = Template(
			"{% macro hello(data) %}Vector<{{data.type}}, {{data.size}}>{% end %}{{ hello(small) }}\n{{ hello(large) }}"
		)
		result = template.render({"small": {"type": "int", "size": 12}, "large": {"type": "float", "size": 34}})
		self.assertEqual("Vector<int, 12>\nVector<float, 34>", result)

	def testMacroSameName(self) -> None:
		template = Template(
			"{% macro world(name) -%} {{ name }} world {%- end %}{% macro hello(name) -%} {{ world(name) }} {%- end %}{{ hello(\"hello\") }}"
		)
		result = template.render({})
		self.assertEqual("hello world", result)


if __name__ == '__main__':
	unittest.main()
