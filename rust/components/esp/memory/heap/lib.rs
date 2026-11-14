#![no_std]

use bzd::base::error::Error;
use core::alloc::Layout;
use esp_alloc::MemoryCapability;
pub use rust_components_esp_memory_heap_interface::{
    BzdComponentsEspMemoryHeapContext, BzdComponentsEspMemoryHeapInterface,
};
use rust_interfaces_memory::BzdMemoryInterface;

/// Cache line size, used to keep allocations aligned.
const ALIGNMENT: usize = 64;

/// Internal heap memory exposed through the [`BzdMemoryInterface`] interface.
///
/// Allocations are served from the global `esp-alloc` heap and never freed.
/// The platform must have registered an internal heap region before this
/// component is used.
pub struct BzdComponentsEspMemoryHeap {
    context: BzdComponentsEspMemoryHeapContext,
    allocated: usize,
}

impl BzdComponentsEspMemoryHeap {
    pub fn new(context: BzdComponentsEspMemoryHeapContext) -> Self {
        Self {
            context,
            allocated: 0,
        }
    }
}

impl BzdComponentsEspMemoryHeapInterface for BzdComponentsEspMemoryHeap {
    async fn init(&mut self) -> Result<(), Error> {
        self.allocated = 0;
        if esp_alloc::HEAP.stats().size == 0 {
            return Err(bzd::base::error::failure!(
                "Heap memory: no internal heap region configured"
            ));
        }
        Ok(())
    }
}

impl BzdMemoryInterface for BzdComponentsEspMemoryHeap {
    fn capacity(&self) -> usize {
        self.context.size as usize
    }

    fn alloc(&mut self, size: usize) -> Result<&'static mut [u8], Error> {
        if size == 0 {
            return Ok(&mut []);
        }

        let available = self.capacity().saturating_sub(self.allocated);
        if size > available {
            return Err(bzd::base::error::failure!(
                "Heap memory: {} bytes requested, {} bytes available",
                size,
                available
            ));
        }

        let layout = Layout::from_size_align(size, ALIGNMENT)
            .map_err(|_| bzd::base::error::failure!("Heap memory: invalid size {}", size))?;
        // SAFETY: the layout has a non-zero size, checked above.
        let pointer =
            unsafe { esp_alloc::HEAP.alloc_caps(MemoryCapability::Internal.into(), layout) };
        if pointer.is_null() {
            return Err(bzd::base::error::failure!(
                "Heap memory: allocation of {} bytes failed",
                size
            ));
        }
        self.allocated += size;

        // SAFETY: the allocation is owned by this component and never freed.
        unsafe { core::ptr::write_bytes(pointer, 0, size) };
        Ok(unsafe { core::slice::from_raw_parts_mut(pointer, size) })
    }
}
