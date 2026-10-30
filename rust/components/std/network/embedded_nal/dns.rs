use core::net::IpAddr;
use embedded_nal_async::{AddrType, Dns as _};
use rust_interfaces_network::BzdDnsInterface;
use std_embedded_nal_async::Stack;

pub struct BzdComponentsStdNetworkNalDns {
    stack: Stack,
}

impl BzdComponentsStdNetworkNalDns {
    pub fn new() -> Self {
        Self {
            stack: Stack::default(),
        }
    }
}

impl Default for BzdComponentsStdNetworkNalDns {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdDnsInterface for BzdComponentsStdNetworkNalDns {
    async fn resolve(&self, hostname: &str) -> Result<IpAddr, bzd::base::error::Error> {
        self.stack
            .get_host_by_name(hostname, AddrType::Either)
            .await
            .map_err(|_| bzd::base::error::failure("failed to resolve hostname"))
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    fn test_new() -> TestResult {
        let _dns = BzdComponentsStdNetworkNalDns::new();
        Ok(())
    }
}
