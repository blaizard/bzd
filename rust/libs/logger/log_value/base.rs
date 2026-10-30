use bzd::base::error::Error;
use bzd::base::stream::OStream;

/// Write a value directly to a stream without any intermediate buffer.
#[allow(async_fn_in_trait)]
pub trait LogValue {
    /// Write the value to the given stream.
    async fn write_to(&self, out: &mut impl OStream) -> Result<(), Error>;
}

impl<T: LogValue + ?Sized> LogValue for &T {
    async fn write_to(&self, out: &mut impl OStream) -> Result<(), Error> {
        (*self).write_to(out).await
    }
}
