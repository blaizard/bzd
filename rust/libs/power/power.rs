#![no_std]
#![no_main]

/// Design:
/// Coordinate deep sleep for the current target.
/// Light sleep is handled by the platform executor runtime: on ESP it is
/// triggered by the esp-rtos idle hook when no task is runnable.
use bzd::sync::watcher::Watcher;
use core::future::Future;
use core::sync::atomic::{AtomicU32, AtomicUsize, Ordering};
use core::time::Duration;

/// Maximum number of executors that can wait for deep sleep.
const MAX_WAITERS: usize = 4;

const NO_REQUEST: u32 = 0;
const INDEFINITE_REQUEST: u32 = 1;
const DURATION_OFFSET: u32 = 2;
const MAX_DURATION_MILLIS: u32 = u32::MAX - DURATION_OFFSET;

/// A deep-sleep request made by an executor or component.
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct DeepSleepRequest {
    duration: Option<Duration>,
}

impl DeepSleepRequest {
    /// The requested sleep duration, or `None` to wait for an external wake source.
    #[must_use]
    pub fn duration(&self) -> Option<Duration> {
        self.duration
    }
}

/// Shared power state for the current target.
pub struct Power {
    request: AtomicU32,
    watcher: Watcher<(), MAX_WAITERS>,
}

impl Power {
    #[must_use]
    pub const fn new() -> Self {
        Self {
            request: AtomicU32::new(NO_REQUEST),
            watcher: Watcher::new(),
        }
    }

    /// Request deep sleep. The first request wins.
    pub(crate) fn request_deep_sleep(&self, duration: Option<Duration>) {
        let encoded = encode_request(duration);
        if self
            .request
            .compare_exchange(NO_REQUEST, encoded, Ordering::AcqRel, Ordering::Acquire)
            .is_ok()
        {
            self.watcher.send(());
        }
    }

    /// Get the current deep-sleep request, if any.
    #[must_use]
    pub fn deep_sleep_requested(&self) -> Option<DeepSleepRequest> {
        decode_request(self.request.load(Ordering::Acquire))
    }
}

impl Default for Power {
    fn default() -> Self {
        Self::new()
    }
}

/// Power interface exposed to the executor and components.
#[allow(async_fn_in_trait)] // Power handles are polled locally and do not require Send bounds.
pub trait BzdPowerInterface {
    /// Access the underlying power handle.
    fn power_handle(&self) -> &PowerHandle;

    /// Request deep sleep.
    async fn request_deep_sleep(&self, duration: Option<Duration>) {
        BzdPowerInterface::request_deep_sleep(self.power_handle(), duration).await;
    }

    /// Prevent deep sleep until the future is resolved.
    async fn prevent_deep_sleep<F>(&self, work: F) -> F::Output
    where
        F: Future,
    {
        BzdPowerInterface::prevent_deep_sleep(self.power_handle(), work).await
    }

    /// Whether deep sleep is requested for this handle, taking prevention into account.
    fn is_deep_sleep(&self) -> bool {
        BzdPowerInterface::is_deep_sleep(self.power_handle())
    }

    /// Wait until deep sleep is requested and no prevention is active.
    async fn wait_deep_sleep(&self) {
        self.power_handle().wait_deep_sleep().await;
    }
}

/// Per-consumer power handle.
pub struct PowerHandle {
    power: &'static Power,
    preventing: AtomicUsize,
}

impl PowerHandle {
    /// Create a handle for a specific power state.
    #[must_use]
    pub const fn with_power(power: &'static Power) -> Self {
        Self {
            power,
            preventing: AtomicUsize::new(0),
        }
    }

    /// Create a handle bound to the target-wide power state.
    #[must_use]
    pub const fn new() -> Self {
        Self::with_power(power())
    }
}

impl Default for PowerHandle {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdPowerInterface for PowerHandle {
    fn power_handle(&self) -> &PowerHandle {
        self
    }

