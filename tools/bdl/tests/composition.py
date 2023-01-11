import unittest
import typing

from tools.bdl.object import Object, ObjectContext
from tools.bdl.visitors.composition.visitor import Composition


class TestRun(unittest.TestCase):

	def testTemp(self):
		interface = Object.fromContent(content="""
			component Hello {
			config:
				var = Integer(10) [min(10)];
			}
			""",
			objectContext=ObjectContext(resolve=True))

		compositionNormal = Object.fromContent(content="""
			component Executor { }
			composition {
				executor = Executor();
			}
			composition Comp {
				default = Integer(32);
				test = Hello(var = default);
			}
			""",
			objectContext=ObjectContext(resolve=True))
		Composition().visit(interface).visit(compositionNormal).process()

	def testCompositionOrder(self) -> None:

		interface = Object.fromContent(content="""
			component Hello {
			config:
				var = Integer(10) [min(10)];
			}
			""",
			objectContext=ObjectContext(resolve=True))

		compositionNormal = Object.fromContent(content="""
			component Executor { }
			composition {
				executor = Executor();
			}
			composition Comp {
				default = Integer(32);
				test = Hello(var = default);
			}
			""",
			objectContext=ObjectContext(resolve=True))
		Composition().visit(interface).visit(compositionNormal).process()

		# Variable defined after.
		compositionWrongOrder = Object.fromContent(content="""
			component Executor { }
			composition {
				executor = Executor();
			}
			composition Comp {
				test = Hello(var = default);
				default = Integer(32);
			}
			""",
			objectContext=ObjectContext(resolve=True))
		Composition().visit(interface).visit(compositionWrongOrder).process()

		# Variable defined after with wrong contract.
		compositionWrongOrder = Object.fromContent(content="""
			component Executor { }
			composition {
				executor = Executor();
			}
			composition Comp {
				test = Hello(var = default);
				default = Integer(9);
			}
			""",
			objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"lower than"):
			Composition().visit(interface).visit(compositionWrongOrder).process()

	def testCompositionInit(self) -> None:

		bdl = Object.fromContent(content="""
			component Hello {
			interface:
				method init() [init];
			}
			component Executor { }
			composition {
				executor = Executor();
				test1 = Hello();
				test2 = Hello();
			}
			""",
			objectContext=ObjectContext(resolve=True))

		composition = Composition()
		composition.visit(bdl).process()

	def testConnections(self) -> None:

		common = Object.fromContent(content="""
			component Executor { }
			component Hello {
			interface:
				a = Integer;
				b = Float;
			}
			composition {
				executor = Executor();
				hello1 = Hello();
				hello2 = Hello();
			}
			""",
			objectContext=ObjectContext(resolve=True))

		composition = Object.fromContent(content="""
			composition {
				connect(hello1.a, hello1.a);
			}
			""",
			objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"cannot connect to itself"):
			Composition().visit(common).visit(composition).process()

		composition = Object.fromContent(content="""
			composition {
				connect(hello1.a, hello1.b);
			}
			""",
			objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"between same types"):
			Composition().visit(common).visit(composition).process()

		composition = Object.fromContent(content="""
			composition {
				connect(hello1.a, hello2.a);
			}
			""",
			objectContext=ObjectContext(resolve=True))
		Composition().visit(common).visit(composition).process()

		composition = Object.fromContent(content="""
			composition {
				connect(hello1.a, hello2.a);
				connect(hello1.a, hello2.a);
			}
			""",
			objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"defined multiple times"):
			Composition().visit(common).visit(composition).process()

		composition = Object.fromContent(content="""
			composition {
				connect(hello1.a, hello2.a);
				connect(hello2.a, hello1.a);
			}
			""",
			objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"defined as an output"):
			Composition().visit(common).visit(composition).process()


if __name__ == '__main__':
	unittest.main()
