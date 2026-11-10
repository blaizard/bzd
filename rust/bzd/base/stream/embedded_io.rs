use core::fmt;
use error::Error;

use super::{IStream, OStream};

pub use embedded_io_async;

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
            error::ErrorType::Eof => embedded_io_async::ErrorKind::BrokenPipe,
            error::ErrorType::Failure | error::ErrorType::Busy => {
                embedded_io_async::ErrorKind::Other
            }
        }
    }
}

/// Adapter exposing a bzd stream as `embedded_io_async` traits.
///
/// A wrapper is required because the orphan rules forbid implementing the
/// foreign `embedded_io_async` traits directly for bzd stream types.
pub struct EmbeddedStream<'a, S: ?Sized> {
    stream: &'a mut S,
}

impl<'a, S: ?Sized> EmbeddedStream<'a, S> {
    /// Creates an adapter from a stream reference.
    pub fn new(stream: &'a mut S) -> Self {
        Self { stream }
    }
}

impl<S: ?Sized> embedded_io_async::ErrorType for EmbeddedStream<'_, S> {
    type Error = EmbeddedError;
}

impl<S: IStream + ?Sized> embedded_io_async::Read for EmbeddedStream<'_, S> {
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Self::Error> {
        error::ensure!(!buffer.is_empty(), "stream read with an empty buffer")
            .map_err(EmbeddedError::new)?;
        self.stream.read(buffer).await.map_err(EmbeddedError::new)
    }
}

impl<S: OStream + ?Sized> embedded_io_async::Write for EmbeddedStream<'_, S> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Self::Error> {
        error::ensure!(!data.is_empty(), "stream write with an empty buffer")
            .map_err(EmbeddedError::new)?;
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
    use crate::{IOStream, IStream, OStream};
    use embedded_io_async::{Error as _, ErrorKind, Read as _, Write as _};

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
    async fn test_embedded_stream() -> TestResult {
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

        let mut stream = Buffer::<8>::new();
        {
            let mut io = stream.as_embedded_io();
            let size = io
                .write(b"hello")
                .await
                .map_err(EmbeddedError::into_error)?;
            assert_eq!(size, 5)?;
            io.flush().await.map_err(EmbeddedError::into_error)?;
        }
        let mut buffer = [0u8; 8];
        let size = {
            let mut io = stream.as_embedded_io();
            io.read(&mut buffer)
                .await
                .map_err(EmbeddedError::into_error)?
        };
        assert_eq!(size, 5)?;
        assert_eq!(&buffer[..size], &b"hello"[..])?;
        Ok(())
    }

    #[test]
    async fn test_embedded_adapters_reject_empty_buffers() -> TestResult {
        // Empty transfers can never make progress and some implementations block
        // on them, so they must fail instead of being forwarded.
        struct PanicStream;

        impl IStream for PanicStream {
            async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Error> {
                assert_eq!(buffer.is_empty(), false)?;
                Ok(0)
            }
        }

        impl OStream for PanicStream {
            async fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
                assert_eq!(data.is_empty(), false)?;
                Ok(0)
            }
        }

        let mut stream = PanicStream;
        let mut io = stream.as_embedded_io();
        let error = io.read(&mut []).await.unwrap_err().into_error();
        assert_eq!(error.error_type(), error::ErrorType::Failure)?;
        assert_eq!(error.message(), "stream read with an empty buffer")?;
        let error = io.write(&[]).await.unwrap_err().into_error();
        assert_eq!(error.error_type(), error::ErrorType::Failure)?;
        assert_eq!(error.message(), "stream write with an empty buffer")?;

        let mut stream = PanicStream;
        let mut reader = stream.as_embedded_read();
        let error = reader.read(&mut []).await.unwrap_err().into_error();
        assert_eq!(error.message(), "stream read with an empty buffer")?;

        let mut stream = PanicStream;
        let mut writer = stream.as_embedded_write();
        let error = writer.write(&[]).await.unwrap_err().into_error();
        assert_eq!(error.message(), "stream write with an empty buffer")?;
        Ok(())
    }

    #[test]
    fn test_embedded_error_kind() -> TestResult {
        let timeout = EmbeddedError::new(error::timeout!("timeout"));
        assert_eq!(timeout.kind(), ErrorKind::TimedOut)?;
        assert_eq!(timeout.as_error().error_type(), error::ErrorType::Timeout)?;
        assert_eq!(timeout.into_error().error_type(), error::ErrorType::Timeout)?;

        let data = EmbeddedError::new(error::data!("data"));
        assert_eq!(data.kind(), ErrorKind::InvalidData)?;
        assert_eq!(
            EmbeddedError::new(error::failure!("failure")).kind(),
            ErrorKind::Other
        )?;
        assert_eq!(
            EmbeddedError::new(error::busy!("busy")).kind(),
            ErrorKind::Other
        )?;
        assert_eq!(
            EmbeddedError::new(error::eof!("eof")).kind(),
            ErrorKind::BrokenPipe
        )?;
        Ok(())
    }
}
