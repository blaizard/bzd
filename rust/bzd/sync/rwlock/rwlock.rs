#![no_std]

use embassy_sync::blocking_mutex::raw::CriticalSectionRawMutex;
use embassy_sync::rwlock::{
    RwLock as EmbassyRwLock, RwLockReadGuard as EmbassyRwLockReadGuard,
    RwLockWriteGuard as EmbassyRwLockWriteGuard,
};

pub struct RwLock<T: ?Sized> {
    inner: EmbassyRwLock<CriticalSectionRawMutex, T>,
}

pub type RwLockReadGuard<'a, T> = EmbassyRwLockReadGuard<'a, CriticalSectionRawMutex, T>;

pub type RwLockWriteGuard<'a, T> = EmbassyRwLockWriteGuard<'a, CriticalSectionRawMutex, T>;

impl<T> RwLock<T> {
    pub const fn new(value: T) -> Self {
        Self {
            inner: EmbassyRwLock::new(value),
        }
    }
}

impl<T: ?Sized> RwLock<T> {
    /// Acquire a shared read lock.
    pub async fn read(&self) -> RwLockReadGuard<'_, T> {
        self.inner.read().await
    }

    /// Acquire an exclusive write lock.
    pub async fn write(&self) -> RwLockWriteGuard<'_, T> {
        self.inner.write().await
    }
}
