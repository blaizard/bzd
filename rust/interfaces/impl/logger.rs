#![no_std]
#![allow(async_fn_in_trait)]

use bzd::base::error::Error;
use interfaces_stream::BzdOStreamInterface;

/// One log event, including its source and lazy formatter message.
pub struct LogRecord<L, M: LogMessage> {
    pub level: L,
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
    type Level;

    async fn log<M: LogMessage>(&mut self, record: LogRecord<Self::Level, M>) -> Result<(), Error>;
}
