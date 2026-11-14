#![no_std]

mod board;
mod dvp;
mod sccb;
mod sensor;
mod settings;
use bzd::base::error::Error;
use embassy_time::{Duration, Instant, Timer};
use esp_hal::dma_rx_stream_buffer;
pub use rust_components_embassy_camera_ov3660_interface::{
    BzdComponentsEmbassyCameraOv3660Context, BzdComponentsEmbassyCameraOv3660ContextConstraint,
    BzdComponentsEmbassyCameraOv3660ContextConstraintTypes,
    BzdComponentsEmbassyCameraOv3660Interface,
};
use rust_components_esp_hal::init as esp_hal_init;
use rust_interfaces_camera::{BzdCameraInterface, Frame, JpegFormat};
use rust_interfaces_memory::BzdMemoryInterface;

use crate::dvp::{Capture, DvpCamera};
use crate::sccb::Sccb;
use crate::sensor::{Ov3660, PRODUCT_ID};

/// DMA staging ring. The LCD_CAM peripheral writes here while the CPU extracts
/// JPEG frames into the memory component's frame buffer. It must stay in
/// internal DMA-capable RAM.
const DMA_RING_SIZE: usize = 8 * 1024;
/// Bytes per DMA descriptor; must be <= 4095.
const DMA_RING_CHUNK: usize = 1024;
/// Upper bound for a single capture attempt.
const ATTEMPT_TIMEOUT_MS: u64 = 2000;
/// On a successful capture, try one step better on the next one.
const QUALITY_PROBE_STEP: u8 = 1;
/// On failure, drop quality by this many register units to find a working value.
const QUALITY_FALLBACK_STEP: u8 = 8;
/// Lowest sensor quality (highest register value).
const QUALITY_MAX: u8 = 63;

pub struct BzdComponentsEmbassyCameraOv3660<C>
where
    C: BzdComponentsEmbassyCameraOv3660ContextConstraint,
{
    context: BzdComponentsEmbassyCameraOv3660Context<C>,
    dvp: Option<DvpCamera>,
    sensor: Option<Ov3660>,
    frame: Option<&'static mut [u8]>,
    /// Working sensor JPEG quality register (0 = best, 63 = worst).
    quality: u8,
    initialized: bool,
}

impl<C> BzdComponentsEmbassyCameraOv3660<C>
where
    C: BzdComponentsEmbassyCameraOv3660ContextConstraint,
{
    pub fn new(context: BzdComponentsEmbassyCameraOv3660Context<C>) -> Self {
        Self {
            context,
            dvp: None,
            sensor: None,
            frame: None,
            quality: 0,
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
            .init(self.context.width, self.context.height, self.quality)
            .await?;
        if self.context.white_balance {
            sensor.set_sunny_white_balance()?;
        }

        let frame = self.allocate_frame().await?;
        let ring = dma_rx_stream_buffer!(DMA_RING_SIZE, DMA_RING_CHUNK);
        self.dvp = Some(DvpCamera::new(camera, ring));
        self.sensor = Some(sensor);
        self.frame = Some(frame);
        self.initialized = true;
        Ok(())
    }

    async fn allocate_frame(&mut self) -> Result<&'static mut [u8], Error> {
        let width = self.context.width as usize;
        let height = self.context.height as usize;
        let mut memory = self.context.memory.lock().await;
        // Leave 2 bytes per pixel for large JPEGs, capped by the memory size.
        let size = (width * height * 2).min(memory.capacity());
        memory.alloc(size)
    }

    async fn attempt(&mut self, quality: u8, timeout_ms: u64) -> Result<Capture, Error> {
        if let Some(sensor) = self.sensor.as_mut() {
            sensor.set_quality(quality)?;
        } else {
            return Err(bzd::base::error::failure!(
                "OV3660: sensor is not initialized"
            ));
        }
        let frame = self
            .frame
            .as_deref_mut()
            .ok_or_else(|| bzd::base::error::failure!("OV3660: frame buffer is not initialized"))?;
        let dvp = self
            .dvp
            .as_mut()
            .ok_or_else(|| bzd::base::error::failure!("OV3660: camera is not initialized"))?;
        dvp.capture(frame, timeout_ms).await
    }
}

impl<C> BzdComponentsEmbassyCameraOv3660Interface for BzdComponentsEmbassyCameraOv3660<C>
where
    C: BzdComponentsEmbassyCameraOv3660ContextConstraint,
{
    async fn init(&mut self) -> Result<(), Error> {
        self.initialize().await
    }
}

impl<C> BzdCameraInterface for BzdComponentsEmbassyCameraOv3660<C>
where
    C: BzdComponentsEmbassyCameraOv3660ContextConstraint,
{
    type Data<'a>
        = JpegFormat<'a>
    where
        Self: 'a;

    async fn capture<'a>(&'a mut self) -> Result<Frame<Self::Data<'a>>, Error> {
        self.initialize().await?;

        let width = self.context.width as usize;
        let height = self.context.height as usize;
        let deadline = Instant::now()
            .checked_add(Duration::from_millis(self.context.timeout_ms as u64))
            .ok_or_else(|| bzd::base::error::failure!("OV3660: invalid capture timeout"))?;

        // Try one step better first, then the last working quality, then drop
        // quality in larger steps until a complete frame arrives.
        let stored = self.quality;
        let mut candidate = stored.saturating_sub(QUALITY_PROBE_STEP);
        let mut probing = candidate != stored;
        let mut last_quality = candidate;
        let mut last_length = 0;

        let length = loop {
            let remaining_ms = deadline
                .checked_duration_since(Instant::now())
                .map(|duration| duration.as_millis())
                .unwrap_or(0);
            if remaining_ms == 0 {
                return Err(bzd::base::error::failure!(
                    "OV3660: no complete JPEG frame (quality {}, last attempt {} bytes)",
                    last_quality,
                    last_length
                ));
            }

            last_quality = candidate;
            let capture = self
                .attempt(candidate, remaining_ms.min(ATTEMPT_TIMEOUT_MS))
                .await?;
            last_length = capture.length;
            if !capture.truncated {
                self.quality = candidate;
                break capture.length;
            }

            if probing {
                probing = false;
                candidate = stored;
                continue;
            }
            if candidate >= QUALITY_MAX {
                return Err(bzd::base::error::failure!(
                    "OV3660: no complete JPEG frame (quality {}, last attempt {} bytes)",
                    last_quality,
                    last_length
                ));
            }
            candidate = (candidate + QUALITY_FALLBACK_STEP).min(QUALITY_MAX);
        };

        let data = self
            .frame
            .as_deref()
            .ok_or_else(|| bzd::base::error::failure!("OV3660: frame buffer is not initialized"))?;
        Ok(Frame::new(width, height, JpegFormat::new(&data[..length])))
    }
}
