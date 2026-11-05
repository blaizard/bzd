#![no_std]
#![no_main]

use bzd::base::error::Error;
use bzd::sync::signal::Signal;
use embassy_futures::yield_now;
use ring_buffer::RingBuffer;
pub use rust_components_generic_logger_backend_interface::{
    BzdComponentsGenericLoggerBackendContext, BzdComponentsGenericLoggerBackendContextConstraint,
    BzdComponentsGenericLoggerBackendContextConstraintTypes,
    BzdComponentsGenericLoggerBackendInterface,
};
use rust_interfaces_logger::{BzdLoggerBackendInterface, BzdLoggerLevel, BzdLoggerSinkInterface};

const RING_SIZE: usize = 1024;

const fn level_priority(level: BzdLoggerLevel) -> u8 {
    match level {
        BzdLoggerLevel::Error => 0,
        BzdLoggerLevel::Warning => 1,
        BzdLoggerLevel::Info => 2,
        BzdLoggerLevel::Debug => 3,
    }
}

struct Shared {
    ring: RingBuffer<RING_SIZE>,
    wake: Signal<()>,
}

impl Shared {
    const fn new() -> Self {
        Self {
            ring: RingBuffer::new(),
            wake: Signal::new(),
        }
    }
}

/// A frontend handle used to enqueue records without taking the backend lock.
pub struct LoggerFront {
    min_level: BzdLoggerLevel,
    shared: *const Shared,
    pending: Option<usize>,
}

impl LoggerFront {
    fn shared(&self) -> &'static Shared {
        // SAFETY: the backend lives in a static component, so its address is
        // stable for the program lifetime. The frontend and the backend service
        // run on the same executor thread.
        unsafe { &*self.shared }
    }
}

impl BzdLoggerBackendInterface for LoggerFront {
    fn push<F>(&mut self, level: BzdLoggerLevel, write: F) -> Result<(), Error>
    where
        F: FnOnce(&mut dyn core::fmt::Write) -> core::fmt::Result,
    {
        if level_priority(level) > level_priority(self.min_level) {
            return Ok(());
        }
        let shared = self.shared();
        self.pending = Some(shared.ring.push(write)?);
        shared.wake.signal(());
        Ok(())
    }

    async fn flush(&mut self) -> Result<(), Error> {
        if let Some(target) = self.pending {
            let shared = self.shared();
            while !shared.ring.is_consumed(target) {
                yield_now().await;
            }
            self.pending = None;
        }
        Ok(())
    }
}

/// A logger backend owning the ring and draining it to a specialization.
pub struct BzdComponentsGenericLoggerBackend<C: BzdComponentsGenericLoggerBackendContextConstraint>
{
    context: BzdComponentsGenericLoggerBackendContext<C>,
    shared: Shared,
}

impl<C: BzdComponentsGenericLoggerBackendContextConstraint> BzdComponentsGenericLoggerBackend<C> {
    /// Create a new logger backend.
    pub fn new(context: BzdComponentsGenericLoggerBackendContext<C>) -> Self {
        Self {
            context,
            shared: Shared::new(),
        }
    }

    /// Create a frontend handle.
    pub fn front(&self) -> LoggerFront {
        LoggerFront {
            min_level: self.context.min_level,
            shared: &self.shared as *const Shared,
            pending: None,
        }
    }
}

impl<C: BzdComponentsGenericLoggerBackendContextConstraint> BzdLoggerBackendInterface
    for BzdComponentsGenericLoggerBackend<C>
{
    fn push<F>(&mut self, level: BzdLoggerLevel, write: F) -> Result<(), Error>
    where
        F: FnOnce(&mut dyn core::fmt::Write) -> core::fmt::Result,
    {
        let mut front = self.front();
        front.push(level, write)
    }

    async fn flush(&mut self) -> Result<(), Error> {
        let mut sink = self.context.sink.lock().await;
        drain(&self.shared, &mut *sink).await?;
        sink.flush().await
    }
}

