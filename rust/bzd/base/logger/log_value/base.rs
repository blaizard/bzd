use bzd_stream_impl::BzdOStreamInterface;
use error::Error;

/// Write a value directly to a stream without any intermediate buffer.
#[allow(async_fn_in_trait)]
pub trait LogValue {
    /// Write the value to the given stream.
    async fn write(&self, out: &mut impl BzdOStreamInterface) -> Result<(), Error>;
}

impl<T: LogValue + ?Sized> LogValue for &T {
    async fn write(&self, out: &mut impl BzdOStreamInterface) -> Result<(), Error> {
        (*self).write(out).await
    }
}
