#![no_std]
#![allow(async_fn_in_trait)]

use bzd::base::error::Error;
use interfaces_stream::BzdOStreamInterface;

pub use rust_interfaces_logger_level::BzdLoggerLevel;

/// One log event, including its source and lazy formatter message.
pub struct LogRecord<M: LogMessage> {
    pub level: BzdLoggerLevel,
    pub file: &'static str,
    pub line: u32,
    pub message: M,
}

/// A log message that writes itself to provided stream.
pub trait LogMessage {
    async fn write<S: BzdOStreamInterface>(&self, out: &mut S) -> Result<(), Error>;
}

/// A backend that decides how a log event is formatted and emitted.
pub trait BzdLoggerBackendInterface {
    async fn log<M: LogMessage>(&mut self, record: LogRecord<M>) -> Result<(), Error>;
}
