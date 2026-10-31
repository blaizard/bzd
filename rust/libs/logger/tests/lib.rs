#![no_std]
#![no_main]

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use bzd::base::error::{self, Error};
    use bzd::base::stream::OStream;
    use bzd::sync::mutex::Mutex;
    use logger::{log_debug, log_error, log_info, log_warning, Logger};
    use rust_components_generic_logger_backend_ostream::{
        BzdComponentsGenericOStreamLoggerBackend, BzdComponentsGenericOStreamLoggerBackendContext,
    };
    use rust_interfaces_logger::BzdLoggerLevel;

    struct RecordingSink {
        data: [u8; 1024],
        len: usize,
        flush_count: usize,
    }

    impl RecordingSink {
        const fn new() -> Self {
            Self {
                data: [0; 1024],
                len: 0,
                flush_count: 0,
            }
        }

        fn as_str(&self) -> &str {
            core::str::from_utf8(&self.data[..self.len]).unwrap()
        }
    }

    impl OStream for RecordingSink {
        async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
            self.data[self.len..self.len + data.len()].copy_from_slice(data);
            self.len += data.len();
            Ok(data.len())
        }

        async fn flush(&mut self) -> Result<(), Error> {
            self.flush_count += 1;
            Ok(())
        }
    }

    // Tests run sequentially, the sink is reset at the beginning of each test.
    static SINK: Mutex<RecordingSink> = Mutex::new(RecordingSink::new());

    #[test]
    async fn test_logging() -> TestResult {
        *SINK.lock().await = RecordingSink::new();
        {
            let context =
                BzdComponentsGenericOStreamLoggerBackendContext::new(&SINK, BzdLoggerLevel::Info);
            let mut backend = BzdComponentsGenericOStreamLoggerBackend::new(context);
            let mut logger = Logger::new(&mut backend);
            log_info!(logger, "count = {}, name = {}", 42, "Alex").await?;
            log_warning!(logger, "careful").await?;
            log_error!(logger, "boom").await?;
            log_debug!(logger, "hidden").await?;
        }
        let guard = SINK.lock().await;
        let message = guard.as_str();
        let name = file!().rsplit_once('/').map_or(file!(), |(_, name)| name);
        assert_eq!(message.contains("[i] ["), true)?;
        assert_eq!(message.contains("] count = 42, name = Alex\n"), true)?;
        assert_eq!(message.contains("[w] ["), true)?;
        assert_eq!(message.contains("] careful\n"), true)?;
        assert_eq!(message.contains("[e] ["), true)?;
        assert_eq!(message.contains("] boom\n"), true)?;
        assert_eq!(message.contains(name), true)?;
        // Debug is filtered out by the default minimum level.
        assert_eq!(message.contains("[d] ["), false)?;
        // One flush per emitted record.
        assert_eq!(guard.flush_count, 3)?;
        Ok(())
    }

    #[test]
    async fn test_minimum_level() -> TestResult {
        *SINK.lock().await = RecordingSink::new();
        {
            let context =
                BzdComponentsGenericOStreamLoggerBackendContext::new(&SINK, BzdLoggerLevel::Info);
            let mut backend = BzdComponentsGenericOStreamLoggerBackend::new(context);
            backend.set_minimum_level(BzdLoggerLevel::Error);
            let mut logger = Logger::new(&mut backend);
            log_error!(logger, "shown").await?;
            log_warning!(logger, "hidden").await?;
        }
        let guard = SINK.lock().await;
        let message = guard.as_str();
        assert_eq!(message.contains("] shown\n"), true)?;
        assert_eq!(message.contains("hidden"), false)?;
        Ok(())
    }

    #[test]
    async fn test_error_value() -> TestResult {
        *SINK.lock().await = RecordingSink::new();
        {
            let context =
                BzdComponentsGenericOStreamLoggerBackendContext::new(&SINK, BzdLoggerLevel::Info);
            let mut backend = BzdComponentsGenericOStreamLoggerBackend::new(context);
            let mut logger = Logger::new(&mut backend);
            log_error!(logger, "{}", error::failure!("boom")).await?;
        }
        let guard = SINK.lock().await;
        assert_eq!(guard.as_str().ends_with(": failure: boom\n"), true)?;
        Ok(())
    }
}
