use bzd::base::error::Error;
use core::cell::UnsafeCell;
use core::net::{IpAddr, SocketAddr};
use embassy_net::tcp::client::TcpClient;
use embassy_net::{IpAddress, Stack};
use embedded_nal_async::TcpConnect;

use crate::stream::TcpStream;

pub type TcpClientType = TcpClient<'static, 2, 1536, 1536>;
pub type TcpConnectionType = <TcpClientType as TcpConnect>::Connection<'static>;

pub struct NetworkState {
    pub stack: Stack<'static>,
    pub tcp_client: &'static TcpClientType,
    pub timeout: embassy_time::Duration,
}

impl NetworkState {
    async fn wait_for_config(&self) -> Result<(), Error> {
        if self.stack.is_config_up() {
            return Ok(());
        }
        embassy_time::with_timeout(self.timeout, self.stack.wait_config_up())
            .await
            .map_err(|_| {
                bzd::base::error::timeout!(
                    "network configuration timeout after {}s",
                    self.timeout.as_secs()
                )
            })?;
        Ok(())
    }

    pub async fn resolve(&self, hostname: &str) -> Result<IpAddr, Error> {
        self.wait_for_config().await?;
        let addresses = self
            .stack
            .dns_query(hostname, embassy_net::dns::DnsQueryType::A)
            .await
            .map_err(|error| {
                bzd::base::error::failure!("DNS A query failed for '{hostname}': {error:?}")
            })?;
        if let Some(address) = addresses.first() {
            return Ok(to_ip_addr(address));
        }
        let addresses = self
            .stack
            .dns_query(hostname, embassy_net::dns::DnsQueryType::Aaaa)
            .await
            .map_err(|error| {
                bzd::base::error::failure!("DNS AAAA query failed for '{hostname}': {error:?}")
            })?;
        addresses.first().map(to_ip_addr).ok_or_else(|| {
            bzd::base::error::failure!("no IPv4 or IPv6 address found for '{hostname}'")
        })
    }

    pub async fn connect(
        &self,
        address: SocketAddr,
    ) -> Result<TcpStream<TcpConnectionType>, Error> {
        self.wait_for_config().await?;
        let connection = self.tcp_client.connect(address).await.map_err(|error| {
            bzd::base::error::failure!("TCP connection to {address} failed: {error}")
        })?;
        Ok(TcpStream::new(connection))
    }
}

fn to_ip_addr(address: &IpAddress) -> IpAddr {
    match address {
        IpAddress::Ipv4(address) => IpAddr::V4(core::net::Ipv4Addr::from(address.octets())),
        IpAddress::Ipv6(address) => IpAddr::V6(core::net::Ipv6Addr::from(address.octets())),
    }
}

struct Singleton<T>(UnsafeCell<Option<T>>);

// SAFETY: the composition is single-threaded, all accesses go through the same executor.
unsafe impl<T> Sync for Singleton<T> {}

impl<T> Singleton<T> {
    const fn new() -> Self {
        Self(UnsafeCell::new(None))
    }

    fn get(&'static self) -> Option<&'static T> {
        // SAFETY: single-threaded access.
        unsafe { (&*self.0.get()).as_ref() }
    }

    fn get_mut_or_init(&'static self, init: impl FnOnce() -> T) -> &'static mut T {
        // SAFETY: single-threaded access.
        unsafe { (&mut *self.0.get()).get_or_insert_with(init) }
    }
}

static NETWORK: Singleton<NetworkState> = Singleton::new();

pub fn init(state: NetworkState) -> &'static NetworkState {
    NETWORK.get_mut_or_init(|| state)
}

pub fn get() -> Option<&'static NetworkState> {
    NETWORK.get()
}
