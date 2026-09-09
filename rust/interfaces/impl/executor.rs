#![no_std]
#![no_main]

use core::future::Future;

#[allow(async_fn_in_trait)] // Executors are polled locally and do not require Send bounds.
pub trait Executor: Sized {
    /// Register a workload.
    fn push_workload<F>(self, future: F) -> impl Executor
    where
        F: Future<Output = i32>;

    /// Register a service.
    fn push_service<F>(self, future: F) -> impl Executor
    where
        F: Future<Output = i32>;

    /// Run untl completion and return the status.
    async fn join(self) -> i32;
}