impl<C: BzdComponentsGenericLoggerBackendContextConstraint>
    BzdComponentsGenericLoggerBackendInterface for BzdComponentsGenericLoggerBackend<C>
{
    async fn run(&mut self) -> Result<(), Error> {
        let shared = &self.shared;
        loop {
            {
                let mut sink = self.context.sink.lock().await;
                drain(shared, &mut *sink).await?;
                sink.flush().await?;
            }
            shared.wake.wait().await;
        }
    }
}

async fn drain<S: BzdLoggerSinkInterface + 'static>(
    shared: &Shared,
    sink: &mut S,
) -> Result<(), Error> {
    while let Some((first, second, length)) = shared.ring.peek() {
        if !first.is_empty() {
            sink.write(first).await?;
        }
        if !second.is_empty() {
            sink.write(second).await?;
        }
        shared.ring.commit(length);
    }
    Ok(())
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;
    use bzd::sync::mutex::Mutex;

    struct RecordingSink {
        data: [u8; 1024],
        length: usize,
        flush_count: usize,
    }

    impl RecordingSink {
        const fn new() -> Self {
            Self {
                data: [0; 1024],
                length: 0,
                flush_count: 0,
            }
        }

        fn as_str(&self) -> &str {
            core::str::from_utf8(&self.data[..self.length]).unwrap()
        }
    }

    impl BzdLoggerSinkInterface for RecordingSink {
        async fn write(&mut self, data: &[u8]) -> Result<(), Error> {
            self.data[self.length..self.length + data.len()].copy_from_slice(data);
            self.length += data.len();
            Ok(())
        }

        async fn flush(&mut self) -> Result<(), Error> {
            self.flush_count += 1;
            Ok(())
        }
    }

    // Tests run sequentially, the sink is reset at the beginning of each test.
    static SINK: Mutex<RecordingSink> = Mutex::new(RecordingSink::new());

    type Backend = BzdComponentsGenericLoggerBackend<
        BzdComponentsGenericLoggerBackendContextConstraintTypes<RecordingSink>,
    >;

    fn backend(min_level: BzdLoggerLevel) -> Backend {
        BzdComponentsGenericLoggerBackend::new(BzdComponentsGenericLoggerBackendContext::new(
            &SINK, min_level,
        ))
    }

    #[test]
    async fn test_drain() -> TestResult {
        *SINK.lock().await = RecordingSink::new();
        let mut backend = backend(BzdLoggerLevel::Info);
        let mut front = backend.front();
        front.push(BzdLoggerLevel::Info, |record| {
            write!(record, "hello {}", 42)
        })?;
        front.push(BzdLoggerLevel::Debug, |record| write!(record, "hidden"))?;
        backend.flush().await?;

        let guard = SINK.lock().await;
        assert_eq!(guard.as_str(), "hello 42")?;
        assert_eq!(guard.flush_count, 1)?;
        Ok(())
    }

    #[test]
    async fn test_minimum_level() -> TestResult {
        *SINK.lock().await = RecordingSink::new();
        let mut backend = backend(BzdLoggerLevel::Error);
        let mut front = backend.front();
        front.push(BzdLoggerLevel::Error, |record| write!(record, "shown"))?;
        front.push(BzdLoggerLevel::Warning, |record| write!(record, "hidden"))?;
        backend.flush().await?;

        let guard = SINK.lock().await;
        assert_eq!(guard.as_str(), "shown")?;
        Ok(())
    }

    #[test]
    async fn test_flush() -> TestResult {
        *SINK.lock().await = RecordingSink::new();
        let mut backend = backend(BzdLoggerLevel::Info);
        let mut front = backend.front();
        front.push(BzdLoggerLevel::Info, |record| write!(record, "pending"))?;
        backend.flush().await?;

        let guard = SINK.lock().await;
        assert_eq!(guard.as_str(), "pending")?;
        Ok(())
    }
}
