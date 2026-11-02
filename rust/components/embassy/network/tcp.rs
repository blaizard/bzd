use bzd::base::error::Error;
use core::net::SocketAddr;
use rust_components_embassy_network_interface::{
    BzdComponentsEmbassyNetworkTcpContext, BzdComponentsEmbassyNetworkTcpContextConstraint,
    BzdComponentsEmbassyNetworkTcpInterface,
};
use rust_interfaces_network::BzdTcpInterface;

use crate::state::{self, TcpConnectionType};
use crate::stream::TcpStream;

pub struct BzdComponentsEmbassyNetworkTcp<C>
where
    C: BzdComponentsEmbassyNetworkTcpContextConstraint,
{
    _context: BzdComponentsEmbassyNetworkTcpContext<C>,
}

impl<C> BzdComponentsEmbassyNetworkTcp<C>
where
    C: BzdComponentsEmbassyNetworkTcpContextConstraint,
{
    pub fn new(context: BzdComponentsEmbassyNetworkTcpContext<C>) -> Self {
        Self { _context: context }
    }
}

impl<C> BzdTcpInterface for BzdComponentsEmbassyNetworkTcp<C>
where
    C: BzdComponentsEmbassyNetworkTcpContextConstraint,
{
    type Stream = TcpStream<TcpConnectionType>;

    async fn connect(&self, address: SocketAddr) -> Result<Self::Stream, Error> {
        let network = state::get().ok_or_else(|| {
            bzd::base::error::failure!("TCP connect to {address}: network is not initialized")
        })?;
        network.connect(address).await
    }
}

impl<C> BzdComponentsEmbassyNetworkTcpInterface for BzdComponentsEmbassyNetworkTcp<C> where
    C: BzdComponentsEmbassyNetworkTcpContextConstraint
{
}
