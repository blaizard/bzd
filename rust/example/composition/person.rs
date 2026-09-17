pub use rust_example_composition_interface::{
    BzdComponentsPersonContext, BzdComponentsPersonInterface,
};

// The component implementation, written once per component type.
pub struct BzdComponentsPerson {
    pub context: BzdComponentsPersonContext,
    // Internal variables
    pub nb_time_printed: u32,
}

impl BzdComponentsPerson {
    pub fn new(context: BzdComponentsPersonContext) -> Self {
        Self {
            context,
            nb_time_printed: 1,
        }
    }
}

impl BzdComponentsPersonInterface for BzdComponentsPerson {
    async fn print_info(&mut self) -> Result<(), bzd::base::error::Error> {
        println!(
            "Person: {}, Age: {}, Printed: {} time(s)",
            self.context.username, self.context.age, self.nb_time_printed
        );
        self.nb_time_printed += 1;
        Ok(())
    }
}
