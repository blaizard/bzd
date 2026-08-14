#![no_std]
#![allow(async_fn_in_trait)]

pub trait BzdOStream {
    async fn write(&mut self, data: &[u8]);
}
