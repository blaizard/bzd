import unittest

from bdl.object import Object, ObjectContext
from bdl.visitors.composition.visitor import Composition

from rust.bdl.generator.impl.visitor import compositionRust


class TestComposition(unittest.TestCase):
	def _generate(self, content: str, imports: list[str], target: str = "default") -> str:
		bdl = Object.fromContent(content=content, objectContext=ObjectContext(resolve=True))
		composition = Composition()
		composition.visit(bdl).process()
		view = composition.view(target=target)
		return compositionRust(composition=view, data={"rust": {"imports": imports}})

	def testCompositionWithoutOut(self) -> None:
		content = """
namespace default;

component Person {
config:
	username = String;
	age = Integer [min(0)];
interface:
	method print_info();
}

component Child : Person {
}

component Parent : Person {
config:
	children = list(Child);
}

component Executor {
}

composition {
	executor = Executor() [executor];
	person_child = Child(age = 28, username = "Alex");
	person_parent = Parent(username = "Bob", age = 42, children = list(person_child));
	person_child.print_info();
	person_parent.print_info();
}
"""
		actual = self._generate(content=content, imports=["child", "parent"])
		self.assertIn(
			'let mut logger = logger_backend.lock().await.logger("default.executor");',
			actual,
		)
		self.assertNotIn("install", actual)

	def testCompositionInlineDefaultComponent(self) -> None:
		content = """
namespace default;
interface Iface { }
component Error : Iface { }
component User {
config:
	dep: Iface = Error();
interface:
	method run();
}
component Executor { }
composition {
	executor = Executor() [executor];
	user = User();
	user.run();
}
"""
		actual = self._generate(content=content, imports=["user", "error"])
		self.assertIn("struct DefaultUserBdlDefaultDepEntry", actual)
		self.assertIn("component: CompositionComponent<DefaultError>", actual)
		self.assertIn("DefaultUserContextConstraintTypes<DefaultError>", actual)
		self.assertIn("&DefaultUserBdlDefaultDepEntry::get().component.handle()", actual)

	def testCompositionInlineDefaultComponentOverride(self) -> None:
		content = """
namespace default;
interface Iface { }
component Error : Iface { }
component Other : Iface { }
component User {
config:
	dep: Iface = Error();
interface:
	method run();
}
component Executor { }
composition {
	executor = Executor() [executor];
	other = Other();
	user = User(dep = other);
	user.run();
}
"""
		actual = self._generate(content=content, imports=["user", "error", "other"])
		self.assertNotIn("BdlDefaultDepEntry", actual)
		self.assertIn("&DefaultOtherEntry::get().component.handle()", actual)

	def testCompositionLogger(self) -> None:
		content = """
namespace bzd.logger;
enum Level {
	Debug,
	Info
}
struct LoggerConfig {
	level = Level(Level.Info);
	topic = String("");
}

namespace default;
component Hello {
config:
	logger = bzd.logger.LoggerConfig();
interface:
	method run();
}
component Executor {
}

composition {
	executor = Executor() [executor];
	hello = Hello();
	hello.run();
}
"""
		actual = self._generate(content=content, imports=[], target="bzd")
		self.assertIn(
			'logger: logger::logger_state().logger("bzd.logger.default.hello", BzdLoggerLevel::Info)',
			actual,
		)
		self.assertNotIn("LoggerBackendState", actual)
		self.assertNotIn("install", actual)

		content = content.replace(
			"hello = Hello();",
			"hello = Hello(logger = bzd.logger.LoggerConfig(level = bzd.logger.Level.Debug));",
		)
		actual = self._generate(content=content, imports=[], target="bzd")
		self.assertIn(
			'logger: logger::logger_state().logger("bzd.logger.default.hello", BzdLoggerLevel::Debug)',
			actual,
		)

		content = content.replace(
			"hello = Hello(logger = bzd.logger.LoggerConfig(level = bzd.logger.Level.Debug));",
			'hello = Hello(logger = bzd.logger.LoggerConfig(topic = "custom.topic"));',
		)
		actual = self._generate(content=content, imports=[], target="bzd")
		self.assertIn(
			'logger: logger::logger_state().logger("custom.topic", BzdLoggerLevel::Info)',
			actual,
		)

	def testCompositionEnum(self) -> None:
		content = """
namespace default;

enum Device {
	uart0
}

component Out {
config:
	device = Device;
}

component Printer {
config:
	out = Out;
interface:
	method run();
}

component Executor {
}

composition {
	executor = Executor() [executor];
	out = Out(device = Device.uart0);
	printer = Printer(out = out);
	printer.run();
}
"""
		actual = self._generate(content=content, imports=[])
		self.assertIn("device: DefaultDevice::Uart0", actual)

	def testCompositionWithOut(self) -> None:
		content = """
namespace default;

component Person {
config:
	username = String;
	age = Integer [min(0)];
interface:
	method print_info();
}

component Child : Person {
}

component Parent : Person {
config:
	children = list(Child);
}

component Out {
}

component Printer {
config:
	out = Out;
interface:
	method run();
}

component Executor {
}

composition {
	executor = Executor() [executor];
	out = Out();
	printer = Printer(out = out);
	printer.run();
	person_child = Child(age = 28, username = "Alex");
	person_parent = Parent(username = "Bob", age = 42, children = list(person_child));
	person_child.print_info();
	person_parent.print_info();
}
"""
		expectedContent = """#![no_std]

#[allow(unused_imports)]
use component::{CompositionComponent, Lifecycle, StaticComponent};

#[allow(unused_imports)]
use logger::{log_error, log_info};

#[allow(unused_imports)]
use rust_interfaces_logger::BzdLoggerLevel;

#[allow(unused_imports)]
use rust_interfaces_executor::BzdExecutorInterface;
#[allow(unused_imports)]
use child::*;
#[allow(unused_imports)]
use parent::*;

// ---- Registry
struct DefaultExecutorEntry {
	component: CompositionComponent<DefaultExecutor>,
}

impl DefaultExecutorEntry {
	fn get() -> &'static Self {
		static COMPONENT: StaticComponent<DefaultExecutorEntry> = StaticComponent::new();
		COMPONENT.get_or_init(|| Self {
			component: CompositionComponent::new(DefaultExecutor::new())
		})
	}
}

#[allow(unused_variables)]
impl Lifecycle for DefaultExecutorEntry {
	async fn init(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.acquire(async |component| {
			let mut lifecycle_logger = logger::logger_state().logger("default.executor", BzdLoggerLevel::Info);
			log_info!(lifecycle_logger, "Initializing default.Executor")?;
			
			Ok(())
		}).await?;
		Ok(())
	}

	async fn shutdown(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.release(async |component| {
			let mut lifecycle_logger = logger::logger_state().logger("default.executor", BzdLoggerLevel::Info);
			log_info!(lifecycle_logger, "Shutting down default.Executor")?;
			
			Ok(())
		}).await?;
		Ok(())
	}
}

struct DefaultOutEntry {
	component: CompositionComponent<DefaultOut>,
}

impl DefaultOutEntry {
	fn get() -> &'static Self {
		static COMPONENT: StaticComponent<DefaultOutEntry> = StaticComponent::new();
		COMPONENT.get_or_init(|| Self {
			component: CompositionComponent::new(DefaultOut::new())
		})
	}
}

#[allow(unused_variables)]
impl Lifecycle for DefaultOutEntry {
	async fn init(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.acquire(async |component| {
			let mut lifecycle_logger = logger::logger_state().logger("default.out", BzdLoggerLevel::Info);
			log_info!(lifecycle_logger, "Initializing default.Out")?;
			
			Ok(())
		}).await?;
		Ok(())
	}

	async fn shutdown(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.release(async |component| {
			let mut lifecycle_logger = logger::logger_state().logger("default.out", BzdLoggerLevel::Info);
			log_info!(lifecycle_logger, "Shutting down default.Out")?;
			
			Ok(())
		}).await?;
		Ok(())
	}
}

struct DefaultPersonChildEntry {
	component: CompositionComponent<DefaultChild>,
}

impl DefaultPersonChildEntry {
	fn get() -> &'static Self {
		static COMPONENT: StaticComponent<DefaultPersonChildEntry> = StaticComponent::new();
		COMPONENT.get_or_init(|| Self {
			component: CompositionComponent::new(DefaultChild::new(DefaultChildContext { username: "Alex", age: 28 }))
		})
	}
}

#[allow(unused_variables)]
impl Lifecycle for DefaultPersonChildEntry {
	async fn init(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.acquire(async |component| {
			let mut lifecycle_logger = logger::logger_state().logger("default.person_child", BzdLoggerLevel::Info);
			log_info!(lifecycle_logger, "Initializing default.Child")?;
			
			Ok(())
		}).await?;
		Ok(())
	}

	async fn shutdown(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.release(async |component| {
			let mut lifecycle_logger = logger::logger_state().logger("default.person_child", BzdLoggerLevel::Info);
			log_info!(lifecycle_logger, "Shutting down default.Child")?;
			
			Ok(())
		}).await?;
		Ok(())
	}
}

struct DefaultPersonParentEntry {
	component: CompositionComponent<DefaultParent<DefaultParentContextConstraintTypes<DefaultChild>>>,
}

impl DefaultPersonParentEntry {
	fn get() -> &'static Self {
		static COMPONENT: StaticComponent<DefaultPersonParentEntry> = StaticComponent::new();
		COMPONENT.get_or_init(|| Self {
			component: CompositionComponent::new(DefaultParent::<DefaultParentContextConstraintTypes<DefaultChild>>::new(DefaultParentContext::<DefaultParentContextConstraintTypes<DefaultChild>>::new("Bob", 42, [&DefaultPersonChildEntry::get().component.handle()])))
		})
	}
}

#[allow(unused_variables)]
impl Lifecycle for DefaultPersonParentEntry {
	async fn init(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.acquire(async |component| {
			let mut lifecycle_logger = logger::logger_state().logger("default.person_parent", BzdLoggerLevel::Info);
			log_info!(lifecycle_logger, "Initializing default.Parent")?;
					DefaultPersonChildEntry::get().init().await?;
			
			Ok(())
		}).await?;
		Ok(())
	}

	async fn shutdown(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.release(async |component| {
			let mut lifecycle_logger = logger::logger_state().logger("default.person_parent", BzdLoggerLevel::Info);
			log_info!(lifecycle_logger, "Shutting down default.Parent")?;
					DefaultPersonChildEntry::get().shutdown().await?;
			
			Ok(())
		}).await?;
		Ok(())
	}
}

struct DefaultPrinterEntry {
	component: CompositionComponent<DefaultPrinter<DefaultPrinterContextConstraintTypes<DefaultOut>>>,
}

impl DefaultPrinterEntry {
	fn get() -> &'static Self {
		static COMPONENT: StaticComponent<DefaultPrinterEntry> = StaticComponent::new();
		COMPONENT.get_or_init(|| Self {
			component: CompositionComponent::new(DefaultPrinter::<DefaultPrinterContextConstraintTypes<DefaultOut>>::new(DefaultPrinterContext::<DefaultPrinterContextConstraintTypes<DefaultOut>>::new(&DefaultOutEntry::get().component.handle())))
		})
	}
}

#[allow(unused_variables)]
impl Lifecycle for DefaultPrinterEntry {
	async fn init(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.acquire(async |component| {
			let mut lifecycle_logger = logger::logger_state().logger("default.printer", BzdLoggerLevel::Info);
			log_info!(lifecycle_logger, "Initializing default.Printer")?;
					DefaultOutEntry::get().init().await?;
			
			Ok(())
		}).await?;
		Ok(())
	}

	async fn shutdown(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.release(async |component| {
			let mut lifecycle_logger = logger::logger_state().logger("default.printer", BzdLoggerLevel::Info);
			log_info!(lifecycle_logger, "Shutting down default.Printer")?;
					DefaultOutEntry::get().shutdown().await?;
			
			Ok(())
		}).await?;
		Ok(())
	}
}

// ---- Execution

#[allow(non_snake_case)]
pub async fn runExecutor() -> Result<(), bzd::base::error::Error>
{
	// Initialize all registry entries.
	DefaultExecutorEntry::get().init().await?;
	DefaultOutEntry::get().init().await?;
	DefaultPersonChildEntry::get().init().await?;
	DefaultPersonParentEntry::get().init().await?;
	DefaultPrinterEntry::get().init().await?;

	let executor = DefaultExecutor::new();
	let logger_backend = executor.logger_backend();
	let mut logger = logger_backend.lock().await.logger("default.executor");

	log_info!(logger, "Initialization completed.")?;

	// Execute the workloads and services.
	log_info!(logger, "Composition phase.")?;
	let failure: core::cell::RefCell<Option<bzd::base::error::Error>> = core::cell::RefCell::new(None);
	let executor = executor
		.push_workload(async {
			match DefaultPersonChildEntry::get().component.lock().await.print_info().await {
				Ok(()) => 0,
				Err(e) => {
					*failure.borrow_mut() = Some(e);
					1
				}
			}
		})
		.push_workload(async {
			match DefaultPersonParentEntry::get().component.lock().await.print_info().await {
				Ok(()) => 0,
				Err(e) => {
					*failure.borrow_mut() = Some(e);
					1
				}
			}
		})
		.push_workload(async {
			match DefaultPrinterEntry::get().component.lock().await.run().await {
				Ok(()) => 0,
				Err(e) => {
					*failure.borrow_mut() = Some(e);
					1
				}
			}
		})
		;

	log_info!(logger, "Execution phase.")?;
	let result = executor.join().await;

	let error = (result != 0)
		.then(|| failure.into_inner().unwrap_or_else(|| bzd::base::error::failure!("One or more workloads failed.")));
	log_info!(logger, "Completion.")?;
	if let Some(error) = error.as_ref() {
		log_error!(logger, "Workload failure: {}", error)?;
	}
	logger_backend.lock().await.flush().await?;

	// Shutdown all registry entries.
	DefaultExecutorEntry::get().shutdown().await?;
	DefaultOutEntry::get().shutdown().await?;
	DefaultPersonChildEntry::get().shutdown().await?;
	DefaultPersonParentEntry::get().shutdown().await?;
	DefaultPrinterEntry::get().shutdown().await?;
	let _ = logger_backend.lock().await.flush().await;

	error.map_or(Ok(()), Err)
}

"""
		actual = self._generate(content=content, imports=["child", "parent"])
		self.assertEqual(actual.rstrip("\n"), expectedContent.rstrip("\n"))


if __name__ == "__main__":
	unittest.main()
