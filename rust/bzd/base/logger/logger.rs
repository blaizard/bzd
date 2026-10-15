#![no_std]

pub mod backend;
pub mod backend_interface;
mod log_value;

use error::Error;

use backend_interface::{LogBackend, LogMessage, LogRecord};
pub use bzd_stream_impl::BzdOStreamInterface;
pub use log_macro::{log_debug, log_error, log_info, log_warning};
pub use log_value::LogValue;

/// Frontend that dispatches log events to a backend.
pub struct Logger<B: LogBackend> {
    backend: B,
}

impl<B: LogBackend> Logger<B> {
    /// Create a new logger.
    pub fn new(backend: B) -> Self {
        Self { backend }
    }

    /// Dispatch an event to the backend.
    pub async fn log<M: LogMessage>(&mut self, record: LogRecord<M>) -> Result<(), Error> {
        self.backend.log(record).await
    }
}
