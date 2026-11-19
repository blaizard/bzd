#![no_std]
#![no_main]

use bzd::base::panic::PanicPrint;
use embassy_executor::Spawner;
// Forces the global allocator from esp-alloc to be linked.
use esp_alloc as _;
use esp_hal::rtc_cntl::sleep::{LowPower, RtcSleepConfig};
use esp_hal::system::software_reset;
use esp_hal::timer::timg::TimerGroup;
use esp_println::{print, println};
use rust_components_esp_hal::init as esp_hal_init;

use composition::runExecutor;

esp_bootloader_esp_idf::esp_app_desc!();

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
async fn main(_spawner: Spawner) -> ! {
    let peripherals = esp_hal_init();
    let timer_group = TimerGroup::new(peripherals.TIMG0.reborrow());
    esp_rtos::start(timer_group.timer0, peripherals.FROM_CPU_INTR0.reborrow());

    match runExecutor().await {
        Ok(()) => {
            if let Some(request) = rust_libs_power::power().deep_sleep_requested() {
                let mut low_power = LowPower::new(peripherals.LPWR.reborrow());
                if let Some(duration) = request.duration() {
                    let micros = u64::try_from(duration.as_micros()).unwrap_or(u64::MAX);
                    low_power.set_wakeup_deadline(
                        esp_hal::time::Instant::now()
                            + esp_hal::time::Duration::from_micros(micros),
                    );
                }
                low_power.sleep_deep(RtcSleepConfig::deep())
            }
            exit(0)
        }
        Err(e) => {
            println!("executor() failed: {e}");
            exit(1);
        }
    }
}
