#![no_std]
#![no_main]
#![allow(async_fn_in_trait)]

use error::Error;

mod embedded_io;
pub use embedded_io::*;

/// A window of bytes returned by [`IStream::read`].
///
/// Dropping a chunk releases it; the whole window is consumed unless
/// [`BorrowChunk::consumed`] was called.
pub trait BorrowChunk: AsRef<[u8]> {
    /// Reports that only the first `used_len` bytes were consumed.
    ///
    /// `used_len` is clamped to the chunk length.
    fn consumed(&mut self, used_len: usize);
}

/// Passthrough chunks already live in the caller's buffer.
impl BorrowChunk for &[u8] {
    fn consumed(&mut self, _used_len: usize) {}
}

/// Asynchronous byte-oriented input stream.
pub trait IStream {
    /// Bytes made available by [`read`], borrowing from the stream and/or the buffer.
    type Chunk<'a>: BorrowChunk + 'a
    where
        Self: 'a;

    /// Returns a window of bytes borrowed from the stream and/or from `buffer`.
    ///
    /// The window may be larger than `buffer` when the stream exposes its own
    /// storage.
    async fn read<'a>(&'a mut self, buffer: &'a mut [u8]) -> Result<Self::Chunk<'a>, Error>;

    /// Reads into `buf`, copying from the returned chunk when it does not
    /// already start at the beginning of `buf`.
    async fn read_into(&mut self, buf: &mut [u8]) -> Result<usize, Error> {
        if buf.is_empty() {
            return Ok(0);
        }

        let buffer_ptr = buf.as_mut_ptr();
        let buffer_len = buf.len();
        let mut chunk = self.read(buf).await?;
        let (data_ptr, data_len) = {
            let data = chunk.as_ref();
            (data.as_ptr(), data.len())
        };
        let size = data_len.min(buffer_len);

        let buffer_start = buffer_ptr as usize;
        let buffer_end = buffer_start + buffer_len;
        let data_start = data_ptr as usize;
        let data_end = data_start + data_len;

        if data_start == buffer_start {
            chunk.consumed(size);
            return Ok(size);
        }

        if data_start < buffer_end && buffer_start < data_end {
            chunk.consumed(0);
            return Err(error::failure!(
                "read chunk overlaps the destination buffer at a non-zero offset"
            ));
        }

        // SAFETY: the ranges checked above are disjoint, so the chunk does not
        // alias the destination buffer, whose pointer stays valid.
        unsafe { core::ptr::copy_nonoverlapping(data_ptr, buffer_ptr, size) };
        chunk.consumed(size);
        Ok(size)
    }

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

    struct WindowStream {
        data: [u8; 4],
        consumed: core::cell::Cell<usize>,
        returned: core::cell::Cell<usize>,
    }

    impl WindowStream {
        fn new() -> Self {
            Self {
                data: *b"data",
                consumed: core::cell::Cell::new(0),
                returned: core::cell::Cell::new(0),
            }
        }
    }

