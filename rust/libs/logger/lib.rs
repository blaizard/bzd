#![no_std]

use bzd::base::error::Error;

pub use rust_interfaces_logger::{BzdLoggerBackendInterface, BzdLoggerLevel, LogRecord};

/// Frontend that dispatches log events to a backend.
pub struct Logger<'a, B: BzdLoggerBackendInterface> {
    backend: &'a mut B,
}

impl<'a, B: BzdLoggerBackendInterface> Logger<'a, B> {
    /// Create a new logger.
    pub fn new(backend: &'a mut B) -> Self {
        Self { backend }
    }

    /// Dispatch an event to the backend.
    pub async fn log(&mut self, record: LogRecord<'_>) -> Result<(), Error> {
        self.backend.log(record).await
    }
}

/// Log a message at the given level.
#[doc(hidden)]
#[macro_export]
macro_rules! __bzd_log {
    ($logger:expr, $level:expr, $($format:tt)*) => {
        async {
            ($logger).log($crate::LogRecord {
                level: $level,
                file: ::core::file!(),
                line: ::core::line!(),
                message: ::core::format_args!($($format)*),
            }).await
        }
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
