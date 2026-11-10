#![no_std]
#![no_main]
#![allow(async_fn_in_trait)]

use error::Error;

mod embedded_io;
pub use embedded_io::*;

/// Asynchronous byte-oriented input stream.
pub trait IStream {
    /// Reads up to `buffer.len()` bytes and returns the number read.
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Error>;

    /// Returns an adapter implementing `embedded_io_async::Read`.
    fn as_embedded_read(&mut self) -> EmbeddedStream<'_, Self> {
        EmbeddedStream::new(self)
    }
}

/// Asynchronous byte-oriented output stream.
pub trait OStream {
    /// Writes up to `data.len()` bytes and returns the number written.
    async fn write(&mut self, data: &[u8]) -> Result<usize, Error>;

    /// Writes all bytes, failing if the stream stops making progress.
    async fn write_all(&mut self, mut data: &[u8]) -> Result<(), Error> {
        while !data.is_empty() {
            let size = self.write(data).await?;
            error::ensure!(size != 0, "stream write returned 0 bytes written")?;
            data = &data[size..];
        }
        Ok(())
    }

    /// Flushes buffered data. This is a no-op by default.
    async fn flush(&mut self) -> Result<(), Error> {
        Ok(())
    }

    /// Returns an adapter implementing `embedded_io_async::Write`.
    fn as_embedded_write(&mut self) -> EmbeddedStream<'_, Self> {
        EmbeddedStream::new(self)
    }
}

/// Bidirectional asynchronous byte stream.
pub trait IOStream: IStream + OStream {
    /// Returns an adapter implementing `embedded_io_async::Read + Write`.
    fn as_embedded_io(&mut self) -> EmbeddedStream<'_, Self> {
        EmbeddedStream::new(self)
    }
}

impl<T: IStream + OStream + ?Sized> IOStream for T {}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    struct ChunkedOStream<const N: usize> {
        data: [u8; N],
        len: usize,
        max_write: usize,
    }

    impl<const N: usize> ChunkedOStream<N> {
        fn new(max_write: usize) -> Self {
            Self {
                data: [0; N],
                len: 0,
                max_write,
            }
        }
    }

    impl<const N: usize> OStream for ChunkedOStream<N> {
        async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
            let size = data.len().min(self.max_write);
            self.data[self.len..self.len + size].copy_from_slice(&data[..size]);
            self.len += size;
            Ok(size)
        }
    }

    struct ZeroOStream;

    impl OStream for ZeroOStream {
        async fn write(&mut self, _: &[u8]) -> Result<usize, Error> {
            Ok(0)
        }
    }

    #[test]
    async fn test_write_all_writes_in_chunks() -> TestResult {
        let mut stream = ChunkedOStream::<8>::new(2);
        stream.write_all(b"hello").await?;
        assert_eq!(stream.len, 5)?;
        assert_eq!(&stream.data[..stream.len], &b"hello"[..])?;
        Ok(())
    }

    #[test]
    async fn test_write_all_rejects_zero_progress() -> TestResult {
        match ZeroOStream.write_all(b"hello").await {
            Err(err) => {
                assert_eq!(err.error_type(), error::ErrorType::Failure)?;
                assert_eq!(err.message(), "stream write returned 0 bytes written")?;
            }
            Ok(()) => return Err(error::failure!("expected write_all to fail")),
        }
        Ok(())
    }
}
