#![no_std]
#![no_main]

use main as _;

#[unsafe(no_mangle)]
pub fn run_executor() -> bool {
    true
}