    struct WindowChunk<'a> {
        data: &'a [u8],
        consumed: &'a core::cell::Cell<usize>,
        returned: &'a core::cell::Cell<usize>,
        finalized: bool,
    }

    impl AsRef<[u8]> for WindowChunk<'_> {
        fn as_ref(&self) -> &[u8] {
            self.data
        }
    }

    impl BorrowChunk for WindowChunk<'_> {
        fn consumed(&mut self, used_len: usize) {
            let used_len = used_len.min(self.data.len());
            let remaining = self.data.len() - used_len;
            self.consumed.set(self.consumed.get() + used_len);
            self.returned.set(self.returned.get() + remaining);
            self.data = &self.data[..used_len];
            self.finalized = true;
        }
    }

    impl Drop for WindowChunk<'_> {
        fn drop(&mut self) {
            if !self.finalized {
                self.consumed.set(self.consumed.get() + self.data.len());
            }
        }
    }

    impl IStream for WindowStream {
        type Chunk<'a>
            = WindowChunk<'a>
        where
            Self: 'a;

        async fn read<'a>(&'a mut self, _buffer: &'a mut [u8]) -> Result<Self::Chunk<'a>, Error> {
            Ok(WindowChunk {
                data: &self.data,
                consumed: &self.consumed,
                returned: &self.returned,
                finalized: false,
            })
        }
    }

    struct PassthroughStream {
        input: &'static [u8],
        offset: usize,
    }

    impl PassthroughStream {
        fn new(input: &'static [u8]) -> Self {
            Self { input, offset: 0 }
        }
    }

    impl IStream for PassthroughStream {
        type Chunk<'a>
            = &'a [u8]
        where
            Self: 'a;

        async fn read<'a>(&'a mut self, buffer: &'a mut [u8]) -> Result<Self::Chunk<'a>, Error> {
            let remaining = &self.input[self.offset..];
            let size = buffer.len().min(remaining.len());
            buffer[..size].copy_from_slice(&remaining[..size]);
            self.offset += size;
            Ok(&buffer[..size])
        }
    }

    struct OverlapStream {
        consumed: core::cell::Cell<usize>,
        returned: core::cell::Cell<usize>,
    }

    impl OverlapStream {
        fn new() -> Self {
            Self {
                consumed: core::cell::Cell::new(0),
                returned: core::cell::Cell::new(0),
            }
        }
    }

    impl IStream for OverlapStream {
        type Chunk<'a>
            = WindowChunk<'a>
        where
            Self: 'a;

        async fn read<'a>(&'a mut self, buffer: &'a mut [u8]) -> Result<Self::Chunk<'a>, Error> {
            Ok(WindowChunk {
                data: &buffer[1..3],
                consumed: &self.consumed,
                returned: &self.returned,
                finalized: false,
            })
        }
    }

    struct PanicIStream;

    impl IStream for PanicIStream {
        type Chunk<'a>
            = &'a [u8]
        where
            Self: 'a;

        async fn read<'a>(&'a mut self, _buffer: &'a mut [u8]) -> Result<Self::Chunk<'a>, Error> {
            Err(error::failure!("read called with an empty destination"))
        }
    }

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

    #[test]
    async fn test_chunk_release_defaults_to_full_consumption() -> TestResult {
        let mut stream = WindowStream::new();
        let mut buffer = [0u8; 4];
        {
            let chunk = stream.read(&mut buffer).await?;
            assert_eq!(chunk.as_ref(), &b"data"[..])?;
        }
        assert_eq!(stream.consumed.get(), 4)?;
        assert_eq!(stream.returned.get(), 0)?;
        Ok(())
    }

    #[test]
    async fn test_chunk_consumed() -> TestResult {
        let mut stream = WindowStream::new();
        let mut buffer = [0u8; 4];
        {
            let mut chunk = stream.read(&mut buffer).await?;
            chunk.consumed(2);
            assert_eq!(chunk.as_ref(), &b"da"[..])?;
        }
        assert_eq!(stream.consumed.get(), 2)?;
        assert_eq!(stream.returned.get(), 2)?;
        Ok(())
    }

    #[test]
    async fn test_chunk_consumed_clamps() -> TestResult {
        let mut stream = WindowStream::new();
        let mut buffer = [0u8; 4];
        {
            let mut chunk = stream.read(&mut buffer).await?;
            chunk.consumed(10);
            assert_eq!(chunk.as_ref(), &b"data"[..])?;
        }
        assert_eq!(stream.consumed.get(), 4)?;
        assert_eq!(stream.returned.get(), 0)?;
        Ok(())
    }

    #[test]
    async fn test_read_into_copies_internal_chunk() -> TestResult {
        let mut stream = WindowStream::new();
        let mut buffer = [0u8; 4];
        let size = stream.read_into(&mut buffer).await?;
        assert_eq!(size, 4)?;
        assert_eq!(&buffer, b"data")?;
        assert_eq!(stream.consumed.get(), 4)?;
        assert_eq!(stream.returned.get(), 0)?;
        Ok(())
    }

    #[test]
    async fn test_read_into_consumes_destination_length() -> TestResult {
        let mut stream = WindowStream::new();
        let mut buffer = [0u8; 2];
        let size = stream.read_into(&mut buffer).await?;
        assert_eq!(size, 2)?;
        assert_eq!(&buffer, b"da")?;
        assert_eq!(stream.consumed.get(), 2)?;
        assert_eq!(stream.returned.get(), 2)?;
        Ok(())
    }

    #[test]
    async fn test_read_into_larger_destination() -> TestResult {
        let mut stream = WindowStream::new();
        let mut buffer = [0u8; 8];
        let size = stream.read_into(&mut buffer).await?;
        assert_eq!(size, 4)?;
        assert_eq!(&buffer[..4], b"data")?;
        assert_eq!(stream.consumed.get(), 4)?;
        assert_eq!(stream.returned.get(), 0)?;
        Ok(())
    }

    #[test]
    async fn test_read_into_passthrough_chunk() -> TestResult {
        let mut stream = PassthroughStream::new(b"hello");
        let mut buffer = [0u8; 8];
        let size = stream.read_into(&mut buffer).await?;
        assert_eq!(size, 5)?;
        assert_eq!(&buffer[..5], b"hello")?;
        Ok(())
    }

    #[test]
    async fn test_read_into_empty_destination() -> TestResult {
        let mut stream = PanicIStream;
        let size = stream.read_into(&mut []).await?;
        assert_eq!(size, 0)?;
        Ok(())
    }

    #[test]
    async fn test_read_into_rejects_offset_overlap() -> TestResult {
        let mut stream = OverlapStream::new();
        let mut buffer = [0u8; 4];
        match stream.read_into(&mut buffer).await {
            Err(err) => {
                assert_eq!(err.error_type(), error::ErrorType::Failure)?;
                assert_eq!(
                    err.message(),
                    "read chunk overlaps the destination buffer at a non-zero offset"
                )?;
            }
            Ok(_) => return Err(error::failure!("expected read_into to fail")),
        }
        assert_eq!(stream.consumed.get(), 0)?;
        assert_eq!(stream.returned.get(), 2)?;
        Ok(())
    }

    #[test]
    fn test_slice_chunk_consumed_is_noop() -> TestResult {
        let mut chunk: &[u8] = b"hello";
        chunk.consumed(2);
        assert_eq!(chunk, &b"hello"[..])?;
        chunk.consumed(10);
        assert_eq!(chunk, &b"hello"[..])?;
        Ok(())
    }
}
