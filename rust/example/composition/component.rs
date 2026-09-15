#![no_std]

use core::cell::UnsafeCell;
use core::ops::{Deref, DerefMut};

#[allow(async_fn_in_trait)] // Components are initialized locally and do not require Send bounds.
pub trait Lifecycle {
    /// Initialize the component.
    async fn init(&mut self) -> Result<(), bzd::base::error::Error>;
    /// Shutdown the component.
    async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error>;
}

/// Wrapper around a composition entry.
///
/// It reference-counts accesses so that a component shared between multiple
/// entries is only initialized once and shutdown when the last user releases it.
pub struct Wrapper<T> {
    inner: T,
    ref_count: usize,
}

impl<T> Wrapper<T> {
    pub const fn new(inner: T) -> Self {
        Self {
            inner,
            ref_count: 0,
        }
    }
}

impl<T> Deref for Wrapper<T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl<T> DerefMut for Wrapper<T> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.inner
    }
}

impl<T: Lifecycle> Wrapper<T> {
    pub async fn init(&mut self) -> Result<(), bzd::base::error::Error> {
        self.ref_count += 1;
        if self.ref_count == 1 {
            self.inner.init().await?;
        }
        Ok(())
    }

    pub async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error> {
        if self.ref_count > 0 {
            self.ref_count -= 1;
            if self.ref_count == 0 {
                self.inner.shutdown().await?;
            }
        }
        Ok(())
    }
}

// Safety: Only safe in single-threaded environments.
pub struct LocalStatic<T>(UnsafeCell<Option<T>>);
unsafe impl<T> Sync for LocalStatic<T> {}

impl<T> LocalStatic<T> {
    pub const fn new() -> Self {
        Self(UnsafeCell::new(None))
    }

    #[allow(clippy::mut_from_ref)] // Sound because access is exclusive and single-threaded.
    pub fn get_mut_or_init(&self, init: impl FnOnce() -> T) -> &mut T {
        // Safety: The returned mutable reference is exclusive, a new one is only
        // taken once the previous one is no longer in use.
        let cell = unsafe { &mut *self.0.get() };
        cell.get_or_insert_with(init)
    }
}

impl<T> Default for LocalStatic<T> {
    fn default() -> Self {
        Self::new()
    }
}
