#![no_std]
#![no_main]

use rust_bdl_tests_components_interface::BzdTestUserContext;
use rust_bdl_tests_components_interface::BzdTestUserInterface;

#[allow(dead_code)]
struct User {
    context: BzdTestUserContext,
}

impl BzdTestUserInterface for User {
    async fn print_info(&mut self) -> Result<(), bzd::base::error::Error> {
        Ok(())
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    fn test_component() -> TestResult {
        let mut user = User {
            context: BzdTestUserContext {
                age: 28,
                username: "Alex",
            },
        };
        let _future = user.print_info();
        Ok(())
    }
}
