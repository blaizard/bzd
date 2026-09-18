import unittest

from bdl.object import Object, ObjectContext

from rust.bdl.generator.impl.visitor import formatRust


class TestInterface(unittest.TestCase):
	def testSimpleInterface(self) -> None:

		bdlContent = """interface MyService {
	method add(a = const Integer, b = const Integer) -> Integer;
}
"""
		expectedContent = """#[allow(async_fn_in_trait)]
pub trait MyServiceInterface {
	async fn add(&mut self, a: i32, b: i32) -> Result<i32, bzd::base::error::Error>;
}
"""

		bdl = Object.fromContent(content=bdlContent, objectContext=ObjectContext(resolve=True))
		actual = formatRust(bdl)

		self.assertEqual(
			actual.rstrip("\n"),
			expectedContent.rstrip("\n"),
		)

	def testNamespace(self) -> None:

		bdlContent = """namespace bzd.test;

interface MyService {
	method add(a = const Integer, b = const Integer) -> Integer;
}
"""
		expectedContent = """#[allow(async_fn_in_trait)]
pub trait BzdTestMyServiceInterface {
	async fn add(&mut self, a: i32, b: i32) -> Result<i32, bzd::base::error::Error>;
}
"""

		bdl = Object.fromContent(content=bdlContent, objectContext=ObjectContext(resolve=True))
		actual = formatRust(bdl)

		self.assertEqual(
			actual.rstrip("\n"),
			expectedContent.rstrip("\n"),
		)

	def testComponent(self) -> None:

		bdlContent = """component MyComponent {
config:
	age = Integer;
interface:
	method foo(a = const Integer) -> Integer;
}
"""
		expectedContent = """#[allow(async_fn_in_trait)]
pub trait MyComponentInterface {
	async fn foo(&mut self, a: i32) -> Result<i32, bzd::base::error::Error>;
}

pub struct MyComponentContext {
	pub age: i32,
}
"""

		bdl = Object.fromContent(content=bdlContent, objectContext=ObjectContext(resolve=True))
		actual = formatRust(bdl)

		self.assertEqual(
			actual.rstrip("\n"),
			expectedContent.rstrip("\n"),
		)

	def testComponentWithParents(self) -> None:

		bdlContent = """component Base {
config:
	age = Integer;
interface:
	method foo(a = const Integer) -> Integer;
}

component Derived : Base {
config:
	username = String;
interface:
	method bar() -> Void;
}
"""
		expectedContent = """#[allow(async_fn_in_trait)]
pub trait BaseInterface {
	async fn foo(&mut self, a: i32) -> Result<i32, bzd::base::error::Error>;
}

pub struct BaseContext {
	pub age: i32,
}

#[allow(async_fn_in_trait)]
pub trait DerivedInterface: BaseInterface {
	async fn bar(&mut self) -> Result<(), bzd::base::error::Error>;
}

pub struct DerivedContext {
	pub age: i32,
	pub username: &'static str,
}
"""

		bdl = Object.fromContent(content=bdlContent, objectContext=ObjectContext(resolve=True))
		actual = formatRust(bdl)

		self.assertEqual(
			actual.rstrip("\n"),
			expectedContent.rstrip("\n"),
		)

	def testEnum(self) -> None:

		bdlContent = """namespace bzd.components.esp;

enum UartDevice
{
	uart0
,	uart1
}
"""
		expectedContent = """#[repr(u8)]
#[derive(Copy, Clone, PartialEq, Eq, Debug)]
pub enum BzdComponentsEspUartDevice {
    Uart0,
    Uart1,
}
"""

		bdl = Object.fromContent(content=bdlContent, objectContext=ObjectContext(resolve=True))
		actual = formatRust(bdl)

		self.assertEqual(
			actual.rstrip("\n"),
			expectedContent.rstrip("\n"),
		)

	def testEnumNoNamespace(self) -> None:

		bdlContent = """enum LightState {
	on
,	off
}
"""
		expectedContent = """#[repr(u8)]
#[derive(Copy, Clone, PartialEq, Eq, Debug)]
pub enum LightState {
    On,
    Off,
}
"""

		bdl = Object.fromContent(content=bdlContent, objectContext=ObjectContext(resolve=True))
		actual = formatRust(bdl)

		self.assertEqual(
			actual.rstrip("\n"),
			expectedContent.rstrip("\n"),
		)

	def testBuiltins(self) -> None:

		testCases = {
			"Float": (
				"""interface MyService {
	method f(a = const Float) -> Float;
}
""",
				"""#[allow(async_fn_in_trait)]
pub trait MyServiceInterface {
	async fn f(&mut self, a: f32) -> Result<f32, bzd::base::error::Error>;
}
""",
			),
			"Boolean": (
				"""interface MyService {
	method f(a = const Boolean) -> Boolean;
}
""",
				"""#[allow(async_fn_in_trait)]
pub trait MyServiceInterface {
	async fn f(&mut self, a: bool) -> Result<bool, bzd::base::error::Error>;
}
""",
			),
			"Byte": (
				"""interface MyService {
	method f(a = const Byte) -> Byte;
}
""",
				"""#[allow(async_fn_in_trait)]
pub trait MyServiceInterface {
	async fn f(&mut self, a: u8) -> Result<u8, bzd::base::error::Error>;
}
""",
			),
			"String": (
				"""interface MyService {
	method f(a = const String) -> String;
}
""",
				"""#[allow(async_fn_in_trait)]
pub trait MyServiceInterface {
	async fn f(&mut self, a: &'static str) -> Result<&'static str, bzd::base::error::Error>;
}
""",
			),
			"Void": (
				"""interface MyService {
	method f(a = Boolean) -> Void;
}
""",
				"""#[allow(async_fn_in_trait)]
pub trait MyServiceInterface {
	async fn f(&mut self, a: bool) -> Result<(), bzd::base::error::Error>;
}
""",
			),
			"None": (
				"""interface MyService {
	method f(a = Boolean) -> None;
}
""",
				"""#[allow(async_fn_in_trait)]
pub trait MyServiceInterface {
	async fn f(&mut self, a: bool) -> Result<(), bzd::base::error::Error>;
}
""",
			),
			"Array default capacity": (
				"""interface MyService {
	method f(a = const Integer) -> Array<Integer>;
}
""",
				"""#[allow(async_fn_in_trait)]
pub trait MyServiceInterface {
	async fn f(&mut self, a: i32) -> Result<&[i32; 1], bzd::base::error::Error>;
}
""",
			),
			"Array with capacity": (
				"""interface MyService {
	method f() -> Array<Integer> [capacity(4)];
}
""",
				"""#[allow(async_fn_in_trait)]
pub trait MyServiceInterface {
	async fn f(&mut self) -> Result<&[i32; 4], bzd::base::error::Error>;
}
""",
			),
			"Vector": (
				"""interface MyService {
	method f(a = const Integer) -> Vector<Integer>;
}
""",
				"""#[allow(async_fn_in_trait)]
pub trait MyServiceInterface {
	async fn f(&mut self, a: i32) -> Result<&[i32], bzd::base::error::Error>;
}
""",
			),
		}

		for name, (bdlContent, expectedContent) in testCases.items():
			with self.subTest(name=name):
				bdl = Object.fromContent(content=bdlContent, objectContext=ObjectContext(resolve=True))
				actual = formatRust(bdl)
				self.assertEqual(
					actual.rstrip("\n"),
					expectedContent.rstrip("\n"),
				)


if __name__ == "__main__":
	unittest.main()
