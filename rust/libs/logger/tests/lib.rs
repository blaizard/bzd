#![no_std]
#![no_main]

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use bzd::base::error::{self, Error};
    use logger::{log_debug, log_error, log_info, log_warning, BzdLoggerBackendInterface, Logger};
    use rust_interfaces_logger::BzdLoggerLevel;

    fn level_priority(level: BzdLoggerLevel) -> u8 {
        match level {
            BzdLoggerLevel::Error => 0,
            BzdLoggerLevel::Warning => 1,
            BzdLoggerLevel::Info => 2,
            BzdLoggerLevel::Debug => 3,
        }
    }

    struct MockBackend {
        data: [u8; 1024],
        len: usize,
        min_level: BzdLoggerLevel,
        flush_count: usize,
    }

    impl MockBackend {
        const fn new(min_level: BzdLoggerLevel) -> Self {
            Self {
                data: [0; 1024],
                len: 0,
                min_level,
                flush_count: 0,
            }
        }

        fn as_str(&self) -> &str {
            core::str::from_utf8(&self.data[..self.len]).unwrap()
        }

        fn set_minimum_level(&mut self, level: BzdLoggerLevel) {
            self.min_level = level;
        }

        fn is_enabled(&self, level: BzdLoggerLevel) -> bool {
            level_priority(level) <= level_priority(self.min_level)
        }
    }

    impl BzdLoggerBackendInterface for MockBackend {
        fn push<F>(&mut self, level: BzdLoggerLevel, write: F) -> Result<(), Error>
        where
            F: FnOnce(&mut dyn core::fmt::Write) -> core::fmt::Result,
        {
            if !self.is_enabled(level) {
                return Ok(());
            }
            let mut writer = Writer {
                data: &mut self.data,
                len: &mut self.len,
            };
            write(&mut writer).map_err(|_| error::failure!("failed to format the record"))
        }

        async fn flush(&mut self) -> Result<(), Error> {
            self.flush_count += 1;
            Ok(())
        }
    }

    struct Writer<'a> {
        data: &'a mut [u8],
        len: &'a mut usize,
    }

    impl core::fmt::Write for Writer<'_> {
        fn write_str(&mut self, text: &str) -> core::fmt::Result {
            let bytes = text.as_bytes();
            if bytes.len() > self.data.len() - *self.len {
                return Err(core::fmt::Error);
            }
            self.data[*self.len..*self.len + bytes.len()].copy_from_slice(bytes);
            *self.len += bytes.len();
            Ok(())
        }
    }

    #[test]
    async fn test_logging() -> TestResult {
        let mut backend = MockBackend::new(BzdLoggerLevel::Info);
        let mut logger = Logger::new(&mut backend);
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
        // One flush per backend flush.
        assert_eq!(backend.flush_count, 1)?;
        Ok(())
    }

    #[test]
    async fn test_minimum_level() -> TestResult {
        let mut backend = MockBackend::new(BzdLoggerLevel::Info);
        backend.set_minimum_level(BzdLoggerLevel::Error);
        let mut logger = Logger::new(&mut backend);
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
        let mut backend = MockBackend::new(BzdLoggerLevel::Info);
        let mut logger = Logger::new(&mut backend);
        log_info!(logger, "chained")?.flush().await?;

        assert_eq!(backend.as_str().contains("] chained\n"), true)?;
        Ok(())
    }

    #[test]
    async fn test_error_value() -> TestResult {
        let mut backend = MockBackend::new(BzdLoggerLevel::Info);
        let mut logger = Logger::new(&mut backend);
        log_error!(logger, "{}", error::failure!("boom"))?;
        logger.flush().await?;

        assert_eq!(backend.as_str().ends_with(": failure: boom\n"), true)?;
        Ok(())
    }
}
