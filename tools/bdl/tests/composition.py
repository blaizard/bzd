import unittest
import typing

from tools.bdl.object import Object, ObjectContext
from tools.bdl.visitors.composition.visitor import Composition


class TestRun(unittest.TestCase):

	def testTemp(self) -> None:
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
				anotherDefault = 12;
				test2 = Hello(var = anotherDefault);
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

	def testNestedComposition(self) -> None:

		bdl = Object.fromContent(content="""
			component World {
			}
			component Hello {
			interface:
				method run();
			composition:
				world = World();
			}
			component Executor { }
			composition {
				executor = Executor();
				test1 = Hello();
				test2 = Hello();
				test1.run();
				test2.run();
			}
			""",
		                         objectContext=ObjectContext(resolve=True))

		composition = Composition()
		composition.visit(bdl).process()
		composition.view().entity("test1.world")
		composition.view().entity("test2.world")

	def testConnections(self) -> None:

		common = Object.fromContent(content="""
			component Executor { }
			component Sender {
			interface:
				a = Integer;
				b = Float;
			}
			component Receiver {
			interface:
				a = const Integer;
				b = const Float;
			}
			composition {
				executor = Executor();
				sender = Sender();
				receiver = Receiver();
			}
			""",
		                            objectContext=ObjectContext(resolve=True))

		composition = Object.fromContent(content="""
			composition {
				connect(sender.a, sender.a);
			}
			""",
		                                 objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"is not a valid reader IO"):
			Composition().visit(common).visit(composition).process()

		composition = Object.fromContent(content="""
			composition {
				connect(sender.a, receiver.b);
			}
			""",
		                                 objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"between the same types"):
			Composition().visit(common).visit(composition).process()

		composition = Object.fromContent(content="""
			composition {
				connect(sender.a, receiver.a);
			}
			""",
		                                 objectContext=ObjectContext(resolve=True))
		Composition().visit(common).visit(composition).process()

		composition = Object.fromContent(content="""
			composition {
				connect(sender.a, receiver.a);
				connect(sender.a, receiver.a);
			}
			""",
		                                 objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"defined multiple times"):
			Composition().visit(common).visit(composition).process()

		composition = Object.fromContent(content="""
			composition {
				connect(sender.a, receiver.a);
				connect(receiver.a, sender.a);
			}
			""",
		                                 objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"not a valid writer IO"):
			Composition().visit(common).visit(composition).process()

	def testCompositionThis(self) -> None:

		composition = Object.fromContent(content="""
			component MyComponent {
			interface:
				method run();

			composition:
				this.run();
			}

			composition {
				component1 = MyComponent();
				component2 = MyComponent();
				component3 = MyComponent();
				component3.run();
			}
			""",
		                                 objectContext=ObjectContext(resolve=True))
		Composition().visit(composition).process()


if __name__ == '__main__':
	unittest.main()
