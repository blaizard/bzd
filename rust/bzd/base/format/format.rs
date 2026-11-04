#![no_std]
#![no_main]
#![allow(async_fn_in_trait)]

use core::fmt;
use error::Error;
use stream::OStream;

/// Fixed-size writer used by [`write_into`].
struct MessageWriter<'a> {
    data: &'a mut [u8],
    written: usize,
}

impl<'a> MessageWriter<'a> {
    fn new(data: &'a mut [u8]) -> Self {
        Self { data, written: 0 }
    }
}

impl fmt::Write for MessageWriter<'_> {
    fn write_str(&mut self, text: &str) -> fmt::Result {
        let bytes = text.as_bytes();
        if bytes.len() > self.data.len() - self.written {
            return Err(fmt::Error);
        }
        self.data[self.written..self.written + bytes.len()].copy_from_slice(bytes);
        self.written += bytes.len();
        Ok(())
    }
}

/// Format `args` into `data`, returning the number of bytes written or `None`
/// when it does not fit.
pub fn write_into(data: &mut [u8], args: fmt::Arguments<'_>) -> Option<usize> {
    let mut writer = MessageWriter::new(data);
    fmt::write(&mut writer, args).ok()?;
    Some(writer.written)
}

/// Custom serialization for a formatting argument.
pub trait Serialize {
    /// Serialize this value into a formatter.
    fn serialize(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result;
}

impl<T: fmt::Display + ?Sized> Serialize for T {
    fn serialize(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        fmt::Display::fmt(self, formatter)
    }
}

/// Adapter exposing a [`Serialize`] value as a `Display` argument.
pub struct Serialized<'a, T: ?Sized>(&'a T);

/// Wrap a value so it is formatted through its [`Serialize`] implementation.
pub fn serialize<T: Serialize + ?Sized>(value: &T) -> Serialized<'_, T> {
    Serialized(value)
}

impl<T: Serialize + ?Sized> fmt::Display for Serialized<'_, T> {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        self.0.serialize(formatter)
    }
}

/// Extension trait for writing formatted messages to an [`OStream`].
pub trait OStreamFormat: OStream {
    /// Write formatted bytes to this stream.
    async fn write_message(&mut self, data: &[u8]) -> Result<(), Error> {
        self.write_all(data).await
    }
}

impl<T: OStream + ?Sized> OStreamFormat for T {}

/// Write a formatted message to an output stream.
///
/// The message is built in a temporary 1 KiB buffer. If it does not fit, it is
/// dropped.
#[macro_export]
macro_rules! format_to {
    ($out:expr, $($arg:tt)*) => {
        async {
            use $crate::OStreamFormat;
            let mut data = [0u8; 1024];
            let ::core::option::Option::Some(length) =
                $crate::write_into(&mut data, ::core::format_args!($($arg)*))
            else {
                return ::core::result::Result::Ok(());
            };
            $out.write_message(&data[..length]).await
        }
    };
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    async fn test_custom_serialize() -> TestResult {
        struct Mac([u8; 3]);

        impl Serialize for Mac {
            fn serialize(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
                write!(
                    formatter,
                    "{:02x}:{:02x}:{:02x}",
                    self.0[0], self.0[1], self.0[2]
                )
            }
        }

        let mut buffer = Buffer::<64>::new();
        let mac = Mac([1, 2, 3]);
        format_to!(buffer, "mac={}", serialize(&mac)).await?;
        assert_eq!(buffer.as_str(), "mac=01:02:03")?;
        Ok(())
    }

    #[test]
    async fn test_display_fallback() -> TestResult {
        let mut buffer = Buffer::<64>::new();
        format_to!(buffer, "{}", serialize(&42)).await?;
        assert_eq!(buffer.as_str(), "42")?;
        Ok(())
    }

    struct Buffer<const N: usize> {
        data: [u8; N],
        len: usize,
        write_count: usize,
    }

    impl<const N: usize> Buffer<N> {
        fn new() -> Self {
            Self {
                data: [0; N],
                len: 0,
                write_count: 0,
            }
        }

        fn as_str(&self) -> &str {
            core::str::from_utf8(&self.data[..self.len]).unwrap()
        }
    }

    impl<const N: usize> OStream for Buffer<N> {
        async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
            self.data[self.len..self.len + data.len()].copy_from_slice(data);
            self.len += data.len();
            self.write_count += 1;
            Ok(data.len())
        }
    }

    #[test]
    async fn test_format_to_short_message() -> TestResult {
        let mut buffer = Buffer::<64>::new();
        format_to!(buffer, "hello {}", 42).await?;
        assert_eq!(buffer.as_str(), "hello 42")?;
        assert_eq!(buffer.write_count, 1)?;
        Ok(())
    }

    #[test]
    async fn test_format_to_empty_message() -> TestResult {
        let mut buffer = Buffer::<64>::new();
        format_to!(buffer, "").await?;
        assert_eq!(buffer.as_str(), "")?;
        assert_eq!(buffer.write_count, 0)?;
        Ok(())
    }

    #[test]
    async fn test_format_to_drops_oversized() -> TestResult {
        struct Repeated;

        impl fmt::Display for Repeated {
            fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
                for _ in 0..2000 {
                    formatter.write_str("x")?;
                }
                Ok(())
            }
        }

        let mut buffer = Buffer::<2048>::new();
        format_to!(buffer, "{}", Repeated).await?;
        assert_eq!(buffer.as_str(), "")?;
        assert_eq!(buffer.write_count, 0)?;
        Ok(())
    }

    #[test]
    async fn test_format_to_multibyte() -> TestResult {
        struct Repeated;

        impl fmt::Display for Repeated {
            fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
                for _ in 0..300 {
                    formatter.write_str("é")?;
                }
                Ok(())
            }
        }

        let mut buffer = Buffer::<2048>::new();
        format_to!(buffer, "a{}", Repeated).await?;
        assert_eq!(buffer.as_str().matches('é').count(), 300)?;
        assert_eq!(buffer.as_str().starts_with("aé"), true)?;
        Ok(())
    }
}
