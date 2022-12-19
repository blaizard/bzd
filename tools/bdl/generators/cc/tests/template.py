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
			config:
				var = Integer(1);
			}
			component MyComponent : MyInterface {
			config:
				using T = Any;
			}
			composition {
				executor = Void;
				myVar = const Integer(-2);
				myVarNotInit = Integer;
				myVarVarArgs = Vector<Integer>(1, 2, 3, 4);
				myStruct = MyStruct(2);
				myComponent = MyComponent<Float>();
			}
			""",
				objectContext=ObjectContext(resolve=True))
		composition = Composition()
		composition.visit(bdl).process()
		return composition

	def render(self, template: str) -> str:
		composition = self.composition
		return Template("""{%- include "tools/bdl/generators/cc/template/declarations.h.btl" -%}""" + template).render(composition, Transform(composition=composition))

	def testTypeToStr(self) -> None:
		output = self.render("""{{ entity("MyStruct.var").typeResolved | typeToStr }}""")
		self.assertEqual(output, "bzd::Int32")

		output = self.render("""{{ entity("myVar").typeResolved | typeToStr }}""")
		self.assertEqual(output, "const bzd::Int32")

		output = self.render("""{{ entity("myStruct").typeResolved | typeToStr }}""")
		self.assertEqual(output, "MyStruct")

		output = self.render("""{{ entity("myComponent").typeResolved | typeToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>")

	def testTypeReferenceToStr(self) -> None:
		output = self.render("""{{ entity("MyStruct.var").typeResolved | typeReferenceToStr }}""")
		self.assertEqual(output, "bzd::Int32&")

		output = self.render("""{{ entity("myVar").typeResolved | typeReferenceToStr }}""")
		self.assertEqual(output, "const bzd::Int32&")

		output = self.render("""{{ entity("myStruct").typeResolved | typeReferenceToStr }}""")
		self.assertEqual(output, "MyStruct&")

		output = self.render("""{{ entity("myComponent").typeResolved | typeReferenceToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>&")

	def testTypeDefinitionToStr(self) -> None:
		output = self.render("""{{ entity("MyStruct.var").typeResolved | typeDefinitionToStr }}""")
		self.assertEqual(output, "constexpr bzd::Int32")

		output = self.render("""{{ entity("myVar").typeResolved | typeDefinitionToStr }}""")
		self.assertEqual(output, "const constexpr bzd::Int32")

		output = self.render("""{{ entity("myStruct").typeResolved | typeDefinitionToStr }}""")
		self.assertEqual(output, "MyStruct")

		output = self.render("""{{ entity("myComponent").typeResolved | typeDefinitionToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>")

	def testTypeNonConstToStr(self) -> None:
		output = self.render("""{{ entity("MyStruct.var").typeResolved | typeNonConstToStr }}""")
		self.assertEqual(output, "bzd::Int32")

		output = self.render("""{{ entity("myVar").typeResolved | typeNonConstToStr }}""")
		self.assertEqual(output, "bzd::Int32")

		output = self.render("""{{ entity("myStruct").typeResolved | typeNonConstToStr }}""")
		self.assertEqual(output, "MyStruct")

		output = self.render("""{{ entity("myComponent").typeResolved | typeNonConstToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>")

	def testTypeRegistryToStr(self) -> None:
		output = self.render("""{{ entity("MyStruct.var").typeResolved | typeRegistryToStr }}""")
		self.assertEqual(output, "bzd::Int32")

		output = self.render("""{{ entity("myVar").typeResolved | typeRegistryToStr }}""")
		self.assertEqual(output, "const bzd::Int32")

		output = self.render("""{{ entity("myStruct").typeResolved | typeRegistryToStr }}""")
		self.assertEqual(output, "MyStruct")

		output = self.render("""{{ entity("myComponent").typeResolved | typeRegistryToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>")

	def testValueToRValue(self) -> None:
		output = self.render("""{{ entity("MyStruct.var") | valueToRValue }}""")
		self.assertEqual(output, "bzd::Int32{1}")

		output = self.render("""{{ entity("myVar") | valueToRValue }}""")
		self.assertEqual(output, "const bzd::Int32{-2}")

		output = self.render("""{{ entity("myVarNotInit") | valueToRValue }}""")
		self.assertEqual(output, "bzd::Int32{0}")

		output = self.render("""{{ entity("myVarVarArgs") | valueToRValue }}""")
		self.assertEqual(output, "bzd::Vector<bzd::Int32, 1u>{1, 2, 3, 4}")

		output = self.render("""{{ entity("myStruct") | valueToRValue }}""")
		self.assertEqual(output, "MyStruct{2}")

		output = self.render("""{{ entity("myComponent") | valueToRValue }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>{}")

if __name__ == '__main__':
	unittest.main(failfast=True)
