#![no_std]

use embassy_sync::blocking_mutex::raw::CriticalSectionRawMutex;
use embassy_sync::signal::Signal as EmbassySignal;

pub struct Signal<T> {
    inner: EmbassySignal<CriticalSectionRawMutex, T>,
}

impl<T> Signal<T> {
    pub const fn new() -> Self {
        Self {
            inner: EmbassySignal::new(),
        }
    }

    pub fn signal(&self, value: T) {
        self.inner.signal(value)
    }

    pub async fn wait(&self) -> T {
        self.inner.wait().await
    }
}

impl<T> Default for Signal<T> {
    fn default() -> Self {
        Self::new()
    }
}
