#![no_std]
#![allow(async_fn_in_trait)]

use bzd::base::error::Error;
use core::time::Duration;

pub trait BzdTimerInterface {
    async fn delay(&mut self, duration: Duration) -> Result<(), Error>;
}
