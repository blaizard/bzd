#![no_std]
#![no_main]

use rust_bdl_tests_dependencies_file_a::*;
use rust_bdl_tests_dependencies_file_b::*;

#[allow(dead_code)]
struct B;

impl BzdTestFileBInterface for B {
    async fn compute(&mut self) -> Result<BzdTestTypeA, bzd::base::error::Error> {
        Ok(BzdTestTypeA::One)
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    fn test_dependencies() -> TestResult {
        let mut b = B;
        let _future = b.compute();
        Ok(())
    }
}
