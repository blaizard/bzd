import unittest
import typing

from tools.bdl.object import Object, ObjectContext


class TestRun(unittest.TestCase):

	def testContracts(self) -> None:

		with self.assertRaisesRegex(Exception, r"lower than"):
			Object.fromContent(content="""
			using NewType = Integer [min(1) max(4)];
			struct temp {
				var = NewType(0);
			}
			""",
				objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"lower than"):
			Object.fromContent(content="""
				using NewType = Integer [min(1) max(4)];
				using InterType = NewType [min(2)];
				using InterfaceType = InterType;
				struct temp {
					var = InterfaceType(1);
				}
				""",
				objectContext=ObjectContext(resolve=True))

		# To be updated, this should fail.
		#with self.assertRaisesRegex(Exception, r"expects.*integer"):
		Object.fromContent(content="""
			interface Test { config: value = Integer; }
			using MyType = Test [integer];
			struct temp {
				var = MyType(1);
			}
			""",
			objectContext=ObjectContext(resolve=True))

	def testMandatory(self) -> None:

		# Not mandatory
		Object.fromContent(content="""
				using NewType = Integer;
				struct temp { var = NewType; }
				""",
			objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="""
				using NewType = Integer [mandatory];
				struct temp { var = NewType; }
				""",
				objectContext=ObjectContext(resolve=True))

		# Mandatory in that case is not applied to this value.
		Object.fromContent(content="""
			using NewType = Integer;
			struct temp { var = NewType [mandatory]; }
			""",
			objectContext=ObjectContext(resolve=True))

	def testTemplates(self) -> None:

		# Non mandatory template
		Object.fromContent(content="""
			interface Test { config: value = Integer [template]; }
			composition MyComposition { val1 = Test; }
			""",
			objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="""
				interface Test { config: value = Integer [template mandatory]; }
				composition MyComposition { val1 = Test; }
				""",
				objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"expects.*integer"):
			Object.fromContent(content="""
			interface Test { config: value = Integer [template]; }
			composition MyComposition { val1 = Test<Void>; }
			""",
				objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"lower than"):
			Object.fromContent(content="""
			interface Test { config: value = Integer [template min(10)]; }
			composition MyComposition { val1 = Test<2>; }
			""",
				objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"higher than"):
			Object.fromContent(content="""
			interface Test { config: value = Integer [template max(10)]; }
			composition MyComposition { val1 = Test<20>; }
			""",
				objectContext=ObjectContext(resolve=True))

		Object.fromContent(content="""
			interface Test { config: value = Integer [template min(10) max(32)]; }
			composition MyComposition { val1 = Test<23>; }
			""",
			objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="""
				interface Test { config: a = Integer [template min(10) max(32)]; b = Integer [template mandatory]; }
				composition MyComposition { val1 = Test<23>; }
				""",
				objectContext=ObjectContext(resolve=True))


if __name__ == '__main__':
	unittest.main()
