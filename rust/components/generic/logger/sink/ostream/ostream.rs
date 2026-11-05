#![no_std]

use bzd::base::error::Error;
use bzd::base::stream::OStream;
pub use rust_components_generic_logger_sink_ostream_interface::{
    BzdComponentsGenericOStreamLoggerSinkContext,
    BzdComponentsGenericOStreamLoggerSinkContextConstraint,
    BzdComponentsGenericOStreamLoggerSinkContextConstraintTypes,
    BzdComponentsGenericOStreamLoggerSinkInterface,
};
use rust_interfaces_logger::BzdLoggerSinkInterface;

/// A logger specialization writing records to a shared stream sink.
pub struct BzdComponentsGenericOStreamLoggerSink<
    C: BzdComponentsGenericOStreamLoggerSinkContextConstraint,
> {
    context: BzdComponentsGenericOStreamLoggerSinkContext<C>,
}

impl<C: BzdComponentsGenericOStreamLoggerSinkContextConstraint>
    BzdComponentsGenericOStreamLoggerSink<C>
{
    /// Create a new logger sink.
    pub fn new(context: BzdComponentsGenericOStreamLoggerSinkContext<C>) -> Self {
        Self { context }
    }
}

impl<C: BzdComponentsGenericOStreamLoggerSinkContextConstraint> BzdLoggerSinkInterface
    for BzdComponentsGenericOStreamLoggerSink<C>
{
    async fn write(&mut self, data: &[u8]) -> Result<(), Error> {
        self.context.out.lock().await.write_all(data).await
    }

    async fn flush(&mut self) -> Result<(), Error> {
        self.context.out.lock().await.flush().await
    }
}

impl<C: BzdComponentsGenericOStreamLoggerSinkContextConstraint>
    BzdComponentsGenericOStreamLoggerSinkInterface for BzdComponentsGenericOStreamLoggerSink<C>
{
}
