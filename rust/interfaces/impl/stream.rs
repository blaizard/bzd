#![no_std]
#![allow(async_fn_in_trait)]

use error::Error;

pub trait BzdOStreamInterface {
    async fn write(&mut self, data: &[u8]) -> Result<(), Error>;
}
