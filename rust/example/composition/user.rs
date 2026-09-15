pub use rust_example_composition_interface::{
    BzdComponentsUserContext, BzdComponentsUserInterface,
};

// The component implementation, written once per component type.
pub struct BzdComponentsUser {
    pub context: BzdComponentsUserContext,
    // Internal variables
    pub nb_time_printed: u32,
}

impl BzdComponentsUserInterface for BzdComponentsUser {
    fn new(context: BzdComponentsUserContext) -> Self {
        Self {
            context,
            nb_time_printed: 1,
        }
    }

    async fn print_info(&mut self) -> Result<(), bzd::base::error::Error> {
        println!(
            "User: {}, Age: {}, Printed: {} time(s)",
            self.context.username, self.context.age, self.nb_time_printed
        );
        self.nb_time_printed += 1;
        Ok(())
    }
}
