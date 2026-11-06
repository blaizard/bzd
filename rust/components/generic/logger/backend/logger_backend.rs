#![no_std]
#![no_main]

use bzd::base::error::Error;
use logger::{logger_state, BzdLoggerBackendInterface, BzdLoggerLevel, Logger};
pub use rust_components_generic_logger_backend_interface::{
    BzdComponentsGenericLoggerBackendContext, BzdComponentsGenericLoggerBackendContextConstraint,
    BzdComponentsGenericLoggerBackendContextConstraintTypes,
    BzdComponentsGenericLoggerBackendInterface,
};

/// A logger backend draining the global logger state to a specialization.
pub struct BzdComponentsGenericLoggerBackend<C: BzdComponentsGenericLoggerBackendContextConstraint>
{
    context: BzdComponentsGenericLoggerBackendContext<C>,
}

impl<C: BzdComponentsGenericLoggerBackendContextConstraint> BzdComponentsGenericLoggerBackend<C> {
    /// Create a new logger backend.
    pub fn new(context: BzdComponentsGenericLoggerBackendContext<C>) -> Self {
        Self { context }
    }

    /// Create a frontend handle for the given topic.
    pub fn logger(&self, topic: &'static str) -> Logger {
        logger_state().logger(topic, self.context.min_level)
    }

    /// Drain all queued records through the sink.
    pub async fn flush(&mut self) -> Result<(), Error> {
        let mut sink = self.context.sink.lock().await;
        logger_state().flush(&mut *sink).await
    }
}

impl<C: BzdComponentsGenericLoggerBackendContextConstraint> BzdLoggerBackendInterface
    for BzdComponentsGenericLoggerBackend<C>
{
    fn push(
        &self,
        level: BzdLoggerLevel,
        write: &mut dyn FnMut(&mut dyn core::fmt::Write) -> core::fmt::Result,
    ) -> Result<Option<usize>, Error> {
        logger_state().push(level, write)
    }

    fn is_consumed(&self, target: usize) -> bool {
        logger_state().is_consumed(target)
    }
}

impl<C: BzdComponentsGenericLoggerBackendContextConstraint>
    BzdComponentsGenericLoggerBackendInterface for BzdComponentsGenericLoggerBackend<C>
{
    async fn run(&mut self) -> Result<(), Error> {
        loop {
            {
                let mut sink = self.context.sink.lock().await;
                logger_state().flush(&mut *sink).await?;
            }
            logger_state().wait().await;
        }
    }
}
