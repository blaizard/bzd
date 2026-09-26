pub type TestResult = Result<(), bzd::base::error::Error>;

#[repr(C)]
// Force the linker to start the struct on a 4-byte boundary, which is important on some architectures.
#[repr(align(4))]
pub struct TestMetadata {
    pub name: &'static str,
    pub test_fn: fn() -> TestResult,
    pub ignore: bool,
}
