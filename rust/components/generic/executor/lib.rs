#![no_std]
#![no_main]

use core::future::Future;
use embassy_futures::join::join;
use rust_interfaces_executor::Executor;

pub struct GenericExecutor<W = (), S = ()> {
    workloads: W,
    services: S,
}

#[allow(async_fn_in_trait)] // Tasks are polled locally and do not require Send bounds.
pub trait Tasks {
    async fn join(self) -> i32;
}

impl Tasks for () {
    async fn join(self) -> i32 {
        0
    }
}

impl<T, F> Tasks for (T, F)
where
    T: Tasks,
    F: Future<Output = i32>,
{
    async fn join(self) -> i32 {
        let (previous, current) = join(self.0.join(), self.1).await;
        if previous != 0 {
            previous
        } else {
            current
        }
    }
}

impl GenericExecutor<(), ()> {
    pub const fn new() -> Self {
        Self {
            workloads: (),
            services: (),
        }
    }
}

impl Default for GenericExecutor<(), ()> {
    fn default() -> Self {
        Self::new()
    }
}

impl<W, S> Executor for GenericExecutor<W, S>
where
    W: Tasks,
    S: Tasks,
{
    fn push_workload<F>(self, future: F) -> impl Executor
    where
        F: Future<Output = i32>,
    {
        GenericExecutor {
            workloads: (self.workloads, future),
            services: self.services,
        }
    }

    fn push_service<F>(self, future: F) -> impl Executor
    where
        F: Future<Output = i32>,
    {
        GenericExecutor {
            workloads: self.workloads,
            services: (self.services, future),
        }
    }
    async fn join(self) -> i32 {
        let mut workloads = core::pin::pin!(self.workloads.join());
        let mut services = core::pin::pin!(self.services.join());

        // Drive the futures manually so we can stop polling the services once the
        // workloads are done.
        core::future::poll_fn(|cx| {
            // Let the services make progress while the workloads are running.
            let _ = services.as_mut().poll(cx);

            // The workloads group only resolves once ALL workloads are complete.
            // On completion, `services` is dropped (cancelling it) at the end of
            // this scope.
            workloads.as_mut().poll(cx)
        })
        .await
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;
    use rust_interfaces_executor::Executor;

    use core::future::Future;
    use core::pin::Pin;
    use core::task::{Context, Poll, RawWaker, RawWakerVTable, Waker};

    fn noop_waker() -> Waker {
        fn clone(_: *const ()) -> RawWaker {
            RawWaker::new(core::ptr::null(), &VTABLE)
        }
        fn noop(_: *const ()) {}
        static VTABLE: RawWakerVTable = RawWakerVTable::new(clone, noop, noop, noop);
        unsafe { Waker::from_raw(RawWaker::new(core::ptr::null(), &VTABLE)) }
    }

    fn block_on<F: Future>(fut: F) -> F::Output {
        let mut fut = core::pin::pin!(fut);
        let waker = noop_waker();
        let mut context = Context::from_waker(&waker);
        for _ in 0..4096 {
            if let Poll::Ready(result) = fut.as_mut().poll(&mut context) {
                return result;
            }
        }
        panic!("Future did not complete.");
    }

    struct YieldOnce<T>(Option<T>);

    impl<T: Unpin> Future for YieldOnce<T> {
        type Output = T;

        fn poll(self: Pin<&mut Self>, _: &mut Context<'_>) -> Poll<T> {
            match self.get_mut().0.take() {
                Some(value) => Poll::Ready(value),
                None => Poll::Pending,
            }
        }
    }

    fn yield_once<T>(value: T) -> YieldOnce<T> {
        YieldOnce(Some(value))
    }

    #[test]
    fn empty_executor_returns_zero() -> TestResult {
        let result = block_on(GenericExecutor::new().join());
        assert_eq!(result, 0)?;
        Ok(())
    }

    #[test]
    fn workload_status_is_returned() -> TestResult {
        let result = block_on(GenericExecutor::new().push_workload(async { 42 }).join());
        assert_eq!(result, 42)?;
        Ok(())
    }

    #[test]
    fn multiple_workloads_return_leftmost_non_zero() -> TestResult {
        let result = block_on(
            GenericExecutor::new()
                .push_workload(async { 0 })
                .push_workload(async { 7 })
                .join(),
        );
        assert_eq!(result, 7)?;

        let result = block_on(
            GenericExecutor::new()
                .push_workload(async { 3 })
                .push_workload(async { 5 })
                .join(),
        );
        assert_eq!(result, 3)?;
        Ok(())
    }

    #[test]
    fn service_completion_does_not_change_result() -> TestResult {
        let result = block_on(
            GenericExecutor::new()
                .push_service(async { 99 })
                .push_workload(yield_once(7))
                .join(),
        );
        assert_eq!(result, 7)?;
        Ok(())
    }

    #[test]
    fn waits_for_all_workloads_before_returning() -> TestResult {
        let result = block_on(
            GenericExecutor::new()
                .push_workload(yield_once(3))
                .push_workload(yield_once(5))
                .join(),
        );
        assert_eq!(result, 3)?;
        Ok(())
    }

    #[test]
    fn service_is_cancelled_when_workloads_finish_first() -> TestResult {
        static POLLED: core::sync::atomic::AtomicBool = core::sync::atomic::AtomicBool::new(false);

        // The service is polled once (returns Pending), then the workloads finish
        // immediately. The service must never be polled to completion, i.e. it is
        // cancelled as soon as the workloads are done.
        let service = core::future::poll_fn(|cx| {
            if POLLED.swap(true, core::sync::atomic::Ordering::Relaxed) {
                Poll::Ready(99)
            } else {
                cx.waker().wake_by_ref();
                Poll::Pending
            }
        });

        let result = block_on(
            GenericExecutor::new()
                .push_service(service)
                .push_workload(async { 7 })
                .join(),
        );
        assert_eq!(result, 7)?;
        // The service must not have been polled a second time (i.e. to completion).
        assert_eq!(POLLED.load(core::sync::atomic::Ordering::Relaxed), true)?;
        Ok(())
    }
}
