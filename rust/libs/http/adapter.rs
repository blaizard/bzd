use bzd::base::error::{Error, ErrorType};
use bzd::base::stream::{IStream, OStream};

/// Adapter exposing a bzd stream as an embedded-io async transport.
pub(crate) struct EmbeddedIOStream<'a, S: ?Sized> {
    stream: &'a mut S,
}

impl<'a, S: ?Sized> EmbeddedIOStream<'a, S> {
    /// Create an adapter from a bzd stream.
    pub(crate) fn new(stream: &'a mut S) -> Self {
        Self { stream }
    }
}

/// Error adapter from bzd errors to embedded-io errors.
#[derive(Debug)]
pub(crate) struct StreamError {
    error: Error,
}

impl StreamError {
    pub(crate) fn new(error: Error) -> Self {
        Self { error }
    }
}

impl core::fmt::Display for StreamError {
    fn fmt(&self, formatter: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        self.error.fmt(formatter)
    }
}

impl core::error::Error for StreamError {}

impl embedded_io_async::Error for StreamError {
    fn kind(&self) -> embedded_io_async::ErrorKind {
        match self.error.error_type() {
            ErrorType::Timeout => embedded_io_async::ErrorKind::TimedOut,
            ErrorType::Data => embedded_io_async::ErrorKind::InvalidData,
            ErrorType::Failure | ErrorType::Busy | ErrorType::Eof => {
                embedded_io_async::ErrorKind::Other
            }
        }
    }
}

impl<S: ?Sized> embedded_io_async::ErrorType for EmbeddedIOStream<'_, S> {
    type Error = StreamError;
}

impl<S: IStream + ?Sized> embedded_io_async::Read for EmbeddedIOStream<'_, S> {
    async fn read(&mut self, buffer: &mut [u8]) -> Result<usize, Self::Error> {
        self.stream.read(buffer).await.map_err(StreamError::new)
    }
}

impl<S: OStream + ?Sized> embedded_io_async::Write for EmbeddedIOStream<'_, S> {
    async fn write(&mut self, data: &[u8]) -> Result<usize, Self::Error> {
        self.stream.write(data).await.map_err(StreamError::new)
    }

    async fn flush(&mut self) -> Result<(), Self::Error> {
        self.stream.flush().await.map_err(StreamError::new)
    }
}
