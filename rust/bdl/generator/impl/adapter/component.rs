#![no_std]

use core::cell::UnsafeCell;
use core::sync::atomic::{AtomicUsize, Ordering};

use bzd::sync::rwlock::{RwLock, RwLockWriteGuard};

#[allow(async_fn_in_trait)] // Components are initialized locally and do not require Send bounds.
pub trait Lifecycle {
    /// Initialize the component.
    async fn init(&self) -> Result<(), bzd::base::error::Error>;
    /// Shutdown the component.
    async fn shutdown(&self) -> Result<(), bzd::base::error::Error>;
}

pub type ComponentHandle<T> = &'static RwLock<T>;

pub struct StaticComponent<T>(UnsafeCell<Option<T>>);

unsafe impl<T> Sync for StaticComponent<T> {}

impl<T> StaticComponent<T> {
    pub const fn new() -> Self {
        Self(UnsafeCell::new(None))
    }

    pub fn get_or_init(&'static self, init: impl FnOnce() -> T) -> &'static T {
        // Safety: registry initialization is single-threaded and exclusive.
        let cell = unsafe { &mut *self.0.get() };
        &*cell.get_or_insert_with(init)
    }
}

impl<T> Default for StaticComponent<T> {
    fn default() -> Self {
        Self::new()
    }
}

pub struct CompositionComponent<T> {
    component: RwLock<T>,
    ref_count: AtomicUsize,
}

impl<T> CompositionComponent<T> {
    pub const fn new(inner: T) -> Self {
        Self {
            component: RwLock::new(inner),
            ref_count: AtomicUsize::new(0),
        }
    }

    pub async fn acquire<F>(&self, callback: F) -> Result<bool, bzd::base::error::Error>
    where
        F: AsyncFnOnce(&mut T) -> Result<(), bzd::base::error::Error>,
    {
        let mut component = self.component.write().await;
        let is_first = self.ref_count.fetch_add(1, Ordering::Relaxed) == 0;

        if is_first {
            callback(&mut component).await?;
        }

        Ok(is_first)
    }

    pub async fn release<F>(&self, callback: F) -> Result<bool, bzd::base::error::Error>
    where
        F: AsyncFnOnce(&mut T) -> Result<(), bzd::base::error::Error>,
    {
        let mut component = self.component.write().await;
        let is_last = self.ref_count.fetch_sub(1, Ordering::Relaxed) == 1;

        if is_last {
            callback(&mut component).await?;
        }

        Ok(is_last)
    }

    pub async fn write(&self) -> RwLockWriteGuard<'_, T> {
        self.component.write().await
    }

    pub fn handle(&self) -> &RwLock<T> {
        &self.component
    }
}
