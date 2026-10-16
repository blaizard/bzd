#![no_std]
#![no_main]

use bzd::sync::mutex::Mutex;
use core::future::Future;
use rust_interfaces_logger::BzdLoggerBackendInterface;

#[allow(async_fn_in_trait)] // Executors are polled locally and do not require Send bounds.
pub trait BzdExecutorInterface: Sized {
    type LoggerBackend: BzdLoggerBackendInterface + 'static;

    /// The logger backend associated with this executor.
    fn logger_backend(&self) -> &'static Mutex<Self::LoggerBackend>;

    /// Register a workload.
    fn push_workload<F>(
        self,
        future: F,
    ) -> impl BzdExecutorInterface<LoggerBackend = Self::LoggerBackend>
    where
        F: Future<Output = i32>;

    /// Register a service.
    fn push_service<F>(
        self,
        future: F,
    ) -> impl BzdExecutorInterface<LoggerBackend = Self::LoggerBackend>
    where
        F: Future<Output = i32>;

    /// Run until completion and return the status.
    async fn join(self) -> i32;
}
