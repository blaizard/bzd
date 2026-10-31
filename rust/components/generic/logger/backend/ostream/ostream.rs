#![no_std]

use bzd::base::error::Error;
use bzd::base::format;
use bzd::base::stream::OStream;
pub use rust_components_generic_logger_backend_ostream_interface::{
    BzdComponentsGenericOStreamLoggerBackendContext,
    BzdComponentsGenericOStreamLoggerBackendContextConstraint,
    BzdComponentsGenericOStreamLoggerBackendContextConstraintTypes,
    BzdComponentsGenericOStreamLoggerBackendInterface,
};
use rust_interfaces_logger::{BzdLoggerBackendInterface, BzdLoggerLevel, LogRecord};

/// A logger writing formatted messages to a shared stream sink.
pub struct BzdComponentsGenericOStreamLoggerBackend<
    C: BzdComponentsGenericOStreamLoggerBackendContextConstraint,
> {
    context: BzdComponentsGenericOStreamLoggerBackendContext<C>,
}

impl<C: BzdComponentsGenericOStreamLoggerBackendContextConstraint>
    BzdComponentsGenericOStreamLoggerBackend<C>
{
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

fn level_prefix(level: BzdLoggerLevel) -> &'static str {
    match level {
        BzdLoggerLevel::Error => "[e]",
        BzdLoggerLevel::Warning => "[w]",
        BzdLoggerLevel::Info => "[i]",
        BzdLoggerLevel::Debug => "[d]",
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
    for BzdComponentsGenericOStreamLoggerBackend<C>
{
    async fn log(&mut self, record: LogRecord<'_>) -> Result<(), Error> {
        if !self.is_enabled(record.level) {
            return Ok(());
        }
        let mut guard = self.context.out.lock().await;
        let name = record
            .file
            .rsplit_once('/')
            .map_or(record.file, |(_, name)| name);
        format::write_to(
            &mut *guard,
            format_args!("{} [{}:{}] ", level_prefix(record.level), name, record.line),
        )
        .await?;
        format::write_to(&mut *guard, record.message).await?;
        guard.write_all(b"\n").await?;
        guard.flush().await?;
        Ok(())
    }
}

impl<C: BzdComponentsGenericOStreamLoggerBackendContextConstraint>
    BzdComponentsGenericOStreamLoggerBackendInterface
    for BzdComponentsGenericOStreamLoggerBackend<C>
{
}
