#![allow(async_fn_in_trait)]
#![no_main]

use bzd::base::error::Error;
use nokhwa::utils::{
    CameraIndex, FrameFormat as NokhwaFrameFormat, RequestedFormat, RequestedFormatType,
};
use nokhwa::{Camera, NokhwaError};
pub use rust_components_std_camera_nokhwa_interface::{
    BzdComponentsStdCameraNokhwaContext, BzdComponentsStdCameraNokhwaInterface,
};
use rust_interfaces_camera::{BzdCameraInterface, Frame, RuntimeFormat};

/// Formats accepted from the camera. All nokhwa formats are supported, so the
/// device is free to use whatever it produces.
const SUPPORTED_FORMATS: [NokhwaFrameFormat; 6] = [
    NokhwaFrameFormat::MJPEG,
    NokhwaFrameFormat::YUYV,
    NokhwaFrameFormat::NV12,
    NokhwaFrameFormat::GRAY,
    NokhwaFrameFormat::RAWRGB,
    NokhwaFrameFormat::RAWBGR,
];

/// Camera component backed by nokhwa.
pub struct BzdComponentsStdCameraNokhwa {
    device: u32,
    last_frame: Option<nokhwa::Buffer>,
}

impl BzdComponentsStdCameraNokhwa {
    /// Create a new nokhwa camera component.
    pub fn new(context: BzdComponentsStdCameraNokhwaContext) -> Self {
        Self {
            device: context.device,
            last_frame: None,
        }
    }
}

impl BzdCameraInterface for BzdComponentsStdCameraNokhwa {
    type Data<'a>
        = rust_interfaces_camera::RuntimeFormat<'a>
    where
        Self: 'a;

    async fn capture<'a>(&'a mut self) -> Result<Frame<Self::Data<'a>>, Error> {
        initialize();

        let mut camera = open_camera(self.device)?;
        camera.open_stream().map_err(map_nokhwa_error)?;
        let buffer = camera.frame().map_err(map_nokhwa_error)?;
        // Release the device after this single capture.
        drop(camera);

        let resolution = buffer.resolution();
        let format = buffer.source_frame_format();
        self.last_frame = Some(buffer);
        let data = self
            .last_frame
            .as_ref()
            .expect("last frame was just stored")
            .buffer();
        Ok(Frame::new(
            resolution.width_x as usize,
            resolution.height_y as usize,
            map_runtime_format(format, data, resolution.width_x as usize),
        ))
    }
}

impl BzdComponentsStdCameraNokhwaInterface for BzdComponentsStdCameraNokhwa {}

fn open_camera(device: u32) -> Result<Camera, Error> {
    let index = CameraIndex::Index(device);
    let requested = RequestedFormat::with_formats(
        RequestedFormatType::AbsoluteHighestResolution,
        &SUPPORTED_FORMATS,
    );
    Camera::new(index, requested).map_err(|error| match error {
        NokhwaError::GetPropertyError { property, error } if property == "CameraFormat" => {
            bzd::base::error::data!(
                "camera '{device}': no supported format ({error}); check the device index"
            )
        }
        error => map_nokhwa_error(error),
    })
}

fn map_runtime_format<'a>(
    format: NokhwaFrameFormat,
    data: &'a [u8],
    width: usize,
) -> RuntimeFormat<'a> {
    match format {
        NokhwaFrameFormat::MJPEG => RuntimeFormat::Jpeg { data },
        NokhwaFrameFormat::YUYV => RuntimeFormat::Yuyv {
            data,
            stride: width.saturating_mul(2),
        },
        NokhwaFrameFormat::NV12 => RuntimeFormat::Nv12 {
            data,
            stride: width,
        },
        NokhwaFrameFormat::GRAY => RuntimeFormat::Gray8 {
            data,
            stride: width,
        },
        NokhwaFrameFormat::RAWRGB => RuntimeFormat::Rgb8 {
            data,
            stride: width.saturating_mul(3),
        },
        NokhwaFrameFormat::RAWBGR => RuntimeFormat::Bgr8 {
            data,
            stride: width.saturating_mul(3),
        },
    }
}

fn map_nokhwa_error(error: NokhwaError) -> Error {
    match error {
        NokhwaError::StructureError { .. } | NokhwaError::ProcessFrameError { .. } => {
            bzd::base::error::data!("nokhwa: {error}")
        }
        _ => bzd::base::error::failure!("nokhwa: {error}"),
    }
}

