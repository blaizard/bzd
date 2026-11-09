#![no_std]
#![no_main]
#![allow(async_fn_in_trait)]

pub use rust_components_generic_network_http_interface::{
    BzdComponentsGenericNetworkHttpContext, BzdComponentsGenericNetworkHttpContextConstraint,
    BzdComponentsGenericNetworkHttpContextConstraintTypes,
    BzdComponentsGenericNetworkHttpInterface,
};

use bzd::base::error::Error;
use bzd_http::{Method, Request, ResponseInfo, Scheme, Url};
use core::net::SocketAddr;
use interfaces_tcp::BzdNetworkTcpClientInterface;
use rust_interfaces_dns::BzdNetworkDnsInterface;
use rust_interfaces_tls::BzdNetworkTlsClientInterface;

/// Performs HTTP requests over the component's network context.
pub trait BzdNetworkHttpClientInterface {
    async fn request(
        &self,
        method: Method,
        url: &str,
        headers: &[(&str, &str)],
        body: &[u8],
        buffer: &mut [u8],
    ) -> Result<ResponseInfo, Error>;
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
    async fn request(
        &self,
        method: Method,
        url: &str,
        headers: &[(&str, &str)],
        body: &[u8],
        buffer: &mut [u8],
    ) -> Result<ResponseInfo, Error> {
        let url = Url::parse(url)?;
        let address = SocketAddr::new(
            self.context.dns.lock().await.resolve(url.host()).await?,
            url.port(),
        );
        let request = Request {
            method,
            url,
            headers,
            body,
        };

        match request.url.scheme() {
            Scheme::Http => {
                let mut stream = self.context.tcp.lock().await.connect(address).await?;
                bzd_http::request(&mut stream, request, buffer).await
            }
            Scheme::Https => {
                let mut stream = self
                    .context
                    .tls
                    .lock()
                    .await
                    .connect(address, request.url.host())
                    .await?;
                bzd_http::request(&mut stream, request, buffer).await
            }
        }
    }
}

impl<C> BzdComponentsGenericNetworkHttpInterface for BzdComponentsGenericNetworkHttp<C> where
    C: BzdComponentsGenericNetworkHttpContextConstraint
{
}
