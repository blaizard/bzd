#![no_std]
#![allow(async_fn_in_trait)]

use core::time::Duration;
use error::Error;

pub trait BzdTimerInterface {
    async fn delay(&mut self, duration: Duration) -> Result<(), Error>;
}
