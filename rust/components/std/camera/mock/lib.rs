#![allow(async_fn_in_trait)]
#![no_std]
#![no_main]

use bzd::base::error::Error;
use rust_components_std_camera_mock_config::{HEIGHT, IMAGE, WIDTH};
pub use rust_components_std_camera_mock_interface::BzdComponentsStdCameraMockInterface;
use rust_interfaces_camera::{BzdCameraInterface, Frame, RuntimeFormat};

/// Mock camera component exposing a bundled image.
pub struct BzdComponentsStdCameraMock;

impl BzdComponentsStdCameraMock {
    pub fn new() -> Self {
        Self
    }
}

impl Default for BzdComponentsStdCameraMock {
    fn default() -> Self {
        Self::new()
    }
}

impl BzdCameraInterface for BzdComponentsStdCameraMock {
    type Data<'a> = RuntimeFormat<'a>;

    async fn capture<'a>(&'a mut self) -> Result<Frame<Self::Data<'a>>, Error> {
        Ok(Frame::new(
            WIDTH,
            HEIGHT,
            RuntimeFormat::Jpeg { data: IMAGE },
        ))
    }
}

impl BzdComponentsStdCameraMockInterface for BzdComponentsStdCameraMock {}

#[cfg(test)]
#[bzd_test::test]
mod tests {
    use super::*;

    #[test]
    async fn test_capture() -> TestResult {
        let mut camera = BzdComponentsStdCameraMock::new();
        let frame = camera.capture().await?;
        assert_eq!(frame.width, WIDTH)?;
        assert_eq!(frame.height, HEIGHT)?;
        match frame.data {
            RuntimeFormat::Jpeg { data } => {
                assert!(data.len() > 4);
                assert_eq!(&data[..2], &[0xFF, 0xD8])?;
                assert_eq!(&data[data.len() - 2..], &[0xFF, 0xD9])?;
            }
            _ => return Err(bzd::base::error::failure!("unexpected format")),
        }
        Ok(())
    }
}
