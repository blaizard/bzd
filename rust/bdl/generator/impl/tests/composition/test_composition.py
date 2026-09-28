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
		with self.assertRaises(Exception) as context:
			self._generate(content=content, imports=["child", "parent"])
		self.assertIn("must declare a referenced 'out' entry", str(context.exception))

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
use component::{Lifecycle, LocalStatic, Wrapper};

#[allow(unused_imports)]
use bzd::base::logger::{log_error, log_info, Logger};

#[allow(unused_imports)]
use rust_interfaces_executor::BzdExecutorInterface;
#[allow(unused_imports)]
use child::*;
#[allow(unused_imports)]
use parent::*;

// ---- Registry

#[allow(dead_code)]
struct DefaultExecutorEntry {
	instance: Executor,
}

impl Lifecycle for DefaultExecutorEntry {
	async fn init(&mut self) -> Result<(), bzd::base::error::Error>
	{
		Ok(())
	}

	async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error>
	{
		Ok(())
	}
}

fn registry_default_executor() -> &'static mut Wrapper<DefaultExecutorEntry> {
	static COMPONENT: LocalStatic<Wrapper<DefaultExecutorEntry>> = LocalStatic::new();
	COMPONENT.get_mut_or_init(|| {
		Wrapper::new(DefaultExecutorEntry {
			instance: Executor::new(),
		})
	})
}

#[allow(dead_code)]
struct DefaultOutEntry {
	instance: DefaultOut,
}

impl Lifecycle for DefaultOutEntry {
	async fn init(&mut self) -> Result<(), bzd::base::error::Error>
	{
		Ok(())
	}

	async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error>
	{
		Ok(())
	}
}

fn registry_default_out() -> &'static mut Wrapper<DefaultOutEntry> {
	static COMPONENT: LocalStatic<Wrapper<DefaultOutEntry>> = LocalStatic::new();
	COMPONENT.get_mut_or_init(|| {
		Wrapper::new(DefaultOutEntry {
			instance: DefaultOut::new(),
		})
	})
}

#[allow(dead_code)]
struct DefaultPerson_childEntry {
	instance: DefaultChild,
}

impl Lifecycle for DefaultPerson_childEntry {
	async fn init(&mut self) -> Result<(), bzd::base::error::Error>
	{
		Ok(())
	}

	async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error>
	{
		Ok(())
	}
}

fn registry_default_person_child() -> &'static mut Wrapper<DefaultPerson_childEntry> {
	static COMPONENT: LocalStatic<Wrapper<DefaultPerson_childEntry>> = LocalStatic::new();
	COMPONENT.get_mut_or_init(|| {
		Wrapper::new(DefaultPerson_childEntry {
			instance: DefaultChild::new(DefaultChildContext { username: "Alex", age: 28 }),
		})
	})
}

#[allow(dead_code)]
struct DefaultPerson_parentEntry {
	instance: DefaultParent<DefaultParentContextConstraintTypes<DefaultChild>>,
}

impl Lifecycle for DefaultPerson_parentEntry {
	async fn init(&mut self) -> Result<(), bzd::base::error::Error>
	{
		registry_default_person_child().init().await?;
		Ok(())
	}

	async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error>
	{
		registry_default_person_child().shutdown().await?;
		Ok(())
	}
}

fn registry_default_person_parent() -> &'static mut Wrapper<DefaultPerson_parentEntry> {
	static COMPONENT: LocalStatic<Wrapper<DefaultPerson_parentEntry>> = LocalStatic::new();
	COMPONENT.get_mut_or_init(|| {
		Wrapper::new(DefaultPerson_parentEntry {
			instance: DefaultParent::<DefaultParentContextConstraintTypes<DefaultChild>>::new(DefaultParentContext::<DefaultParentContextConstraintTypes<DefaultChild>>::new("Bob", 42, [&mut registry_default_person_child().instance])),
		})
	})
}

#[allow(dead_code)]
struct DefaultPrinterEntry {
	instance: DefaultPrinter,
}

impl Lifecycle for DefaultPrinterEntry {
	async fn init(&mut self) -> Result<(), bzd::base::error::Error>
	{
		registry_default_out().init().await?;
		Ok(())
	}

	async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error>
	{
		registry_default_out().shutdown().await?;
		Ok(())
	}
}

fn registry_default_printer() -> &'static mut Wrapper<DefaultPrinterEntry> {
	static COMPONENT: LocalStatic<Wrapper<DefaultPrinterEntry>> = LocalStatic::new();
	COMPONENT.get_mut_or_init(|| {
		Wrapper::new(DefaultPrinterEntry {
			instance: DefaultPrinter::new(DefaultPrinterContext { out: &mut registry_default_out().instance }),
		})
	})
}

// ---- Execution

#[allow(non_snake_case)]
pub async fn runExecutor() -> Result<(), bzd::base::error::Error>
{
	// Create the logger, bound to the 'out' registry entry.
	let mut logger = Logger::new(&mut registry_default_out().instance);

	// Initialize all registry entries.
	registry_default_executor().init().await?;
	registry_default_out().init().await?;
	registry_default_person_child().init().await?;
	registry_default_person_parent().init().await?;
	registry_default_printer().init().await?;
	log_info!(logger, "[default.executor] Initialization completed.").await?;

	// Execute the workloads and services.
	log_info!(logger, "[default.executor] Composition phase.").await?;
	let failure: core::cell::RefCell<Option<bzd::base::error::Error>> = core::cell::RefCell::new(None);
	let executor = GenericExecutor::new()
		.push_workload(async {
			match registry_default_person_child().instance.print_info().await {
				Ok(()) => 0,
				Err(e) => {
					*failure.borrow_mut() = Some(e);
					1
				}
			}
		})
		.push_workload(async {
			match registry_default_person_parent().instance.print_info().await {
				Ok(()) => 0,
				Err(e) => {
					*failure.borrow_mut() = Some(e);
					1
				}
			}
		})
		.push_workload(async {
			match registry_default_printer().instance.run().await {
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

	log_info!(logger, "[default.executor] Completion.").await?;

	// Shutdown all registry entries.
	registry_default_executor().shutdown().await?;
	registry_default_out().shutdown().await?;
	registry_default_person_child().shutdown().await?;
	registry_default_person_parent().shutdown().await?;
	registry_default_printer().shutdown().await?;

	if result != 0 {
		let error = failure.into_inner().unwrap_or_else(|| bzd::base::error::failure("One or more workloads failed."));
		log_error!(logger, "[default.executor] Workload failure: {}", error).await?;
		return Err(error);
	}
	Ok(())
}
"""
		actual = self._generate(content=content, imports=["child", "parent"])
		self.assertEqual(actual.rstrip("\n"), expectedContent.rstrip("\n"))


if __name__ == "__main__":
	unittest.main()
