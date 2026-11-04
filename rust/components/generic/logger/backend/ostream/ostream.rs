#![no_std]

use bzd::base::error::Error;
use bzd::base::stream::OStream;
use ring_buffer::RingBuffer;
pub use rust_components_generic_logger_backend_ostream_interface::{
    BzdComponentsGenericOStreamLoggerBackendContext,
    BzdComponentsGenericOStreamLoggerBackendContextConstraint,
    BzdComponentsGenericOStreamLoggerBackendContextConstraintTypes,
    BzdComponentsGenericOStreamLoggerBackendInterface,
};
use rust_interfaces_logger::{BzdLoggerBackendInterface, BzdLoggerLevel};

const RING_SIZE: usize = 1024;

/// A logger writing serialized records to a shared stream sink.
pub struct BzdComponentsGenericOStreamLoggerBackend<
    C: BzdComponentsGenericOStreamLoggerBackendContextConstraint,
> {
    context: BzdComponentsGenericOStreamLoggerBackendContext<C>,
    ring: RingBuffer<RING_SIZE>,
}

impl<C: BzdComponentsGenericOStreamLoggerBackendContextConstraint>
    BzdComponentsGenericOStreamLoggerBackend<C>
{
    /// Create a new logger.
    pub fn new(context: BzdComponentsGenericOStreamLoggerBackendContext<C>) -> Self {
        Self {
            context,
            ring: RingBuffer::<RING_SIZE>::new(),
        }
    }

    /// Set the minimum level to be logged.
    pub fn set_minimum_level(&mut self, level: BzdLoggerLevel) {
        self.context.min_level = level;
    }
}

fn level_priority(level: BzdLoggerLevel) -> u8 {
    match level {
        BzdLoggerLevel::Error => 0,
        BzdLoggerLevel::Warning => 1,
        BzdLoggerLevel::Info => 2,
        BzdLoggerLevel::Debug => 3,
    }
}

impl<C: BzdComponentsGenericOStreamLoggerBackendContextConstraint> BzdLoggerBackendInterface
    for BzdComponentsGenericOStreamLoggerBackend<C>
{
    fn push<F>(&mut self, level: BzdLoggerLevel, write: F) -> Result<(), Error>
    where
        F: FnOnce(&mut dyn core::fmt::Write) -> core::fmt::Result,
    {
        if level_priority(level) > level_priority(self.context.min_level) {
            return Ok(());
        }
        self.ring.push(write)
    }

    async fn flush(&mut self) -> Result<(), Error> {
        let output = &mut self.context.out;
        let ring = &mut self.ring;
        let mut guard = output.lock().await;
        while let Some((first, second)) = ring.pop() {
            guard.write_all(first).await?;
            guard.write_all(second).await?;
        }
        guard.flush().await?;
        Ok(())
    }
}

impl<C: BzdComponentsGenericOStreamLoggerBackendContextConstraint>
    BzdComponentsGenericOStreamLoggerBackendInterface
    for BzdComponentsGenericOStreamLoggerBackend<C>
{
}
