#![no_std]
#![no_main]

pub use rust_components_generic_power_interface::BzdComponentsGenericPowerInterface;

use rust_libs_power::{BzdPowerInterface, LightSleep, PowerHandle};

pub struct YieldLightSleep;

impl LightSleep for YieldLightSleep {
    const DEFAULT: Self = Self;

    async fn request_sleep(&self) {
        embassy_futures::yield_now().await;
    }
}

pub struct BzdComponentsGenericPower(PowerHandle<YieldLightSleep>);

impl BzdComponentsGenericPower {
    #[must_use]
    pub const fn new() -> Self {
        Self(PowerHandle::new())
    }
}

impl Default for BzdComponentsGenericPower {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdComponentsGenericPowerInterface for BzdComponentsGenericPower {}

impl BzdPowerInterface for BzdComponentsGenericPower {
    type LightSleepImpl = YieldLightSleep;

    fn power_handle(&self) -> &PowerHandle<YieldLightSleep> {
        &self.0
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;
    use bzd::sync::rwlock::RwLock;

    #[test]
    fn component_forwards_power_interface() -> TestResult {
        let power = BzdComponentsGenericPower::new();
        assert_eq!(power.is_deep_sleep(), false)?;
        Ok(())
    }

    #[test]
    async fn multiple_readers_can_prevent_deep_sleep() -> TestResult {
        let power = RwLock::new(BzdComponentsGenericPower::new());
        let first = power.read().await;
        let second = power.read().await;
        assert_eq!(first.prevent_deep_sleep(async { 1 }).await, 1)?;
        assert_eq!(second.prevent_deep_sleep(async { 2 }).await, 2)?;
        Ok(())
    }
}
