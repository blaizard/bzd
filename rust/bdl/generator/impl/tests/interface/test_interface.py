import unittest

from bdl.object import Object, ObjectContext

from rust.bdl.generator.impl.visitor import formatRust


class TestInterface(unittest.TestCase):
	def testSimpleInterface(self) -> None:

		bdlContent = """interface MyInterface {
	method add(a = const Integer, b = const Integer) -> Integer;
}
"""
		expectedContent = """pub trait MyInterface {
    fn add(&self, a: &i32, b: &i32) -> i32;
}
"""

		bdl = Object.fromContent(content=bdlContent, objectContext=ObjectContext(resolve=True))
		actual = formatRust(bdl)

		self.assertEqual(
			actual.rstrip("\n"),
			expectedContent.rstrip("\n"),
		)

	def testBuiltins(self) -> None:

		testCases = {
			"Float": (
				"""interface MyInterface {
	method f(a = const Float) -> Float;
}
""",
				"""pub trait MyInterface {
    fn f(&self, a: &f32) -> f32;
}
""",
			),
			"Boolean": (
				"""interface MyInterface {
	method f(a = const Boolean) -> Boolean;
}
""",
				"""pub trait MyInterface {
    fn f(&self, a: &bool) -> bool;
}
""",
			),
			"Byte": (
				"""interface MyInterface {
	method f(a = const Byte) -> Byte;
}
""",
				"""pub trait MyInterface {
    fn f(&self, a: &u8) -> u8;
}
""",
			),
			"String": (
				"""interface MyInterface {
	method f(a = const String) -> String;
}
""",
				"""pub trait MyInterface {
    fn f(&self, a: &'static str) -> &'static str;
}
""",
			),
			"Void": (
				"""interface MyInterface {
	method f(a = Boolean) -> Void;
}
""",
				"""pub trait MyInterface {
    fn f(&self, a: bool) -> ();
}
""",
			),
			"None": (
				"""interface MyInterface {
	method f(a = Boolean) -> None;
}
""",
				"""pub trait MyInterface {
    fn f(&self, a: bool) -> ();
}
""",
			),
			"Result<Void>": (
				"""interface MyInterface {
	method f() -> Result<Void>;
}
""",
				"""pub trait MyInterface {
    fn f(&self) -> Result<(), bzd::base::error::Error>;
}
""",
			),
			"Result<Integer>": (
				"""interface MyInterface {
	method f(a = Boolean) -> Result<Integer>;
}
""",
				"""pub trait MyInterface {
    fn f(&self, a: bool) -> Result<i32, bzd::base::error::Error>;
}
""",
			),
			"Array default capacity": (
				"""interface MyInterface {
	method f(a = const Integer) -> Array<Integer>;
}
""",
				"""pub trait MyInterface {
    fn f(&self, a: &i32) -> &[i32; 1];
}
""",
			),
			"Array with capacity": (
				"""interface MyInterface {
	method f() -> Array<Integer> [capacity(4)];
}
""",
				"""pub trait MyInterface {
    fn f(&self) -> &[i32; 4];
}
""",
			),
			"Vector": (
				"""interface MyInterface {
	method f(a = const Integer) -> Vector<Integer>;
}
""",
				"""pub trait MyInterface {
    fn f(&self, a: &i32) -> &[i32];
}
""",
			),
		}

		for name, (bdlContent, expectedContent) in testCases.items():
			with self.subTest(name=name):
				bdl = Object.fromContent(content=bdlContent, objectContext=ObjectContext(resolve=True))
				actual = formatRust(bdl)
				self.assertEqual(
					actual.rstrip("\n"),
					expectedContent.rstrip("\n"),
				)


if __name__ == "__main__":
	unittest.main()
