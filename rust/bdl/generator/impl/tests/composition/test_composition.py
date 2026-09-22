import unittest

from bdl.object import Object, ObjectContext
from bdl.visitors.composition.visitor import Composition

from rust.bdl.generator.impl.visitor import compositionRust


class TestComposition(unittest.TestCase):
	def _generate(self, imports: list[str]) -> str:
		bdl = Object.fromContent(
			content="""
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
""",
			objectContext=ObjectContext(resolve=True),
		)

		composition = Composition()
		composition.visit(bdl).process()
		view = composition.view(target="default")
		return compositionRust(composition=view, data={"rust": {"imports": imports}})

	def testComposition(self) -> None:
		expectedContent = """#![no_std]

#[allow(unused_imports)]
use component::{Lifecycle, LocalStatic, Wrapper};

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
			instance: DefaultParent<DefaultParentContextConstraintTypes<DefaultChild>>::new(DefaultParentContext::<DefaultParentContextConstraintTypes<DefaultChild>>::new("Bob", 42, [&mut registry_default_person_child().instance])),
		})
	})
}

// ---- Execution

#[allow(non_snake_case)]
pub async fn runExecutor() -> Result<(), bzd::base::error::Error>
{
	// Create all registry entries.
	let registry_default_executor = registry_default_executor();
	let registry_default_person_child = registry_default_person_child();
	let registry_default_person_parent = registry_default_person_parent();

	// Initialize all registry entries.
	registry_default_executor.init().await?;
	registry_default_person_child.init().await?;
	registry_default_person_parent.init().await?;

	// Execute the workloads and services.
	let failure: core::cell::RefCell<Option<bzd::base::error::Error>> = core::cell::RefCell::new(None);
	let result = GenericExecutor::new()
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
		.join()
		.await;

	// Shutdown all registry entries.
	registry_default_executor.shutdown().await?;
	registry_default_person_child.shutdown().await?;
	registry_default_person_parent.shutdown().await?;

	if result != 0 {
		return Err(failure.into_inner().unwrap_or_else(|| bzd::base::error::failure("One or more workloads failed.")));
	}
	Ok(())
}
"""
		actual = self._generate(imports=["child", "parent"])
		self.assertEqual(
			actual.rstrip("\n"),
			expectedContent.rstrip("\n"),
		)


if __name__ == "__main__":
	unittest.main()
