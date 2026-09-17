pub use rust_example_composition_interface::{
    BzdComponentsChildContext, BzdComponentsChildInterface, BzdComponentsPersonInterface,
};

// The component implementation, written once per component type.
pub struct BzdComponentsChild {
    pub context: BzdComponentsChildContext,
}

impl BzdComponentsChild {
    pub fn new(context: BzdComponentsChildContext) -> Self {
        Self { context }
    }
}

impl BzdComponentsPersonInterface for BzdComponentsChild {
    async fn print_info(&mut self) -> Result<(), bzd::base::error::Error> {
        println!(
            "Child: {}, Age: {}",
            self.context.username, self.context.age
        );
        Ok(())
    }
}

impl BzdComponentsChildInterface for BzdComponentsChild {}
