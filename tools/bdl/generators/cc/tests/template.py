import unittest
import typing

from tools.bdl.object import Object, ObjectContext
from tools.bdl.generators.cc.visitor import Transform
from bzd.template.template import Template

class TestRun(unittest.TestCase):

	@staticmethod
	def render(template: str) -> str:
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
				myVar = const Integer(-2);
				myStruct = MyStruct(2);
				myComponent = MyComponent<Float>();
			}
			""",
				objectContext=ObjectContext(resolve=True, composition=True))

		return Template("""{%- include "tools/bdl/generators/cc/template/declarations.h.btl" -%}""" + template).render(bdl.tree, Transform())

	def testTemp1(self):
		output = self.render("""{{ entity("myStruct").typeResolved | typeDefinitionToStr }}""")
		self.assertEqual(output, "MyStruct<2>")


	def testTemplate(self) -> None:
		output = self.render("""{{ entity("myMethod") | declareMethod }}""")
		self.assertEqual(output, "void myMethod(bzd::Int32& arg1 = bzd::Int32{1})")

		output = self.render("""{{ entity("MyStruct.var").typeResolved | typeDefinitionToStr }}""")
		self.assertEqual(output, "constexpr bzd::Int32<1>")

		output = self.render("""{{ entity("myVar").typeResolved | typeDefinitionToStr }}""")
		self.assertEqual(output, "const constexpr bzd::Int32<-2>")

		output = self.render("""{{ entity("myStruct").typeResolved | typeDefinitionToStr }}""")
		self.assertEqual(output, "MyStruct<2>")

		output = self.render("""{{ entity("myComponent").typeResolved | typeDefinitionToStr }}""")
		self.assertEqual(output, "MyComponent<bzd::Float32>")

if __name__ == '__main__':
	unittest.main(failfast=True)
