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

	def testExplicitCallableSubsitution(self) -> None:
		template = Template("Hello {{a()}}")
		result = template.process({"a": lambda: 42})
		self.assertEqual("Hello 42", result)

	def testExplicitCallableArgsSubsitution(self) -> None:
		template = Template("Hello {{a(b1, b2)}}")
		result = template.process({"a": lambda x, y: x + y, "b1": "[", "b2": "]"})
		self.assertEqual("Hello []", result)

		template = Template("Hello {{a(3, 7)}}")
		result = template.process({"a": lambda x, y: x + y})
		self.assertEqual("Hello 10.0", result)

		template = Template("Hello {{a(\"agent\", \" 007\")}}")
		result = template.process({"a": lambda x, y: x + y})
		self.assertEqual("Hello agent 007", result)

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

	def testNoStrip(self) -> None:
		template = Template("   Hello {{world}}    I am   happy ")
		result = template.process({"world": "World"})
		self.assertEqual("   Hello World    I am   happy ", result)

	def testStripLeft(self) -> None:
		template = Template("Hel {{-world}}")
		result = template.process({"world": "lo"})
		self.assertEqual("Hello", result)

	def testStripRight(self) -> None:
		template = Template("{{world -}} ld")
		result = template.process({"world": "Wor"})
		self.assertEqual("World", result)

	def testStripVarious(self) -> None:
		template = Template("{{a -}} | {{- a}} | {{a}} | {{- a -}} |")
		result = template.process({"a": "0"})
		self.assertEqual("0|0 | 0 |0|", result)

	def testStripSpecialChars(self) -> None:
		template = Template(" {  12 }  }}   \n     a ")
		result = template.process({"a": "0"})
		self.assertEqual(" {  12 }  }}   \n     a ", result)


if __name__ == '__main__':
	unittest.main()
