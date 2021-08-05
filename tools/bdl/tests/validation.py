import unittest
import typing

from tools.bdl.object import Object, ObjectContext


class TestRun(unittest.TestCase):

	def testMethod(self) -> None:

		with self.assertRaisesRegex(Exception, r"already defined"):
			Object.fromContent(content="method hello(a = Integer(), a = Interger());")
		Object.fromContent(content="method hello(a = Integer(), b = Interger());")

	def testContracts(self) -> None:
		with self.assertRaisesRegex(Exception, r"'you'.*not supported"):
			Object.fromContent(content="using MyType = Integer [you];")
		with self.assertRaisesRegex(Exception, r"'hello'.*not supported"):
			Object.fromContent(content="method hello(a = Integer() [hello]);")
		with self.assertRaisesRegex(Exception, r"missing mandatory"):
			Object.fromContent(content="using MyType = Integer [min]; struct A { temp = MyType; }",
				objectContext=ObjectContext(resolve=True))
		Object.fromContent(content="using MyType = Integer [min(2)]; struct A { temp = MyType; }")
		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="using MyType = Integer [min(2,3)]; struct A { temp = MyType; }",
				objectContext=ObjectContext(resolve=True))

		# template only in config context
		with self.assertRaisesRegex(Exception, r"template.*config"):
			Object.fromContent(content="struct temp { var = Integer [template]; }")
		with self.assertRaisesRegex(Exception, r"template.*config"):
			Object.fromContent(content="using MyType = Integer [template];")
		Object.fromContent(content="interface temp { config: var = Integer [template]; }")


if __name__ == '__main__':
	unittest.main()
