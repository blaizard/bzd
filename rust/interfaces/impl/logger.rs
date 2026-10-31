#![no_std]
#![allow(async_fn_in_trait)]

use bzd::base::error::Error;
use core::fmt::Arguments;

pub use rust_interfaces_logger_level::BzdLoggerLevel;

/// One log event, including its source and its lazily formatted message.
pub struct LogRecord<'a> {
    pub level: BzdLoggerLevel,
    pub file: &'static str,
    pub line: u32,
    pub message: Arguments<'a>,
}

/// A backend that decides how a log event is formatted and emitted.
pub trait BzdLoggerBackendInterface {
    async fn log(&mut self, record: LogRecord<'_>) -> Result<(), Error>;
}
