#![no_std]
#![no_main]
#![allow(async_fn_in_trait)]

use core::fmt;
use error::Error;

/// An embedded I/O error wrapping a bzd::error object.
#[derive(Debug)]
pub struct EmbeddedError {
    error: Error,
}

impl EmbeddedError {
    /// Creates an embedded error from a bzd error.
    pub fn new(error: Error) -> Self {
        Self { error }
    }

    /// Returns a reference to the wrapped bzd error.
    pub fn as_error(&self) -> &Error {
        &self.error
    }

    /// Consumes the embedded error and returns the wrapped bzd error.
    pub fn into_error(self) -> Error {
        self.error
    }
}

impl fmt::Display for EmbeddedError {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        self.error.fmt(formatter)
    }
}

impl core::error::Error for EmbeddedError {}

impl embedded_io_async::Error for EmbeddedError {
    fn kind(&self) -> embedded_io_async::ErrorKind {
        match self.error.error_type() {
            error::ErrorType::Timeout => embedded_io_async::ErrorKind::TimedOut,
            error::ErrorType::Data => embedded_io_async::ErrorKind::InvalidData,
            error::ErrorType::Failure | error::ErrorType::Busy | error::ErrorType::Eof => {
                embedded_io_async::ErrorKind::Other
            }
        }
    }
}

/// Asynchronous byte-oriented input stream.
pub trait IStream {
    /// Reads up to `buffer.len()` bytes and returns the number read.
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Error>;

    /// Returns an adapter implementing `embedded_io_async::Read`.
    fn as_embedded_read(&mut self) -> EmbeddedIStream<'_, Self>
    where
        Self: Sized,
    {
        EmbeddedIStream::new(self)
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
            if size == 0 {
                return Err(error::failure("stream write returned 0 bytes written"));
            }
            data = &data[size..];
        }
        Ok(())
    }

    /// Flushes buffered data. This is a no-op by default.
    async fn flush(&mut self) -> Result<(), Error> {
        Ok(())
    }

    /// Returns an adapter implementing `embedded_io_async::Write`.
    fn as_embedded_write(&mut self) -> EmbeddedOStream<'_, Self>
    where
        Self: Sized,
    {
        EmbeddedOStream::new(self)
    }
}

/// Adapter exposing an `IStream` as an `embedded_io_async::Read`.
pub struct EmbeddedIStream<'a, S: ?Sized> {
    stream: &'a mut S,
}

impl<'a, S: ?Sized> EmbeddedIStream<'a, S> {
    /// Creates an adapter from a stream reference.
    pub fn new(stream: &'a mut S) -> Self {
        Self { stream }
    }
}

impl<S: ?Sized> embedded_io_async::ErrorType for EmbeddedIStream<'_, S> {
    type Error = EmbeddedError;
}

impl<S: IStream + ?Sized> embedded_io_async::Read for EmbeddedIStream<'_, S> {
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Self::Error> {
        self.stream.read(buffer).await.map_err(EmbeddedError::new)
    }
}

/// Adapter exposing an `OStream` as an `embedded_io_async::Write`.
pub struct EmbeddedOStream<'a, S: ?Sized> {
    stream: &'a mut S,
}

impl<'a, S: ?Sized> EmbeddedOStream<'a, S> {
    /// Creates an adapter from a stream reference.
    pub fn new(stream: &'a mut S) -> Self {
        Self { stream }
    }
}

impl<S: ?Sized> embedded_io_async::ErrorType for EmbeddedOStream<'_, S> {
    type Error = EmbeddedError;
}

impl<S: OStream + ?Sized> embedded_io_async::Write for EmbeddedOStream<'_, S> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Self::Error> {
        self.stream.write(data).await.map_err(EmbeddedError::new)
    }

    async fn flush(&mut self) -> Result<(), Self::Error> {
        self.stream.flush().await.map_err(EmbeddedError::new)
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;
    use embedded_io_async::{Error as _, ErrorKind, Read as _, Write as _};

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

    struct Buffer<const N: usize> {
        data: [u8; N],
        len: usize,
        offset: usize,
    }

    impl<const N: usize> Buffer<N> {
        fn new() -> Self {
            Self {
                data: [0; N],
                len: 0,
                offset: 0,
            }
        }
    }

    impl<const N: usize> IStream for Buffer<N> {
        async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Error> {
            let size = buffer.len().min(self.len - self.offset);
            buffer[..size].copy_from_slice(&self.data[self.offset..self.offset + size]);
            self.offset += size;
            Ok(size)
        }
    }

    impl<const N: usize> OStream for Buffer<N> {
        async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
            self.data[self.len..self.len + data.len()].copy_from_slice(data);
            self.len += data.len();
            Ok(data.len())
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
            Ok(()) => return Err(error::failure("expected write_all to fail")),
        }
        Ok(())
    }

    #[test]
    async fn test_embedded_adapters() -> TestResult {
        let mut stream = Buffer::<8>::new();
        {
            let mut writer = stream.as_embedded_write();
            let size = writer
                .write(b"hello")
                .await
                .map_err(EmbeddedError::into_error)?;
            assert_eq!(size, 5)?;
            writer.flush().await.map_err(EmbeddedError::into_error)?;
        }
        let mut buffer = [0u8; 8];
        let size = {
            let mut reader = stream.as_embedded_read();
            reader
                .read(&mut buffer)
                .await
                .map_err(EmbeddedError::into_error)?
        };
        assert_eq!(size, 5)?;
        assert_eq!(&buffer[..size], &b"hello"[..])?;
        Ok(())
    }

    #[test]
    fn test_embedded_error_kind() -> TestResult {
        let timeout = EmbeddedError::new(error::timeout("timeout"));
        assert_eq!(timeout.kind(), ErrorKind::TimedOut)?;
        assert_eq!(timeout.as_error().error_type(), error::ErrorType::Timeout)?;
        assert_eq!(timeout.into_error().error_type(), error::ErrorType::Timeout)?;

        let data = EmbeddedError::new(error::data("data"));
        assert_eq!(data.kind(), ErrorKind::InvalidData)?;
        assert_eq!(
            EmbeddedError::new(error::failure("failure")).kind(),
            ErrorKind::Other
        )?;
        assert_eq!(
            EmbeddedError::new(error::busy("busy")).kind(),
            ErrorKind::Other
        )?;
        assert_eq!(
            EmbeddedError::new(error::eof("eof")).kind(),
            ErrorKind::Other
        )?;
        Ok(())
    }
}
