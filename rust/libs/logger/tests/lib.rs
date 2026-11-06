#![no_std]
#![no_main]

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use bzd::base::error::{self, Error};
    use core::cell::RefCell;
    use logger::{
        log_debug, log_error, log_info, log_warning, BzdLoggerBackendInterface, Logger, LoggerCell,
    };
    use rust_interfaces_logger::BzdLoggerLevel;

    struct MockState {
        data: [u8; 1024],
        len: usize,
    }

    struct MockBackend {
        state: RefCell<MockState>,
    }

    impl MockBackend {
        const fn new() -> Self {
            Self {
                state: RefCell::new(MockState {
                    data: [0; 1024],
                    len: 0,
                }),
            }
        }

        fn as_str(&self) -> core::cell::Ref<'_, str> {
            core::cell::Ref::map(self.state.borrow(), |state| {
                core::str::from_utf8(&state.data[..state.len]).unwrap()
            })
        }
    }

    impl BzdLoggerBackendInterface for MockBackend {
        fn push(
            &self,
            _level: BzdLoggerLevel,
            write: &mut dyn FnMut(&mut dyn core::fmt::Write) -> core::fmt::Result,
        ) -> Result<Option<usize>, Error> {
            let mut state = self.state.borrow_mut();
            let mut writer = Writer { state: &mut state };
            write(&mut writer).map_err(|_| error::failure!("failed to format the record"))?;
            Ok(Some(state.len))
        }

        fn is_consumed(&self, _target: usize) -> bool {
            true
        }
    }

    struct Writer<'a> {
        state: &'a mut MockState,
    }

    impl core::fmt::Write for Writer<'_> {
        fn write_str(&mut self, text: &str) -> core::fmt::Result {
            let bytes = text.as_bytes();
            if bytes.len() > self.state.data.len() - self.state.len {
                return Err(core::fmt::Error);
            }
            let len = self.state.len;
            self.state.data[len..len + bytes.len()].copy_from_slice(bytes);
            self.state.len += bytes.len();
            Ok(())
        }
    }

    fn logger(backend: &MockBackend, min_level: BzdLoggerLevel) -> Logger {
        let queue =
            backend as &dyn BzdLoggerBackendInterface as *const dyn BzdLoggerBackendInterface;
        Logger::new(queue, min_level)
    }

    #[test]
    async fn test_logging() -> TestResult {
        let backend = MockBackend::new();
        let mut logger = logger(&backend, BzdLoggerLevel::Info);
        log_info!(logger, "count = {}, name = {}", 42, "Alex")?;
        log_warning!(logger, "careful")?;
        log_error!(logger, "boom")?;
        log_debug!(logger, "hidden")?;
        logger.flush().await?;

        let message = backend.as_str();
        let name = file!().rsplit_once('/').map_or(file!(), |(_, name)| name);
        assert_eq!(message.contains("[i] ["), true)?;
        assert_eq!(message.contains("] count = 42, name = Alex\n"), true)?;
        assert_eq!(message.contains("[w] ["), true)?;
        assert_eq!(message.contains("] careful\n"), true)?;
        assert_eq!(message.contains("[e] ["), true)?;
        assert_eq!(message.contains("] boom\n"), true)?;
        assert_eq!(message.contains(name), true)?;
        // Debug is filtered out by the minimum level.
        assert_eq!(message.contains("[d] ["), false)?;
        Ok(())
    }

    #[test]
    async fn test_minimum_level() -> TestResult {
        let backend = MockBackend::new();
        let mut logger = logger(&backend, BzdLoggerLevel::Error);
        log_error!(logger, "shown")?;
        log_warning!(logger, "hidden")?;
        logger.flush().await?;

        let message = backend.as_str();
        assert_eq!(message.contains("] shown\n"), true)?;
        assert_eq!(message.contains("hidden"), false)?;
        Ok(())
    }

    #[test]
    async fn test_chained_flush() -> TestResult {
        let backend = MockBackend::new();
        let mut logger = logger(&backend, BzdLoggerLevel::Info);
        log_info!(logger, "chained")?.flush().await?;

        assert_eq!(backend.as_str().contains("] chained\n"), true)?;
        Ok(())
    }

    #[test]
    async fn test_error_value() -> TestResult {
        let backend = MockBackend::new();
        let mut logger = logger(&backend, BzdLoggerLevel::Info);
        log_error!(logger, "{}", error::failure!("boom"))?;
        logger.flush().await?;

        assert_eq!(backend.as_str().ends_with(": failure: boom\n"), true)?;
        Ok(())
    }

    #[test]
    async fn test_logger_cell_before_install() -> TestResult {
        static CELL: LoggerCell = LoggerCell::new();
        let mut logger = CELL.logger(BzdLoggerLevel::Info);
        log_info!(logger, "dropped")?;
        logger.flush().await?;
        Ok(())
    }

    #[test]
    async fn test_logger_cell_after_install() -> TestResult {
        static CELL: LoggerCell = LoggerCell::new();
        let backend = MockBackend::new();
        // SAFETY: the backend outlives the cell usage in this test.
        unsafe { CELL.install(&backend) };
        let mut logger = CELL.logger(BzdLoggerLevel::Info);
        log_info!(logger, "cell")?;
        logger.flush().await?;

        assert_eq!(backend.as_str().contains("] cell\n"), true)?;
        Ok(())
    }

    #[test]
    async fn test_logger_cell_shared() -> TestResult {
        static CELL: LoggerCell = LoggerCell::new();
        let backend = MockBackend::new();
        // SAFETY: the backend outlives the cell usage in this test.
        unsafe { CELL.install(&backend) };
        let mut first = CELL.logger(BzdLoggerLevel::Info);
        let mut second = CELL.logger(BzdLoggerLevel::Warning);
        log_info!(first, "first")?;
        log_warning!(second, "second")?;
        log_debug!(second, "debug")?;
        first.flush().await?;
        second.flush().await?;

        let message = backend.as_str();
        assert_eq!(message.contains("] first\n"), true)?;
        assert_eq!(message.contains("] second\n"), true)?;
        assert_eq!(message.contains("] debug\n"), false)?;
        Ok(())
    }
}
