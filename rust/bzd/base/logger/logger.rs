#![no_std]

mod log_value;

use core::panic::Location;

use bzd_stream_impl::BzdOStreamInterface;
use error::Error;
use log_value::write_unsigned;

pub use log_macro::{log_debug, log_error, log_info, log_warning};
pub use log_value::LogValue;

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

/// A logger writing formatted messages to a stream sink.
///
/// The sink is generic rather than a trait object since the stream interface
/// uses `async fn` which is not object-safe.
pub struct Logger<'a, S: BzdOStreamInterface> {
    sink: &'a mut S,
    min_level: Level,
}

impl<'a, S: BzdOStreamInterface> Logger<'a, S> {
    /// Create a new logger writing to the given sink.
    pub fn new(sink: &'a mut S) -> Self {
        Self {
            sink,
            min_level: Level::Info,
        }
    }

    /// Set the minimum level to be logged.
    pub fn set_minimum_level(&mut self, level: Level) {
        self.min_level = level;
    }

    /// Whether the given level is currently logged.
    pub fn is_enabled(&self, level: Level) -> bool {
        (level as u8) <= (self.min_level as u8)
    }

    /// Write the header prefix of a log entry.
    pub async fn write_header(
        &mut self,
        level: Level,
        location: &'static Location<'static>,
    ) -> Result<(), Error> {
        self.write_str(level.as_str()).await?;
        self.write_str(" [").await?;
        self.write_str(location.file()).await?;
        self.write_str(":").await?;
        write_unsigned(location.line() as u64, self.sink).await?;
        self.write_str("] ").await
    }

    /// Write a string directly to the stream.
    pub async fn write_str(&mut self, s: &str) -> Result<(), Error> {
        self.sink.write(s.as_bytes()).await
    }

    /// Write a value directly to the stream.
    pub async fn write_value<T: LogValue + ?Sized>(&mut self, value: &T) -> Result<(), Error> {
        value.write(self.sink).await
    }
}
