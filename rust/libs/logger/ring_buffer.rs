#![no_std]
#![no_main]

use bzd::base::error::{self, Error};
use core::fmt;

const HEADER_SIZE: usize = 2;

/// A fixed-size circular buffer of serialized records.
///
/// Records are stored as a little-endian `u16` length followed by their bytes.
/// One slot is reserved to distinguish an empty ring from a full one, so the
/// usable capacity is `N - 1`.
pub struct RingBuffer<const N: usize> {
    data: [u8; N],
    read: usize,
    write: usize,
    overflowed: usize,
}

impl<const N: usize> Default for RingBuffer<N> {
    fn default() -> Self {
        Self::new()
    }
}

impl<const N: usize> RingBuffer<N> {
    /// Create an empty ring.
    pub const fn new() -> Self {
        Self {
            data: [0; N],
            read: 0,
            write: 0,
            overflowed: 0,
        }
    }

    /// Serialize and append one record in place.
    ///
    /// If the record does not fit, it is dropped and the overflow counter is
    /// incremented.
    pub fn push<F>(&mut self, write: F) -> Result<(), Error>
    where
        F: FnOnce(&mut dyn fmt::Write) -> fmt::Result,
    {
        let free = self.free();
        if free < HEADER_SIZE {
            self.overflowed += 1;
            return Ok(());
        }
        let header = self.write;
        let start = (header + HEADER_SIZE) % N;
        let limit = (free - HEADER_SIZE).min(u16::MAX as usize);
        let (written, full, failed) = {
            let mut writer = RecordWriter {
                data: &mut self.data,
                start,
                limit,
                written: 0,
                full: false,
            };
            let result = write(&mut writer);
            (writer.written, writer.full, result.is_err() && !writer.full)
        };
        if failed {
            return Err(error::failure!("failed to format the record"));
        }
        if full {
            self.overflowed += 1;
            return Ok(());
        }
        self.write_header(header, written);
        self.write = (header + HEADER_SIZE + written) % N;
        Ok(())
    }

    /// Remove and return the next record as two slices.
    ///
    /// The returned bytes remain valid until the ring is modified again.
    pub fn pop(&mut self) -> Option<(&[u8], &[u8])> {
        if self.read == self.write {
            return None;
        }
        let length = self.read_header(self.read);
        debug_assert!(length <= N - HEADER_SIZE);
        let start = (self.read + HEADER_SIZE) % N;
        self.read = (start + length) % N;
        Some(self.slices(start, length))
    }

    /// Number of records dropped because they did not fit.
    pub fn overflow_count(&self) -> usize {
        self.overflowed
    }

    fn free(&self) -> usize {
        (self.read + N - self.write - 1) % N
    }

    fn write_header(&mut self, index: usize, length: usize) {
        self.data[index] = length as u8;
        self.data[(index + 1) % N] = (length >> 8) as u8;
    }

    fn read_header(&self, index: usize) -> usize {
        self.data[index] as usize | ((self.data[(index + 1) % N] as usize) << 8)
    }

    fn slices(&self, start: usize, length: usize) -> (&[u8], &[u8]) {
        let end = start + length;
        if end <= N {
            (&self.data[start..end], &[])
        } else {
            (&self.data[start..], &self.data[..end - N])
        }
    }
}

/// Writer for the bytes of one record, provided by [`RingBuffer::push`].
struct RecordWriter<'a, const N: usize> {
    data: &'a mut [u8; N],
    start: usize,
    limit: usize,
    written: usize,
    full: bool,
}

