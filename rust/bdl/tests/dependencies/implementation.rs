#![no_std]
#![no_main]

use rust_bdl_tests_dependencies_file_b::*;

#[allow(dead_code)]
struct A;

impl BzdTestFileA for A {
    fn value(&self) -> i32 {
        42
    }
}

#[allow(dead_code)]
struct B;

impl BzdTestFileB for B {
    fn compute(&self) -> i32 {
        43
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    fn test_dependencies() -> TestResult {
        assert_eq!(A.value(), 42)?;
        assert_eq!(B.compute(), 43)?;
        Ok(())
    }
}
