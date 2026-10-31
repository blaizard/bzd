#![no_std]
#![no_main]

mod arena;

use core::fmt;
use core::panic::Location;

/// Platform agnostic error type.
#[repr(u8)]
#[derive(Copy, Clone, Debug, PartialEq, Eq, Hash)]
pub enum ErrorType {
    /// Any error that does not fall in the below categories.
    Failure,
    /// Happens when a timeout occurs, it could provide information to the handler
    /// that either the timeout was too short or the input was too slow.
    Timeout,
    /// Happens while accessing a busy resource for example,
    /// this type of error notifies the handler that it can be retried if necessary.
    Busy,
    /// Happens when a stream reaches the end of its channel for example.
    Eof,
    /// Error related to data received, it can be due to data corruption, etc. Generally,
    /// reasons that are out of our control.
    Data,
}

impl ErrorType {
    /// Return the error type as a string.
    pub fn as_str(self) -> &'static str {
        match self {
            ErrorType::Failure => "failure",
            ErrorType::Timeout => "timeout",
            ErrorType::Busy => "busy",
            ErrorType::Eof => "eof",
            ErrorType::Data => "data",
        }
    }
}

impl fmt::Display for ErrorType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.write_str(self.as_str())
    }
}

/// Global definition of an error object.
///
/// This is a lightweight data structure that combines the following:
/// - An abstracted information regarding the error type on which
///   the error handler could react.
/// - The error location to trace the error for debugging purpose.
/// - A human readable message that describes details of the actual
///   error, which could include for example, implementation specific information.
#[must_use]
pub struct Error {
    source: &'static str,
    line: u32,
    error_type: ErrorType,
    message: Message,
}

#[derive(Copy, Clone)]
enum Message {
    /// A static message that is not stored in the message pool.
    Static(&'static str),
    /// A formatted message owned in the shared message pool.
    Formatted { slot: u8, length: u16 },
}

impl Message {
    fn as_str(&self) -> &str {
        match self {
            Self::Static(message) => message,
            Self::Formatted { slot, length } => {
                let data = arena::data(*slot as usize);
                core::str::from_utf8(&data[..*length as usize]).unwrap_or("")
            }
        }
    }
}

impl Drop for Error {
    fn drop(&mut self) {
        if let Message::Formatted { slot, .. } = self.message {
            arena::release(slot as usize);
        }
    }
}

impl Error {
    #[track_caller]
    fn new(error_type: ErrorType, message: Message) -> Self {
        let location = Location::caller();
        Self {
            source: location.file(),
            line: location.line(),
            error_type,
            message,
        }
    }

    /// Create a new error with a static message.
    ///
    /// Static messages are not stored in the message pool and are never truncated.
    #[track_caller]
    pub fn with_message(error_type: ErrorType, message: &'static str) -> Self {
        Self::new(error_type, Message::Static(message))
    }

    /// Create a new error with a formatted message.
    ///
    /// A message without format arguments is stored as a static string and does
    /// not use the pool. Formatted messages are stored in a fixed-size pool
    /// shared by all errors: a message that does not fit is truncated and
    /// suffixed with `...`, and if every slot is already in use, a static
    /// placeholder message is used instead.
    #[track_caller]
    pub fn with_format(error_type: ErrorType, args: fmt::Arguments<'_>) -> Self {
        let message = match args.as_str() {
            Some(message) => Message::Static(message),
            None => match arena::SlotGuard::claim() {
                Some(mut guard) => {
                    let length = guard.write(args);
                    Message::Formatted {
                        slot: guard.commit() as u8,
                        length: length as u16,
                    }
                }
                None => Message::Static(arena::MESSAGE_POOL_EXHAUSTED),
            },
        };
        Self::new(error_type, message)
    }

    /// The file where the error was created.
    pub fn file(&self) -> &'static str {
        self.source
    }

    /// The line where the error was created.
    pub fn line(&self) -> u32 {
        self.line
    }

    /// The type of the error.
    pub fn error_type(&self) -> ErrorType {
        self.error_type
    }

    /// The error message.
    pub fn message(&self) -> &str {
        self.message.as_str()
    }
}

/// Create a failure error, with optional formatting.
#[macro_export]
macro_rules! failure {
    ($($arg:tt)*) => {
        $crate::Error::with_format($crate::ErrorType::Failure, ::core::format_args!($($arg)*))
    };
}

/// Create a timeout error, with optional formatting.
#[macro_export]
macro_rules! timeout {
    ($($arg:tt)*) => {
        $crate::Error::with_format($crate::ErrorType::Timeout, ::core::format_args!($($arg)*))
    };
}

