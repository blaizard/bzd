#![no_std]
#![no_main]

//use esp_backtrace as _;
use esp_hal::{main, system::software_reset};

unsafe extern "Rust" {
    fn run_executor() -> bool;
}

#[panic_handler]
fn panic(_: &core::panic::PanicInfo) -> ! {
    esp_println::println!("<exit code 1>");
    software_reset();
}

#[main]
fn main() -> ! {
    let result = unsafe { run_executor() };

    esp_println::println!("<exit code 0>");

    software_reset();
    loop {}
}
