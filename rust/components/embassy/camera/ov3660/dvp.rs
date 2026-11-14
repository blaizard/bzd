//! DVP capture over esp-hal's LCD_CAM camera and GDMA ring.

use bzd::base::error::Error;
use embassy_time::{Duration, Instant, Timer};
use esp_hal::dma::DmaRxStreamBuf;
use esp_hal::lcd_cam::cam::{Camera, CameraTransfer};
use rust_components_embassy_camera_ov3660_jpeg::JpegFramer;

/// A truncated frame is detected this long after the last received byte.
const IDLE_GAP_MS: u64 = 250;

/// A camera streaming into a DMA ring, one JPEG frame at a time.
pub struct DvpCamera {
    camera: Option<Camera<'static>>,
    ring: Option<DmaRxStreamBuf>,
}

/// Outcome of a capture attempt.
pub struct Capture {
    /// Number of bytes written to the frame buffer.
    pub length: usize,
    /// Whether the attempt ended without a complete JPEG frame.
    pub truncated: bool,
}

impl DvpCamera {
    pub fn new(camera: Camera<'static>, ring: DmaRxStreamBuf) -> Self {
        Self {
            camera: Some(camera),
            ring: Some(ring),
        }
    }

    /// Capture one JPEG frame into `frame`, returning its length.
    pub async fn capture(&mut self, frame: &mut [u8], timeout_ms: u64) -> Result<Capture, Error> {
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
) -> (Result<Capture, Error>, Camera<'static>, DmaRxStreamBuf) {
    let mut framer = JpegFramer::new(frame);
    let started = Instant::now();
    let mut last_data = started;
    let result = loop {
        if framer.truncated() {
            break Ok(Capture {
                length: framer.offset(),
                truncated: true,
            });
        }
        if let Some(length) = framer.length() {
            break Ok(Capture {
                length,
                truncated: false,
            });
        }

        let available = transfer.available_bytes();
        if available > 0 {
            last_data = Instant::now();
        }
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
        if transfer.is_done() && consumed == 0 {
            break Ok(Capture {
                length: framer.offset(),
                truncated: true,
            });
        }
        if framer.offset() > 0 && last_data.elapsed().as_millis() > IDLE_GAP_MS {
            // The stream started but stopped without an EOI; this is the
            // OV3660 frame-boundary truncation, detected before the next SOI.
            break Ok(Capture {
                length: framer.offset(),
                truncated: true,
            });
        }
        if started.elapsed().as_millis() > timeout_ms {
            // Attempt budget exhausted (including no SOI at all).
            break Ok(Capture {
                length: framer.offset(),
                truncated: true,
            });
        }
        if consumed == 0 {
            // Poll often enough that the DMA cannot overrun the ring between wake-ups.
            Timer::after(Duration::from_micros(100)).await;
        }
    };

    let (camera, ring) = transfer.stop();
    (result, camera, ring)
}
