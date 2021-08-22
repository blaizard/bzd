import unittest
import typing

from tools.bdl.object import Object, ObjectContext


class TestRun(unittest.TestCase):

	def testContracts(self) -> None:

		with self.assertRaisesRegex(Exception, r"lower.*minimum"):
			bdl = Object.fromContent(content="""
				interface Test { config: value = Integer [min(10) max(32)]; }
				composition MyComposition { val1 = Integer(1); val2 = Test(value = val1); }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))
			print(bdl)


if __name__ == '__main__':
	unittest.main()
