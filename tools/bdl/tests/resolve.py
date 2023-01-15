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

		Object.fromContent(content="""
				interface Test { config: value = Integer; }
				using MyType = Test;
				struct temp {
					var = MyType(value=1);
				}
				""",
			objectContext=ObjectContext(resolve=True))

		with self.assertRaisesRegex(Exception, r"expects an integer"):
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
		bdl = Object.fromContent(content="""
				using NewType = Integer;
				struct temp { var = NewType; }
				""",
			objectContext=ObjectContext(resolve=True))
		self.assertTrue(bdl.entity("temp.var").isRValue)

		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="""
				struct First { val = Integer [mandatory]; } 
				struct Temp { var = First; }
				""",
				objectContext=ObjectContext(resolve=True))

	def testNestedInheritance(self) -> None:

		bdl = Object.fromContent(content="""
				struct A { a = Integer; }
				struct B { b = Integer; }
				struct C : A, B {}
				""",
			objectContext=ObjectContext(resolve=True))
		self.assertTrue(bdl.entity("A.a").isRValue)
		self.assertTrue(bdl.entity("B.b").isRValue)

		# Overload
		bdl = Object.fromContent(content="""
				struct A { a = Float; }
				struct B { a = Integer; }
				struct C : A, B { a = Float; }
				""",
			objectContext=ObjectContext(resolve=True))
		self.assertTrue(bdl.entity("A.a").isRValue)
		self.assertTrue(bdl.entity("B.a").isRValue)
		self.assertTrue(bdl.entity("C.a").isRValue)

		with self.assertRaisesRegex(Exception, r"nested class"):
			Object.fromContent(content="""
					struct A { a = Integer; }
					struct C : A, Integer {}
					""",
				objectContext=ObjectContext(resolve=True))

		bdl = Object.fromContent(content="""
				struct A { a = Integer; }
				using B = A;
				struct C : B {}
				""",
			objectContext=ObjectContext(resolve=True))
		self.assertTrue(bdl.entity("A.a").isRValue)

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

		with self.assertRaisesRegex(Exception, r"Unsupported inheritance"):
			Object.fromContent(content="""
					struct A {}
					composition B : A {}
					""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"lower than"):
			Object.fromContent(content="""
				struct Y1 { a = Integer [min(13)]; }
				struct X1 { using Y = Y1; }
				struct A { using X = X1; }
				using B = A;
				struct C : B {}
				composition { a = C.X.Y(a = 12); }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

	def testNamespaces(self) -> None:

		bdl = Object.fromContent(content="""
				namespace bzd.test.nested;
				struct Test { }
				composition MyComposition { test = bzd.test.nested.Test; }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("bzd.test.nested.MyComposition.test").isRValue)

		bdl = Object.fromContent(content="""
				namespace bzd.test.nested;
				struct Test { a = Integer; }
				using Other = Test;
				composition MyComposition { test = bzd.test.nested.Other.a; }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("bzd.test.nested.MyComposition.test").isLValue)

		# Inheritance
		bdl = Object.fromContent(content="""
				namespace bzd.test.nested;
				struct A { a = Integer; }
				struct B { b = Integer; }
				struct C : A, B {}
				composition MyComposition { test = C.a; }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("bzd.test.nested.A.a").isRValue)
		self.assertTrue(bdl.entity("bzd.test.nested.B.b").isRValue)
		self.assertTrue(bdl.entity("bzd.test.nested.MyComposition.test").isLValue)

	def testMethods(self) -> None:

		# Can only refer to methods from an instance.
		bdl = Object.fromContent(content="""
				interface Test { method hello(); }
				composition MyComposition { test = Test; hello = Test.hello(); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.test").isRValue)
		self.assertTrue(bdl.entity("MyComposition.hello").isRValue)

		with self.assertRaisesRegex(Exception, r"not expected"):
			Object.fromContent(content="""
					interface Test { method hello(var = Integer); }
					composition MyComposition { test = Test; hello = test.hello(dad = 3); }
					""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"higher than"):
			Object.fromContent(content="""
					interface Test { method hello(var = Integer [max(2)]); }
					composition MyComposition { test = Test; hello = test.hello(var = 3); }
					""",
				objectContext=ObjectContext(resolve=True, composition=True))

		bdl = Object.fromContent(content="""
				interface Test { method hello(var = Integer [max(10)]); }
				composition MyComposition { test = Test; hello = test.hello(var = 3); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.test").isRValue)
		self.assertTrue(bdl.entity("MyComposition.hello").isRValue)

	def testParameters(self):

		bdl = Object.fromContent(content="""
				interface MyInterface {}
				component MyComponent : MyInterface {}
				method hello(var1 = Integer, var2 = Float, var3 = MyInterface);
				composition { ref = MyComponent(), call = hello(var3 = ref, var1 = 1); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		call = bdl.entity("call")
		self.assertTrue(call.isRValue)
		self.assertEqual(len(call.parameters), 2)
		self.assertTrue(call.parameters[0].isLValue)
		self.assertTrue(call.parameters[1].isRValue)
		self.assertEqual(call.parameters[1].literal, "1")

		self.assertEqual(len(call.parametersResolved), 3)
		self.assertEqual(call.parametersResolved[0].name, "var1")
		self.assertTrue(call.parametersResolved[0].isRValue)
		self.assertEqual(call.parametersResolved[0].param.literal, "1")
		self.assertEqual(call.parametersResolved[0].expected.literal, "0")
		self.assertEqual(call.parametersResolved[1].name, "var2")
		self.assertTrue(call.parametersResolved[1].isRValue)
		self.assertEqual(call.parametersResolved[1].param.literal, "0")
		self.assertEqual(call.parametersResolved[1].expected.literal, "0")
		self.assertEqual(call.parametersResolved[2].name, "var3")
		self.assertTrue(call.parametersResolved[2].isLValue)
		self.assertEqual(str(call.parametersResolved[2].param.symbol), "ref")

	def testStruct(self) -> None:

		bdl = Object.fromContent(content="""
					struct Test { var = Integer; }
					composition { test = Test(); }
					""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("test").isRValue)

		bdl = Object.fromContent(content="""
					struct Test { var = Integer; }
					composition { test = Test(32); }
					""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("test").isRValue)

		bdl = Object.fromContent(content="""
					struct Test { var = Integer; }
					composition { test = Test(var = 32); }
					""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("test").isRValue)

		bdl = Object.fromContent(content="""
					struct Test { var = Integer; }
					composition { test = Test(var = Integer(32)); }
					""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("test").isRValue)

	def testValues(self) -> None:

		bdl = Object.fromContent(content="""
				interface Test { config: value = Integer; }
				composition MyComposition { val1 = Test(value=12); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.val1").isRValue)

		bdl = Object.fromContent(content="""
					interface Test { config: value = Integer; }
					composition MyComposition { val1 = Test(12); }
					""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.val1").isRValue)

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

		bdl = Object.fromContent(content="""
			interface Test { config: value = Integer; }
			composition MyComposition { val1 = Integer(32); val2 = Test(value=val1); }
			""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.val1").isRValue)
		self.assertEqual(bdl.entity("MyComposition.val1").literal, "32")
		self.assertTrue(bdl.entity("MyComposition.val2").isRValue)

		# Variable arguments
		with self.assertRaisesRegex(Exception, r"Variable arguments.*end"):
			Object.fromContent(content="""
					interface Test { config: multi... = Integer; i = Integer; }
					composition MyComposition { val1 = Test(12); }
					""",
				objectContext=ObjectContext(resolve=True, composition=True))

		# Value with no argument
		bdl = Object.fromContent(content="""
				interface Test { config: a = Vector<Integer>(); }
				composition { val1 = Test(); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("val1").isRValue)

		bdl = Object.fromContent(content="""
				using MyInteger3 = Integer;
				interface Test { config: value = MyInteger3(20) [min(10) max(32)]; }
				composition MyComposition { val1 = Test(); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.val1").isRValue)

	def testTemplates(self) -> None:

		# Mandatory template.
		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="""
				interface Test { config: using Type = Integer [mandatory]; }
				composition MyComposition { val1 = Test; }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"type"):
			Object.fromContent(content="""
				interface Test { config: using Value = Integer; }
				composition MyComposition { val1 = Test<12>; }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		bdl = Object.fromContent(content="""
			interface Test { config: using Type = Void; }
			composition MyComposition { val1 = Test<Void>; }
			""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.val1").isRValue)

		with self.assertRaisesRegex(Exception, r"lower than"):
			Object.fromContent(content="""
			interface Test { config: value = Integer [min(10)]; }
			composition MyComposition { val1 = Test(2); }
			""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"higher than"):
			Object.fromContent(content="""
			interface Test { config: value = Integer [max(10)]; }
			composition MyComposition { val1 = Test(20); }
			""",
				objectContext=ObjectContext(resolve=True, composition=True))

		bdl = Object.fromContent(content="""
			interface Test { config: value = Integer(15) [min(10) max(32)]; }
			composition MyComposition { val1 = Test(23); }
			""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.val1").isRValue)

		with self.assertRaisesRegex(Exception, r"mandatory"):
			Object.fromContent(content="""
				interface Test { config: value1 = Integer [min(10) max(32)]; valuw2 = [mandatory]; }
				composition MyComposition { val1 = Test(23); }
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

		bdl = Object.fromContent(content="""
				using MyInteger1 = Integer;
				using MyInteger2 = MyInteger1;
				using MyInteger3 = MyInteger2;
				interface Test { config: value = MyInteger3(20) [min(10) max(32)]; }
				composition MyComposition { val1 = Test(); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.val1").isRValue)

		bdl = Object.fromContent(content="""
				interface Test { config: val1 = Integer(23); val2 = Integer;}
				composition MyComposition { value = Test(val2 = 3); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.value").isRValue)

		bdl = Object.fromContent(content="""
				interface Test { config: value = Integer; }
				composition MyComposition { val1 = Test(value=Integer(12)); }
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.val1").isRValue)

		with self.assertRaisesRegex(Exception, r"cannot have a 'meta' role"):
			Object.fromContent(content="""
					composition MyComposition { val1 = Array<Any>(1); }
					""",
				objectContext=ObjectContext(resolve=True, composition=True))

	def testDefaultValues(self) -> None:

		with self.assertRaisesRegex(Exception, r"Missing.*mandatory"):
			Object.fromContent(content="""
				interface Test { config: value = Integer [mandatory]; }
				composition MyComposition {
					val1 = Test;
				}
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		with self.assertRaisesRegex(Exception, r"lower.*minimum"):
			Object.fromContent(content="""
				interface Test { config: value = Integer(1) [min(10) max(32)]; }
				composition MyComposition { val1 = Test; }
				""",
				objectContext=ObjectContext(resolve=True, composition=True))

		bdl = Object.fromContent(content="""
				interface Test { config: value = Vector<Integer>; }
				composition MyComposition {
					value1 = Integer(1);
					value2 = 2;
					value3 = value2;
					vector = Test(value = Vector<Integer>(value1, value2, 3, Integer(4)));
				}
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.value1").isRValue)
		self.assertEqual(bdl.entity("MyComposition.value1").literal, "1")
		self.assertTrue(bdl.entity("MyComposition.value2").isRValue)
		self.assertEqual(bdl.entity("MyComposition.value2").literal, "2")
		self.assertTrue(bdl.entity("MyComposition.value3").isLValue)
		self.assertTrue(bdl.entity("MyComposition.vector").isRValue)

	def testInstanceWithParameters(self) -> None:
		Object.fromContent(content="""
				interface Test { config: value = Integer; }
				composition MyComposition
				{
					val1 = Test(value = 23);
					hello = val1;
				}
				""",
			objectContext=ObjectContext(resolve=True, composition=True))

		bdl = Object.fromContent(content="""
				interface Test { config: using hello = Integer; }
				composition MyComposition
				{
					val1 = Test<Float>();
				}
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.val1").isRValue)

		bdl = Object.fromContent(content="""
				interface Test { config: value = Integer; }
				component MyComponent : Test { config: hello = Integer; }
				composition MyComposition
				{
					val1 = MyComponent(hello = 2);
				}
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertTrue(bdl.entity("MyComposition.val1").isRValue)

	def testExecutor(self) -> None:
		bdl = Object.fromContent(content="""
				component Test {
				interface:
					method run();
				}
				composition MyComposition
				{
					a = Test() [executor(hello)];
					b = a.run();
				}
				""",
			objectContext=ObjectContext(resolve=True, composition=True))
		self.assertEqual(bdl.entity("MyComposition.a").executor, "hello")
		self.assertEqual(bdl.entity("MyComposition.b").executor, "hello")

		with self.assertRaisesRegex(Exception, r"component instantiation"):
			Object.fromContent(content="""
					component Test {
					interface:
						method run();
					}
					composition MyComposition
					{
						a = Test();
						b = a.run() [executor(hello)];
					}
					""",
				objectContext=ObjectContext(resolve=True, composition=True))

	def testComponent(self) -> None:
		with self.assertRaisesRegex(Exception, r"Only.*struct.*interface"):
			Object.fromContent(content="""
					component A {}
					component B {
					interface:
						a = A();
					}
					""",
				objectContext=ObjectContext(resolve=True))


if __name__ == '__main__':
	unittest.main(failfast=True)
