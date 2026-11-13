#![no_std]

mod board;
mod dvp;
mod sccb;
mod sensor;
mod settings;
use bzd::base::error::Error;
use embassy_time::{Duration, Timer};
use esp_hal::dma_rx_stream_buffer;
pub use rust_components_embassy_camera_ov3660_interface::{
    BzdComponentsEmbassyCameraOv3660Context, BzdComponentsEmbassyCameraOv3660Interface,
};
use rust_components_esp_hal::init as esp_hal_init;
use rust_interfaces_camera::{BzdCameraInterface, Frame, JpegFormat};
use static_cell::StaticCell;

use crate::dvp::DvpCamera;
use crate::sccb::Sccb;
use crate::sensor::{Ov3660, PRODUCT_ID};

/// Ring the camera DMA fills. Large enough for a VGA JPEG frame; raise it
/// together with `MAX_FRAME_SIZE` for larger resolutions.
const DMA_RING_SIZE: usize = 64 * 1024;
/// Largest JPEG frame accepted by the component.
const MAX_FRAME_SIZE: usize = 64 * 1024;

static FRAME_BUFFER: StaticCell<[u8; MAX_FRAME_SIZE]> = StaticCell::new();

pub struct BzdComponentsEmbassyCameraOv3660 {
    context: BzdComponentsEmbassyCameraOv3660Context,
    dvp: Option<DvpCamera>,
    frame: Option<&'static mut [u8]>,
    initialized: bool,
}

impl BzdComponentsEmbassyCameraOv3660 {
    pub fn new(context: BzdComponentsEmbassyCameraOv3660Context) -> Self {
        Self {
            context,
            dvp: None,
            frame: None,
            initialized: false,
        }
    }

    async fn initialize(&mut self) -> Result<(), Error> {
        if self.initialized {
            return Ok(());
        }

        let (camera, i2c) = board::create_hardware(esp_hal_init(), self.context.xclk_mhz as u32)?;
        let mut sccb = Sccb::new(i2c);
        // The sensor needs a stable XCLK before it answers on SCCB.
        Timer::after(Duration::from_millis(200)).await;
        let mut product_id = 0;
        for attempt in 0..3 {
            product_id = sccb.product_id()?;
            if product_id == PRODUCT_ID {
                break;
            }
            if attempt < 2 {
                Timer::after(Duration::from_millis(100)).await;
            }
        }
        if product_id != PRODUCT_ID {
            return Err(bzd::base::error::failure!(
                "OV3660: unexpected product id 0x{:04X}, expected 0x{:04X}",
                product_id,
                PRODUCT_ID
            ));
        }
        let mut sensor = Ov3660::new(sccb);
        sensor
            .init(
                self.context.width,
                self.context.height,
                self.context.quality,
            )
            .await?;
        if self.context.white_balance {
            sensor.set_sunny_white_balance()?;
        }

        let ring = dma_rx_stream_buffer!(DMA_RING_SIZE, 4095);
        self.dvp = Some(DvpCamera::new(camera, ring));
        self.frame = Some(FRAME_BUFFER.init([0u8; MAX_FRAME_SIZE]));
        self.initialized = true;
        Ok(())
    }
}

impl BzdComponentsEmbassyCameraOv3660Interface for BzdComponentsEmbassyCameraOv3660 {
    async fn init(&mut self) -> Result<(), Error> {
        self.initialize().await
    }
}

impl BzdCameraInterface for BzdComponentsEmbassyCameraOv3660 {
    type Data<'a>
        = JpegFormat<'a>
    where
        Self: 'a;

    async fn capture<'a>(&'a mut self) -> Result<Frame<Self::Data<'a>>, Error> {
        self.initialize().await?;

        let timeout_ms = self.context.timeout_ms as u64;
        let frame = self
            .frame
            .as_deref_mut()
            .ok_or_else(|| bzd::base::error::failure!("OV3660: frame buffer is not initialized"))?;
        let dvp = self
            .dvp
            .as_mut()
            .ok_or_else(|| bzd::base::error::failure!("OV3660: camera is not initialized"))?;
        let length = dvp.capture(frame, timeout_ms).await?;

        let data = self
            .frame
            .as_deref()
            .ok_or_else(|| bzd::base::error::failure!("OV3660: frame buffer is not initialized"))?;
        Ok(Frame::new(
            self.context.width as usize,
            self.context.height as usize,
            JpegFormat::new(&data[..length]),
        ))
    }
}
