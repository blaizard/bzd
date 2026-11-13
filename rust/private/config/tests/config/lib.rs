#![no_std]
#![no_main]

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use config_rust::{A, B_C, DATA, E, OTHER};

    #[test]
    fn test_config() -> TestResult {
        assert_eq!(A, 42)?;
        assert!(B_C);
        assert_eq!(E, 1.5)?;
        assert_eq!(DATA, &b"hello\n"[..])?;
        assert_eq!(OTHER, &b"other\n"[..])?;
        Ok(())
    }
}