    async fn request_deep_sleep(&self, duration: Option<Duration>) {
        self.power.request_deep_sleep(duration);
    }

    async fn prevent_deep_sleep<F>(&self, work: F) -> F::Output
    where
        F: Future,
    {
        self.preventing.fetch_add(1, Ordering::AcqRel);
        let _guard = PreventDeepSleepGuard {
            counter: &self.preventing,
            power: self.power,
        };
        work.await
    }

    fn is_deep_sleep(&self) -> bool {
        self.preventing.load(Ordering::Acquire) == 0 && self.power.deep_sleep_requested().is_some()
    }

    async fn wait_deep_sleep(&self) {
        let mut receiver = self
            .power
            .watcher
            .receiver()
            .expect("power: too many executors waiting for deep sleep");
        loop {
            if self.power.deep_sleep_requested().is_some()
                && self.preventing.load(Ordering::Acquire) == 0
            {
                return;
            }
            receiver.changed().await;
        }
    }
}

struct PreventDeepSleepGuard<'a> {
    counter: &'a AtomicUsize,
    power: &'static Power,
}

impl Drop for PreventDeepSleepGuard<'_> {
    fn drop(&mut self) {
        self.counter.fetch_sub(1, Ordering::AcqRel);
        self.power.watcher.send(());
    }
}

fn encode_request(duration: Option<Duration>) -> u32 {
    match duration {
        None => INDEFINITE_REQUEST,
        Some(duration) => {
            let millis = u32::try_from(duration.as_millis()).unwrap_or(MAX_DURATION_MILLIS);
            millis + DURATION_OFFSET
        }
    }
}

fn decode_request(encoded: u32) -> Option<DeepSleepRequest> {
    match encoded {
        NO_REQUEST => None,
        INDEFINITE_REQUEST => Some(DeepSleepRequest { duration: None }),
        _ => Some(DeepSleepRequest {
            duration: Some(Duration::from_millis(u64::from(encoded - DURATION_OFFSET))),
        }),
    }
}

static POWER: Power = Power::new();

