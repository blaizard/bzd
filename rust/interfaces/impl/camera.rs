#![no_std]
#![allow(async_fn_in_trait)]

use bzd::base::error::Error;

/// A frame of data captured by a camera.
pub struct Frame<Data> {
    /// The width of the frame in pixels.
    pub width: usize,
    /// The height of the frame in pixels.
    pub height: usize,
    /// The data contained in the frame.
    pub data: Data,
}

impl<Data> Frame<Data> {
    /// Creates a new frame with the given width, height, and data.
    pub fn new(width: usize, height: usize, data: Data) -> Self {
        Self {
            width,
            height,
            data,
        }
    }
}

/// A JPEG-compressed frame payload.
#[derive(Clone, Copy)]
pub struct JpegFormat<'a> {
    /// The compressed JPEG bytes.
    pub data: &'a [u8],
}

impl<'a> JpegFormat<'a> {
    /// Creates a new JPEG payload.
    pub fn new(data: &'a [u8]) -> Self {
        Self { data }
    }
}

impl<'a> From<JpegFormat<'a>> for RuntimeFormat<'a> {
    fn from(value: JpegFormat<'a>) -> Self {
        RuntimeFormat::Jpeg { data: value.data }
    }
}

/// Runtime description of a captured frame payload.
#[derive(Clone, Copy)]
pub enum RuntimeFormat<'a> {
    /// JPEG-compressed frame.
    Jpeg { data: &'a [u8] },
    /// Packed YUYV 4:2:2.
    Yuyv { data: &'a [u8], stride: usize },
    /// NV12: a Y plane followed by an interleaved UV plane.
    Nv12 { data: &'a [u8], stride: usize },
    /// 8-bit grayscale.
    Gray8 { data: &'a [u8], stride: usize },
    /// Packed RGB888.
    Rgb8 { data: &'a [u8], stride: usize },
    /// Packed BGR888.
    Bgr8 { data: &'a [u8], stride: usize },
}

/// A trait representing a camera interface.
pub trait BzdCameraInterface {
    /// The runtime format of the frame captured by the camera.
    type Data<'a>
    where
        Self: 'a;

    /// Captures a single frame from the camera.
    async fn capture<'a>(&'a mut self) -> Result<Frame<Self::Data<'a>>, Error>;
}
