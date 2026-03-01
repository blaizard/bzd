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

pub mod public {
    pub use crate::assert_eq;
    pub use crate::types::{TestError, TestResult};
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
