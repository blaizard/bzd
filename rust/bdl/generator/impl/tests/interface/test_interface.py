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
    fn add(&self, a: i32, b: i32) -> i32;
}
"""

		bdl = Object.fromContent(content=bdlContent, objectContext=ObjectContext(resolve=True))
		actual = formatRust(bdl)

		self.assertEqual(
			actual.rstrip("\n"),
			expectedContent.rstrip("\n"),
		)


if __name__ == "__main__":
	unittest.main()
