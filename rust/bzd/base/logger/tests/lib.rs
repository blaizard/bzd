#![no_std]
#![no_main]

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use bzd::base::error::{self, Error};
    use bzd::base::logger::{log_debug, log_error, log_info, log_warning, Level, Logger};
    use interfaces_stream::BzdOStreamInterface;

    struct RecordingSink {
        data: [u8; 2048],
        len: usize,
    }

    impl RecordingSink {
        fn new() -> Self {
            Self {
                data: [0; 2048],
                len: 0,
            }
        }

        fn as_str(&self) -> &str {
            core::str::from_utf8(&self.data[..self.len]).unwrap()
        }
    }

    impl BzdOStreamInterface for RecordingSink {
        async fn write(&mut self, data: &[u8]) -> Result<(), Error> {
            self.data[self.len..self.len + data.len()].copy_from_slice(data);
            self.len += data.len();
            Ok(())
        }
    }

    fn block_on<F: core::future::Future>(future: F) -> F::Output {
        use core::task::{Context, Poll, RawWaker, RawWakerVTable, Waker};

        fn clone(_: *const ()) -> RawWaker {
            RawWaker::new(core::ptr::null(), &VTABLE)
        }
        fn noop(_: *const ()) {}
        static VTABLE: RawWakerVTable = RawWakerVTable::new(clone, noop, noop, noop);

        let mut future = core::pin::pin!(future);
        // Safety: The waker never dereferences its data pointer.
        let waker = unsafe { Waker::from_raw(RawWaker::new(core::ptr::null(), &VTABLE)) };
        let mut context = Context::from_waker(&waker);
        loop {
            if let Poll::Ready(result) = future.as_mut().poll(&mut context) {
                return result;
            }
        }
    }

    fn expect_ok(result: Result<(), Error>) -> TestResult {
        match result {
            Ok(()) => Ok(()),
            Err(_) => Err(TestError {
                file: file!(),
                line: line!(),
                message: "logger call failed",
            }),
        }
    }

    #[test]
    fn test_logging() -> TestResult {
        let mut sink = RecordingSink::new();
        {
            let mut logger = Logger::new(&mut sink);
            let result = block_on(async {
                log_info!(logger, "count = {}, name = {}", 42, "Alex");
                log_warning!(logger, "careful");
                log_error!(logger, "boom");
                log_debug!(logger, "hidden");
                Ok::<(), Error>(())
            });
            expect_ok(result)?;
        }
        let message = sink.as_str();
        assert_eq!(message.contains("[i] ["), true)?;
        assert_eq!(message.contains("] count = 42, name = Alex\n"), true)?;
        assert_eq!(message.contains("[w] ["), true)?;
        assert_eq!(message.contains("] careful\n"), true)?;
        assert_eq!(message.contains("[e] ["), true)?;
        assert_eq!(message.contains("] boom\n"), true)?;
        // Debug is filtered out by the default minimum level.
        assert_eq!(message.contains("[d] ["), false)?;
        Ok(())
    }

    #[test]
    fn test_error_value() -> TestResult {
        let mut sink = RecordingSink::new();
        {
            let mut logger = Logger::new(&mut sink);
            let error = error::failure("boom");
            let result = block_on(async {
                log_error!(logger, "{}", error);
                Ok::<(), Error>(())
            });
            expect_ok(result)?;
        }
        assert_eq!(sink.as_str().ends_with(": failure: boom\n"), true)?;
        Ok(())
    }

    #[test]
    fn test_long_message() -> TestResult {
        let mut sink = RecordingSink::new();
        {
            let mut logger = Logger::new(&mut sink);
            let long = core::str::from_utf8(&[b'x'; 600]).unwrap();
            let result = block_on(async {
                log_info!(logger, "{}", long);
                Ok::<(), Error>(())
            });
            expect_ok(result)?;
        }
        let message = sink.as_str();
        assert_eq!(message.matches('x').count(), 600)?;
        assert_eq!(message.ends_with("\n"), true)?;
        Ok(())
    }

    #[test]
    fn test_minimum_level_filtering() -> TestResult {
        let mut sink = RecordingSink::new();
        {
            let mut logger = Logger::new(&mut sink);
            logger.set_minimum_level(Level::Error);
            let result = block_on(async {
                log_error!(logger, "shown");
                log_warning!(logger, "hidden");
                log_info!(logger, "hidden");
                Ok::<(), Error>(())
            });
            expect_ok(result)?;
        }
        let message = sink.as_str();
        assert_eq!(message.contains("] shown\n"), true)?;
        assert_eq!(message.contains("hidden"), false)?;
        Ok(())
    }

    #[test]
    fn test_location() -> TestResult {
        let mut sink = RecordingSink::new();
        {
            let mut logger = Logger::new(&mut sink);
            let result = block_on(async {
                log_info!(logger, "hello");
                Ok::<(), Error>(())
            });
            expect_ok(result)?;
        }
        assert_eq!(sink.as_str().contains(file!()), true)?;
        Ok(())
    }
}