fn initialize() {
    #[cfg(target_os = "macos")]
    {
        static ONCE: std::sync::Once = std::sync::Once::new();
        ONCE.call_once(|| nokhwa::nokhwa_initialize(|_| {}));
    }
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;
    use bzd::base::error::ErrorType as BzdErrorType;

    #[test]
    fn test_map_jpeg() -> TestResult {
        let data = [0u8, 1, 2];
        match map_runtime_format(NokhwaFrameFormat::MJPEG, &data, 4) {
            RuntimeFormat::Jpeg { data: slice } => {
                assert_eq!(slice, &data[..])?;
            }
            _ => return Err(bzd::base::error::failure!("unexpected format")),
        }
        Ok(())
    }

    #[test]
    fn test_map_yuyv() -> TestResult {
        let data = [0u8; 16];
        match map_runtime_format(NokhwaFrameFormat::YUYV, &data, 4) {
            RuntimeFormat::Yuyv {
                data: slice,
                stride,
            } => {
                assert_eq!(slice, &data[..])?;
                assert_eq!(stride, 8)?;
            }
            _ => return Err(bzd::base::error::failure!("unexpected format")),
        }
        Ok(())
    }

    #[test]
    fn test_map_nv12() -> TestResult {
        let data = [0u8; 16];
        match map_runtime_format(NokhwaFrameFormat::NV12, &data, 4) {
            RuntimeFormat::Nv12 {
                data: slice,
                stride,
            } => {
                assert_eq!(slice, &data[..])?;
                assert_eq!(stride, 4)?;
            }
            _ => return Err(bzd::base::error::failure!("unexpected format")),
        }
        Ok(())
    }

    #[test]
    fn test_map_gray() -> TestResult {
        let data = [0u8; 16];
        match map_runtime_format(NokhwaFrameFormat::GRAY, &data, 4) {
            RuntimeFormat::Gray8 {
                data: slice,
                stride,
            } => {
                assert_eq!(slice, &data[..])?;
                assert_eq!(stride, 4)?;
            }
            _ => return Err(bzd::base::error::failure!("unexpected format")),
        }
        Ok(())
    }

    #[test]
    fn test_map_rgb() -> TestResult {
        let data = [0u8; 16];
        match map_runtime_format(NokhwaFrameFormat::RAWRGB, &data, 4) {
            RuntimeFormat::Rgb8 {
                data: slice,
                stride,
            } => {
                assert_eq!(slice, &data[..])?;
                assert_eq!(stride, 12)?;
            }
            _ => return Err(bzd::base::error::failure!("unexpected format")),
        }
        Ok(())
    }

    #[test]
    fn test_map_bgr() -> TestResult {
        let data = [0u8; 16];
        match map_runtime_format(NokhwaFrameFormat::RAWBGR, &data, 4) {
            RuntimeFormat::Bgr8 {
                data: slice,
                stride,
            } => {
                assert_eq!(slice, &data[..])?;
                assert_eq!(stride, 12)?;
            }
            _ => return Err(bzd::base::error::failure!("unexpected format")),
        }
        Ok(())
    }

    #[test]
    fn test_map_structure_error_is_data() -> TestResult {
        let error = map_nokhwa_error(NokhwaError::StructureError {
            structure: "test".to_owned(),
            error: "bad structure".to_owned(),
        });
        assert_eq!(error.error_type(), BzdErrorType::Data)?;
        Ok(())
    }

    #[test]
    fn test_map_process_error_is_data() -> TestResult {
        let error = map_nokhwa_error(NokhwaError::ProcessFrameError {
            src: NokhwaFrameFormat::MJPEG,
            destination: "RGB".to_owned(),
            error: "decode failed".to_owned(),
        });
        assert_eq!(error.error_type(), BzdErrorType::Data)?;
        Ok(())
    }

    #[test]
    fn test_map_device_error_is_failure() -> TestResult {
        let error = map_nokhwa_error(NokhwaError::OpenDeviceError(
            "0".to_owned(),
            "permission denied".to_owned(),
        ));
        assert_eq!(error.error_type(), BzdErrorType::Failure)?;
        Ok(())
    }
}
