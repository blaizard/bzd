#![cfg_attr(target_os = "none", no_std)]
#![no_main]

use embassy as _;
use embassy_executor::{task, Spawner};

#[task]
async fn hello_task() {
    embassy::exit(0);
}

#[unsafe(no_mangle)]
pub fn run_embassy(spawner: Spawner) {
    spawner.spawn(hello_task()).unwrap();
}
