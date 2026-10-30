#![no_std]

use bzd::base::stream::OStream;
use esp_hal::gpio::AnyPin;
use esp_hal::uart::{Config, Uart};
use rust_components_esp_hal::init as esp_hal_init;
pub use rust_components_esp_stream_uart_interface::{
    BzdComponentsEspUartDevice, BzdComponentsEspUartOutContext, BzdComponentsEspUartOutInterface,
};

pub struct BzdComponentsEspUartOut {
    context: BzdComponentsEspUartOutContext,
    uart: Option<Uart<'static, esp_hal::Async>>,
}

impl BzdComponentsEspUartOut {
    pub fn new(context: BzdComponentsEspUartOutContext) -> Self {
        Self {
            context,
            uart: None,
        }
    }

    fn pin(&self) -> Result<AnyPin<'static>, bzd::base::error::Error> {
        // Safety this component owns the UART device.
        let pin = unsafe { AnyPin::steal(self.context.pin as u8) };
        Ok(pin)
    }
}

impl BzdComponentsEspUartOutInterface for BzdComponentsEspUartOut {
    async fn init(&mut self) -> Result<(), bzd::base::error::Error> {
        let peripheral = esp_hal_init();
        let pin = self.pin()?;
        let config = Config::default().with_baudrate(self.context.baudrate as u32);

        let uart = match self.context.device {
            BzdComponentsEspUartDevice::Uart0 => Uart::new(peripheral.UART0.reborrow(), config),
            BzdComponentsEspUartDevice::Uart1 => Uart::new(peripheral.UART1.reborrow(), config),
        }
        .map_err(|_| bzd::base::error::failure("failed to initialize UART"))?
        .with_tx(pin)
        .into_async();

        self.uart = Some(uart);
        Ok(())
    }

    async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error> {
        self.uart = None;
        Ok(())
    }
}

impl OStream for BzdComponentsEspUartOut {
    async fn write(&mut self, data: &[u8]) -> Result<usize, bzd::base::error::Error> {
        let uart = self
            .uart
            .as_mut()
            .ok_or_else(|| bzd::base::error::failure("UART not initialized"))?;

        uart.write_async(data)
            .await
            .map_err(|_| bzd::base::error::failure("failed to write to UART"))
    }

    async fn flush(&mut self) -> Result<(), bzd::base::error::Error> {
        let uart = self
            .uart
            .as_mut()
            .ok_or_else(|| bzd::base::error::failure("UART not initialized"))?;

        uart.flush_async()
            .await
            .map_err(|_| bzd::base::error::failure("failed to flush UART"))
    }
}
