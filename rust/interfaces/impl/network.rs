#![no_std]
#![allow(async_fn_in_trait)]

use bzd::base::error::Error;
use bzd::base::stream::{IStream, OStream};
use core::net::{IpAddr, SocketAddr};

/// Marker for a network provider that can be injected into network components.
pub trait BzdNetworkInterface {}

/// Resolves hostnames to IP addresses.
pub trait BzdNetworkDnsInterface {
    /// Resolve `hostname` into an address.
    async fn resolve(&self, hostname: &str) -> Result<IpAddr, Error>;
}

/// Opens outbound TCP connections.
pub trait BzdNetworkTcpInterface {
    /// Bidirectional byte stream owned by the connection.
    ///
    /// Must own its state or borrow static data: callers release the component
    /// handle as soon as `connect` returns.
    type Stream: IStream + OStream + 'static;

    /// Connect to `address` and return the resulting stream.
    async fn connect(&self, address: SocketAddr) -> Result<Self::Stream, Error>;
}
