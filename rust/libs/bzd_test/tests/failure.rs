#![no_std]
#![no_main]

#[cfg(test)]
#[bzd_test::test]
mod tests {
    #[test]
    fn test_fail() -> TestResult {
        assert_eq!(1, 2)?;
        Ok(())
    }
}
