import unittest
import typing

from tools.bdl.object import Object, ObjectContext


class TestRun(unittest.TestCase):

	def testMethod(self) -> None:

		with self.assertRaisesRegex(Exception, r"already defined"):
			Object.fromContent(content="method hello(a = Integer(), a = Interger());")


if __name__ == '__main__':
	unittest.main()
