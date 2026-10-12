#![no_std]

use embassy_sync::blocking_mutex::raw::NoopRawMutex;
use embassy_sync::mutex::{Mutex as EmbassyMutex, MutexGuard as EmbassyMutexGuard};

pub struct Mutex<T: ?Sized> {
    inner: EmbassyMutex<NoopRawMutex, T>,
}

pub type MutexGuard<'a, T> = EmbassyMutexGuard<'a, NoopRawMutex, T>;

impl<T> Mutex<T> {
    pub const fn new(value: T) -> Self {
        Self {
            inner: EmbassyMutex::new(value),
        }
    }
}

impl<T: ?Sized> Mutex<T> {
    pub async fn lock(&self) -> MutexGuard<'_, T> {
        self.inner.lock().await
    }
}
