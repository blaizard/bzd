#![no_std]
#![no_main]

use core::sync::atomic::{AtomicU32, Ordering};
use esp_backtrace as _;
use esp_hal::{
    handler,
    interrupt::{self, Priority},
    main,
    time::Duration,
    timer::{timg::TimerGroup, Timer},
    Config,
};
use esp_println::println;

esp_bootloader_esp_idf::esp_app_desc!();

static FIRE_COUNT: AtomicU32 = AtomicU32::new(0);

#[handler(priority = Priority::Priority1)]
fn tm0_handler() {
    FIRE_COUNT.fetch_add(1, Ordering::Relaxed);
}

#[panic_handler]
fn panic(info: &core::panic::PanicInfo) -> ! {
    println!("PANIC: {}", info);
    loop {}
}

#[main]
fn main() -> ! {
    println!("Booted");

    let peripherals = esp_hal::init(Config::default());
    let timg0 = TimerGroup::new(peripherals.TIMG0);
    let timer0 = timg0.timer0;

    timer0.load_value(Duration::from_secs(1)).unwrap();
    timer0.enable_auto_reload(true);
    timer0.set_interrupt_handler(tm0_handler);
    timer0.start();
    timer0.enable_interrupt(true);

    println!("Timer configured");

    let mut last_seen = 0u32;
    loop {
        let count = FIRE_COUNT.load(Ordering::Relaxed);
        if count != last_seen {
            println!("Interrupt {}", count);
            last_seen = count;
        }
    }
}
