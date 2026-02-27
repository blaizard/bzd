#![no_std]
#![no_main]

//use esp_backtrace as _;
use bzd::base::panic::PanicPrint;
use esp_hal::{main, system::software_reset};
use esp_println::{print, println};

unsafe extern "Rust" {
    fn run_executor() -> bool;
}

#[panic_handler]
fn panic(info: &core::panic::PanicInfo) -> ! {
    println!();
    println!("{}", PanicPrint { info });

    println!();
    print!("Backtrace:");
    let backtrace = esp_backtrace::Backtrace::capture();
    for frame in backtrace.frames() {
        print!(" 0x{:x}", frame.program_counter());
    }
    println!();

    println!("<exit code 1>");
    software_reset();
}

#[main]
fn main() -> ! {
    let result = unsafe { run_executor() };

    println!("<exit code 0>");

    software_reset();
    loop {}
}
