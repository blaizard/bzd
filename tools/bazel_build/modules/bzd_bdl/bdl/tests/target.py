import unittest

from bdl.object import Object, ObjectContext
from bdl.visitors.composition.visitor import Composition


class TestTarget(unittest.TestCase):
	def _build(self, content: str) -> Object:
		return Object.fromContent(content=content, objectContext=ObjectContext(resolve=True))

	def testTargetDefaultPrecompile(self) -> None:
		self._build(
			content="""
namespace default;
extern interface OStream;
component Hello {
config:
	out: OStream = target.out;
}
"""
		)

	def testTargetMethodParameterPrecompile(self) -> None:
		self._build(
			content="""
namespace default;
extern interface OStream;
method run(out: OStream = target.out);
"""
		)

	def testTargetComposition(self) -> None:
		bdl = self._build(
			content="""
namespace default;
extern interface OStream;
component SomeOut : OStream {
}
component Executor {}
component Hello {
config:
	out: OStream = target.out;
interface:
	method run();
}
composition {
	out = SomeOut();
	executor = Executor() [executor];
	hello = Hello();
	hello.run();
}
"""
		)
		composition = Composition()
		composition.visit(bdl).process()
		context = next(iter(composition.contexts))
		entry = composition.registry[context]["default.hello"]
		self.assertIn("default.out", [dep.fqn for dep in entry.deps])

	def testTargetOverride(self) -> None:
		bdl = self._build(
			content="""
namespace default;
extern interface OStream;
component SomeOut : OStream {
}
component OtherOut : OStream {
}
component Executor {}
component Hello {
config:
	out: OStream = target.out;
interface:
	method run();
}
composition {
	out = SomeOut();
	other = OtherOut();
	executor = Executor() [executor];
	hello = Hello(out = other);
	hello.run();
}
"""
		)
		composition = Composition()
		composition.visit(bdl).process()
		context = next(iter(composition.contexts))
		entry = composition.registry[context]["default.hello"]
		self.assertIn("default.other", [dep.fqn for dep in entry.deps])
		self.assertNotIn("default.out", [dep.fqn for dep in entry.deps])

	def testTargetUntypedArgument(self) -> None:
		bdl = self._build(
			content="""
namespace default;
extern interface OStream;
component SomeOut : OStream {
}
component Executor {}
method run(out = OStream);
composition {
	out = SomeOut();
	executor = Executor() [executor];
	run(out = target.out);
}
"""
		)
		composition = Composition()
		composition.visit(bdl).process()

	def testTypedValueComposition(self) -> None:
		bdl = self._build(
			content="""
namespace default;
extern interface OStream;
component SomeOut : OStream {
}
component Executor {}
component Hello {
config:
	out: OStream = other;
interface:
	method run();
}
composition {
	other = SomeOut();
	executor = Executor() [executor];
	hello = Hello();
	hello.run();
}
"""
		)
		composition = Composition()
		composition.visit(bdl).process()
		context = next(iter(composition.contexts))
		entry = composition.registry[context]["default.hello"]
		self.assertIn("default.other", [dep.fqn for dep in entry.deps])


if __name__ == "__main__":
	unittest.main()
