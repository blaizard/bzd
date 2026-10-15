#![no_std]

use core::time::Duration;
use embassy_time::{Duration as EmbassyDuration, Timer as EmbassyTimer};
use interfaces_timer::BzdTimerInterface;

pub struct BzdComponentsEmbassyTimer;

impl Default for BzdComponentsEmbassyTimer {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdComponentsEmbassyTimer {
    pub fn new() -> Self {
        Self
    }
}

impl BzdTimerInterface for BzdComponentsEmbassyTimer {
    async fn delay(&mut self, duration: Duration) -> Result<(), bzd::base::error::Error> {
        let duration_us = u64::try_from(duration.as_micros())
            .map_err(|_| bzd::base::error::failure("duration exceeds Embassy timer range"))?;
        EmbassyTimer::after(EmbassyDuration::from_micros(duration_us)).await;
        Ok(())
    }
}
