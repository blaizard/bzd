#![no_std]

mod assertions;
mod types;

pub use proc_macro::test;

pub mod tests {
    pub use crate::types::TestMetadata;

    #[allow(improper_ctypes)]
    unsafe extern "C" {
        static __start_bzd_test_data: TestMetadata;
        static __stop_bzd_test_data: TestMetadata;
    }

    pub fn get_tests() -> &'static [TestMetadata] {
        // We do the unsafe work inside, and return a safe, immutable slice
        unsafe {
            let start = core::ptr::addr_of!(__start_bzd_test_data);
            let stop = core::ptr::addr_of!(__stop_bzd_test_data);
            let count = (stop as usize - start as usize) / core::mem::size_of::<TestMetadata>();
            core::slice::from_raw_parts(start, count)
        }
    }
}

/// Create a failure error at the caller's location for use in `assert_eq!`.
#[doc(hidden)]
#[track_caller]
pub fn assert_failed(message: &'static str) -> bzd::base::error::Error {
    bzd::base::error::failure(message)
}

#[doc(hidden)]
pub fn block_on<F: core::future::Future>(future: F) -> F::Output {
    use core::task::{Context, Poll, RawWaker, RawWakerVTable, Waker};

    fn clone(_: *const ()) -> RawWaker {
        RawWaker::new(core::ptr::null(), &VTABLE)
    }
    fn noop(_: *const ()) {}
    static VTABLE: RawWakerVTable = RawWakerVTable::new(clone, noop, noop, noop);

    let mut future = core::pin::pin!(future);
    // Safety: The waker never dereferences its data pointer.
    let waker = unsafe { Waker::from_raw(RawWaker::new(core::ptr::null(), &VTABLE)) };
    let mut context = Context::from_waker(&waker);
    loop {
        if let Poll::Ready(result) = future.as_mut().poll(&mut context) {
            return result;
        }
    }
}

pub mod public {
    pub use crate::assert_eq;
    pub use crate::types::TestResult;
}

#[unsafe(no_mangle)]
pub fn run_executor() -> bool {
    let tests = tests::get_tests();

    for test in tests {
        if test.ignore {
            continue;
        }
        let result = (test.test_fn)();
        if let Err(err) = result {
            panic!("{}", err);
        }
    }

    true
}
