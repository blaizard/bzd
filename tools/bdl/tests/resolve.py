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

		with self.assertRaisesRegex(Exception, r"associated.*configuration"):
			Object.fromContent(content="""
				interface Test { config: value = Integer; }
				using MyType = Test [integer];
				struct temp {
					var = MyType(value=1);
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

	def testNestedInheritance(self) -> None:

		Object.fromContent(content="""
				struct A { a = Integer; }
				struct B { b = Integer; }
				struct C : A, B {}
				""",
			objectContext=ObjectContext(resolve=True))

		# Overload
		Object.fromContent(content="""
				struct A { a = Float; }
				struct B { a = Integer; }
				struct C : A, B { a = Float; }
				""",
			objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"nested class"):
			Object.fromContent(content="""
					struct A { a = Integer; }
					struct C : A, Integer {}
					""",
				objectContext=ObjectContext(resolve=True))

		Object.fromContent(content="""
				struct A { a = Integer; }
				using B = A;
				struct C : B {}
				""",
			objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"nested class"):
			Object.fromContent(content="""
					using B = Float;
					struct C : B {}
					""",
				objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"struct.*inherits.*struct"):
			Object.fromContent(content="""
					interface A {}
					struct B : A {}
					""",
				objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"interface.*inherits.*interface"):
			Object.fromContent(content="""
					struct A {}
					interface B : A {}
					""",
				objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"component.*inherits.*interface"):
			Object.fromContent(content="""
					struct A {}
					component B : A {}
					""",
				objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"Composition.*cannot.*inheritance"):
			Object.fromContent(content="""
					struct A {}
					composition B : A {}
					""",
				objectContext=ObjectContext(resolve=True))

	def testNamespaces(self) -> None:

		Object.fromContent(content="""
				namespace bzd.test.nested;
				struct Test { }
				composition MyComposition { test = bzd.test.nested.Test; }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))

		Object.fromContent(content="""
				namespace bzd.test.nested;
				struct Test { a = Integer; }
				using Other = Test;
				composition MyComposition { test = bzd.test.nested.Other.a; }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))

		# Inheritance
		Object.fromContent(content="""
				namespace bzd.test.nested;
				struct A { a = Integer; }
				struct B { b = Integer; }
				struct C : A, B {}
				composition MyComposition { test = C.a; }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))

	def testMethods(self) -> None:

		# TODO: enable this
		# Can only refer to methods from an instance.
		Object.fromContent(content="""
				interface Test { method hello(); }
				composition MyComposition { test = Test; hello = Test.hello(); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))

		Object.fromContent(content="""
				interface Test { method hello(); }
				composition MyComposition { test = Test; hello = test.hello(); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))

	def testValues(self) -> None:

		Object.fromContent(content="""
				interface Test { config: value = Integer; }
				composition MyComposition { val1 = Test(value=12); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"named parameters"):
			Object.fromContent(content="""
					interface Test { config: value = Integer; }
					composition MyComposition { val1 = Test(12); }
					""",
				objectContext=ObjectContext(resolve=True, composition=True))

		# Mispelled value key
		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="""
				interface Test { config: value = Integer; }
				composition MyComposition { val1 = Test(vaue=1); }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		# Multiple arguments
		with self.assertRaisesRegex(Exception, r"integer"):
			Object.fromContent(content="""
				interface Test { config: value = Integer; }
				composition MyComposition { val1 = Test(value="dsdsd"); }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"Symbol.*resolved"):
			Object.fromContent(content="""
				interface Test { config: value = Callable; }
				composition MyComposition { val2 = Test(value=val1); }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"instantiate.*value"):
			Object.fromContent(content="""
				composition MyComposition { val1 = Integer(12); val2 = val1(12); }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		Object.fromContent(content="""
			interface Test { config: value = Callable; }
			composition MyComposition { val1 = Callable; val2 = Test(value=val1); }
			""",
			objectContext=ObjectContext(resolve=True, composition=True))

		# Variable arguments
		with self.assertRaisesRegex(Exception, r"Variable arguments.*end"):
			Object.fromContent(content="""
					interface Test { config: ... = Integer; i = Integer; }
					composition MyComposition { val1 = Test(12); }
					""",
				objectContext=ObjectContext(resolve=True, composition=True))

	def testTemplates(self) -> None:

		# Mandatory template.
		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="""
				interface Test { config: Integer [template mandatory]; }
				composition MyComposition { val1 = Test; }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"named parameter"):
			Object.fromContent(content="""
				interface Test { config: value = Integer [template]; }
				composition MyComposition { val1 = Test<12>; }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"expects.*integer"):
			bdl = Object.fromContent(content="""
			interface Test { config: Integer [template]; }
			composition MyComposition { val1 = Test<Void>; }
			""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"lower than"):
			Object.fromContent(content="""
			interface Test { config: Integer [template min(10)]; }
			composition MyComposition { val1 = Test<2>; }
			""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"higher than"):
			Object.fromContent(content="""
			interface Test { config: Integer [template max(10)]; }
			composition MyComposition { val1 = Test<20>; }
			""",
				objectContext=ObjectContext(resolve=True, composition=True))

		Object.fromContent(content="""
			interface Test { config: Integer [template min(10) max(32)]; }
			composition MyComposition { val1 = Test<23>; }
			""",
			objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="""
				interface Test { config: Integer [template min(10) max(32)]; Integer [template mandatory]; }
				composition MyComposition { val1 = Test<23>; }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"lower.*minimum"):
			Object.fromContent(content="""
				interface Test { config: value = Integer [min(10) max(32)]; }
				composition MyComposition { val1 = Integer(1); val2 = Test(value = val1); }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"lower.*minimum"):
			Object.fromContent(content="""
				using MyInteger = Integer;
				interface Test { config: value = MyInteger [min(10) max(32)]; }
				composition MyComposition { val1 = MyInteger(1); val2 = Test(value = val1); }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

	def testDefaultValues(self) -> None:

		with self.assertRaisesRegex(Exception, r"missing.*mandatory"):
			Object.fromContent(content="""
				interface Test { config: value = Integer [mandatory]; }
				composition MyComposition { val1 = Test; }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"lower.*minimum"):
			Object.fromContent(content="""
				interface Test { config: value = Integer(1) [min(10) max(32)]; }
				composition MyComposition { val1 = Test; }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))


if __name__ == '__main__':
	unittest.main()
