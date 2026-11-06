#![no_std]

use bzd::base::error::Error;
use core::cell::UnsafeCell;
use embassy_futures::yield_now;

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

/// Late-bound logger backend.
///
/// Frontends can be created before the backend is installed and start
/// forwarding once it is.
pub struct LoggerCell {
    backend: UnsafeCell<Option<*const dyn BzdLoggerBackendInterface>>,
}

// SAFETY: the cell is installed and used from the single executor thread.
unsafe impl Sync for LoggerCell {}

impl LoggerCell {
    pub const fn new() -> Self {
        Self {
            backend: UnsafeCell::new(None),
        }
    }

    /// Install the backend associated with this cell.
    ///
    /// # Safety
    ///
    /// The backend must outlive every logger created from this cell.
    pub unsafe fn install(&self, backend: &dyn BzdLoggerBackendInterface) {
        let backend = unsafe {
            core::mem::transmute::<
                &dyn BzdLoggerBackendInterface,
                &'static dyn BzdLoggerBackendInterface,
            >(backend)
        };
        unsafe { *self.backend.get() = Some(backend as *const dyn BzdLoggerBackendInterface) };
    }

    /// Create a frontend associated with this cell.
    pub fn logger(&self, min_level: BzdLoggerLevel) -> Logger {
        Logger::new(self as *const dyn BzdLoggerBackendInterface, min_level)
    }

    fn backend(&self) -> Option<&dyn BzdLoggerBackendInterface> {
        unsafe { (*self.backend.get()).map(|backend| &*backend) }
    }
}

impl BzdLoggerBackendInterface for LoggerCell {
    fn push(
        &self,
        level: BzdLoggerLevel,
        write: &mut dyn FnMut(&mut dyn core::fmt::Write) -> core::fmt::Result,
    ) -> Result<Option<usize>, Error> {
        match self.backend() {
            Some(backend) => backend.push(level, write),
            None => Ok(None),
        }
    }

    fn is_consumed(&self, target: usize) -> bool {
        match self.backend() {
            Some(backend) => backend.is_consumed(target),
            None => true,
        }
    }
}

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
