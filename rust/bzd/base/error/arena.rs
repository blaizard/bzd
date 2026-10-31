//! Fixed-size pool used to store formatted error messages without allocation.
//!
//! A slot is exclusively owned by an [`SlotGuard`] between `claim` and `commit`,
//! and by the [`Error`](crate::Error) holding it afterwards. `Error::drop` releases
//! the slot, so the borrow checker prevents a message from outliving its slot.

use core::cell::UnsafeCell;
use core::fmt;
use core::sync::atomic::{AtomicUsize, Ordering};

/// Number of bytes available to store a single formatted message.
pub(crate) const MESSAGE_SIZE: usize = 256;

/// Number of bytes available for the message itself, reserving room for the
/// `...` suffix appended to truncated messages.
pub(crate) const MESSAGE_CAPACITY: usize = MESSAGE_SIZE - 3;

/// Number of formatted messages that can be alive at the same time.
pub(crate) const SLOT_COUNT: usize = 4;

/// Static message used when all slots are already in use.
pub(crate) const MESSAGE_POOL_EXHAUSTED: &str = "<error message pool exhausted>";

const ALL_FREE: usize = (1usize << SLOT_COUNT) - 1;

const _: () = assert!(MESSAGE_SIZE >= 3);
const _: () = assert!(SLOT_COUNT <= u8::MAX as usize);
const _: () = assert!(SLOT_COUNT <= usize::BITS as usize);

struct Slot {
    data: UnsafeCell<[u8; MESSAGE_SIZE]>,
}

struct Arena {
    slots: [Slot; SLOT_COUNT],
    free: AtomicUsize,
}

// Safety: the `free` bitmask is the only shared mutable state and is accessed
// atomically. Slot data is accessed exclusively: a slot is only handed out by
// `claim` once and is not touched by anyone else until it is released.
unsafe impl Sync for Arena {}

impl Arena {
    const fn new() -> Self {
        Self {
            slots: [const {
                Slot {
                    data: UnsafeCell::new([0; MESSAGE_SIZE]),
                }
            }; SLOT_COUNT],
            free: AtomicUsize::new(ALL_FREE),
        }
    }
}

static ARENA: Arena = Arena::new();

/// Exclusive ownership of a pool slot, released on drop unless committed.
pub(crate) struct SlotGuard {
    slot: usize,
    committed: bool,
}

impl SlotGuard {
    /// Claim a free slot, returning `None` if the pool is exhausted.
    pub(crate) fn claim() -> Option<Self> {
        let mut free = ARENA.free.load(Ordering::Relaxed);
        loop {
            if free == 0 {
                return None;
            }
            let slot = free.trailing_zeros() as usize;
            match ARENA.free.compare_exchange_weak(
                free,
                free & !(1 << slot),
                Ordering::Acquire,
                Ordering::Relaxed,
            ) {
                Ok(_) => {
                    return Some(Self {
                        slot,
                        committed: false,
                    });
                }
                Err(actual) => free = actual,
            }
        }
    }

    /// Format a message into the slot and return its length.
    ///
    /// A message that does not fit is truncated at a character boundary and
    /// suffixed with `...`.
    pub(crate) fn write(&mut self, args: fmt::Arguments<'_>) -> usize {
        let mut writer = MessageWriter {
            data: self.data_mut(),
            length: 0,
        };
        let truncated = fmt::write(&mut writer, args).is_err();
        writer.finish(truncated)
    }

    fn data_mut(&mut self) -> &mut [u8; MESSAGE_SIZE] {
        // Safety: the slot is exclusively owned by this guard and `&mut self`
        // prevents any other access while the returned borrow is alive.
        unsafe { &mut *ARENA.slots[self.slot].data.get() }
    }

    /// Give up ownership of the slot without releasing it.
    ///
    /// The caller becomes responsible for calling [`release`] when done.
    pub(crate) fn commit(mut self) -> usize {
        self.committed = true;
        self.slot
    }
}

impl Drop for SlotGuard {
    fn drop(&mut self) {
        if !self.committed {
            release(self.slot);
        }
    }
}

/// A `core::fmt::Write` sink formatting a message into a slot.
struct MessageWriter<'a> {
    data: &'a mut [u8; MESSAGE_SIZE],
    length: usize,
}

impl MessageWriter<'_> {
    fn finish(mut self, truncated: bool) -> usize {
        if truncated {
            self.data[self.length..self.length + 3].copy_from_slice(b"...");
            self.length += 3;
        }
        self.length
    }
}

impl fmt::Write for MessageWriter<'_> {
    fn write_str(&mut self, text: &str) -> fmt::Result {
        let available = MESSAGE_CAPACITY - self.length;
        let mut length = text.len().min(available);
        while !text.is_char_boundary(length) {
            length -= 1;
        }
        self.data[self.length..self.length + length].copy_from_slice(&text.as_bytes()[..length]);
        self.length += length;
        if length < text.len() {
            Err(fmt::Error)
        } else {
            Ok(())
        }
    }
}

/// Return a slot to the pool.
pub(crate) fn release(slot: usize) {
    ARENA.free.fetch_or(1 << slot, Ordering::Release);
}

/// Read the data of a slot owned by the caller.
pub(crate) fn data(slot: usize) -> &'static [u8; MESSAGE_SIZE] {
    // Safety: the caller owns the slot until it releases it.
    unsafe { &*ARENA.slots[slot].data.get() }
}

#[cfg(test)]
pub(crate) fn available() -> usize {
    ARENA.free.load(Ordering::Relaxed).count_ones() as usize
}
