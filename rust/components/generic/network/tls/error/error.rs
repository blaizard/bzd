#![no_std]
#![no_main]

use bzd::base::error::Error;
use bzd::base::stream::{IStream, OStream};
use core::net::SocketAddr;
pub use rust_components_generic_network_tls_error_interface::BzdComponentsGenericNetworkTlsErrorInterface;
use rust_interfaces_tls::BzdNetworkTlsClientInterface;

/// TLS client used when no TLS implementation is configured.
pub struct BzdComponentsGenericNetworkTlsError;

impl BzdComponentsGenericNetworkTlsError {
    pub fn new() -> Self {
        Self
    }
}

impl Default for BzdComponentsGenericNetworkTlsError {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdComponentsGenericNetworkTlsErrorInterface for BzdComponentsGenericNetworkTlsError {}

/// Stream returned by the error TLS client.
pub struct ErrorStream;

impl IStream for ErrorStream {
    type Chunk<'a>
        = &'a [u8]
    where
        Self: 'a;

    async fn read<'a>(&'a mut self, _buffer: &'a mut [u8]) -> Result<Self::Chunk<'a>, Error> {
        Err(bzd::base::error::failure!("tls: no TLS client configured"))
    }
}

impl OStream for ErrorStream {
    async fn write(&mut self, _data: &[u8]) -> Result<usize, Error> {
        Err(bzd::base::error::failure!("tls: no TLS client configured"))
    }
}

impl BzdNetworkTlsClientInterface for BzdComponentsGenericNetworkTlsError {
    type Stream = ErrorStream;

    async fn connect(
        &self,
        _address: SocketAddr,
        _server_name: &str,
    ) -> Result<Self::Stream, Error> {
        Err(bzd::base::error::failure!(
            "https: no TLS client configured"
        ))
    }
}
