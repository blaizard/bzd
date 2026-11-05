#![no_std]

use bzd::base::error::Error;

pub use rust_interfaces_logger_level::BzdLoggerLevel;

/// A backend that owns the byte ring and is drained by a separate service.
pub trait BzdLoggerBackendInterface {
    /// Write one complete record into the backend ring.
    ///
    /// Returns the index of the enqueued record, or `None` when the record was
    /// dropped because the ring was full.
    fn push(
        &self,
        level: BzdLoggerLevel,
        write: &mut dyn FnMut(&mut dyn core::fmt::Write) -> core::fmt::Result,
    ) -> Result<Option<usize>, Error>;

    /// Whether the record up to `target` has been consumed.
    fn is_consumed(&self, target: usize) -> bool;
}