/// Create a busy error, with optional formatting.
#[macro_export]
macro_rules! busy {
    ($($arg:tt)*) => {
        $crate::Error::with_format($crate::ErrorType::Busy, ::core::format_args!($($arg)*))
    };
}

/// Create an end-of-channel error, with optional formatting.
#[macro_export]
macro_rules! eof {
    ($($arg:tt)*) => {
        $crate::Error::with_format($crate::ErrorType::Eof, ::core::format_args!($($arg)*))
    };
}

/// Create a data error, with optional formatting.
#[macro_export]
macro_rules! data {
    ($($arg:tt)*) => {
        $crate::Error::with_format($crate::ErrorType::Data, ::core::format_args!($($arg)*))
    };
}

/// Return a failure error if the condition is not satisfied.
///
/// The macro evaluates to `Result<(), Error>`, so it is meant to be used with `?`.
#[macro_export]
macro_rules! ensure {
    ($condition:expr) => {
        if $condition {
            ::core::result::Result::Ok(())
        } else {
            ::core::result::Result::Err($crate::Error::with_message(
                $crate::ErrorType::Failure,
                ::core::concat!("condition failed: ", ::core::stringify!($condition)),
            ))
        }
    };
    ($condition:expr, $($arg:tt)*) => {
        if $condition {
            ::core::result::Result::Ok(())
        } else {
            ::core::result::Result::Err($crate::failure!($($arg)*))
        }
    };
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "{}:{}: {}: {}",
            self.source,
            self.line,
            self.error_type,
            self.message()
        )
    }
}

impl fmt::Debug for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Error")
            .field("file", &self.source)
            .field("line", &self.line)
            .field("error_type", &self.error_type)
            .field("message", &self.message())
            .finish()
    }
}

