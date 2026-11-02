#![no_std]

use bzd::base::error::Error;
use bzd::base::format::format_to;
use embassy_net::tcp::client::{TcpClient, TcpClientState};
use embassy_net::StackResources;
use esp_radio::wifi::sta::StationConfig;
use esp_radio::wifi::{
    AuthenticationMethodConfig, Config as WifiConfig, ControllerConfig, Interface, Password, Ssid,
    WifiController,
};
use rust_components_embassy_network::state::{self, NetworkState, TcpClientType};
use rust_components_esp_hal::init as esp_hal_init;
pub use rust_components_esp_network_wifi_interface::*;
use rust_interfaces_network::BzdNetworkInterface;
use static_cell::StaticCell;

static STACK_RESOURCES: StaticCell<StackResources<4>> = StaticCell::new();
static TCP_CLIENT_STATE: StaticCell<TcpClientState<2, 1536, 1536>> = StaticCell::new();
static TCP_CLIENT: StaticCell<TcpClientType> = StaticCell::new();

pub struct BzdComponentsEspNetworkWiFi<C>
where
    C: BzdComponentsEspNetworkWiFiContextConstraint,
{
    context: BzdComponentsEspNetworkWiFiContext<C>,
    controller: Option<WifiController<'static>>,
    runner: Option<embassy_net::Runner<'static, Interface>>,
}

impl<C> BzdComponentsEspNetworkWiFi<C>
where
    C: BzdComponentsEspNetworkWiFiContextConstraint,
{
    pub fn new(context: BzdComponentsEspNetworkWiFiContext<C>) -> Self {
        Self {
            context,
            controller: None,
            runner: None,
        }
    }
}

impl<C> BzdNetworkInterface for BzdComponentsEspNetworkWiFi<C> where
    C: BzdComponentsEspNetworkWiFiContextConstraint
{
}

impl<C> BzdComponentsEspNetworkWiFiInterface for BzdComponentsEspNetworkWiFi<C>
where
    C: BzdComponentsEspNetworkWiFiContextConstraint,
{
    async fn init(&mut self) -> Result<(), Error> {
        if state::get().is_some() {
            return Err(bzd::base::error::failure!(
                "WiFi init for SSID '{}': network is already initialized",
                self.context.ssid
            ));
        }
        esp_alloc::heap_allocator!(size: 96 * 1024);

        let peripherals = esp_hal_init();
        let ssid = Ssid::try_from(self.context.ssid).map_err(|error| {
            bzd::base::error::failure!("invalid WiFi SSID '{}': {error:?}", self.context.ssid)
        })?;
        let password = Password::try_from(self.context.password).map_err(|error| {
            bzd::base::error::failure!(
                "invalid WiFi password for SSID '{}': {error:?}",
                self.context.ssid
            )
        })?;
        let station_config = StationConfig::default()
            .with_ssid(ssid)
            .with_authentication(AuthenticationMethodConfig::WpaWpa2Personal(password));
        let config =
            ControllerConfig::default().with_initial_config(WifiConfig::Station(station_config));
        let controller =
            WifiController::new(peripherals.WIFI.reborrow(), config).map_err(|error| {
                bzd::base::error::failure!(
                    "failed to initialize WiFi for SSID '{}': {error:?}",
                    self.context.ssid
                )
            })?;
        let station = Interface::station();

        let rng = esp_hal::rng::Rng::new();
        let seed = (rng.random() as u64) << 32 | rng.random() as u64;
        let (stack, runner) = embassy_net::new(
            station,
            embassy_net::Config::dhcpv4(Default::default()),
            STACK_RESOURCES.init_with(|| StackResources::<4>::new()),
            seed,
        );

        let tcp_state = TCP_CLIENT_STATE.init(TcpClientState::new());
        let mut client = TcpClient::new(stack, tcp_state);
        client.set_timeout(Some(embassy_time::Duration::from_secs(10)));
        let client = TCP_CLIENT.init(client);
        state::init(NetworkState {
            stack,
            tcp_client: client,
            timeout: embassy_time::Duration::from_secs(self.context.timeout_s as u64),
        });

        self.controller = Some(controller);
        self.runner = Some(runner);
        Ok(())
    }

    async fn shutdown(&mut self) -> Result<(), Error> {
        self.runner = None;
        self.controller = None;
        Ok(())
    }

    async fn service(&mut self) -> Result<(), Error> {
        let mut controller = self.controller.take().ok_or_else(|| {
            bzd::base::error::failure!(
                "WiFi service for SSID '{}': controller is not initialized",
                self.context.ssid
            )
        })?;
        let mut runner = self.runner.take().ok_or_else(|| {
            bzd::base::error::failure!(
                "WiFi service for SSID '{}': network runner is not initialized",
                self.context.ssid
            )
        })?;

        let out = self.context.out;
        let ssid = self.context.ssid;
        let connection = async move {
            loop {
                match controller.connect_async().await {
                    Ok(info) => {
                        let _ = format_to!(
                            out.lock().await,
                            "WiFi connected: ssid={} channel={}\n",
                            info.ssid.as_str(),
                            info.channel
                        )
                        .await;
                        let _ = controller.wait_for_disconnect_async().await;
                        embassy_time::Timer::after(embassy_time::Duration::from_secs(1)).await;
                    }
                    Err(err) => {
                        let _ = format_to!(
                            out.lock().await,
                            "WiFi connect failed for '{ssid}': {err:?}\n"
                        )
                        .await;
                        embassy_time::Timer::after(embassy_time::Duration::from_secs(5)).await;
                    }
                }
            }
        };

        let (_, never) = embassy_futures::join::join(connection, runner.run()).await;
        match never {}
    }
}
