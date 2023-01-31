import unittest
import typing
from functools import cached_property

from tools.bdl.object import Object, ObjectContext
from tools.bdl.generators.cc.visitor import Transform
from bzd.template.template import Template
from tools.bdl.visitors.composition.visitor import Composition


class TestRun(unittest.TestCase):

	@cached_property
	def composition(self) -> Composition:
		bdl = Object.fromContent(content="""
			method myMethod(arg1 = Integer(1));
			struct MyStruct {
				var = Integer(1);
			}
			interface MyInterface {
			}
			component MyComponentParent {
			config:
				var = Integer(1);
			}
			component MyComponent : MyComponentParent {
			config:
				using T = Any;
			}
			composition {
				executor = Void;
				myVar = const Integer(-2);
				myVarNotInit = Integer;
				myVarVarArgs = Vector<Integer>(1, 2, 3, 4);
				myStruct = MyStruct(2);
				myStruct2 = MyStruct(Float(2));
				myComponent = MyComponent<Float>();
			}
			""",
			objectContext=ObjectContext(resolve=True))
		composition = Composition()
		composition.visit(bdl).process()
		return composition

	def render(self, template: str) -> str:
		composition = self.composition.makeView()
		return Template("""{%- include "tools/bdl/generators/cc/template/declarations.h.btl" -%}""" + template).render(
			composition, Transform(composition=composition))

	def testSymbolToStr(self) -> None:
		output = self.render("""{{ entity("MyStruct.var").typeResolved | symbolToStr }}""")
		self.assertEqual(output, "bzd::Int32")

		output = self.render("""{{ entity("myVar").typeResolved | symbolToStr }}""")
		self.assertEqual(output, "const bzd::Int32")

		output = self.render("""{{ entity("myStruct").typeResolved | symbolToStr }}""")
		self.assertEqual(output, "MyStruct")

		output = self.render("""{{ entity("myComponent").typeResolved | symbolToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>")

	def testSymbolReferenceToStr(self) -> None:
		output = self.render("""{{ entity("MyStruct.var").typeResolved | symbolReferenceToStr }}""")
		self.assertEqual(output, "bzd::Int32&")

		output = self.render("""{{ entity("myVar").typeResolved | symbolReferenceToStr }}""")
		self.assertEqual(output, "const bzd::Int32&")

		output = self.render("""{{ entity("myStruct").typeResolved | symbolReferenceToStr }}""")
		self.assertEqual(output, "MyStruct&")

		output = self.render("""{{ entity("myComponent").typeResolved | symbolReferenceToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>&")

	def testSymbolDefinitionToStr(self) -> None:
		output = self.render("""{{ entity("MyStruct.var").typeResolved | symbolDefinitionToStr }}""")
		self.assertEqual(output, "constexpr bzd::Int32")

		output = self.render("""{{ entity("myVar").typeResolved | symbolDefinitionToStr }}""")
		self.assertEqual(output, "const constexpr bzd::Int32")

		output = self.render("""{{ entity("myStruct").typeResolved | symbolDefinitionToStr }}""")
		self.assertEqual(output, "MyStruct")

		output = self.render("""{{ entity("myComponent").typeResolved | symbolDefinitionToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>")

	def testSymbolNonConstToStr(self) -> None:
		output = self.render("""{{ entity("MyStruct.var").typeResolved | symbolNonConstToStr }}""")
		self.assertEqual(output, "bzd::Int32")

		output = self.render("""{{ entity("myVar").typeResolved | symbolNonConstToStr }}""")
		self.assertEqual(output, "bzd::Int32")

		output = self.render("""{{ entity("myStruct").typeResolved | symbolNonConstToStr }}""")
		self.assertEqual(output, "MyStruct")

		output = self.render("""{{ entity("myComponent").typeResolved | symbolNonConstToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>")

	def testSymbolRegistryToStr(self) -> None:
		output = self.render("""{{ entity("MyStruct.var").typeResolved | symbolRegistryToStr }}""")
		self.assertEqual(output, "bzd::Int32")

		output = self.render("""{{ entity("myVar").typeResolved | symbolRegistryToStr }}""")
		self.assertEqual(output, "const bzd::Int32")

		output = self.render("""{{ entity("myStruct").typeResolved | symbolRegistryToStr }}""")
		self.assertEqual(output, "MyStruct")

		output = self.render("""{{ entity("myComponent").typeResolved | symbolRegistryToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>")

	def testExpressionToValue(self) -> None:
		output = self.render("""{{ entity("MyStruct.var") | expressionToValue }}""")
		self.assertEqual(output, "bzd::Int32{1}")

		output = self.render("""{{ entity("myVar") | expressionToValue }}""")
		self.assertEqual(output, "const bzd::Int32{-2}")

		output = self.render("""{{ entity("myVarNotInit") | expressionToValue }}""")
		self.assertEqual(output, "bzd::Int32{0}")

		output = self.render("""{{ entity("myVarVarArgs") | expressionToValue }}""")
		self.assertEqual(
			output, "bzd::Vector<bzd::Int32, 4u>{bzd::inPlace, /*values*/1, /*values*/2, /*values*/3, /*values*/4}")

		output = self.render("""{{ entity("myStruct") | expressionToValue }}""")
		self.assertEqual(output, "MyStruct{/*var*/bzd::Int32{2}}")

		output = self.render("""{{ entity("myStruct2") | expressionToValue }}""")
		self.assertEqual(output, "MyStruct{/*var*/bzd::Int32{bzd::Float32{2}}}")

		output = self.render("""{{ entity("myComponent") | expressionToValue }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>{}")


if __name__ == '__main__':
	unittest.main(failfast=True)
