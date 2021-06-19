import unittest

from bzd.template2.template2 import Template


class TestRun(unittest.TestCase):

	def testSimpleCondition(self) -> None:
		template = Template("{% if value %}hello{% end %}")

		result = template.render({"value": True})
		self.assertEqual("hello", result)

		result = template.render({"value": False})
		self.assertEqual("", result)

	def testNestedCondition(self) -> None:
		template = Template("a{% if value1 %}b{% if value2 %}c{% end %}{% end %}d")

		result = template.render({"value1": True, "value2": True})
		self.assertEqual("abcd", result)

		result = template.render({"value1": False, "value2": True})
		self.assertEqual("ad", result)

	def testEqualNumberCondition(self) -> None:
		template = Template("{% if value == 2 %}hello{% end %}")

		result = template.render({"value": 12})
		self.assertEqual("", result)

		result = template.render({"value": 2})
		self.assertEqual("hello", result)

	def testEqualStringCondition(self) -> None:
		template = Template("{% if value == \"yes\" %}hello{% end %}")

		result = template.render({"value": "no"})
		self.assertEqual("", result)

		result = template.render({"value": "yes"})
		self.assertEqual("hello", result)

	def testInCondition(self) -> None:
		template = Template("{% if value in [10, 23] %}hello{% end %}")

		result = template.render({"value": 1})
		self.assertEqual("", result)

		result = template.render({"value": 23})
		self.assertEqual("hello", result)

	def testElseCondition(self) -> None:
		template = Template("{% if value %}hello{% else %}goodbye{% end %}")

		result = template.render({"value": True})
		self.assertEqual("hello", result)

		result = template.render({"value": False})
		self.assertEqual("goodbye", result)

	def testElIfCondition(self) -> None:
		template = Template("{% if value1 %}hello{% elif value2 %}goodbye{% else %}what?!{% end %}")

		result = template.render({"value1": True})
		self.assertEqual("hello", result)

		result = template.render({"value1": False, "value2": True})
		self.assertEqual("goodbye", result)

		result = template.render({"value1": False, "value2": False})
		self.assertEqual("what?!", result)

	def testElseNestedCondition(self) -> None:
		template = Template("{% if value1 %}0{% if value2 %}1{% else %}2{% end %}{% else %}3{% end %}")

		result = template.render({"value1": True, "value2": True})
		self.assertEqual("01", result)

		result = template.render({"value1": False, "value2": True})
		self.assertEqual("3", result)

		result = template.render({"value1": True, "value2": False})
		self.assertEqual("02", result)

	def testSripCondition(self) -> None:
		template = Template("{% if value -%} hello {%- end %}")
		result = template.render({"value": True})
		self.assertEqual("hello", result)

	def testSripNestedCondition(self) -> None:
		template = Template("{% if value -%} {%if value %}  {%if value -%}    hello  {%- end %}{%- end %} {%- end %}")
		result = template.render({"value": True})
		self.assertEqual("  hello", result)

	def testSripElseIfCondition(self) -> None:
		template = Template("{% if value1 -%} a {%- elif value2 -%} b {%- else -%} c {%- end %}")
		result = template.render({"value1": True, "value2": False})
		self.assertEqual("a", result)
		result = template.render({"value1": False, "value2": True})
		self.assertEqual("b", result)
		result = template.render({"value1": False, "value2": False})
		self.assertEqual("c", result)

	def testAutoSripEndBlock(self) -> None:
		template = Template("{% if value %}hello{% end %}   ")
		result = template.render({"value": True})
		self.assertEqual("hello", result)

	def testAutoSripNewLineEndBlock(self) -> None:
		template = Template("{% if value %}hello{% end %}  \n ")
		result = template.render({"value": True})
		self.assertEqual("hello ", result)

	def testAutoSrip(self) -> None:
		template = Template("""
{% if value %}
  hello
{% end %}""")
		result = template.render({"value": True})
		self.assertEqual("\n  hello\n", result)


if __name__ == '__main__':
	unittest.main()
