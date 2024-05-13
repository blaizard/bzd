import unittest
import typing

from bdl.object import Object, ObjectContext
from bdl.visitors.composition.visitor import Composition


class TestRun(unittest.TestCase):

	def testTemp(self) -> None:
		interface = Object.fromContent(
		    content="""
			component Hello {
			config:
				var = Integer(10) [min(10)];
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)

		compositionNormal = Object.fromContent(
		    content="""
			namespace default;
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
		    objectContext=ObjectContext(resolve=True),
		)
		Composition(targets={"default"}).visit(interface).visit(compositionNormal).process()

	def testCompositionOrder(self) -> None:
		interface = Object.fromContent(
		    content="""
			namespace default;
			component Hello {
			config:
				var = Integer(10) [min(10)];
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)

		compositionNormal = Object.fromContent(
		    content="""
			namespace default;
			component Executor { }
			composition {
				executor = Executor() [executor];
			}
			composition Comp {
				default = Integer(32);
				test = Hello(var = default);
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)
		Composition(targets={"default"}).visit(interface).visit(compositionNormal).process()

		# Variable defined after.
		compositionWrongOrder = Object.fromContent(
		    content="""
			namespace default;
			component Executor { }
			composition {
				executor = Executor() [executor];
			}
			composition Comp {
				test = Hello(var = default);
				default = Integer(32);
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)
		Composition(targets={"default"}).visit(interface).visit(compositionWrongOrder).process()

		# Variable defined after with wrong contract.
		compositionWrongOrder = Object.fromContent(
		    content="""
			namespace default;
			component Executor { }
			composition {
				executor = Executor() [executor];
			}
			composition Comp {
				test = Hello(var = default);
				default = Integer(9);
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)
		with self.assertRaisesRegex(Exception, r"lower than"):
			Composition(targets={"default"}).visit(interface).visit(compositionWrongOrder).process()

	def testCompositionInit(self) -> None:
		bdl = Object.fromContent(
		    content="""
			namespace default;
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
		    objectContext=ObjectContext(resolve=True),
		)

		composition = Composition(targets={"default"})
		composition.visit(bdl).process()

	def testNestedComposition(self) -> None:
		bdl = Object.fromContent(
		    content="""
			component Executor { }
			component World {
			}
			component Hello {
			interface:
				method run();
			composition:
				world = World();
			}
			composition default {
				executor = Executor() [executor];
				test1 = Hello();
				test2 = Hello();
				test1.run();
				test2.run();
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)

		composition = Composition(targets={"default"})
		composition.visit(bdl).process()
		composition.view(target="default").entity("default.test1.world")
		composition.view(target="default").entity("default.test2.world")

	def testConnections(self) -> None:
		common = Object.fromContent(
		    content="""
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
			composition default {
				executor = Executor() [executor];
				sender = Sender();
				receiver = Receiver();
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)

		composition = Object.fromContent(
		    content="""
			composition {
				connect(default.sender.a, default.sender.a);
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)
		with self.assertRaisesRegex(Exception, r"is not a valid sink IO"):
			Composition(targets={"default"}).visit(common).visit(composition).process()

		composition = Object.fromContent(
		    content="""
			composition {
				connect(default.sender.a, default.receiver.b);
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)
		with self.assertRaisesRegex(Exception, r"between the same types"):
			Composition(targets={"default"}).visit(common).visit(composition).process()

		composition = Object.fromContent(
		    content="""
			composition {
				connect(default.sender.a, default.receiver.a);
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)
		Composition(targets={"default"}).visit(common).visit(composition).process()

		composition = Object.fromContent(
		    content="""
			composition {
				connect(default.sender.a, default.receiver.a);
				connect(default.sender.a, default.receiver.a);
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)
		with self.assertRaisesRegex(Exception, r"is already connected to"):
			Composition(targets={"default"}).visit(common).visit(composition).process()

		composition = Object.fromContent(
		    content="""
			composition {
				connect(default.sender.a, default.receiver.a);
				connect(default.receiver.a, default.sender.a);
			}
			""",
		    objectContext=ObjectContext(resolve=True),
		)
		with self.assertRaisesRegex(Exception, r"not a valid source IO"):
			Composition(targets={"default"}).visit(common).visit(composition).process()

	def testCompositionThis(self) -> None:
		composition = Object.fromContent(
		    content="""
			namespace default;
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
		    objectContext=ObjectContext(resolve=True),
		)
		Composition(targets={"default"}).visit(composition).process()

	def testCompositionThisMulti(self) -> None:
		composition = Object.fromContent(
		    content="""
			namespace default;
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
		    objectContext=ObjectContext(resolve=True),
		)
		Composition(targets={"default"}).visit(composition).process()

	def testExecutor(self) -> None:
		composition = Object.fromContent(
		    content="""
				namespace default;
				composition {
					hello = Void;
				}
				component Test {
				interface:
					method run();
				}
				composition MyComposition
				{
					a = Test() [executor(default.hello)];
					b = a.run();
				}
				""",
		    objectContext=ObjectContext(resolve=True),
		)
		Composition(targets={"default"}).visit(composition).process()

		with self.assertRaisesRegex(Exception, r"executors between this expression"):
			composition = Object.fromContent(
			    content="""
					namespace default;
					component Test {
					interface:
						method run();
					}
					composition MyComposition
					{
						a = Test();
						b = a.run() [executor(default.hello)];
					}
					""",
			    objectContext=ObjectContext(resolve=True),
			)
			Composition(targets={"default"}).visit(composition).process()


if __name__ == "__main__":
	unittest.main()
