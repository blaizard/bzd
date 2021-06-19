import unittest

from bzd.template2.template2 import Template


class TestRun(unittest.TestCase):

	def testSimpleSubsitution(self) -> None:
		template = Template("Hello {{world}}")
		result = template.render({"world": "World"})
		self.assertEqual("Hello World", result)

	def testDeepSubsitution(self) -> None:
		template = Template("Hello {{a.b.c.d.world}}")
		result = template.render({"a": {"b": {"c": {"d": {"world": "World"}}}}})
		self.assertEqual("Hello World", result)

	def testCallableSubsitution(self) -> None:
		template = Template("Hello {{a}}")
		result = template.render({"a": lambda: 42})
		self.assertEqual("Hello 42", result)

	def testExplicitCallableSubsitution(self) -> None:
		template = Template("Hello {{a()}}")
		result = template.render({"a": lambda: 42})
		self.assertEqual("Hello 42", result)

	def testExplicitCallableArgsSubsitution(self) -> None:
		template = Template("Hello {{a(b1, b2)}}")
		result = template.render({"a": lambda x, y: x + y, "b1": "[", "b2": "]"})
		self.assertEqual("Hello []", result)

		template = Template("Hello {{a(3, 7)}}")
		result = template.render({"a": lambda x, y: x + y})
		self.assertEqual("Hello 10.0", result)

		template = Template("Hello {{a(\"agent\", \" 007\")}}")
		result = template.render({"a": lambda x, y: x + y})
		self.assertEqual("Hello agent 007", result)

	def testCallableNestedSubsitution(self) -> None:
		template = Template("Hello {{a.b}}")
		result = template.render({"a": lambda: {"b": lambda: "Me!"}})
		self.assertEqual("Hello Me!", result)

	def testPipeSubsitution(self) -> None:
		template = Template("{{str | lowerCase | capitalize }}")
		result = template.render({
			"str": "ThIs IS SOMEhting MesSy",
			"lowerCase": lambda x: x.lower(),
			"capitalize": lambda x: x.capitalize()
		})
		self.assertEqual("This is somehting messy", result)

	def testNoStrip(self) -> None:
		template = Template("   Hello {{world}}    I am   happy ")
		result = template.render({"world": "World"})
		self.assertEqual("   Hello World    I am   happy ", result)

	def testStripLeft(self) -> None:
		template = Template("Hel {{-world}}")
		result = template.render({"world": "lo"})
		self.assertEqual("Hello", result)

	def testStripRight(self) -> None:
		template = Template("{{world -}} ld")
		result = template.render({"world": "Wor"})
		self.assertEqual("World", result)

	def testStripVarious(self) -> None:
		template = Template("{{a -}} | {{- a}} | {{a}} | {{- a -}} |")
		result = template.render({"a": "0"})
		self.assertEqual("0|0 | 0 |0|", result)

	def testStripSpecialChars(self) -> None:
		template = Template(" {  12 }  }}   \n     a ")
		result = template.render({"a": "0"})
		self.assertEqual(" {  12 }  }}   \n     a ", result)

	def testIndent(self) -> None:
		template = Template("    {{ a }}", indent=True)
		result = template.render({"a": "0\n1\n2"})
		self.assertEqual("    0\n    1\n    2", result)

if __name__ == '__main__':
	unittest.main()
