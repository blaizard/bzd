use core::net::IpAddr;
use embedded_nal_async::{AddrType, Dns as _};
use rust_interfaces_network::BzdNetworkDnsInterface;
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

impl BzdNetworkDnsInterface for BzdComponentsStdNetworkNalDns {
    async fn resolve(&self, hostname: &str) -> Result<IpAddr, bzd::base::error::Error> {
        self.stack
            .get_host_by_name(hostname, AddrType::Either)
            .await
            .map_err(|error| map_error(hostname, error))
    }
}

fn map_error(hostname: &str, error: std::io::Error) -> bzd::base::error::Error {
    bzd::base::error::failure!("failed to resolve hostname '{hostname}': {error}")
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

    #[test]
    fn test_map_error() -> TestResult {
        let error = map_error(
            "example.com",
            std::io::Error::new(std::io::ErrorKind::NotFound, "no such host"),
        );
        assert_eq!(
            error.message(),
            "failed to resolve hostname 'example.com': no such host"
        )?;
        Ok(())
    }
}
