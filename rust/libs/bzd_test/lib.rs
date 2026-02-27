#![no_std]

pub use proc_macro::test;

pub mod tests {

    #[repr(C)]
    // Force the linker to start the struct on a 4-byte boundary, which is important on some architectures.
    #[repr(align(4))]
    pub struct TestMetadata {
        pub name: &'static str,
        pub test_fn: fn(),
        pub ignore: bool,
    }

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

#[unsafe(no_mangle)]
pub fn run_executor() -> bool {
    let tests = tests::get_tests();

    for test in tests {
        if test.ignore {
            continue;
        }
        (test.test_fn)();
    }

    true
}
