#![no_std]
#![no_main]

use bzd::base::panic::PanicPrint;
use embassy_executor::Spawner;
use embassy_sync::blocking_mutex::raw::CriticalSectionRawMutex;
use embassy_sync::signal::Signal;
use esp_hal::system::software_reset;
use esp_println::{print, println};

esp_bootloader_esp_idf::esp_app_desc!();

static FINISHED_SIGNAL: Signal<CriticalSectionRawMutex, ()> = Signal::new();
unsafe extern "Rust" {
    fn run_executor(spawner: Spawner);
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

    exit(1);
}

pub fn exit(code: i32) -> ! {
    println!("<exit code {}>", code);
    software_reset()
}

#[esp_rtos::main]
async fn main(spawner: Spawner) -> ! {
    unsafe { run_executor(spawner) };
    FINISHED_SIGNAL.wait().await;
    exit(0);
}
