#![no_std]
#![no_main]

use core::fmt;
use core::panic::Location;

/// Platform agnostic error type.
#[repr(u8)]
#[derive(Copy, Clone, Debug, PartialEq, Eq, Hash)]
pub enum ErrorType {
    /// Any error that do not fall in the below categories.
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
    /// reason that are out of our control.
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
    message: &'static str,
}

impl Error {
    /// Create a new error with a message.
    #[track_caller]
    pub fn with_message(error_type: ErrorType, message: &'static str) -> Self {
        let location = Location::caller();
        Self {
            source: location.file(),
            line: location.line(),
            error_type,
            message,
        }
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
    pub fn message(&self) -> &'static str {
        self.message
    }
}

/// Create a failure error.
#[track_caller]
pub fn failure(message: &'static str) -> Error {
    Error::with_message(ErrorType::Failure, message)
}

/// Create a timeout error.
#[track_caller]
pub fn timeout(message: &'static str) -> Error {
    Error::with_message(ErrorType::Timeout, message)
}

/// Create a busy error.
#[track_caller]
pub fn busy(message: &'static str) -> Error {
    Error::with_message(ErrorType::Busy, message)
}

/// Create an end-of-channel error.
#[track_caller]
pub fn eof(message: &'static str) -> Error {
    Error::with_message(ErrorType::Eof, message)
}

/// Create a data error.
#[track_caller]
pub fn data(message: &'static str) -> Error {
    Error::with_message(ErrorType::Data, message)
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "{}:{}: {}: {}",
            self.source, self.line, self.error_type, self.message
        )
    }
}

impl fmt::Debug for Error {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("Error")
            .field("file", &self.source)
            .field("line", &self.line)
            .field("error_type", &self.error_type)
            .field("message", &self.message)
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

    fn assert_location(error: &Error) -> TestResult {
        if error.file().is_empty() || error.line() == 0 {
            return Err(TestError {
                file: file!(),
                line: line!(),
                message: "error location not captured",
            });
        }
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
        assert_eq!(failure("msg").error_type(), ErrorType::Failure)?;
        assert_eq!(timeout("msg").error_type(), ErrorType::Timeout)?;
        assert_eq!(busy("msg").error_type(), ErrorType::Busy)?;
        assert_eq!(eof("msg").error_type(), ErrorType::Eof)?;
        assert_eq!(data("msg").error_type(), ErrorType::Data)?;
        Ok(())
    }

    #[test]
    fn test_factories_message() -> TestResult {
        assert_eq!(failure("failed").message(), "failed")?;
        assert_eq!(timeout("timed out").message(), "timed out")?;
        assert_eq!(busy("is busy").message(), "is busy")?;
        assert_eq!(eof("end of file").message(), "end of file")?;
        assert_eq!(data("bad data").message(), "bad data")?;
        Ok(())
    }

    #[test]
    fn test_factories_location() -> TestResult {
        assert_location(&failure("msg"))?;
        assert_location(&timeout("msg"))?;
        assert_location(&busy("msg"))?;
        assert_location(&eof("msg"))?;
        assert_location(&data("msg"))?;
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
        let error = failure("boom");
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
