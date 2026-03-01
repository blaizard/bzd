#[cfg(test)]
#[bzd_test::test]
mod tests {
    #[test]
    #[allow(clippy::eq_op)]
    fn test_extra_success() -> TestResult {
        assert_eq!(1, 1)?;
        Ok(())
    }
}
