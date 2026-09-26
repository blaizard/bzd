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

    struct YieldOnce(Option<i32>);

    impl core::future::Future for YieldOnce {
        type Output = i32;

        fn poll(
            self: core::pin::Pin<&mut Self>,
            _: &mut core::task::Context<'_>,
        ) -> core::task::Poll<i32> {
            match self.get_mut().0.take() {
                Some(value) => core::task::Poll::Ready(value),
                None => core::task::Poll::Pending,
            }
        }
    }

    #[test]
    async fn test_async() -> TestResult {
        assert_eq!(YieldOnce(Some(42)).await, 42)?;
        Ok(())
    }

    #[test]
    fn test_nb_tests() -> TestResult {
        let tests = get_tests();
        assert_eq!(tests.len(), 5)?;
        Ok(())
    }
}
