#![no_std]
#![no_main]

use bzd::base::error::{self, Error};
use core::cell::UnsafeCell;
use core::fmt;
use core::ptr;
use core::sync::atomic::{AtomicUsize, Ordering};

const HEADER_SIZE: usize = 2;

/// A fixed-size circular buffer of serialized records.
///
/// Records are stored as a little-endian `u16` length followed by their bytes.
/// One slot is reserved to distinguish an empty ring from a full one, so the
/// usable capacity is `N - 1`.
///
/// This is a single-producer / single-consumer queue. The producer publishes
/// the new write index with `Release`, and the consumer publishes the read
/// index with `Release` only after it has finished using the record. The
/// producer therefore never overwrites a record that is still being consumed.
pub struct RingBuffer<const N: usize> {
    data: UnsafeCell<[u8; N]>,
    read: AtomicUsize,
    write: AtomicUsize,
    overflowed: AtomicUsize,
}

// SAFETY: the single-producer / single-consumer protocol guarantees that the
// producer and consumer only access disjoint regions of `data`.
unsafe impl<const N: usize> Sync for RingBuffer<N> {}

impl<const N: usize> Default for RingBuffer<N> {
    fn default() -> Self {
        Self::new()
    }
}

impl<const N: usize> RingBuffer<N> {
    /// Create an empty ring.
    pub const fn new() -> Self {
        Self {
            data: UnsafeCell::new([0; N]),
            read: AtomicUsize::new(0),
            write: AtomicUsize::new(0),
            overflowed: AtomicUsize::new(0),
        }
    }

