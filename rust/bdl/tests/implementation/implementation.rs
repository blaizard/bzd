#![no_std]
#![no_main]

use rust_bdl_tests_implementation_interface::*;

#[allow(dead_code)]
struct A;

impl BzdTestExtern for A {
    fn foo(&self) -> i32 {
        42
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    fn test_extern() -> TestResult {
        assert_eq!(A.foo(), 42)?;
        Ok(())
    }
}
