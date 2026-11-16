#![no_std]
#![no_main]

use bzd::sync::mutex::Mutex;
use bzd::sync::watcher::Watcher;
use core::time::Duration;

/// Maximum number of executors that can wait for deep sleep.
const MAX_EXECUTORS: usize = 4;

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
    request: Mutex<Option<DeepSleepRequest>>,
    watcher: Watcher<(), MAX_EXECUTORS>,
}

impl Power {
    #[must_use]
    pub const fn new() -> Self {
        Self {
            request: Mutex::new(None),
            watcher: Watcher::new(),
        }
    }

    /// Request deep sleep. The first request wins.
    pub async fn request_deep_sleep(&self, duration: Option<Duration>) {
        let mut request = self.request.lock().await;
        if request.is_none() {
            *request = Some(DeepSleepRequest { duration });
            self.watcher.send(());
        }
    }

    /// Get the current deep-sleep request, if any.
    #[must_use]
    pub async fn deep_sleep_requested(&self) -> Option<DeepSleepRequest> {
        *self.request.lock().await
    }

    /// Wait until a deep-sleep request is made.
    pub async fn wait_deep_sleep(&self) -> DeepSleepRequest {
        let mut receiver = self
            .watcher
            .receiver()
            .expect("power: too many executors waiting for deep sleep");
        loop {
            if let Some(request) = self.deep_sleep_requested().await {
                return request;
            }
            receiver.changed().await;
        }
    }
}

impl Default for Power {
    fn default() -> Self {
        Self::new()
    }
}

static POWER: Power = Power::new();

/// Access the shared power state.
#[must_use]
pub fn power() -> &'static Power {
    &POWER
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    async fn no_request_by_default() -> TestResult {
        let power = Power::new();
        assert_eq!(power.deep_sleep_requested().await, None)?;
        Ok(())
    }

    #[test]
    async fn first_request_wins() -> TestResult {
        let power = Power::new();
        power.request_deep_sleep(None).await;
        power.request_deep_sleep(Some(Duration::from_secs(1))).await;
        assert_eq!(power.deep_sleep_requested().await.unwrap().duration(), None)?;
        Ok(())
    }

    #[test]
    async fn duration_is_preserved() -> TestResult {
        let power = Power::new();
        power.request_deep_sleep(Some(Duration::from_secs(3))).await;
        assert_eq!(
            power.deep_sleep_requested().await.unwrap().duration(),
            Some(Duration::from_secs(3))
        )?;
        Ok(())
    }
}