    /// Serialize and append one record in place.
    ///
    /// Returns the monotonic write index after the operation.
    ///
    /// If the record does not fit, it is dropped and the overflow counter is
    /// incremented.
    pub fn push<F>(&self, write: F) -> Result<usize, Error>
    where
        F: FnOnce(&mut dyn fmt::Write) -> fmt::Result,
    {
        let read = self.read.load(Ordering::Acquire);
        let write_index = self.write.load(Ordering::Relaxed);
        let free = N - 1 - (write_index - read);
        if free < HEADER_SIZE {
            self.overflowed.fetch_add(1, Ordering::Relaxed);
            return Ok(write_index);
        }
        let start = (write_index + HEADER_SIZE) % N;
        let limit = (free - HEADER_SIZE).min(u16::MAX as usize);
        let (written, full, failed) = {
            let mut writer = RecordWriter::<N> {
                data: self.data.get().cast::<u8>(),
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
            self.overflowed.fetch_add(1, Ordering::Relaxed);
            return Ok(write_index);
        }
        self.write_header(write_index, written);
        let new_write = write_index + HEADER_SIZE + written;
        self.write.store(new_write, Ordering::Release);
        Ok(new_write)
    }

    /// Return the next record as two slices and its length, without consuming it.
    ///
    /// The record remains valid until [`RingBuffer::commit`] is called, so the
    /// caller can write it in place without copying.
    pub fn peek(&self) -> Option<(&[u8], &[u8], usize)> {
        let read = self.read.load(Ordering::Relaxed);
        let write = self.write.load(Ordering::Acquire);
        if read == write {
            return None;
        }
        let length = self.read_header(read);
        debug_assert!(length <= N - HEADER_SIZE);
        let start = (read + HEADER_SIZE) % N;
        let (first, second) = self.slices(start, length);
        Some((first, second, length))
    }

    /// Mark the record returned by [`RingBuffer::peek`] as consumed.
    pub fn commit(&self, length: usize) {
        let read = self.read.load(Ordering::Relaxed);
        self.read
            .store(read + HEADER_SIZE + length, Ordering::Release);
    }

    /// Whether the record up to `target` has been consumed.
    pub fn is_consumed(&self, target: usize) -> bool {
        self.read.load(Ordering::Acquire) >= target
    }

    /// Number of records dropped because they did not fit.
    pub fn overflow_count(&self) -> usize {
        self.overflowed.load(Ordering::Relaxed)
    }

    fn write_header(&self, index: usize, length: usize) {
        let index = index % N;
        // SAFETY: the producer writes the header of the record it is appending,
        // which is in the free region.
        unsafe {
            let data = self.data.get().cast::<u8>();
            ptr::write(data.add(index), length as u8);
            ptr::write(data.add((index + 1) % N), (length >> 8) as u8);
        }
    }

    fn read_header(&self, index: usize) -> usize {
        let index = index % N;
        // SAFETY: the consumer reads the header of an unconsumed record.
        unsafe {
            let data = self.data.get().cast::<u8>();
            (*data.add(index) as usize) | ((*data.add((index + 1) % N) as usize) << 8)
        }
    }

    fn slices(&self, start: usize, length: usize) -> (&[u8], &[u8]) {
        // SAFETY: the consumer only reads the unconsumed record. The producer
        // may write to the free region in the meantime, but never to this one.
        unsafe {
            let data = self.data.get().cast::<u8>();
            let end = start + length;
            if end <= N {
                (core::slice::from_raw_parts(data.add(start), length), &[])
            } else {
                (
                    core::slice::from_raw_parts(data.add(start), N - start),
                    core::slice::from_raw_parts(data, end - N),
                )
            }
        }
    }
}

/// Writer for the bytes of one record, provided by [`RingBuffer::push`].
struct RecordWriter<const N: usize> {
    data: *mut u8,
    start: usize,
    limit: usize,
    written: usize,
    full: bool,
}

impl<const N: usize> fmt::Write for RecordWriter<N> {
    fn write_str(&mut self, text: &str) -> fmt::Result {
        let bytes = text.as_bytes();
        if bytes.len() > self.limit - self.written {
            self.full = true;
            return Err(fmt::Error);
        }
        for (index, byte) in bytes.iter().enumerate() {
            // SAFETY: the writer only writes into the record's free region.
            unsafe {
                ptr::write(
                    self.data.add((self.start + self.written + index) % N),
                    *byte,
                );
            }
        }
        self.written += bytes.len();
        Ok(())
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    fn take<const N: usize>(ring: &RingBuffer<N>, out: &mut [u8; N]) -> Option<usize> {
        let (first, second, length) = ring.peek()?;
        out[..first.len()].copy_from_slice(first);
        out[first.len()..length].copy_from_slice(second);
        ring.commit(length);
        Some(length)
    }

    #[test]
    fn test_push_pop() -> TestResult {
        let ring = RingBuffer::<32>::new();
        ring.push(|record| write!(record, "hello {}", 42))?;
        assert_eq!(ring.overflow_count(), 0)?;
        let mut bytes = [0u8; 32];
        let length = take(&ring, &mut bytes).unwrap();
        assert_eq!(&bytes[..length], b"hello 42")?;
        assert_eq!(ring.peek().is_none(), true)?;
        Ok(())
    }

    #[test]
    fn test_peek_does_not_consume() -> TestResult {
        let ring = RingBuffer::<16>::new();
        ring.push(|record| write!(record, "abc"))?;
        let (first, second, length) = ring.peek().unwrap();
        assert_eq!(first, b"abc")?;
        assert_eq!(second, b"")?;
        let (first, _, _) = ring.peek().unwrap();
        assert_eq!(first, b"abc")?;
        ring.commit(length);
        assert_eq!(ring.peek().is_none(), true)?;
        Ok(())
    }

    #[test]
    fn test_is_consumed() -> TestResult {
        let ring = RingBuffer::<16>::new();
        let target = ring.push(|record| write!(record, "abc"))?;
        assert_eq!(ring.is_consumed(target), false)?;
        let (_, _, length) = ring.peek().unwrap();
        ring.commit(length);
        assert_eq!(ring.is_consumed(target), true)?;
        Ok(())
    }

    #[test]
    fn test_multiple_records() -> TestResult {
        let ring = RingBuffer::<64>::new();
        ring.push(|record| write!(record, "one"))?;
        ring.push(|record| write!(record, "two"))?;
        let mut bytes = [0u8; 64];
        let length = take(&ring, &mut bytes).unwrap();
        assert_eq!(&bytes[..length], b"one")?;
        let length = take(&ring, &mut bytes).unwrap();
        assert_eq!(&bytes[..length], b"two")?;
        Ok(())
    }

    #[test]
    fn test_exact_fit_and_drop() -> TestResult {
        let ring = RingBuffer::<8>::new();
        ring.push(|record| write!(record, "12345"))?;
        assert_eq!(ring.overflow_count(), 0)?;
        ring.push(|record| write!(record, "x"))?;
        assert_eq!(ring.overflow_count(), 1)?;
        let mut bytes = [0u8; 8];
        let length = take(&ring, &mut bytes).unwrap();
        assert_eq!(&bytes[..length], b"12345")?;
        ring.push(|record| write!(record, "abcde"))?;
        let length = take(&ring, &mut bytes).unwrap();
        assert_eq!(&bytes[..length], b"abcde")?;
        Ok(())
    }

    #[test]
    fn test_drop_when_too_large() -> TestResult {
        let ring = RingBuffer::<16>::new();
        ring.push(|record| write!(record, "abcdefghijklmnopqrst"))?;
        assert_eq!(ring.overflow_count(), 1)?;
        assert_eq!(ring.peek().is_none(), true)?;
        ring.push(|record| write!(record, "ok"))?;
        let mut bytes = [0u8; 16];
        let length = take(&ring, &mut bytes).unwrap();
        assert_eq!(&bytes[..length], b"ok")?;
        Ok(())
    }

    #[test]
    fn test_record_wraps() -> TestResult {
        let ring = RingBuffer::<16>::new();
        ring.push(|record| write!(record, "aa"))?;
        ring.push(|record| write!(record, "bb"))?;
        ring.push(|record| write!(record, "cc"))?;
        let mut bytes = [0u8; 16];
        let length = take(&ring, &mut bytes).unwrap();
        assert_eq!(&bytes[..length], b"aa")?;
        let length = take(&ring, &mut bytes).unwrap();
        assert_eq!(&bytes[..length], b"bb")?;
        ring.push(|record| write!(record, "123456789"))?;
        let length = take(&ring, &mut bytes).unwrap();
        assert_eq!(&bytes[..length], b"cc")?;
        let (first, second, length) = ring.peek().unwrap();
        assert_eq!(first, b"12")?;
        assert_eq!(second, b"3456789")?;
        ring.commit(length);
        Ok(())
    }

    #[test]
    fn test_send() -> TestResult {
        fn assert_send<T: Send>() {}
        assert_send::<RingBuffer<1024>>();
        Ok(())
    }
}
