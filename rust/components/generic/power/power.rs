#![no_std]
#![no_main]

pub use rust_components_generic_power_interface::BzdComponentsGenericPowerInterface;
use rust_interfaces_power::BzdPowerInterface;
use rust_libs_power::Power as PowerState;

/// Shared power component for all targets.
pub struct BzdComponentsGenericPower;

impl BzdComponentsGenericPower {
    #[must_use]
    pub fn new() -> Self {
        Self
    }
}

impl Default for BzdComponentsGenericPower {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdComponentsGenericPowerInterface for BzdComponentsGenericPower {}

impl BzdPowerInterface for BzdComponentsGenericPower {
    fn accessor(&self) -> &'static PowerState {
        rust_libs_power::power()
    }
}
