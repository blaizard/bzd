#![no_std]
#![allow(async_fn_in_trait)]

pub trait BzdOStreamInterface {
    async fn write(&mut self, data: &[u8]) -> Result<(), bzd::base::error::Error>;
}
