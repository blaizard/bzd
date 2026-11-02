#![no_std]

use bzd::base::stream::OStream;
use embedded_io_async::Write;
use esp_hal::usb::usb_serial_jtag::UsbSerialJtag;
use rust_components_esp_hal::init as esp_hal_init;
pub use rust_components_esp_stream_usb_serial_jtag_interface::BzdComponentsEspUsbSerialJtagOutInterface;

pub struct BzdComponentsEspUsbSerialJtagOut {
    uart: Option<UsbSerialJtag<'static, esp_hal::Async>>,
}

impl BzdComponentsEspUsbSerialJtagOut {
    pub fn new() -> Self {
        Self { uart: None }
    }
}

impl Default for BzdComponentsEspUsbSerialJtagOut {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdComponentsEspUsbSerialJtagOutInterface for BzdComponentsEspUsbSerialJtagOut {
    async fn init(&mut self) -> Result<(), bzd::base::error::Error> {
        let peripheral = esp_hal_init();
        self.uart = Some(UsbSerialJtag::new(peripheral.USB_DEVICE.reborrow()).into_async());
        Ok(())
    }

    async fn shutdown(&mut self) -> Result<(), bzd::base::error::Error> {
        self.uart = None;
        Ok(())
    }
}

impl OStream for BzdComponentsEspUsbSerialJtagOut {
    async fn write(&mut self, data: &[u8]) -> Result<usize, bzd::base::error::Error> {
        let uart = self
            .uart
            .as_mut()
            .ok_or_else(|| bzd::base::error::failure!("USB serial JTAG not initialized"))?;

        Write::write(uart, data)
            .await
            .map_err(|_| bzd::base::error::failure!("failed to write to USB serial JTAG"))
    }

    async fn flush(&mut self) -> Result<(), bzd::base::error::Error> {
        let uart = self
            .uart
            .as_mut()
            .ok_or_else(|| bzd::base::error::failure!("USB serial JTAG not initialized"))?;

        Write::flush(uart)
            .await
            .map_err(|_| bzd::base::error::failure!("failed to flush USB serial JTAG"))
    }
}
