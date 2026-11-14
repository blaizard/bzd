#![no_std]

use bzd::base::error::Error;
use esp_hal::psram::{Psram, PsramConfig, PsramSize};
use rust_components_esp_hal::init as esp_hal_init;
pub use rust_components_esp_memory_psram_interface::{
    BzdComponentsEspMemoryPsramContext, BzdComponentsEspMemoryPsramInterface,
};
use rust_interfaces_memory::BzdMemoryInterface;

/// Cache line size, used to keep allocations aligned.
const ALIGNMENT: usize = 64;

/// External PSRAM exposed through the [`BzdMemoryInterface`] interface.
pub struct BzdComponentsEspMemoryPsram {
    context: BzdComponentsEspMemoryPsramContext,
    psram: Option<Psram>,
    base: usize,
    capacity: usize,
    offset: usize,
}

impl BzdComponentsEspMemoryPsram {
    pub fn new(context: BzdComponentsEspMemoryPsramContext) -> Self {
        Self {
            context,
            psram: None,
            base: 0,
            capacity: 0,
            offset: 0,
        }
    }
}

impl BzdComponentsEspMemoryPsramInterface for BzdComponentsEspMemoryPsram {
    async fn init(&mut self) -> Result<(), Error> {
        if self.psram.is_some() {
            return Ok(());
        }

        let peripherals = esp_hal_init();
        let mut config = PsramConfig::default();
        if self.context.size > 0 {
            config.size = PsramSize::Size(self.context.size as usize);
        }

        // Mapping the PSRAM suspends the data cache, so no interrupt handler
        // may run from flash while the MMU is being updated.
        let psram = critical_section::with(|_| Psram::new(peripherals.PSRAM.reborrow(), config));
        let (base, capacity) = psram.raw_parts();
        if capacity == 0 {
            return Err(bzd::base::error::failure!(
                "PSRAM memory: no PSRAM detected"
            ));
        }

        self.psram = Some(psram);
        self.base = base as usize;
        self.capacity = capacity;
        self.offset = 0;
        Ok(())
    }
}

impl BzdMemoryInterface for BzdComponentsEspMemoryPsram {
    fn capacity(&self) -> usize {
        self.capacity
    }

    fn alloc(&mut self, size: usize) -> Result<&'static mut [u8], Error> {
        if size == 0 {
            return Ok(&mut []);
        }
        if self.psram.is_none() {
            return Err(bzd::base::error::failure!("PSRAM memory: not initialized"));
        }

        let start = align_up(
            self.base
                .checked_add(self.offset)
                .ok_or_else(|| bzd::base::error::failure!("PSRAM memory: allocation overflow"))?,
            ALIGNMENT,
        );
        let end = start
            .checked_add(size)
            .ok_or_else(|| bzd::base::error::failure!("PSRAM memory: allocation overflow"))?;
        let limit = self.base + self.capacity;
        if end > limit {
            return Err(bzd::base::error::failure!(
                "PSRAM memory: {} bytes requested, {} bytes available",
                size,
                limit.saturating_sub(start)
            ));
        }
        self.offset = end - self.base;

        // SAFETY: the region is mapped for the whole program and the bump
        // allocator hands out disjoint ranges.
        let pointer = start as *mut u8;
        unsafe { core::ptr::write_bytes(pointer, 0, size) };
        Ok(unsafe { core::slice::from_raw_parts_mut(pointer, size) })
    }
}

fn align_up(value: usize, alignment: usize) -> usize {
    value.div_ceil(alignment) * alignment
}
