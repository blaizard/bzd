#![no_std]

use bzd::base::error::Error;
use interfaces_stream::BzdOStreamInterface;
use logger::LogValue;
pub use rust_components_generic_logger_backend_ostream_interface::{
    BzdComponentsGenericOStreamLoggerBackendContext,
    BzdComponentsGenericOStreamLoggerBackendContextConstraint,
    BzdComponentsGenericOStreamLoggerBackendContextConstraintTypes,
    BzdComponentsGenericOStreamLoggerBackendInterface,
};
use rust_interfaces_logger::{BzdLoggerBackendInterface, BzdLoggerLevel, LogMessage, LogRecord};

/// A logger writing formatted messages to a shared stream sink.
pub struct BzdOStreamBackend<C: BzdComponentsGenericOStreamLoggerBackendContextConstraint> {
    context: BzdComponentsGenericOStreamLoggerBackendContext<C>,
}

pub type BzdComponentsGenericOStreamLoggerBackend<C> = BzdOStreamBackend<C>;

impl<C: BzdComponentsGenericOStreamLoggerBackendContextConstraint> BzdOStreamBackend<C> {
    /// Create a new logger.
    pub fn new(context: BzdComponentsGenericOStreamLoggerBackendContext<C>) -> Self {
        Self { context }
    }

    /// Set the minimum level to be logged.
    pub fn set_minimum_level(&mut self, level: BzdLoggerLevel) {
        self.context.min_level = level;
    }

    /// Whether the given level is currently logged.
    fn is_enabled(&self, level: BzdLoggerLevel) -> bool {
        level_priority(level) <= level_priority(self.context.min_level)
    }
}

fn level_prefix(level: BzdLoggerLevel) -> &'static [u8] {
    match level {
        BzdLoggerLevel::Error => b"[e]",
        BzdLoggerLevel::Warning => b"[w]",
        BzdLoggerLevel::Info => b"[i]",
        BzdLoggerLevel::Debug => b"[d]",
    }
}

fn level_priority(level: BzdLoggerLevel) -> u8 {
    match level {
        BzdLoggerLevel::Error => 0,
        BzdLoggerLevel::Warning => 1,
        BzdLoggerLevel::Info => 2,
        BzdLoggerLevel::Debug => 3,
    }
}

impl<C: BzdComponentsGenericOStreamLoggerBackendContextConstraint> BzdLoggerBackendInterface
    for BzdOStreamBackend<C>
{
    type Level = BzdLoggerLevel;

    async fn log<M: LogMessage>(&mut self, record: LogRecord<Self::Level, M>) -> Result<(), Error> {
        if !self.is_enabled(record.level) {
            return Ok(());
        }
        let mut guard = self.context.out.lock().await;
        let name = record
            .file
            .rsplit_once('/')
            .map_or(record.file, |(_, name)| name);
        guard.write(level_prefix(record.level)).await?;
        guard.write(b" [").await?;
        guard.write(name.as_bytes()).await?;
        guard.write(b":").await?;
        record.line.write(&mut *guard).await?;
        guard.write(b"] ").await?;
        record.message.write(&mut *guard).await?;
        guard.write(b"\n").await?;
        Ok(())
    }
}

impl<C: BzdComponentsGenericOStreamLoggerBackendContextConstraint>
    BzdComponentsGenericOStreamLoggerBackendInterface for BzdOStreamBackend<C>
{
}
