#![no_std]
#![no_main]

pub use rust_example_http_composition::{
    BzdExampleHttpExampleContext, BzdExampleHttpExampleContextConstraint,
    BzdExampleHttpExampleContextConstraintTypes, BzdExampleHttpExampleInterface,
};

use bzd::base::error::Error;
use bzd_http::Method;
use interfaces_stream::BzdOStreamInterface;
use rust_components_generic_network_http::BzdNetworkHttpClientInterface;

const BUFFER_SIZE: usize = 4096;

pub struct BzdExampleHttpExample<C>
where
    C: BzdExampleHttpExampleContextConstraint,
{
    pub context: BzdExampleHttpExampleContext<C>,
}

impl<C> BzdExampleHttpExample<C>
where
    C: BzdExampleHttpExampleContextConstraint,
{
    pub fn new(context: BzdExampleHttpExampleContext<C>) -> Self {
        Self { context }
    }
}

impl<C> BzdExampleHttpExampleInterface for BzdExampleHttpExample<C>
where
    C: BzdExampleHttpExampleContextConstraint,
    C::Http: BzdNetworkHttpClientInterface,
{
    async fn run(&mut self) -> Result<(), Error> {
        let http = self.context.http.lock().await;
        let mut out = self.context.out.lock().await;
        let mut buffer = [0u8; BUFFER_SIZE];

        let response = http
            .request(Method::Get, "http://example.com/", &[], &[], &mut buffer)
            .await?;
        out.write_all(&buffer[..response.body_length]).await?;

        let response = http
            .request(
                Method::Get,
                "https://www.google.com/humans.txt",
                &[],
                &[],
                &mut buffer,
            )
            .await?;
        out.write_all(&buffer[..response.body_length]).await?;

        out.flush().await?;

        Ok(())
    }
}
