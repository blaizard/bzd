#![no_std]
#![allow(async_fn_in_trait)]

use bzd::base::error::Error;

pub use rust_interfaces_logger_level::BzdLoggerLevel;

/// A backend that owns the byte ring and drains it asynchronously.
pub trait BzdLoggerBackendInterface {
    /// Write one complete record into the backend ring.
    ///
    /// The backend only provides the storage and level filtering; the caller
    /// owns the record formatting.
    fn push<F>(&mut self, level: BzdLoggerLevel, write: F) -> Result<(), Error>
    where
        F: FnOnce(&mut dyn core::fmt::Write) -> core::fmt::Result;

    /// Emit all queued records.
    async fn flush(&mut self) -> Result<(), Error>;
}
