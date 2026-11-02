#![no_std]

pub use rust_example_http_composition::{
    BzdExampleHttpGetContext, BzdExampleHttpGetContextConstraint,
    BzdExampleHttpGetContextConstraintTypes, BzdExampleHttpGetInterface,
};

use bzd_http::{request, Method, Request};
use core::net::SocketAddr;
use interfaces_stream::BzdOStreamInterface;
use interfaces_tcp::BzdNetworkTcpClientInterface;
use rust_interfaces_dns::BzdNetworkDnsInterface;

const HOSTNAME: &str = "example.com";
const PORT: u16 = 80;
const PATH: &str = "/";

pub struct BzdExampleHttpGet<C>
where
    C: BzdExampleHttpGetContextConstraint,
{
    pub context: BzdExampleHttpGetContext<C>,
}

impl<C> BzdExampleHttpGet<C>
where
    C: BzdExampleHttpGetContextConstraint,
{
    pub fn new(context: BzdExampleHttpGetContext<C>) -> Self {
        Self { context }
    }
}

impl<C> BzdExampleHttpGetInterface for BzdExampleHttpGet<C>
where
    C: BzdExampleHttpGetContextConstraint,
{
    async fn run(&mut self) -> Result<(), bzd::base::error::Error> {
        let address = self.context.dns.lock().await.resolve(HOSTNAME).await?;

        let mut stream = self
            .context
            .tcp
            .lock()
            .await
            .connect(SocketAddr::new(address, PORT))
            .await?;

        let mut buffer = [0u8; 4096];
        let response = request(
            &mut stream,
            Request {
                method: Method::Get,
                host: HOSTNAME,
                path: PATH,
                headers: &[],
                body: &[],
            },
            &mut buffer,
        )
        .await?;

        let mut out = self.context.out.lock().await;
        out.write_all(&buffer[..response.body_length]).await?;
        out.flush().await?;

        Ok(())
    }
}
