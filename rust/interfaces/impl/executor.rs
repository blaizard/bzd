#![no_std]
#![no_main]

use core::future::Future;

#[allow(async_fn_in_trait)] // Executors are polled locally and do not require Send bounds.
pub trait BzdExecutorInterface: Sized {
    /// Register a workload.
    fn push_workload<F>(self, future: F) -> impl BzdExecutorInterface
    where
        F: Future<Output = i32>;

    /// Register a service.
    fn push_service<F>(self, future: F) -> impl BzdExecutorInterface
    where
        F: Future<Output = i32>;

    /// Run until completion and return the status.
    async fn join(self) -> i32;
}
