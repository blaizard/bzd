#![no_std]

mod log_value;

use bzd::base::error::Error;
pub use bzd::base::stream::OStream;

pub use log_macro::{log_debug, log_error, log_info, log_warning};
pub use log_value::LogValue;

use rust_interfaces_logger::{BzdLoggerBackendInterface, LogMessage, LogRecord};

/// Frontend that dispatches log events to a backend.
pub struct Logger<'a, B: BzdLoggerBackendInterface> {
    backend: &'a mut B,
}

impl<'a, B: BzdLoggerBackendInterface> Logger<'a, B> {
    /// Create a new logger.
    pub fn new(backend: &'a mut B) -> Self {
        Self { backend }
    }

    /// Dispatch an event to the backend.
    pub async fn log<M: LogMessage>(&mut self, record: LogRecord<M>) -> Result<(), Error> {
        self.backend.log(record).await
    }
}
