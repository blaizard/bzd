#![no_std]

use bzd::base::error::Error;

pub use rust_interfaces_logger::{BzdLoggerBackendInterface, BzdLoggerLevel};

/// Frontend that dispatches log events to a backend.
///
/// Logging is synchronous; use [`Logger::flush`] to wait for queued messages to
/// be emitted.
pub struct Logger<'a, B: BzdLoggerBackendInterface> {
    backend: &'a mut B,
}

impl<'a, B: BzdLoggerBackendInterface> Logger<'a, B> {
    /// Create a new logger.
    pub fn new(backend: &'a mut B) -> Self {
        Self { backend }
    }

    /// Enqueue a log message.
    ///
    /// Returns the logger so a flush can be chained:
    /// `log_info!(logger, "...")?.flush().await?`.
    pub fn log(
        &mut self,
        level: BzdLoggerLevel,
        file: &'static str,
        line: u32,
        args: core::fmt::Arguments<'_>,
    ) -> Result<&mut Self, Error> {
        let name = file.rsplit_once('/').map_or(file, |(_, name)| name);
        self.backend.push(level, |record| {
            write!(record, "{} [{}:{}] ", level_prefix(level), name, line)?;
            record.write_fmt(args)?;
            record.write_str("\n")
        })?;
        Ok(self)
    }

    /// Emit all queued messages.
    pub async fn flush(&mut self) -> Result<(), Error> {
        self.backend.flush().await
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

/// Log a message at the given level.
#[doc(hidden)]
#[macro_export]
macro_rules! __bzd_log {
    ($logger:expr, $level:expr, $($format:tt)*) => {
        ($logger).log(
            $level,
            ::core::file!(),
            ::core::line!(),
            ::core::format_args!($($format)*),
        )
    };
}

/// Log an error message.
#[macro_export]
macro_rules! log_error {
    ($logger:expr, $($format:tt)*) => {
        $crate::__bzd_log!($logger, $crate::BzdLoggerLevel::Error, $($format)*)
    };
}

/// Log a warning message.
#[macro_export]
macro_rules! log_warning {
    ($logger:expr, $($format:tt)*) => {
        $crate::__bzd_log!($logger, $crate::BzdLoggerLevel::Warning, $($format)*)
    };
}

/// Log an informational message.
#[macro_export]
macro_rules! log_info {
    ($logger:expr, $($format:tt)*) => {
        $crate::__bzd_log!($logger, $crate::BzdLoggerLevel::Info, $($format)*)
    };
}

/// Log a debug message.
#[macro_export]
macro_rules! log_debug {
    ($logger:expr, $($format:tt)*) => {
        $crate::__bzd_log!($logger, $crate::BzdLoggerLevel::Debug, $($format)*)
    };
}
