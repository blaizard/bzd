import unittest
import typing

from tools.bdl.object import Object, ObjectContext


class TestRun(unittest.TestCase):

	def testMethod(self) -> None:

		with self.assertRaisesRegex(Exception, r"already defined"):
			Object.fromContent(content="method hello(a = Integer(), a = Interger());")

	def testContracts(self) -> None:
		with self.assertRaisesRegex(Exception, r"'you'.*not supported"):
			Object.fromContent(content="using MyType = Integer [you];")
		with self.assertRaisesRegex(Exception, r"'hello'.*not supported"):
			Object.fromContent(content="method hello(a = Integer() [hello]);")
		with self.assertRaisesRegex(Exception, r"missing mandatory"):
			Object.fromContent(content="using MyType = Integer [min];")
		Object.fromContent(content="using MyType = Integer [min(2)];")
		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="using MyType = Integer [min(2, 32)];")


if __name__ == '__main__':
	unittest.main()
