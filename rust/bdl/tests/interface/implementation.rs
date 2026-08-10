#![no_std]
#![no_main]

use interface::MyInterface;

#[allow(dead_code)]
struct Calculator;

impl MyInterface for Calculator {
    fn add(&self, a: &i32, b: &i32) -> i32 {
        *a + *b
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    fn test_calculator() -> TestResult {
        let calculator = Calculator;
        assert_eq!(calculator.add(&2, &3), 5)?;
        Ok(())
    }
}
