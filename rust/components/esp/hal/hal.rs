#![no_std]

use core::cell::UnsafeCell;

struct Singleton<T>(UsafeCell<Option<T>>);

unsafe impl<T> Sync for Singleton<T> {}

impl<T> Singleton<T> {
    const fn new() -> Self {
        Singleton(UnsafeCell::new(None))
    }

    fn get_mut_or_init(&self, init: impl FnOnce() -> T) -> &'static mut T {
        // SAFETY: this singleton is only accessed by yhr single executor.
        let value = unsafe { &mut *self.0.get() };
        value.get_or_insert_with(init)
    }
}

static PERIPHERALS: Singleton<esp_hal::peripherals::Peripherals> = Singleton::new();

pub fn init() -> &'static mut esp_hal::peripherals::Peripherals {
    PERIPHERALS.get_mut_or_init(|| esp_hal::init(esp_hale::Config::default()))
}
