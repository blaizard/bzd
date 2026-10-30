use super::write_unsigned;
use super::LogValue;
use bzd::base::error::Error;
use bzd::base::stream::OStream;

impl LogValue for Error {
    async fn write_to(&self, out: &mut impl OStream) -> Result<(), Error> {
        out.write_all(self.file().as_bytes()).await?;
        out.write_all(b":").await?;
        write_unsigned(self.line() as u64, out).await?;
        out.write_all(b": ").await?;
        out.write_all(self.error_type().as_str().as_bytes()).await?;
        out.write_all(b": ").await?;
        out.write_all(self.message().as_bytes()).await
    }
}
