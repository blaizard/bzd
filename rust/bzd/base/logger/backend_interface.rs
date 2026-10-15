use bzd_stream_impl::BzdOStreamInterface;
use error::Error;

/// Log level, ordered by increasing verbosity.
#[repr(u8)]
#[derive(Copy, Clone, Debug, PartialEq, Eq, PartialOrd, Ord)]
pub enum Level {
    Error = 0,
    Warning = 1,
    Info = 2,
    Debug = 3,
}

impl Level {
    /// Return the short string representation of the level.
    pub fn as_str(self) -> &'static str {
        match self {
            Level::Error => "[e]",
            Level::Warning => "[w]",
            Level::Info => "[i]",
            Level::Debug => "[d]",
        }
    }
}

/// One log event, including its source and lazy formatter message.
pub struct LogRecord<M: LogMessage> {
    pub level: Level,
    pub file: &'static str,
    pub line: u32,
    pub message: M,
}

/// A log message that writes itself to provided stream.
#[allow(async_fn_in_trait)]
pub trait LogMessage {
    async fn write<S: BzdOStreamInterface>(&self, out: &mut S) -> Result<(), Error>;
}

/// A backend that decides how a log event is formatted and emitted.
#[allow(async_fn_in_trait)]
pub trait LogBackend {
    async fn log<M: LogMessage>(&mut self, record: LogRecord<M>) -> Result<(), Error>;
}
