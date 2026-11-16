#![no_std]
#![no_main]

use rust_libs_power::Power;

/// Gives access to the target-wide power state.
pub trait BzdPowerInterface {
    /// Get the shared power state.
    fn accessor(&self) -> &'static Power;
}
