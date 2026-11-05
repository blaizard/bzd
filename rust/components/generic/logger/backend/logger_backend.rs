#![no_std]
#![no_main]

use bzd::base::error::Error;
use bzd::sync::signal::Signal;
use logger::Logger;
use ring_buffer::RingBuffer;
pub use rust_components_generic_logger_backend_interface::{
    BzdComponentsGenericLoggerBackendContext, BzdComponentsGenericLoggerBackendContextConstraint,
    BzdComponentsGenericLoggerBackendContextConstraintTypes,
    BzdComponentsGenericLoggerBackendInterface,
};
use rust_interfaces_logger::{BzdLoggerBackendInterface, BzdLoggerLevel, BzdLoggerSinkInterface};

const RING_SIZE: usize = 1024;

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

impl BzdLoggerBackendInterface for Shared {
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
    pub fn logger(&self) -> Logger {
        Logger::new(
            &self.shared as *const dyn BzdLoggerBackendInterface,
            self.context.min_level,
        )
    }

    /// Drain all queued records through the sink.
    pub async fn flush(&mut self) -> Result<(), Error> {
        let mut sink = self.context.sink.lock().await;
        drain(&self.shared, &mut *sink).await?;
        sink.flush().await
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
        self.shared.push(level, write)
    }

    fn is_consumed(&self, target: usize) -> bool {
        self.shared.is_consumed(target)
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
    use logger::{log_debug, log_error, log_info, log_warning};

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
        let mut front = backend.logger();
        log_info!(front, "hello {}", 42)?;
        log_debug!(front, "hidden")?;
        backend.flush().await?;

        let guard = SINK.lock().await;
        assert_eq!(guard.as_str().contains("] hello 42\n"), true)?;
        assert_eq!(guard.flush_count, 1)?;
        Ok(())
    }

    #[test]
    async fn test_minimum_level() -> TestResult {
        *SINK.lock().await = RecordingSink::new();
        let mut backend = backend(BzdLoggerLevel::Error);
        let mut front = backend.logger();
        log_error!(front, "shown")?;
        log_warning!(front, "hidden")?;
        backend.flush().await?;

        let guard = SINK.lock().await;
        assert_eq!(guard.as_str().contains("] shown\n"), true)?;
        assert_eq!(guard.as_str().contains("hidden"), false)?;
        Ok(())
    }

    #[test]
    async fn test_flush() -> TestResult {
        *SINK.lock().await = RecordingSink::new();
        let mut backend = backend(BzdLoggerLevel::Info);
        let mut front = backend.logger();
        log_info!(front, "pending")?;
        backend.flush().await?;

        let guard = SINK.lock().await;
        assert_eq!(guard.as_str().contains("] pending\n"), true)?;
        Ok(())
    }
}
