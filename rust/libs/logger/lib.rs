#![no_std]

use bzd::base::error::Error;
use embassy_futures::yield_now;

pub use rust_interfaces_logger::{BzdLoggerBackendInterface, BzdLoggerLevel};

/// Frontend that dispatches log events to a backend.
///
/// Logging is synchronous; use [`Logger::flush`] to wait for queued messages to
/// be emitted.
pub struct Logger {
    queue: *const dyn BzdLoggerBackendInterface,
    min_level: BzdLoggerLevel,
    pending: Option<usize>,
}

impl Logger {
    /// Create a new logger from a backend queue handle.
    pub fn new(queue: *const dyn BzdLoggerBackendInterface, min_level: BzdLoggerLevel) -> Self {
        Self {
            queue,
            min_level,
            pending: None,
        }
    }

    fn queue(&self) -> &dyn BzdLoggerBackendInterface {
        // SAFETY: the queue is owned by a static component, so its address is
        // stable for the program lifetime. The frontend and the backend service
        // run on the same executor thread.
        unsafe { &*self.queue }
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
        if level_priority(level) > level_priority(self.min_level) {
            return Ok(self);
        }
        let name = file.rsplit_once('/').map_or(file, |(_, name)| name);
        let mut write = |record: &mut dyn core::fmt::Write| -> core::fmt::Result {
            write!(record, "{} [{}:{}] ", level_prefix(level), name, line)?;
            record.write_fmt(args)?;
            record.write_str("\n")
        };
        if let Some(index) = self.queue().push(level, &mut write)? {
            self.pending = Some(index);
        }
        Ok(self)
    }

    /// Wait until all queued messages have been consumed.
    pub async fn flush(&mut self) -> Result<(), Error> {
        if let Some(target) = self.pending {
            while !self.queue().is_consumed(target) {
                yield_now().await;
            }
            self.pending = None;
        }
        Ok(())
    }
}

const fn level_priority(level: BzdLoggerLevel) -> u8 {
    match level {
        BzdLoggerLevel::Error => 0,
        BzdLoggerLevel::Warning => 1,
        BzdLoggerLevel::Info => 2,
        BzdLoggerLevel::Debug => 3,
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
