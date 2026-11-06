#![no_std]
#![no_main]

pub use rust_bdl_tests_logger_interface::BzdTestLoggerUserContext;
pub use rust_bdl_tests_logger_interface::BzdTestLoggerUserInterface;

#[cfg(test)]
#[bzd_test::test]
mod tests {
    #[test]
    fn test_compiles() -> TestResult {
        Ok(())
    }
}
