use super::backend_interface::{Level, LogBackend, LogMessage, LogRecord};
use super::log_value::write_unsigned;
use bzd_stream_impl::BzdOStreamInterface;
use error::Error;
use mutex::Mutex;

/// A logger writing formatted messages to a shared stream sink.
pub struct BzdOStreamBackend<'a, S: BzdOStreamInterface> {
    sink: &'a Mutex<S>,
    min_level: Level,
}

impl<'a, S: BzdOStreamInterface> BzdOStreamBackend<'a, S> {
    /// Create a new logger writing to the given protected sink.
    pub fn new(sink: &'a Mutex<S>) -> Self {
        Self {
            sink,
            min_level: Level::Info,
        }
    }

    /// Set the minimum level to be logged.
    pub fn set_minimum_level(&mut self, level: Level) {
        self.min_level = level;
    }

    /// Whether the given level is currently logged.
    fn is_enabled(&self, level: Level) -> bool {
        (level as u8) <= (self.min_level as u8)
    }
}

impl<S: BzdOStreamInterface> LogBackend for BzdOStreamBackend<'_, S> {
    async fn log<M: LogMessage>(&mut self, record: LogRecord<M>) -> Result<(), Error> {
        if !self.is_enabled(record.level) {
            return Ok(());
        }
        let mut guard = self.sink.lock().await;
        let name = record
            .file
            .rsplit_once('/')
            .map_or(record.file, |(_, name)| name);
        guard.write(record.level.as_str().as_bytes()).await?;
        guard.write(b" [").await?;
        guard.write(name.as_bytes()).await?;
        guard.write(b":").await?;
        write_unsigned(record.line as u64, &mut *guard).await?;
        guard.write(b"] ").await?;
        record.message.write(&mut *guard).await?;
        guard.write(b"\n").await?;
        Ok(())
    }
}
