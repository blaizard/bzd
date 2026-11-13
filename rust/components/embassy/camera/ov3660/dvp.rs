//! DVP capture over esp-hal's LCD_CAM camera and GDMA ring.

use bzd::base::error::Error;
use embassy_time::{Duration, Instant, Timer};
use esp_hal::dma::DmaRxStreamBuf;
use esp_hal::lcd_cam::cam::{Camera, CameraTransfer};
use rust_components_embassy_camera_ov3660_jpeg::JpegFramer;

/// A camera streaming into a DMA ring, one JPEG frame at a time.
pub struct DvpCamera {
    camera: Option<Camera<'static>>,
    ring: Option<DmaRxStreamBuf>,
}

impl DvpCamera {
    pub fn new(camera: Camera<'static>, ring: DmaRxStreamBuf) -> Self {
        Self {
            camera: Some(camera),
            ring: Some(ring),
        }
    }

    /// Capture one complete JPEG frame into `frame`, returning its length.
    pub async fn capture(&mut self, frame: &mut [u8], timeout_ms: u64) -> Result<usize, Error> {
        let camera = self
            .camera
            .take()
            .ok_or_else(|| bzd::base::error::failure!("OV3660: camera is not initialized"))?;
        let ring = self
            .ring
            .take()
            .ok_or_else(|| bzd::base::error::failure!("OV3660: DMA ring is not available"))?;

        let transfer = match camera.receive(ring) {
            Ok(transfer) => transfer,
            Err((error, camera, ring)) => {
                self.camera = Some(camera);
                self.ring = Some(ring);
                return Err(bzd::base::error::failure!(
                    "OV3660: failed to start a DMA transfer: {:?}",
                    error
                ));
            }
        };

        let (result, camera, ring) = read_frame(transfer, frame, timeout_ms).await;
        self.camera = Some(camera);
        self.ring = Some(ring);
        result
    }
}

async fn read_frame(
    mut transfer: CameraTransfer<'static, DmaRxStreamBuf>,
    frame: &mut [u8],
    timeout_ms: u64,
) -> (Result<usize, Error>, Camera<'static>, DmaRxStreamBuf) {
    let mut framer = JpegFramer::new(frame);
    let started = Instant::now();
    let result = loop {
        if let Some(length) = framer.length() {
            break Ok(length);
        }

        let available = transfer.available_bytes();
        let consumed = if available == 0 {
            0
        } else {
            let chunk = transfer.peek();
            match framer.feed(chunk) {
                Ok(consumed) => consumed,
                Err(error) => break Err(error),
            }
        };
        transfer.consume(consumed);

        if framer.length().is_some() {
            continue;
        }
        if started.elapsed().as_millis() > timeout_ms {
            break Err(bzd::base::error::failure!(
                "OV3660: JPEG frame capture timed out"
            ));
        }
        if transfer.is_done() && consumed == 0 {
            break Err(bzd::base::error::failure!(
                "OV3660: DMA stopped before a complete JPEG frame"
            ));
        }
        if consumed == 0 {
            Timer::after(Duration::from_millis(1)).await;
        }
    };

    let (camera, ring) = transfer.stop();
    (result, camera, ring)
}
