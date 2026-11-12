#![no_std]
#![no_main]
#![allow(async_fn_in_trait)]

pub use rust_components_generic_network_http_interface::{
    BzdComponentsGenericNetworkHttpContext, BzdComponentsGenericNetworkHttpContextConstraint,
    BzdComponentsGenericNetworkHttpContextConstraintTypes,
    BzdComponentsGenericNetworkHttpInterface,
};

use bzd::base::error::Error;
use bzd_http::{Request, ResponseInfo, Scheme};
use core::net::SocketAddr;
use interfaces_tcp::BzdNetworkTcpClientInterface;
use rust_interfaces_dns::BzdNetworkDnsInterface;
use rust_interfaces_tls::BzdNetworkTlsClientInterface;

/// Performs HTTP requests over the component's network context.
pub trait BzdNetworkHttpClientInterface {
    /// Send `request` over the network and read the full response body.
    ///
    /// The buffer is used as scratch space for the response headers and then holds the
    /// response body, available as `buffer[..body_length]`.
    async fn send(&self, request: Request<'_>, buffer: &mut [u8]) -> Result<ResponseInfo, Error>;
}

/// HTTP client component.
pub struct BzdComponentsGenericNetworkHttp<C>
where
    C: BzdComponentsGenericNetworkHttpContextConstraint,
{
    pub context: BzdComponentsGenericNetworkHttpContext<C>,
}

impl<C> BzdComponentsGenericNetworkHttp<C>
where
    C: BzdComponentsGenericNetworkHttpContextConstraint,
{
    pub fn new(context: BzdComponentsGenericNetworkHttpContext<C>) -> Self {
        Self { context }
    }
}

impl<C> BzdNetworkHttpClientInterface for BzdComponentsGenericNetworkHttp<C>
where
    C: BzdComponentsGenericNetworkHttpContextConstraint,
{
    async fn send(&self, request: Request<'_>, buffer: &mut [u8]) -> Result<ResponseInfo, Error> {
        let url = request.url();
        let address = SocketAddr::new(
            self.context.dns.lock().await.resolve(url.host()).await?,
            url.port(),
        );

        match url.scheme() {
            Scheme::Http => {
                let mut stream = self.context.tcp.lock().await.connect(address).await?;
                request.send(&mut stream, buffer).await
            }
            Scheme::Https => {
                let mut stream = self
                    .context
                    .tls
                    .lock()
                    .await
                    .connect(address, url.host())
                    .await?;
                request.send(&mut stream, buffer).await
            }
        }
    }
}

impl<C> BzdComponentsGenericNetworkHttpInterface for BzdComponentsGenericNetworkHttp<C> where
    C: BzdComponentsGenericNetworkHttpContextConstraint
{
}
