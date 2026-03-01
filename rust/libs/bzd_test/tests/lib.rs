#![no_std]
#![no_main]

mod extra_file;

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use bzd_test::tests::get_tests;

    #[test]
    #[allow(clippy::eq_op)]
    fn test_success() -> TestResult {
        assert_eq!(1, 1)?;

        Ok(())
    }

    #[test]
    #[ignore]
    fn test_fail() -> TestResult {
        assert_eq!(1, 2)?;
        Ok(())
    }

    #[test]
    fn test_nb_tests() -> TestResult {
        let tests = get_tests();
        assert_eq!(tests.len(), 4)?;
        Ok(())
    }
}
