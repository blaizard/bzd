//! SCCB (I²C) register access shared by the OV sensors.

use bzd::base::error::Error;
use embassy_time::{Duration, Timer};
use esp_hal::i2c::master::I2c;
use esp_hal::Blocking;

use crate::settings::RegOp;

const SCCB_ADDRESS: u8 = 0x3C;
const PID_HIGH: u16 = 0x300A;

/// A 16-bit-register SCCB device.
pub struct Sccb {
    i2c: I2c<'static, Blocking>,
}

impl Sccb {
    pub fn new(i2c: I2c<'static, Blocking>) -> Self {
        Self { i2c }
    }

    /// The product id reported at `0x300A`/`0x300B`.
    pub fn product_id(&mut self) -> Result<u16, Error> {
        let high = self.read(PID_HIGH)?;
        let low = self.read(PID_HIGH + 1)?;
        Ok(u16::from_be_bytes([high, low]))
    }

    pub fn write(&mut self, register: u16, value: u8) -> Result<(), Error> {
        let bytes = register.to_be_bytes();
        self.i2c
            .write(SCCB_ADDRESS, &[bytes[0], bytes[1], value])
            .map_err(|error| {
                bzd::base::error::failure!("SCCB write 0x{:04X} failed: {:?}", register, error)
            })
    }

    pub fn read(&mut self, register: u16) -> Result<u8, Error> {
        let bytes = register.to_be_bytes();
        self.i2c.write(SCCB_ADDRESS, &bytes).map_err(|error| {
            bzd::base::error::failure!("SCCB address 0x{:04X} failed: {:?}", register, error)
        })?;
        let mut value = [0u8; 1];
        self.i2c.read(SCCB_ADDRESS, &mut value).map_err(|error| {
            bzd::base::error::failure!("SCCB read 0x{:04X} failed: {:?}", register, error)
        })?;
        Ok(value[0])
    }

    pub fn write16(&mut self, register: u16, value: u16) -> Result<(), Error> {
        self.write(register, (value >> 8) as u8)?;
        self.write(register + 1, value as u8)
    }

    pub fn write_addr(&mut self, register: u16, x: u16, y: u16) -> Result<(), Error> {
        self.write16(register, x)?;
        self.write16(register + 2, y)
    }

    pub fn write_bits(&mut self, register: u16, mask: u8, on: bool) -> Result<(), Error> {
        let current = self.read(register)?;
        let value = if on { current | mask } else { current & !mask };
        self.write(register, value)
    }

    pub async fn apply(&mut self, table: &[RegOp]) -> Result<(), Error> {
        for operation in table {
            match *operation {
                RegOp::Write { addr, value } => self.write(addr, value)?,
                RegOp::DelayMs(milliseconds) => {
                    Timer::after(Duration::from_millis(milliseconds as u64)).await
                }
            }
        }
        Ok(())
    }
}
