#![no_std]

use embassy_sync::blocking_mutex::raw::CriticalSectionRawMutex;
use embassy_sync::mutex::{Mutex as EmbassyMutex, MutexGuard as EmbassyMutexGuard};

pub struct Mutex<T: ?Sized> {
    inner: EmbassyMutex<CriticalSectionRawMutex, T>,
}

pub type MutexGuard<'a, T> = EmbassyMutexGuard<'a, CriticalSectionRawMutex, T>;

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
