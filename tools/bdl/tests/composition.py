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
				executor = Executor() [executor];
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
				executor = Executor() [executor];
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
				executor = Executor() [executor];
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
				executor = Executor() [executor];
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
				executor = Executor() [executor];
				test1 = Hello();
				test2 = Hello();
			}
			""",
		                         objectContext=ObjectContext(resolve=True))

		composition = Composition()
		composition.visit(bdl).process()

	def testNestedComposition(self) -> None:

		bdl = Object.fromContent(content="""
			component Executor { }
			component World {
			}
			component Hello {
			interface:
				method run();
			composition:
				world = World();
			}
			composition {
				executor = Executor() [executor];
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
				executor = Executor() [executor];
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
		with self.assertRaisesRegex(Exception, r"is not a valid sink IO"):
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
		with self.assertRaisesRegex(Exception, r"is already connected to"):
			Composition().visit(common).visit(composition).process()

		composition = Object.fromContent(content="""
			composition {
				connect(sender.a, receiver.a);
				connect(receiver.a, sender.a);
			}
			""",
		                                 objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"not a valid source IO"):
			Composition().visit(common).visit(composition).process()

	def testCompositionThis(self) -> None:

		composition = Object.fromContent(content="""
			component Executor { }
			component MyComponent {
			interface:
				method run();

			composition:
				this.run();
			}

			composition {
				executor = Executor() [executor];
				component1 = MyComponent();
				component2 = MyComponent();
				component3 = MyComponent();
				component3.run();
			}
			""",
		                                 objectContext=ObjectContext(resolve=True))
		Composition().visit(composition).process()

	def testCompositionThisMulti(self) -> None:

		composition = Object.fromContent(content="""
			component Executor { }
			component MyComponent {
			interface:
				method run();

			composition:
				this.run();
			}

			composition {
				executor = Executor() [executor];
				component1 = MyComponent();
				component1.run();
				component2 = MyComponent();
				component2.run();
				component3 = MyComponent();
				component3.run();
			}
			""",
		                                 objectContext=ObjectContext(resolve=True))
		Composition().visit(composition).process()

	def testExecutor(self) -> None:
		composition = Object.fromContent(content="""
				composition {
					hello = Void;
				}
				component Test {
				interface:
					method run();
				}
				composition MyComposition
				{
					a = Test() [executor(hello)];
					b = a.run();
				}
				""",
		                                 objectContext=ObjectContext(resolve=True))
		Composition().visit(composition).process()

		with self.assertRaisesRegex(Exception, r"executors between this expression"):
			composition = Object.fromContent(content="""
					component Test {
					interface:
						method run();
					}
					composition MyComposition
					{
						a = Test();
						b = a.run() [executor(hello)];
					}
					""",
			                                 objectContext=ObjectContext(resolve=True))
			Composition().visit(composition).process()


if __name__ == '__main__':
	unittest.main()
