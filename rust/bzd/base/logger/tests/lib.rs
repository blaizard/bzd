#![no_std]
#![no_main]

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use bzd::base::error::{self, Error};
    use bzd::base::logger::backend::BzdOStreamBackend;
    use bzd::base::logger::backend_interface::Level;
    use bzd::base::logger::{log_debug, log_error, log_info, log_warning, Logger};
    use bzd::sync::mutex::Mutex;
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

    #[test]
    async fn test_logging() -> TestResult {
        let sink = Mutex::new(RecordingSink::new());
        {
            let mut logger = Logger::new(BzdOStreamBackend::new(&sink));
            log_info!(logger, "count = {}, name = {}", 42, "Alex").await?;
            log_warning!(logger, "careful").await?;
            log_error!(logger, "boom").await?;
            log_debug!(logger, "hidden").await?;
        }
        let guard = sink.lock().await;
        let message = guard.as_str();
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
    async fn test_error_value() -> TestResult {
        let sink = Mutex::new(RecordingSink::new());
        {
            let mut logger = Logger::new(BzdOStreamBackend::new(&sink));
            let error = error::failure("boom");
            log_error!(logger, "{}", error).await?;
        }
        let guard = sink.lock().await;
        let message = guard.as_str();
        assert_eq!(message.ends_with(": failure: boom\n"), true)?;
        Ok(())
    }

    #[test]
    async fn test_long_message() -> TestResult {
        let sink = Mutex::new(RecordingSink::new());
        {
            let mut logger = Logger::new(BzdOStreamBackend::new(&sink));
            let long = core::str::from_utf8(&[b'x'; 600]).unwrap();
            log_info!(logger, "{}", long).await?;
        }
        let guard = sink.lock().await;
        let message = guard.as_str();
        assert_eq!(message.matches('x').count(), 600)?;
        assert_eq!(message.ends_with("\n"), true)?;
        Ok(())
    }

    #[test]
    async fn test_minimum_level_filtering() -> TestResult {
        let sink = Mutex::new(RecordingSink::new());
        {
            let mut backend = BzdOStreamBackend::new(&sink);
            backend.set_minimum_level(Level::Error);
            let mut logger = Logger::new(backend);
            log_error!(logger, "shown").await?;
            log_warning!(logger, "hidden").await?;
            log_info!(logger, "hidden").await?;
        }
        let guard = sink.lock().await;
        let message = guard.as_str();
        assert_eq!(message.contains("] shown\n"), true)?;
        assert_eq!(message.contains("hidden"), false)?;
        Ok(())
    }

    #[test]
    async fn test_location() -> TestResult {
        let sink = Mutex::new(RecordingSink::new());
        {
            let mut logger = Logger::new(BzdOStreamBackend::new(&sink));
            log_info!(logger, "hello").await?;
        }
        let guard = sink.lock().await;
        let message = guard.as_str();
        let name = file!().rsplit_once('/').map_or(file!(), |(_, name)| name);
        assert_eq!(message.contains(name), true)?;
        Ok(())
    }
}
