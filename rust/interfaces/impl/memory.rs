#![no_std]

use bzd::base::error::Error;

/// Provides long-lived memory allocations.
pub trait BzdMemoryInterface {
    /// Number of bytes this memory can provide.
    fn capacity(&self) -> usize;

    /// Allocates `size` bytes.
    ///
    /// The returned memory lives for the remainder of the program and is never
    /// freed.
    fn alloc(&mut self, size: usize) -> Result<&'static mut [u8], Error>;
}
