//! Seeed Studio XIAO ESP32-S3 Sense camera wiring.
//!
//! The Sense expansion board connects the camera to these GPIOs. PWDN and
//! RESET are not wired.

use bzd::base::error::Error;
use esp_hal::i2c::master::{Config as I2cConfig, I2c};
use esp_hal::lcd_cam::cam::{self, EofMode, VhdeMode, VsyncFilterThreshold};
use esp_hal::lcd_cam::ByteOrder;
use esp_hal::lcd_cam::LcdCam;
use esp_hal::peripherals::Peripherals;
use esp_hal::time::Rate;
use esp_hal::Blocking;

const SCCB_KHZ: u32 = 100;

pub fn create_hardware(
    peripherals: &'static mut Peripherals,
    xclk_mhz: u32,
) -> Result<(cam::Camera<'static>, I2c<'static, Blocking>), Error> {
    // The camera clock must be running before the sensor answers on SCCB.
    let config = cam::Config::default()
        .with_frequency(Rate::from_mhz(xclk_mhz))
        .with_eof_mode(EofMode::VsyncSignal)
        .with_vh_de_mode(VhdeMode::De)
        .with_vsync_filter_threshold(VsyncFilterThreshold::Four)
        .with_byte_order(ByteOrder::Native)
        .with_enable_2byte_mode(false);
    let lcd_cam = LcdCam::new(peripherals.LCD_CAM.reborrow());
    let camera = cam::Camera::new(lcd_cam.cam, peripherals.DMA_CH0.reborrow(), config)
        .map_err(|error| bzd::base::error::failure!("OV3660 camera setup failed: {:?}", error))?
        .with_master_clock(peripherals.GPIO10.reborrow())
        .with_pixel_clock(peripherals.GPIO13.reborrow())
        .with_vsync(peripherals.GPIO38.reborrow())
        .with_h_enable(peripherals.GPIO47.reborrow())
        .with_data0(peripherals.GPIO15.reborrow())
        .with_data1(peripherals.GPIO17.reborrow())
        .with_data2(peripherals.GPIO18.reborrow())
        .with_data3(peripherals.GPIO16.reborrow())
        .with_data4(peripherals.GPIO14.reborrow())
        .with_data5(peripherals.GPIO12.reborrow())
        .with_data6(peripherals.GPIO11.reborrow())
        .with_data7(peripherals.GPIO48.reborrow());

    let i2c = I2c::new(
        peripherals.I2C0.reborrow(),
        I2cConfig::default().with_frequency(Rate::from_khz(SCCB_KHZ)),
    )
    .map_err(|error| bzd::base::error::failure!("OV3660 SCCB setup failed: {:?}", error))?
    .with_sda(peripherals.GPIO40.reborrow())
    .with_scl(peripherals.GPIO39.reborrow());

    Ok((camera, i2c))
}
