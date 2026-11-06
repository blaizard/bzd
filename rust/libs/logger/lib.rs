#![no_std]

pub use rust_interfaces_logger::{BzdLoggerBackendInterface, BzdLoggerLevel, Logger, LoggerCell};

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
