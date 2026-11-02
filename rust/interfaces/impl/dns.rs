#![no_std]
#![allow(async_fn_in_trait)]

use bzd::base::error::Error;
use core::net::IpAddr;

/// Resolves hostnames to IP addresses.
pub trait BzdNetworkDnsInterface {
    /// Resolve `hostname` into an address.
    async fn resolve(&self, hostname: &str) -> Result<IpAddr, Error>;
}
