use core::fmt;

#[derive(Debug)]
pub struct TestError {
    pub file: &'static str,
    pub line: u32,
    pub message: &'static str,
}

pub type TestResult = Result<(), TestError>;

#[repr(C)]
// Force the linker to start the struct on a 4-byte boundary, which is important on some architectures.
#[repr(align(4))]
pub struct TestMetadata {
    pub name: &'static str,
    pub test_fn: fn() -> TestResult,
    pub ignore: bool,
}

impl fmt::Display for TestError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}:{}: {}", self.file, self.line, self.message)
    }
}
