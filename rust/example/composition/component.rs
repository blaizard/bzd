use core::future::Future;
use core::ops::{Deref, DerefMut};

// 1. Generic Component struct holding your type and async callbacks
pub struct Component<T, FInit, FShutdown> {
    pub instance: T,
    pub on_init: FInit,
    pub on_shutdown: FShutdown,
}

// 2. Wrapper definition
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

// 3. Wrapper impl bounded by async closure traits
impl<T, FInit, FutInit, FShutdown, FutShutdown> Wrapper<Component<T, FInit, FShutdown>>
where
    FInit: FnMut(&mut T) -> FutInit,
    FutInit: Future<Output = Result<(), bzd::base::error::Error>>,
    FShutdown: FnMut(&mut T) -> FutShutdown,
    FutShutdown: Future<Output = Result<(), bzd::base::error::Error>>,
{
    pub async fn init(&mut self) -> Result<(), bzd::base::error::Error> {
        self.ref_count += 1;
        if self.ref_count == 1 {
            (self.inner.on_init)(&mut self.inner.instance).await?;
        }
        Ok(())
    }

    pub async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error> {
        if self.ref_count > 0 {
            self.ref_count -= 1;
            if self.ref_count == 0 {
                (self.inner.on_shutdown)(&mut self.inner.instance).await?;
            }
        }
        Ok(())
    }
}
