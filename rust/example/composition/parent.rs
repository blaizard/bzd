pub use rust_example_composition_interface::{
    BzdComponentsParentContext, BzdComponentsParentContextConstraint,
    BzdComponentsParentContextConstraintTypes, BzdComponentsParentInterface,
    BzdComponentsPersonInterface,
};

// The component implementation, written once per component type.
pub struct BzdComponentsParent<C>
where
    C: BzdComponentsParentContextConstraint,
{
    pub context: BzdComponentsParentContext<C>,
}

impl<C> BzdComponentsParent<C>
where
    C: BzdComponentsParentContextConstraint,
{
    pub fn new(context: BzdComponentsParentContext<C>) -> Self {
        Self { context }
    }
}

impl<C> BzdComponentsPersonInterface for BzdComponentsParent<C>
where
    C: BzdComponentsParentContextConstraint,
{
    async fn print_info(&mut self) -> Result<(), bzd::base::error::Error> {
        println!(
            "Parent: {}, Age: {}, Children: {}:",
            self.context.username,
            self.context.age,
            self.context.children.len()
        );
        for child in self.context.children.iter_mut() {
            child.print_info().await?;
        }
        Ok(())
    }
}

impl<C> BzdComponentsParentInterface for BzdComponentsParent<C> where
    C: BzdComponentsParentContextConstraint
{
}