impl core::error::Error for Error {}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    struct Buffer<const N: usize> {
        data: [u8; N],
        len: usize,
    }

    impl<const N: usize> Buffer<N> {
        fn new() -> Self {
            Self {
                data: [0; N],
                len: 0,
            }
        }

        fn as_str(&self) -> &str {
            core::str::from_utf8(&self.data[..self.len]).unwrap()
        }
    }

    impl<const N: usize> fmt::Write for Buffer<N> {
        fn write_str(&mut self, string: &str) -> fmt::Result {
            let bytes = string.as_bytes();
            if self.len + bytes.len() > N {
                return Err(fmt::Error);
            }
            self.data[self.len..self.len + bytes.len()].copy_from_slice(bytes);
            self.len += bytes.len();
            Ok(())
        }
    }

    struct Repeated {
        text: &'static str,
        count: usize,
    }

    impl fmt::Display for Repeated {
        fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
            for _ in 0..self.count {
                formatter.write_str(self.text)?;
            }
            Ok(())
        }
    }

    fn assert_location(error: &Error) -> TestResult {
        assert_eq!(error.file().is_empty(), false)?;
        assert_eq!(error.line() == 0, false)?;
        Ok(())
    }

    #[test]
    fn test_error_type_as_str() -> TestResult {
        assert_eq!(ErrorType::Failure.as_str(), "failure")?;
        assert_eq!(ErrorType::Timeout.as_str(), "timeout")?;
        assert_eq!(ErrorType::Busy.as_str(), "busy")?;
        assert_eq!(ErrorType::Eof.as_str(), "eof")?;
        assert_eq!(ErrorType::Data.as_str(), "data")?;
        Ok(())
    }

    #[test]
    fn test_error_type_display() -> TestResult {
        let mut buffer = Buffer::<16>::new();
        core::fmt::write(&mut buffer, format_args!("{}", ErrorType::Failure)).unwrap();
        assert_eq!(buffer.as_str(), "failure")?;
        let mut buffer = Buffer::<16>::new();
        core::fmt::write(&mut buffer, format_args!("{}", ErrorType::Timeout)).unwrap();
        assert_eq!(buffer.as_str(), "timeout")?;
        let mut buffer = Buffer::<16>::new();
        core::fmt::write(&mut buffer, format_args!("{}", ErrorType::Busy)).unwrap();
        assert_eq!(buffer.as_str(), "busy")?;
        let mut buffer = Buffer::<16>::new();
        core::fmt::write(&mut buffer, format_args!("{}", ErrorType::Eof)).unwrap();
        assert_eq!(buffer.as_str(), "eof")?;
        let mut buffer = Buffer::<16>::new();
        core::fmt::write(&mut buffer, format_args!("{}", ErrorType::Data)).unwrap();
        assert_eq!(buffer.as_str(), "data")?;
        Ok(())
    }

    #[test]
    fn test_factories_error_type() -> TestResult {
        assert_eq!(crate::failure!("msg").error_type(), ErrorType::Failure)?;
        assert_eq!(crate::timeout!("msg").error_type(), ErrorType::Timeout)?;
        assert_eq!(crate::busy!("msg").error_type(), ErrorType::Busy)?;
        assert_eq!(crate::eof!("msg").error_type(), ErrorType::Eof)?;
        assert_eq!(crate::data!("msg").error_type(), ErrorType::Data)?;
        Ok(())
    }

    #[test]
    fn test_factories_message() -> TestResult {
        {
            let error = crate::failure!("failed");
            assert_eq!(error.message(), "failed")?;
        }
        {
            let error = crate::timeout!("timed out");
            assert_eq!(error.message(), "timed out")?;
        }
        {
            let error = crate::busy!("is busy");
            assert_eq!(error.message(), "is busy")?;
        }
        {
            let error = crate::eof!("end of file");
            assert_eq!(error.message(), "end of file")?;
        }
        {
            let error = crate::data!("bad data");
            assert_eq!(error.message(), "bad data")?;
        }
        Ok(())
    }

    #[test]
    fn test_factories_location() -> TestResult {
        assert_location(&crate::failure!("msg"))?;
        assert_location(&crate::timeout!("msg"))?;
        assert_location(&crate::busy!("msg"))?;
        assert_location(&crate::eof!("msg"))?;
        assert_location(&crate::data!("msg"))?;
        Ok(())
    }

    #[test]
    fn test_ensure_ok() -> TestResult {
        fn check(condition: bool) -> Result<(), crate::Error> {
            crate::ensure!(condition, "condition failed")?;
            Ok(())
        }
        match check(true) {
            Ok(()) => Ok(()),
            Err(error) => panic!("ensure should have passed: {}", error),
        }
    }

    #[test]
    fn test_ensure_failure() -> TestResult {
        let value = 3;
        let error = match crate::ensure!(value == 4, "expected 4, got {value}") {
            Ok(()) => panic!("ensure should have failed"),
            Err(error) => error,
        };
        assert_eq!(error.error_type(), ErrorType::Failure)?;
        assert_eq!(error.message(), "expected 4, got 3")?;
        Ok(())
    }

    #[test]
    fn test_ensure_without_message() -> TestResult {
        let error = match crate::ensure!(1 == 2) {
            Ok(()) => panic!("ensure should have failed"),
            Err(error) => error,
        };
        assert_eq!(error.error_type(), ErrorType::Failure)?;
        assert_eq!(error.message(), "condition failed: 1 == 2")?;
        Ok(())
    }

    #[test]
    fn test_ensure_static_message_skips_pool() -> TestResult {
        let guards: [Option<arena::SlotGuard>; arena::SLOT_COUNT] =
            core::array::from_fn(|_| arena::SlotGuard::claim());
        assert_eq!(guards.iter().all(|guard| guard.is_some()), true)?;
        let error = match crate::ensure!(false, "static message") {
            Ok(()) => panic!("ensure should have failed"),
            Err(error) => error,
        };
        assert_eq!(error.message(), "static message")?;
        Ok(())
    }

    #[test]
    fn test_ensure_location() -> TestResult {
        let line = core::line!() + 1;
        let error = match crate::ensure!(false, "boom") {
            Ok(()) => panic!("ensure should have failed"),
            Err(error) => error,
        };
        assert_eq!(error.line(), line)?;
        Ok(())
    }

    #[test]
    fn test_format_positional() -> TestResult {
        let error = crate::failure!("value is {} and {}", 1, "two");
        assert_eq!(error.message(), "value is 1 and two")?;
        assert_eq!(error.error_type(), ErrorType::Failure)?;
        assert_location(&error)?;
        Ok(())
    }

    #[test]
    fn test_format_captured() -> TestResult {
        let value = 42;
        let error = crate::timeout!("hello {value}");
        assert_eq!(error.message(), "hello 42")?;
        assert_eq!(error.error_type(), ErrorType::Timeout)?;
        Ok(())
    }

    #[test]
    fn test_format_location() -> TestResult {
        let line = core::line!() + 1;
        let error = crate::failure!("msg");
        assert_eq!(error.line(), line)?;
        Ok(())
    }

    #[test]
    fn test_assert_eq_location() -> TestResult {
        let line = core::line!() + 1;
        match ::bzd_test::public::assert_eq!(1, 2) {
            Err(error) => {
                assert_eq!(error.line(), line)?;
                assert_eq!(error.file().is_empty(), false)?;
            }
            Ok(()) => panic!("assert_eq should have failed"),
        }
        Ok(())
    }

    #[test]
    fn test_format_exact_fit() -> TestResult {
        let error = crate::failure!(
            "{}",
            Repeated {
                text: "x",
                count: arena::MESSAGE_CAPACITY,
            }
        );
        assert_eq!(error.message().len(), arena::MESSAGE_CAPACITY)?;
        assert_eq!(error.message().ends_with("..."), false)?;
        Ok(())
    }

    #[test]
    fn test_format_truncated() -> TestResult {
        let error = crate::failure!(
            "{}",
            Repeated {
                text: "x",
                count: arena::MESSAGE_CAPACITY + 1,
            }
        );
        let message = error.message();
        assert_eq!(message.len(), arena::MESSAGE_SIZE)?;
        assert_eq!(message.ends_with("..."), true)?;
        assert_eq!(
            message[..arena::MESSAGE_CAPACITY].chars().all(|c| c == 'x'),
            true
        )?;
        Ok(())
    }

    #[test]
    fn test_format_truncated_multibyte() -> TestResult {
        let error = crate::failure!(
            "{}",
            Repeated {
                text: "é",
                count: 300
            }
        );
        let message = error.message();
        assert_eq!(message.len() <= arena::MESSAGE_SIZE, true)?;
        assert_eq!(message.ends_with("..."), true)?;
        let prefix = message.strip_suffix("...").unwrap();
        assert_eq!(prefix.is_empty(), false)?;
        assert_eq!(prefix.chars().all(|c| c == 'é'), true)?;
        Ok(())
    }

    #[test]
    fn test_slot_reuse() -> TestResult {
        for index in 0..(arena::SLOT_COUNT * 4) {
            let index = core::hint::black_box(index);
            let error = crate::failure!("value {}", index);
            let mut expected = Buffer::<32>::new();
            core::fmt::write(&mut expected, format_args!("value {}", index)).unwrap();
            assert_eq!(error.message(), expected.as_str())?;
        }
        Ok(())
    }

    #[test]
    fn test_message_pool_exhausted() -> TestResult {
        let guards: [Option<arena::SlotGuard>; arena::SLOT_COUNT] =
            core::array::from_fn(|_| arena::SlotGuard::claim());
        assert_eq!(guards.iter().all(|guard| guard.is_some()), true)?;
        assert_eq!(arena::available(), 0)?;
        let value = core::hint::black_box(42);
        assert_eq!(format_args!("hello {}", value).as_str().is_none(), true)?;
        let error = crate::failure!("hello {}", value);
        assert_eq!(error.message(), arena::MESSAGE_POOL_EXHAUSTED)?;
        Ok(())
    }

    #[test]
    fn test_concat_literal_skips_pool() -> TestResult {
        let guards: [Option<arena::SlotGuard>; arena::SLOT_COUNT] =
            core::array::from_fn(|_| arena::SlotGuard::claim());
        assert_eq!(guards.iter().all(|guard| guard.is_some()), true)?;
        let error = crate::failure!(concat!("plain ", "message"));
        assert_eq!(error.message(), "plain message")?;
        Ok(())
    }

    #[test]
    fn test_static_message_skips_pool() -> TestResult {
        let guards: [Option<arena::SlotGuard>; arena::SLOT_COUNT] =
            core::array::from_fn(|_| arena::SlotGuard::claim());
        assert_eq!(guards.iter().all(|guard| guard.is_some()), true)?;
        let error = crate::failure!("plain message");
        assert_eq!(error.message(), "plain message")?;
        let error = crate::failure!("escaped {{braces}}");
        assert_eq!(error.message(), "escaped {braces}")?;
        Ok(())
    }

    #[test]
    fn test_with_message() -> TestResult {
        let error = Error::with_message(ErrorType::Failure, "boom");
        assert_eq!(error.error_type(), ErrorType::Failure)?;
        assert_eq!(error.message(), "boom")?;
        assert_location(&error)?;
        Ok(())
    }

    #[test]
    fn test_display() -> TestResult {
        let error = crate::failure!("boom");
        let mut buffer = Buffer::<256>::new();
        core::fmt::write(&mut buffer, format_args!("{}", error)).unwrap();
        let mut expected = Buffer::<256>::new();
        core::fmt::write(
            &mut expected,
            format_args!(
                "{}:{}: {}: {}",
                error.file(),
                error.line(),
                error.error_type(),
                error.message()
            ),
        )
        .unwrap();
        assert_eq!(buffer.as_str(), expected.as_str())?;
        Ok(())
    }
}
