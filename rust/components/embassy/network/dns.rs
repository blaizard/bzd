use bzd::base::error::Error;
use core::net::IpAddr;
use rust_components_embassy_network_interface::{
    BzdComponentsEmbassyNetworkDnsContext, BzdComponentsEmbassyNetworkDnsContextConstraint,
    BzdComponentsEmbassyNetworkDnsInterface,
};
use rust_interfaces_dns::BzdNetworkDnsInterface;

use crate::state;

pub struct BzdComponentsEmbassyNetworkDns<C>
where
    C: BzdComponentsEmbassyNetworkDnsContextConstraint,
{
    _context: BzdComponentsEmbassyNetworkDnsContext<C>,
}

impl<C> BzdComponentsEmbassyNetworkDns<C>
where
    C: BzdComponentsEmbassyNetworkDnsContextConstraint,
{
    pub fn new(context: BzdComponentsEmbassyNetworkDnsContext<C>) -> Self {
        Self { _context: context }
    }
}

impl<C> BzdNetworkDnsInterface for BzdComponentsEmbassyNetworkDns<C>
where
    C: BzdComponentsEmbassyNetworkDnsContextConstraint,
{
    async fn resolve(&self, hostname: &str) -> Result<IpAddr, Error> {
        let network = state::get().ok_or_else(|| {
            bzd::base::error::failure!("DNS resolve '{hostname}': network is not initialized")
        })?;
        network.resolve(hostname).await
    }
}

impl<C> BzdComponentsEmbassyNetworkDnsInterface for BzdComponentsEmbassyNetworkDns<C> where
    C: BzdComponentsEmbassyNetworkDnsContextConstraint
{
}
