import unittest
import typing

from tools.bdl.object import Object, ObjectContext
from tools.bdl.visitors.composition.visitor import Composition


class TestRun(unittest.TestCase):

	def testCompositionOrder(self) -> None:

		interface = Object.fromContent(content="""
			interface Hello {
			config:
				var = Integer [min(10)];
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


if __name__ == '__main__':
	unittest.main()
