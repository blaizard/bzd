#![no_std]
#![no_main]

use rust_bdl_tests_interface_interface::BzdTestHelloInterface;

#[allow(dead_code)]
struct Calculator;

impl BzdTestHelloInterface for Calculator {
    async fn add(&mut self, a: i32, b: i32) -> Result<i32, bzd::base::error::Error> {
        Ok(a + b)
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    fn test_calculator() -> TestResult {
        let mut calculator = Calculator;
        let _future = calculator.add(2, 3);
        Ok(())
    }
}
