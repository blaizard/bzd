import unittest

from bdl.object import Object, ObjectContext
from bdl.visitors.composition.visitor import Composition

from rust.bdl.generator.impl.visitor import compositionRust


class TestComposition(unittest.TestCase):
	def _generate(self, content: str, imports: list[str]) -> str:
		bdl = Object.fromContent(content=content, objectContext=ObjectContext(resolve=True))
		composition = Composition()
		composition.visit(bdl).process()
		view = composition.view(target="default")
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
		self.assertIn("let mut logger = Logger::new(&mut *logger_backend);", actual)

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
use logger::{log_error, log_info, Logger};

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
			
			Ok(())
		}).await?;
		Ok(())
	}

	async fn shutdown(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.release(async |component| {
			
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
			
			Ok(())
		}).await?;
		Ok(())
	}

	async fn shutdown(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.release(async |component| {
			
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
			
			Ok(())
		}).await?;
		Ok(())
	}

	async fn shutdown(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.release(async |component| {
			
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
					DefaultPersonChildEntry::get().init().await?;
			
			Ok(())
		}).await?;
		Ok(())
	}

	async fn shutdown(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.release(async |component| {
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
					DefaultOutEntry::get().init().await?;
			
			Ok(())
		}).await?;
		Ok(())
	}

	async fn shutdown(&self) -> Result<(), bzd::base::error::Error>
	{
		self.component.release(async |component| {
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
	let mut logger_backend = executor.logger_backend().lock().await;
	let mut logger = Logger::new(&mut *logger_backend);

	log_info!(logger, "[default.executor] Initialization completed.").await?;

	// Execute the workloads and services.
	log_info!(logger, "[default.executor] Composition phase.").await?;
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

	log_info!(logger, "[default.executor] Execution phase.").await?;
	let result = executor.join().await;

	let error = (result != 0)
		.then(|| failure.into_inner().unwrap_or_else(|| bzd::base::error::failure("One or more workloads failed.")));
	log_info!(logger, "[default.executor] Completion.").await?;
	if let Some(error) = error.as_ref() {
		log_error!(logger, "[default.executor] Workload failure: {}", error).await?;
	}
	drop(logger_backend);

	// Shutdown all registry entries.
	DefaultExecutorEntry::get().shutdown().await?;
	DefaultOutEntry::get().shutdown().await?;
	DefaultPersonChildEntry::get().shutdown().await?;
	DefaultPersonParentEntry::get().shutdown().await?;
	DefaultPrinterEntry::get().shutdown().await?;

	error.map_or(Ok(()), Err)
}

"""
		actual = self._generate(content=content, imports=["child", "parent"])
		self.assertEqual(actual.rstrip("\n"), expectedContent.rstrip("\n"))


if __name__ == "__main__":
	unittest.main()