/// Access the shared power state.
///
/// This is intended for the power implementation and platform bootstraps.
/// Components and executors should use [`BzdPowerInterface`] instead.
#[must_use]
pub const fn power() -> &'static Power {
    &POWER
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;
    use core::future::{pending, Future};
    use core::pin::pin;
    use core::task::{Context, Poll, RawWaker, RawWakerVTable, Waker};

    struct TestWrapper(PowerHandle);

    impl BzdPowerInterface for TestWrapper {
        fn power_handle(&self) -> &PowerHandle {
            &self.0
        }
    }

    fn noop_waker() -> Waker {
        fn clone(_: *const ()) -> RawWaker {
            RawWaker::new(core::ptr::null(), &VTABLE)
        }
        fn noop(_: *const ()) {}
        static VTABLE: RawWakerVTable = RawWakerVTable::new(clone, noop, noop, noop);
        unsafe { Waker::from_raw(RawWaker::new(core::ptr::null(), &VTABLE)) }
    }

    #[test]
    fn no_request_by_default() -> TestResult {
        let power = Power::new();
        assert_eq!(power.deep_sleep_requested(), None)?;
        Ok(())
    }

    #[test]
    fn first_request_wins() -> TestResult {
        let power = Power::new();
        power.request_deep_sleep(None);
        power.request_deep_sleep(Some(Duration::from_secs(1)));
        assert_eq!(power.deep_sleep_requested().unwrap().duration(), None)?;
        Ok(())
    }

    #[test]
    fn duration_is_preserved() -> TestResult {
        let power = Power::new();
        power.request_deep_sleep(Some(Duration::from_secs(3)));
        assert_eq!(
            power.deep_sleep_requested().unwrap().duration(),
            Some(Duration::from_secs(3))
        )?;
        Ok(())
    }

    #[test]
    fn duration_is_saturated() -> TestResult {
        let power = Power::new();
        power.request_deep_sleep(Some(Duration::MAX));
        assert_eq!(
            power.deep_sleep_requested().unwrap().duration(),
            Some(Duration::from_millis(u64::from(MAX_DURATION_MILLIS)))
        )?;
        Ok(())
    }

    #[test]
    async fn handle_wait_deep_sleep_returns_existing_request() -> TestResult {
        static POWER: Power = Power::new();
        POWER.request_deep_sleep(None);
        let handle = PowerHandle::with_power(&POWER);
        handle.wait_deep_sleep().await;
        Ok(())
    }

    #[test]
    async fn handle_requests_deep_sleep() -> TestResult {
        static POWER: Power = Power::new();
        let handle = PowerHandle::with_power(&POWER);
        handle
            .request_deep_sleep(Some(Duration::from_secs(2)))
            .await;
        assert_eq!(
            POWER.deep_sleep_requested().unwrap().duration(),
            Some(Duration::from_secs(2))
        )?;
        Ok(())
    }

    #[test]
    async fn prevention_hides_deep_sleep() -> TestResult {
        static POWER: Power = Power::new();
        POWER.request_deep_sleep(None);
        let handle = PowerHandle::with_power(&POWER);
        handle
            .prevent_deep_sleep(async { assert_eq!(handle.is_deep_sleep(), false) })
            .await?;
        assert_eq!(handle.is_deep_sleep(), true)?;
        Ok(())
    }

    #[test]
    async fn prevention_can_be_nested() -> TestResult {
        static POWER: Power = Power::new();
        POWER.request_deep_sleep(None);
        let handle = PowerHandle::with_power(&POWER);
        handle
            .prevent_deep_sleep(async {
                handle
                    .prevent_deep_sleep(async { assert_eq!(handle.is_deep_sleep(), false) })
                    .await?;
                assert_eq!(handle.is_deep_sleep(), false)
            })
            .await?;
        assert_eq!(handle.is_deep_sleep(), true)?;
        Ok(())
    }

    #[test]
    async fn prevention_is_local_to_handle() -> TestResult {
        static POWER: Power = Power::new();
        POWER.request_deep_sleep(None);
        let preventing = PowerHandle::with_power(&POWER);
        let other = PowerHandle::with_power(&POWER);
        preventing
            .prevent_deep_sleep(async {
                assert_eq!(preventing.is_deep_sleep(), false)?;
                assert_eq!(other.is_deep_sleep(), true)
            })
            .await?;
        Ok(())
    }

    #[test]
    fn prevention_is_released_when_dropped() -> TestResult {
        static POWER: Power = Power::new();
        POWER.request_deep_sleep(None);
        let handle = PowerHandle::with_power(&POWER);
        let waker = noop_waker();
        let mut context = Context::from_waker(&waker);
        {
            let mut future = pin!(handle.prevent_deep_sleep(pending::<()>()));
            assert_eq!(future.as_mut().poll(&mut context), Poll::Pending)?;
            assert_eq!(handle.is_deep_sleep(), false)?;
        }
        assert_eq!(handle.is_deep_sleep(), true)?;
        Ok(())
    }

    #[test]
    async fn defaults_delegate_to_power_handle() -> TestResult {
        static POWER: Power = Power::new();
        let wrapper = TestWrapper(PowerHandle::with_power(&POWER));
        wrapper
            .request_deep_sleep(Some(Duration::from_secs(4)))
            .await;
        assert_eq!(
            POWER.deep_sleep_requested().unwrap().duration(),
            Some(Duration::from_secs(4))
        )?;
        wrapper
            .prevent_deep_sleep(async {
                assert_eq!(wrapper.is_deep_sleep(), false)?;
                Ok(())
            })
            .await?;
        assert_eq!(wrapper.is_deep_sleep(), true)?;
        Ok(())
    }
}
