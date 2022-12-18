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

		# Capacity
		with self.assertRaisesRegex(Exception, r"missing mandatory"):
			Object.fromContent(content="using MyType = Integer [capacity];", objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="using MyType = Integer [capacity(1,2)];",
				objectContext=ObjectContext(resolve=True))
		with self.assertRaisesRegex(Exception, r".*lower.*"):
			Object.fromContent(content="using MyType = Integer [capacity(0)];",
				objectContext=ObjectContext(resolve=True))
		Object.fromContent(content="using MyType = Integer [capacity(10)];", objectContext=ObjectContext(resolve=True))

		# Init
		Object.fromContent(content="interface temp { method hello() [init]; }",
			objectContext=ObjectContext(resolve=True))
		#Object.fromContent(content="interface temp { method hello() [init(my_func)]; }",
		#	objectContext=ObjectContext(resolve=True))


if __name__ == '__main__':
	unittest.main()
