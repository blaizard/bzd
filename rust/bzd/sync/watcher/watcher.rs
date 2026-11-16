#![no_std]

use embassy_sync::blocking_mutex::raw::CriticalSectionRawMutex;
use embassy_sync::watch::{Receiver as EmbassyReceiver, Watch as EmbassyWatcher};

/// Multiple-reader notification primitive.
pub struct Watcher<T: Clone, const N: usize> {
    inner: EmbassyWatcher<CriticalSectionRawMutex, T, N>,
}

impl<T: Clone, const N: usize> Watcher<T, N> {
    pub const fn new() -> Self {
        Self {
            inner: EmbassyWatcher::new(),
        }
    }

    pub fn send(&self, value: T) {
        self.inner.sender().send(value);
    }

    pub fn receiver(&self) -> Option<WatcherReceiver<'_, T, N>> {
        self.inner.receiver().map(|inner| WatcherReceiver { inner })
    }
}

impl<T: Clone, const N: usize> Default for Watcher<T, N> {
    fn default() -> Self {
        Self::new()
    }
}

pub struct WatcherReceiver<'a, T: Clone, const N: usize> {
    inner: EmbassyReceiver<'a, CriticalSectionRawMutex, T, N>,
}

impl<T: Clone, const N: usize> WatcherReceiver<'_, T, N> {
    pub async fn changed(&mut self) -> T {
        self.inner.changed().await
    }
}
