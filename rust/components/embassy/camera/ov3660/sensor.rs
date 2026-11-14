//! OV3660 sensor control over SCCB (I²C).
//!
//! The initialization and frame-size arithmetic follow Espressif
//! `esp32-camera` (`sensors/ov3660.c`). The OV3660 is fixed-focus.

use bzd::base::error::Error;
use embassy_time::{Duration, Timer};

use crate::sccb::Sccb;
use crate::settings::{DEFAULT_REGS, FMT_JPEG, RATIO_TABLE, RESOLUTIONS};

/// Product id expected at `0x300A`/`0x300B`.
pub const PRODUCT_ID: u16 = 0x3660;

const SYSTEM_CTROL0: u16 = 0x3008;
const SC_PLLS_CTRL0: u16 = 0x303A;
const SC_PLLS_CTRL1: u16 = 0x303B;
const SC_PLLS_CTRL2: u16 = 0x303C;
const SC_PLLS_CTRL3: u16 = 0x303D;
const X_ADDR_START: u16 = 0x3800;
const X_ADDR_END: u16 = 0x3804;
const X_OUTPUT_SIZE: u16 = 0x3808;
const X_TOTAL_SIZE: u16 = 0x380C;
const X_OFFSET: u16 = 0x3810;
const X_INCREMENT: u16 = 0x3814;
const Y_INCREMENT: u16 = 0x3815;
const TIMING_TC_REG20: u16 = 0x3820;
const TIMING_TC_REG21: u16 = 0x3821;
const PCLK_RATIO: u16 = 0x3824;
const COMPRESSION_CTRL07: u16 = 0x4407;
const VFIFO_CTRL0C: u16 = 0x460C;
const ISP_CONTROL_01: u16 = 0x5001;

/// QXGA is ordinal 19 in esp32-camera's `framesize_t` and the largest
/// resolution the OV3660 supports.
const QXGA_ORDINAL: usize = 19;

/// An OV3660 on an SCCB bus.
pub struct Ov3660 {
    sccb: Sccb,
}

impl Ov3660 {
    pub fn new(sccb: Sccb) -> Self {
        Self { sccb }
    }

    /// Reset the sensor, load the default table, and configure JPEG output.
    pub async fn init(&mut self, width: u16, height: u16, quality: u8) -> Result<(), Error> {
        self.sccb.write(SYSTEM_CTROL0, 0x82)?;
        Timer::after(Duration::from_millis(100)).await;
        self.sccb.apply(DEFAULT_REGS).await?;
        self.set_auto_exposure()?;
        Timer::after(Duration::from_millis(100)).await;
        self.set_framesize(width, height)?;
        self.sccb.apply(FMT_JPEG).await?;
        self.set_quality(quality)
    }

    pub(crate) fn set_quality(&mut self, quality: u8) -> Result<(), Error> {
        self.sccb.write(COMPRESSION_CTRL07, quality & 0x3F)
    }

    /// Apply Espressif's Sunny white-balance preset.
    pub fn set_sunny_white_balance(&mut self) -> Result<(), Error> {
        self.sccb.write(0x3406, 0x01)?;
        for (register, value) in [
            (0x3400, 0x05),
            (0x3401, 0xE0),
            (0x3402, 0x04),
            (0x3403, 0x10),
            (0x3404, 0x05),
            (0x3405, 0x40),
        ] {
            self.sccb.write(register, value)?;
        }
        Ok(())
    }

    fn set_auto_exposure(&mut self) -> Result<(), Error> {
        for (register, value) in [
            (0x3A0F, 59),
            (0x3A10, 50),
            (0x3A1B, 59),
            (0x3A1E, 50),
            (0x3A11, 118),
            (0x3A1F, 25),
        ] {
            self.sccb.write(register, value)?;
        }
        Ok(())
    }

    fn set_framesize(&mut self, width: u16, height: u16) -> Result<(), Error> {
        let ordinal = RESOLUTIONS
            .iter()
            .position(|resolution| resolution.width == width && resolution.height == height)
            .ok_or_else(|| {
                bzd::base::error::failure!("OV3660: unsupported resolution {}x{}", width, height)
            })?;
        let resolution = &RESOLUTIONS[ordinal];
        let window = &RATIO_TABLE[resolution.aspect_ratio];

        let binning = width <= window.max_width / 2 && height <= window.max_height / 2;
        let scaled = !((width == window.max_width && height == window.max_height)
            || (width == window.max_width / 2 && height == window.max_height / 2));

        self.sccb
            .write_addr(X_ADDR_START, window.start_x, window.start_y)?;
        self.sccb
            .write_addr(X_ADDR_END, window.end_x, window.end_y)?;
        self.sccb.write_addr(X_OUTPUT_SIZE, width, height)?;
        if binning {
            self.sccb
                .write_addr(X_TOTAL_SIZE, window.total_x, window.total_y / 2 + 1)?;
            self.sccb.write_addr(X_OFFSET, 8, 2)?;
        } else {
            self.sccb
                .write_addr(X_TOTAL_SIZE, window.total_x, window.total_y)?;
            self.sccb.write_addr(X_OFFSET, 16, 6)?;
        }
        self.sccb.write_bits(ISP_CONTROL_01, 0x20, scaled)?;
        self.set_image_options(binning)?;

        self.set_jpeg_pll(ordinal == QXGA_ORDINAL)
    }

    fn set_image_options(&mut self, binning: bool) -> Result<(), Error> {
        let mut timing20 = 0u8;
        let mut timing21 = 0x20; // JPEG compression.
        if binning {
            timing20 |= 0x01;
            timing21 |= 0x01;
        } else {
            timing20 |= 0x40;
        }
        let option4514 = if binning { 0xAA } else { 0x88 };
        self.sccb.write(TIMING_TC_REG20, timing20)?;
        self.sccb.write(TIMING_TC_REG21, timing21)?;
        self.sccb.write(0x4514, option4514)?;
        if binning {
            self.sccb.write(0x4520, 0x0B)?;
            self.sccb.write(X_INCREMENT, 0x31)?;
            self.sccb.write(Y_INCREMENT, 0x31)
        } else {
            self.sccb.write(0x4520, 0xB0)?;
            self.sccb.write(X_INCREMENT, 0x11)?;
            self.sccb.write(Y_INCREMENT, 0x11)
        }
    }

    fn set_jpeg_pll(&mut self, qxga: bool) -> Result<(), Error> {
        let (multiplier, pclk_divider) = if qxga { (24, 8) } else { (30, 10) };
        self.sccb.write(SC_PLLS_CTRL0, 0x00)?;
        self.sccb.write(SC_PLLS_CTRL1, multiplier)?;
        self.sccb.write(SC_PLLS_CTRL2, 0x11)?;
        self.sccb.write(SC_PLLS_CTRL3, 0x30)?;
        self.sccb.write(PCLK_RATIO, pclk_divider)?;
        self.sccb.write(VFIFO_CTRL0C, 0x22)
    }
}
