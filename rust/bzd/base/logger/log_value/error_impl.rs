use super::write_unsigned;
use super::LogValue;
use bzd_stream_impl::BzdOStreamInterface;
use error::Error;

impl LogValue for Error {
    async fn write(&self, out: &mut impl BzdOStreamInterface) -> Result<(), Error> {
        out.write(self.file().as_bytes()).await?;
        out.write(b":").await?;
        write_unsigned(self.line() as u64, out).await?;
        out.write(b": ").await?;
        out.write(self.error_type().as_str().as_bytes()).await?;
        out.write(b": ").await?;
        out.write(self.message().as_bytes()).await
    }
}
