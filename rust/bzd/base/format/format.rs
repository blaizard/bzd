#![no_std]
#![no_main]

use core::fmt::{write, Arguments};
use error::{self, Error};
use stream::OStream;

/// The number of bytes written to the stream per formatting pass.
const CHUNK_SIZE: usize = 256;

/// A `core::fmt::Write` sink that drops the first `skip` bytes of the formatted output,
/// fills its buffer with the following bytes and discards the rest.
///
/// It allows a message to be formatted and written in chunks without any allocation,
/// the remaining bytes being produced by formatting the message again with a bigger skip.
struct SkipWriter<'a> {
    buffer: &'a mut [u8],
    skip: usize,
    written: usize,
    overflowed: bool,
}

impl<'a> SkipWriter<'a> {
    fn new(buffer: &'a mut [u8], skip: usize) -> Self {
        Self {
            buffer,
            skip,
            written: 0,
            overflowed: false,
        }
    }

    fn written(&self) -> usize {
        self.written
    }

    fn overflowed(&self) -> bool {
        self.overflowed
    }
}

impl core::fmt::Write for SkipWriter<'_> {
    fn write_str(&mut self, text: &str) -> core::fmt::Result {
        let bytes = text.as_bytes();
        if self.skip >= bytes.len() {
            self.skip -= bytes.len();
            return Ok(());
        }
        let remaining = &bytes[self.skip..];
        self.skip = 0;
        let size = remaining.len().min(self.buffer.len() - self.written);
        self.buffer[self.written..self.written + size].copy_from_slice(&remaining[..size]);
        self.written += size;
        if size < remaining.len() {
            self.overflowed = true;
        }
        Ok(())
    }
}

/// Write a formatted message to a stream, chunk by chunk, without any allocation.
///
/// A message that fits in [`CHUNK_SIZE`] bytes is formatted only once. A longer message
/// is formatted again for each chunk, which requires its `Display` implementations to be
/// pure, that is to always produce the same output for the same input.
pub async fn write_to<S: OStream>(out: &mut S, args: Arguments<'_>) -> Result<(), Error> {
    let mut chunk = [0u8; CHUNK_SIZE];
    let mut skip = 0;
    loop {
        let (written, overflowed) = {
            let mut writer = SkipWriter::new(&mut chunk, skip);
            write(&mut writer, args)
                .map_err(|_| error::failure!("failed to format the message"))?;
            (writer.written(), writer.overflowed())
        };
        error::ensure!(written > 0 || !overflowed, "format chunk is empty")?;
        if written > 0 {
            out.write_all(&chunk[..written]).await?;
            skip += written;
        }
        if !overflowed {
            return Ok(());
        }
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    fn format_into(buffer: &mut [u8], skip: usize, args: Arguments<'_>) -> (usize, bool) {
        let mut writer = SkipWriter::new(buffer, skip);
        write(&mut writer, args).unwrap();
        (writer.written(), writer.overflowed())
    }

    #[test]
    fn test_fit() -> TestResult {
        let mut buffer = [0u8; 16];
        let (written, overflowed) = format_into(&mut buffer, 0, format_args!("hello {}", 42));
        assert_eq!(written, 8)?;
        assert_eq!(overflowed, false)?;
        assert_eq!(&buffer[..written], b"hello 42")?;
        Ok(())
    }

    #[test]
    fn test_exact_fit() -> TestResult {
        let mut buffer = [0u8; 8];
        let (written, overflowed) = format_into(&mut buffer, 0, format_args!("hello {}", 42));
        assert_eq!(written, 8)?;
        assert_eq!(overflowed, false)?;
        Ok(())
    }

    #[test]
    fn test_overflow() -> TestResult {
        let mut buffer = [0u8; 4];
        let (written, overflowed) = format_into(&mut buffer, 0, format_args!("hello {}", 42));
        assert_eq!(written, 4)?;
        assert_eq!(overflowed, true)?;
        assert_eq!(&buffer[..written], b"hell")?;
        Ok(())
    }

    #[test]
    fn test_skip_whole_pieces() -> TestResult {
        let mut buffer = [0u8; 4];
        let (written, overflowed) = format_into(&mut buffer, 6, format_args!("hello {}", 42));
        assert_eq!(written, 2)?;
        assert_eq!(overflowed, false)?;
        assert_eq!(&buffer[..written], b"42")?;
        Ok(())
    }

    #[test]
    fn test_skip_part_of_piece() -> TestResult {
        let mut buffer = [0u8; 4];
        let (written, overflowed) = format_into(&mut buffer, 2, format_args!("hello {}", 42));
        assert_eq!(written, 4)?;
        assert_eq!(overflowed, true)?;
        assert_eq!(&buffer[..written], b"llo ")?;
        Ok(())
    }

    #[test]
    fn test_empty_message() -> TestResult {
        let mut buffer = [0u8; 4];
        let (written, overflowed) = format_into(&mut buffer, 0, format_args!(""));
        assert_eq!(written, 0)?;
        assert_eq!(overflowed, false)?;
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
    async fn test_write_short_message() -> TestResult {
        let mut buffer = Buffer::<64>::new();
        write_to(&mut buffer, format_args!("hello {}", 42)).await?;
        assert_eq!(buffer.as_str(), "hello 42")?;
        assert_eq!(buffer.write_count, 1)?;
        Ok(())
    }

    #[test]
    async fn test_write_empty_message() -> TestResult {
        let mut buffer = Buffer::<64>::new();
        write_to(&mut buffer, format_args!("")).await?;
        assert_eq!(buffer.as_str(), "")?;
        assert_eq!(buffer.write_count, 0)?;
        Ok(())
    }

    #[test]
    async fn test_write_long_message() -> TestResult {
        struct Repeated;

        impl core::fmt::Display for Repeated {
            fn fmt(&self, formatter: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
                for _ in 0..1000 {
                    formatter.write_str("x")?;
                }
                Ok(())
            }
        }

        let mut buffer = Buffer::<2048>::new();
        write_to(&mut buffer, format_args!("{}", Repeated)).await?;
        assert_eq!(buffer.as_str().len(), 1000)?;
        assert_eq!(buffer.as_str().matches('x').count(), 1000)?;
        assert_eq!(buffer.write_count, 4)?;
        Ok(())
    }

    #[test]
    async fn test_write_split_multibyte_character() -> TestResult {
        struct Repeated;

        impl core::fmt::Display for Repeated {
            fn fmt(&self, formatter: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
                for _ in 0..300 {
                    formatter.write_str("é")?;
                }
                Ok(())
            }
        }

        let mut buffer = Buffer::<2048>::new();
        // The leading 'a' makes the chunk boundaries fall in the middle of a character.
        write_to(&mut buffer, format_args!("a{}", Repeated)).await?;
        assert_eq!(buffer.as_str().matches('é').count(), 300)?;
        assert_eq!(buffer.as_str().starts_with("aé"), true)?;
        Ok(())
    }
}
