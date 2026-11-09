#![no_std]

use bzd::base::error::Error;
use bzd::base::format::write_into;
use bzd::sync::signal::Signal;
use ring_buffer::RingBuffer;
use rust_interfaces_logger::BzdLoggerSinkInterface;

pub use rust_interfaces_logger::{BzdLoggerBackendInterface, BzdLoggerLevel, Logger};

const RING_SIZE: usize = 1024;
const MAX_RECORDS_PER_BATCH: usize = 32;

/// Persistent state of a logger backend.
///
/// Frontends can be created before the backend service starts and keep
/// pushing into the ring until it is drained by the backend.
pub struct LoggerBackendState {
    ring: RingBuffer<RING_SIZE>,
    wake: Signal<()>,
}

impl LoggerBackendState {
    /// Create a new logger backend state.
    pub const fn new() -> Self {
        Self {
            ring: RingBuffer::new(),
            wake: Signal::new(),
        }
    }

    /// Create a frontend handle for the given topic.
    pub fn logger(&self, topic: &'static str, min_level: BzdLoggerLevel) -> Logger {
        Logger::new(
            self as *const dyn BzdLoggerBackendInterface,
            topic,
            min_level,
        )
    }

    /// Drain queued records in batches, report dropped ones, and flush the sink.
    pub async fn flush<S: BzdLoggerSinkInterface + 'static>(
        &self,
        sink: &mut S,
    ) -> Result<(), Error> {
        loop {
            let mut drained = 0;
            while drained < MAX_RECORDS_PER_BATCH {
                let Some((first, second, length)) = self.ring.peek() else {
                    break;
                };

                if !first.is_empty() {
                    sink.write(first).await?;
                }
                if !second.is_empty() {
                    sink.write(second).await?;
                }
                self.ring.commit(length);
                drained += 1;
            }

            let overflowed = self.ring.overflow_count();
            if overflowed > 0 {
                report_overflow(sink, overflowed).await?;
                self.ring.clear_overflow_count(overflowed);
            }

            sink.flush().await?;

            if drained < MAX_RECORDS_PER_BATCH {
                break;
            }
        }
        Ok(())
    }

    /// Wait until a new record is available.
    pub async fn wait(&self) {
        self.wake.wait().await;
    }
}

async fn report_overflow<S: BzdLoggerSinkInterface + 'static>(
    sink: &mut S,
    overflowed: usize,
) -> Result<(), Error> {
    let mut buffer = [0u8; 128];
    let Some(length) = write_into(
        &mut buffer,
        format_args!(
            "[w] [logger] {} message(s) dropped due to overflow\n",
            overflowed
        ),
    ) else {
        return Err(bzd::base::error::failure!(
            "logger: overflow message too long"
        ));
    };

    sink.write(&buffer[..length]).await
}

impl Default for LoggerBackendState {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdLoggerBackendInterface for LoggerBackendState {
    fn push(
        &self,
        _level: BzdLoggerLevel,
        write: &mut dyn FnMut(&mut dyn core::fmt::Write) -> core::fmt::Result,
    ) -> Result<Option<usize>, Error> {
        let before = self.ring.overflow_count();
        let index = self.ring.push(write)?;
        if self.ring.overflow_count() == before {
            self.wake.signal(());
            Ok(Some(index))
        } else {
            Ok(None)
        }
    }

    fn is_consumed(&self, target: usize) -> bool {
        self.ring.is_consumed(target)
    }
}

static LOGGER_BACKEND_STATE: LoggerBackendState = LoggerBackendState::new();

/// Access the single logger backend state shared by all frontends.
pub fn logger_state() -> &'static LoggerBackendState {
    &LOGGER_BACKEND_STATE
}

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
