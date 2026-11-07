#![no_std]
#![allow(async_fn_in_trait)]

use bzd::base::error::Error;
use bzd::base::stream::{IStream, OStream};
use core::net::SocketAddr;

/// Opens outbound TLS connections.
pub trait BzdNetworkTlsClientInterface {
    /// Bidirectional byte stream owned by the connection.
    ///
    /// Must own its state or borrow static data: callers release the component
    /// handle as soon as `connect` returns.
    type Stream: IStream + OStream + 'static;

    /// Connect to `address` and negotiate TLS for `server_name`.
    ///
    /// `server_name` is used for SNI and certificate hostname verification.
    async fn connect(&self, address: SocketAddr, server_name: &str) -> Result<Self::Stream, Error>;
}