impl<const N: usize> fmt::Write for RecordWriter<'_, N> {
    fn write_str(&mut self, text: &str) -> fmt::Result {
        let bytes = text.as_bytes();
        if bytes.len() > self.limit - self.written {
            self.full = true;
            return Err(fmt::Error);
        }
        for (index, byte) in bytes.iter().enumerate() {
            self.data[(self.start + self.written + index) % N] = *byte;
        }
        self.written += bytes.len();
        Ok(())
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    fn record_bytes<const N: usize>(record: (&[u8], &[u8])) -> ([u8; N], usize) {
        let (first, second) = record;
        let mut bytes = [0u8; N];
        bytes[..first.len()].copy_from_slice(first);
        bytes[first.len()..first.len() + second.len()].copy_from_slice(second);
        (bytes, first.len() + second.len())
    }

    #[test]
    fn test_push_pop() -> TestResult {
        let mut ring = RingBuffer::<32>::new();
        ring.push(|record| write!(record, "hello {}", 42))?;
        assert_eq!(ring.overflow_count(), 0)?;
        let record = ring.pop().unwrap();
        let (bytes, length) = record_bytes::<32>(record);
        assert_eq!(&bytes[..length], b"hello 42")?;
        assert_eq!(ring.pop().is_none(), true)?;
        Ok(())
    }

    #[test]
    fn test_multiple_records() -> TestResult {
        let mut ring = RingBuffer::<64>::new();
        ring.push(|record| write!(record, "one"))?;
        ring.push(|record| write!(record, "two"))?;
        let first = ring.pop().unwrap();
        let (bytes, length) = record_bytes::<64>(first);
        assert_eq!(&bytes[..length], b"one")?;
        let second = ring.pop().unwrap();
        let (bytes, length) = record_bytes::<64>(second);
        assert_eq!(&bytes[..length], b"two")?;
        Ok(())
    }

    #[test]
    fn test_exact_fit_and_drop() -> TestResult {
        let mut ring = RingBuffer::<8>::new();
        ring.push(|record| write!(record, "12345"))?;
        assert_eq!(ring.overflow_count(), 0)?;
        ring.push(|record| write!(record, "x"))?;
        assert_eq!(ring.overflow_count(), 1)?;
        let record = ring.pop().unwrap();
        let (bytes, length) = record_bytes::<8>(record);
        assert_eq!(&bytes[..length], b"12345")?;
        ring.push(|record| write!(record, "abcde"))?;
        let record = ring.pop().unwrap();
        let (bytes, length) = record_bytes::<8>(record);
        assert_eq!(&bytes[..length], b"abcde")?;
        Ok(())
    }

    #[test]
    fn test_drop_when_too_large() -> TestResult {
        let mut ring = RingBuffer::<16>::new();
        ring.push(|record| write!(record, "abcdefghijklmnopqrst"))?;
        assert_eq!(ring.overflow_count(), 1)?;
        assert_eq!(ring.pop().is_none(), true)?;
        ring.push(|record| write!(record, "ok"))?;
        let record = ring.pop().unwrap();
        let (bytes, length) = record_bytes::<16>(record);
        assert_eq!(&bytes[..length], b"ok")?;
        Ok(())
    }

    #[test]
    fn test_record_wraps() -> TestResult {
        let mut ring = RingBuffer::<16>::new();
        ring.push(|record| write!(record, "aa"))?;
        ring.push(|record| write!(record, "bb"))?;
        ring.push(|record| write!(record, "cc"))?;
        let first = ring.pop().unwrap();
        let (bytes, length) = record_bytes::<16>(first);
        assert_eq!(&bytes[..length], b"aa")?;
        let second = ring.pop().unwrap();
        let (bytes, length) = record_bytes::<16>(second);
        assert_eq!(&bytes[..length], b"bb")?;
        ring.push(|record| write!(record, "123456789"))?;
        let third = ring.pop().unwrap();
        let (bytes, length) = record_bytes::<16>(third);
        assert_eq!(&bytes[..length], b"cc")?;
        let wrapped = ring.pop().unwrap();
        assert_eq!(wrapped.0, b"12")?;
        assert_eq!(wrapped.1, b"3456789")?;
        Ok(())
    }

    #[test]
    fn test_send() -> TestResult {
        fn assert_send<T: Send>() {}
        assert_send::<RingBuffer<1024>>();
        Ok(())
    }
}
