#![no_std]
#![no_main]

use rust_bdl_tests_dependencies_file_a::*;
use rust_bdl_tests_dependencies_file_b::*;

#[allow(dead_code)]
struct B;

impl BzdTestFileB for B {
    fn compute(&self) -> BzdTestTypeA {
        BzdTestTypeA::One
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    fn test_dependencies() -> TestResult {
        assert_eq!(B.compute(), BzdTestTypeA::One)?;
        Ok(())
    }
}
