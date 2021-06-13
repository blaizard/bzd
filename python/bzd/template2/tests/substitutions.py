import unittest

from bzd.template2.template2 import Template


class TestRun(unittest.TestCase):

	def testSimpleSubsitution(self) -> None:
		template = Template("Hello {{world}}")
		result = template.process({"world": "World"})
		self.assertEqual("Hello World", result)

	def testDeepSubsitution(self) -> None:
		template = Template("Hello {{a.b.c.d.world}}")
		result = template.process({"a": {"b": {"c": {"d": {"world": "World"}}}}})
		self.assertEqual("Hello World", result)

	def testCallableSubsitution(self) -> None:
		template = Template("Hello {{a}}")
		result = template.process({"a": lambda: 42})
		self.assertEqual("Hello 42", result)

	def testCallableNestedSubsitution(self) -> None:
		template = Template("Hello {{a.b}}")
		result = template.process({"a": lambda: {"b": lambda: "Me!"}})
		self.assertEqual("Hello Me!", result)

	def testPipeSubsitution(self) -> None:
		template = Template("{{str | lowerCase | capitalize }}")
		result = template.process({
			"str": "ThIs IS SOMEhting MesSy",
			"lowerCase": lambda x: x.lower(),
			"capitalize": lambda x: x.capitalize()
		})
		self.assertEqual("This is somehting messy", result)


if __name__ == '__main__':
	unittest.main()
