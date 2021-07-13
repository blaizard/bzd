import unittest
import typing

from tools.bdl.object import Object, ObjectContext


class TestRun(unittest.TestCase):

	def testContracts(self) -> None:

		# This should fail
		Object.fromContent(content="""
		using NewType = Integer [min = 1, max = 4];
		using InterType = NewType [min = 2];
		using InterfaceType = NewType;
		struct temp {
			var = InterfaceType(1);
		}
		""",
			objectContext=ObjectContext(resolve=True))


if __name__ == '__main__':
	unittest.main()
